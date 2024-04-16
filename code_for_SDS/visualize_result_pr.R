library(data.table)
library(sf)
library(tidyverse)
library(rnaturalearth)


MCMC_result_files_name <- c("result3_naa", "result3_nei", "result3_nai")
location_file_name <- "ortsnetz.txt"
swiss_polygon_name <- "swissboundaries3d_2024-01_2056_5728/swissBOUNDARIES3D_1_5_TLM_HOHEITSGEBIET.shp"

num.years.by.timestep <- 20

MCMC_results <- lapply(MCMC_result_files_name, function (x) {
  fread(paste(x, ".csv", sep = ""), 
        encoding = "UTF-8")})

names(MCMC_results) <- c("naa", "nei", "nai")

sds_network <- fread(location_file_name, encoding = "UTF-8")
num_location <- nrow(sds_network)

# Polygon for Switzerland 
switzerland_sf <- st_read(swiss_polygon_name) %>% st_union()

################summarizing the result

sds_networks_sf <- lapply(MCMC_results, function(x){
  sample_size <- nrow(x)
  freq_origin_by_loc <- rep(0, num_location)
  x$origin_time <- x$origin_time * num.years.by.timestep
  x$loss_rate <- x$loss_rate / num.years.by.timestep
  #increase the index by 1 (C++ -> R)
  x$origin_node_id <- x$origin_node_id + 1
  
  num_origin_by_loc <- x %>% count(origin_node_id)
  freq_origin_by_loc[num_origin_by_loc$origin_node_id] <-
    num_origin_by_loc$n / sample_size
  
  sds_net <- cbind(sds_network, freq_origin_by_loc)
  sds_net_sf <- st_as_sf(sds_net, coords = c("LONG", "LAT"), 
                         crs = 4326) %>% st_transform(crs=2056)
}
  )

min_freq <- lapply(sds_networks_sf, function (x) {
  min(x$freq_origin_by_loc)
}) %>% unlist() %>% min()

max_freq <- lapply(sds_networks_sf, function (x) {
  max(x$freq_origin_by_loc)
}) %>% unlist() %>% max()
  
# Plot
lapply(names(sds_networks_sf), function(x) {
  
  legend_title <- paste0("origin of ", x, " (%)")
    
  freq_map <- ggplot(switzerland_sf) + 
    geom_sf(fill=NA) + 
    geom_sf(data = sds_networks_sf[[x]], aes(color = freq_origin_by_loc),
            size = 1) +
    scale_colour_gradient(name = legend_title, low = "blue", high = "red", 
                          limits=c(min_freq, max_freq)) +
    theme_minimal() +
    theme(panel.grid.major = element_line(colour = "transparent"))
  
  ggsave(paste0(x, "_origin_space.png"), 
        dpi = 300, bg = "white", width = 6, height = 4)
  ggsave(paste0(x, "_origin_space.pdf", sep = ""), 
         dpi = 300,  width = 6, height = 4)
  
})




