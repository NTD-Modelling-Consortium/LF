setwd("~/Documents/GitHub/")
source("vis_functions.R")

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

IUs_vec <- c(2, 5, 7, 8, 13) 
no_IUs <- length(IUs_vec)

# empty matrix to store results
res <- matrix(ncol = 3, nrow = no_IUs)
colnames(res) <- c("IU_order", "difference", "costs")

for(i in 1:no_IUs){
  
  data_files <- read_files(scenario, coverage, non_compliance,
                           IU_order = IUs_vec[i], measure)
  
  # calculate relevant measuyre of impact (median at 2030)
  summary_res <- extract_medians(data_files)
  
  # calculate relevant costs
  costs <- calculate_costs(summary_res, cost_scenario = 15, cost_development = 1000, cost_cf = 10)
  
  diff_measures <- summary_res$cf - summary_res$scenario
  # just store difference in measure
  res[i, ] <- c(IUs_vec[i], diff_measures[1], costs)
}

plot(res[,"difference"], res[,"costs"], ylab = "Cost", xlab = "Difference")
abline(v = 0, lty = "dashed", col = "gray")

# how to present cumulative worm count reduction given cost per IU?
# perhaps infected people would be easier as we could sum it (DALYS would be summed I assume)
sum(res[,"costs"])

######## compare other scenarios #######



######## extra : do all measures (WC, MF, IC) give same result? #######
