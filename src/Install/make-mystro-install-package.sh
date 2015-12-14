#!/bin/bash

MYSTROHOME=../..
if [ a$LINUXHOME == a ] || ! [ -d $LINUXHOME ] 
then
	echo "Error, please export LINUXHOME"
	exit 1
fi

echo "begin ..."

PRODUCT=mystro
TARGET=~/tmp
INSTALL="install -D"
INSTALLSCRIPT=install-$PRODUCT.sh
/bin/rm -rf $TARGET/AOS
mkdir -p $TARGET/AOS/Config
mkdir -p $TARGET/AOS/Bin


#compile aos
mkdir -p $MYSTROHOME/{lib,obj}
make -C $MYSTROHOME/src
make -C $MYSTROHOME/src
make -C $MYSTROHOME/src/CliDeamon
make -C $MYSTROHOME/src/tools
make -C $MYSTROHOME/src/Ktcpvs/userspace/lookup/server/

#install aos source
make -C $MYSTROHOME/src/Install installMystro


#compile kernel
make -C $LINUXHOME bzImage
#make -C $LINUXHOME modules
#INSTALL_MOD_PATH=$TARGET/AOS  make -C $LINUXHOME modules_install

RELEASE=`$MYSTROHOME/src/KernelInterface_exe/Cli.exe -r`

#copy files
$INSTALL $MYSTROHOME/src/KernelInterface_exe/Cli.exe              $TARGET/AOS/Bin/Cli.exe
$INSTALL $MYSTROHOME/src/CliDeamon/CliDeamon.exe              $TARGET/AOS/Bin/CliDeamon.exe
strip $TARGET/AOS/Bin/Cli.exe
strip $TARGET/AOS/Bin/CliDeamon.exe
$INSTALL $MYSTROHOME/src/Ktcpvs/userspace/lookup/server/dnslookup $TARGET/AOS/Bin/dnslookup
$INSTALL $MYSTROHOME/src/QoS/qos_stat							   $TARGET/AOS/Bin/qos_stat
$INSTALL $LINUXHOME/arch/i386/boot/bzImage                     $TARGET/AOS/Kernel/vmlinuz-$PRODUCT-$RELEASE
$INSTALL $MYSTROHOME/Data/cmd.txt                                 $TARGET/AOS/Bin/cmd.txt
$INSTALL $MYSTROHOME/src/Install/$INSTALLSCRIPT                   $TARGET/AOS/Bin/$INSTALLSCRIPT
$INSTALL $MYSTROHOME/src/tools/mystrorun                 		   $TARGET/AOS/Bin/mystrorun
$INSTALL $MYSTROHOME/src/tools/system_update.pl                   $TARGET/AOS/Bin/system_update.pl

#make tgz
tar czf $TARGET/$PRODUCT-$RELEASE.tar.gz -C $TARGET AOS
cp -f $MYSTROHOME/src/Install/$INSTALLSCRIPT $TARGET


echo '#!/bin/bash' > $TARGET/install-$PRODUCT-$RELEASE.bin
LINES=`cat $TARGET/$INSTALLSCRIPT|wc -l`
LINES=`expr 3 + $LINES`
echo "LINES=$LINES" >>$TARGET/install-$PRODUCT-$RELEASE.bin
cat $TARGET/$INSTALLSCRIPT >> $TARGET/install-$PRODUCT-$RELEASE.bin
cat $TARGET/$PRODUCT-$RELEASE.tar.gz >>$TARGET/install-$PRODUCT-$RELEASE.bin
chmod 755 $TARGET/install-$PRODUCT-$RELEASE.bin

rm -f $TARGET/$PRODUCT-$RELEASE.tar.gz
rm -f $TARGET/$INSTALLSCRIPT

echo "=================================================================="
echo "SUCCESS! The install file is $TARGET/install-$PRODUCT-$RELEASE.bin"
echo "To install, please run: sudo $TARGET/install-$PRODUCT-$RELEASE.bin"
echo "=================================================================="

