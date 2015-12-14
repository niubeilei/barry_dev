<?php

class DB
{
	var $Host = C_DB_HOST;			// Hostname of our MySQL server
	var $Database = "";	//C_DB_NAME;		// Logical database name on that server
	var $User = C_DB_USER;			// Database user
	var $Password = C_DB_PASS;		// Database user's password
	var $Persistent = C_USE_PERSISTENT;	// Use a persistent connection
	var $Link_ID = 0;				// Result of mysql_connect()
	var $Query_ID = 0;				// Result of most recent mysql_query()
	var $Record	= array();			// Current mysql_fetch_array()-result
	var $Row;						// Current row number
	var $Errno = 0;					// Error state of query
	var $Error = "";

	function halt($msg)
	{
		echo("</TD></TR></TABLE><B>Database error:</B> $msg<BR>\n");
		echo("<B>MySQL error</B>: $this->Errno ($this->Error)<BR>\n");
//--------------------------- zzc 2002-06-27 ---------------------------------
		//die("Session halted.");
//--------------------------- zzc 2002-06-27 ---------------------------------
	}

	function connect()
	{
//		print("$this->Host,$this->User,$this->Password<br>");
		if($this->Link_ID == 0)
		{
			if ($this->Persistent)
			{
				$this->Link_ID = mysql_pconnect($this->Host, $this->User, $this->Password);
			}
			else
			{
				$this->Link_ID = mysql_connect($this->Host, $this->User, $this->Password);
			};
			if (!$this->Link_ID)
			{
//--------------------------- zzc 2002-06-27 ---------------------------------
				$this->Errno=$ERR_NO_MYSQL;
				$this->Error="mysql数据库系统连接失败，可能是数据库系统没有启动！";
//--------------------------- zzc 2002-06-27 ---------------------------------
				$this->halt("Link_ID == false, connect failed");
			}
			if ($this->Database != "")
			{
				$SelectResult = mysql_select_db($this->Database, $this->Link_ID);
				if(!$SelectResult)
				{
					$this->Errno = mysql_errno($this->Link_ID);
					$this->Error = mysql_error($this->Link_ID);
					$this->halt("cannot select database <I>".$this->Database."</I>");
				}
			}
		}
	}

	function query($Query_String)
	{
		$this->connect();
		$this->Query_ID = mysql_query($Query_String,$this->Link_ID);
		$this->Row = 0;
		$this->Errno = mysql_errno();
		$this->Error = mysql_error();
		if (!$this->Query_ID)
		{
			$this->halt("Invalid SQL: ".$Query_String);
		}
		return $this->Query_ID;
	}

	function next_record()
	{
		$this->Record = mysql_fetch_array($this->Query_ID);
		$this->Row += 1;
		$this->Errno = mysql_errno();
		$this->Error = mysql_error();
		$stat = is_array($this->Record);
		if (!$stat)
		{
			mysql_free_result($this->Query_ID);
			$this->Query_ID = 0;
		}
		return $this->Record;
	}

	function num_rows()
	{
		return mysql_num_rows($this->Query_ID);
	}

	function clean_results()
	{
		if($this->Query_ID != 0)
			mysql_freeresult($this->Query_ID);
	}

	function close()
	{
		if($this->Link_ID != 0 && !$this->Persistent)
			mysql_close($this->Link_ID);
	}
}
?>