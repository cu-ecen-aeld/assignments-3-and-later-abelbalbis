#!/bin/sh
# Writer script for assignment 1
# Author: Abel Balbis

if [ -z $1 ]
then
	echo "Frist parameter 'writefile' not specified"
	exit 1

fi
if [ -z $2 ]
then
	echo "Second parameter 'writestr' not specified"
	exit 1
fi

writefile=$1
writestr=$2

dir_writefile=$(dirname $1writefile)


if [ ! -e writefile ]
then
	mkdir -p "$dir_writefile"
	touch "$writefile"

	if [ ! $? -eq 0 ]
	then
		echo "$writefile cannot be created"
		exit 1
	fi
fi


echo $writestr >> $writefile


exit 0