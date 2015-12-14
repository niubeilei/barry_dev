<?php
include("constant.php");
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '<p><br><br><center><font face="楷体_GB2312"><b>请先登陆！</font></b><p>';
	    echo '<a href="login.html"><img border="0" src="image/goback.gif" width="40" height="40"></a>';
            echo '</center></body></html>';
            exit();			
	}
        echo '<html><head><meta http-equiv="refresh" content="60;url=shutdown.php"><title>服务器重新启动</title></head><body  background="image/pmiback.gif">' ;	
	echo "<p><br><br><center><font face=\"楷体_GB2312\"><b>代理服务器正在重新启动，".$ln;
        echo "请耐心等待...</b></font>".$ln;
//	register_shutdown_function(MyReboot);
        echo '</center></body></html>';
        
        //-------------------------20060101;by yt;reboot
        exec(MYSUDO . " /sbin/reboot -f");
        //-------------------------20060101;by yt;reboot
?>
