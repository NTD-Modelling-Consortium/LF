This is an individual based model of lymphatic filariasis (LF) in a human population. To model LF we keep track of the number of worms within human individuals. Worms produce microfilariae (mf), which can then infect mosquitoes in the environment when they feed on humans. Within the mosquito these microfilariae develop through multiple stages until they become L3 larvae, at which point they can infect humans when feeding. The density of these L3 larvae is tracked within the mosquito population.

This simulation requires a .txt file which gives the needed parameters for the model, along with a .xml file which gives the intervention scenario for the simulation. Multiple scenarios can be specified in the .xml file. The initially specified scenario will be run from time 0. Subsequent simulations can be run from time 0, or from a saved point from the original simulation.

A common use of this code may be to investigate the impact of different drug regimens on the spread of LF. To achieve this, we need to edit the Worm.cpp file, which contains infomation related to the effectiveness of drug regimens. We also need to create a .xml file which performs the interventions as we wish them to be done.

# Worm variable:
In the Worm.cpp file, there are functions which define the efficacy of drug regimens. This is important for testing the impact of improved drugs. These functions are briefly described below.

### wormsTreated
This function outputs surviving worms based on a treatment round. This is dependent on the drug used, defined by "type" variable. For example "da" is drug regimen diethylcarbamazine and albendazole. ``` propWormsKilled = (wPropMDA >=0)?wPropMDA:0.55; ``` defines that 55% of worms are killed for this drug regimen. To increase effectivenes to 75% then this line would read ``` propWormsKilled = (wPropMDA >=0)?wPropMDA:0.75; ```
### mfTreated
Proportion of microfilariae killed by treatment. mfPropMDA variable defines the proportion of mf that will be left in an individual after treatment. For the da drug regimen, ```mfPropMDA = 1-1;``` means that 100% of mf will be removed from individual. To change this to 90%, we would have ```mfPropMDA = 1-0.9;``` or just ```mfPropMDA = 0.1;```  

