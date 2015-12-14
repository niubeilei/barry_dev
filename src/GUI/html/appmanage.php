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


<meta name="GENERATOR" content="Microsoft FrontPage 5.0">
<meta name="ProgId" content="FrontPage.Editor.Document">
<!--
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
-->
<title>应用服务信息列表</title>
</head>

<body bgcolor="#eef4f4">

 <form method="POST" action="appmanage.php">   

<table border="0" cellspacing="1" width="100%" id="AutoNumber2" height="480">
  <tr>

    <td width="100%" height="480"><img border="0" src="image/apphead.gif" width="283" height="57"><br>

<table border="2" cellspacing="1" bordercolor="#55847e" width="100%" style="border-collapse: collapse">

<tr bgcolor="#95bdbf">
<td width="15%" height="20"><center><b>应用服务名称</center></b></td>
<td width="10%" height="20"><center><b>代理协议名称</center></b></td>
<td width="10%" height="20"><center><b>服务端口</center></b></td>
<td width="10%" height="20"><center><b>证书验证设置</center></b></td>
<td width="15%" height="20"><center><b>后台服务器IP地址</center></b></td>
<td width="10%" height="20"><center><b>后台服务端口号</center></b></td>
<td width="30%" height="20"><center><b>加入时间</center></b></td>
</tr>
<?php
	include "pmi_db.php";
	$dblink=new DB;
	$dblink->Database=C_DB_NAME;
	$dblink->connect();
	if ($dblink->Errno != 0)
		die();
	$sql="SELECT * FROM ".C_APP_WJ_TBL;
	$dblink->Query($sql);
	$nr=$dblink->num_rows();
	$aaa=0;
	$bbb=$numb*15;
	if ($nr != 0)
	{
		while (list($appid,$appname,$lip,$pname,$lport,$vmode,$sip,$sport,$joindate,$isdeleted)=$dblink->next_record())
		{
//			settype($sip,"integer");
			$temp=$sip-0x80000000;
			if($temp<0){
				$B4=($sip&0xff);
				$B3=(($sip>>8)&0xff);
				$B2=(($sip>>16)&0xff);
				$B1=(($sip>>24)&0xff);
			}else{
				$B4=($temp&0xff);
				$B3=(($temp>>8)&0xff);
				$B2=(($temp>>16)&0xff);
				$B1=(($temp>>24)&0xff)+0x80;
			}
			$ip_str=$B4.".".$B3.".".$B2.".".$B1;
//			$ip_str=(($sip&0xff000000)>>24).".".(($sip&0xff0000)>>16).".".(($sip&0xff00)>>8).".".(($sip&0xff));
			$aaa=$aaa+1;
			if ($isdeleted != 1 && $aaa>$bbb-15 && $aaa<=$bbb)
				echo "<tr><th>$appname</th><th>$pname</th><th>$lport</th><th>$vmode</th><th>$ip_str</th><th>$sport</th><th>".date("Y/m/d--H:i:s",$joindate)."</th></tr>";
		}
		$dblink->clean_results();
	}
?>

</table>
<br>
<font face="楷体_GB2312"  color="#006699"><b>&nbsp;&nbsp;&nbsp;页次:
   <?php  
   	
   	if($nr%15>7)	
		$tmp=round($nr/15);
	else
		$tmp=round($nr/15)+1;
 
 	print("$numb/$tmp&nbsp;&nbsp;&nbsp;每页15条&nbsp;&nbsp;&nbsp;记录数:$nr&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;".
 	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font><font face=\"楷体_GB2312\">");

   	if($numb>1)
   	{	
  		$temp=$numb-1;
	 	echo "<a href=\"appmanage.php?numb=$temp\">上一页</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	}else{
		echo "上一页&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	}
	if($nr>$numb*15)
	{
  		$temp=$numb+1;
	 	echo "<a href=\"appmanage.php?numb=$temp\">下一页</a></font></b>";
	}else{
		echo "下一页</font></b>";
	}
 ?>

    </td>
  </tr>
</table>
</form>

</body>

</html>
