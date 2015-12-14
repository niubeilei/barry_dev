#!/bin/sh
#This is the running script for AOS system

#set vars firstly
user_name=`whoami`
htmlsvr_path="~${user_name}/AOS/src/HtmlServer/Exe/htmlserver.exe"

cd $(dirname "$0")
work_dir=`echo $(cd "$(dirname "$0")"; pwd)`
test ! -z "$2" && work_dir=$2
test ! -d $work_dir && echo "ERROR: work directoy not exist ($work_dir)" && exit


# functions
# shoot target ammo
shoot () {
for q in `ps -u ${user_name} | sed -n "/$1/p" | awk '{print $1}'`
do
	echo "|||||||||||||shooting: $1"
	kill $2 $q > /dev/null 2>&1
done
return 0
}

start_daemons() {
	echo
	echo '******************* start daemons ... **************************'
	echo

	#start the processes one by one
	${work_dir}/SvrProxy.exe -config ${work_dir}/config_proxy.txt &


	#start access server when frontServer started
	sleep 10s
	check_times=1
	while test $check_times -lt 20
	do
		flag=`ps -u ${user_name}|sed -n "/frontend.exe/p"|wc -l`
		if test $flag -eq 0
		then 
			echo "frontend.exe	no"
		else
			echo "frontend.exe	yes"
			sleep 5s
			${work_dir}/access.exe -config config_access.txt	
			break
		fi
		check_times=`expr $check_times + 1`
		sleep 10s
	done
}

stop_daemons() {
	echo 
	echo '******************* stop daemons ... **************************'
	echo

	#send alarm to all the relative deamons in the reverse order of starting
	shoot SvrProxy.exe -ALRM
}



# 1. check htmlServer whether start. if no start, start it.
count=`ps -u ${user_name}|sed -n '/htmlserver.exe/p'|wc -l`
if test $count -eq 0
then
	if test ! -e ${htmlsvr_path} 
	then
		echo 
		#echo "no htmlserver.exe,path: ${htmlsvr_path}"
	else
		${htmlsvr_path}
	fi
elif test $count -eq 1
then
	echo
else
	echo "ERROR: htmlserver process more than 1, please check it."
	exit
fi

#link with shared libs
shared_lib_dir=${work_dir}/shared_libs
export LD_LIBRARY_PATH=${shared_lib_dir}:$LD_LIBRARY_PATH

# 2. auto start/kill servers
#	 take actions based on different user input
case "$1" in
	stop)
		stop_daemons

		;;
	start)
		start_daemons

		;;
	restart)
		#stop
		stop_daemons
		sleep 10s

		#start
		start_daemons

		;;
	cleanstart)
		#stop 
		stop_daemons
		sleep 10s

		#clean data
		rm -rf ${work_dir}/../../DataSvr0
		rm -rf ${work_dir}/../../ProxyTmp0

		#start 
		start_daemons

		;;
	*)
		echo "Usage : $0 {start|stop|restart|cleanstart}"
		exit 1
esac


