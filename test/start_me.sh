#!/bin/sh


go_testSiStripPedestals_write(){
echo -e "\n&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
echo "Start write file test_SiStripPedestals.db "
echo -e "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n"

$CMSSW_BASE/test/slc3_ia32_gcc323/testSiStripPedestals | tee out_testSiStripPedestals_write

echo -e "\n&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n"
}

go_testSiStripPedestals_read(){
echo -e "\n&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
echo "Start read file test_SiStripPedestals.db "
echo -e "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n"

$CMSSW_BASE/test/slc3_ia32_gcc323/testSiStripPedestals TrackerCalibration | tee out_testSiStripPedestals_read

echo -e "\n&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n"
}

go_SiStripCalibAnalyser(){
echo -e "\n&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
echo "Start read file test_SiStripPedestals.db with the Analyser"
echo -e "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n"

cmsRun --parameter-set print_ped.cfg

echo -e "\n&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n"
}


eval `scramv1 runtime -sh`

[ "$#" == 0 ] || [ "$1" == "1" ]  && go_testSiStripPedestals_write
[ "$1" == "2" ] && go_testSiStripPedestals_read
[ "$1" == "3" ] && go_SiStripCalibAnalyser




