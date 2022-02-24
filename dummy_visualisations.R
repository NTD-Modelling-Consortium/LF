source("vis_functions.R")
setwd("~/Documents/GitHub/")
setwd("/Users/matthewgraham/Dropbox/LFProjCode")


mean_IU_pop <- 500000000 / 4653
cost_development <- 1000
cost_cf <- 10
# assumes there is an output folder in github repo

# dummy visualisations need to illustrate impact versus cost
# first plot idea: 
# impact : difference in median worm burden
# cost : number of MDAs, with different cost per MDA + cost of development

scenario <- "M1" # scenario name
coverage <- "65" # coverage percentage
non_compliance <- "02" # non-compliance parameter, equivalent to 0.2
IU_order <- "2" # IU order number
measure <- "WC" # output measure, WC : worm count
elim <- "MF" # check if elimination was achieved before 2030
######## for one IU #######
# read in files
data_files <- read_files_def_cf(scenario, coverage, cf_coverage = "65", non_compliance,
                         IU_order, measure)

# calculate relevant measuyre of impact (median at 2030)
which_years <- c("Jan-2021","Jan-2022","Jan-2023","Jan-2024","Jan-2025","Jan-2026",
                 "Jan-2027","Jan-2028","Jan-2029","Jan-2030","Jan-2031")
#which_years <- "Jan-2031"
summary_res <- extract_medians(data_files, which_years)

# what is the difference in WC and number of MDA rounds
random_population <- max(0, rnorm(n = 1,mean = mean_IU_pop, sd = mean_IU_pop/3)) + 5000 # choose some random population size for the IU
diff_measures <- random_population * summary_res[which_years,"cf"] - random_population * summary_res[which_years,"scenario"]
sum(diff_measures)


# calculate relevant costs
costs <- calculate_costs(summary_res, cost_scenario = 15, cost_development = 1000, cost_cf = 10)

######## across all IUs #######
IUs <- read.csv("runIU.csv")
IUs_vec <- which(IUs$V1 == 1)
no_IUs <- length(IUs_vec)

#which_years <- "Jan-2031"
which_years <- c("Jan-2021","Jan-2022","Jan-2023","Jan-2024","Jan-2025","Jan-2026",
                 "Jan-2027","Jan-2028","Jan-2029","Jan-2030","Jan-2031")
######## compare other scenarios #######

which_measure <- "MF"
res_M1 <- calculate_blob_data(scenario = "M1", # scenario name
                             coverage = "65", # coverage percentage
                             cf_coverage = "65",# coverage for cf
                             non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                             IU_order = "2",# IU order number
                             measure = which_measure, # output measure, WC : worm count
                             elim = "MF",
                             mean_IU_pop ,
                             cost_scenario = cost_cf * 8/6.5,
                             cost_development,
                             cost_cf, no_IUs =  40, 
                             which_years)

res_M2 <- calculate_blob_data(scenario = "M2", # scenario name
                             coverage = "65", # coverage percentage
                             cf_coverage = "65",# coverage for cf
                             non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                             IU_order = "2",# IU order number
                             measure = which_measure, # output measure, WC : worm count
                             elim = "MF",
                             mean_IU_pop ,
                             cost_scenario = cost_cf * 8/6.5,
                             cost_development,
                             cost_cf, no_IUs =  40,
                             which_years)


res_NC2 <- calculate_blob_data(scenario = "NC", # scenario name
                              coverage = "80", # coverage percentage
                              cf_coverage = "65",# coverage for cf
                              non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                              IU_order = "2",# IU order number
                              measure = which_measure, # output measure, WC : worm count
                              elim = "WC",
                              mean_IU_pop ,
                              cost_scenario = cost_cf * 8/6.5,
                              cost_development = 0,
                              cost_cf, 
                              no_IUs =  40,
                              which_years)
# plot 
res_list <- list(res_NC2, res_M1, res_M2)
labels <- c("1", "2", "3") # what to label blobs in same order as res_list

make_blob_plot(res_list, labels)
