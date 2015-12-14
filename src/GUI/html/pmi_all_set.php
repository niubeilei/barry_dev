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

include("constant.php");

if ($op == 0)
{
	$fp=fopen("/tmp/".$appid.".DepName","w");
	if ($fp)
	{
		fwrite($fp,$depname,strlen($depname));
		fclose($fp);
	}
	$fp=fopen("/tmp/".$appid.".AppName","w");
	if ($fp)
	{
		fwrite($fp,$appname,strlen($appname));
		fclose($fp);
	}
	$cmd=SUDO.MOUNT_CMD;
	$ret=system($cmd,$a);

	$cmd=SUDO."cp "."/tmp/".$appid.".DepName"." ".MOUNT_PATH."SSLMPS/Config/".$appid.".DepName";
	$ret=system($cmd,$a);
	$cmd=SUDO."cp "."/tmp/".$appid.".AppName"." ".MOUNT_PATH."SSLMPS/Config/".$appid.".AppName";
	$ret=system($cmd,$a);

	$cmd=SUDO.UMOUNT_CMD;
	$ret=system($cmd,$a);
	
	$cmd=SUDO."cp "."/tmp/".$appid.".DepName"." /usr/SSLMPS/Config/".$appid.".DepName";
	$ret=system($cmd,$a);
	$cmd=SUDO."cp "."/tmp/".$appid.".AppName"." /usr/SSLMPS/Config/".$appid.".AppName";
	$ret=system($cmd,$a);

	echo "设置PMI统一授权访问控制－－成功<br>";
	echo "单位名称：$depname<br>";
	echo "应用系统名称：$appname<br>";
}
else if ($op == 1)
{
	$cmd=SUDO."rm -f /usr/SSLMPS/Config/".$appid.".DepName";
	$ret=system($cmd);
	$cmd=SUDO."rm -f /usr/SSLMPS/Config/".$appid.".AppName";
	$ret=system($cmd);

	$cmd=SUDO.MOUNT_CMD;
	$ret=system($cmd,$a);

	$cmd=SUDO."rm -f ".MOUNT_PATH."SSLMPS/Config/".$appid.".DepName";
	$ret=system($cmd,$a);
	$cmd=SUDO."rm -f ".MOUNT_PATH."SSLMPS/Config/".$appid.".AppName";
	$ret=system($cmd,$a);

	$cmd=SUDO.UMOUNT_CMD;
	$ret=system($cmd,$a);
	echo "撤销PMI统一授权访问控制－－成功";
}
?>