

# Worm variable:
In the Worm.cpp file, there are functions which define the efficacy of drug regimens. This is important for testing the impact of improved drugs. These functions are briefly described below.

### wormsTreated
This function outputs surviving worms based on a treatment round. This is dependent on the drug used, defined by "type" variable. For example "da" is drug regimen diethylcarbamazine and albendazole. ``` propWormsKilled = (wPropMDA >=0)?wPropMDA:0.55; ``` defines that 55% of worms are killed for this drug regimen. To increase effectivenes to 75% then this line would read ``` propWormsKilled = (wPropMDA >=0)?wPropMDA:0.75; ```
### mfTreated
Proportion of microfilariae killed by treatment. mfPropMDA variable defines the proportion of mf that will be left in an individual after treatment. For the da drug regimen, ```mfPropMDA = 1-1;``` means that 100% of mf will be removed from individual. To change this to 90%, we would have ```mfPropMDA = 1-0.9;``` or just ```mfPropMDA = 0.1;```  

