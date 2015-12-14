#! /bin/sh



pid=`ps ax -o comm,pid |grep $1|awk '{print $2}'`

#if [ "$pid" == "" ]; then
#	exit 0;
#fi

echo Pid: $pid
printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' "CPU" "MEM" "RES(M)" "VIRT(M)" "R/S" "W/S" "R_MB/S" "W_MB/S" "R_MB/S" "S_MB/S"

while true
do
	if [ "$pid" != "" ]; then
		status=`ps -o %cpu,%mem,rss,vsz -p $pid --no-headers`
		cpu=`echo $status | awk '{print $1}'`
		mem=`echo $status | awk '{print $2}'`
		res=`echo $status | awk '{print $3}'`
		res=$((($res)/1024))
		virt=`echo $status | awk '{print $4}'`
		virt=$((($virt)/1024))
		iostat=`dstat -rdn 10 1|tail -n 1`
		rw=`echo $iostat | cut -d '|' -f 1`
		r=`echo $rw | awk '{print $1}'`
		w=`echo $rw | awk '{print $2}'`
		rwb=`echo $iostat | cut -d '|' -f 2`
		rb=`echo $rwb | awk '{total=0;if(match($1, "M")) total = $1*1024;if(match($1, "k")) total=$1; printf("%5.2f\n", total/1024)}'`
		wb=`echo $rwb | awk '{total=0;if(match($2, "M")) total = $2*1024;if(match($2, "k")) total=$2; printf("%5.2f\n", total/1024)}'`
		nb=`echo $iostat | cut -d '|' -f 3`
		nib=`echo $nb | awk '{total=0;if(match($1, "M")) total = $1*1024;if(match($1, "k")) total=$1; printf("%5.2f\n", total/1024)}'`
		nob=`echo $nb | awk '{total=0;if(match($2, "M")) total = $2*1024;if(match($2, "k")) total=$2; printf("%5.2f\n", total/1024)}'`
		printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n' $cpu $mem $res $virt $r $w $rb $wb $nib $nob
	fi
done
