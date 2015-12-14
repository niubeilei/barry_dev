#!/bin/bash

if [ a$LINUXHOME == a ] || ! [ -d $LINUXHOME ] 
then
	echo "Error, please export LINUXHOME"
	exit 1
fi

if [ a$AOSHOME == a ] || ! [ -d $AOSHOME ] 
then
	echo "Error, please export AOSHOME"
	exit 1
fi


echo "begin ..."

TARGET=~/tmp
INSTALL="install -D"
/bin/rm -rf $TARGET/AOS
mkdir -p $TARGET/AOS


#compile aos
mkdir -p $AOSHOME/{lib,obj}
make -C $AOSHOME/src
make -C $AOSHOME/src

#install aos source
make -C $AOSHOME/src/Install installMystro2_6_15_5


#compile kernel
make -C $LINUXHOME bzImage
#make -C $LINUXHOME modules
#INSTALL_MOD_PATH=$TARGET/AOS  make -C $LINUXHOME modules_install

RELEASE=`$AOSHOME/src/KernelInterface_exe/Cli.exe -r`

#copy files
$INSTALL $AOSHOME/src/KernelInterface_exe/Cli.exe              $TARGET/AOS/Bin/Cli.exe
$INSTALL $AOSHOME/src/Ktcpvs/userspace/lookup/server/dnslookup $TARGET/AOS/Bin/dnslookup
$INSTALL $LINUXHOME/arch/i386/boot/bzImage                     $TARGET/AOS/Kernel/vmlinuz-sslproxy-$RELEASE
$INSTALL $AOSHOME/Data/cmd.txt                                 $TARGET/AOS/Bin/cmd.txt
$INSTALL $AOSHOME/src/Install/install-aos.sh                   $TARGET/AOS/Bin/install-aos.sh

#make tgz
tar czf $TARGET/sslproxy-$RELEASE.tar.gz -C $TARGET AOS
cp -f $AOSHOME/src/Install/install-aos.sh $TARGET


echo '#!/bin/bash' > $TARGET/install-sslproxy-$RELEASE.bin
LINES=`cat $TARGET/install-aos.sh|wc -l`
LINES=`expr 3 + $LINES`
echo "LINES=$LINES" >>$TARGET/install-sslproxy-$RELEASE.bin
cat $TARGET/install-aos.sh >> $TARGET/install-sslproxy-$RELEASE.bin
cat $TARGET/sslproxy-$RELEASE.tar.gz >>$TARGET/install-sslproxy-$RELEASE.bin
chmod 755 $TARGET/install-sslproxy-$RELEASE.bin

rm -f $TARGET/sslproxy-$RELEASE.tar.gz
rm -f $TARGET/install-aos.sh

echo "=================================================================="
echo "SUCCESS! The install file is $TARGET/install-sslproxy-$RELEASE.bin"
echo "To install, please run: sudo $TARGET/install-sslproxy-$RELEASE.bin"
echo "=================================================================="

