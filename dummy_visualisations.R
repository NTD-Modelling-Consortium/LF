setwd("/Users/matthewgraham/Dropbox/LFProjCode")
setwd("~/Documents/GitHub/") 
source("vis_functions.R")


cost_development <- 5000
cost_cf <- 0.01
cost_scenario <- 0.02
preTAS_survey_cost = 5
TAS_survey_cost = 5
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
#### testing code DO NOT DELETE
which_years <- c("Jan-2021","Jan-2022","Jan-2023","Jan-2024","Jan-2025","Jan-2026",
                 "Jan-2027","Jan-2028","Jan-2029","Jan-2030","Jan-2031")


data_files <- read_files_def_cf(scenario, coverage, cf_non_compliance = "03", cf_coverage = "65", non_compliance,
                                IU_order = 14, measure)

# calculate relevant measuyre of impact (median at 2030)
summary_res <- extract_medians(data_files, which_years)

# what is the difference in WC and number of MDA rounds
diff_measures <- summary_res$cf - summary_res$scenario

# calculate relevant costs
costs <- calculate_costs(summary_res, cost_scenario = 15, cost_development = 1000, cost_cf = 10, population = 1000)

######## across all IUs #######
IUs <- read.csv("runIU.csv")
IUs_vec <- which(IUs$IUID == 1)
no_IUs <- length(IUs_vec)
no_IUs = 10
#which_years <- "Jan-2031"
which_years <- c("Jan-2021","Jan-2022","Jan-2023","Jan-2024","Jan-2025","Jan-2026",
                 "Jan-2027","Jan-2028","Jan-2029","Jan-2030","Jan-2031")


######## compare other scenarios #######

which_measure <- "MF"

# moved IU file here to reduce repeated calls to read in the file
IUs <- read.csv("runIU.csv")
which_IUs <- which(IUs$IUID == 1)
IUs_vec <- sample(which_IUs, 50)

IUs_vec <- c(4129, 3279, 3917)


res_0 <- calculate_blob_data(scenario = "NC", # scenario name
                             coverage = "65", # coverage percentage
                             cf_coverage = "65",# coverage for cf
                             non_compliance = "03",  # non-compliance parameter, equivalent to 0.3
                             cf_non_compliance = "03", # # non-compliance parameter for cf
                             measure = which_measure, # output measure, WC : worm count
                             elim = "MF",
                             cost_scenario = cost_cf,
                             cost_development = 0,
                             cost_cf, 
                             no_IUs = no_IUs,
                             which_years = which_years,
                             preTAS_survey_cost ,
                             TAS_survey_cost,
                             IUs_vec)


res_1 <- calculate_blob_data(scenario = "NC", # scenario name
                             coverage = "65", # coverage percentage
                             cf_coverage = "65",# coverage for cf
                             non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                             cf_non_compliance = "03", # # non-compliance parameter for cf
                             measure = which_measure, # output measure, WC : worm count
                             elim = "MF",
                             cost_scenario = cost_cf,
                             cost_development = 0,
                             cost_cf, 
                             no_IUs = no_IUs,
                             which_years = which_years,
                             preTAS_survey_cost ,
                             TAS_survey_cost,
                             IUs_vec)


res_2 <- calculate_blob_data(scenario = "NC", # scenario name
                             coverage = "80", # coverage percentage
                             cf_coverage = "65",# coverage for cf
                             non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                             cf_non_compliance = "03", # # non-compliance parameter for cf
                             measure = which_measure, # output measure, WC : worm count
                             elim = "MF",
                             cost_scenario = cost_cf * 80/65,
                             cost_development = 0,
                             cost_cf, 
                             no_IUs = no_IUs,
                             which_years,
                             preTAS_survey_cost,
                             TAS_survey_cost,
                             IUs_vec)


res_3a <- calculate_blob_data(scenario = "M1", # scenario name
                              coverage = "65", # coverage percentage
                              cf_coverage = "65",# coverage for cf
                              non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                              cf_non_compliance = "03", # # non-compliance parameter for cf
                              measure = which_measure, # output measure, WC : worm count
                              elim = "MF",
                              cost_scenario = cost_scenario,
                              cost_development,
                              cost_cf, no_IUs = no_IUs, 
                              which_years,
                              preTAS_survey_cost,
                              TAS_survey_cost,
                              IUs_vec)

res_3b <- calculate_blob_data(scenario = "M1", # scenario name
                              coverage = "80", # coverage percentage
                              cf_coverage = "65",# coverage for cf
                              non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                              cf_non_compliance = "03", # # non-compliance parameter for cf
                              measure = which_measure, # output measure, WC : worm count
                              elim = "MF",
                              cost_scenario = cost_scenario,
                              cost_development,
                              cost_cf, no_IUs = no_IUs, 
                              which_years,
                              preTAS_survey_cost,
                              TAS_survey_cost,
                              IUs_vec)

res_3c <- calculate_blob_data(scenario = "M2", # scenario name
                              coverage = "65", # coverage percentage
                              cf_coverage = "65",# coverage for cf
                              non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                              cf_non_compliance = "03", # # non-compliance parameter for cf
                              measure = which_measure, # output measure, WC : worm count
                              elim = "MF",
                              cost_scenario = cost_scenario,
                              cost_development,
                              cost_cf, no_IUs = no_IUs,
                              which_years,
                              preTAS_survey_cost,
                              TAS_survey_cost,
                              IUs_vec)

res_3d <- calculate_blob_data(scenario = "M2", # scenario name
                              coverage = "80", # coverage percentage
                              cf_coverage = "65",# coverage for cf
                              non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                              cf_non_compliance = "03", # # non-compliance parameter for cf
                              measure = which_measure, # output measure, WC : worm count
                              elim = "MF",
                              cost_scenario = cost_scenario,
                              cost_development,
                              cost_cf, no_IUs = no_IUs,
                              which_years,
                              preTAS_survey_cost,
                              TAS_survey_cost,
                              IUs_vec)

# plot 
res_list <- list(res_0$res, res_1$res, res_2$res, res_3a$res, res_3b$res, res_3c$res, res_3d$res)

# number of infected people between 2020 and 2030 for all scenarios
num_infs <- list(res_0$num_infections_over_time, res_1$num_infections_over_time, res_2$num_infections_over_time, 
                 res_3a$num_infections_over_time, res_3b$num_infections_over_time,
                 res_3c$num_infections_over_time, res_3d$num_infections_over_time)
# proportion of IUs stopped MDA between 2020 and 2030 for all scenarios
IUs_stopped <- list(res_0$prop_IUs_Finished_MDA, res_1$prop_IUs_Finished_MDA, 
                    res_2$prop_IUs_Finished_MDA, res_3a$prop_IUs_Finished_MDA, 
                    res_3b$prop_IUs_Finished_MDA, res_3c$prop_IUs_Finished_MDA,
                    res_3d$prop_IUs_Finished_MDA)
labels <- c("0", "1", "2", "3a", "3b", "3c", "3d") # what to label blobs in same order as res_list

##### trajectories
# plot mean number of infections over time for each scenario
make_time_plots(num_infs , labels, "number of infections")
# plot mean number of IUs stopped MDA over time for each scenario
make_time_plots(IUs_stopped , labels, "proportion of IUs stopped MDA")

# original blob plot
make_blob_plot(res_list, labels)

# blob plot with net monetary elimination benefit colouring
make_blob_plot_v2(res_list, labels, lambda_DALY = 10, lambda_EOT = 1)

