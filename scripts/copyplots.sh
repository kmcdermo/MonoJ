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
    mkdir -p ~/www${dir}
fi


if [ "${move}" == true ] ; then
    echo "Moving directory: ${subdir} to ~/www/{dir}" 
    mv ${dir} ~/www/
else
    echo "Copying in new directory: ${subdir} to ~/www/${dir}" 
    cp -r ${subdir} ~/www/${dir}
fi

cd ~/www/
echo "Make the plots ready for da interwebz" 
./copyphp.sh
cd -

echo "Done"