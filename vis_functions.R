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
  colnames(df)[ncols+1] <- "Total_MDA"
  colnames(df)[ncols+2] <- "post2020MDA"
  colnames(df)[ncols+3] <- "No_Pre_TAS_surveys"
  colnames(df)[ncols+4] <- "No_TAS_surveys"
  colnames(df)[ncols+5] <- "t_TAS_pass"
  
  
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
  scenario_data = 0
  simulation_name <- paste("0_Endgame", scenario, coverage, non_compliance,
                           IU_order, measure, sep = "_")
  filename <- paste0("output/",simulation_name, ".txt")
  
  info = file.info(filename)
  if(!is.na(info$size)){
    if(info$size > 10000){
      scenario_data <- parse_transfil_output(paste(filename, sep = "/"))
    }
    
  }
  
  
  
  return(scenario_data)
}



#' extract_medians 
#'
#' @param data_files 
#'
#' @return summary_res : median measure and number of MDAs
#'
#' @examples extract_medians(data_files)
extract_medians <- function(data_files, which_years){
  # extract needed columns from scenario and counter-factual (cf)
  measure_2030 <- data_files$data_scenario[ , c( "post2020MDA","No_Pre_TAS_surveys", "No_TAS_surveys", which_years)]
  measure_2030_cf <- data_files$data_cf[ , c("post2020MDA", "No_Pre_TAS_surveys", "No_TAS_surveys", which_years)]
  
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
  rownames(res) <- c( "post2020MDA","No_Pre_TAS_surveys", "No_TAS_surveys", which_years)
  
  return(summary_res = as.data.frame(res))
}


#' calculate_costs
#'
#' @param summary_res : the summary file to use for number of mdas 
#' @param cost_scenario : cost per MDA round in given scenario
#' @param cost_development : cost of development
#' @param cost_cf : cost per MDA round in counterfactual
#' @param population : population size of IU

#'
#' @return costs : cost difference of scenario versus counter-factual 
#'
#' @examples calculate_costs(summary_res, cost_scenario = 15, cost_development = 1000, cost_cf = 10)
calculate_costs <- function(summary_res, cost_scenario, cost_development, cost_cf, 
                            preTAS_survey_cost,
                            TAS_survey_cost, population){
  
  # extract median number mdas
  no_mdas_scenario <- summary_res["post2020MDA", "scenario"]
  no_mdas_cf <- summary_res["post2020MDA", "cf"]
  no_pre_tas_scenario <- summary_res["No_Pre_TAS_surveys", "scenario"]
  no_pre_tas_cf <- summary_res["No_Pre_TAS_surveys", "cf"]
  no_tas_scenario <- summary_res["No_TAS_surveys", "scenario"]
  no_tas_cf <- summary_res["No_TAS_surveys", "cf"]
  
  costs <- (no_mdas_scenario*cost_scenario*population + cost_development + preTAS_survey_cost * no_pre_tas_scenario + TAS_survey_cost * no_tas_scenario) #-  
    #(no_mdas_cf*cost_cf*population + preTAS_survey_cost * no_pre_tas_cf + TAS_survey_cost * no_tas_cf)
  
  return(costs)
}




#' calculate probability of elimination of LF in an IU between 2020 and 2030
#'
#' @param data_files_elim : file containing mf data, from which we will calculate the proportion 
#' of simulations which reach elimination between 2020 and 2030
#'
#' @return elim_mat : a matrix with proportion of simulations reaching elimination. 2 entries, one for cf and one for scenario
#' @export
#'
#' @examples calculate_probability_of_elimination(data_files_elim)
calculate_probability_of_elimination <- function(data_files_elim){
  k = which(data_files_elim$data_scenario$`Init cond` > 0.01) # only want to consider simulations where we begin with a >1% mf prevalence
  # check if there are any occasions between 2020 and 2030 where the mf prev is <1%. 
  # If there are any, then we would class that simulation as having reached elimination
  
  scen_m = data_files_elim$data_scenario[k,"t_TAS_pass"] # get the data we want to look at
  cf_m = data_files_elim$data_cf[k,"t_TAS_pass"] # get the data we want to look at
  elim_mat = data.frame(matrix(0, 1, 2)) # set up matrix to hold probs
  
  colnames(elim_mat) = c("cf_elim", "scen_elim")

  # elim_mat[1] = sum(apply(X = cf_m, MARGIN = 1, FUN = check_sim_elimination)) / nrow(cf_m)# calculate the proportion of simulations which go below 1%
  # elim_mat[2] = sum(apply(X = scen_m, MARGIN = 1, FUN = check_sim_elimination)) / nrow(scen_m)# calculate the proportion of simulations which go below 1%
  
  elim_mat[1] = length(which(cf_m > 0))/length(cf_m) # calculate the proportion of simulations which go below 1%
  elim_mat[2] = length(which(scen_m > 0))/length(scen_m) # calculate the proportion of simulations which go below 1%
  return(elim_mat)
}


#' function to check which simulations, held in a matrix with each simulation on a row, have reach elimination
#'
#' @param x : matrix holding simulation data
#'
#' @return 
#' @export
#'
#' @examples 
check_sim_elimination <- function(x){
  any(x < 0.01)
}





calculate_blob_data <- function(scenario, # scenario name
                                coverage, # coverage percentage
                                cf_coverage, # coverage for cf
                                non_compliance,  # non-compliance parameter, equivalent to 0.2
                                measure, # output measure, WC : worm count
                                elim ,
                                cost_scenario,
                                cost_development,
                                cost_cf, 
                                no_IUs,
                                which_years,
                                preTAS_survey_cost ,
                                TAS_survey_cost){
  
  IUs = read.csv("runIU.csv")
  IUs_vec <- which(IUs$IUID == 1)
  
  # empty matrix to store results
  res <- matrix(ncol = 5, nrow = no_IUs)
  colnames(res) <- c("IU_order", "difference", "elim_prob_cf",  "elim_prob_scen", "costs")
  # cf_res <- matrix(ncol = 5, nrow = no_IUs)
  # colnames(cf_res) <- c("IU_order", "num_worms", "elim_prob", "num_MDAs", "costs")
  # scen_res <- matrix(ncol = 5, nrow = no_IUs)
  # colnames(scen_res) <- c("IU_order", "num_worms", "elim_prob", "num_MDAs",  "costs")
  # 
  for(i in 1:no_IUs){
    population <- IUs$pop[IUs_vec[i]] # choose some random population size for the IU
    
    data_files <- read_files_def_cf(scenario, coverage, cf_coverage, non_compliance,
                                    IU_order = IUs_vec[i], measure)
    
    data_files_elim <- read_files_def_cf(scenario, coverage,cf_coverage,  non_compliance,
                                         IU_order = IUs_vec[i], elim)
    if(data_files$data_scenario != 0 ){
      # calculate probability of elimination for cf and scen
      elim_prob = calculate_probability_of_elimination(data_files_elim)
      
      # calculate relevant measure of impact 
      summary_res <- extract_medians(data_files, which_years)
      
      # calculate relevant costs
      costs <- calculate_costs(summary_res, cost_scenario, cost_development, cost_cf, preTAS_survey_cost ,
                               TAS_survey_cost, population)
      
      diff_measures <- population * summary_res[which_years, "cf"] - population * summary_res[which_years,"scenario"] 
      
      
      if(length(which_years > 1)) diff_measures <- sum(diff_measures)
      
      
      # just store difference in measure
      res[i, ] <- c(IUs_vec[i], diff_measures, elim_prob$cf_elim, elim_prob$scen_elim, costs)
      # cf_res[i, ] <- c(IUs_vec[i], summary_res$cf[1]* random_population, elim_prob$cf_elim, summary_res$cf[2], summary_res$cf[2] * cost_cf)
      # scen_res[i, ] <- c(IUs_vec[i], summary_res$scenario[1]* random_population, elim_prob$scen_elim, 
    }
    
    #                    summary_res$scenario[2], 
    #                    cost_development +summary_res$scenario[2] * cost_scenario)
    
  }
  x = which(!is.na(res[,1]))
  res = res[x, ]
  return(res)
  
}


#' read_files_def_cf
#' reads in scenario and partner counter-factual file
#'
#' @param scenario : NC, M1, M2 etc
#' @param coverage : coverage percentage
#' @param non_compliance : non-compliance parameter, 02 equivalent to 0.2
#' @param IU_order : IU order number
#' @param measure : output measure, WC : worm count, MF : mf prev, IC : ict prev
#' @param cf_coverage : the coverage level for the  cf
#'
#' @examples
read_files_def_cf <- function(scenario, coverage, 
                              cf_coverage = "65", 
                              non_compliance,
                              IU_order, measure){
  # scenario file
  data_scenario <- read_scenario(scenario, coverage, non_compliance,
                                 IU_order, measure)
  
  # counter-factual partner file (scenario : NC)
  data_cf <- read_scenario(scenario = "NC", cf_coverage, non_compliance,
                           IU_order, measure)
  
  return(list(data_scenario = data_scenario, data_cf = data_cf))
}

#' add_blobs
#' add blobs to existing plot
#' @param res 
#' @param label 
#'
#' @return 
#'
#' @examples add_points(res_M2, "M2")
add_blobs <- function(res, label, cex_i){
  points(mean(res[,"difference"]), mean(res[,"elim_prob_scen"]),
         pch = 16, cex = cex_i, col = rgb(0/255,154/255,205/255, 0.6))
  text(mean(res[,"difference"]), mean(res[,"elim_prob_scen"]), labels = label) 
}


#' find_mean_difference
#' returns the mean difference, used to calculate xlim
#' @param x 
#'
#'
#' @examples lapply(res_list, find_mean_difference)
find_mean_difference <- function(x){
  mean(x[,"difference"])
} 

#' find_mean_elim_prob
#' returns the mean elim prob, used to calculate ylim
#' @param x 
#'
#'
#' @examples  lapply(res_list, find_mean_elim_prob)
find_mean_elim_prob <- function(x){
  mean(x[,"elim_prob_scen"])
}


#' find_mean_cost
#' returns the mean elim prob, used to calculate ylim
#' @param x 
#'
#'
#' @examples  lapply(res_list, find_mean_cost)
find_mean_cost <- function(x){
  mean(x[,"costs"])
}


#' make_blob_plot
#'
#' @param res_list 
#' @param labels 
#'
#'
#' @examples make_blob_plot(res_list, labels)
make_blob_plot <- function(res_list, labels){
  
  mean_diff <- unlist(lapply(res_list, find_mean_difference))
  mean_prob <- unlist(lapply(res_list, find_mean_elim_prob))
  mean_cost <- unlist(lapply(res_list, find_mean_cost))
  
  pos_costs <- mean_cost+abs(min(mean_cost)) # make all costs positive
  cex_vec <- (pos_costs)/max(pos_costs)*20 # cex is relative to max mean cost
  
  plot(NA, NA, ylab = "Mean prob. of elimination by 2030", xlab = "Extra DALYs averted",
       xlim = c(-100, 1.05*max(mean_diff)),
       ylim = c(0, 1.15*max(mean_prob)),
       bty = 'n', cex.axis = 1.5, cex.lab = 1.5)
  
  for(i in 1:length(res_list)){
    add_blobs(res_list[[i]], labels[i], cex_vec[i])
  }
}
