<?php
	include ("constant.php");
	@session_start();
	$records_each_pages = 15;
	if($manager_ID==NULL)
	{
		echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
		echo '<p><br><br><center><font face="楷体_GB2312"><b>请先登陆！</font></b><p>';
		echo '<a href="login.html"><img border="0" src="image/goback.gif" width="40" height="40"></a>';
		echo '</center></body></html>';
		exit();			
	}	
	if($numberu)
	{
		if( !IsUsbKeyUserExist($numberu))
		{
			if(!AosAddUsbKeyUser($usernameu,$departmentu,$numberu))
			{
				echo "<html><head><title>Key用户注册失败</title></head><body  background=\"image/pmiback.gif\"><b>";
				echo "<p><br><br><center><font face=\"楷体_GB2312\"><b>警官证号为 '$numberu' 的用户注册失败!</font></b><p>";
				echo "<a href=\"usbkey.php?numb=$numb\">";
		        echo "<img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a>";
		        echo "</center></body></html>";
		        exit();
			}
			// 添加Usbkey用户到系统内核内存中，执行CLI命令
			$cmd=MYSUDO.MYCLI."\\\"usbkey add $numberu\\\"";
	  		exec($cmd,$a);
		}	
		else
		{
			echo "<html><head><title>Key用户注册失败</title></head><body  background=\"image/pmiback.gif\"><b>";
			echo "<p><br><br><center><font face=\"楷体_GB2312\"><b>警官证号为 '$numberu' 的用户已经存在,<br>如果确实要注册此用户请先删除原用户!</font></b><p>";
			echo "<a href=\"usbkey.php?numb=$numb\">";
	        echo "<img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a>";
	        echo "</center></body></html>";
	        exit();
		}
	}
	if($numbtemp!=NULL)
	{
		$arr = array($check0,$check1,$check2,$check3,$check4,$check5,$check6,$check7,$check8,$check9,$check10,$check11,$check12,$check13,$check14);
		$usbkey_userstemp = GetUsbkeyUserList();
    	$nr = count($usbkey_userstemp);
		$a=0;
		$b=$numbtemp*$records_each_pages;
		$c=0;
		while ($a < $nr)
		{
			$a=$a+1;
			if ($a>$b-$records_each_pages && $a<=$b )
			{
				if($arr[$c]=="ON")
				{
					$user[$c]=$usbkey_userstemp[$a-1][1];
				}
				$c=$c+1;
			}
		}
		for($i=0; $i<$c; $i++)
		{
			if($user[$i]== NULL) continue;
			if(!AosUsbkeyDelUser($user[$i]))
			{
				echo "<html><head><title>Key用户注销失败</title></head><body  background=\"image/pmiback.gif\"><b>";
				echo "<p><br><br><center><font face=\"楷体_GB2312\"><b>警官证号为 '$numberu' 的用户注销失败!<br>";
				echo "此用户后续选择的用户也无法注销。</font></b><p>";
				echo "<a href=\"usbkey.php?numb=$numb\">";
		        echo "<img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a>";
		        echo "</center></body></html>";
		        exit();
			}
			// 从系统内核内存中删除，执行CLI命令
			$cmd=MYSUDO.MYCLI."\\\"usbkey remove $user[$i]\\\"";
	  		exec($cmd,$a);
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
	echo "<form name=\"usbkey\" method=\"POST\" action=\"usbkey.php?numb=1\">";
?>

<table border="0" cellspacing="1" width="100%" id="AutoNumber2" height="400">
  <tr>

    <td width="100%" height="380">
<br><img border="0" src="image/keyusehead.gif" width="283" height="57"><br>
<?php
	echo "<input type=\"hidden\" name=\"numbtemp\" value=\"$numb\">";
?>   
<table border="2" cellspacing="1" bordercolor="#55847e" width="100%" style="border-collapse: collapse">
<tr bgcolor="#95bdbf">
<td width="5%" height="20"></td>
<td width="28%" height="20"><center><b>用户名</b></center></td>
<td width="28%" height="20"><center><b>部门名称</b></center></td>
<td width="39%" height="20"><center><b>警官证号</b></center></td>
</tr>
<?php
	$usbkey_users = GetUsbkeyUserList();
    $nr = count($usbkey_users);
    
	//$records_each_pages = 15;
	if( $nr%$records_each_pages == 0 )
		$tmp = floor($nr/$records_each_pages);
	else
		$tmp = floor($nr/$records_each_pages)+1;
    if ($numb <= 0) $numb = 1;

    $aaa = ($numb-1)*$records_each_pages+1;	//该页的第一个记录的rid
    if (($numb*$records_each_pages) <= $nr)
    	$bbb=$numb*$records_each_pages;      //该页的最后一个记录的rid
    else
		$bbb=$nr;

    $rs=$bbb-$aaa;                   		//该页显示的记录数-1；
    $ri=0;
	if ($nr != 0)
	{
		$count = 0;
		while ($ri <= $rs)
		{
			$rid = $usbkey_users[$aaa-1][0];
	        $userid = $usbkey_users[$aaa-1][1];
	        $username = $usbkey_users[$aaa-1][2];
	        $department = $usbkey_users[$aaa-1][3];
			echo "<tr bgcolor=\"#eef4f4\"><th><input type=\"checkbox\" name=\"check$count\" value=\"ON\"></th><th>$username</th><th>$department</th><th>$userid</th></tr>";
			$count = $count+1;
			$aaa = $aaa+1;
		    $ri += 1;
		}
	}
	
?>

</table>
<br>
<input type="button" value="删除用户" name="usbkeydelb" onclick="usbkeydel()">
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
	 	echo "<a href=\"usbkey.php?numb=$temp\">上一页</a>&nbsp;&nbsp;";
	}else{
		echo "上一页&nbsp;&nbsp;";
	}
	if($nr>$numb*15)
	{
  		$temp=$numb+1;
	 	echo "<a href=\"usbkey.php?numb=$temp\">下一页</a></font></b>";
	}else{
		echo "下一页</font></b>";
	}
 ?>
 
 	<p align="center"><input type="button" value="Key用户注册" name="usbkeyregb" onclick="usbkeyreg()"></p>
    </td>
  </tr>
</table>
</form>

</body>
<script language="javaScript">
function usbkeydel()
{
	if(window.confirm("确定删除所选择的Key用户吗？"))
	{
		document.usbkey.action="usbkey.php?numb=1";
		document.usbkey.submit();
	}
}

function usbkeyreg()
{
	document.usbkey.action="usbkeyreg.php";
	document.usbkey.submit();
}
</script>
</html>