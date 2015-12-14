<?php
	include("constant.php");
	include("mysql.lib.php");

function MakeWjManagerTable($TableName)
{
	// 开始生成数据表
	// ###################### 管理员 #######################
	$dblink=new DB;
	$dblink->Database=C_DB_NAME;
	if ($TableName == "" || $TableName == C_MANAGER_WJ_TBL)
	{
		$dblink->query("SELECT managername FROM ".C_MANAGER_WJ_TBL);
		if ($dblink->Errno != 0)
		{
			$dblink->query("CREATE TABLE ".C_MANAGER_WJ_TBL." (
				managername char(40),
				mpasswd char(40),
				isdeleted SMALLINT NOT NULL
				)");// IF NOT EXISTS ".C_MANAGER_WJ_TBL);
		}
		$dblink->clean_results();
	}
	$dblink->close();
}

function MakeWjRuleTable($TableName)
{
	// 开始生成数据表
	// ###################### 管理员 #######################
	$dblink=new DB;
	$dblink->Database=C_DB_NAME;
	if ($TableName == "" || $TableName == C_RULE_WJ_TBL)
	{
		$dblink->query("SELECT rule FROM ".C_RULE_WJ_TBL);
		if ($dblink->Errno != 0)
		{
			$dblink->query("CREATE TABLE ".C_RULE_WJ_TBL." (
				appid INT UNSIGNED,
				userid INT UNSIGNED,
				rule SMALLINT NOT NULL,
				isdeleted SMALLINT NOT NULL
				)");// IF NOT EXISTS ".C_RULE_WJ_TBL);
		}
		$dblink->clean_results();
	}
	$dblink->close();
}

function MakeWjUserTable($TableName)
{
	// 开始生成数据表
	// ###################### 管理员 #######################
	$dblink=new DB;
	$dblink->Database=C_DB_NAME;
	if ($TableName == "" || $TableName == C_USER_WJ_TBL)
	{
		$dblink->query("SELECT userid FROM ".C_USER_WJ_TBL);
		if ($dblink->Errno != 0)
		{
			$dblink->query("CREATE TABLE ".C_USER_WJ_TBL." (
				userid INT UNSIGNED NOT NULL AUTO_INCREMENT,
				username char(40),
				department char(40),
				number char(40),
				isdeleted SMALLINT NOT NULL,
				PRIMARY KEY(userid)
				)");// IF NOT EXISTS ".C_USER_WJ_TBL);
		}
		$dblink->clean_results();
	}
	$dblink->close();
}

function MakeWjAppTable($TableName)
{
	// 开始生成数据表
	// ###################### 管理员 #######################
	$dblink=new DB;
	$dblink->Database=C_DB_NAME;
	if ($TableName == "" || $TableName == C_APP_WJ_TBL)
	{
		$dblink->query("SELECT appid FROM ".C_APP_WJ_TBL);
		if ($dblink->Errno != 0)
		{
			$dblink->query("CREATE TABLE ".C_APP_WJ_TBL." (
				appid INT UNSIGNED NOT NULL AUTO_INCREMENT,
				appname char(40),
				listenip INT UNSIGNED,
				protocolname char(40),
				listenport SMALLINT,
				verifymode INT UNSIGNED,
				serverip INT UNSIGNED NOT NULL,
				serverport INT NOT NULL,
				joindate INT UNSIGNED NOT NULL,
				isdeleted SMALLINT NOT NULL,
				PRIMARY KEY(appid)
				)");// IF NOT EXISTS ".C_APP_WJ_TBL);
		}
		$dblink->clean_results();
	}
	$dblink->close();
}

function MakeWjDB()
{
	$dblink=new DB;
	$dblink->query("CREATE DATABASE ".C_DB_NAME);
	$dblink->Database=C_DB_NAME;
        echo "<p>尝试再次连接...</p>";
        $dblink->connect(C_DB_NAME);
        if ($dblink->Errno == 0)
        {
        	echo "<p>成功连接数据库!</p>";
        }
        else
        {
        	echo "<p>再次连接数据库失败! 请确认数据库和服务器设置正确然后再试.</p>";
        	echo "错误号  ：$dblink->Errno<br>";
        	echo "错误说明：$dblink->Error<br>";
        }
        $dblink->close();
	MakeWjManagerTable(C_MANAGER_WJ_TBL);
	MakeWjRuleTable(C_RULE_WJ_TBL);
	MakeWjAppTable(C_APP_WJ_TBL);
	MakeWjUserTable(C_USER_WJ_TBL);
}

	$dblink=new DB;
	$dblink->Database=C_DB_NAME;

	$dblink->connect();
	if ($dblink->Errno != 0)
	{
		if ($dblink->Errno == $ERR_NO_MYSQL)
		{
			print("$dblink->Error");
			$dblink->close();
			die();
		}
		else
		{
			print("连接数据库出错，正在尝试重建，请等待...");
			$dblink->close();
			MakeWjDB();
		}
	}
	else
	{
		$dblink->close();
		MakeWjManagerTable(C_MANAGER_WJ_TBL);
		MakeWjRuleTable(C_RULE_WJ_TBL);
		MakeWjAppTable(C_APP_WJ_TBL);
		MakeWjUserTable(C_USER_WJ_TBL);
	}
?>