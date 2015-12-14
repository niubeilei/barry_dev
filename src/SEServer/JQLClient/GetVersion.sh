#!/bin/sh
#clone version file to local
cp -rf ../../etc/version.h .
tag=tag;
ver=version.h;

#get tag
#1. formal build will have a tag file
#2. local build will use build time
if [ -e $tag ];
then 
	token=`head -n 1 $tag`;
else 
	token=`date`;
fi

#translate the file
sed -i "s/AOS_JIMO_VERSION *\".*\"/AOS_JIMO_VERSION \"${token}\"/g" $ver;

