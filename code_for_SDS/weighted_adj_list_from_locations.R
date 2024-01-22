library(data.table)
library(sf)
library(tidyverse)

location_file_name <- "ortsnetz.txt"
gamma <- 0.1

sds_network <- fread(location_file_name, encoding = "UTF-8")
num_location <- nrow(sds_network)

sds_network_sf <- st_as_sf(sds_network, coords = c("LONG", "LAT"), crs = 2056)

#create dist_matrix without units
sds_dist_mat_tmp <- st_distance(sds_network_sf)
sds_dist_mat <- matrix(nrow = num_location, ncol = num_location)
for(i in 1:num_location){
  sds_dist_mat[i,] <- as.numeric(sds_dist_mat_tmp[i,])
}
remove(sds_dist_mat_tmp)

transmission_rate_mat <- 1 / (sds_dist_mat ^ 2 + gamma ^ 2)
for(i in 1:num_location){
  tmp_sum <- sum(transmission_rate_mat[i,])
  transmission_rate_mat[i,] <- transmission_rate_mat[i,] / tmp_sum
}

write.csv(transmission_rate_mat, "weighted_adj_mat.csv",
          row.names = F)