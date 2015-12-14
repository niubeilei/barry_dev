<?
include "pmi_db.php";
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  bgcolor="#eef4f4"><b>' ;
	    echo '请先登陆！<p>';
	    echo '<a href="login.html">返回</a>';
            echo '</b></body></html>';
            exit();			
	}
	$dblink=new DB;
	$dblink->Database=C_DB_NAME;
	$dblink->connect();
	if ($dblink->Errno != 0)
		die();
	$sql="select * from ".C_APP_WJ_TBL." where appname='".$appnamex."'";
	$dblink->Query($sql);
	$nr=$dblink->num_rows();
	if ($nr != 0)
	{
		list($appid,$appname,$listenip,$protocolname,$listenport,$verifymode,$serverip,$serverport,$joindate,$isdeleted)=$dblink->next_record();
		$temp=$serverip-0x80000000;
		if($temp<0){
			$B4=($serverip&0xff);
			$B3=(($serverip>>8)&0xff);
			$B2=(($serverip>>16)&0xff);
			$B1=(($serverip>>24)&0xff);
		}else{
			$B4=($temp&0xff);
			$B3=(($temp>>8)&0xff);
			$B2=(($temp>>16)&0xff);
			$B1=(($temp>>24)&0xff)+0x80;
		}
		$ip_str=$B4.".".$B3.".".$B2.".".$B1;

		echo '<html><head><title>应用服务信息</title></head><body  background="image/pmiback.gif">' ;
		echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face=\"楷体_GB2312\">应用服务ID：&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font><font face=\"楷体_GB2312\" color=#0000ff>$appid</font></p>";
		echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face=\"楷体_GB2312\">应用服务名称：&nbsp;&nbsp;&nbsp;&nbsp;</font><font face=\"楷体_GB2312\" color=#0000ff>$appname</font></p>";
		echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face=\"楷体_GB2312\">监听IP地址：&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font><font face=\"楷体_GB2312\" color=#0000ff>$listenip</font></p>";
		echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face=\"楷体_GB2312\">代理协议名称：&nbsp;&nbsp;&nbsp;&nbsp;</font><font face=\"楷体_GB2312\" color=#0000ff>$protocolname</font></p>";
		echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face=\"楷体_GB2312\">监听端口：&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font><font face=\"楷体_GB2312\" color=#0000ff>$listenport</font></p>";
		echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face=\"楷体_GB2312\">证书验证设置：&nbsp;&nbsp;&nbsp;&nbsp;</font><font face=\"楷体_GB2312\" color=#0000ff>$verifymode</font></p>";
		echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face=\"楷体_GB2312\">后台服务器IP地址：</font><font face=\"楷体_GB2312\" color=#0000ff>$ip_str</font></p>";
		echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face=\"楷体_GB2312\">后台服务端口：&nbsp;&nbsp;&nbsp;&nbsp;</font><font face=\"楷体_GB2312\" color=#0000ff>$serverport</font></p>";
		echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face=\"楷体_GB2312\">加入时间：&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font><font face=\"楷体_GB2312\" color=#0000ff>".date("Y/m/d--H:i:s",$joindate)."</font></p>";
		if($isdeleted==0)
			echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face=\"楷体_GB2312\">删除否：&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font><font face=\"楷体_GB2312\" color=#0000ff>否</font></p>";
		echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"pmirole.php?appid=$appid&rolenumb=1\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a></p>";
            echo '</body></html>';
        }
	$dblink->close();
?>
		
