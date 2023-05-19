#!/bin/bash

# This script is intended to create hv to be used with David Hanna's
# flatfielding procedure.
# MEMO: http://veritash.sao.arizona.edu:8081/AnalysisAndCalibration/3579
# WIKI: https://veritas.sao.arizona.edu/wiki/index.php/Flatfieldflasher
#
# FileNumber  Inner  Outer
# 1            95%    100%
# 2           100%    100%
# 3           105%    100%
# 4           100%     95%
# 5           100%    100%
# 6           100%    105%

awk -f ../Utilities/FlatFielding/halfDimInner.wk T1_default.hv > Flatfielding/t1_1.hv
awk -f ../Utilities/FlatFielding/halfDimInner.wk T2_default.hv > Flatfielding/t2_1.hv
awk -f ../Utilities/FlatFielding/halfDimInner.wk T3_default.hv > Flatfielding/t3_1.hv
awk -f ../Utilities/FlatFielding/halfDimInner.wk T4_default.hv > Flatfielding/t4_1.hv

cp -f T1_default.hv Flatfielding/t1_2.hv
cp -f T2_default.hv Flatfielding/t2_2.hv
cp -f T3_default.hv Flatfielding/t3_2.hv
cp -f T4_default.hv Flatfielding/t4_2.hv

awk -f ../Utilities/FlatFielding/halfBrightInner.wk T1_default.hv > Flatfielding/t1_3.hv
awk -f ../Utilities/FlatFielding/halfBrightInner.wk T2_default.hv > Flatfielding/t2_3.hv
awk -f ../Utilities/FlatFielding/halfBrightInner.wk T3_default.hv > Flatfielding/t3_3.hv
awk -f ../Utilities/FlatFielding/halfBrightInner.wk T4_default.hv > Flatfielding/t4_3.hv

awk -f ../Utilities/FlatFielding/halfDimOuter.wk T1_default.hv > Flatfielding/t1_4.hv
awk -f ../Utilities/FlatFielding/halfDimOuter.wk T2_default.hv > Flatfielding/t2_4.hv
awk -f ../Utilities/FlatFielding/halfDimOuter.wk T3_default.hv > Flatfielding/t3_4.hv
awk -f ../Utilities/FlatFielding/halfDimOuter.wk T4_default.hv > Flatfielding/t4_4.hv

cp -f T1_default.hv Flatfielding/t1_5.hv
cp -f T2_default.hv Flatfielding/t2_5.hv
cp -f T3_default.hv Flatfielding/t3_5.hv
cp -f T4_default.hv Flatfielding/t4_5.hv

awk -f ../Utilities/FlatFielding/halfBrightOuter.wk T1_default.hv > Flatfielding/t1_6.hv
awk -f ../Utilities/FlatFielding/halfBrightOuter.wk T2_default.hv > Flatfielding/t2_6.hv
awk -f ../Utilities/FlatFielding/halfBrightOuter.wk T3_default.hv > Flatfielding/t3_6.hv
awk -f ../Utilities/FlatFielding/halfBrightOuter.wk T4_default.hv > Flatfielding/t4_6.hv

awk -f ../Utilities/FlatFielding/halfDimInner.wk T1_ReducedHV_default.hv > Flatfielding/t1_reducedhv_1.hv
awk -f ../Utilities/FlatFielding/halfDimInner.wk T2_ReducedHV_default.hv > Flatfielding/t2_reducedhv_1.hv
awk -f ../Utilities/FlatFielding/halfDimInner.wk T3_ReducedHV_default.hv > Flatfielding/t3_reducedhv_1.hv
awk -f ../Utilities/FlatFielding/halfDimInner.wk T4_ReducedHV_default.hv > Flatfielding/t4_reducedhv_1.hv

cp -f T1_ReducedHV_default.hv Flatfielding/t1_reducedhv_2.hv
cp -f T2_ReducedHV_default.hv Flatfielding/t2_reducedhv_2.hv
cp -f T3_ReducedHV_default.hv Flatfielding/t3_reducedhv_2.hv
cp -f T4_ReducedHV_default.hv Flatfielding/t4_reducedhv_2.hv

awk -f ../Utilities/FlatFielding/halfBrightInner.wk T1_ReducedHV_default.hv > Flatfielding/t1_reducedhv_3.hv
awk -f ../Utilities/FlatFielding/halfBrightInner.wk T2_ReducedHV_default.hv > Flatfielding/t2_reducedhv_3.hv
awk -f ../Utilities/FlatFielding/halfBrightInner.wk T3_ReducedHV_default.hv > Flatfielding/t3_reducedhv_3.hv
awk -f ../Utilities/FlatFielding/halfBrightInner.wk T4_ReducedHV_default.hv > Flatfielding/t4_reducedhv_3.hv

awk -f ../Utilities/FlatFielding/halfDimOuter.wk T1_ReducedHV_default.hv > Flatfielding/t1_reducedhv_4.hv
awk -f ../Utilities/FlatFielding/halfDimOuter.wk T2_ReducedHV_default.hv > Flatfielding/t2_reducedhv_4.hv
awk -f ../Utilities/FlatFielding/halfDimOuter.wk T3_ReducedHV_default.hv > Flatfielding/t3_reducedhv_4.hv
awk -f ../Utilities/FlatFielding/halfDimOuter.wk T4_ReducedHV_default.hv > Flatfielding/t4_reducedhv_4.hv

cp -f T1_ReducedHV_default.hv Flatfielding/t1_reducedhv_5.hv
cp -f T2_ReducedHV_default.hv Flatfielding/t2_reducedhv_5.hv
cp -f T3_ReducedHV_default.hv Flatfielding/t3_reducedhv_5.hv
cp -f T4_ReducedHV_default.hv Flatfielding/t4_reducedhv_5.hv

awk -f ../Utilities/FlatFielding/halfBrightOuter.wk T1_ReducedHV_default.hv > Flatfielding/t1_reducedhv_6.hv
awk -f ../Utilities/FlatFielding/halfBrightOuter.wk T2_ReducedHV_default.hv > Flatfielding/t2_reducedhv_6.hv
awk -f ../Utilities/FlatFielding/halfBrightOuter.wk T3_ReducedHV_default.hv > Flatfielding/t3_reducedhv_6.hv
awk -f ../Utilities/FlatFielding/halfBrightOuter.wk T4_ReducedHV_default.hv > Flatfielding/t4_reducedhv_6.hv
