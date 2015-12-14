<?php
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '<p><br><br><center><font face="楷体_GB2312"><b>请先登陆！</font></b><p>';
	    echo '<a href="login.html"><img border="0" src="image/goback.gif" width="40" height="40"></a>';
            echo '</center></body></html>';
            exit();			
	}	include "pmi_db.php";
?>
<html>

<head>
<meta name="GENERATOR" content="Microsoft FrontPage 5.0">
<meta name="ProgId" content="FrontPage.Editor.Document">
<!--
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
-->
<title>访问控制</title>
</head>

<body background="image/pmiback.gif">

<?php
 	echo "<form name=\"control\" method=\"POST\" action=\"pmi1.php?appid=$appid&numb=1&rolenumb=1\">";
 ?>   

<table border="0" cellspacing="1" width="100%" id="AutoNumber2" height="480">
  <tr>

    <td width="100%" height="480">
     <p align="center"><font face="楷体_GB2312"><b>请选择应用服务:</font><select size="1" name="appid">
      <?php
	$db = mysql_connect(C_DB_HOST, "root");
	mysql_select_db(C_DB_NAME,$db);
 	$result = mysql_query("SELECT * FROM ".C_APP_WJ_TBL,$db);
  	if(!$result)
 	{
 		echo '没有满足条件的纪录';
		mysql_close($db);
 		return 0;
 	}
        while($row =mysql_fetch_row($result))
        {
         	echo "<option value=$row[0]>$row[1]</option>";
        }
        mysql_close($db);
     ?>      
        </select>
        <p align="center"><input type="radio" value="V1" checked name="R1"><font face="楷体_GB2312"><b>本地角色规则</b></font></p>
	<p align="center"><input type="radio" name="R1" value="V2"><font face="楷体_GB2312"><b>本地用户规则</b></font></p>
	<p align="center"><input type="radio" name="R1" value="V3"><font face="楷体_GB2312"><b>PMI统一授权访问控制</b></font></p>
<br><p align="center">
        <input type="button" value="确 定" name="B1" onclick="set()">
</p>       <INPUT type=hidden value="2" name=issubmit>
<INPUT type=hidden value="" name=appnamex><INPUT type=hidden value="" name=usernamex>

 </td>
  </tr>
  
</table>
</form>
</body>

<script language="javaScript">

function set(){
	if(document.control.R1[0].checked)
	{
		document.control.action="pmirole.php?numb=1&rolenumb=1";
		document.control.submit();
	}else if(document.control.R1[1].checked)
	{
		document.control.action="pmi1.php?numb=1&rolenumb=1";
		document.control.submit();
	}else if(document.control.R1[2].checked)
	{
		document.control.action="pmi_all.php?numb=1&rolenumb=1";
		document.control.submit();
	}
	
}



</script>
</html>
