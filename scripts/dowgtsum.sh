#!/bin/sh

for sample in wln wz ww zz singletw singlett singletbart singletbarw ttbar qcd1000to1500 qcd100to200 qcd1500to2000 qcd2000toinfqcd200to300 qcd300to500 qcd500to700 qcd700to1000
do
    echo "Doing sample: " ${sample}
    root -l -b -q "addwgtsum.C("\"${sample}\"")"
    echo "" 
done