#!/bin/bash

stardate=$(expr `date -u +%Y%m%d_%H%M`)

awk -f ../Utilities/scale81Percent.wk T1_default.hv > T1_81_$stardate.hv
awk -f ../Utilities/scale81Percent.wk T2_default.hv > T2_81_$stardate.hv
awk -f ../Utilities/scale81Percent.wk T3_default.hv > T3_81_$stardate.hv
awk -f ../Utilities/scale81Percent.wk T4_default.hv > T4_81_$stardate.hv
