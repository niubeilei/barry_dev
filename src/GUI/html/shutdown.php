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
 <form name="shut" method="POST" action="--WEBBOT-SELF--">  
<p>　</p>
<p>　</p>

<table border="0" cellpadding="0" cellspacing="0" style="border-collapse: collapse" bordercolor="#111111" width="600" id="AutoNumber1" height="250">
  <tr>
    <td width="150" height="250">
    </td>
    <td width="450" height="250" background="image/oprate.gif">
    <p align="left">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
    <img border="0" src="image/reserver.jpg" width="34" height="34" onclick=rebootser()>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
    <img border="0" src="image/reser.jpg" width="34" height="34" onclick=reboot()>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
    <img border="0" src="image/shutser.jpg" width="34" height="34" onclick=halt()>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
    <img border="0" src="image/update.gif" width="34" height="34" onclick=update()></td>
  </tr>
</table>
<p align="center">　</p>
</form>
</body>
<script language="javaScript">

function reboot(){
	if(window.confirm("服务器将重新启动"))
	{
		document.shut.action="reboot.php";
		document.shut.submit();
	}else{
	}
//	window.alert(document.control.numbtemp.value);
}

function halt(){
	if(window.confirm("服务器将关闭"))
	{
		document.shut.action="halt.php";
		document.shut.submit();
	}else{
	}
}
function rebootser(){
	if(window.confirm("代理服务将重新启动"))
	{
		document.shut.action="rebootser.php";
		document.shut.submit();
	}else{
	}
}
function update(){
		document.shut.action="uploadfile.php";
		document.shut.submit();

}
</script>
</html>
