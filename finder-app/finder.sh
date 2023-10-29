#!/bin/sh
# Finder script for assignment 1
# Author: Abel Balbis

if [ -z $1 ]
then
	echo "Frist parameter filedir not specified"
	exit 1

fi
if [ -z $2 ]
then
	echo "Second parameter searchstr not specified"
	exit 1
fi

filedir=$1
searchstr=$2

if [ ! -d $filedir ]
then
	
	echo "filesdir not found"
	exit 1
else 
	X=$(ls "$filedir"| wc -l)
fi


Y=$(grep -r "$searchstr" "$filedir"| wc -l)

echo "The number of files are $X and the number of matching lines are $Y"
exit 0
