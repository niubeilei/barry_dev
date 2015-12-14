<?php
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '<p><br><br><center><font face="楷体_GB2312"><b>请先登陆！</font></b><p>';
	    echo '<a href="login.html"><img border="0" src="image/goback.gif" width="40" height="40"></a>';
            echo '</center></body></html>';
            exit();
	}
?>
<html>
<head>
<title>处理上载文件</title>
</head>
<body  background="image/pmiback.gif">
<?php
include "constant.php";

//echo $userfile1;

$cmd=MYSUDO . "mv -f " . $userfile1 . " /tmp/upfile.tar.gz";
exec($cmd);
$cmd=MYSUDO . "/usr/local/AOS/Bin/tar-conf-to-ed /tmp/upfile.tar.gz";
exec($cmd);
$cmd=MYSUDO . "rm -f /tmp/upfile.tar.gz";
exec($cmd);
//echo "Please reboot"

/*
//-------------------------20060101;by yt;配置文件上载
$cmd=MYSUDO."mkdir /tmp/upfile";
exec($cmd,$a);
$cmd=MYSUDO."chmod 777 /tmp/upfile";
exec($cmd,$a);
copy($userfile1,"/tmp/upfile.tgz");
$cmd=MYSUDO."tar xfz /tmp/upfile.tgz -C /";
exec($cmd,$a);
//$cmd=MYSUDO."unzip ".WWWDOCDIR."upfile.zip -d /tmp/upfile";
//exec($cmd,$a);
$cmd=MYSUDO."mv /tmp/download/ifcfg-eth0 /etc/sysconfig/network-scripts/ifcfg-eth0";
exec($cmd,$a);
$cmd=MYSUDO."chown root:root /etc/sysconfig/network-scripts/ifcfg-eth0";
exec($cmd,$a);
$cmd=MYSUDO."mv /tmp/download/ifcfg-eth1 /etc/sysconfig/network-scripts/ifcfg-eth1";
exec($cmd,$a);
$cmd=MYSUDO."chown root:root /etc/sysconfig/network-scripts/ifcfg-eth1";
exec($cmd,$a);
$cmd=MYSUDO."mv /tmp/download/network /etc/sysconfig/network";
exec($cmd,$a);
$cmd=MYSUDO."chown root:root /etc/sysconfig/network";
exec($cmd,$a);
$cmd=MYSUDO."mv /tmp/download/crontab /etc/crontab";
exec($cmd,$a);
$cmd=MYSUDO."chown root:root /etc/crontab";
exec($cmd,$a);
$cmd=MYSUDO."cp /tmp/download/*.* ".CFGFILEPATH;
exec($cmd,$a);
$cmd=MYSUDO."cp /tmp/download/Denypages/* ".DENYPAGEFILEPATH;
exec($cmd,$a);
$cmd=MYSUDO."rm -rf /tmp/download";
exec($cmd,$a);
$cmd=MYSUDO."rm -f /tmp/upfile.tgz";
exec($cmd,$a);
*/

//-------------------------20060101;by yt;配置文件上载


//$cmd=MYSUDO."find /tmp/upfile -type f > /tmp/upfile/filelist.txt";
//exec($cmd,$a);
/*
$fd = fopen("/tmp/upfile/filelist.txt", "r");
//$dirlen=fgets($fd,250);
//$dirlen=strlen($dirlen);
$dirlen=12;
while ($buffer = fgets($fd, 250))
{
	$len=strlen($buffer);
	$buffer=substr($buffer,0,$len-1);
	$oldbuffer=substr($buffer,$dirlen,$len-1-$dirlen);

	//启动电子盘
 	$cmd=MYSUDO."mount /dev/hda2 /mnt/data";
  //	echo $cmd;
	exec($cmd,$a);

	//向电子盘上写新版本
//	if($oldbuffer=="db_origin.tgz")
	if ($oldbuffer=="wjproxy_backup.sql.tgz")
	{
  		$cmd=MYSUDO."cp  ".$buffer."  /mnt/data/Manage/".$oldbuffer;
  //		echo $cmd;
		exec($cmd,$a);
	}else if($oldbuffer=="ifcfg-eth0")
	{
		$cmd=MYSUDO."cp  ".$buffer."  /mnt/data/SSLMPS/Config/".$oldbuffer;
  //		echo $cmd;
		exec($cmd,$a);
	}else if($oldbuffer=="ifcfg-eth1")
	{
		$cmd=MYSUDO."cp  ".$buffer."  /mnt/data/SSLMPS/Config/".$oldbuffer;
  //		echo $cmd;
		exec($cmd,$a);
	}else if($oldbuffer=="network")
	{
		$cmd=MYSUDO."cp  ".$buffer."  /mnt/data/SSLMPS/Config/".$oldbuffer;
  //		echo $cmd;
		exec($cmd,$a);
	}else if($oldbuffer=="Protocol.conf")
	{
		$cmd=MYSUDO."cp  ".$buffer."  /mnt/data/SSLMPS/Config/".$oldbuffer;
  //		echo $cmd;
		exec($cmd,$a);
	}else if($oldbuffer=="root")
	{
		$cmd=MYSUDO."cp  ".$buffer."  /mnt/data/SSLMPS/Config/cron".$oldbuffer;
  //		echo $cmd;
		exec($cmd,$a);
	}else if($oldbuffer=="SetCA.cfg")
	{
		$cmd=MYSUDO."cp  ".$buffer."  /mnt/data/SSLMPS/".$oldbuffer;
  //		echo $cmd;
		exec($cmd,$a);
	}

	//卸载电子盘
	$cmd=MYSUDO."umount /mnt/data";
  //	echo $cmd;
	exec($cmd,$a);
}


使用了一个函数copy()将上载到临时目录下的文件拷贝到一个特定的目录，
并重新命名为"newfile",这是最简单的一种方法。
*/
/*
值得注意的是文件upload.html中表单选项 MAX_FILE_SIZE 的隐藏值域，通过设置
其Value(值)可以限制上载文件的大小。

echo $userfile."-用户上传到服务器上的文件临时存放的名称<br>";
echo $userfile_name."-在用户机器上该文件的原始名称<br>";
echo $userfile_size."-上传文件的实际字节数<br>";
echo $userfile_type."-如果用户的浏览器提供了这个信息的话，它表示mime的类型。例如image/gif<br>";
*/
?>
<p>
<center><font face="楷体_GB2312"><b>文件上传完毕!</font></b></center>
<p>
<center><a href='confile.php'><img border="0" src="image/goback.gif" width="40" height="40"></a>
</center>

</body>
</html>
