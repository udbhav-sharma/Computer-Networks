# !/bin/bash

awk 'BEGIN { if(10000 > '$1') printf "greater\n" }'

exit

find -regex [.][//][A-Za-z0-9]+ > tmp_file

while read filename
do
	#echo $filename
	if [ -f $filename ];
	then
		mv -t 2014-2-12/Q5 $filename
	fi
done<tmp_file
