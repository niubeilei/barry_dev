<?php
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/backgroud.gif"><b>' ;
	    echo '请先登陆！<p>';
	    echo '<a href="login.html">返回</a>';
            echo '</b></body></html>';
            exit();			
	}
		print("<form name=\"execcmd\" method=\"post\">
		请输入需要执行的命令行<br>
		<input type=\"TEXT\" name=\"cmdline\" value=\"\" size=100 maxlength=400>
		<br>
	        <input type=\"submit\" value=\"执行确认\" name=\"OK\" onclick=\"execcmd.action='eusercmd.php'\">
	        <input type=\"submit\" value=\"返回登录页\" name=\"log\" onclick=\"execcmd.action='login.html'\">
		</form>");
?>
