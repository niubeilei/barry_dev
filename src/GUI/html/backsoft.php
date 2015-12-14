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
<body> 
<? 
include "constant.php";
/*
//启动电子盘
 	$cmd=SUDO."mount /dev/hda2 /mnt/data";
  //	echo $cmd;
	system($cmd,$a);

$cmd=SUDO."mkdir /tmp/upgrade";
system($cmd,$a);
$cmd=SUDO."chmod 777 /tmp/upgrade";
system($cmd,$a);




if(!file_exists("/mnt/data/backup/oldversion.tgz"))
{
            echo '<html><head><title>恢复失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '<p><br><br><center><font face="楷体_GB2312"><b>没有可更新的版本</font></b><p>';
	    echo '<a href="uploadfile.php"><img border="0" src="image/goback.gif" width="40" height="40"></a>';
            echo '</center></body></html>';
            exit();			
	
}

$cmd=SUDO."tar xvfz /mnt/data/backup/oldversion.tgz -C /tmp/upgrade";
system($cmd,$a);
$cmd=SUDO."mv /tmp/upgrade/tmp/backup/* /tmp/upgrade";
system($cmd,$a);
$cmd=SUDO."find /tmp/upgrade -type f > /tmp/upgrade/filelist.txt";
system($cmd,$a);
$fd = fopen("/tmp/upgrade/filelist.txt", "r");
//$dirlen=fgets($fd,250);
//$dirlen=strlen($dirlen);
$dirlen=13;
while ($buffer = fgets($fd, 250)) 
{
	$len=strlen($buffer);
	$buffer=substr($buffer,0,$len-1);
	$oldbuffer=substr($buffer,$dirlen,$len-1-$dirlen);
	
	//启动电子盘
 	$cmd=SUDO."mount /dev/hda2 /mnt/data";
  //	echo $cmd;
	system($cmd,$a);
	
	
	//向电子盘上写老版本
	if($oldbuffer!="filelist.txt")
	{
  		$cmd=SUDO."cp  ".$buffer."  /mnt/data/".$oldbuffer;
  //		echo $cmd;
		system($cmd,$a);
	}
}
	
	//清除临时文件
	$cmd=SUDO."rm -rf /tmp/upgrade";
  //	echo $cmd;
	system($cmd,$a);

	
	//卸载电子盘
	$cmd=SUDO."umount /mnt/data";
  //	echo $cmd;
	system($cmd,$a);
	

fclose($fd);
*/


$cmd=MYSUDO."/usr/local/AOS/Bin/downgrade-ed";
echo passthru($cmd,&$a);

if($a==0)
{
	echo "<p>\n";
	echo "恢复到旧版本成功，请重新启动，以便使用就版本 <p>\n";
}
else
{
	echo "<p>\n";
	echo "恢复到旧版本失败<p>\n";
}

?> 
</body> 
</html> 
