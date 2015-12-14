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
<title>处理下载文件</title>
</head>
<body  background="image/pmiback.gif">
<?php
include "constant.php";


$cmd=MYSUDO . "/usr/local/AOS/Bin/tar-conf-ed /usr/local/AOS/GUI/html/confile.tar.gz";
exec($cmd);
?>
<p>
<center><font face="楷体_GB2312"><b>文件打包完成,是否下载?</font></b></center>
<p>
<center><a href="confile.tar.gz">下  载</a></center>
<center><a href='confile.php'><img border="0" src="image/goback.gif" width="40" height="40"></a>
</center>

</body>
</html>
