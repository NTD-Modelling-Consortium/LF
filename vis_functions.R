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


# extract a simulation instead of a median
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

# extract whether elimination occured or not 
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



calculate_number_infected_over_time <- function(d, population){
  c1 = which(colnames(d) == "Jan-2020")
  c2 = which(colnames(d) == "Jan-2030")
  prevs = d[, c1:c2]
  number_infected = colMeans(population * prevs)
  return(number_infected)
}

calculate_number_IUs_stopped_MDA <- function(d){
  years = (20:30)*12
  stop_time = d$t_TAS_pass
  prop_stopped = matrix(length(years), 1)
  for(i in 1:length(years)){
    prop_stopped[i] = length(which(stop_time <= years[i]))/length(stop_time)
  }
  
  return(prop_stopped)
}

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
  num_infections_over_time <- matrix(ncol = 11, nrow = no_IUs)
  prop_IUs_Finished_MDA <- matrix(ncol = 11, nrow = no_IUs)
  colnames(res) <- c("IU_order", "difference", "elim_prob_cf",  "elim_prob_scen", "costs")
  # cf_res <- matrix(ncol = 5, nrow = no_IUs)
  # colnames(cf_res) <- c("IU_order", "num_worms", "elim_prob", "num_MDAs", "costs")
  # scen_res <- matrix(ncol = 5, nrow = no_IUs)
  # colnames(scen_res) <- c("IU_order", "num_worms", "elim_prob", "num_MDAs",  "costs")
  # 
  for(i in 1:no_IUs){
    population <- IUs$pop[IUs_vec[i]] # choose some random population size for the IU
    
    data_files <- read_files_def_cf(scenario, coverage, cf_coverage, cf_non_compliance, non_compliance, 
                                    IU_order = IUs_vec[i], measure)
    
    data_files_elim <- read_files_def_cf(scenario, coverage,cf_coverage,cf_non_compliance,  non_compliance,
                                         IU_order = IUs_vec[i], elim)
    if(length(data_files$data_scenario) > 1 ){
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
      
      num_infections_over_time[i, ] = calculate_number_infected_over_time(data_files$data_scenario, population)
      prop_IUs_Finished_MDA[i, ] = calculate_number_IUs_stopped_MDA(data_files$data_scenario)
    }
    
    
  }
  x = which(!is.na(res[,1]))
  res = res[x, ]
  num_infections_over_time =  num_infections_over_time[x, ]
  prop_IUs_Finished_MDA = prop_IUs_Finished_MDA[x, ]
  return(list(res = res, num_infections_over_time = num_infections_over_time, prop_IUs_Finished_MDA = prop_IUs_Finished_MDA))
  
}

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


add_cloud <- function(res, label, col, subtract_cost, max_cost, res_0, cex_fixed){
  nsims <- dim(res)[1]
  
  #add cloud part for each simulation
  for(j in 1:nsims){
    # subtract the corresponding cost from scenario 0 for each simulation
    add_cloud_part(res[j,], labels[i], col, subtract_cost = res_0[j,"costs"], max_cost, cex_fixed)
  }
}

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



