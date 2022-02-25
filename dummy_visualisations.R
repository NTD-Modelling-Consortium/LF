setwd("/Users/matthewgraham/Dropbox/LFProjCode")
source("vis_functions.R")

setwd("~/Documents/GitHub/")


mean_IU_pop <- 500000000 / 4653
cost_development <- 500
cost_cf <- 0.01
cost_scenario <- 0.02
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
which_years <- c("Jan-2021","Jan-2022","Jan-2023","Jan-2024","Jan-2025","Jan-2026",
                 "Jan-2027","Jan-2028","Jan-2029","Jan-2030","Jan-2031")
# read in files
data_files <- read_files_def_cf(scenario, coverage, cf_coverage = "65", non_compliance,
                                IU_order, measure)

# calculate relevant measuyre of impact (median at 2030)
summary_res <- extract_medians(data_files, which_years)

# what is the difference in WC and number of MDA rounds
diff_measures <- summary_res$cf - summary_res$scenario

# calculate relevant costs
costs <- calculate_costs(summary_res, cost_scenario = 15, cost_development = 1000, cost_cf = 10, population = 1000)

######## across all IUs #######
IUs <- read.csv("runIU.csv")
IUs_vec <- which(IUs$V1 == 1)
no_IUs <- length(IUs_vec)
no_IUs = 10
#which_years <- "Jan-2031"
which_years <- c("Jan-2021","Jan-2022","Jan-2023","Jan-2024","Jan-2025","Jan-2026",
                 "Jan-2027","Jan-2028","Jan-2029","Jan-2030","Jan-2031")


######## compare other scenarios #######

which_measure <- "MF"

res_1 <- calculate_blob_data(scenario = "NC", # scenario name
                             coverage = "65", # coverage percentage
                             cf_coverage = "65",# coverage for cf
                             non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                             measure = which_measure, # output measure, WC : worm count
                             elim = "MF",
                             mean_IU_pop ,
                             cost_scenario = cost_cf,
                             cost_development = 0,
                             cost_cf, 
                             no_IUs = no_IUs,
                             which_years = which_years)


res_2 <- calculate_blob_data(scenario = "NC", # scenario name
                                coverage = "80", # coverage percentage
                                cf_coverage = "65",# coverage for cf
                                non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                                measure = which_measure, # output measure, WC : worm count
                                elim = "MF",
                                mean_IU_pop ,
                                cost_scenario = cost_cf,
                                cost_development = 0,
                                cost_cf, 
                                no_IUs = no_IUs,
                                which_years)


res_3a <- calculate_blob_data(scenario = "M1", # scenario name
                              coverage = "65", # coverage percentage
                              cf_coverage = "65",# coverage for cf
                              non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                              measure = which_measure, # output measure, WC : worm count
                              elim = "MF",
                              mean_IU_pop ,
                              cost_scenario = cost_scenario,
                              cost_development,
                              cost_cf, no_IUs = no_IUs, 
                              which_years)

res_3b <- calculate_blob_data(scenario = "M1", # scenario name
                              coverage = "80", # coverage percentage
                              cf_coverage = "65",# coverage for cf
                              non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                              measure = which_measure, # output measure, WC : worm count
                              elim = "MF",
                              mean_IU_pop ,
                              cost_scenario = cost_scenario,
                              cost_development,
                              cost_cf, no_IUs = no_IUs, 
                              which_years)

res_3c <- calculate_blob_data(scenario = "M2", # scenario name
                              coverage = "65", # coverage percentage
                              cf_coverage = "65",# coverage for cf
                              non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                              measure = which_measure, # output measure, WC : worm count
                              elim = "MF",
                              mean_IU_pop ,
                              cost_scenario = cost_scenario,
                              cost_development,
                              cost_cf, no_IUs = no_IUs,
                              which_years)

res_3d <- calculate_blob_data(scenario = "M2", # scenario name
                              coverage = "80", # coverage percentage
                              cf_coverage = "65",# coverage for cf
                              non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                              measure = which_measure, # output measure, WC : worm count
                              elim = "MF",
                              mean_IU_pop ,
                              cost_scenario = cost_scenario,
                              cost_development,
                              cost_cf, no_IUs = no_IUs,
                              which_years)

# plot 
res_list <- list(res_1, res_2, res_3a, res_3b, res_3c, res_3d)
labels <- c("1", "2", "3a", "3b", "3c", "3d") # what to label blobs in same order as res_list

make_blob_plot(res_list, labels)

