#! /bin/sh

dir=$1
subdir=$2
move=$3

# delete old directory if it exists
#cd ~/www/
#if [ -d ${dir} ] ; then
#    echo "Deleting old directory: ${dir}" 
#    rm -rf ${dir}
#fi
#cd -

if [ ! -d ~/www/${dir} ] ; then
    echo "making directory ~/www/${dir}"
    mkdir -p ~/www/${dir}
fi

if [ ! -d ~/www/${subdir} ] ; then
    echo "making subdirectory ~/www/${subdir}"
    mkdir -p ~/www/${subdir}
#else
#    echo "removing then making directory ~/www/${subdir}"
#    rm -rf ~/www/${subdir}
#    mkdir -p ~/www/${subdir}
fi

if [ "${move}" == true ] ; then
    echo "Moving PURW plots"
    mv ${dir}/nvtx_beforePURW* ~/www/${dir}
    mv ${dir}/nvtx_afterPURW* ~/www/${dir}
    echo "Moving directory: ${subdir} to ~/www/{dir}" 
    mv ${subdir} ~/www/${subdir}
else
    echo "Copying PURW plots"
    cp ${dir}/nvtx_beforePURW* ~/www/${dir}
    cp ${dir}/nvtx_afterPURW* ~/www/${dir}
    echo "Copying in new directory: ${subdir} to ~/www/${dir}" 
    cp -r ${subdir} ~/www/${dir}
fi

cd ~/www/
echo "Make the plots ready for da interwebz" 
./copyphp.sh
cd -

echo "Done"