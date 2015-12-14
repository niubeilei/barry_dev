<?php
	include("pmi_db.php");

	@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '<p><br><br><center><font face="楷体_GB2312"><b>请先登陆！</font></b><p>';
	    echo '<a href="login.html"><img border="0" src="image/goback.gif" width="40" height="40"></a>';
            echo '</center></body></html>';
            exit();			
	}
	
	if($oprate==1)
		addfileconn($appname,$appid,$filenm);
	if($oprate==2)
		delfileconn($appname,$appid,$connectid);
		
?>
<html>
<head>
<meta http-equiv="Content-Language" content="zh-cn">
<meta name="GENERATOR" content="Microsoft FrontPage 5.0">
<meta name="ProgId" content="FrontPage.Editor.Document">
<!--
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
-->
<title>新建网页 1</title>
</head>

<body background="image/pmiback.gif">


<?php
	echo "<form name=\"setfile\" method=\"post\" action=\"setfile.php?appname=".$appname."&appid=".$appid."\">";
	echo "<p align=\"center\">";
	echo "<b><font face=\"楷体_GB2312\" size=\"4\" color=\"#FF0000\">".$appname."</font><font face=\"楷体_GB2312\" size=\"4\" color=\"#136b6b\">应用服务的安全用户帐号关联文件</font></b>";
	echo "<input type=\"hidden\" name=\"appid1\" value=".$appid.">";
?>
</p>
<center><b><font face="楷体_GB2312">文件名：</font></b><input type="text" name="filenm" size="17">
<p align="center"><input type="button" value="添 加" name="B3" onclick="setopra()"></p>
<p align="center"><b><font face="楷体_GB2312">选择已指定的文件：</font></b><select size="1" name="connectid">
<?php
	$dblink=new DB;
	$dblink->Database=C_DB_NAME;
	$dblink->connect();
	if ($dblink->Errno != 0)
		die();
	$sql="select * from ".C_APP_CONN_FILE." where appid=".$appid;
	$dblink->Query($sql);
	$nr=$dblink->num_rows();
	$count=0;
	if ($nr != 0)
	{
		while (list($connid,$appid,$filename)=$dblink->next_record())
		{
			if($count==0)
				echo "<option value=".$connid." selected>".$filename."</option>";
			else
				echo "<option value=".$connid.">".$filename."</option>";
		}
	}
	$dblink->close();
?>		
</select></p>

	
<p align="center"><input type="button" value="删除关联文件" name="delete" onclick="setdelete()">&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" value="参数设置" name="setpara" onclick="setparam()"></p>
<p align="center"><a href="server.php?numb=1"><img border="0" src="image/goback.gif" width="40" height="40"></a></p>
<input type="hidden" name="oprate" value="0">
<?php
	echo "<input type=\"hidden\" name=\"appname1\" value=".$appname.">";

?>
</form>
</body>
<script language="javaScript">
function setopra(){
	var flen=document.setfile.filenm.value;
	
	if(document.setfile.filenm.value==""){
		window.alert("文件名不能为空！");
	}else if(flen.length>235){
		alert("文件名过长！");
	}else{
		document.setfile.oprate.value=1;
		document.setfile.submit();
	}
//	window.alert(document.control.numbtemp.value);
}
function setdelete(){
	if(window.confirm("若删除此关联文件，其相关的参数也将删除。\n确定删除此关联文件吗？"))
	{
		document.setfile.oprate.value=2;
		document.setfile.submit();
	}else{
	}
//	window.alert(document.control.numbtemp.value);
}
function setparam()
{
	document.setfile.action="setpara.php";
	//window.alert("here");
	document.setfile.submit();
	
}
</script>
</html>