PACKAGE_NAME=keyou_tmp
Fi_PACKAGE_NAME=keyou
PREFIX=/usr/local
TARGET=$PREFIX/$PACKAGE_NAME
Fi_TARGET=$PREFIX/$Fi_PACKAGE_NAME

add_mystro_user()
{

	USERNAME=mystro
	USERID=234
	USERSHELL=/usr/bin/Cli.exe
	USERHOME=$TARGET/Bin
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


start_am()
{

	if [ -f /etc/init.d/rc ]
	then
		BOOTFILE=/etc/init.d/rc
	elif [ -f /etc/conf.d/local.start ]
	then
		BOOTFILE=/etc/conf.d/local.start
	elif [ -f /etc/rc.local ]
	then
		BOOTFILE=/etc/rc.local
	else
		echo "Can not file the boot file"
		return 
	fi


	grep -q start_am $BOOTFILE
	if [ $? -ne 0 ]
	then
		echo "Install AM..."
		echo "#start_am" >> $BOOTFILE
		echo "$TARGET/Bin/am -m file -a $TARGET/Config/user.conf -f $TARGET/Log/proxyuserauth.log" >> $BOOTFILE
		echo "/usr/local/keyou/Bin/hac" >> $BOOTFILE
	else
		echo "AM already installed"
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
/bin/rm -rf $TARGET
TMPFILE=`mktemp /tmp/XXXXXXXX`
/bin/cp -f $0 $TMPFILE
tail -n +$LINES $TMPFILE | tar zx -C $PREFIX
chown -R root: $TARGET
chmod +s $TARGET/Bin/mysudo
/bin/cp -f $TARGET/Bin/Cli.exe /usr/bin
/bin/cp -f $TARGET/Bin/cmd.txt /etc
/bin/cp -f  $TARGET/Config/libcrypto.so.0.9.8 /usr/lib/
/bin/rm -f  $TARGET/Config/libcrypto.so.0.9.8

RELEASE=`$TARGET/Bin/Cli.exe -r`
echo "Version:$RELEASE"
/bin/cp -f $TARGET/Kernel/vmlinuz-$Fi_PACKAGE_NAME-$RELEASE /boot
/bin/rm -f $TARGET/Kernel/vmlinuz-$Fi_PACKAGE_NAME-$RELEASE 
ln -sf vmlinuz-$Fi_PACKAGE_NAME-$RELEASE /boot/vmlinuz-$Fi_PACKAGE_NAME

/bin/mkdir -p $Fi_TARGET

/bin/cp -rf $TARGET/Kernel $Fi_TARGET/
/bin/cp -rf $TARGET/Bin $Fi_TARGET/
/bin/cp -rf --reply=no $TARGET/Config $Fi_TARGET/
/bin/cp -rf --reply=no $TARGET/Log $Fi_TARGET/

/bin/rm -rf $TARGET

#add_mystro_user
start_am

echo "Install ok"
echo "Please edit /boot/grub/menu.lst, add the item of the new AOS kernel."
exit 0 
