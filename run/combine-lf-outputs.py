#!/usr/bin/env python
import pandas as pd
import sys
import re

def usage():
	print( f"usage: python {sys.argv[0]} <num-simulations> <model-file-path> <output-file-path>" )
	sys.exit( 1 )

if len( sys.argv ) != 4:
	usage()

num_simulations = sys.argv[ 1 ]
model_file_path = sys.argv[ 2 ]
output_file_path = sys.argv[ 3 ]

m = re.match( r'.*res_endgame/(?P<institute>I(HME|PM))_scen(?P<scenario>[123abc]{,2})/[^_]*_(?P<iu>[A-Z]{3}[0-9]{5})$', model_file_path )

if not m:
	usage()

matches = m.groupdict()
scenario = matches[ 'scenario' ]
iu = matches[ 'iu' ]
institute = matches[ 'institute' ]

print( f"-> processing {institute} outputs for scenario {scenario}, IU {iu}", file = sys.stderr )

path_template = f"{model_file_path}/{institute}_scen{scenario}_{iu}_rep_%d.csv"

file0 = path_template % 0
main_df = pd.read_csv( file0 )

for rep in range( 1, int( num_simulations ) ):
	next_file = path_template % rep
	next_df = pd.read_csv( next_file )

	iloc_idx = {
		'IHME': 0,
		'IPM': 5
	}[ institute ]

	main_df[ f'draw_{rep}' ] = next_df.iloc[ :, iloc_idx ]
	main_df = main_df.copy()

main_df.to_csv( output_file_path, index = False )
