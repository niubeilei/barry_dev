<?
include "pmi_db.php";
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '<p><br><br><center><font face="楷体_GB2312"><b>请先登陆！</font></b><p>';
	    echo '<a href="login.html"><img border="0" src="image/goback.gif" width="40" height="40"></a>';
            echo '</center></body></html>';
            exit();			
	}
?>
<form name=fileupdown method=post>
	FTP服务器IP地址<br>
	<input type=text name=FTPSvrIP value=192.168.1.200><br>
	FTP服务端口号<br>
	<input type=text name=FTPSvrPort value=21><br>
	用户名<br>
	<input type=text name=UserName value=zzc><br>
	用户口令<br>
	<input type=password name=UserPasswd value=poiuyt><br>
	FTP服务器中文件名<br>
	<input type=text name=SvrFileName><br>
	PWL517中文件名<br>
	<input type=text name=LocalFName><br><br>
	<input type=submit value=下载文件 onclick="fileupdown.action='upwjfiles.php?upload=0'">
	<input type=submit value=上传文件 onclick="fileupdown.action='upwjfiles.php?upload=1'">
	<input type=reset value=重置参数 >
</form>
