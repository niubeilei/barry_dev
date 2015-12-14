<?php
    include("constant.php");
    @session_start();

    //echo "ocspser";
    if($manager_ID==NULL)
	{
        echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '请先登陆！<p>';
	    echo '<a href="login.html">返回</a>';
        echo '</b></body></html>';
        exit();
	}
    echo '<html><head><title>OCSP服务器设置</title></head><body  background="image/pmiback.gif"><center><font face="楷体_GB2312"><b>';
    echo "$REMOTE_ADDR";
	echo "请求设置OCSP服务器的IP地址和服务端口</b><br><br>";

    $ipss=IsIPStr($ocsp_addr);
    //echo "$ipss";
    if ($ipss != 10)
	{
		echo "OCSP服务器设置的IP地址输入有误，请仔细检查！"."<br>";
		echo "<p></b></font><a href=\"parameter.php\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";
		exit();
	}

	if (!VarIsValid($ocsp_addr,1) or !VarIsValid($ocsp_port,3) or ($ocsp_port>=65536) or ($ocsp_port<=0))
	{
		echo "OCSP服务器设置的IP地址或端口输入有误,请仔细检查！"."<br>";
		echo "<p></b></font><a href=\"parameter.php\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";
		exit();
	}
	$ret=SetOCSP($ocspRadioGroup,$ocsp_addr,$ocsp_port);

    echo "OCSP服务器设置修改成功！";
    
    //------------20060101;by yt
    $cmd=MYSUDO.MYCLI."\\\"ocsp set addr $ocsp_addr $ocsp_port\\\"";
    exec($cmd,$a);
    $cmd=MYSUDO.MYCLI."\\\"ocsp status $ocspRadioGroup\\\"";
    exec($cmd,$a);    
  
	echo "<p></b></font><a href=\"parameter.php\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";
?>