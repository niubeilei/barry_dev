<?php
	include ("constant.php");
	include("mysql.lib.php");
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/backgroud.gif"><b>' ;
	    echo '请先登陆！<p>';
	    echo '<a href="login.html">返回</a>';
            echo '</b></body></html>';
            exit();			
	}
	$dblink=new DB;
	$dblink->Database=C_DB_NAME;

//	echo $passwd."<br>";
	if ($passwd != $passwd2)
	{
		echo "对不起，两次输入的警官证号不相同，请检查！";
		exit();
	}
	$slen=strlen($passwd);
	if ($slen > 18)
	{
		echo "对不起，警官证号长度超过18字节，请检查！";
		exit();
	}
	else
	{
		for ($i=0;$i<(18-$slen);$i++)
			$passwd="0".$passwd;
	}
//	echo $passwd."<br>";
	
	if ($opcode == 1)
	{
		$dblink->query("SELECT COUNT(*) FROM ".C_MNG_TBL." WHERE policenum='".addslashes($passwd)."' AND isdeleted!=1");
		list($count) = $dblink->next_record();
		$dblink->clean_results();
		if ($count != 0)
		{
			echo $count;
			echo "请注意：警官".$passwd."已经被授权为管理员！";
		}
		else
		{
			$dblink->query("INSERT INTO ".C_MNG_TBL." (name,policenum,joindate,isdeleted) VALUES ('".addslashes($username)."','".addslashes($passwd)."','".time()."',0)");
			if ($dblink->Errno == 0)
				echo "警官".$passwd."被成功授权为管理员！";
			else
				echo "警官".$passwd."授权为管理员失败！";
		}
	}

	if ($opcode == 2)
	{
		$dblink->query("SELECT COUNT(*) FROM ".C_MNG_TBL." WHERE policenum='".addslashes($passwd)."' AND isdeleted!=1");
		list($count) = $dblink->next_record();
		$dblink->clean_results();
		if ($count == 0)
		{
			echo "请注意：警官".$passwd."还没有被授权为管理员，不能修改资料！";
		}
		else
		{
			$dblink->query("UPDATE ".C_MNG_TBL." SET name='".addslashes($username)."',policenum='".addslashes($passwd)."',joindate='".time()."',isdeleted=0 WHERE policenum='".addslashes($passwd)."' AND isdeleted!=1");
			if ($dblink->Errno == 0)
				echo "警官".$passwd."的资料被成功修改！";
			else
				echo "警官".$passwd."的资料修改不成功";
		}
	}

	if ($opcode == 3)
	{
		$dblink->query("SELECT COUNT(*) FROM ".C_MNG_TBL." WHERE isdeleted!=1");
		list($count) = $dblink->next_record();
		$dblink->clean_results();
		$act=1;
		if ($count < 2)
		{
			echo "请首先加入一名管理员！（必须至少保留一名管理员！）";
			$act=0;
		}
		else
		{
			$dblink->query("DELETE FROM ".C_MNG_TBL." WHERE policenum='".addslashes($passwd)."' AND isdeleted!=1");
			if ($dblink->Errno != 0)
				$act=0;
		}
		if ($act != 0)
			echo "将警官".$passwd."清除出管理员列表已经成功！";
		else
			echo "将警官".$passwd."清除出管理员列表没有成功";
	}
?>