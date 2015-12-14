#!/bin/sh
PREFIX=/usr/local/
RHC_KERNEL_MOD=2.6.15.5rhc
RHC_MOD=/lib/modules/2.6.15.5rhc
RHC_DIR=/usr/local/rhc
RHC_BOOT_DIR=/usr/local/rhc/boot
if [ `id -u` -ne 0 ]
then
	echo "You must be root to install"
	exit 1
fi

if [ a$LINES == a ]
then
	echo "LINES error"
	exit 1
fi

echo "starting install..."

echo "
=============================================================
             Install rhc binary package 
=============================================================
"
#for inetctrl
if [ ! -e /var/online_time ]; then
	touch /var/online_time
fi
#end inetctrl
if [ -d $RHC_DIR ]; then
	rm -rf $RHC_DIR
fi
TMPFILE=`mktemp /tmp/XXXXXXXX`
/bin/cp -f $0 $TMPFILE
tail -n +$LINES $TMPFILE | tar zx -C $PREFIX
chown -R root.root $RHC_DIR
# add the lib to system
grep $RHC_DIR/lib/ /etc/ld.so.conf 2>&1>/dev/null

if [ $? -ne 0 ]; then
	echo $RHC_DIR/lib/  >>/etc/ld.so.conf
	ldconfig
	ln -s /usr/local/rhc/lib/libstdc++.so.6.0.8 /usr/local/rhc/lib/libstdc++.so.6
fi

#add the SystemCli start script to system

if [ -f /etc/init.d/rhcinit ]; then
	rm /etc/init.d/rhcinit
fi
cp $RHC_DIR/bin/rhcinit /etc/init.d/
chmod 755 /etc/init.d/rhcinit

rc-update show|grep  rhcinit 2>&1>/dev/null

if [ $? -ne 0 ]; then
	rc-update add rhcinit default
fi
mkdir -p /usr/local/rhc/config/
echo "
=============================================================
            Finished  rhc binary install!
=============================================================
"
#install webwall
echo "
=============================================================
            Install webwall binary!
=============================================================
"
if [ -f $RHC_DIR/bin/webwall.tar.gz ]; then
	tar zxf $RHC_DIR/bin/webwall.tar.gz -C $RHC_DIR/bin/
else
echo "
=============================================================
          	Webwall source package is missing!
=============================================================
"
exit 2
fi
if [ $? -ne 0 ]; then
echo "
=============================================================
           Untar webwall failed!
=============================================================
"
exit 2
fi
#install
cd $RHC_DIR/bin/webwall_tmp/dansguardian-2.9.8.0/
make install
if [ $? -ne 0 ]; then
echo "
=============================================================
            Dansguardian install failed!
=============================================================
"
exit 2
fi
cd ../tinyproxy-1.6.3/
make install
if [ $? -ne 0 ]; then
echo "
=============================================================
            Tinyproxy install failed!
=============================================================
"
exit 2
fi
rm -rf $RHC_DIR/bin/webwall.tar.gz
rm -rf $RHC_DIR/bin/webwall_tmp
echo "
=============================================================
           	Install rhc kernel
=============================================================
"
# cp the kernel image to /boot/
if [ ! -f $RHC_BOOT_DIR/rhcImage ]; then
	echo "=============================================================
           	The rhc Image is not exist! 
			 Please check!
============================================================= "
	    exit 1
fi

cp $RHC_BOOT_DIR/rhcImage /boot/

# add the kernel modules to /lib/modules
if [ ! -d $RHC_BOOT_DIR/$RHC_KERNEL_MOD ]; then 
	echo "==============================================================
       	   	 Rhc Kernel modules is not exist! 
			 Please check!
============================================================== "
	exit 1

fi

if [ -d $RHC_MOD ]; then
	rm -rf $RHC_MOD
fi

cp -a $RHC_BOOT_DIR/$RHC_KERNEL_MOD /lib/modules/

echo "
==============================================================
    Finished the kernel istall, Please edit the menu.lst.	
   After you reboot the machine, rhc should be work well.
============================================================== "
exit
