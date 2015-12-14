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
	echo '<html><head><title>服务器关闭</title></head><body  background="image/pmiback.gif">' ;
	echo "<p><br><br><center><font face=\"楷体_GB2312\"><b>代理服务器正在关机，".$ln;
        echo "请等待...</b></font>".$ln;
//	register_shutdown_function(MyHalt);	
        echo '</center></body></html>';
        
	      //-------------------------20060101;by yt;halt
		
        exec(MYSUDO." /sbin/halt -fnp");
        //-------------------------20060101;by yt;halt
        
?>
