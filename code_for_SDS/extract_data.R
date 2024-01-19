library(data.table)
library(sf)
library(tidyverse)

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
    data.vec[loc_id] <- data_mat[i, var_name]
  }
  
  output_file_name <- paste(data_file_name, "_", var_name, ".csv", sep = "")
  write.csv(t(data.vec), output_file_name, row.names = F)
}