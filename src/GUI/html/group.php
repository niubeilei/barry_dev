<?php
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
 <html>

<head>
<meta http-equiv="Content-Language" content="zh-cn">
<!--
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
-->
<title>新建网页 2</title>
</head>

<body background="image/pmiback.gif">
<form name="group" METHOD="post">
 <table border="0" cellspacing="0" width="100%" id="AutoNumber2" height="44" bgcolor="#eef4f4">
  <tr>
  <td width="20%"></td>
<td width ="80%">
　&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <fieldset style="width: 160px; height: 175px; border-style: groove; border-width: 1px; padding: 0; background-color: #eef4f4">
<legend><font face="华文琥珀" size="4">组别</font></legend>
<p align="center">
<input type="radio" value="V1" checked name="R1"  onclick="checkitem()"><font face="华文楷体" size="4"> 级别1</font><p align="center">
<input type="radio" value="V2" name="R1"  onclick="checkitem()"><font size="4" face="华文楷体"> 级别2</font></p>
<p align="center"><input type="radio" value="V3" name="R1"  onclick="checkitem()"><font face="华文楷体" size="4"> 级别3</font></p>
</fieldset><p><font face="华文楷体" size="4">&nbsp;&nbsp;&nbsp; 选择隶属的1级组：</font><select size="1" name="D1" disabled="true">
</select></p>
<p><font face="华文楷体" size="4">&nbsp;&nbsp;&nbsp; 选择隶属的2级组：</font><select size="1" name="D2" disabled="true">
</select></p>
<p>&nbsp;&nbsp; <font face="华文楷体" size="4">组名：</font><input type="text" name="T1" size="20"></p>
<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<input type="button" value="注 册" name="B1"></p>
</td></tr></table>
<script language="javascript">
function checkitem(){
	if(document.group.R1[0].checked==true)
	{
		document.group.D1.disabled=true;
		document.group.D2.disabled=true;
	}
	if(document.group.R1[1].checked==true)
	{
		document.group.D1.disabled=false;
		document.group.D2.disabled=true;
	}
	if(document.group.R1[2].checked==true)
	{
		document.group.D1.disabled=false;
		document.group.D2.disabled=false;
	}
}</script>
</form>
</body>
</html>