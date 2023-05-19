#!/bin/bash

stardate=$(expr `date -u +%Y%m%d_%H%M`)

awk -f ../Utilities/halfBrightInner.wk T1_default.hv > HiLo/T1_innerHalfBright_$stardate.hv
awk -f ../Utilities/halfBrightInner.wk T2_default.hv > HiLo/T2_innerHalfBright_$stardate.hv
awk -f ../Utilities/halfBrightInner.wk T3_default.hv > HiLo/T3_innerHalfBright_$stardate.hv
awk -f ../Utilities/halfBrightInner.wk T4_default.hv > HiLo/T4_innerHalfBright_$stardate.hv
awk -f ../Utilities/halfBrightOuter.wk T1_default.hv > HiLo/T1_outerHalfBright_$stardate.hv
awk -f ../Utilities/halfBrightOuter.wk T2_default.hv > HiLo/T2_outerHalfBright_$stardate.hv
awk -f ../Utilities/halfBrightOuter.wk T3_default.hv > HiLo/T3_outerHalfBright_$stardate.hv
awk -f ../Utilities/halfBrightOuter.wk T4_default.hv > HiLo/T4_outerHalfBright_$stardate.hv
