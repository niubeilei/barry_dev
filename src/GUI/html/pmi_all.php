<?php
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/backgroud.gif"><b>' ;
	    echo '请先登陆！<p>';
	    echo '<a href="login.html">返回</a>';
            echo '</b></body></html>';
            exit();			
	}
	
?>

<html>
<?php
echo "appid=$appid";
?>
<form name="pmi_all_set" method="post"><br>
<font face="楷体_GB2312">单位名称&nbsp;&nbsp;&nbsp&nbsp;&nbsp;&nbsp&nbsp;</font> 
<?php
if (file_exists("/usr/SSLMPS/Config/".$appid.".DepName"))
{
	$fp=fopen("/usr/SSLMPS/Config/".$appid.".DepName","r");
	if ($fp)
		$dname=fread($fp,256);
	else
		$dname="未设置";
}
else
	$dname="未设置";
echo "<input type=\"TEXT\" name=\"depname\" value=\"$dname\" size=%80><br>"
?>
<font face="楷体_GB2312">应用系统名称&nbsp;&nbsp;&nbsp;</font>
<?php
if (file_exists("/usr/SSLMPS/Config/".$appid.".AppName"))
{
	$fp=fopen("/usr/SSLMPS/Config/".$appid.".AppName","r");
	if ($fp)
		$aname=fread($fp,256);
	else
		$aname="未设置";
}
else
	$aname="未设置";
echo "<input type=\"TEXT\" name=\"appname\" value=\"$aname\" size=%80><br>";
echo "&nbsp;&nbsp;&nbsp&nbsp;&nbsp;&nbsp";
echo "<input type=\"submit\" value=\"设置\" name=\"add\" onclick=\"pmi_all_set.action='pmi_all_set.php?appid=$appid&op=0'\">";
echo "&nbsp;&nbsp;&nbsp&nbsp;&nbsp;&nbsp";
echo "<input type=\"submit\" value=\"撤销\" name=\"del\" onclick=\"pmi_all_set.action='pmi_all_set.php?appid=$appid&op=1'\">";
?>
</form>

</html>
