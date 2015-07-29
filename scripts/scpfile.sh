#! /bin/bash

sample=$1

echo "Checking for sample: " ${sample}

cd /home/avartak/CMS/MonoX/Trees/Spring15MC_50ns/${sample}

if [ -f treewithwgt.root ] ; then
    echo "Copying treewithwgt.root to /afs/cern.ch/user/k/kmcdermo/monojet/CMSSW_7_4_5/src/AnalysisCode/MonoXAnalysis/AnalysisStep/macros/kevin"
    scp treewithwgt.root kmcdermo@lxplus.cern.ch:~/monojet/CMSSW_7_4_5/src/AnalysisCode/MonoXAnalysis/AnalysisStep/macros/kevin/
    echo "Finished copying"
else
    echo "Copying tree.root to /afs/cern.ch/user/k/kmcdermo/monojet/CMSSW_7_4_5/src/AnalysisCode/MonoXAnalysis/AnalysisStep/macros/kevin"
    scp tree.root kmcdermo@lxplus.cern.ch:~/monojet/CMSSW_7_4_5/src/AnalysisCode/MonoXAnalysis/AnalysisStep/macros/kevin/
    echo "Finished copying "
fi

