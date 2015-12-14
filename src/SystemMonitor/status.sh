#!/bin/sh

cpu_line=`vmstat 1 2 |tail -n 1`
idle=`echo "$cpu_line" | awk '{print $15}'`
cpu=$((100-$idle))
echo Cpu:$cpu

line_one=`/bin/cat /proc/meminfo | grep "MemTotal"`
line_two=`/bin/cat /proc/meminfo | grep "MemFree"`

one=`echo "$line_one" | awk '{print $2}'`
two=`echo "$line_two" | awk '{print $2}'`

div=$((($one-$two)*100/($one)))
echo Memory:$div

