source("vis_functions.R")
setwd("~/Documents/GitHub/")
setwd("/Users/matthewgraham/Dropbox/LFProjCode")


mean_IU_pop = 500000000 / 4653
cost_scenario = 15
cost_development = 1000
cost_cf = 10
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
data_files <- read_files(scenario, coverage, non_compliance,
                         IU_order, measure)

# calculate relevant measuyre of impact (median at 2030)
summary_res <- extract_medians(data_files)

# what is the difference in WC and number of MDA rounds
diff_measures <- summary_res$cf - summary_res$scenario

# calculate relevant costs
costs <- calculate_costs(summary_res, cost_scenario = 15, cost_development = 1000, cost_cf = 10)

######## across all IUs #######
IUs = read.csv("runIU.csv")
IUs_vec <- which(IUs$V1 == 1)
no_IUs <- length(IUs_vec)
no_IUs = 50

######## compare other scenarios #######

res_M1 = calculate_blob_data(scenario = "M1", # scenario name
                             coverage = "65", # coverage percentage
                             cf_coverage = "65",# coverage for cf
                             non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                             IU_order = "2",# IU order number
                             measure = "WC", # output measure, WC : worm count
                             elim = "MF",
                             mean_IU_pop ,
                             cost_scenario,
                             cost_development,
                             cost_cf, no_IUs =  50)

######## extra : do all measures (WC, MF, IC) give same result? #######


res_M2 = calculate_blob_data(scenario = "M2", # scenario name
                             coverage = "65", # coverage percentage
                             cf_coverage = "65",# coverage for cf
                             non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                             IU_order = "2",# IU order number
                             measure = "WC", # output measure, WC : worm count
                             elim = "MF",
                             mean_IU_pop ,
                             cost_scenario,
                             cost_development,
                             cost_cf, no_IUs =  50)


res_NC2 = calculate_blob_data(scenario = "NC", # scenario name
                              coverage = "80", # coverage percentage
                              cf_coverage = "65",# coverage for cf
                              non_compliance = "02",  # non-compliance parameter, equivalent to 0.2
                              IU_order = "2",# IU order number
                              measure = "WC", # output measure, WC : worm count
                              elim = "MF",
                              mean_IU_pop ,
                              cost_scenario = cost_cf * 8/6.5,
                              cost_development = 0,
                              cost_cf, 
                              no_IUs =  50)

plot(mean(res_M1[,"difference"]), mean(res_M1[,"elim_prob_scen"]), ylab = "Cost", xlab = "Difference",
     pch = 16, cex = sqrt(mean(res_M1[,"costs"]))/2, 
     xlim = c(min(mean(res_M1[,"difference"]),
                  mean(res_M2[,"difference"]),
                  mean(res_NC2[,"difference"])),
              max(mean(res_M1[,"difference"]),
                  mean(res_M2[,"difference"]),
                  mean(res_NC2[,"difference"]))),
     ylim = c(min(mean(res_M1[,"elim_prob_scen"]),
                  mean(res_M2[,"elim_prob_scen"]),
                  mean(res_NC2[,"elim_prob_scen"])),
              max(mean(res_M1[,"elim_prob_scen"]),
                  mean(res_M2[,"elim_prob_scen"]),
                  mean(res_NC2[,"elim_prob_scen"]))))
points(mean(res_M2[,"difference"]), mean(res_M2[,"elim_prob_scen"]), ylab = "Cost", xlab = "Difference",
       pch = 16, cex = sqrt(mean(res_M2[,"costs"]))/2)
points(mean(res_NC2[,"difference"]), mean(res_NC2[,"elim_prob_scen"]), ylab = "Cost", xlab = "Difference",
       pch = 16, cex = sqrt(mean(res_NC2[,"costs"]))/2)











