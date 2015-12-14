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
<title>setcaip_noonline_ok</title>
</head>
<body background="image/pmiback.gif"><?php
include("constant.php");
//include("func_setca.php");

//	print("OCSP服务器IP地址：$ocsp_ip<br>OCSP服务端口号：$ocsp_port");
	GetCaTxt(&$ca_pol,
		&$ca_ldapgap,
		&$ca_ldapport,
		&$ca_ocspport,
		&$ca_ocsponldapsuc,
		&$ca_ldapipstr,
		&$ca_ocspipstr,
		&$ca_ldapport2,
		&$ca_ldapipstr2);
		
	$ca_pol=3;
//	$ca_ocspport=$ocsp_port;
//	$ca_ocspipstr=$ocsp_ip;

	$ret=SetCaTxt($ca_pol,
		$ca_ldapgap,
		$ca_ldapport,
		$ca_ocspport,
		$ca_ocsponldapsuc,
		$ca_ldapipstr,
		$ca_ocspipstr,
		$ca_ldapport2,
		$ca_ldapipstr2);

	if ($ret != FALSE)
	{		
		system($apachesudo.CATXTCOM." w",$a);
		CopyFileToElecDisk(SETCAFILE,SETCAFILE_DZP);
		if ($a != 0)
		{
			print("<center><font face=\"楷体_GB2312\">更新OCSP配置文件失败！</font>");
		}else{
			print("<center><font face=\"楷体_GB2312\">更新OCSP配置文件成功！</font>");
		}
	}
?>

	<p>
	<a href=parameter.php><img border="0" src="image/goback.gif" width="40" height="40"></a>
</center></body></html>