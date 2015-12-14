<?php
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '<p><br><br><center><font face="楷体_GB2312"><b>请先登陆！</font></b><p>';
	    echo '<a href="login.html"><img border="0" src="image/goback.gif" width="40" height="40"></a>';
            echo '</center></body></html>';
            exit();
	} ?>
 <html>

<head>
<meta http-equiv="Content-Language" content="zh-cn">
<meta name="GENERATOR" content="Microsoft FrontPage 6.0">
<meta name="ProgId" content="FrontPage.Editor.Document">
<!--
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
-->
<title>配置文件更新</title>
</head>

<body background="image/pmiback.gif">
 <form name="shut" method="POST" action="--WEBBOT-SELF--" enctype="multipart/form-data">
<table border="0" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#111111" width="811" id="AutoNumber1" height="250">
  <tr>
  	<td width="50%">
			<p align="center"><img border="0" src="image/downconf1.gif" width="283" height="57">
		  <p align="center">
			<font face="华文楷体"><input type="button" value="下载配置信息" name="B1" onclick=download()></font>
       &nbsp;&nbsp;&nbsp;&nbsp;<br><br><br><br>
    </td>
    <td width="50%">
    	<p align="left"><img border="0" src="image/downconf2.gif" width="283" height="57">
			<p align="left"><input name="userfile1" type="file" size="20">
		  <br><br><p align="left">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
		  	<input type="button" value="上传配置信息" name="B2" onclick=upload()></font>
		  	<br><br>
    </td>
  </tr>
</table>
</form>
</body>

<script language="javaScript">

function upload(){
	if(document.shut.userfile1.value=="")
		window.alert("请先选择要上传的文件");
	else{
		if(window.confirm("确实要覆盖服务器上原有配置文件吗？"))
		{
			document.shut.action="upconf.php";
			document.shut.submit();
		}else{
		}
	}
}
function download(){
	document.shut.action="download.php";
	document.shut.submit();	
}
</script>
</html>