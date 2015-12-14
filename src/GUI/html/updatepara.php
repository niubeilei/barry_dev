<?php
	include("constant.php");
	include("mysql.lib.php");
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '<p><br><br><center><font face="楷体_GB2312"><b>请先登陆！</font></b><p>';
	    echo '<a href="login.html"><img border="0" src="image/goback.gif" width="40" height="40"></a>';
            echo '</center></body></html>';
            exit();			
	}
	$arr = array($check0,$check1,$check2,$check3,$check4,$check5,$check6,$check7,$check8,$check9);
	for($i=0;$i<10;$i++)
		if($arr[$i]=="ON")
			break;
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
		while (list($connid1,$paratype1,$paraname1,$certitem1,$paraform1)=$dblink1->next_record())
		{
			if($c==$i)
				break;
			$c=$c+1;
		}
			
	}
	print("<html><head><title>修改参数信息</title></head><body  background=\"image/pmiback.gif\">");
	print("<form name=\"update\" method=\"post\">");
	print("<center>	<br><br><font face=\"楷体_GB2312\"><b>参数类型:&nbsp;&nbsp;</b></font>");
	print("<select size=\"1\" name=\"partype1\" style=\"position: relative; width: 120; height: 20\"  onchange=\"setface()\">");
	if($paratype1==1)
		print("<option value=1 selected>证书相关参数</option><option value=0>其他参数</option></select><br>");
	else
		print("<option value=1>证书相关参数</option><option value=0 selected>其他参数</option></select><br>");	
		
	echo "<font face=\"楷体_GB2312\"><b>参数名：&nbsp;&nbsp; <input type=\"TEXT\" name=\"parname1\" style=\"position: relative; width: 120; height: 20\"  value=$paraname1 size=18></b></font><br>";
	echo "<font face=\"楷体_GB2312\"><b>证书项：&nbsp;&nbsp; </font>";
	if($paratype1==1)
		echo "<select size=\"1\" name=\"ceritem1\" style=\"position: relative; width: 120; height: 20\"  onchange=\"setface1()\">";
	else if($paratype1==0)
		echo "<select size=\"1\" name=\"ceritem1\" style=\"position: relative; width: 120; height: 20\"  onchange=\"setface1()\" disabled=\"true\">";
	if($certitem1==1 ||$certitem1==NULL)
		echo "<option value=1 selected>证书序列号</option><option value=8>用户名</option><option value=15>警官证号</option><option value=121>用户组1</option><option value=122>用户组2</option><option value=123>用户组3</option><option value=13>职务</option><option value=7>部门</option><option value=6>组织</option><option value=11>权限</option></select><br>";
	else if($certitem1==8)
		echo "<option value=1>证书序列号</option><option value=8 selected>用户名</option><option value=15>警官证号</option><option value=121>用户组1</option><option value=122>用户组2</option><option value=123>用户组3</option><option value=13>职务</option><option value=7>部门</option><option value=6>组织</option><option value=11>权限</option></select><br>";
	else if($certitem1==15)
		echo "<option value=1>证书序列号</option><option value=8>用户名</option><option value=15 selected>警官证号</option><option value=121>用户组1</option><option value=122>用户组2</option><option value=123>用户组3</option><option value=13>职务</option><option value=7>部门</option><option value=6>组织</option><option value=11>权限</option></select><br>";
	else if($certitem1==121)
		echo "<option value=1>证书序列号</option><option value=8>用户名</option><option value=15 >警官证号</option><option value=121 selected>用户组1</option><option value=122>用户组2</option><option value=123>用户组3</option><option value=13>职务</option><option value=7>部门</option><option value=6>组织</option><option value=11>权限</option></select><br>";
	else if($certitem1==122)
		echo "<option value=1>证书序列号</option><option value=8>用户名</option><option value=15 >警官证号</option><option value=121>用户组1</option><option value=122 selected>用户组2</option><option value=123>用户组3</option><option value=13>职务</option><option value=7>部门</option><option value=6>组织</option><option value=11>权限</option></select><br>";
	else if($certitem1==123)
		echo "<option value=1>证书序列号</option><option value=8>用户名</option><option value=15 >警官证号</option><option value=121>用户组1</option><option value=122>用户组2</option><option value=123 selected>用户组3</option><option value=13>职务</option><option value=7>部门</option><option value=6>组织</option><option value=11>权限</option></select><br>";
	else if($certitem1==13)
		echo "<option value=1>证书序列号</option><option value=8>用户名</option><option value=15 >警官证号</option><option value=121>用户组1</option><option value=122>用户组2</option><option value=123>用户组3</option><option value=13 selected>职务</option><option value=7>部门</option><option value=6>组织</option><option value=11>权限</option></select><br>";
	else if($certitem1==7)
		echo "<option value=1>证书序列号</option><option value=8>用户名</option><option value=15 >警官证号</option><option value=121>用户组1</option><option value=122>用户组2</option><option value=123>用户组3</option><option value=13>职务</option><option value=7 selected>部门</option><option value=6>组织</option><option value=11>权限</option></select><br>";
	else if($certitem1==6)
		echo "<option value=1>证书序列号</option><option value=8>用户名</option><option value=15 >警官证号</option><option value=121>用户组1</option><option value=122>用户组2</option><option value=123>用户组3</option><option value=13>职务</option><option value=7>部门</option><option value=6 selected>组织</option><option value=11>权限</option></select><br>";
	else if($certitem1==11)
		echo "<option value=1>证书序列号</option><option value=8>用户名</option><option value=15 >警官证号</option><option value=121>用户组1</option><option value=122>用户组2</option><option value=123>用户组3</option><option value=13>职务</option><option value=7>部门</option><option value=6>组织</option><option value=11 selected>权限</option></select><br>";
	echo "</b><font face=\"楷体_GB2312\"><b>参数格式:&nbsp;&nbsp;</font>";
	
	
	if($paratype1==1&&$certitem1==15)
		echo "<input type=\"TEXT\" name=\"parformat1\" style=\"position: relative; width: 120; height: 20\"  value=$paraform1 size=18><br>";
	else if($certitem1!=15)
		echo "<input type=\"TEXT\" name=\"parformat1\" style=\"position: relative; width: 120; height: 20\"  value=\"空\" size=18 disabled=\"true\"><br>";
	echo "</b><br><input type=\"button\" value=\"修改参数\" name=\"add\" onclick=\"conupd()\">";
	echo "&nbsp;&nbsp;&nbsp;&nbsp;<input type=\"button\" value=\"取消修改\" name=\"add\" onclick=\"upd()\">";
	echo "<input type=\"hidden\" name=connectid value=".$connectid.">";
	echo "<input type=\"hidden\" name=oprate value=\"0\">";
	echo "<input type=\"hidden\" name=oldparaname value=\"".$paraname1."\">";
	$dblink=new DB;
	$dblink->Database=C_DB_NAME;
	$dblink->connect();
	if ($dblink->Errno != 0){
		echo '数据库连接失败';
		return 0;
	} 
	$sql="select *  from ".C_APP_CONN_FILE." where connid=".$connectid;
	$dblink->Query($sql);
	list($cid,$aid,$fn)=$dblink->next_record();
	$sql="select * from ".C_APP_WJ_TBL." where appid=".$aid;
	$dblink->Query($sql);
	list($id,$aname,$lip,$pname,$lport,$vmode,$sip,$sport,$joindate,$isdeleted)=$dblink->next_record();
	$dblink->close();
	echo "<input type=\"hidden\" name=appname1 value='".$aname."'>";
	echo "<input type=\"hidden\" name=appid1 value='".$appid2."'>";
	
?>
</center></form></body>
<script language="javaScript">
function setface(){
	if(document.update.partype1.value==0){
		document.update.ceritem1.disabled=true;
		document.update.parformat1.value="";
		document.update.parformat1.disabled=true;
	}else if(document.update.partype1.value==1){
		document.update.ceritem1.disabled=false;
		if(document.update.ceritem1.value==15)
			document.update.parformat1.disabled=false;
	}
}
function setface1(){
	if(document.update.partype1.value==1)
		if(document.update.ceritem1.value==15){
			document.update.parformat1.disabled=false;
		}else{
			document.update.parformat1.disabled=true;
		}
}
function upd(){
	document.update.action="setpara.php";
	document.update.submit();
//	window.alert(document.control.numbtemp.value);
}
function conupd(){
	if(document.update.parname1.value=="")
		window.alert("参数名不能为空！");
	else if(document.update.partype1.value==1&&document.update.ceritem1.value==15 && document.update.parformat1.value=="空")
		window.alert("若证书项为“警官证号”时，参数格式不能为空！");
	else{
		document.update.oprate.value=3;
		document.update.action="setpara.php";
		document.update.submit();
	}
}
</script>
</html>