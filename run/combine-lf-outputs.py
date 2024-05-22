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

m = re.match( r'.*/(?P<institute>I(HME|PM))_scen(?P<scenario>[0-9a-z]{1,2})/[^_]*_(?P<iu>[A-Z]{3}[0-9]{5})$', model_file_path )

if not m:
    usage()

matches = m.groupdict()
scenario = matches[ 'scenario' ]
iu = matches[ 'iu' ]
institute = matches[ 'institute' ]

print( f"-> processing {institute} outputs for scenario {scenario}, IU {iu}", file = sys.stderr )

institute_template = f"{model_file_path}/{institute}_scen{scenario}_{iu}_rep_%d.csv"
pre_tas_template = f"{model_file_path}/PreTAS_scen{scenario}_{iu}_rep_%d.csv"
tas_template = f"{model_file_path}/TAS_scen{scenario}_{iu}_rep_%d.csv"

file0 = institute_template % 0
try:
    main_df = pd.read_csv( file0 )

    for rep in range( 1, int( num_simulations ) ):
        next_file = institute_template % rep
        next_df = pd.read_csv( next_file )

        iloc_idx = {
            'IHME': 0,
            'IPM': 5
        }[ institute ]

        main_df[ f'draw_{rep}' ] = next_df.iloc[ :, iloc_idx ]
        main_df = main_df.copy()

    if ( institute == 'IHME' ):
        for survey_type in [ 'PreTAS', 'TAS' ]:
            survey_template = {
                'PreTAS': pre_tas_template,
                'TAS': tas_template
            }[ survey_type ]
            survey_file0 = survey_template % 0
            survey_df = pd.read_csv( survey_file0 )
            print( f"  -> adding {survey_type} outputs into IHME file for scenario {scenario}, IU {iu}", file = sys.stderr )
            for rep in range( 1, int( num_simulations ) ):
                next_file = survey_template % rep
                next_df = pd.read_csv( next_file )

                # it's going to be IHME isn't it
                iloc_idx = {
                    'IHME': 0,
                    'IPM': 5
                }[ institute ]

                survey_df[ f'draw_{rep}' ] = next_df.iloc[ :, iloc_idx ]
                survey_df = survey_df.copy()

            main_df = pd.concat( [ main_df, survey_df ] )

    main_df.to_csv( output_file_path, index = False )

except FileNotFoundError as f:
    print( f"xx> couldn't find file {file0}" )
    sys.exit( 1 )
