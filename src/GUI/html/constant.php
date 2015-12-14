<!--
**************************************************************************
                  《 PLW517服务器密码机管理系统Web站点》
                        版权所有 (C) 2005,2006
**************************************************************************
******************************************************
项目:《 PLW517服务器密码机管理系统Web站点》
模块:变量定义及函数定义头文件
描述:
版本:1.0.1.0
日期:2005-12-22
作者:Rechard
更新:
	1.2005-12-24
        ModiServs函数增加全部清空功能；
        ModiDenyPages函数修改了Denypage删除时同时删除其文件；
TODO:
*******************************************************
-->
<?php
	$ln="<br>\n";
    define(CFGFILEPATH,"/usr/local/AOS/Config/");
    define(MYSUDO,"/usr/local/AOS/Bin/mysudo ");
    define(MYCLI,"/usr/local/AOS/Bin/Cli.exe -cmd ");
    define(MYCLIVERSION,"/usr/local/AOS/Bin/Cli.exe -r");
    define(REBOOTSERV,"/usr/local/AOS/Bin/rebootservshell");
    define(ADDOW,"chmod go+w ");
    define(DELOW,"chmod go-w ");
    define(AOS_PATH, "/usr/local/AOS/"); 
    define(AOS_PATH_BIN, AOS_PATH."Bin/"); 
    define(NETCONFIG, AOS_PATH_BIN . "netconfig"); 
    define(CRONTAB, "/etc/cron.d/aoscron"); 

    define(MANAGERFILE, CFGFILEPATH."managers.conf");
    define(PMIGLOBAL, CFGFILEPATH."pmi_global.conf");
    define(PMICFGFILE, CFGFILEPATH."pmi.conf");
    define(OCSPCFGFILE, CFGFILEPATH."ocsp.conf");
    define(PMIRULEFILE, CFGFILEPATH."rules.conf");
    define(AppSvrCFGFILE, CFGFILEPATH."app_proxy.conf");
    define(ASSIGNCFGFILE, CFGFILEPATH."assign.conf");
    define(DENYPAGEFILE, CFGFILEPATH."denypages.conf");       
    define(DENYPAGEFILEPATH, "/usr/local/AOS/Config/Denypages/");
    define(USBKEY_USER_CFGFILE, CFGFILEPATH."usbkeyuser.conf");
    define(CRLGLOBAL,CFGFILEPATH."crlserver.conf");
    
    define(LICENSETXT,CFGFILEPATH."License.txt");
    define(LICENSE_GETHDINFO,AOS_PATH_BIN."getHDinfo");
    define(LICENSE_VERIFY,AOS_PATH_BIN."verify_license");    

    define(WWWDOCDIR,"/usr/local/AOS/GUI/html/");
    define(CRONSHELL,"/usr/local/AOS/Bin/cronshell");
	define(MOUNT_PATH,"/mnt/data/");
	define(MOUNT_CMD,"mount /dev/hda2 /mnt/data");
	define(UMOUNT_CMD,"umount /dev/hda2");

	define(GATEWAY_DZP,"SSLMPS/Config/network");
	define(ETH_DZP,"SSLMPS/Config/ifcfg-eth");
	define(CFGFILE_DZP,"SSLMPS/Config/Protocol.conf");

	define(SETCAFILE,"/usr/SSLMPS/SetCA.cfg");
	define(SETCAFILE_DZP,"SSLMPS/SetCA.cfg");

	define("C_DB_TYPE", "mysql");					// SQL server type ("mysql", "pgsql" or "odbc")
	define("C_DB_HOST", "localhost");				// Hostname of your MySQL server
	define("C_DB_NAME", "wjproxy");					// Logical database name on that server
	define("C_DB_USER", "root");					// Database user
	define("C_DB_PASS", "");					// Database user's password
	define("C_MNG_TBL", "managers");				// Name of the table where messages are stored
	define("C_USR_TBL", "c_users");					// Name of the table where user names are stored
	define("C_REG_TBL", "c_reg_users"); 			// Name of the table where registered users are stored

	define("C_PMI_DB_NAME", "SimpPMI");				// PMI database name on that server
	define("C_SERVER_PMI_TBL", "RuleTable"); 			// Name of the table where registered users are stored
	define("C_RULE_PMI_TBL", "rulepmi"); 			// Name of the table where registered users are stored

	define("C_APP_WJ_TBL", "AppTable"); 			// 已经注册的代理服务列表
	define("C_USER_WJ_TBL", "UserTable"); 			// 已经注册的用户列表
	define("C_RULE_WJ_TBL", "RuleTable"); 			// 授权规则表
	define("C_MANAGER_WJ_TBL", "ManagerTable"); 			// 授权规则表

	define("C_APP_CONN_FILE","AppConnFile");		//安全用户帐号关联文件列表
	define("C_FILE_CONN_PARA","FileConnPara");		//关联文件参数列表

	define("C_DB_ROLE","RoleTable");
	define("C_ROLE_CONT","RoleContTable");
	define("C_ROLE_RULE","RoleRuleTable");
	// Persistent connection
	define("C_USE_PERSISTENT", "0");				// Use a persistent connection? : 0 = no, 1 = yes (it doesn't work on all servers)
	define("C_PERSISTENT_REFRESH", "2");			// Message refreshing when using persistent connection mode. Allowed values are from 1 to 20 seconds.

	// Cleaning settings for messages and usernames
	define("C_MSG_DEL", 96);						// Messages are deleted when there are 'xx' hours old
	define("C_USR_DEL", 4);							// Usernames are deleted when its last use is 'xx' minutes old
													//	the second setting must be greater than the maximum time
													//	authorized between messages list refresh
	define("C_REG_DEL", 0);							// Registered profiles are deleted when its last use is 'xx' days old

	define("ERR_NO_MYSQL", 0x7ffff);				// SQL server is not present

	define("CATXTFILE", "/usr/SSLMPS/SetCA.txt");				// config text file of setca.cfg
	define("CATXTCOM", "/usr/SSLMPS/catxt");				// convert command of setca.cfg

	define("ETH", "/etc/sysconfig/network-scripts/ifcfg-eth");				// config file of eth0
	define("GATEWAY", "/etc/sysconfig/network");				// config file of gateway
	define("GATEWAYBAK", "/etc/sysconfig/network.bak");				// config file of gateway

	define("COL_NUM_PROFILE",5);			// 每一行的配置项目数
	define("SEVICE_NAME_PROFILE", 0);		// 协议配置文件的 “代理协议名”
	define("LOCAL_PORT_PROFILE", 1);		// “本地处理端口”
	define("AUTH_MODE_PROFILE", 2);			// “代理服务器与客户端的认证模式”
	define("SERVER_IP_PROFILE", 3);			// “应用服务器IP地址”
	define("SERVER_PORT_PROFILE", 4);		// “应用服务器服务端口”

function AosAddWriteOption($filename)
{
	$mysudo = MYSUDO;
	$chmodaddow = ADDOW;
	$cmd = $mysudo.$chmodaddow.$filename;
	exec($cmd);
}

function AosDelWriteOption($filename)
{
   	$mysudo = MYSUDO;
	$chmodaddow = DELOW;
	$cmd = $mysudo.$chmodaddow.$filename;
	exec($cmd); 
}

function GetPMICfg(&$ldap_port,&$pmi_subtree,&$pmi_ip,&$pmi_port,&$pmi_dn,&$ca_ip,&$ca_port,&$ca_basedn)
{
	
	$fcont =file(PMICFGFILE);
	if (!$fcont)
	{
		return false;
	}
	list($index,$pmicfgstr)=each($fcont);

	list($index,$pmicfgstr)=each($fcont);
	$temp=strtok($pmicfgstr,"\"");
	$ldap_port=strtok("\"");
	
	list($index,$pmicfgstr)=each($fcont);
	list($index,$pmicfgstr)=each($fcont);
	list($index,$pmicfgstr)=each($fcont);
	$temp=strtok($pmicfgstr,"\"");
	$pmi_subtree=strtok("\"");

	list($index,$pmicfgstr)=each($fcont);
	$temp=strtok($pmicfgstr,"\"");
	$pmi_ip=strtok("\"");
	list($index,$pmicfgstr)=each($fcont);
	$temp=strtok($pmicfgstr,"\"");
	$pmi_port=strtok("\"");
	list($index,$pmicfgstr)=each($fcont);
	$temp=strtok($pmicfgstr,"\"");
	$pmi_dn=strtok("\"");
	list($index,$pmicfgstr)=each($fcont);
	$temp=strtok($pmicfgstr,"\"");
	$ca_ip=strtok("\"");
	list($index,$pmicfgstr)=each($fcont);
	$temp=strtok($pmicfgstr,"\"");
	$ca_port=strtok("\"");
	list($index,$pmicfgstr)=each($fcont);
	$temp=strtok($pmicfgstr,"\"");
	$ca_basedn=strtok("\"");
	return true;
}
function SetPMICfg($ldap_port,$PMISubTree,$pmi_IP,$pmi_port,$pmi_dn,$ca_ip,$ca_port,$CABaseDN)
{
	$filename = PMICFGFILE;
	if (!file_exists($filename))
	{
    	$cmd = MYSUDO."touch ".$filename;
		exec($cmd);
	}
	AosAddWriteOption($filename);
	$fp = fopen($filename, "wb");
	if ($fp == FALSE)
		return FALSE;
	$catxtstr="[PORT]"."\012"."LDAP_PROXY=\"".$ldap_port."\""."\012"."\012"."[Ldap]"."\012"."PMISubTree=\"".$PMISubTree."\""."\012"."PMIServerIP=\"".$pmi_IP.
		"\""."\012"."PMIServerPort=\"".$pmi_port."\""."\012"."PMIBaseDN=\"".$pmi_dn."\""."\012"."CAServerIP=\"".$ca_ip."\""."\012".
		"CAServerPort=\"".$ca_port."\""."\012"."CABaseDN=\"".$CABaseDN."\"";
	fwrite($fp,$catxtstr,strlen($catxtstr));
	fclose($fp);
	AosDelWriteOption($filename);
	//write_ed();
	return TRUE;
}

//--------------20051214;by yy;增加PMI参数信息读写函数
function AosGetPMI(&$ponoff,&$paddr,&$pport,&$porg)
{
    $filename = PMIGLOBAL;

    if (file_exists($filename))
    {
        //copy(PMIGLOBAL,PMIGLOBALBAK);
        $handle = fopen($filename, "r");

        if ($handle)
        {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($ponoff,$paddr, $pport, $porg)=split(" ",$buffer,10);
            fclose($handle);
        }
    }
}

function AosSetPMI($ponoff,$paddr,$pport,$porg)
{
    $filename = PMIGLOBAL;

	if (!file_exists($filename))
	{
    	$cmd = MYSUDO."touch ".$filename;
		exec($cmd);
	}
    //add write option to the file   --by ytao
	AosAddWriteOption($filename);
	
    $handle = fopen($filename, "w");
    if ($handle)
    {
        $buffer = $ponoff." ".$paddr." ".$pport." ".$porg."\n";
        fwrite($handle,$buffer);
        fclose($handle);
        //del write option to the file   --by ytao
    	AosDelWriteOption($filename);
	//write_ed();
       	return true;           
    }
    return false;
}

//————————————————————————————20051230;by cw;增加CRL服务器设置功能————————————————————————————
function GetCRL(&$ponoff,&$paddr,&$pport,&$purl,&$ptime)
{
    $filename = CRLGLOBAL;

    if (file_exists($filename))
    {
        //copy(PMIGLOBAL,PMIGLOBALBAK);
        $handle = fopen($filename, "r");

        if ($handle)
        {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($ponoff,$paddr, $pport, $purl, $ptime)=split(" ",$buffer,10);
            fclose($handle);
        }
    }
}

function SetCRL($ponoff,$paddr,$pport,$purl,$ptime)
{
    $filename = CRLGLOBAL;

	if (!file_exists($filename))
	{
    	$cmd = MYSUDO."touch ".$filename;
		exec($cmd);
	}
      
    //add write option to the file   --by ytao
	AosAddWriteOption($filename);
	
    $handle = fopen($filename, "w");

    if ($handle)
    {
        $buffer = $ponoff." ".$paddr." ".$pport." ".$purl." ".$ptime."\n";
        fwrite($handle,$buffer);
        fclose($handle);
        //del write option to the file   --by ytao
    	AosDelWriteOption($filename);
	//write_ed();
        return true;           
        
    }
    return false;
}
//————————————————————————————20051230;by cw;增加CRL服务器设置功能————————————————————————————

//*******VARType=1;检查是否存在空格；=2检查是否存在非数字；=4检查是否存在非标准ASCII码
//*******  =5检查是否存在非数字或该数值〉65536
//*******返回True表示不存在，返回false表示存在！
function VarIsValid($VAR,$VARType)
{
    if (strlen(trim($VAR))<1)
    {
        return false;
    }
    
    if (($VARType & 1)==1)
    //检查是否存在空格
    {

       if (strpos($VAR," ")!=false)
       {
           return false;
       }
       //$j= $VARType & 2;
       //echo "$j";
    }
    if (($VARType & 2) == 2)
    {
       //echo "比较是否存在非数字nijio";
       $l=strlen($VAR);
       for ($i=0;$i<$l;$i++)
       {
           $f=substr($VAR,$i,1);
           //echo $f;
           if (($f<"0") or ($f>"9"))
           {

               return false;
               break;
           }
       }
       //$j= $VARType & 4;
    }
    if (($VARType & 4) == 4)
    //检查是否存在非标准ASCII码
    {
       for ($i=0;$i<strlen($VAR);$i++)
       {
           if ((substr($VAR,$i,1)<" ") or (substr($VAR,$i,1)>"~"))
           {
               return false;
               break;
           }
       }
    }
    return true;
}
//--------------20051214;by yy;增加PMI参数信息读写函数

//--------------20051214;by yy;增加OCSP参数信息读写函数
function GetOCSP(&$oswi,&$paddr,&$pport)
{
    $filename = OCSPCFGFILE;

    if (file_exists($filename))
    {
        //copy(PMIGLOBAL,PMIGLOBALBAK);
        $handle = fopen($filename, "r");

        if ($handle)
        {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($oswi,$paddr, $pport)=split(" ",$buffer,10);
            fclose($handle);
            if ($oswi=="on")
            {
                $oswi=true;
            } else
            {
                $oswi=false;
            }
        }
    }
    else
    {
        $oswi=false;
        $paddr="0.0.0.0";
        $pport="0";
    }
}

function SetOCSP($oswi,$paddr,$pport)
{
    $filename = OCSPCFGFILE;

    if (file_exists($filename))
    {
    	//add write option to the file   --by ytao
    	AosAddWriteOption($filename);
    	
        $handle = fopen($filename, "w");

        if ($handle)
        {
            $buffer =$oswi." ".$paddr." ".$pport."\n";

            fwrite($handle,$buffer);

            fclose($handle);
            
    	    //del write option to the file   --by ytao
    	    AosDelWriteOption($filename);
	    //write_ed();
        }
        
    }
	return false;
}
//--------------20051214;by yy;增加OCSP参数信息读写函数

//--------------20051215;by yy;增加PMI访问控制配置读写操作等函数
function GetPMIRules()
{
    $filename = PMIRULEFILE;
    $PMIrules=array();
    //$MANNAME[]="uuuu";
    if (file_exists($filename))
    {
        //$MANNAME[]="ooo";
        $handle = fopen($filename, "r");
        $ni=0;
        if ($handle)
        {
          while (!feof($handle))
          {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($PAppname, $POrg, $PSysname, $Ponoff)=split(" ",$buffer,10);
            //$MANNAME[]=$user;
            if ($PAppname>" ")
            {
                $nj=$ni+1;
                $PMIrules[$ni]=array("$nj","$PAppname","$POrg","$PSysname","$Ponoff");
                $ni+=1;
            }

          }
          fclose($handle);
        }
    }
    else
    {
        $PMIrules[]="";
    }
    //print_r($MANNAME);
    return $PMIrules;
}

//****** 20051222;by cw;查询[代理服务]是否已经存在
//****** $Qtype=1;查询代理服务名；$PID用来指示修改时本记录的序号
function PMIRuleExists($PAppname,$PID,$Qtype)
{
    $Apps=GetPMIRules();
    $acount=count($Apps);
    for($ai=0;$ai<$acount;$ai++)
    {
        $id=$Apps[$ai][0];
        $appn=$Apps[$ai][1];

        if (($Qtype & 1)==1)
        {
            if ((strcmp($PAppname,$appn)==0) && ($PID!=$id))
            {
                return true;
            }
        }
    }
    return false;
}

//******增加一条PMI规则；
function AddPMIRules($PAppname,$POrg,$PSysname,$Ponoff)
{
    $filename = PMIRULEFILE;
    $AddOK=0;
    $rules=GetPMIRules();
    $rcount=count($rules);
    if (file_exists($filename))
    {
    	//add write option to the file   --by ytao
		AosAddWriteOption($filename);    	
        if (($rcount>0) && ($rules[0][1]>" "))
        {
            $handle = @fopen($filename, "a");
            $MCode="\r\n".$PAppname.' '.$POrg.' '.$PSysname.' '.$Ponoff;
        } else
        {
            $handle = @fopen($filename, "w");
            $MCode=$PAppname.' '.$POrg.' '.$PSysname.' '.$Ponoff;
        }
        if ($handle)
        {
            if (fwrite($handle, $MCode) == true)
            {
              $AddOK=1;
            }
            fclose($handle);
	    //write_ed();
        }
        
    	//del write option to the file   --by ytao
    	AosDelWriteOption($filename);
    }
    //print_r($Users);
    if ($AddOK==1)
    {
        return true;
    } else
    {
        return false;
    }
}

//**** $MType用以指明删除该用户还是修改该用户密码
//**** =0:删除该条记录；1:修改该条记录；2:删除应用服务PAppname对应的所有记录
function ModiPMIRules($PRID,$PAppname,$POrg,$PSysname,$Ponoff,$MType)
{
    $filename = PMIRULEFILE;
    $ModiOK=0;
    if (file_exists($filename))
    {
    	//add write option to the file   --by ytao
		AosAddWriteOption($filename);
		$handle = @fopen($filename, "r");
        $Rules=array();
        $i=0;
        $j=0;
        if ($handle)
        {
          while (!feof($handle))
          {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($Appname1, $Org1, $Sysname1, $Onoff1)=split(" ",$buffer,10);
            /*
            if (((strcmp($PAppname,$Appname)==0) && (strcmp($PSysname,$Sysname)==0) && (PRID==($i+1)) && ($MType==0))
              or ((strcmp($PAppname,$Appname)==0) && ($MType==2)))
            */
            if (!($Appname1>" "))
            {
                continue;
            }
            $j+=1;
            if ((($PRID==($j)) && ($MType==0))
              or ((strcmp($PAppname,$Appname1)==0) && ($MType==2)))
            {
                continue;
            } else if (($PRID==($j)) && ($MType==1))
            {
                $Appname1=$PAppname;
                $Sysname1=$PSysname;
                $Org1=$POrg;
                $Onoff1=$Ponoff;
            }
            $Rules[$i]=array("$Appname1","$Org1","$Sysname1","$Onoff1");

            $i+=1;
          }
          fclose($handle);
        }
        $handle = @fopen($filename, "w");

        $buffer="";
        if ($handle)
        {
          for ($j = 0; $j < $i; $j++)
          {
            if ($j != 0)
            {
                $buffer="\r\n";
            } else
            {
                $buffer="";
            }

            $buffer .= $Rules[$j][0]." ".$Rules[$j][1]." ".$Rules[$j][2]." ".$Rules[$j][3];
            fwrite($handle,$buffer);

          }
          fclose($handle);
	  //write_ed();
          $ModiOK=1;
        }
        //del write option to the file   --by ytao
    	AosDelWriteOption($filename);
    }
    //print_r($Users);
    if ($ModiOK==1)
    {
        return true;
    } else
    {
        return false;
    }
}

//**** MType=0,修改组织名Poldname对应的所有记录为Mnewname
//**** Mtype=1修改应用服务Poldname对应的所有记录为Mnewname
function ModiPMIRulesAPP($Poldname,$Mnewname,$MType)
{
    $filename = PMIRULEFILE;
    $ModiOK=0;
    if (file_exists($filename))
    {
    	//add write option to the file   --by ytao
		AosAddWriteOption($filename);
		$handle = @fopen($filename, "r");
        $Rules=array();
        $i=0;
        $j=0;
        if ($handle)
        {
          while (!feof($handle))
          {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($Appname, $Sysname, $Org, $Onoff)=split(" ",$buffer,10);
            if (!($Appname>" "))
            {
                continue;
            }
            $j+=1;
            if ((strcmp($Poldname,$Appname)==0) && ($MType==1))
            {
                $Appname=$Mnewname;
            } else if ((strcmp($Poldname,$Org)==0) && ($MType==0))
            {
                $Org=$Mnewname;
            }
            $Rules[$i]=array("$Appname","$Sysname","$Org","$Onoff");
            $i+=1;
          }
          fclose($handle);
        }
        $handle = @fopen($filename, "w");

        $buffer="";
        if ($handle)
        {
          for ($j = 0; $j < $i; $j++)
          {
            if ($j != 0)
            {
                $buffer="\r\n";
            } else
            {
                $buffer="";
            }

            $buffer .= $Rules[$j][0]." ".$Rules[$j][1]." ".$Rules[$j][2]." ".$Rules[$j][3];
            fwrite($handle,$buffer);

          }
          fclose($handle);
	  //write_ed();
          $ModiOK=1;
        }
        
        //del write option to the file   --by ytao
    	AosDelWriteOption($filename); 
    }
    //print_r($Users);
    if ($ModiOK==1)
    {
        return true;
    } else
    {
        return false;
    }
}
//--------------20051215;by yy;增加PMI访问控制配置读写操作等函数

//--------------20051216;by yy;增加应用服务管理配置读写操作等函数
function GetAppSvrList($apptype)
{
    $filename = AppSvrCFGFILE;
    $AppSvrs=array();
    //$MANNAME[]="uuuu";
    if (file_exists($filename))
    {
        //$MANNAME[]="ooo";
        $handle = fopen($filename, "r");
        $ni=0;
        if ($handle)
        {
          while (!feof($handle))
          {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($Modienable,$Proxyname,$Proxytype,$Svrtype,$Svrport,$Secutag,$Sname,$Sport,$Opensw,$Addtime,$denypages)=split(" ",$buffer,12);
            //$MANNAME[]=$user;
            if (strcmp($apptype,"all")==0 || strcmp($apptype,$Proxytype)==0)
            {
            	if ($Proxytype>" ")
            	{
                	$nj=$ni+1;
                	//*******20060101;by yy;加上$Modienable;
                	$AppSvrs[$ni]=array("$nj","$Modienable","$Proxyname","$Proxytype","$Svrtype","$Svrport","$Secutag","$Sname","$Sport","$Opensw","$Addtime","$denypages");
                	$ni+=1;
            	}
            }
          }
          fclose($handle);
        }
    }
    else
    {
        $AppSvrs[]="";
    }
    //print_r($MANNAME);
    return $AppSvrs;
}

//******* QType=1;列出所有AppName；
//******* QType=2;只列出正向+HTTP类型的；且Modienable=0的不要显示
function GetAppSvrNameList($QType)
{
    $filename = AppSvrCFGFILE;
    $AppSvrs=array();
    //$MANNAME[]="uuuu";
    if (file_exists($filename))
    {
        //$MANNAME[]="ooo";
        $handle = fopen($filename, "r");
        $ni=0;
        if ($handle)
        {
          while (!feof($handle))
          {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            //*******20060101;by yy;加上$Modienable;
            list($Modienable,$Proxyname,$Proxytype,$Svrtype,$Svrport,$Secutag,$Sname,$Sport,$Opensw,$Addtime,$denypages)=split(" ",$buffer,12);
            //$MANNAME[]=$user;
            if ($Proxytype>" ")
            {
                $nj=$ni+1;
                if ($QType==1)
                {
                    $AppSvrs[]="$Proxyname";
                }
                if ($QType==2)
                {
                    if ((strcmp($Proxytype,"forward")==0) && ($Svrtype=="http") &&($Modienable==1))
                    {
                        $AppSvrs[]="$Proxyname";
                    }
                }
                $ni+=1;
            }
          }
          fclose($handle);
        }
    }
    else
    {
        $AppSvrs[]="";
    }
    //print_r($MANNAME);
    return $AppSvrs;
}
// 20051222;by cw;查询及是否已经存在
// $Qtype=1;查询代理服务[代理类型,代理服务]；
// $Qtype=2;查询服务端口[代理类型,服务端口];$PID用来指示修改时本记录的序号
// $Qtype=4;只查询代理服务是否已经存在；
function AppSvrsExists($Proxytype,$Appname,$Svrsport,$PID,$Qtype)
{
    $Apps=GetAppSvrList("all");
    $acount=count($Apps);
    for($ai=0;$ai<$acount;$ai++)
    {
        $id=$Apps[$ai][0];
        $appn=$Apps[$ai][2];
        $ptype=$Apps[$ai][3];
        $sport=$Apps[$ai][5];
        if (($Qtype & 1)==1)
        {
            if ((strcmp($Proxytype,$ptype)==0) && (strcmp($Appname,$appn)==0))
            {
                return true;
            }
        }
        if (($Qtype & 2)==2)
        {
            if ((strcmp($Appname,$appn)!=0) &&
            		(strcmp($Proxytype,$ptype)==0) && 
            		(strcmp($Svrsport,$sport)==0) && 
            		($PID!=$id))
            {
                return true;
            }
        }
        if (($Qtype & 4)==4)
        {
            if ((strcmp($Appname,$appn)==0))
            {
                return true;
            }
        }
    }
    return false;
}

// $MType用以指明删除该服务还是修改该服务参数
// =0:删除该条记录；
// =1:修改该条记录；
// =2:删除该条记录并同时删除/修改PMI规则(rules.conf)里appname=PProxyname的记录
// =9:<modified on 2005-12-24;by yy>删除全部记录并同时删除全部对应PMI规则；
// 全部删除时不删除$Modienable=0的记录
function ModiServs($PRID,$PProxyname,$PProxytype,$PSvrtype,$PSvrport,$PSecutag,$PSname,$PSport,$POpensw,$PAddtime,$Pdenypages,$MType)
{
    $filename = AppSvrCFGFILE;
    $ModiOK=0;
    if (file_exists($filename))
    {
    	//add write option to the file   --by ytao
    	AosAddWriteOption($filename);
    	
        $handle = @fopen($filename, "r");
        $Servs=array();
        $i=0;
        $j=0;
        if ($handle)
        {
          while (!feof($handle))
          {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($Modienable,$Proxyname,$Proxytype,$Svrtype,$Svrport,$Secutag,$Sname,$Sport,$Opensw,$Addtime,$denypages)=split(" ",$buffer,12);
            if (!($Proxytype>" "))
            {
                continue;
            }
            $j+=1;
            if (((strcmp($Proxyname, $PProxyname)== 0) && ($MType==0)) or
              	((strcmp($Proxyname, $PProxyname)== 0) && ($MType==2)) or 
              	(($MType==9) && ($Modienable==1) && (strcmp($PProxytype, $Proxytype)==0) ))
            {
                if (($MType==2) or ($MType==9))
                {
                    //****** 20051217;by yy;删除应用服务的同时删除PMI访问规则里所对应Appname的记录，
                    //******    也可使用ModiPMIRulesAPP修改这些记录中的Appname="-";
                    ModiPMIRules("",$Proxyname,"","","",2);
                }
                continue;
            } 
            else if ((strcmp($Proxyname, $PProxyname)== 0) && ($MType==1))
            {
                $Proxytype=$PProxytype;
                $Proxyname=$PProxyname;
                $Svrtype=$PSvrtype;
                $Svrport=$PSvrport;
                $Secutag=$PSecutag;
                $Sname=$PSname;
                $Sport=$PSport;
                $Opensw=$POpensw;
                $Addtime=$PAddtime;
                $denypages=$Pdenypages;
            }
            $Servs[$i]=array("$Modienable","$Proxyname","$Proxytype","$Svrtype","$Svrport","$Secutag","$Sname","$Sport","$Opensw","$Addtime","$denypages");

            $i+=1;
          }
          fclose($handle);
        }
        $handle = @fopen($filename, "w");

        $buffer="";
        if ($handle)
        {
          for ($j = 0; $j < $i; $j++)
          {
            if ($j != 0)
            {
                $buffer="\r\n";
            } else
            {
                $buffer="";
            }

            $buffer .= $Servs[$j][0]." ".$Servs[$j][1]." ".$Servs[$j][2]." "
                .$Servs[$j][3]." ".$Servs[$j][4]." ".$Servs[$j][5]." ".$Servs[$j][6]
                ." ".$Servs[$j][7]." ".$Servs[$j][8]." ".$Servs[$j][9]." ".$Servs[$j][10];
            fwrite($handle,$buffer);

          }
          fclose($handle);
    	  //write_ed();
          $ModiOK=1;
        }
        
        //del write option to the file   --by ytao
    	AosDelWriteOption($filename);
        
    }
    //print_r($Users);
    if ($ModiOK==1)
    {
        return true;
    } else
    {
        return false;
    }
}

//**** MType=0,修改DenyPage=Poldname对应的所有记录为Mnewname
//**** （暂时无用）Mtype=1修改所有服务器地址Sname=PSname对应的所有记录为Mnewname
function ModiServsPara($Poldname,$Mnewname,$MType)
{
    $filename = AppSvrCFGFILE;
    $ModiOK=0;
    
    if (file_exists($filename))
    {
    	//add write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = ADDOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$filename;
    	/* for test --by ytao
    	echo '<html>';
	echo '<head>';
	echo "this is the $cmd:";
    	echo $cmd;
    	echo '</head>';
	echo '<body>';
	echo '</body>';
	echo '</html>';
    	*/
    	exec($cmd);
        $handle = @fopen($filename, "r");
        $Servs=array();
        $i=0;
        $j=0;
        if ($handle)
        {
          while (!feof($handle))
          {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($Modienable,$Proxyname,$Proxytype,$Svrtype,$Svrport,$Secutag,$Sname,$Sport,$Opensw,$Addtime,$denypages)=split(" ",$buffer,12);
            if (!($Proxytype>" "))
            {
                continue;
            }
            $j+=1;
            if ((strcmp($Poldname,$Sname)==0) && ($MType==1))
            {
                $Sname=$Mnewname;
            } else if ((strcmp($Poldname,$denypages)==0) && ($MType==0))
            {
                $denypages=$Mnewname;
            }
            $Servs[$i]=array("$Modienable","$Proxyname","$Proxytype","$Svrtype","$Svrport","$Secutag","$Sname","$Sport","$Opensw","$Addtime","$denypages");
            $i+=1;
          }
          fclose($handle);
        }
        $handle = @fopen($filename, "w");

        $buffer="";
        if ($handle)
        {
          for ($j = 0; $j < $i; $j++)
          {
            if ($j != 0)
            {
                $buffer="\r\n";
            } else
            {
                $buffer="";
            }

            $buffer .= $Servs[$j][0]." ".$Servs[$j][1]." ".$Servs[$j][2]." "
                .$Servs[$j][3]." ".$Servs[$j][4]." ".$Servs[$j][5]." ".$Servs[$j][6]
                ." ".$Servs[$j][7]." ".$Servs[$j][8]." ".$Servs[$j][9]." ".$Servs[$j][10];
            fwrite($handle,$buffer);

          }
          fclose($handle);
    	  //write_ed();
          $ModiOK=1;
        }
        
        //del write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = DELOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$filename;
    	/* for test --by ytao
    	echo '<html>';
	echo '<head>';
	echo "this is the $cmd:";
    	echo $cmd;
    	echo '</head>';
	echo '<body>';
	echo '</body>';
	echo '</html>';
    	*/    	
    	exec($cmd); 
    }
    //print_r($Users);
    if ($ModiOK==1)
    {
        return true;
    }  else
    {
        return false;
    }
}

//******增加一条应用服务；
function AddServs($PProxyname,$PProxytype,$PSvrtype,$PSvrport,$PSecutag,$PSname,$PSport,$POpensw,$PAddtime,$Pdenypages)
{
    $filename = AppSvrCFGFILE;
    $AddOK=0;
    $servs=GetAppSvrNameList(1);
    $rcount=count($servs);
    if (file_exists($filename))
    {
    	//add write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = ADDOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$filename;
    
    	/*
	echo "this is the $cmd:";
    	echo $cmd;
    	*/
    	
    	exec($cmd);
        if (($rcount>0) && ($servs[0]>" "))
        {
            $handle = @fopen($filename, "a");
            $MCode="\n"."1 ".$PProxyname." ".$PProxytype." ".$PSvrtype." ".$PSvrport
                ." ".$PSecutag." ".$PSname." ".$PSport." ".$POpensw." ".$PAddtime
                ." ".$Pdenypages;
        } else
        {
            $handle = @fopen($filename, "w");
            $MCode="1 ".$PProxyname." ".$PProxytype." ".$PSvrtype." ".$PSvrport
                ." ".$PSecutag." ".$PSname." ".$PSport." ".$POpensw." ".$PAddtime
                ." ".$Pdenypages;
        }
        if ($handle)
        {
            if (fwrite($handle, $MCode) == true)
            {
              $AddOK=1;
            }
            fclose($handle);
    	    //write_ed();
        }
        
        //del write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = DELOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$filename;
    	/* for test --by ytao
    	echo '<html>';
	echo '<head>';
	echo "this is the $cmd:";
    	echo $cmd;
    	echo '</head>';
	echo '<body>';
	echo '</body>';
	echo '</html>';
    	*/    	
    	exec($cmd); 

    }
    //print_r($Users);
    if ($AddOK==1)
    {
        return true;
    } else
    {
        return false;
    }
}
//--------------20051216;by yy;增加应用服务管理配置读写操作等函数

//--------------20051217;by yy;增加Denypage管理配置读写操作函数
//******  只读取Denypage名字
function GetDenysList()
{
    $filename = DENYPAGEFILE;
    $Denys=array();
    //$MANNAME[]="uuuu";
    if (file_exists($filename))
    {
        //$MANNAME[]="ooo";
        $handle = fopen($filename, "r");
        $ni=0;
        if ($handle)
        {
          while (!feof($handle))
          {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($Denyname,$Denypath)=split(" ",$buffer,12);
            //$MANNAME[]=$user;
            if ($Denyname>" ")
            {
                $nj=$ni+1;
                $Denys[]="$Denyname";
                $ni+=1;
            }
          }
          fclose($handle);
        }
    }
    else
    {
        $Denys[]="";
    }
    //print_r($MANNAME);
    return $Denys;
}

function DenypageExists($Denyname,$Denypath)
{
    $Apps=GetDenypagesList();
    $rcount=count($Apps);
    for($ii=0;$ii<$rcount;$ii++)
    {
        $name=$Apps[$ii][1];
        $path=$Apps[$ii][2];
        if ((strcmp($Denyname,$name)==0) or (strcmp($Denypath,$path)==0))
        {
            return true;
        }
    }
    return false;
}

//读取整个Denypages列表
function GetDenypagesList()
{
    $filename = DENYPAGEFILE;
    $Denyps=array();
    //$MANNAME[]="uuuu";
    if (file_exists($filename))
    {
        //$MANNAME[]="ooo";
        $handle = fopen($filename, "r");
        $ni=0;
        if ($handle)
        {
          while (!feof($handle))
          {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($Denyname,$Denypath)=split(" ",$buffer,12);
            //$MANNAME[]=$user;
            if ($Denyname>" ")
            {
                $nj=$ni+1;
                $Denyps[$ni]=array("$nj","$Denyname","$Denypath");
                $ni+=1;
            }
          }
          fclose($handle);
        }
    }
    else
    {
        $Denyps[]="";
    }
    //print_r($MANNAME);
    return $Denyps;
}

//******增加一条Denypage规则；
function AddDenyPage($PDenyname,$PDenypath)
{
    $filename = DENYPAGEFILE;
    $AddOK=0;
    $rules=GetDenypagesList();
    $rcount=count($rules);
    //if (file_exists($filename))
    {
    	//add write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = ADDOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$filename;
    	/* for test --by ytao
    	echo '<html>';
	echo '<head>';
	echo "this is the $cmd:";
    	echo $cmd;
    	echo '</head>';
	echo '<body>';
	echo '</body>';
	echo '</html>';
    	*/
    	exec($cmd);

        if (($rcount>0) && ($rules[0][1]>" "))
        {
            $handle = @fopen($filename, "a");
            $MCode="\r\n".$PDenyname.' '.$PDenypath;
        } else
        {
            $handle = @fopen($filename, "w");
            $MCode=$PDenyname.' '.$PDenypath;
        }
        if ($handle)
        {
            if (fwrite($handle, $MCode) == true)
            {
              $AddOK=1;
            }
            fclose($handle);
	    //write_ed();
        }
        //del write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = DELOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$filename;
    	/* for test --by ytao
    	echo '<html>';
	echo '<head>';
	echo "this is the $cmd:";
    	echo $cmd;
    	echo '</head>';
	echo '<body>';
	echo '</body>';
	echo '</html>';
    	*/    	
    	exec($cmd); 

    }
    //print_r($Users);
    if ($AddOK==1)
    {
        return true;
    } else
    {
        return false;
    }
}

//**** $MType用以指明删除该用户还是修改该用户密码
//**** =0:删除该条记录；1:修改该条记录；
//**** =2; 20051218;by cw;删除Denypage同时修改对应的代理服务的Denypage项为"-";
function ModiDenyPages($PRID,$PDenyname,$PDenypath,$MType)
{
    $filename = DENYPAGEFILE;
    $ModiOK=0;
    if (file_exists($filename))
    {
    	//add write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = ADDOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$filename;
    	/* for test --by ytao
    	echo '<html>';
	echo '<head>';
	echo "this is the $cmd:";
    	echo $cmd;
    	echo '</head>';
	echo '<body>';
	echo '</body>';
	echo '</html>';
    	*/
    	exec($cmd);
    	
        $handle = @fopen($filename, "r");
        $Rules=array();
        $i=0;
        $j=0;
        if ($handle)
        {
          while (!feof($handle))
          {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($name1, $path1)=split(" ",$buffer,10);
            /*
            if (((strcmp($PAppname,$Appname)==0) && (strcmp($PSysname,$Sysname)==0) && (PRID==($i+1)) && ($MType==0))
              or ((strcmp($PAppname,$Appname)==0) && ($MType==2)))
            */
            if (!($name1>" "))
            {
                continue;
            }
            $j+=1;
            if ((($PRID==$j) && ($MType==0))
              or (($PRID==$j) && ($MType==2)))
            {
                if ($MType==2)
                {
                    //20051218;by cw;删除Denypage同时修改对应的代理服务的Denypage项为"-";
                    ModiServsPara($name1,"-",0);
                }
                //20051224;by yy;删除Denypage同时删除其文件;
                //unlink(DENYPAGEFILEPATH.$path1);
		passthru(MYSUDO . "rm -rf " . DENYPAGEFILEPATH . $path1);
                continue;
            } else if (($PRID==($j)) && ($MType==1))
            {
                $name1=$PDenyname;
                $path1=$PDenypath;
            }
            $Rules[$i]=array("$name1","$path1");

            $i+=1;
          }
          fclose($handle);
        }
        $handle = @fopen($filename, "w");

        $buffer="";
        if ($handle)
        {
          for ($j = 0; $j < $i; $j++)
          {
            if ($j != 0)
            {
                $buffer="\r\n";
            } else
            {
                $buffer="";
            }

            $buffer .= $Rules[$j][0]." ".$Rules[$j][1];
            fwrite($handle,$buffer);

          }
          fclose($handle);
	  //write_ed();
          $ModiOK=1;
          
        //del write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = DELOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$filename;
    	/* for test --by ytao
    	echo '<html>';
	echo '<head>';
	echo "this is the $cmd:";
    	echo $cmd;
    	echo '</head>';
	echo '<body>';
	echo '</body>';
	echo '</html>';
    	*/    	
    	exec($cmd); 
          
        }
    }
    //print_r($Users);
    if ($ModiOK==1)
    {
        return true;
    } else
    {
        return false;
    }
}
//--------------20051217;by yy;增加Denypage管理配置读写操作函数

//--------------20051218;by cw;增加自动登陆关联文件配置读写操作函数
function GetAssignsList()
{
    $filename = ASSIGNCFGFILE;
    $Assigns=array();
    //$MANNAME[]="uuuu";
    if (file_exists($filename))
    {
        //$MANNAME[]="ooo";
        $handle = fopen($filename, "r");
        $ni=0;
        if ($handle)
        {
          while (!feof($handle))
          {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($Appname,$Assfile,$Paratype1,$Paraname1,$Certitem1,$Paraformat1,
                 $Paratype2,$Paraname2,$Certitem2,$Paraformat2,
                 $Paratype3,$Paraname3,$Certitem3,$Paraformat3)=split(" ",$buffer,15);
            //$MANNAME[]=$user;
            if ($Appname>" ")
            {
                $nj=$ni+1;
                $Assigns[$ni]=array("$nj","$Appname","$Assfile",
                    "$Paratype1","$Paraname1","$Certitem1","$Paraformat1",
                    "$Paratype2","$Paraname2","$Certitem2","$Paraformat2",
                    "$Paratype3","$Paraname3","$Certitem3","$Paraformat3");
                $ni+=1;
            }
          }
          fclose($handle);
        }
    }
    else
    {
        $Assigns[]="";
    }
    //print_r($MANNAME);
    return $Assigns;
}

//******增加一条Assignfile规则；
function AddAssignFile($Appname,$Assfile,$Paratype1,$Paraname1,$Certitem1,$Paraformat1,
                        $Paratype2,$Paraname2,$Certitem2,$Paraformat2,
                        $Paratype3,$Paraname3,$Certitem3,$Paraformat3)
{
    $filename = ASSIGNCFGFILE;
    $AddOK=0;
    $rules=GetAssignsList();
    $rcount=count($rules);
    for($rii=0;$rii<$rcount;$rii++)
    {
        if (strcmp($rules[$rii][1],$Appname)==0)
        {
            return false;
        }
    }
    if (file_exists($filename))
    {
    	
    	//add write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = ADDOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$filename;
    	/* for test --by ytao
    	echo '<html>';
	echo '<head>';
	echo "this is the $cmd:";
    	echo $cmd;
    	echo '</head>';
	echo '<body>';
	echo '</body>';
	echo '</html>';
    	*/
    	exec($cmd);
    	
        if (($rcount>0) && ($rules[0][1]>" "))
        {
            $handle = @fopen($filename, "a");
            $MCode="\r\n".$Appname.' '.$Assfile.' '
                .$Paratype1.' '.$Paraname1.' '.$Certitem1.' '.$Paraformat1.' '
                .$Paratype2.' '.$Paraname2.' '.$Certitem2.' '.$Paraformat2.' '
                .$Paratype3.' '.$Paraname3.' '.$Certitem3.' '.$Paraformat3;
        } else
        {
            $handle = @fopen($filename, "w");
            $MCode=$Appname.' '.$Assfile.' '
                .$Paratype1.' '.$Paraname1.' '.$Certitem1.' '.$Paraformat1.' '
                .$Paratype2.' '.$Paraname2.' '.$Certitem2.' '.$Paraformat2.' '
                .$Paratype3.' '.$Paraname3.' '.$Certitem3.' '.$Paraformat3;
        }
        if ($handle)
        {
            if (fwrite($handle, $MCode) == true)
            {
              $AddOK=1;
            }
            fclose($handle);            
	    //write_ed();
        }
        
        //del write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = DELOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$filename;
    	/* for test --by ytao
    	echo '<html>';
	echo '<head>';
	echo "this is the $cmd:";
    	echo $cmd;
    	echo '</head>';
	echo '<body>';
	echo '</body>';
	echo '</html>';
    	*/    	
    	exec($cmd); 

    }
    //print_r($Users);
    if ($AddOK==1)
    {
        return true;
    } else
    {
        return false;
    }
}

//**** $MType用以指明删除关联文件还是修改关联文件
//**** =0:删除该条记录；1:修改该条记录；
//**** =2;20051218;by cw;删除所有Appname=PAppname的记录；
function ModiAssignFiles($PRID,$PAppname,$PAssfile,
             $PParatype1,$PParaname1,$PCertitem1,$PParaformat1,
             $PParatype2,$PParaname2,$PCertitem2,$PParaformat2,
             $PParatype3,$PParaname3,$PCertitem3,$PParaformat3,$MType)
{
    $filename = ASSIGNCFGFILE;
    $ModiOK=0;
    if (file_exists($filename))
    {
    	//add write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = ADDOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$filename;
    	/* for test --by ytao
    	echo '<html>';
	echo '<head>';
	echo "this is the $cmd:";
    	echo $cmd;
    	echo '</head>';
	echo '<body>';
	echo '</body>';
	echo '</html>';
    	*/
    	exec($cmd);
    	
        $handle = @fopen($filename, "r");
        $Rules=array();
        $i=0;
        $j=0;
        if ($handle)
        {
          while (!feof($handle))
          {
            $buffer = fgetss($handle, 4096);
            $buffer = trim($buffer);
            list($Appname,$Assfile,
                 $Paratype1,$Paraname1,$Certitem1,$Paraformat1,
                 $Paratype2,$Paraname2,$Certitem2,$Paraformat2,
                 $Paratype3,$Paraname3,$Certitem3,$Paraformat3)=split(" ",$buffer,15);
            /*
            if (((strcmp($PAppname,$Appname)==0) && (strcmp($PSysname,$Sysname)==0) && (PRID==($i+1)) && ($MType==0))
              or ((strcmp($PAppname,$Appname)==0) && ($MType==2)))
            */
            if (!($Appname>" "))
            {
                continue;
            }
            $j+=1;
            if ((($PRID==$j) && ($MType==0))
              or ((strcmp($PAppname,$Appname)==0) && ($MType==2)))
            {

                continue;
            } else if (($PRID==($j)) && ($MType==1))
            {
                $Appname=$PAppname;
                $Assfile=$PAssfile;
                $Paratype1=$PParatype1;
                $Paraname1=$PParaname1;
                $Certitem1=$PCertitem1;
                $Paraformat1=$PParaformat1;
                $Paratype2=$PParatype2;
                $Paraname2=$PParaname2;
                $Certitem2=$PCertitem2;
                $Paraformat2=$PParaformat2;
                $Paratype3=$PParatype3;
                $Paraname3=$PParaname3;
                $Certitem3=$PCertitem3;
                $Paraformat3=$PParaformat3;
            }
            $Rules[$i]=array("$Appname","$Assfile",
                         "$Paratype1","$Paraname1","$Certitem1","$Paraformat1",
                         "$Paratype2","$Paraname2","$Certitem2","$Paraformat2",
                         "$Paratype3","$Paraname3","$Certitem3","$Paraformat3");

            $i+=1;
          }
          fclose($handle);       
          
        }
        $handle = @fopen($filename, "w");

        $buffer="";
        if ($handle)
        {
          for ($j = 0; $j < $i; $j++)
          {
            if ($j != 0)
            {
                $buffer="\r\n";
            } else
            {
                $buffer="";
            }

            $buffer .= $Rules[$j][0]." ".$Rules[$j][1]." ".$Rules[$j][2]." "
                      .$Rules[$j][3]." ".$Rules[$j][4]." ".$Rules[$j][5]." "
                      .$Rules[$j][6]." ".$Rules[$j][7]." ".$Rules[$j][8]." "
                      .$Rules[$j][9]." ".$Rules[$j][10]." ".$Rules[$j][11]." "
                      .$Rules[$j][12]." ".$Rules[$j][13];
            fwrite($handle,$buffer);

          }
          fclose($handle);
	  //write_ed();
          $ModiOK=1;
          
        //del write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = DELOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$filename;
    	/* for test --by ytao
    	echo '<html>';
	echo '<head>';
	echo "this is the $cmd:";
    	echo $cmd;
    	echo '</head>';
	echo '<body>';
	echo '</body>';
	echo '</html>';
    	*/    	
    	exec($cmd); 
          
        }
    }
    //print_r($Users);
    if ($ModiOK==1)
    {
        return true;
    } else
    {
        return false;
    }
}
//--------------20051218;by cw;增加自动登陆关联文件配置读写操作函数

function MyCopyFile($src,$dst)
{
	$cmd=SUDO."cp ".$src." ".$dst;
//	echo $cmd;
	$ret=system($cmd,$a);
	return $ret;
}

function CopyFileToElecDisk($src,$dst)
{	//dst是电子盘上的相对路径
	$cmd=SUDO.MOUNT_CMD;
	$ret=system($cmd,$a);

	$cmd=SUDO."cp ".$src." ".MOUNT_PATH.$dst;
	$ret=system($cmd,$a);

	$cmd=SUDO.UMOUNT_CMD;
	$ret=system($cmd,$a);

	return $ret;
}

function BackupFile($src)
{
	return MyCopyFile($src,$src.".bak");
}

function MyChmod($src)
{
	$ret=system(SUDO."chmod 0666 ".$src,$a);
	return $ret;
}

function MyReboot()
{
	system(SUDO."reboot -f");
}

function MyHalt()
{
	system(SUDO."halt -p");
}

function EffectEth0()
{
	system(SUDO."ifdown eth0");
	system(SUDO."ifup eth0");
}

function EffectEth1()
{
	system(SUDO."ifdown eth1");
	system(SUDO."ifup eth1");
}

function GetGateway(&$ip_addr,&$host_name)
{
	if(!file_exists(GATEWAY))
	{
		$cmd=NETCONFIG . " get gateway";
		$ip_addr=trim(exec($cmd));
		return 0;
	}
	MyChmod(GATEWAYBAK);
	$fcont=file(GATEWAY);
	$gwstr=join("&",$fcont);
	parse_str($gwstr);
	$ip_addr=$GATEWAY;
	$gw_dev=$GATEWAYDEV;
	$host_name=$HOSTNAME;


	if(strlen($ip_addr)<7)
	{
		$new_gw="/etc/sysconfig/network-scripts/ifcfg-eth0";
		MyChmod($new_gw);
		$fcont=file($new_gw);
		$gwstr=join("&",$fcont);
		parse_str($gwstr);
		$ip_addr=$GATEWAY;
		$gw_dev=$GATEWAYDEV;
		$host_name=$HOSTNAME;
	}
	return 0;
}

function SetGateway($ip_addr,$host_name)
{
	if(!file_exists(GATEWAY))
	{
		$cmd=MYSUDO . NETCONFIG . " set gateway " . $ip_addr;
		exec($cmd);
		//write_ed();
		return 0;
	}
	$gwstr="NETWORKING=yes"."\012"."HOSTNAME=".$host_name."\012"."GATEWAY=".$ip_addr."\012"."GATEWAYDEV=eth0";
	//echo "<br>".$gwstr."<br>";
	//$fp=fopen(GATEWAYBAK,"wb");
	
	//add write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = ADDOW;    	
    	$filename = GATEWAY;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$filename;
    	/* for test --by ytao
    	echo '<html>';
	echo '<head>';
	echo "this is the $cmd:";
    	echo $cmd;
    	echo '</head>';
	echo '<body>';
	echo '</body>';
	echo '</html>';
    	*/
    	exec($cmd);
	
    	$fp=fopen(GATEWAY,"wb");
	if ($fp == FALSE)
	{
		echo "错误：写网络配置文件出错！";
		die();
	}
	else
	{
		fwrite($fp,$gwstr,strlen($gwstr));
		fclose($fp);
	}
	
	//del write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmoddelow = DELOW;    	
    	$filename = GATEWAY;
    	$cmd = 0;
    	$cmd = $mysudo.$chmoddelow.$filename;
    	/* for test --by ytao
    	echo '<html>';
	echo '<head>';
	echo "this is the $cmd:";
    	echo $cmd;
    	echo '</head>';
	echo '<body>';
	echo '</body>';
	echo '</html>';
    	*/
    	exec($cmd);
	//-------------20051214；by yy;将原有函数改为PHP标准函数
	//$ret=MyCopyFile(GATEWAYBAK,GATEWAY);
	//-------------

	//-------------20051214；by yy;将原有函数改为PHP标准函数
	//$ret=Copy(GATEWAYBAK,GATEWAY);
	//-------------20051214；by yy;将原有函数改为PHP标准函数

	//echo "<br>".$ret."<br>";
	//-------------20051214;by yy;修正错误调试信息；
	//echo "<br>修改成功！<br>";

	//$ret=CopyFileToElecDisk(GATEWAYBAK,GATEWAY_DZP);

	echo "您改变了外网口的网关地址，有可能会造成访问暂时中断。";
	flush();
	
	//register_shutdown_function(EffectEth0);
	//return $ret;
}

function GetIPAddr($ethnum,&$ip_addr,&$net_mask)
{
	$cfgfile=ETH.$ethnum;
	if(file_exists($cfgfile))
	{
		$fcont=file($cfgfile);
		$cfgstr=join("&",$fcont);
		parse_str($cfgstr);
		$ip_addr=$IPADDR;
		$net_mask=$NETMASK;
		$ip_addr=trim($ip_addr);
	}
	else
	{
		$cmd=NETCONFIG . " get eth" . $ethnum . " address";
		$ip_addr=trim(exec($cmd));

		$cmd=NETCONFIG . " get eth" . $ethnum . " netmask";
		$net_mask=trim(exec($cmd));
	}
	return 1;
}

function write_ed()
{
	$cmd=MYSUDO . "/usr/local/AOS/Bin/write-ed";
	exec($cmd);
}

function SetIPAddr($ethnum,$ip_addr,$net_mask)
{
	if(!file_exists(GATEWAY))
	{

		$cmd=MYSUDO . NETCONFIG . " set eth" . $ethnum . " address ". $ip_addr;
		exec($cmd);

		$cmd=MYSUDO . NETCONFIG . " set eth" . $ethnum . " netmask " . $net_mask;
		exec($cmd);
		//write_ed();
		return 1;
	}
	$cfgstr="DEVICE=eth".$ethnum."\012"."BOOTPROTO=static"."\012"."IPADDR=".$ip_addr."\012"."NETMASK=".$net_mask."\012"."ONBOOT=yes";


	$cfgfile=ETH.$ethnum;
	
	//add write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = ADDOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$cfgfile;
   	exec($cmd);
	
    	$fp=fopen($cfgfile,"wb");	
	
	if ($fp == FALSE)
	{
		echo "错误：写网络配置文件出错！";
		die();
	}
	else
	{
		fwrite($fp,$cfgstr,strlen($cfgstr));
		fclose($fp);
	}
    	//-------------20051214；by yy;将原有函数改为PHP标准函数
    	
    	
    	//del write option to the file   --by ytao
    	$mysudo = MYSUDO;
    	$chmodaddow = DELOW;
    	$cmd = 0;
    	$cmd = $mysudo.$chmodaddow.$cfgfile;
   	exec($cmd);

    	//$ret=CopyFileToElecDisk($cfgfile,ETH_DZP.$ethnum);

	if ($ethnum == 1)
		$nw="内";
	else if ($ethnum == 0)
		$nw="外";
	{
		echo "<br><br><br>您改变了".$nw."网口IP地址，如果您正在从".$nw."网口进行访问本代理服务器，<br>";
		echo "请稍后访问<a href=http://".$ip_addr.":5680/login.html>访问新家...</a>";
		flush();
	}
}

function IsIPStr(&$str)
{
	$sss=explode(".",$str);
	$scount=count($sss);
	if ($scount!=4)
	  return -9;
	//list($s1,$s2,$s3,$s4,$s5)=split($str,".",10);
	$s1=$sss[0];
	$s2=$sss[1];
	$s3=$sss[2];
	$s4=$sss[3];
	//$s1=strtok($str,".");
  //	if (!$s1 ||
	if(!is_numeric($s1) || $s1 < 0 || $s1 > 255)
		return -1;

	//$s2=strtok(".");
	if (!is_numeric($s2) || $s2 < 0 || $s2 > 255)
		return -2;

	//$s3=strtok(".");
	if (!is_numeric($s3) || $s3 < 0 || $s3 > 255)
		return -3;

	//$s4=strtok(".");
  //	if (!$s4 ||
	if (!is_numeric($s4) || $s4 < 0 || $s4 > 255)
		return -4;
	/*	
	$s5=strtok(".");
	if ($s5===true)
		return -5;
	if (!is_numeric($s5) || $s5 < 0 || $s5 > 255)
		return -4;
	*/
	if (($s1===false) || ($s2===false) || ($s3===false) || ($s4===false))
	  return -10;

  settype($s1, "integer");
  settype($s2, "integer");
  settype($s3, "integer");
  settype($s4, "integer"); 
	$str=$s1.".".$s2.".".$s3.".".$s4;
	return 10;
}

function IsMaskNum($num)
{
	$masknum=array(255,254,252,248,240,224,192,128,0);
	foreach($masknum as $maskkey)
	{
	    if ($maskkey == $num)
	    {
	    	return true;
	    	break;
	    }
	}
	return false;
}

function IsMaskStr($str)
{
	$sss=explode(".",$str);
	$scount=count($sss);
	if ($scount!=4)
	  return -9;
	
	$s1=$sss[0];
	$s2=$sss[1];
	$s3=$sss[2];
	$s4=$sss[3];
	
	//$s1=strtok($str,".");
	if (!is_numeric($s1) || !IsMaskNum($s1))
		return -1;

	//$s2=strtok(".");
	if (!is_numeric($s2) || !IsMaskNum($s2))
		return -2;

	//$s3=strtok(".");
	if (!is_numeric($s3) || !IsMaskNum($s3))
		return -3;

	//$s4=strtok(".");
	if (!is_numeric($s4) || !IsMaskNum($s4))
		return -4;
	/*
	$s5=strtok(".");
	if ($s5)
		return -5;
	*/
	
	if (($s1!=255) && (($s2!=0) || ($s3!=0) || ($s4!=0)))
	  return -5;
	if (($s2!=255) && (($s3!=0) || ($s4!=0)))
	  return -5;
	if (($s3!=255) && ($s4!=0))
	  return -5;

	//$str=$s1.".".$s2.".".$s3.".".$s4;
	return 10;
}

function InOneSubnet($ip1,$mask1,$ip2,$mask2)
{
	$m11=strtok($mask1,".");
	settype($m11,"integer");
	$m12=strtok(".");
	settype($m12,"integer");
	$m13=strtok(".");
	settype($m13,"integer");
	$m14=strtok(".");
	settype($m14,"integer");

	$m21=strtok($mask1,".");
	settype($m21,"integer");
	$m22=strtok(".");
	settype($m22,"integer");
	$m23=strtok(".");
	settype($m23,"integer");
	$m24=strtok(".");
	settype($m24,"integer");

	$m1=max($m11,$m21);
	$m2=max($m12,$m22);
	$m3=max($m13,$m23);
	$m4=max($m14,$m24);

	$s11=strtok($ip1,".");
	settype($s11,"integer");
	$s12=strtok(".");
	settype($s12,"integer");
	$s13=strtok(".");
	settype($s13,"integer");
	$s14=strtok(".");
	settype($s14,"integer");

	$s21=strtok($ip2,".");
	settype($s21,"integer");
	$s22=strtok(".");
	settype($s22,"integer");
	$s23=strtok(".");
	settype($s23,"integer");
	$s24=strtok(".");
	settype($s24,"integer");

//	echo $m1,$m2,$m3,$m4;
//	echo $s11,$s12,$s13,$s14;
//	echo $s21,$s22,$s23,$s24;

	if ((($m1 & $s11) != ($m1 & $s21)) || (($m2 & $s12) != ($m2 & $s22)) ||
		(($m3 & $s13) != ($m3 & $s23)) || (($m4 & $s14) != ($m4 & $s24)))
		return false;
	else
		return true;
}

function IsPortInUse($confstr,$port,&$line)
{
	if (!is_array($confstr))
	{
		$line="";
		return -1;
	}

	while (list($key_pro,$line_pro) = each($confstr))
	{
		$tok=strtok($line_pro," ");
		$i=0;
		while ($tok)
		{
//			print("$tok<br>");
//			if ($i == 1 && $tok == $port)
			if ($i == LOCAL_PORT_PROFILE && $tok == $port)
			{
				$line=$line_pro;
				return $key_pro;
			}
			$tok=strtok(" ");
			$i++;
		}
	}
	$line="";
	return -1;
}

function SortArrayByX($src_arr , $sortby)
{
	$dst_arr=array();

	if (!is_array($src_arr))
	{
		return $dst_arr;
	}

	while (list($key_pro,$line_pro) = each($src_arr))
	{
		$tok=strtok($line_pro," ");
		$i=0;
		while ($tok)
		{
			if ($i == $sortby)
			{
				$dst_arr[$tok]=$line_pro;
				break;
			}
			$tok=strtok(" ");
			$i++;
		}
	}
	ksort($dst_arr);
	return $dst_arr;
}

function MakeSSLConfLine($svname,$svport,$verifycert,$svrip,$svrport)
{
	$str=$svname." ".$svport." ".$verifycert." ".$svrip." ".$svrport." ".$svrip." ".$svrport;
	return $str;
}

function AddArrayItem($src_arr , $item)
{
	$dst_arr=$src_arr;

	if (!is_array($src_arr))
	{
		return $dst_arr;
	}
	$count=count($src_arr);
//	echo "count=".$count."<br>";
	$dst_arr[$count]=$item;

	return $dst_arr;
}

function EditArrayItem($src_arr , $index , $item)
{
	$dst_arr=$src_arr;

	if (!is_array($src_arr))
	{
		return $dst_arr;
	}
	$count=count($src_arr);
	if ($count <= $index)
		return $dst_arr;
	$dst_arr[$index]=$item;

	return $dst_arr;
}

function DelArrayItem($src_arr , $index)
{
	$dst_arr=array();

	if (!is_array($src_arr))
	{
		return $dst_arr;
	}
	$count=count($src_arr);
	if ($count <= $index)
		return $dst_arr;
	for ($i=0,$j=0;$i<$count;$i++)
	{
		if ($i != $index)
		{
			$j++;
			$dst_arr[$j]=$src_arr[$i];
		}
	}

	return $dst_arr;
}

function AddSSLConfArray($src_arr , $item)
{
	return AddArrayItem($src_arr , $item);
}

function EditSSLConfArray($src_arr , $port , $item)
{
	$linenum=IsPortInUse($src_arr,$port,$line);
	if ($linenum != -1)
	{
		return EditArrayItem($src_arr , $linenum , $item);
	}
	else
		return $src_arr;
}

function DelSSLConfArray($src_arr , $port)
{
	$linenum=IsPortInUse($src_arr,$port,$line);
	if ($linenum != -1)
	{
		return DelArrayItem($src_arr , $linenum);
	}
	else
		return $src_arr;
}

function WriteArray2SSLConfFile($fname,$arr)
{
	if (!is_array($arr))
	{
		return false;
	}

	$arr=SortArrayByX($arr,1);
	$fp=fopen($fname,"wb");
	if ($fp == false)
	{
		echo "文件".$fname."打开失败";
		return false;
	}
	else
	{
		$count=count($arr);
		$i=0;
		while (list($k,$v) = each($arr))
		{
			$i++;
			$arr[$k]=str_replace("\015\012","",$v);//strtr($v,"\015\012\015\012","\015\012");
			if (strlen(ltrim($arr[$k])) >= 9)//!= 0)
			{
				echo "wolaile".$arr[$k];

				fwrite($fp,$arr[$k],strlen($arr[$k]));
				if ($i < $count)
					fwrite($fp,"\015\012",2);
			}
			else
				echo "空行"."<br>";
		}
//		$str=join("\015\012",$arr);
//		fwrite($fp,$str,strlen($str));
		fclose($fp);
		return true;
	}
}

function GetCaTxt(&$ca_pol,
	&$ca_ldapgap,
	&$ca_ldapport,
	&$ca_ocspport,
	&$ca_ocsponldapsuc,
	&$ca_ldapipstr,
	&$ca_ocspipstr,
	&$ca_ldapport2,
	&$ca_ldapipstr2)
{
	$fcont=file(CATXTFILE);
	list($key_pro,$catxtstr) = each($fcont);

	$ca_pol=strtok($catxtstr," ");
//	print($ca_pol.$ln);
	$ca_ldapgap=strtok(" ");
//	print($ca_ldapgap.$ln);
	$ca_ldapport=strtok(" ");
	$ca_ocspport=strtok(" ");
//	print($ca_ocspport.$ln);
	$ca_ocsponldapsuc=strtok(" ");
	$ca_ldapipstr=strtok(" ");
	$ca_ocspipstr=strtok(" ");
//	print($ca_ocspipstr.$ln);
	$ca_ldapport2=strtok(" ");
	$ca_ldapipstr2=strtok(" ");
}

function SetCaTxt($ca_pol,
	$ca_ldapgap,
	$ca_ldapport,
	$ca_ocspport,
	$ca_ocsponldapsuc,
	$ca_ldapipstr,
	$ca_ocspipstr,
	$ca_ldapport2,
	$ca_ldapipstr2)
{
	$fp=fopen(CATXTFILE,"wb");
	if ($fp == FALSE)
		return FALSE;
	$catxtstr=$ca_pol." ".$ca_ldapgap." ".$ca_ldapport." ".$ca_ocspport." ".$ca_ocsponldapsuc." ".$ca_ldapipstr." ".$ca_ocspipstr." ".$ca_ldapport2." ".$ca_ldapipstr2;
//	print($catxtstr.$ln);
	fwrite($fp,$catxtstr,strlen($catxtstr));
	fclose($fp);
	return TRUE;
}

function ExecUserCmd($cmdline)
{
	$ret=system(SUDO.$cmdline,$a);
	echo $cmdline."<br>".$a."<br>".$ret."<br>"."<br>";
}

function AosProxyAddCli($proxyname,$proxytype,$svrstype,$svrsport,$Msecstag,$asaddr,$asport,$openstat,$addtime,$denypage)
{
	if(($svrstype=="smtp")or($svrstype=="SMTP")) 	$svrstype="http";
	if(($svrstype=="pop3")or($svrstype=="POP3")) 	$svrstype="http";
	if(($svrstype=="tcp")or($svrstype=="TCP"))		$svrstype="http";
	if($svrstype=="yzw")	$svrstype="jns"; 
	if($svrstype=="HTTP")	$svrstype="http"; 
	if($svrstype=="FTP")	$svrstype="ftp"; 
	
	// 20060110, 增加“添加应用服务操作簇
	if (($proxytype=="forward") or ($proxytype=="Forward"))
	{
	  	GetIPAddr(0,$ip_eth0,$mask_eth0);
	  	$cmd=MYSUDO.MYCLI."\\\"app proxy add $proxyname $svrstype $ip_eth0 $svrsport ssl\\\"";
	  	exec($cmd,$a);
	    $cmd=MYSUDO.MYCLI."\\\"app proxy rs add $proxyname $asaddr $asport\\\"";
		exec($cmd,$a);
	  	$cmd=MYSUDO.MYCLI."\\\"app proxy ssl clientauth $proxyname on\\\"";
	  	exec($cmd,$a); 
	  
	  	if ($denypage != "-")
		{ 
			$cmd=MYSUDO.MYCLI."\\\"deny page assign $proxyname $denypage\\\"";
			exec($cmd,$a);
		} 
	                  	
		$pmi_rules=GetPMIRules();
		$countofrule=count($pmi_rules);
		for ($i=0;$i<$countofrule;$i++)
		{
			if ($pmi_rules[$i][1]==$proxyname)
			{
				$appnametmp=$pmi_rules[$i][1];
				$ruleorgnametmp=$pmi_rules[$i][2];
				$rulesysnametmp=$pmi_rules[$i][3];
				$rulestatustmp=$pmi_rules[$i][4]; 
		  		$cmd=MYSUDO.MYCLI."\\\"app proxy set pmi $appnametmp $rulestatustmp\\\"";
				exec($cmd,$a);
				$cmd=MYSUDO.MYCLI."\\\"app proxy pmi sysname $appnametmp $rulesysnametmp\\\"";
				exec($cmd,$a);
				break;
			}
		}                  	
	
	  	if (($openstat=="on") or ($openstat=="ON"))
		{
			$cmd=MYSUDO.MYCLI."\\\"app proxy set status $proxyname $openstat\\\"";
			exec($cmd,$a);
		}                        
	} 
	
	if (($proxytype=="backward") or ($proxytype=="Backward"))
	{
	  	
		
		GetIPAddr(1,$ip_eth1,$mask_eth1);
	  	if($svrstype=="dhttp")
	  	{
	  		$cmd=MYSUDO.MYCLI."\\\"app proxy add $proxyname http $ip_eth1 $svrsport plain\\\"";
	  		exec($cmd,$a);
	  	}
	  	else
	  	{
	  		$cmd=MYSUDO.MYCLI."\\\"app proxy add $proxyname $svrstype $ip_eth1 $svrsport plain\\\"";
	  		exec($cmd,$a);
	  	}
	  	if ($asaddr!="-")
		{
	  		$cmd=MYSUDO.MYCLI."\\\"app proxy rs add $proxyname $asaddr $asport\\\"";
	  		exec($cmd,$a);
	  	}
	  	                      
	  	$cmd=MYSUDO.MYCLI."\\\"app proxy ssl backend $proxyname on\\\"";
	  	exec($cmd,$a);  
	  
	  	if($svrstype=="dhttp" || $svrstype == "jns")
	  	{
	  		$cmd=MYSUDO.MYCLI."\\\"app proxy forward table $proxyname on\\\"";
	  		exec($cmd,$a);
	  	}
	      
	  	if (($openstat=="on") or ($openstat=="ON"))
		{
			$cmd=MYSUDO.MYCLI."\\\"app proxy set status $proxyname $openstat\\\"";
			exec($cmd,$a);
		}                            
	}
}

function GetUsbkeyUserList()
{
    $filename = USBKEY_USER_CFGFILE;
    $usbkey_user = array();

    if (!file_exists($filename))
    {
    	$cmd = MYSUDO."touch ".$filename;
		exec($cmd);
        $usbkey_user[] = "";
        return $usbkey_user;
    }
     
    $handle = fopen($filename, "r");
    $ni=0;
	if ($handle)
	{
		while (!feof($handle))
		{
			$buffer = fgetss($handle, 4096);
			$buffer = trim($buffer);
			list($userid,$username,$department) = split(" ",$buffer,10);
			if ($userid > " ")
			{
		    	$nj=$ni+1;
		    	$usbkey_user[$ni] = array("$nj","$userid","$username","$department");
		    	$ni+=1;
			}
		}
		fclose($handle);
	}

    return $usbkey_user;
}
function IsUsbKeyUserExist($numberu)
{
    $filename = USBKEY_USER_CFGFILE;
    if (!file_exists($filename))
    {
    	$cmd = MYSUDO."touch ".$filename;
		exec($cmd);
        return false;
    }
    $handle = fopen($filename, "r");
	if ($handle)
	{
		while (!feof($handle))
		{
			$buffer = fgetss($handle, 4096);
			$buffer = trim($buffer);
			list($number,$username,$department) = split(" ",$buffer,10);
			if (strcmp($numberu, $number) == 0)
			{
				fclose($handle);
		    	return true;
			}
		}
		fclose($handle);
	}

    return false;

}

function AosAddUsbKeyUser($usernameu,$departmentu,$numberu)
{
    $filename = USBKEY_USER_CFGFILE;
    if (!file_exists($filename))
    {
    	$cmd = MYSUDO."touch ".$filename;
		exec($cmd);
    }
    AosAddWriteOption($filename);
    $handle = fopen($filename, "a+");
	if (!$handle)
		return false;
	$buffer = $numberu." ".$usernameu." ".$departmentu."\n";
	if (fwrite($handle, $buffer) != true)
	{
		fclose($handle);
		return false;
	}
	fclose($handle);
	//write_ed();
	AosDelWriteOption($filename);
    return true;
}

function AosUsbkeyDelUser($number)
{
	$filename = USBKEY_USER_CFGFILE;
	if (!file_exists($filename))
		return false;
	
	//add write option to the file
	AosAddWriteOption($filename);
		
	$handle = @fopen($filename, "r");
	if (!$handle)
	{
		AosDelWriteOption($filename);
		return false;
	}
	$users = array();
	$remain_counts = 0;
	
	while (!feof($handle))
	{
		$buffer = fgetss($handle, 4096);
		$buffer = trim($buffer);
		list($userid, $username, $department) = split(" ", $buffer, 10);
		if (!($userid > " ")) continue;
		if ( strcmp($userid, $number) == 0 ) continue;
		$users[$remain_counts]=array("$userid", "$username", "$department");
		$remain_counts += 1;
	}
	fclose($handle);

	$handle = @fopen($filename, "w");
	if (!$handle)
	{
		AosDelWriteOption($filename);
		return false;
	}
	for ($i = 0; $i < $remain_counts; $i++)
	{
	    $buffer="";
	    $buffer .= $users[$i][0]." ".$users[$i][1]." ".$users[$i][2]."\n";
	    fwrite($handle,$buffer);
	}
	fclose($handle);

	//write_ed();
    //del write option to the file
	AosDelWriteOption($filename);
    return true;
}
?>
