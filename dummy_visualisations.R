#setwd("/Users/matthewgraham/Dropbox/LFProjCode")
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
cf_coverage <-"65"
cf_non_compliance <- "03"

# moved IU file here to reduce repeated calls to read in the file
IUs <- read.csv("runIU.csv")
IUs_vec <- which(IUs$IUID == 1)


######## for one IU #######
#### testing code DO NOT DELETE
which_years <- c("Jan-2021","Jan-2022","Jan-2023","Jan-2024","Jan-2025","Jan-2026",
                 "Jan-2027","Jan-2028","Jan-2029","Jan-2030","Jan-2031")


data_files <- read_files_def_cf(scenario, coverage, cf_non_compliance = "03", cf_coverage = "65", non_compliance,
                                IU_order = IUs_vec[1], measure)

data_files_elim <- read_files_def_cf(scenario, coverage, cf_non_compliance = "03", cf_coverage = "65", non_compliance,
                                     IU_order = IUs_vec[1], elim)

######## across all IUs #######
IUs <- read.csv("runIU.csv")
which_IUs <- which(IUs$IUID == 1)
# sub sample of IUs
IUs_vec  <- which_IUs[1:25]
# which years to calculate measures for 
which_years <- c("Jan-2021","Jan-2022","Jan-2023","Jan-2024","Jan-2025","Jan-2026",
                 "Jan-2027","Jan-2028","Jan-2029","Jan-2030","Jan-2031")

######## compare other scenarios #######
# which measure (worm count, mf prevalence) do yo want to use to calculate proxy for DALYs
which_measure <- "MF"

start_times= Sys.time()
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
                             which_years = which_years,
                             preTAS_survey_cost ,
                             TAS_survey_cost,
                             IUs_vec)
Sys.time() - start_times

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
                              cost_cf,
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
                              cost_scenario = cost_scenario*80/65,
                              cost_development,
                              cost_cf, 
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
                              cost_cf,
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
                              cost_scenario = cost_scenario *80/65,
                              cost_development,
                              cost_cf,
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
labels <- c("1", "2a", "2b", "3a", "3b", "3c", "3d") # what to label blobs in same order as res_list

##### trajectories
# plot mean number of infections over time for each scenario
png("number_infections_over_time.png", width = 12, height = 8, res = 300, units = "in")
make_time_plots(num_infs , labels, "number of infections")
dev.off()
# plot mean number of IUs stopped MDA over time for each scenario
png("IUS_pass_TAS.png", width = 12, height = 8, res = 300, units = "in")
make_time_plots(IUs_stopped , labels, "proportion of IUs stopped MDA")
dev.off()

# original blob plot
cols <- c(hcl.colors(length(labels), palette = "ag_Sunset", alpha = 0.6))
make_blob_plot(res_list, labels, cols)
abline(h = cost_development, lty = "dashed", col = "gray")

# blob plot with net monetary elimination benefit colouring
make_blob_plot_nmeb(res_list, labels, lambda_DALY = 2, lambda_EOT = 100000)
abline(h = cost_development, lty = "dashed", col = "gray")

###################################################################################################
#####################################################################################################################
###################################################################################################
# cloud plot

IUs_vec = which_IUs

start_time = Sys.time()
scen0 = calculate_dalys_and_costs_for_scenario_cloud(IUs_vec, 
                                                     scenario = "NC", 
                                                     coverage = "65", 
                                                     non_compliance = "03", 
                                                     cost_development = 0, 
                                                     cost_scenario = cost_cf,
                                                     preTAS_survey_cost , 
                                                     TAS_survey_cost, which_years )
Sys.time() - start_time

scen1 = calculate_dalys_and_costs_for_scenario_cloud(IUs_vec, scenario = "NC", 
                                                     coverage = "65", 
                                                     non_compliance = "02",
                                                     cost_development = 0, 
                                                     cost_scenario = cost_cf,
                                                     preTAS_survey_cost, 
                                                     TAS_survey_cost, which_years )

scen2 = calculate_dalys_and_costs_for_scenario_cloud(IUs_vec, scenario = "NC", 
                                                     coverage = "80", 
                                                     non_compliance = "02",
                                                     cost_development = 0, 
                                                     cost_scenario = cost_cf * 80/65,
                                                     preTAS_survey_cost, 
                                                     TAS_survey_cost, which_years )


scen3a = calculate_dalys_and_costs_for_scenario_cloud(IUs_vec, scenario = "M1", 
                                                      coverage = "65", 
                                                      non_compliance = "02",
                                                      cost_development = cost_development, 
                                                      cost_scenario = cost_scenario,
                                                      preTAS_survey_cost, 
                                                      TAS_survey_cost, which_years )

scen3b = calculate_dalys_and_costs_for_scenario_cloud(IUs_vec, scenario = "M1", 
                                                      coverage = "80", 
                                                      non_compliance = "02",
                                                      cost_development = cost_development, 
                                                      cost_scenario = cost_scenario * 80/65,
                                                      preTAS_survey_cost, 
                                                      TAS_survey_cost, which_years )

scen3c = calculate_dalys_and_costs_for_scenario_cloud(IUs_vec, scenario = "M2", 
                                                      coverage = "65", 
                                                      non_compliance = "02",
                                                      cost_development = cost_development, 
                                                      cost_scenario = cost_scenario,
                                                      preTAS_survey_cost, 
                                                      TAS_survey_cost, which_years )

scen3d = calculate_dalys_and_costs_for_scenario_cloud(IUs_vec, scenario = "M2", coverage = "80", 
                                                      non_compliance = "02",
                                                      cost_development = cost_development * 80/65, 
                                                      cost_scenario = cost_scenario,
                                                      preTAS_survey_cost, 
                                                      TAS_survey_cost, which_years )


all_costs = list(scen0$costs - scen0$costs, scen1$costs - scen0$costs,
                 scen2$costs - scen0$costs, scen3a$costs - scen0$costs,
                 scen3b$costs - scen0$costs, scen3c$costs - scen0$costs,
                 scen3d$costs - scen0$costs)

all_dalys = list(scen0$dalys - scen0$dalys, scen0$dalys - scen1$dalys,
                 scen0$dalys - scen2$dalys, scen0$dalys - scen3a$dalys,
                 scen0$dalys - scen3b$dalys, scen0$dalys - scen3c$dalys,
                 scen0$dalys - scen3d$dalys)
# make a cloud plot


labels <- c("1", "2a", "2b", "3a", "3b", "3c", "3d") # what to label blobs in same order as res_list
cols <- c(hcl.colors(length(labels), palette = "ag_Sunset", alpha = 0.6))

png("Cloud_blobs.png", width = 12, height = 8, res = 300, units = "in")
plot_clouds(all_dalys, all_costs, labels, draw_slopes = T)
dev.off()


mean_cost_diff  = c(mean(scen0$costs) - mean(scen0$costs), mean(scen1$costs - scen0$costs),
                       mean(scen2$costs) - mean(scen0$costs), mean(scen3a$costs) - mean(scen0$costs),
                       mean(scen3b$costs - scen0$costs), mean(scen3c$costs - scen0$costs),
                       mean(scen3d$costs - scen0$costs))

mean_dalys_diff = c(mean(scen0$dalys - scen0$dalys), mean(scen0$dalys - scen1$dalys),
                 mean(scen0$dalys - scen2$dalys), mean(scen0$dalys - scen3a$dalys),
                 mean(scen0$dalys - scen3b$dalys), mean(scen0$dalys - scen3c$dalys),
                 mean(scen0$dalys - scen3d$dalys))

mean_elim_diff = c(mean(scen0$elims) - mean(scen0$elims), mean(scen1$elims - scen0$elims),
                   mean(scen2$elims) - mean(scen0$elims), mean(scen3a$elims) - mean(scen0$elims),
                   mean(scen3b$elims - scen0$elims), mean(scen3c$elims - scen0$elims),
                   mean(scen3d$elims - scen0$elims))

mean_true_elim_diff = c(mean(scen0$true_elims) - mean(scen0$true_elims), mean(scen1$true_elims - scen0$true_elims),
                        mean(scen2$true_elims) - mean(scen0$true_elims), mean(scen3a$true_elims) - mean(scen0$true_elims),
                        mean(scen3b$true_elims - scen0$true_elims), mean(scen3c$true_elims - scen0$true_elims),
                        mean(scen3d$true_elims - scen0$true_elims))


png("blob_1.png", width = 12, height = 8, res = 300, units = "in")
plot_blob_plot_from_all_daly_costs(mean_dalys_diff, mean_cost_diff, labels, draw_slopes = T,additional_cost_dev = 0)
dev.off()
png("blob_high.png", width = 12, height = 8, res = 300, units = "in")
plot_blob_plot_from_all_daly_costs(mean_dalys_diff, mean_cost_diff, labels, draw_slopes = T,additional_cost_dev = 15000)
dev.off()
png("blob_low.png", width = 12, height = 8, res = 300, units = "in")
plot_blob_plot_from_all_daly_costs(mean_dalys_diff, mean_cost_diff, labels, draw_slopes = T,additional_cost_dev = - 5000)
dev.off()


png("nmeb_3_10_0_dev_no_lines.png", width = 12, height = 8, units = "in", res = 300)
make_blob_plot_from_cloud_data(mean_dalys_diff, mean_cost_diff, mean_true_elim_diff, 
                                           labels, lambda_DALY = 3, lambda_EOT=10, additional_cost_dev = 0, draw_slopes = F)
dev.off()


png("nmeb_3_10_10000_dev_no_lines.png", width = 12, height = 8, units = "in", res = 300)
make_blob_plot_from_cloud_data(mean_dalys_diff, mean_cost_diff, mean_true_elim_diff, 
                               labels, lambda_DALY = 3, lambda_EOT=10, additional_cost_dev = 10000, draw_slopes = F)
dev.off()

png("nmeb_1_10_0_dev_no_lines.png", width = 12, height = 8, units = "in", res = 300)
make_blob_plot_from_cloud_data(mean_dalys_diff, mean_cost_diff, mean_true_elim_diff, 
                               labels, lambda_DALY = 1, lambda_EOT=10, additional_cost_dev = 0, draw_slopes = F)
dev.off()

png("nmeb_1_3500_0_dev_no_lines.png", width = 12, height = 8, units = "in", res = 300)
make_blob_plot_from_cloud_data(mean_dalys_diff, mean_cost_diff, mean_elim_diff, 
                               labels, lambda_DALY = 1, lambda_EOT=3500, additional_cost_dev = 0, draw_slopes = F)
dev.off()
nmeb = make_blob_plot_from_cloud_data(mean_dalys_diff, mean_cost_diff, mean_elim_diff, 
                               labels, lambda_DALY = 3, lambda_EOT=10, additional_cost_dev = -5000, draw_slopes = T)


