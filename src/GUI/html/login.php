<?php
@session_start();
?>
<html>

<head>
<meta name="GENERATOR" content="Microsoft FrontPage 5.0">
<meta name="ProgId" content="FrontPage.Editor.Document">
<!--
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
-->
<title>PWL517管理系统登陆</title>
</head>

<body background="image/pmiback.gif">
 <form name="login" method="POST" action="check_user.php">
<table border="0" cellspacing="1" width="100%" id="AutoNumber2" height="360">
  <tr>
  <td width="100%" height="60"><p align="center">
      <font face="楷体_GB2312"><b>管理员名：</b></font>
      <input type="text" name="managername" size="15">
      <p align="center"><font face="楷体_GB2312"><b>密&nbsp;&nbsp;&nbsp;&nbsp;码：</b></font>
      <input type="password" name="password" size="15">
      <p align="center">
      <input type="submit" value="登 陆" name="B1">
  </td>
  </tr>
</table>
</form>
</body>

</html>