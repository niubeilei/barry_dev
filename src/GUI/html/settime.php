<?php
include "constant.php";
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
<meta name="GENERATOR" content="Microsoft FrontPage 5.0">
<meta name="ProgId" content="FrontPage.Editor.Document">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>新建网页 1</title>
</head>

<body  background="image/pmiback.gif">
<form name="sett" method="POST" action="--WEBBOT-SELF--">
 <table border="0" cellspacing="0" width="100%" id="AutoNumber2" height="44" bgcolor="#eef4f4">
  <tr>
    <td width="45%" height="39">
<img border="0" src="image/date1.gif" width="283" height="57"><br>
  <b><font face="隶书" size="4">日期</font></b>
  <br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face="楷体_GB2312"><select size="1" name="yearlist">
<?php
	$year=date("Y");
	$month=date("n");
	$day=date("d");
	$tmp=1900;
	for($tmp;$tmp<2101;$tmp++)
	{
		if($tmp!=$year)
			echo "<option  value=\"".$tmp."\">".$tmp."</option>";
		else
			echo "<option selected  value=\"".$tmp."\">".$tmp."</option>";
	}
	echo "</select>年";
	echo "<select size=\"1\" name=\"monthlist\">";
	$tmp=1;
	for($tmp;$tmp<13;$tmp++)
	{
		if($tmp!=$month)
			echo "<option value=\"".$tmp."\">".$tmp."</option>";
		else
			echo "<option selected  value=\"".$tmp."\">".$tmp."</option>";
	}
	echo "</select>月";
	echo "<select size=\"1\" name=\"daylist\">";
	$tmp=1;
	for($tmp;$tmp<32;$tmp++)
	{
		if($tmp!=$day)
			echo "<option value=\"".$tmp."\">".$tmp."</option>";
		else
			echo "<option selected  value=\"".$tmp."\">".$tmp."</option>";
	}
	echo "</select>日";
	
//	echo "<param name=\"Month\" value=\"".$month."\">";
//	echo "<param name=\"Day\" value=\"".$day."\">";
?>
</font>
 <p><b><font face="隶书" size="4">时间</font></b>
　<br>
  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face="楷体_GB2312"><select size="1" name="hourlist">
  <?php
  $hour=date("H");
  for($i=0;$i<24;$i++){
  	if($i==$hour ){
  		if($i<10)
  			echo "<option selected value=\"0".$i."\">"."0".$i."</option>";
  		else
  			echo "<option selected value=\"$i\">$i</option>";
  	}else if($i<10){
  		
  		echo "<option value=\"0".$i."\">"."0".$i."</option>";
  	}else
  		echo "<option value=\"$i\">$i</option>";
  	
  }
  ?>
  </select>时<select size="1" name="minulist">
   <?php
   $minu=date("i");
  for($i=0;$i<60;$i++){
  	if($i==$minu ){
  		if($i<10)
  			echo "<option selected value=\"0".$i."\">"."0".$i."</option>";
  		else
  			echo "<option selected value=\"$i\">$i</option>";
  	}else if($i<10){
  		
  		echo "<option value=\"0".$i."\">"."0".$i."</option>";
  	}else
  		echo "<option value=\"$i\">$i</option>";
  }
  ?>
  
  </select>分</font>
  <p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
  <input type="button" value="修改系统时间" name="B1" onclick="settime()"></p>
  <INPUT type=hidden value="" name=timetemp>
  </td><td width="55%" height="39">
 <img border="0" src="image/date2.gif" width="283" height="57"><br><br>
 
    <p align="left">
    
<?php
	/*
	$cmd=SUDO."cp /var/spool/cron/root /tmp/rootemp";
	$ret=exec($cmd,$a);
	$cmd=SUDO."chmod 777 /tmp/rootemp";
	$ret=exec($cmd,$a);
	*/
		
		$fp=file(CRONTAB);
		if ($fp ==false || strlen($fp[0])<10 )
		{
			//echo "文件找不到或文件打开失败，请检查系统！<br>";
			echo "<input type=\"radio\" name=\"rool\" value=\"norool\" checked><b><font face=\"楷体_GB2312\">不循环启动</p>";
			echo "<p align=\"left\">";
			echo " <input type=\"radio\" name=\"rool\" value=\"yesrool\">循环启动<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
			
			echo "<br> 每日服务器重启时间：</font></b><select size=\"1\" name=\"roolhour\">";
			echo "<option selected value=\"00\">00</option>";
			for($i=1;$i<24;$i++)
			{
				if($i<10)
					echo "<option value=\"0".$i."\">"."0".$i."</option>";
				else
					echo "<option value=\"$i\">$i</option>";
			}
			echo " </select>时<select size=\"1\" name=\"roolminu\">";
			echo "<option selected value=\"00\">00</option>";
			for($i=1;$i<60;$i++)
			{
				if($i<10)
					echo "<option value=\"0".$i."\">"."0".$i."</option>";
				else
					echo "<option value=\"$i\">$i</option>";
			}
		}else{
			$line1=$fp[0];
		    list($minu,$hour,$a,$b,$c,$r,$cronshel)=split(" ",$line1,10);
			    echo "<input type=\"radio\" name=\"rool\" value=\"norool\" ><b><font face=\"楷体_GB2312\">不循环启动</p>";
			    echo "<p align=\"left\">";
			    echo " <input type=\"radio\" name=\"rool\" value=\"yesrool\" checked>循环启动<br><br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
			
			echo "<b><font face=\"楷体_GB2312\"> 每日服务器重启时间：</font></b><select size=\"1\" name=\"roolhour\">";
			for($i=0;$i<24;$i++){
				if($i==$hour ){
			  		if($i<10)
			  			echo "<option selected value=\"0".$i."\">"."0".$i."</option>";
			  		else
			  			echo "<option selected value=\"$i\">$i</option>";
			  	}else if($i<10){
			  		
			  		echo "<option value=\"0".$i."\">"."0".$i."</option>";
			  	}else
			  		echo "<option value=\"$i\">$i</option>";
			  	
			  }
			echo " </select>时<select size=\"1\" name=\"roolminu\">";
			  for($i=0;$i<60;$i++){
			  	if($i==$minu ){
			  		if($i<10)
			  			echo "<option selected value=\"0".$i."\">"."0".$i."</option>";
			  		else
			  			echo "<option selected value=\"$i\">$i</option>";
			  	}else if($i<10){
			  		
			  		echo "<option value=\"0".$i."\">"."0".$i."</option>";
			  	}else
			  		echo "<option value=\"$i\">$i</option>";
			  }
		}
?>
  
  </select>分</p>
  <p align="center"><input type="button" value="确 定" name="B1" onclick="setrool()"></p>
    </td>
  </tr>
  </table>

</form>
</body>
<script language="javaScript">
function settime()
{
	if(window.confirm("服务器系统时间将被修改"))
	{
//		var timetem=document.sett.Calendar1.value;
//		document.sett.timetemp.value=timetem;
		document.sett.action="setsystemtime.php";
		document.sett.submit();
	}else{
	}

}
function setrool()
{
	if(document.sett.rool[0].checked)
	{
		if(window.confirm("服务器将取消循环启动"))
		{
			document.sett.action="setreboottime.php?opnumb=2";
			document.sett.submit();
		}else{
		}
	}else if(document.sett.rool[1].checked){
		if(window.confirm("服务器将设置为循环启动"))
		{
			document.sett.action="setreboottime.php?opnumb=1";
			document.sett.submit();
		}else{
		}
	}
	//document.sett.action="setreboottime.php?opnumb=1";
	//document.sett.submit();
}
</script>
</html>
