library(data.table)
library(sf)
library(tidyverse)

location_file_name <- "ortsnetz.txt"
gamma <- 10 ^ 4  #i.e., 10 km
sigma <- 10 ^ 4

transmission_rate_mat_cauchy <- function(dist_mat, gamma){
  return(1 / (dist_mat ^ 2 + gamma ^ 2))
}

transmission_rate_mat_gaussian <- function(dist_mat, sigma){
  return(exp(- dist_mat ^ 2 / (2 * sigma ^ 2)))
}



#main routine starts here.
sds_network <- fread(location_file_name, encoding = "UTF-8")
num_location <- nrow(sds_network)

sds_network_sf <- st_as_sf(sds_network, coords = c("LONG", "LAT"), crs = 4326)

#create dist_matrix without units
sds_dist_mat_tmp <- st_distance(sds_network_sf)
sds_dist_mat <- matrix(nrow = num_location, ncol = num_location)
for(i in 1:num_location){
  sds_dist_mat[i,] <- as.numeric(sds_dist_mat_tmp[i,])
}
remove(sds_dist_mat_tmp)

transmission_rate_mat <- transmission_rate_mat_gaussian(sds_dist_mat, sigma)
#transmission_rate_mat <- transmission_rate_mat_cauchy(sds_dist_mat, gamma)
for(i in 1:num_location){
  tmp_sum <- sum(transmission_rate_mat[i,])
  transmission_rate_mat[i,] <- transmission_rate_mat[i,] / tmp_sum
}

write.csv(transmission_rate_mat, "weighted_adj_mat_gaussian_10km.csv",
          row.names = F)