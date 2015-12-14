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
<title>系统日志下载</title>
</head>
<body  background="image/pmiback.gif">
<?php
include "constant.php";

$cmd=MYSUDO . "/etc/cron.daily/logcron";
exec($cmd);
$cmd=MYSUDO . "ln -sf /var/log/msg.gz /usr/local/AOS/GUI/html/aos.syslog";
exec($cmd);
?>
<img border="0" src="image/logdown.gif" width="283" height="57">
<p>
<center><font face="楷体_GB2312"><b>打包完成,是否下载?</font></b></center>
<p>
<center><a href="aos.syslog">下  载</a></center>
<center><a href='aos.syslog'><img border="0" src="image/goback.gif" width="40" height="40"></a>
</center>

</body>
</html>
