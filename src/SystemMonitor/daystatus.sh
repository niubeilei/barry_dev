#!/bin/sh


dir=.
#space_line=`du -s | cut -d '	' -f 1`
#space_line=`find $dir -type f |xargs wc -c |tail -n 1 | cut -d ' ' -f 1`
#echo Total:$space_line
space_line1=`find $dir -name 'iil*' |xargs wc -c |tail -n 1 | cut -d ' ' -f 1`
echo Index:$space_line1
space_line2=`find $dir -name 'doc*' |xargs wc -c |tail -n 1 | cut -d ' ' -f 1`
echo Data:$space_line2

totalnum=0
totaluse=0
df | grep '/dev/' | while read line
do
    total=`echo $line | awk '{print $2}'`
    use=`echo $line | awk '{print $3}'`
    totalnum=$(($total+$totalnum))
    totaluse=$(($use+$totaluse))
    echo $totalnum>temp1.txt
    echo $totaluse>temp2.txt
done

read totalnum <temp1.txt
read totaluse <temp2.txt
echo Total:$totalnum
echo Used:$totaluse
