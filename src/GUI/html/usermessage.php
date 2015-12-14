<?
include "pmi_db.php";
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '<p><br><br><center><font face="楷体_GB2312"><b>请先登陆！</font></b><p>';
	    echo '<a href="login.html"><img border="0" src="image/goback.gif" width="40" height="40"></a>';
            echo '</center></body></html>';
            exit();			
	}	$dblink=new DB;
	$dblink->Database=C_DB_NAME;
	$dblink->connect();
	if ($dblink->Errno != 0)
		die();
//	$sql="select * from ".C_APP_WJ_TBL." where appname='".$appnamex."'";
//	$dblink->Query($sql);
	$sql="select * from ".C_USER_WJ_TBL." where userid='".$usernamex."'";
	$dblink->Query($sql);
	$nr=$dblink->num_rows();
	if ($nr != 0)
	{
		list($userid,$username,$department,$number,$isdeleted)=$dblink->next_record();
		echo '<html><head><title>用户信息</title></head><body  background="image/pmiback.gif">' ;
		echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face=\"楷体_GB2312\">用户ID：&nbsp;&nbsp;</font><font face=\"楷体_GB2312\" color=#0000ff>$userid</font></p>";
		echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face=\"楷体_GB2312\">用户名：&nbsp;&nbsp;</font><font face=\"楷体_GB2312\" color=#0000ff>$username</font></p>";
		echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face=\"楷体_GB2312\">部&nbsp;&nbsp;门：&nbsp;&nbsp;</font><font face=\"楷体_GB2312\" color=#0000ff>$department</font></p>";
		echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face=\"楷体_GB2312\">警官证号：</font><font face=\"楷体_GB2312\" color=#0000ff>$number</font></p>";
		if($isdeleted==0)
			echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font face=\"楷体_GB2312\">删除否：&nbsp;&nbsp;</font><font face=\"楷体_GB2312\" color=#0000ff>否</font></p>";
		echo "<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"pmi1.php?appid=$appid&numb=1\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a></p>";
            echo '</body></html>';
        }
	$dblink->close();
?>
		
