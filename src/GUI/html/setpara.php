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
	{
		addpara($connectid,$partype,$parname,$certitem,$parformat,$appname2);
		$appname1=$appname2;
		$appid1=$appid2;
	}
	if($oprate==2)
	{
		$arr = array($check0,$check1,$check2,$check3,$check4,$check5,$check6,$check7,$check8,$check9);
		$dblink1=new DB;
		$dblink1->Database=C_DB_NAME;
		$dblink1->connect();
		if ($dblink->Errno != 0){
			echo '数据库连接失败';
			return 0;
		} 
		$sql ="SELECT * FROM ".C_FILE_CONN_PARA." where connid=".$connectid;			
		$dblink1->Query($sql);
		$nr=$dblink1->num_rows();
		$c=0;
		if ($nr != 0)
		{
			while (list($conid1,$paratype1,$paraname1,$certitem1,$paraformat)=$dblink1->next_record())
			{
				if($arr[$c]=="ON")
				{
					$parnamearr[$c]=$paraname1;
				}
				$c++;
			}
		}
		for($i=0;$i<$c;$i++)
			if($parnamearr[$i]!=NULL){
				$sql="delete from ".C_FILE_CONN_PARA." where connid=".$connectid." and PageParamName='".$parnamearr[$i]."'";
				$dblink1->Query($sql);
		//		del_user($user[$i]);
			}
		$dblink1->close();
		$appname1=$appname2;
		$appid1=$appid2;
	}
	if($oprate==3)
	{
		updatepara($connectid,$oldparaname,$partype1,$parname1,$ceritem1,$parformat1,$appname1);
	//	$appid1=$appid2;
	}
?>
<html>

<head>
<meta http-equiv="Content-Language" content="zh-cn">
<meta name="GENERATOR" content="Microsoft FrontPage 5.0">
<meta name="ProgId" content="FrontPage.Editor.Document">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>文件参数设置</title>
</head>

<body background="image/pmiback.gif">
<form name="setpar" method="post" action="setpara.php">
<p>
<img border="0" src="image/pagepara.gif" width="283" height="57"></p>
<b><font face="楷体_GB2312">应用服务名称：</font><font face="宋体" color="#ff0000">
<?php
	echo $appname1;
?></font>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<font face="楷体_GB2312">关联文件名：</font>
<font face="宋体" color="#ff0000">
<?php
	$dblink=new DB;
	$dblink->Database=C_DB_NAME;
	$dblink->connect();
	if ($dblink->Errno != 0){
		echo '数据库连接失败';
		return 0;
	} 
	$sql="select *  from ".C_APP_CONN_FILE." where connid=".$connectid;
	$dblink->Query($sql);
	list($connid,$appid,$filename)=$dblink->next_record();
	echo $filename;
	$dblink->close();
?>
</font>
</b><br>
<table border="2" cellspacing="1" bordercolor="#55847e" width="100%" style="border-collapse: collapse">
<tr bgcolor="#95bdbf">
	<th width=8%></th>
	<th width=%23><center>参数类型</center></th>
  	<th width=%23><center>参数名</center></th>
  	<th width=%23><center>证书项</center></th>
  	<th width=%20><center>参数格式</center></th>
 </tr>
<?php
 	$dblink=new DB;
	$dblink->Database=C_DB_NAME;
	$dblink->connect();
	if ($dblink->Errno != 0)
		die();
	$sql="select * from ".C_FILE_CONN_PARA." where connid=".$connectid;
	$dblink->Query($sql);
	$nr=$dblink->num_rows();
	$count=0;
	if ($nr != 0)
	{
		while (list($connid,$paratype,$paraname,$certitemtag,$paraformat)=$dblink->next_record())
		{
			
			echo "<tr><th><input type=\"checkbox\" name=\"check$count\" value=\"ON\"></th>";
			if($paratype!=0)
				echo "<th>证书相关参数</th>";
			else
				echo "<th>其他参数</th>";
			echo "<th>$paraname</th>";
			if($certitemtag==1)
				echo "<th>证书序列号</th>";
			else if($certitemtag==8)
				echo "<th>用户名</th>";
			else if($certitemtag==15)
				echo "<th>警官证号</th>";
			else if($certitemtag==121)
				echo "<th>用户组1</th>";
			else if($certitemtag==122)
				echo "<th>用户组2</th>";
			else if($certitemtag==123)
				echo "<th>用户组3</th>";
			else if($certitemtag==13)
				echo "<th>职务</th>";
			else if ($certitemtag==7)
				echo "<th>部门</th>";
			else if ($certitemtag==6)
				echo "<th>组织</th>";
			else if ($certitemtag==11)
				echo "<th>权限</th>";		
			else
				echo "<th>空</th>";
			if($paraformat==NULL)
				echo "<th>空</th></tr>";
			else
				echo "<th>$paraformat</th></tr>";
			$count=$count+1;
		}
	}
	$dblink->close();
?>
 </table><br>
 <input type="button" value="删除参数" name="del" onclick="delpara()">&nbsp;&nbsp;&nbsp;
<input type="button" value="修改参数" name="edit" onclick="updpara()">
<center>
<br><font face="楷体_GB2312">参数类型：&nbsp;</font> 
<select size="1" name="partype" style="position: relative; width: 120; height: 20" onchange="setface()">
<option value=1 selected>证书相关参数</option>
<option value=0>其他参数</option></select><br><font face="楷体_GB2312">
	参数名：&nbsp;&nbsp;&nbsp; </font><input type="TEXT" name="parname" style="position: relative; width: 120; height: 20"><br>
	<font face="楷体_GB2312">
	证书项：&nbsp;&nbsp;&nbsp; </font>
	<select size="1" name="certitem" style="position: relative; width: 120; height: 20" onchange="setface1()">
<option value=1 selected>证书序列号</option>
<option value=8>用户名</option>
<option value=15>警官证号</option>
<option value=121>用户组1</option>
<option value=122>用户组2</option>
<option value=123>用户组3</option>
<option value=6>组织</option>
<option value=7>部门</option>
<option value=13>职务</option>
<option value=11>权限</option>
</select><br>
	<font face="楷体_GB2312">参数格式：&nbsp;&nbsp;</font><input type="TEXT" name="parformat"  style="position: relative; width: 120; height: 20" disabled="true"><br>
	<input type="button" value="添加参数" name="add" onclick="addpara()">
	<input type="hidden" name="oprate" value="0">
<?php
	echo "<input type=\"hidden\" name=\"connectid\" value=$connectid>";
	echo "<input type=\"hidden\" name=\"appname2\" value=$appname1>";
	echo "<input type=\"hidden\" name=\"appid2\" value=$appid1>";
	echo "<p align=\"center\"><a href=\"setfile.php?appname=$appname1&appid=$appid1\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a></p>";
?>
</form>
</body>
<script language="javaScript">
function setface(){
	if(document.setpar.partype.value==0){
		document.setpar.certitem.disabled=true;
		document.setpar.parformat.value="";
		document.setpar.parformat.disabled=true;
	}else if(document.setpar.partype.value==1){
		document.setpar.certitem.disabled=false;
		if(document.setpar.certitem.value==15)
			document.setpar.parformat.disabled=false;
	}
}
function setface1(){
	if(document.setpar.partype.value==1)
		if(document.setpar.certitem.value==15){
			document.setpar.parformat.disabled=false;
		}else{
			document.setpar.parformat.disabled=true;
		}
}
function addpara(){
	if(document.setpar.parname.value=="")
		window.alert("参数名不能为空！");
	else if(document.setpar.partype.value==1 &&document.setpar.certitem.value==15&& document.setpar.parformat.value=="")
		window.alert("若证书项为“警官证号”时，参数格式不能为空！");
	else{
		document.setpar.oprate.value=1;
		document.setpar.submit();
	}
//	window.alert(document.control.numbtemp.value);
}
function delpara(){
	if(window.confirm("确定删除选择的参数吗？"))
	{
		document.setpar.oprate.value=2;
		document.setpar.submit();
	}else{
	}
//	window.alert(document.control.numbtemp.value);
}
function updpara(){
	document.setpar.action="updatepara.php";
	document.setpar.submit();
//	window.alert(document.control.numbtemp.value);
}
</script>
</html>