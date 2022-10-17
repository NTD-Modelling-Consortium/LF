

# Worm variable:
### wormsTreated
This function outputs surviving worms based on a treatment round. This is dependent on the drug used, defined by "type" variable. For example "da" is drug diethylcarbamazine and albendazole. ``` propWormsKilled = (wPropMDA >=0)?wPropMDA:0.55; ``` defines that 55% of worms are killed.
### mfTreated
Proportion of mf killed by treatment. mfPropMDA variable defines the proportion of mf that will be left in an individual after treatment. For da drug regimen, ```mfPropMDA = 1-1;``` means that 100% of mf will be removed from individual. 
