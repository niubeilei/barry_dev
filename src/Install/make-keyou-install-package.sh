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

PACKAGE_NAME=keyou_tmp
Fi_PACKAGE_NAME=keyou
TARGET=~/tmp
INSTALL="install -D"
/bin/rm -rf $TARGET/$PACKAGE_NAME
mkdir -p $TARGET/$PACKAGE_NAME


#compile aos
make -C $AOSHOME/src/KernelInterface clean
make -C $AOSHOME/src/KernelInterface_exe clean
make -C $AOSHOME/src
make -C $AOSHOME/src
make -C $AOSHOME/src/am
make -C $AOSHOME/src/HacDaemon
make -C $AOSHOME/src/GUI/src -f Makefile.HAC

#install aos source
make -C $AOSHOME/src/Install installKeyou


#compile kernel
make -C $LINUXHOME bzImage
#make -C $LINUXHOME modules

RELEASE=`$AOSHOME/src/KernelInterface_exe/Cli.exe -r`

mkdir -p $TARGET/$PACKAGE_NAME/{Config,Log,Bin,Kernel}
cp -f $AOSHOME/src/am/am                $TARGET/$PACKAGE_NAME/Bin/
cp -f $AOSHOME/src/HacDaemon/daemon                $TARGET/$PACKAGE_NAME/Bin/
cp -f $AOSHOME/src/HacDaemon/daemon.conf                $TARGET/$PACKAGE_NAME/Bin/
cp -f $AOSHOME/src/am/mkcurt                $TARGET/$PACKAGE_NAME/Bin/
cp -f $AOSHOME/src/am/for_mkt                $TARGET/$PACKAGE_NAME/Bin/
cp -f $AOSHOME/src/am/am.conf	$TARGET/$PACKAGE_NAME/Bin/
cp -f $AOSHOME/src/Redundance/hac	$TARGET/$PACKAGE_NAME/Bin/
cp -f $AOSHOME/src/Redundance/messcmd	$TARGET/$PACKAGE_NAME/Bin/
cp -f $AOSHOME/src/Redundance/hacstart.sh	$TARGET/$PACKAGE_NAME/Bin/
cp -f $AOSHOME/src/Redundance/syncfile.sh	$TARGET/$PACKAGE_NAME/Bin/
cp -f  $AOSHOME/src/am/hac.db	$TARGET/$PACKAGE_NAME/Config/
cp -f  $AOSHOME/src/rhcLib/libcrypto.so.0.9.8	$TARGET/$PACKAGE_NAME/Config/
cp -f $AOSHOME/src/KernelInterface_exe/Cli.exe    $TARGET/$PACKAGE_NAME/Bin/
cp -f $AOSHOME/src/GUI/src/setxmlconfig 	 $TARGET/$PACKAGE_NAME/Bin/
cp -f $AOSHOME/src/GUI/src/setproxytimeout	$TARGET/$PACKAGE_NAME/Bin/
cp -f $AOSHOME/src/GUI/src/mysudo		$TARGET/$PACKAGE_NAME/Bin/
cp -f $LINUXHOME/arch/i386/boot/bzImage         $TARGET/$PACKAGE_NAME/Kernel/vmlinuz-$Fi_PACKAGE_NAME-$RELEASE
cp -f $AOSHOME/Data/cmd.txt                                 $TARGET/$PACKAGE_NAME/Bin/
cp -f $AOSHOME/src/Install/install-$Fi_PACKAGE_NAME.sh         $TARGET/$PACKAGE_NAME/Bin/
strip $TARGET/$PACKAGE_NAME/Bin/Cli.exe

chmod 755 $TARGET/$PACKAGE_NAME/Bin/*


#make tgz
tar czf $TARGET/$PACKAGE_NAME-$RELEASE.tar.gz -C $TARGET $PACKAGE_NAME 
cp -f $AOSHOME/src/Install/install-$Fi_PACKAGE_NAME.sh $TARGET


LINES=`cat $TARGET/install-$Fi_PACKAGE_NAME.sh|wc -l`
LINES=`expr 3 + $LINES`
echo '#!/bin/bash' > $TARGET/install-$Fi_PACKAGE_NAME-$RELEASE.bin
echo "LINES=$LINES" >>$TARGET/install-$Fi_PACKAGE_NAME-$RELEASE.bin
cat $TARGET/install-$Fi_PACKAGE_NAME.sh >> $TARGET/install-$Fi_PACKAGE_NAME-$RELEASE.bin
cat $TARGET/$PACKAGE_NAME-$RELEASE.tar.gz >>$TARGET/install-$Fi_PACKAGE_NAME-$RELEASE.bin
chmod 755 $TARGET/install-$Fi_PACKAGE_NAME-$RELEASE.bin

rm -f $TARGET/$PACKAGE_NAME-$RELEASE.tar.gz
rm -f $TARGET/install-$Fi_PACKAGE_NAME.sh

echo "=================================================================="
echo "SUCCESS! The install file is $TARGET/install-$Fi_PACKAGE_NAME-$RELEASE.bin"
echo "To install, please run: sudo $TARGET/install-$Fi_PACKAGE_NAME-$RELEASE.bin"
echo "=================================================================="

