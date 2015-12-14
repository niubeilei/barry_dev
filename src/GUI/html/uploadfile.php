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
<meta name="GENERATOR" content="Microsoft FrontPage 5.0">
<meta name="ProgId" content="FrontPage.Editor.Document">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>新建网页 1</title>
</head>

<body background="image/pmiback.gif">
<form name="update" enctype="multipart/form-data" action="upload.php" method="post"> 
   <input type="hidden" name="max_file_size" value="1000000000"> <br><br><br>
  <center>    <input type="button" value="恢复以前版本" onclick="back()"></center><br><br>
<div align="center"> 

<center><font face="楷体_GB2312"><b>请选择文件：</b></font> 
    <input name="userfile" type="file">
    <!--定义了一个变量 userfile,在upload.php中，我们就可以使用这个变量了，我们可以直接通过$userfile访问上载的文件，但由于这是一个保存文件的变量，因此文件名字必须通过另外一个$userfile_name变量取得。下面是这个变量的具体用途： 
$userfile:在将要存放上载文件的服务器上的临时文件名字。 
$userfile_name:在发送者系统中的初始文件名。 
$userfile_size:按字节计算的上载文件的大小。 
$userfile_type:多用途网际邮件扩充协议类型的文件，前提是浏览器提供这种信息，比如说“image/gif"。 
//--> 
<br><br>
    <input type="button" value="软件升级" onclick="upgrade()"> 
</center> 

</div> 
<p align="center"><a href="shutdown.php"><img border="0" src="image/goback.gif" width="40" height="40"></a></p>
</form></body>
<script language="javaScript">

function back(){
	if(window.confirm("确实要恢复上一次的软件版本吗？"))
	{
		document.update.action="backsoft.php";
		document.update.submit();
	}else{
	}
//	window.alert(document.control.numbtemp.value);
}
function upgrade(){
	if(window.confirm("确实要升级软件版本吗？"))
	{
		document.update.action="upload.php";
		document.update.submit();
	}else{
	}
//	window.alert(document.control.numbtemp.value);
}
</script>
</html>
