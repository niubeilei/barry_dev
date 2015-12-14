<?php
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '请先登陆！<p>';
	    echo '<a href="login.html">返回</a>';
            echo '</b></body></html>';
            exit();			
	}
?>
<html>
<head>
<title>setcaip</title>
</head>
<body background="image/pmiback.gif">
<?php
	print("<center><b><font face=\"楷体_GB2312\"><b>请选择使用的证书验证方式</b></font><br>");
	print("<form name=\"setcaip\" method=\"post\">
	        <input type=\"submit\" value=\"OCSP在线认证方式\" name=\"OCSP\" length=100 onclick=\"setcaip.action='setcaip_ocsp.php'\">
		<br>
		<br>
		<br>
	        <input type=\"submit\" value=\"无在线认证方式\" name=\"NOONLINE\" length=100 onclick=\"setcaip.action='setcaip_noonline.php'\">
		<br>
		<br>
		<br>
		<a href=\"parameter.php\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a>
		</center></form>");
?>
</body></html>