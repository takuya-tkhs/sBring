library(data.table)
library(sf)
library(tidyverse)
library(rnaturalearth)

data_file_name <- "5120_nein"
location_file_name <- "ortsnetz.txt"

data <- fread(paste(data_file_name, ".txt", sep = ""), encoding = "UTF-8")
sds_network <- fread(location_file_name, encoding = "UTF-8")

num_location <- nrow(sds_network)
num_data_points <- nrow(data)
variant_names <- colnames(data)[-c(1, ncol(data))]
data_mat <- as.matrix(data)

for(var_name in variant_names){
  data.vec <- rep(-1, num_location)
  
  for(i in 1:num_data_points){
    loc_id <- which(sds_network$SDS_CODE == data$SDS_CODE[i])
    data.vec[loc_id] <- as.numeric(data_mat[i, var_name])
  }
  
  output_file_name <- paste(data_file_name, "_", var_name, ".csv", sep = "")
  write.csv(t(data.vec), output_file_name, row.names = F)
  
  sds_network <- cbind(sds_network, data.vec)
  tmp.colnames <- colnames(sds_network)
  tmp.colnames[length(tmp.colnames)] <- var_name
  colnames(sds_network) <- tmp.colnames
}

#visualization on the map
sds_network_sf <- st_as_sf(sds_network, coords = c("LONG", "LAT"), crs = 4326)
world_sf <- ne_countries(scale = "medium", returnclass = "sf")
switzerland_sf <- world_sf %>% filter(name == "Switzerland")
st_transform(switzerland_sf, crs = 2056) 

for(var_name in variant_names){
  tmp_sds_network <- sds_network[,c("LAT", "LONG", ..var_name)]
  tmp_sds_network <- tmp_sds_network %>% filter(tmp_sds_network[,3] == 1)
  tmp_sds_network_sf <- 
    st_as_sf(tmp_sds_network, coords = c("LONG", "LAT"), crs = 4326)
  
  ggplot() + 
    geom_sf(data = switzerland_sf) + 
    geom_sf(data = tmp_sds_network_sf, col = "green") +
    theme_minimal()
  
  ggsave(paste(data_file_name, "_", var_name, ".jpg", sep = ""))
}