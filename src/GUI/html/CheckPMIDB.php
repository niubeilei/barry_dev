<?php
	include("mysql.lib.php");

function MakeSimplePMIRuleTable($TableName)
{
	// 开始生成数据表
	// ###################### 管理员 #######################
	$dblink=new DB;
	$dblink->Database=C_PMI_DB_NAME;
	if ($TableName == "" || $TableName == C_RULE_PMI_TBL)
	{
		$dblink->query("SELECT serverserial FROM ".C_RULE_PMI_TBL);
		if ($dblink->Errno != 0)
		{
			$dblink->query("CREATE TABLE ".C_RULE_PMI_TBL." (
				serverserial INT UNSIGNED,
				policenum CHAR(50) NOT NULL,
				rule SMALLINT NOT NULL,
				isdeleted SMALLINT NOT NULL
				)");// IF NOT EXISTS ".C_RULE_PMI_TBL);
		}
		$dblink->clean_results();
	}
	$dblink->close();
}

function MakeSimplePMIServerTable($TableName)
{
	// 开始生成数据表
	// ###################### 管理员 #######################
	$dblink=new DB;
	$dblink->Database=C_PMI_DB_NAME;
	if ($TableName == "" || $TableName == C_SERVER_PMI_TBL)
	{
		$dblink->query("SELECT serverserial FROM ".C_SERVER_PMI_TBL);
		if ($dblink->Errno != 0)
		{
			$dblink->query("CREATE TABLE ".C_SERVER_PMI_TBL." (
				serverserial INT UNSIGNED NOT NULL AUTO_INCREMENT,
				serverip char(16) NOT NULL,
				serverport char(5) NOT NULL,
				joindate INT UNSIGNED NOT NULL,
				isdeleted SMALLINT NOT NULL,
				PRIMARY KEY(serverserial)
				)");// IF NOT EXISTS ".C_SERVER_PMI_TBL);
		}
		$dblink->clean_results();
	}
	$dblink->close();
}

function MakeSimplePMIDB()
{
	$dblink=new DB;
	$dblink->query("CREATE DATABASE ".C_PMI_DB_NAME);
	$dblink->Database=C_PMI_DB_NAME;
        echo "<p>尝试再次连接...</p>";
        $dblink->connect(C_PMI_DB_NAME);
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
	MakeSimplePMIRuleTable(C_RULE_PMI_TBL);
	MakeSimplePMIServerTable(C_SERVER_PMI_TBL);
}

	$dblink=new DB;
	$dblink->Database=C_PMI_DB_NAME;

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
			MakeSimplePMIDB();
		}
	}
	else
	{
		$dblink->close();
		MakeSimplePMIRuleTable(C_RULE_PMI_TBL);
		MakeSimplePMIServerTable(C_SERVER_PMI_TBL);
	}
?>