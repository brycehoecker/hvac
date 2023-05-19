#!/bin/bash

# Shell script for flatfielding HV settings based on a new set of gain values.
# Assumes gains are produced by evndisp and are in a text file with three
# columns:
# 		pixel
#		gain
#		gain rms
#
# Also needs the current HV settings, which should be in 
# /usr/local/veritas/SP05/vhv/HVFiles/T<telID>_default.hv 
# which contains two columns:
#		pixel
#		voltage
#
# New voltage values are only calculated for a pixel if the gain is between
# 0.2 and 5.
#
# Command-line arguments are all required and are:
# $1 filename of gain file
# $2 Telescope ID number, 1-4
# $3 Name of current HV file to use - usually will be TX_default.hv

stardate=$(expr `date -u +%Y%m%d_%H%M`)
paste $1 $3 | awk -f flat.wk > /usr/local/veritas/SP05/hvac/HVFiles/T$2_flatfield_$stardate.hv
#paste $1 /usr/local/veritas/SP05/hvac/HVFiles/T$2_default.hv | awk -f flat.wk > /usr/local/veritas/SP05/hvac/HVFiles/T$2_flatfield_$stardate.hv
#
# flat.wk does the real work.
