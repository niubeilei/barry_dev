AOS=/usr/local/AOS
PRODUCT=mystro

add_mystro_user()
{

	USERNAME=mystro
	USERID=0
	USERSHELL=/usr/bin/Cli.exe
	USERHOME=/usr/local/AOS/Bin
	USERPASSWORD='$1$zN7rsKau$iw9l68lP3Da5y13r7DLmE1'

	grep -q ^$USERNAME /etc/passwd
	if [ $? -ne 0 ]
	then
		echo "Creating the $USERNAME user"
		echo "$USERNAME:x:$USERID:$USERID:,,,:$USERHOME:$USERSHELL" >>/etc/passwd
		echo "$USERNAME:x:$USERID:" >> /etc/group
		echo "$USERNAME:$USERPASSWORD:13341:0:99999:7:::">> /etc/shadow
	else
		echo "The user $USERNAME already exists"
	fi

}

set_clideamon_addr()
{
	BOOTFILE=/etc/hosts

	grep -q CLIDEAMON_ADDR $BOOTFILE
	if [ $? -ne 0 ]
	then
		echo "Install CliDeamon"
		echo "127.0.0.1  CLIDEAMON_ADDR" >> $BOOTFILE
	else
		echo "CliDeamon already installed"
	fi
								
}

start_clideamon()
{
	if [ -f /etc/init.d/rc ]
	then
		BOOTFILE=/etc/init.d/rc
	elif [ -f /etc/conf.d/local.start ]
	then
	    BOOTFILE=/etc/conf.d/local.start
	else
		echo "Can not file the boot file"
	    return
	fi
	grep -q CliDeamon.exe $BOOTFILE
	if [ $? -ne 0 ]
	then
		echo "Install CliDeamon"
		echo "cd /usr/local/AOS/Bin " >> $BOOTFILE
		echo "/usr/local/AOS/Bin/CliDeamon.exe&" >> $BOOTFILE
	else
		echo "CliDeamon already installed"
	fi
								
}


start_dns()
{

	if [ -f /etc/init.d/rc ] 
	then
		BOOTFILE=/etc/init.d/rc
	elif [ -f /etc/conf.d/local.start ]
	then
		BOOTFILE=/etc/conf.d/local.start
	else
		echo "Can not file the boot file"
		return 
	fi

	grep -q dnslookup $BOOTFILE
	if [ $? -ne 0 ]
	then
		echo "Install dnslookup"
		echo "/usr/local/AOS/Bin/dnslookup" >> $BOOTFILE
	else
		echo "dnslookup already installed"
	fi

}


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

killall -9 CliDeamon.exe
/bin/rm -rf $AOS
TMPFILE=`mktemp`
/bin/cp -f $0 $TMPFILE
tail -n +$LINES $TMPFILE | tar zx -C /usr/local
chown -R root: $AOS

/bin/cp -f $AOS/Bin/Cli.exe /usr/bin
/bin/cp -f $AOS/Bin/dnslookup /usr/bin
#/bin/rm -rf /lib/modules/2.6.15.5
#/bin/cp -rf $AOS/lib/modules/2.6.15.5 /lib/modules
/bin/rm -rf $AOS/lib/


RELEASE=`$AOS/Bin/Cli.exe -r`
/bin/cp -f $AOS/Kernel/vmlinuz-$PRODUCT-$RELEASE /boot
/bin/rm -f $AOS/Kernel/vmlinuz-$PRODUCT-$RELEASE 
ln -sf vmlinuz-$PRODUCT-$RELEASE /boot/vmlinuz-$PRODUCT
#/bin/rm -f /boot/initrd.img-$PRODUCT-$RELEASE
#mkinitrd -o /boot/initrd.img-$PRODUCT-$RELEASE 2.6.7
#ln -sf initrd.img-$PRODUCT-$RELEASE /boot/initrd.img-$PRODUCT

add_mystro_user
start_dns
start_clideamon
set_clideamon_addr

echo "install ok"
echo "Please edit /boot/grub/menu.lst, add the item of the new ROS kernel."
exit 0 
