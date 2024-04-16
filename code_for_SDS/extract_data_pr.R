library(data.table)
library(sf)
library(tidyverse)
library(rnaturalearth)

data_file_name <- "5120_nein"
location_file_name <- "ortsnetz.txt"
swiss_polygon_name <- "swissboundaries3d_2024-01_2056_5728/swissBOUNDARIES3D_1_5_TLM_HOHEITSGEBIET.shp"

data <- fread(paste(data_file_name, ".txt", sep = ""), encoding = "UTF-8")
sds_network <- fread(location_file_name, encoding = "UTF-8")

# Polygon for Switzerland 
switzerland_sf <- st_read(swiss_polygon_name) %>% st_union()

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
sds_network_sf <- st_as_sf(sds_network, coords = c("LONG", "LAT"), 
                           crs = 4326)


for(var_name in variant_names){
  tmp_sds_network <- sds_network[,c("LAT", "LONG", ..var_name)]
  tmp_sds_network <- tmp_sds_network %>% filter(tmp_sds_network[,3] == 1)
  tmp_sds_network_sf <- 
    st_as_sf(tmp_sds_network, coords = c("LONG", "LAT"), crs = 4326) %>%
    st_transform(crs=2056)
  
  legend_text <- case_when(
    var_name == "c_nei" ~ "nei",
    var_name == "c_nai" ~ "nai",
    var_name == "c_nee" ~ "nee",
    var_name == "c_nää" ~ "nää",
    var_name == "c_naa" ~ "naa",
    var_name == "c_na_knack" ~ "na knack",
  )
  
  ggplot(switzerland_sf) + 
    geom_sf(fill=NA) + 
    geom_sf(data = tmp_sds_network_sf, aes(col = var_name),
            size = 1) +
    scale_color_manual(labels = c(legend_text), values=c("orange")) + 
    theme_minimal() +
    theme(panel.grid.major = element_line(colour = "transparent"),
          legend.title=element_blank(),
          legend.position = c(0.15, 0.85),
          legend.text=element_text(size=12))
  
  ggsave(paste(data_file_name, "_", var_name, ".png", sep = ""), dpi = 300, 
         bg = "white", width = 6, height = 4)
  ggsave(paste(data_file_name, "_", var_name, ".pdf", sep = ""), dpi = 300,
         width = 6, height = 4)
}

