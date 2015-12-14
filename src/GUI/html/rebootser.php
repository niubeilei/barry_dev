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

        echo '<html><head><meta http-equiv="refresh" content="30;url=shutdown.php"><title>代理服务重新启动</title></head><body  background="image/pmiback.gif">' ;	
	echo "<p><br><br><center><font face=\"楷体_GB2312\"><b>代理服务正在重新启动，".$ln;
        echo "请耐心等待...</b></font></center>".$ln;
        
	echo "执行结果：".'<hr>';
	$a="";
	echo "<p><pre>\n";
	$cmd=MYSUDO."/etc/init.d/aos-script stop";
	echo passthru($cmd,$a);
	$cmd=MYSUDO."/etc/init.d/aos-script start";
	echo passthru($cmd,$a);
	echo "\n";
	echo "</pre>";
	
    echo '</body></html>';
?>
