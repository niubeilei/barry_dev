<?php
	include ("pmi_db.php");
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '<p><br><br><center><font face="楷体_GB2312"><b>请先登陆！</font></b><p>';
	    echo '<a href="login.html"><img border="0" src="image/goback.gif" width="40" height="40"></a>';
            echo '</center></body></html>';
            exit();			
	}	
	if($numbtemp!=NULL)
	{
		$arr = array($check0,$check1,$check2,$check3,$check4,$check5,$check6,$check7,$check8,$check9,$check10,$check11,$check12,$check13,$check14);
		$dblink1=new DB;
		$dblink1->Database=C_DB_NAME;
		$dblink1->connect();
		if ($dblink->Errno != 0){
			echo '数据库连接失败';
			return 0;
		} 
		$sql ="SELECT * FROM UserTable";			
		$dblink1->Query($sql);
		$nr=$dblink1->num_rows();
		$a=0;
		$b=$numbtemp*15;
		$c=0;
		if ($nr != 0)
		{
			while (list($userid1,$username1,$department1,$number1)=$dblink1->next_record())
			{
				$a=$a+1;
				if ($a>$b-15 && $a<=$b ){
					if($arr[$c]=="ON")
					{
						$user[$c]=$userid1;
					}
					$c=$c+1;
				}
			}
		}
	//	$dblink1->close();
		$dblink1->Database=C_DB_NAME;
		$dblink1->connect();
		if ($dblink->Errno != 0){
			echo '数据库连接失败';
			return 0;
		} 
		for($i=0;$i<$c;$i++)
			if($user[$i]!=NULL){
				$sql="delete from UserTable where userid=".$user[$i];
				$dblink1->Query($sql);
				$sql="delete from ".C_SERVER_PMI_TBL." where userid=".$user[$i];
				$dblink1->Query($sql);
		//		del_user($user[$i]);
			}
		$dblink1->close();
	}
	if($usernameu)
	{
		if($numberu)
		{
			add_user($usernameu,$departmentu,$numberu);
		}
	}
	
 ?>
<html>

<head>
<meta name="GENERATOR" content="Microsoft FrontPage 5.0">
<meta name="ProgId" content="FrontPage.Editor.Document">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>用户信息</title>
</head>

<body bgcolor="#eef4f4">
<?php
	echo "<form name=\"use\" method=\"POST\" action=\"usermess.php?numb=1\">";
?>

<table border="0" cellspacing="1" width="100%" id="AutoNumber2" height="400">
  <tr>

    <td width="100%" height="380">
<br><img border="0" src="image/usermeshead.gif" width="283" height="57"><br>
<?php
	echo "<input type=\"hidden\" name=\"numbtemp\" value=".$numb.">";
?>   
<table border="2" cellspacing="1" bordercolor="#55847e" width="100%" style="border-collapse: collapse">
<tr bgcolor="#95bdbf">
<td width="5%" height="20"></td>
<td width="28%" height="20"><center><b>用户名</b></center></td>
<td width="28%" height="20"><center><b>部门名称</b></center></td>
<td width="39%" height="20"><center><b>警官证号</b></center></td>
</tr>
<?php
	$dblink=new DB;
	$dblink->Database=C_DB_NAME;
	$dblink->connect();
	if ($dblink->Errno != 0)
		die();
	$sql="SELECT * FROM UserTable";
	$dblink->Query($sql);
	$nr=$dblink->num_rows();
	$aaa=0;
	$bbb=$numb*15;
	$count=0;
	if ($nr != 0)
	{
	while (list($userid,$username,$department,$number)=$dblink->next_record())
		{
			if($department==NULL)
				$department="空";
			$aaa=$aaa+1;
			if ($aaa>$bbb-15 && $aaa<=$bbb)
			{
				echo "<tr bgcolor=\"#eef4f4\"><th><input type=\"checkbox\" name=\"check$count\" value=\"ON\"></th><th>$username</th><th>$department</th><th>$number</th></tr>";
				$count=$count+1;
			}
		}
		$dblink->clean_results();
	}
?>

</table>
<br>
<input type="button" value="删除用户" name="logout" onclick="set()">
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;

<font face="楷体_GB2312"  color="#006699"><b>&nbsp;&nbsp;&nbsp;页次:
   <?php 
   	if($nr%15>7)	
		$tmp=round($nr/15);
	else
		$tmp=round($nr/15)+1;
 
 	print("$numb/$tmp&nbsp;&nbsp;&nbsp;每页15条&nbsp;&nbsp;&nbsp;记录数:$nr".
 	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font><font face=\"楷体_GB2312\" color=\"#C0C0C0\">");
    
   	if($numb>1)
   	{	
  		$temp=$numb-1;
	 	echo "<a href=\"usermess.php?numb=$temp\">上一页</a>&nbsp;&nbsp;";
	}else{
		echo "上一页&nbsp;&nbsp;";
	}
	if($nr>$numb*15)
	{
  		$temp=$numb+1;
	 	echo "<a href=\"usermess.php?numb=$temp\">下一页</a></font></b>";
	}else{
		echo "下一页</font></b>";
	}
 ?>
	<p align="center"><input type="button" value="新用户注册" name="reg" onclick="reguser()"></p>
    </td>
  </tr>


</table>
</form>

</body>
<script language="javaScript">
function set(){
	if(window.confirm("若删除此用户，则该用户相应的访问控制规\n则也将一同删除，确定删除此用户吗？"))
	{
		document.use.submit();
	}else{
	}
//	window.alert(document.control.numbtemp.value);
}
function reguser(){
	document.use.action="user.php";
	document.use.submit();
}
</script>
</html>