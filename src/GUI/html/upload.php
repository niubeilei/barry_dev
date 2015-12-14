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
<body background="image/pmiback.gif"> 
<? 
include "constant.php";

 
if ($_FILES[userfile] == "")
{ 
	die("没有指定上载的文件");
}

echo "上载成功<p>\n";
echo "临时文件名:" . $_FILES[userfile][tmp_name] . "<p>\n";
echo "文件名:" . $_FILES[userfile][name] . "<p>\n";
echo "文件大小:" . $_FILES[userfile][size] . "<p>\n";

$realfile=$_FILES[userfile][tmp_name] . "-" . $_FILES[userfile][name] ;
echo "改名为:" . $realfile . "<p>\n";
$cmd=MYSUDO . "mv -f " . $_FILES[userfile][tmp_name] . " " . $realfile;
echo passthru($cmd);


echo "upgrade ... <p>\n";
$cmd=MYSUDO . "/usr/local/AOS/Bin/upgrade-ed " . $realfile;
echo passthru($cmd, $a);
if($a==0)
{
	echo "升级成功<p>\n";
}
else
{
	echo "升级失败<p>\n";
}

echo "delete old files <p>\n";
$cmd=MYSUDO . "rm -f " . $realfile;
echo passthru($cmd);
?> 
</body> 
</html> 


