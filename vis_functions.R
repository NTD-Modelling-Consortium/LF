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


# 
#' extract_simulation
#' extract a simulation instead of a median
#' @param data_files 
#' @param which_years 
#' @param j the simulation number
#'
#' @return
#' @export
#'
#' @examples
extract_simulation <- function(data_files, which_years, j){
  # extract needed columns from scenario and counter-factual (cf)
  measure_2030 <- data_files$data_scenario[ , c( "post2020MDA","No_Pre_TAS_surveys", "No_TAS_surveys", which_years)]
  measure_2030_cf <- data_files$data_cf[ , c("post2020MDA", "No_Pre_TAS_surveys", "No_TAS_surveys", which_years)]
  
  # scenario
  scenario_summary <- as.numeric(measure_2030[j,])
  # counter-factual
  cf_summary <- as.numeric(measure_2030_cf[j,])
  
  res <- matrix(c(scenario_summary, cf_summary), byrow = F, ncol = 2)
  colnames(res) <- c("scenario", "cf")
  rownames(res) <- c( "post2020MDA","No_Pre_TAS_surveys", "No_TAS_surveys", which_years)
  
  return(summary_res = as.data.frame(res))
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

#' calculate_elimination_sim
#' extract whether elimination occured or not 
#' @param data_files_elim 
#' @param j simulation number
#'
#' @return
#' @export
#'
#' @examples
calculate_elimination_sim <- function(data_files_elim, j){
  
  elim_mat = data.frame(matrix(0, 1, 2)) # set up matrix to hold probs
  colnames(elim_mat) = c("cf_elim", "scen_elim")
  
  scen_m = data_files_elim$data_scenario[j,"t_TAS_pass"] # get the data we want to look at
  cf_m = data_files_elim$data_cf[j,"t_TAS_pass"] # get the data we want to look at
  
  elim_mat = data.frame(matrix(0, 1, 2)) # set up matrix to hold probs
  colnames(elim_mat) = c("cf_elim", "scen_elim")
  
  
  elim_mat[1] = as.numeric((cf_m > 0) & (cf_m < 371)) # passed TAS survey before 2030?
  elim_mat[2] = as.numeric((scen_m > 0) & (scen_m < 371)) # passed TAS survey before 2030?
  return(elim_mat)
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
  
  elim_mat[1] = length(which((cf_m > 0) & (cf_m < 371)))/length(cf_m) # calculate the proportion of simulations which passed TAS survey before 2030
  elim_mat[2] = length(which((scen_m > 0) & (scen_m < 371)))/length(scen_m) # calculate the proportion of simulations which passed TAS survey before 2030
  return(elim_mat)
}


#' check_sim_elimination
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



#' calculate_number_infected_over_time 
#'
#' @param d the simulation data file to obtain prevalences from 
#' @param population population size
#'
#' @return
#' @export
#'
#' @examples
calculate_number_infected_over_time <- function(d, population){
  c1 = which(colnames(d) == "Jan-2020")
  c2 = which(colnames(d) == "Jan-2030")
  prevs = d[, c1:c2]
  number_infected = colMeans(population * prevs)
  return(number_infected)
}

#' calculate_number_IUs_stopped_MDA 
#'
#' @param d the simulation data file
#'
#' @return
#' @export
#'
#' @examples
calculate_number_IUs_stopped_MDA <- function(d){
  years = (20:30)*12
  stop_time = d$t_TAS_pass
  prop_stopped = matrix(length(years), 1)
  for(i in 1:length(years)){
    prop_stopped[i] = length(which(stop_time <= years[i]))/length(stop_time)
  }
  
  return(prop_stopped)
}

#' calculate_blob_data
#' a wrapper to loop over scenarios, calculate measures of impact and cost
#' and return data in format to make blob plot
#'
#'
#' @param scenario 
#' @param coverage 
#' @param cf_coverage 
#' @param non_compliance 
#' @param cf_non_compliance 
#' @param measure 
#' @param elim 
#' @param cost_scenario 
#' @param cost_development 
#' @param cost_cf 
#' @param which_years 
#' @param preTAS_survey_cost 
#' @param TAS_survey_cost 
#' @param IUs_vec 
#'
#' @return
#' @export
#'
#' @examples
calculate_blob_data <- function(scenario, # scenario name
                                coverage, # coverage percentage
                                cf_coverage, # coverage for cf
                                non_compliance,  # non-compliance parameter, equivalent to 0.2
                                cf_non_compliance , # # non-compliance parameter for cf
                                measure, # output measure, WC : worm count
                                elim ,
                                cost_scenario,
                                cost_development,
                                cost_cf, 
                                which_years,
                                preTAS_survey_cost ,
                                TAS_survey_cost,
                                IUs_vec){
  
  no_IUs <- length(IUs_vec) 
  # empty matrix to store results
  res <- matrix(ncol = 5, nrow = no_IUs)
  colnames(res) <- c("IU_order", "difference", "elim_prob_cf",  "elim_prob_scen", "costs")
  
  num_infections_over_time <- matrix(ncol = 11, nrow = no_IUs)
  prop_IUs_Finished_MDA <- matrix(ncol = 11, nrow = no_IUs)
  
  for(i in 1:no_IUs){
    population <- IUs$pop[IUs_vec[i]] # choose some random population size for the IU
    
    data_files <- read_files_def_cf(scenario, coverage, cf_coverage, cf_non_compliance, non_compliance, 
                                    IU_order = IUs_vec[i], measure)
    
    data_files_elim <- read_files_def_cf(scenario, coverage,cf_coverage,cf_non_compliance,  non_compliance,
                                         IU_order = IUs_vec[i], elim)
    if(length(data_files$data_scenario) > 1 ){
      # calculate probability of elimination for cf and scen
      elim_prob <- calculate_probability_of_elimination(data_files_elim)
      
      # calculate relevant measure of impact 
      summary_res <- extract_medians(data_files, which_years)
      
      # calculate relevant costs
      costs <- calculate_costs(summary_res, cost_scenario, cost_development, cost_cf, preTAS_survey_cost ,
                               TAS_survey_cost, population)
      
      # calculate the difference in (proxy for) DALYs averted
      diff_measures <- population * summary_res[which_years, "cf"] - population * summary_res[which_years,"scenario"] 
      
      # are measures being calculated for more than one year? if yes, then sum
      if(length(which_years > 1)) diff_measures <- sum(diff_measures)
      
      #  store all results
      res[i, ] <- c(IUs_vec[i], diff_measures, elim_prob$cf_elim, elim_prob$scen_elim, costs)
      num_infections_over_time[i, ] <- calculate_number_infected_over_time(data_files$data_scenario, population)
      prop_IUs_Finished_MDA[i, ] <- calculate_number_IUs_stopped_MDA(data_files$data_scenario)
    }
    
    
  }
  x <- which(!is.na(res[,1]))
  res <- res[x, ]
  num_infections_over_time <-  num_infections_over_time[x, ]
  prop_IUs_Finished_MDA <- prop_IUs_Finished_MDA[x, ]
  return(list(res = res, num_infections_over_time = num_infections_over_time, prop_IUs_Finished_MDA = prop_IUs_Finished_MDA))
  
}

#' calculate_cloud_data 
#' wrapper to loop over each simulation across all IUs
#' 
#' 
#' @param scenario 
#' @param coverage 
#' @param cf_coverage 
#' @param non_compliance 
#' @param cf_non_compliance 
#' @param measure 
#' @param elim 
#' @param cost_scenario 
#' @param cost_development 
#' @param cost_cf 
#' @param which_years 
#' @param preTAS_survey_cost 
#' @param TAS_survey_cost 
#' @param IUs_vec 
#' @param nsims 
#'
#' @return
#' @export
#'
#' @examples
calculate_cloud_data <- function(scenario, # scenario name
                                 coverage, # coverage percentage
                                 cf_coverage, # coverage for cf
                                 non_compliance,  # non-compliance parameter, equivalent to 0.2
                                 cf_non_compliance , # # non-compliance parameter for cf
                                 measure, # output measure, WC : worm count
                                 elim ,
                                 cost_scenario,
                                 cost_development,
                                 cost_cf, 
                                 which_years,
                                 preTAS_survey_cost ,
                                 TAS_survey_cost,
                                 IUs_vec,nsims){
  
  no_IUs <- length(IUs_vec)
  
  # empty matrix to store results
  # each line is average across one simulation for all IUs
  res_sim <- matrix(ncol = 5, nrow = nsims)
  colnames(res_sim) <- c("sim", "difference", "elim_prob_cf",  "elim_prob_scen", "costs")
  
  for(j in 1:nsims){
    
    # empty matrix to store results
    res <- matrix(ncol = 5, nrow = no_IUs)
    colnames(res) <- c("IU_order", "difference", "elim_prob_cf",  "elim_prob_scen", "costs")
    
    #num_infections_over_time <- matrix(ncol = 11, nrow = no_IUs)
    #prop_IUs_Finished_MDA <- matrix(ncol = 11, nrow = no_IUs)
    
    for(i in 1:no_IUs){
      population <- IUs$pop[IUs_vec[i]] # choose some random population size for the IU
      
      data_files <- read_files_def_cf(scenario, coverage, cf_coverage, cf_non_compliance, non_compliance, 
                                      IU_order = IUs_vec[i], measure)
      
      data_files_elim <- read_files_def_cf(scenario, coverage,cf_coverage,cf_non_compliance,  non_compliance,
                                           IU_order = IUs_vec[i], elim)
      if(length(data_files$data_scenario) > 1 ){
        # calculate probability of elimination for cf and scen
        elim_prob <- calculate_elimination_sim(data_files_elim, j)
        
        # extract simuation j 
        res_j <- extract_simulation(data_files, which_years, j)
        
        # calculate relevant costs
        costs <- calculate_costs(res_j, cost_scenario, cost_development, cost_cf, preTAS_survey_cost ,
                                 TAS_survey_cost, population)
        
        diff_measures <- population * res_j[which_years, "cf"] - population * res_j[which_years,"scenario"] 
        
        
        if(length(which_years > 1)) diff_measures <- sum(diff_measures)
        
        # just store difference in measure
        res[i, ] <- c(IUs_vec[i], diff_measures, elim_prob$cf_elim, elim_prob$scen_elim, costs)
        
        # # these  do not work for one simulation currently
        # num_infections_over_time[i, ] = calculate_number_infected_over_time(data_files$data_scenario[j], population)
        # prop_IUs_Finished_MDA[i, ] = calculate_number_IUs_stopped_MDA(data_files$data_scenario[j])
        
      }
    }
    x = which(!is.na(res[,1]))
    res = res[x, ]
    # num_infections_over_time =  num_infections_over_time[x, ]
    # prop_IUs_Finished_MDA = prop_IUs_Finished_MDA[x, ]
    
    # calculate average across IUs and store
    means_j <- apply(res[,c("difference", "elim_prob_cf", "elim_prob_scen", "costs")], 2, mean)
    
    res_sim[j, ] <- c(j, means_j)
  }
  
  return(res = res_sim)
  
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
                              cf_non_compliance, 
                              non_compliance,
                              IU_order, measure){
  # scenario file
  data_scenario <- read_scenario(scenario, coverage, non_compliance,
                                 IU_order, measure)
  
  # counter-factual partner file (scenario : NC)
  data_cf <- read_scenario(scenario = "NC", cf_coverage, cf_non_compliance,
                           IU_order, measure)
  
  return(list(data_scenario = data_scenario, data_cf = data_cf))
}




#' read_file - read one simulation file in
#' reads in scenario  file
#'
#' @param scenario : NC, M1, M2 etc
#' @param coverage : coverage percentage
#' @param non_compliance : non-compliance parameter, 02 equivalent to 0.2
#' @param IU_order : IU order number
#' @param measure : output measure, WC : worm count, MF : mf prev, IC : ict prev
#'
#' @examples
read_file <- function(scenario, coverage, 
                      non_compliance,
                      IU_order, measure){
  # scenario file
  data_scenario <- read_scenario(scenario, coverage, non_compliance,
                                 IU_order, measure)
  
  return(data_scenario)
}

#' add_blobs
#' add blobs to existing plot
#'
#' @param res : results for one scenario
#' @param label : label for scenario
#' @param cex_i : size of blob for scenario
#' @param subtract_cost : cost to subtract from cost of scenario (cost of original scenario)
#'
#' @return 
#'
#' @examples add_points(res_M2, "M2", 3, 10)
add_blobs <- function(res, label, cex_i, subtract_cost= 0 , col){
  points(mean(res[,"difference"]), mean(res[,"costs"])- subtract_cost,
         pch = 16, cex = cex_i, col = col)
  text(mean(res[,"difference"]), mean(res[,"costs"])- subtract_cost, labels = label) 
}


#' add blobs to existing plot
#'
#' @param res : results for one scenario
#' @param label : label for scenario
#' @param col_i : colour of blob, defined by net monetary and elimination benefit
#' @param subtract_cost : cost to subtract from cost of scenario (cost of original scenario)
#'
#' @return 
#'
#' @examples add_points(res_M2, "M2")
add_blobs_v2 <- function(res, label, col_i, subtract_cost= 0 ){
  points(mean(res[,"difference"]), mean(res[,"costs"])- subtract_cost,
         pch = 16, cex = 10, col = rgb(col_i[1]/255, col_i[2]/255, col_i[3]/255, 0.6))
  text(mean(res[,"difference"]), mean(res[,"costs"])- subtract_cost, labels = label) 
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


#' TAS_pass_test 
#'
#' @param x value of time when TAS is passed
#'
#' @return
#' @export
#'
#' @examples
TAS_pass_test <- function(x){
  ifelse(((x<371) && (x>0)),1,0)
}

#' make_blob_plot
#'
#' @param res_list 
#' @param labels 
#'
#'
#' @examples make_blob_plot(res_list, labels)
make_blob_plot <- function(res_list, labels, cols){
  
  mean_diff <- unlist(lapply(res_list, find_mean_difference))
  mean_prob <- unlist(lapply(res_list, find_mean_elim_prob))
  mean_cost <- unlist(lapply(res_list, find_mean_cost))
  
  pos_costs <- mean_cost+abs(min(mean_cost)) # make all costs positive
  cex_vec <- (pos_costs)/max(pos_costs)*20 # cex is relative to max mean cost
  
  plot(NA, NA, ylab = "Additional arbitrary costs until 2030", xlab = "Proxy for extra DALYs averted",
       xlim = c(-100, 1.05*max(mean_diff)),
       ylim = c(1.5*(min(mean_cost)-mean_cost[1]), # subtract cost of strategy 0 to get extra cost per scenario
                1.15*(max(mean_cost)-mean_cost[1])),
       bty = 'n', cex.axis = 1.5, cex.lab = 1.5)
  
  for(i in 1:length(res_list)){
    add_blobs(res_list[[i]], labels[i], cex_vec[i], subtract_cost = mean_cost[1], col = cols[i])
  }
}




#' make plots of how one variable changes over time for each scenario
#'

#'
#' @param r : results to plot
#' @param labels : labels for scenarios
#' @param y_label : label for the y-axis, as this will change depending on what is being plotted
#'
#' @examples make_blob_plot(res_list, labels)
make_time_plots <- function(r, labels, y_label){
  
  mean_infections <- lapply(r, colMeans)
  max_infections = 0
  for(i in 1:length(r)){
    max_infections = max(max_infections, max(mean_infections[[i]]))
  }
  cols <- hcl.colors(length(r), palette = "ag_Sunset")
  plot(2020:2030,mean_infections[[1]], ylab = y_label, xlab = "year", 
       bty = 'n', type = 'b', lwd = 2, pch =16, ylim = c(0,max_infections),
       cex.axis = 1.5, cex.lab = 1.5, col = cols[1])
  for(i in 2:length(r)){
    lines(2020:2030, mean_infections[[i]], lwd = 2, col = cols[i])
    points(2020:2030, mean_infections[[i]], pch = 16, col = cols[i])
  }
  legend("bottomleft", bty = "n", col = cols, legend = labels, lwd = 3, ncol = 2, pch = 16)
}


#' make_blob_plot_v2 : all blobs are the same size and the colour of each blob is defined
#' by net monetary and elimination benefit calculation
#' @param res_list 
#' @param labels 
#'
#' @param lambda_DALY : amount of money we are willing to spend to avert one DALY
#' @param lambda_EOT : amount of money we are willing to spend to increase probability of elimination by 1%
#'
#' @examples make_blob_plot(res_list, labels)
make_blob_plot_v2 <- function(res_list, labels, lambda_DALY, lambda_EOT){
  
  mean_diff <- unlist(lapply(res_list, find_mean_difference))
  mean_prob <- unlist(lapply(res_list, find_mean_elim_prob))
  mean_cost <- unlist(lapply(res_list, find_mean_cost))
  
  pos_costs <- mean_cost+abs(min(mean_cost)) # make all costs positive
  pal <- colorRamp(c("red", "blue")) # make a colour palette
  nmeb = calculate_nmeb(lambda_DALY, lambda_EOT, res_list) # calculate net monetary and elimination benefit
  if(any( nmeb < 0)){
    nmeb = nmeb - min(nmeb)
  }
  nmeb_col = nmeb/max(nmeb)
  col_vec <- pal(nmeb_col) # cex is relative to max mean cost
  
  plot(NA, NA, ylab = "Additional arbitrary costs until 2030", xlab = "Proxy for extra DALYs averted",
       xlim = c(-100, 1.05*max(mean_diff)),
       ylim = c(1.5*(min(mean_cost)-mean_cost[1]), 1.15*(max(mean_cost)-mean_cost[1])),
       bty = 'n', cex.axis = 1.5, cex.lab = 1.5)
  points(-999999999, 1,
         pch = 16, cex = 1)
  for(i in 1:length(res_list)){
    add_blobs_v2(res_list[[i]], labels[i], col_vec[i,], subtract_cost = mean_cost[1])
  }
}



#' make_cloud_plot
#' makes cloud plot from cloud plot data obtained from calculate_cloud_data
#' @param res_list 
#' @param labels 
#' @param cols 
#' @param cex_fixed 
#'
#' @return
#' @export
#'
#' @examples
make_cloud_plot <- function(res_list, labels, cols, cex_fixed = TRUE){
  
  mean_diff <- unlist(lapply(res_list, find_mean_difference))
  mean_prob <- unlist(lapply(res_list, find_mean_elim_prob))
  mean_cost <- unlist(lapply(res_list, find_mean_cost))
  
  pos_costs <- mean_cost+abs(min(mean_cost)) # make all costs positive
  #cex_vec <- (pos_costs)/max(pos_costs)*20 # cex is relative to max mean cost
  
  plot(NA, NA, ylab = "Additional arbitrary costs until 2030", xlab = "Proxy for extra DALYs averted",
       xlim = c(-100, 1.05*max(mean_diff)),
       ylim = c(1.5*(min(mean_cost)-mean_cost[1]), # subtract cost of strategy 0 to get extra cost per scenario
                1.15*(max(mean_cost)-mean_cost[1])),
       bty = 'n', cex.axis = 1.5, cex.lab = 1.5)
  
  for(i in 1:length(res_list)){
    add_cloud(res_list[[i]], labels[i], cols[i], subtract_cost= mean_cost[1], 
              max_cost = max(pos_costs),res_0 = res_list[[1]], cex_fixed)
  }
  
  legend("topleft", bty = "n", col = cols, legend = labels, ncol = 2, pch = 16, cex = 1.5)
  
}


#' add_cloud 
#'  called from within make_cloud_plot
#' @param res 
#' @param label 
#' @param col 
#' @param subtract_cost 
#' @param max_cost 
#' @param res_0 
#' @param cex_fixed 
#'
#' @return
#' @export
#'
#' @examples
add_cloud <- function(res, label, col, subtract_cost, max_cost, res_0, cex_fixed){
  nsims <- dim(res)[1]
  
  #add cloud part for each simulation
  for(j in 1:nsims){
    # subtract the corresponding cost from scenario 0 for each simulation
    add_cloud_part(res[j,], labels[i], col, subtract_cost = res_0[j,"costs"], max_cost, cex_fixed)
  }
}


#' add_cloud_part
#' called from within add_cloud
#' @param res_j 
#' @param label 
#' @param col 
#' @param subtract_cost 
#' @param max_cost 
#' @param cex_fixed 
#'
#' @return
#' @export
#'
#' @examples
add_cloud_part <- function(res_j, label, col, subtract_cost= 0, max_cost, cex_fixed){
  if(cex_fixed == TRUE){
    cex_i <- 3
  }else{
    cex_i <- res_j["costs"]/max_cost*10
  }
  points(res_j["difference"], res_j["costs"]- subtract_cost,
         pch = 16, cex = cex_i, col = col)
}



#' calculate net-monetary benefit from economic evaluation paper
#'
#' @param lambda_DALY : maximum price willing to pay to avert one DALY
#' @param res_list : combined data set of different strategies
#'
#' @return net monetary benefit of each strategy
#' @export
#'
#' @examples calculate_nmb(10, res_list)
calculate_nmb <- function(lambda_DALY, res_list){
  mean_diff <- unlist(lapply(res_list, find_mean_difference)) # calculate mean difference in DALYs averted by each strategy
  mean_cost <- unlist(lapply(res_list, find_mean_cost)) # calculate mean cost of each strategy
  
  return(lambda_DALY*mean_diff - (mean_cost - mean_cost[1])) # return the nmb of each strategy
}


#' calculate net-monetary and elimination benefit from economic evaluation paper
#'
#' @param lambda_DALY : maximum price willing to pay to avert one DALY
#' @param lambda_EOT : maximum price willing to pay for additional 1% increase in probability of elimination
#' @param res_list : combined data set of different strategies
#'
#' @return net monetary and elimination benefit of each strategy
#' @export
#'
#' @examples calculate_nmeb(10, 5, res_list)
calculate_nmeb <- function(lambda_DALY, lambda_EOT, res_list){
  mean_diff <- unlist(lapply(res_list, find_mean_difference)) # calculate mean difference in DALYs averted by each strategy
  mean_prob <- unlist(lapply(res_list, find_mean_elim_prob)) # calculate mean probability of elimination for each strategy
  mean_cost <- unlist(lapply(res_list, find_mean_cost)) # calculate mean cost of each strategy
  
  return(100 * lambda_EOT*(mean_prob-mean_prob[1]) + lambda_DALY*mean_diff - (mean_cost - mean_cost[1])) # return the nmeb of each strategy
}


# # add cloud part for nmeb plot
# add_cloud_part_nmeb <- function(res_j, label, col, subtract_cost= 0, max_cost,
#                                 lambda_DALY, lambda_EOT, res_0){
#   cex_i <- res_j["costs"]/max_cost*10
#   nmeb_sim <-calculate_nmeb_simulation(lambda_DALY, lambda_EOT, res_j, res_0)
#   #### use nmeb sim to determine colour ####
#   #### but this needs to be realtive to other nmeb AND within scenario ####
#   points(res_j["difference"], res_j["costs"]- subtract_cost,
#          pch = 16, cex = cex_i, col = col)
# }
# 
# # calculate nmeb per simulation
# calculate_nmeb_simulation <- function(lambda_DALY, lambda_EOT, res, res_0){
#   return(100 * lambda_EOT*(res$elim_prob_scen -elim_prob_cf ) + lambda_DALY*res$difference - (res$costs - res_$costs)) # return the nmeb of each strategy
# }



#' find whether there was true elimination in a simulation
#'
#' @param x : data over time period we are considering
#'
#' @return : TRUE or FALSE
true_elimination <- function(x){
  any(x == 0)
}

#' Calculate data needed for cloud data
#' Idea of this function is to produce a daly array and a cost array both of which 
#' are averaged over each individual simulation for the chosen IUs. 
#' These can then be used to give the data needed for the cloud plot when compared with the baseline
#'
#' @param IUs_vec : IUs to do calculation for
#' @param scenario : which scenario to generate data for
#' @param coverage : coverage of mda
#' @param non_compliance : compliance of mda
#' @param cost_development : development of cost for the drug 
#' @param cost_scenario : cost of a set of drugs for an individual
#' @param preTAS_survey_cost : cost of the pre TAS survey
#' @param TAS_survey_cost : cost of the TAS survey
#'
#' @return mean dalys and cost over each IU for simulations set 1, 2, 3,...
#' @export
#'
#' @examples scen0 = calculate_dalys_and_costs_for_scenario_cloud(IUs_vec, scenario = "NC", coverage = "65", 
#'                                                                non_compliance = "03", cost_development = 0, 
#'                                                                cost_scenario = 0.01,
#'                                                                preTAS_survey_cost = 5, TAS_survey_cost = 5)
calculate_dalys_and_costs_for_scenario_cloud <- function(IUs_vec, scenario, coverage, 
                                                         non_compliance, cost_development, cost_scenario,
                                                         preTAS_survey_cost, TAS_survey_cost,
                                                         which_years){
  
  IUs <- read.csv("runIU.csv") # read in IU file, so we have access to population data
  no_IUs = length(IUs_vec)
  dalys = matrix(NA, no_IUs * 200, 1) # generate array to store daly data
  costs = matrix(NA, no_IUs * 200, 1) # generate array to store cost data
  elims = matrix(NA, no_IUs * 200, 1) # generate array to store elim data
  true_elims = matrix(NA, no_IUs * 200, 1) # generate array to store true elim data
  for(i in 1:no_IUs){
    
    population <- IUs$pop[IUs_vec[i]] # find population size for the IU
    
    # read in the specified file for given scenario and IU
    data_files <- read_file(scenario, coverage, 
                            non_compliance,
                            IU_order= IUs_vec[i], measure)
    
    
    if(length(data_files) > 1){
      # construct the cost for each simulation
      data_files$cost = data_files$post2020MDA * cost_scenario * population + cost_development + 
        data_files$No_Pre_TAS_surveys* preTAS_survey_cost + data_files$No_TAS_surveys* TAS_survey_cost
      
      # was there true elimination in each simulation
      data_files$true_elimination = 1*apply(data_files[, which_years], 1, true_elimination)
      # make proxy for dalys by year
      data_files[, which_years] = data_files[, which_years] * population
      if(length(rowMeans(data_files[, which_years], na.rm = T)) == 200){
        dalys[ seq(((i-1)*200 + 1),i*200), ] = rowMeans(data_files[, which_years], na.rm = T)
        costs[ seq(((i-1)*200 + 1),i*200), ] = data_files$cost
        elims[ seq(((i-1)*200 + 1),i*200), ] = unlist(lapply(data_files$t_TAS_pass, TAS_pass_test))
        true_elims[ seq(((i-1)*200 + 1),i*200), ] = data_files$true_elimination
      }
      
    }
    
  }
  
  mean_daly = matrix(0, 200, 1)
  mean_cost = matrix(0, 200, 1)
  mean_elim = matrix(0, 200, 1)
  mean_true_elim = matrix(0, 200, 1)
  # take the average for cost and dalys, for simulations 1, 2, 3,...
  for(i in 1:200){
    mean_daly[i] = mean(dalys[seq(i, length(dalys), 200)], na.rm = T)
    mean_cost[i] = mean(costs[seq(i, length(costs), 200)], na.rm = T)
    mean_elim[i] = mean(elims[seq(i, length(costs), 200)], na.rm = T)
    mean_true_elim[i] = mean(true_elims[seq(i, length(costs), 200)], na.rm = T)
  }
  return(list(dalys = mean_daly, costs = mean_cost, elims = mean_elim, true_elims = mean_true_elim))
  
}




#' plot_clouds
#'
#' @param all_dalys 
#' @param all_costs 
#' @param labels 
#'
#' @return
#' @export
#'
#' @examples
plot_clouds <- function(all_dalys, all_costs, labels, draw_slopes = F){
  plot(NA, NA, ylab = "Additional arbitrary costs until 2030", xlab = "Proxy for extra DALYs averted",
       xlim = c(1.05*min(unlist(all_dalys)), 1.05*max(unlist(all_dalys))),
       ylim = c(1.5*(min(unlist(all_costs))), # subtract cost of strategy 0 to get extra cost per scenario
                1.15*(max(unlist(all_costs)))),
       bty = 'n', cex.axis = 1.5, cex.lab = 1.5)
  
  cols = brewer.pal(n = length(all_dalys) , name = "Set1")
  cols <- c(hcl.colors(length(labels), palette = "ag_Sunset", alpha = 0.6))
  if(draw_slopes == T){
    abline(a= c(0,0), b= 0.1, lty = 2, col = 'grey')
    text(x=10000,y=1000,"$0.1 per daly averted", col = 'grey', cex = 1.6)
    abline(a= c(0,0), b= 0.5, lty = 2, col = 'grey')
    text(x=8000,y=4000,"$0.5 per daly averted", col = 'grey', cex = 1.6)
    abline(a= c(0,0), b= 1, lty = 2, col = 'grey')
    text(x=6000,y=6000,"$1 per daly averted",col = 'grey', cex = 1.6)
    abline(a= c(0,0), b= 2, lty = 2, col = 'grey')
    text(x=4000,y=8000,"$2 per daly averted", col = 'grey', cex = 1.6)
    abline(a= c(0,0), b= 4, lty = 2, col = 'grey')
    text(x=2500,y=10000,"$4 per daly averted", col = 'grey', cex = 1.6)
  }
  for(i in 1:length(all_dalys)){
    scen_cost = all_costs[[i]]
    scen_dalys = all_dalys[[i]]
    #  cc = paste0(cols[i], "99")
    for(j in 1:length(scen_cost)){
      points(scen_dalys[j], scen_cost[j], col = cols[i], pch = 16, cex = 2)
    }
  }
  legend("topleft", bty = "n", col = cols, legend = labels, lwd = 3, ncol = 2, pch = 16)
  
}




#' plot_blob_plot_from_all_daly_costs 
#'
#' @param all_dalys 
#' @param all_costs 
#' @param labels 
#'
#' @return
#' @export
#'
#' @examples
plot_blob_plot_from_all_daly_costs <- function(mean_dalys_diff, mean_cost_diff, 
                                               labels, draw_slopes = F, 
                                               additional_cost_dev = 0){
  plot(NA, NA, ylab = "Additional arbitrary costs until 2030", xlab = "Proxy for extra DALYs averted",
       xlim = c(1.05*min(mean_dalys_diff), 1.05*max(mean_dalys_diff)),
       ylim = c(-500, # subtract cost of strategy 0 to get extra cost per scenario
                1.15*(additional_cost_dev + max(mean_cost_diff))),
       bty = 'n', cex.axis = 1.5, cex.lab = 1.5)
  
  cols = brewer.pal(n = length(mean_dalys_diff) , name = "Set1")
  cols <- c(hcl.colors(length(labels), palette = "ag_Sunset", alpha = 0.6))
  if(draw_slopes == T){
    abline(a= c(0,0), b= 0.1, lty = 2, col = 'grey')
      text(x=10000,y=1000,"$0.1 per daly averted", col = 'grey', cex = 1.6)
    abline(a= c(0,0), b= 0.5, lty = 2, col = 'grey')
    text(x=8000,y=4000,"$0.5 per daly averted", col = 'grey', cex = 1.6)
    abline(a= c(0,0), b= 1, lty = 2, col = 'grey')
    text(x=6000,y=6000,"$1 per daly averted",col = 'grey', cex = 1.6)
    abline(a= c(0,0), b= 2, lty = 2, col = 'grey')
    text(x=4000,y=8000,"$2 per daly averted", col = 'grey', cex = 1.6)
    abline(a= c(0,0), b= 4, lty = 2, col = 'grey')
    text(x=2500,y=10000,"$4 per daly averted", col = 'grey', cex = 1.6)
  }
  for(i in 1:length(mean_dalys_diff)){
    scen_cost = mean_cost_diff[i]
    scen_dalys = mean_dalys_diff[i]
    #  cc = paste0(cols[i], "99")
    label = labels[i]
    if(label %in% c("3a", "3b", "3c", "3d")){
      scen_add_cost = additional_cost_dev
    }else{
      scen_add_cost = 0
    }
    #print(scen_cost + scen_add_cost)
    points(scen_dalys, scen_cost + scen_add_cost, col = cols[i], pch = 16, cex = 10)
    text(scen_dalys, scen_cost+ scen_add_cost, labels = label) 
  }
  legend("topleft", bty = "n", col = cols, legend = labels, lwd = 3, ncol = 2, pch = 16)
  
}





#' make_blob_plot_from_cloud_data
#'
#' @param mean_dalys_diff 
#' @param mean_cost_diff 
#' @param mean_elim_diff 
#' @param labels 
#' @param lambda_DALY 
#' @param lambda_EOT 
#' @param additional_cost_dev 
#' @param draw_slopes 
#'
#' @return
#' @export
#'
#' @examples
make_blob_plot_from_cloud_data <- function(mean_dalys_diff, mean_cost_diff, mean_elim_diff, 
                                           labels, lambda_DALY, lambda_EOT, additional_cost_dev, draw_slopes = F){
  

  mean_cost_diff <- mean_cost_diff+abs(min(mean_cost_diff)) # make all costs positive
  pal <- colorRamp(c("red", "blue")) # make a colour palette
 
  for(i in 1:length(mean_dalys_diff)){
    label = labels[i]
    if(label %in% c("3a", "3b", "3c", "3d")){
      mean_cost_diff[i] =  mean_cost_diff[i] + additional_cost_dev
    }
  }
  nmeb = 100 * lambda_EOT*(mean_elim_diff) + lambda_DALY*mean_dalys_diff - (mean_cost_diff) 
  if(any( nmeb < 0)){
    nmeb = nmeb - min(nmeb)
  }
  nmeb_col = nmeb/max(nmeb)
  col_vec <- pal(nmeb_col) # cex is relative to max mean cost
  cols = matrix(0, length(mean_dalys_diff))
  plot(NA, NA, ylab = "Additional arbitrary costs until 2030", xlab = "Proxy for extra DALYs averted",
       xlim = c(1.05*min(mean_dalys_diff), 1.05*max(mean_dalys_diff)),
       ylim = c(-500, # subtract cost of strategy 0 to get extra cost per scenario
                1.15*(max(mean_cost_diff))),
       bty = 'n', cex.axis = 1.5, cex.lab = 1.5,
       main = bquote( lambda[DALY] == .(lambda_DALY) ~ ", " ~ lambda[EOT] == .(lambda_EOT)~ ", development cost " == .(5000+ additional_cost_dev) ))
  points(-999999999, 1,
         pch = 16, cex = 1)
  if(draw_slopes == T){
    abline(a= c(0,0), b= 0.1, lty = 2, col = 'grey')
    text(x=10000,y=1000,"$0.1 per daly averted", col = 'grey', cex = 1.6)
    abline(a= c(0,0), b= 0.5, lty = 2, col = 'grey')
    text(x=8000,y=4000,"$0.5 per daly averted", col = 'grey', cex = 1.6)
    abline(a= c(0,0), b= 1, lty = 2, col = 'grey')
    text(x=6000,y=6000,"$1 per daly averted",col = 'grey', cex = 1.6)
    abline(a= c(0,0), b= 2, lty = 2, col = 'grey')
    text(x=4000,y=8000,"$2 per daly averted", col = 'grey', cex = 1.6)
    abline(a= c(0,0), b= 4, lty = 2, col = 'grey')
    text(x=2500,y=10000,"$4 per daly averted", col = 'grey', cex = 1.6)
  }
  for(i in 1:length(mean_dalys_diff)){
    scen_cost = mean_cost_diff[i]
    scen_dalys = mean_dalys_diff[i]
    #  cc = paste0(cols[i], "99")
    label = labels[i]
    # if(label %in% c("3a", "3b", "3c", "3d")){
    #   scen_add_cost = additional_cost_dev
    # }else{
    #   scen_add_cost = 0
    # }
    col_i = col_vec[i,]
    cols[i] = rgb(col_i[1]/255, col_i[2]/255, col_i[3]/255, 0.6)
    points(scen_dalys, scen_cost, col = rgb(col_i[1]/255, col_i[2]/255, col_i[3]/255, 0.6), pch = 16, cex = 10)
    text(scen_dalys, scen_cost, labels = label) 
  }
  legend("topleft", title = "rank of scenarios ", bty = "n", col = cols[order(nmeb, decreasing = T)], 
         legend = labels[order(nmeb, decreasing = T)], lwd = 3, pch = 16)
  return(nmeb)
}
