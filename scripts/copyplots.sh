#! /bin/sh

dir=$1
move=$2

# delete old directory if it exists

cd ~/www/
if [ -d ${dir} ] ; then
    echo "Deleting old directory: ${dir}" 
    rm -rf ${dir}
fi
cd -

if [ "${move}" == true ] ; then
    echo "Moving directory: ${dir} to ~/www/" 
    mv ${dir} ~/www/
else
    echo "Copying in new directory: ${dir} to ~/www/" 
    cp -r ${dir} ~/www/
fi

cd ~/www/
echo "Make the plots ready for da interwebz" 
./copyphp.sh
cd -

echo "Done"