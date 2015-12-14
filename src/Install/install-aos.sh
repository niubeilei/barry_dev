AOS=/usr/local/AOS
PACKAGE=/tmp/sslproxy.tar.gz

add_mystro_user()
{

	USERNAME=mystro
	USERID=234
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

/bin/rm -rf $AOS
TMPFILE=`mktemp`
/bin/cp -f $0 $TMPFILE
tail -n +$LINES $TMPFILE | tar zx -C /usr/local
chown -R root: $AOS

/bin/cp -f $AOS/Bin/Cli.exe /usr/bin
/bin/cp -f $AOS/Bin/dnslookup /usr/bin
#/bin/rm -rf /lib/modules/2.6.7
#/bin/cp -rf $AOS/lib/modules/2.6.7 /lib/modules
#/bin/rm -rf $AOS/lib/


RELEASE=`$AOS/Bin/Cli.exe -r`
/bin/cp -f $AOS/Kernel/vmlinuz-sslproxy-$RELEASE /boot
/bin/rm -f $AOS/Kernel/vmlinuz-sslproxy-$RELEASE 
ln -sf vmlinuz-sslproxy-$RELEASE /boot/vmlinuz-sslproxy
#/bin/rm -f /boot/initrd.img-sslproxy-$RELEASE
#mkinitrd -o /boot/initrd.img-sslproxy-$RELEASE 2.6.7
#ln -sf initrd.img-sslproxy-$RELEASE /boot/initrd.img-sslproxy

add_mystro_user

echo "install ok"
echo "Please edit /boot/grub/menu.lst, add the item of the new AOS kernel."
exit 0 
