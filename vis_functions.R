#' parse_transfil_output
#' Parses output from LF model to R data frame 
#' @param filename the filename of the file to be parsed
#'
#' @return the parsed output : Population, Output for each time point, Number of MDAs,
#' Number of surveys
#'
#' @examples
#' parse_transfil_output("0_IDA_TAS_Test_MF.txt")
parse_transfil_output <- function(filename){
  rawdata <- readLines(filename)
  n <- length(rawdata)
  df <- data.frame(matrix(ncol = length(strsplit(rawdata[14],"\t")[[1]][-c(1,3:7)]), nrow = 0))
  
  cnames <- strsplit(rawdata[4],"\t")[[1]][-c(1,3:7)]
  colnames(df) <- cnames
  colnames(df)[1] <- "Population"
  ncols <- length(cnames)
  colnames(df)[ncols+1] <- "No_MDA"
  colnames(df)[ncols+2] <- "No_surveys"
  
  
  for (i in 14:n){
    
    line <- strsplit(rawdata[i],"\t")[[1]]
    line <- line[-c(1,3:7)]
  
    df[nrow(df) + 1,] <- as.numeric(line)
    
  } 
  
  return(df)
}

#' read_scenario
#' a wrapper function to read in and parse data file
#' @param scenario : NC, M1, M2 etc
#' @param coverage : coverage percentage
#' @param non_compliance : non-compliance parameter, 02 equivalent to 0.2
#' @param IU_order : IU order number
#' @param measure : output measure, WC : worm count, MF : mf prev, IC : ict prev
#'
#' @return parsed data for given scenario
#'
#' @examples read_scenario(scenario = "NC", coverage = "80", non_compliance = "02", IU_order = "2", measure = "WC")
read_scenario <- function(scenario, coverage, non_compliance,
                          IU_order, measure){
  simulation_name <- paste("0_Endgame", scenario, coverage, non_compliance,
                           IU_order, measure, sep = "_")
  filename <- paste0(simulation_name, ".txt")
  scenario_data <- parse_transfil_output(paste("output",filename, sep = "/"))
  return(scenario_data)
}


#' read_files
#' reads in scenario and partner counter-factual file
#'
#' @param scenario : NC, M1, M2 etc
#' @param coverage : coverage percentage
#' @param non_compliance : non-compliance parameter, 02 equivalent to 0.2
#' @param IU_order : IU order number
#' @param measure : output measure, WC : worm count, MF : mf prev, IC : ict prev
#'
#'
#' @examples
read_files <- function(scenario, coverage, non_compliance,
                       IU_order, measure){
  # scenario file
  data_scenario <- read_scenario(scenario, coverage, non_compliance,
                                 IU_order, measure)
  
  # counter-factual partner file (scenario : NC)
  data_cf <- read_scenario(scenario = "NC", coverage, non_compliance,
                           IU_order, measure)
  
  return(list(data_scenario = data_scenario, data_cf = data_cf))
}

#' extract_medians 
#'
#' @param data_files 
#'
#' @return summary_res : median measure and number of MDAs
#'
#' @examples extract_medians(data_files)
extract_medians <- function(data_files){
  # extract needed columns from scenario and counter-factual (cf)
  measure_2030 <- data_files$data_scenario[ , c("Jan-2030", "No_MDA")]
  measure_2030_cf <- data_files$data_cf[ , c("Jan-2030", "No_MDA")]
  
  # calculate summary measures across simulations
  # median worm count and median number of MDA rounds
  # scenario
  scenario_summary <- apply(measure_2030, 2, quantile,
                            probs=c(0.5, 0.025, 0.975))
  # counter-factual
  cf_summary <- apply(measure_2030_cf, 2, quantile,
                               probs=c(0.5, 0.025, 0.975))
  
  res <- matrix(c(scenario_summary[1,], cf_summary[1,]), byrow = F, ncol = 2)
  colnames(res) <- c("scenario", "cf")
  
  return(summary_res = as.data.frame(res))
}


#' calculate_costs
#'
#' @param summary_res : the summary file to use for number of mdas 
#' @param cost_scenario : cost per MDA round in given scenario
#' @param cost_development : cost of development
#' @param cost_cf :c ost per MDA round in counterfactual
#'
#' @return costs : cost difference of scenario versus counter-factual 
#'
#' @examples calculate_costs(summary_res, cost_scenario = 15, cost_development = 1000, cost_cf = 10)
calculate_costs <- function(summary_res, cost_scenario, cost_development, cost_cf){

  # extract median number mdas
  no_mdas_scenario <- summary_res$scenario[2]
  no_mdas_cf <- summary_res$cf[2]
  
  costs <- (no_mdas_scenario*cost_scenario + cost_development) -  no_mdas_cf*cost_cf
  
  return(costs)
}