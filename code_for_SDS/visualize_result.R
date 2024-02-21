library(data.table)
library(sf)
library(tidyverse)
library(rnaturalearth)


MCMC_result_file_name <- "result3_naa"
location_file_name <- "ortsnetz.txt"

num.years.by.timestep <- 20

MCMC_result <- fread(paste(MCMC_result_file_name, ".csv", sep = ""), encoding = "UTF-8")
sds_network <- fread(location_file_name, encoding = "UTF-8")

num_location <- nrow(sds_network)


################summerizing the result
sample_size <- nrow(MCMC_result)
freq_origin_by_loc <- rep(0, num_location)

MCMC_result$origin_time <- MCMC_result$origin_time * num.years.by.timestep
MCMC_result$loss_rate <- MCMC_result$loss_rate / num.years.by.timestep

#increase the index by 1 (C++ -> R)
MCMC_result$origin_node_id <- MCMC_result$origin_node_id + 1

num_origin_by_loc <- MCMC_result %>% count(origin_node_id)
freq_origin_by_loc[num_origin_by_loc$origin_node_id] <-
  num_origin_by_loc$n / sample_size

sds_network <- cbind(sds_network, freq_origin_by_loc)


#visualization on the map
sds_network_sf <- st_as_sf(sds_network, coords = c("LONG", "LAT"), crs = 4326)
world_sf <- ne_countries(scale = "medium", returnclass = "sf")
switzerland_sf <- world_sf %>% filter(name == "Switzerland")
st_transform(switzerland_sf, crs = 2056) 

ggplot() + geom_sf(data = switzerland_sf) + 
  geom_sf(data = sds_network_sf, aes(color = freq_origin_by_loc)) +
  scale_colour_gradient(low = "blue", high = "red") +
  theme_minimal()

ggsave(paste(MCMC_result_file_name, "_origin_space.jpg", sep = ""))


ggplot(MCMC_result, aes(x = origin_time)) + 
  geom_histogram(aes(y=after_stat(density)), color="black", fill="lightblue")+
  geom_density(alpha = 0.5, fill="#FF6666")

ggsave(paste(MCMC_result_file_name, "_origin_time.jpg", sep = ""))

ggplot(MCMC_result, aes(x = loss_rate)) + 
  geom_histogram(aes(y=after_stat(density)), color="black", fill="lightblue")+
  geom_density(alpha = 0.5, fill="#FF6666")

ggsave(paste(MCMC_result_file_name, "_loss_rate.jpg", sep = ""))