<?php

/* use http upload files
<form enctype="multipart/form-data" action=result_up.php method=post>
	<input type=hidden name=MAX_FILE_SIZE value=1000>
	<input type=file name=upfilebox>
	<input type=submit value=upload>
</form>

	$FTPSvrIP="192.168.1.200";
	$FTPSvrPort=21;
	$UserName="zzc";
	$UserPasswd="poiuyt";
	$upload=0;
	$LocalFName="/tmp/tmpfile";
	$SvrFileName="/boot.ini";
*/
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/backgroud.gif"><b>' ;
	    echo '请先登陆！<p>';
	    echo '<a href="login.html">返回</a>';
            echo '</b></body></html>';
            exit();			
	}
	$FTPStream=ftp_connect($FTPSvrIP,$FTPSvrPort);
	if ($FTPStream)
		echo "连接FTP服务器成功<br>";
	else
	{
		echo "连接FTP服务器失败<br>";
		exit(-1);
	}
	$LoginSucc=ftp_login($FTPStream,$UserName,$UserPasswd);
	if ($LoginSucc)
	{
		echo "登录FTP服务器成功<br>";
		if ($upload == 0)
		{
			echo "开始下载文件<br>";
			$LocalFP=fopen($LocalFName,"wb+");
			$Ret=ftp_fget($FTPStream,$LocalFP,$SvrFileName,FTP_BINARY);
			if ($Ret)
				echo "文件下载成功".$Ret."<br>";
			else
				echo "文件下载失败".$Ret."<br>";
			fclose($LocalFP);
		}
		else
		{
			echo "开始文件上传<br>";
			$LocalFP=fopen($LocalFName,"rb+");
			$Ret=ftp_fput($FTPStream,$SvrFileName,$LocalFP,FTP_BINARY);
			if ($Ret)
				echo "文件上传成功".$Ret."<br>";
			else
				echo "文件上传失败".$Ret."<br>";
			fclose($LocalFP);
		}
	}
	else
	{
		echo "登录FTP服务器失败<br>";
		$LogoutSucc=ftp_quit($FTPStream);
	}

/*	echo $fname."<br>".$LocalFName;
	$fp=fopen($fname,"rb");
	$flen=filesize($fname);
	$flen=34;
	echo $flen;
	$fcont=fread($fp,$flen);
	$LocalFP=fopen($LocalFName,"wb");
	$ret=fwrite($LocalFP,$fcont,$flen);
	fclose($fp);
	fclose($LocalFP);*/
?>
