#! /bin/sh

source /afs/cern.ch/project/eos/installation/cms/etc/setup.sh

#for sample in wln #zll ttbar ww wz zz singlett singletbart singletw singletbarw qcd15to30 qcd30to50 qcd50to80 qcd80to120 qcd120to170 qcd170to300 qcd300to470 qcd470to600 qcd600to800 qcd800to1000 qcd1000to1400 qcd1400to1800 qcd1800to2400 qcd2400to3200 qcd3200toinf
for sample in wln ww wz zz singlett singletbart singletw singletbarw qcd15to30 qcd30to50 qcd50to80 qcd80to120 qcd120to170 qcd170to300 qcd300to470 qcd470to600 qcd600to800 qcd800to1000 qcd1000to1400 qcd1400to1800 qcd1800to2400 qcd2400to3200 qcd3200toinf
do
    
    ./launchMCssh.sh ${sample}

#    if [ -f ../treewithwgt.root ] ; then
#    eos rm -r eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/${sample}
    eos mkdir eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/${sample}
    
    echo "xrdcp tree.root to: eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/${sample}" 
    
    xrdcp ../treewithwgt.root root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/${sample}
    rm ../treewithwgt.root
    
    echo "Finished xrdcp, deleted treewithwght.root locally"
 #   else
#	eos rm -r eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/${sample}
#	eos mkdir eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/${sample}
	
#	echo "xrdcp tree.root to: eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/${sample}" 

#	xrdcp ../tree.root root://eoscms//eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/${sample}

#	echo "converting tree.root to treewithwgt.root in eos space"

#	root -l -b -q "../macros/addwgtsum.C("\"${sample}\"")"
#	rm ../tree.root
#	xrd eoscms rm eos/cms/store/user/kmcdermo/MonoJ/Trees/Spring15MC_50ns/${sample}/tree.root

#	echo "Finished conversion, deleted treewithwght.root locally and on eos space"
  #  fi


done