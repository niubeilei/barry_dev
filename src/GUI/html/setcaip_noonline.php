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
<title>setcaip_noonline</title>
</head>
<body background="image/pmiback.gif"><?php
include("constant.php");
//include("func_setca.php");

//	print("$REMOTE_ADDR");
/*
	print("请设置OCSP服务器的IP地址和服务端口<br>");
	$a=filesize($CAIPCFGFILE);
	if ($a < 28)//!= 36)
	{
		print("OCSP配置文件长度不对！文件长度＝".$a);
//		print("	<a href=\"index.php\">返回首页</a>");
//		return;
	}
	else
	{
		system($apachesudo.CATXTCOM." r",$a);
		system($apachesudo."chmod "."666 ".CATXTFILE,$b);
//		print("$a<br>");
		if ($a != 0 || $b != 0)
		{
			print("备份OCSP配置文件失败！");
//			print("	<a href=\"index.php\">返回首页</a>");
//			return;
		}
		else
		{
			$fcont=file(CATXTFILE);
			list($key_pro,$catxtstr) = each($fcont);
			
			GetCaTxt(&$ca_pol,
				&$ca_ldapgap,
				&$ca_ldapport,
				&$ca_ocspport,
				&$ca_ocsponldapsuc,
				&$ca_ldapipstr,
				&$ca_ocspipstr,
				&$ca_ldapport2,
				&$ca_ldapipstr2);
		}
	}*/

	print("<form name=\"setcaip_noonline\" method=\"post\">
		<font face=\"楷体_GB2312\"><b>注意：<br><center>
		使用非在线认证方式存在安全上的隐患！
		请确认使用非在线认证方式，<br>否则请点击“返回设置网络参数页面”！</b></font><p>
	        <input type=\"submit\" value=\"确认\" name=\"NOONLINE\" length=20 onclick=\"setcaip_noonline.action='setcaip_noonline_ok.php'\">
		<br>
		<br>
		<br>
		<a href=\"parameter.php\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a>
		</center></form>");
?>
</body></html>