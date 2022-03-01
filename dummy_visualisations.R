setwd("/Users/matthewgraham/Dropbox/LFProjCode")
source("vis_functions.R")

#setwd("~/Documents/GitHub/") 
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
which_years <- c("Jan-2021","Jan-2022","Jan-2023","Jan-2024","Jan-2025","Jan-2026",
                 "Jan-2027","Jan-2028","Jan-2029","Jan-2030","Jan-2031")
######## across all IUs #######
IUs <- read.csv("runIU.csv")
IUs_vec <- which(IUs$IUID == 1)
no_IUs <- length(IUs_vec)
no_IUs = 50
#which_years <- "Jan-2031"
which_years <- c("Jan-2021","Jan-2022","Jan-2023","Jan-2024","Jan-2025","Jan-2026",
                 "Jan-2027","Jan-2028","Jan-2029","Jan-2030","Jan-2031")


######## compare other scenarios #######

which_measure <- "MF"

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
                             TAS_survey_cost )


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
                             TAS_survey_cost )


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
                             TAS_survey_cost)


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
                              TAS_survey_cost)

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
                              TAS_survey_cost)

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
                              TAS_survey_cost)

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
                              TAS_survey_cost)

# plot 
res_list <- list(res_0$res, res_1$res, res_2$res, res_3a$res, res_3b$res, res_3c$res, res_3d$res)
num_infs <- list(res_0$num_infections_over_time, res_1$num_infections_over_time, res_2$num_infections_over_time, 
                 res_3a$num_infections_over_time, res_3b$num_infections_over_time,
                 res_3c$num_infections_over_time, res_3d$num_infections_over_time)
IUs_stopped <- list(res_0$prop_IUs_Finished_MDA, res_1$prop_IUs_Finished_MDA, 
                    res_2$prop_IUs_Finished_MDA, res_3a$prop_IUs_Finished_MDA, 
                    res_3b$prop_IUs_Finished_MDA, res_3c$prop_IUs_Finished_MDA,
                    res_3d$prop_IUs_Finished_MDA)
labels <- c("0", "1", "2", "3a", "3b", "3c", "3d") # what to label blobs in same order as res_list

make_blob_plot(res_list, labels)

make_blob_plot_v2(res_list, labels, lambda_DALY = 10, lambda_EOT = 1)

make_number_infections_plot(num_infs , labels)
make_number_infections_plot(IUs_stopped , labels)
