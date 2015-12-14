<?php
	include ("constant.php");

	@session_start();
	if($manager_ID==NULL)
	{
	  	echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '请先登陆！<p>';
	    echo '<a href="login.html">返回</a>';
	    echo '</b></body></html>';
	    exit();
	}
	echo '<html><head><title>应用服务配置修改</title></head><body background="image/pmiback.gif"><center><font face="楷体_GB2312"><b>' ;
	GetIPAddr(1,$ip_eth1,$mask_eth1);
    if (!VarIsValid($proxyname,1))
    {
        echo "代理名称中不允许空格存在，请修改后再操作！";
    } 
    else if(!VarIsValid($svrsport,3))
    {
        echo "服务端口中不允许空格或非数字存在，请修改后再操作！";
    } 
    else if(($asport!="") && (!VarIsValid($asport,3)))
    {
        echo "后台服务端口中不允许空格或非数字存在，请修改后再操作！";
    } 
    else if(($asaddr!="") && ((!VarIsValid($asaddr,1)) or (IsIPStr($asaddr)!=10)))
    {
    	if ($proxytype == "forward")
        	echo "后台服务器地址格式不正确或有空格存在，请修改后再操作！";
        else 
        	echo "对方PWL517外口IP地址格式不正确或有空格存在，请修改后再操作！";
    } 
    else if(($proxytype == "forward") && 
    		(!isset($ensure)) && 
    		(!InOneSubnet($ip_eth1,$mask_eth1,$asaddr,$mask_eth1)))
    {
    
		//不在同一网段
		echo '<html><head><title>错误信息</title></head><body  background="image/pmiback.gif"><center>';
		echo "<font face=\"楷体_GB2312\" size=4 color=\"#0a8484\"><b>配置代理服务</b></font><br>";
		echo "正在配置的后台服务器IP地址".$asaddr."与内网口IP地址".$ip_eth1."<br>";
		echo "不在同一网段内，代理服务器可能无法提供该项服务！<br>";
		echo "如果你坚持要进行配置，请点击";
		echo "<a href=\"server_modiok.php?proxyname=$proxyname&proxytype=$proxytype&svrstype=$svrstype&svrsport=$svrsport&Msecstag=$Msecstag&asaddr=$asaddr&asport=$asport&openstat=$openstat&denypage1=$denypage1&opcode=$opcode&ensure=true&RuleID=$RuleID&adtime=$adtime\">确认</a><br>";
		echo "否则请点击浏览器的回退进行修改或点击<a href=\"server.php?numb=1\">设置代理服务</a><br></center>";
		echo '</body></html>';
		exit();
	}
    else
    {
    	if ($asport=="") $asport="-";
    	if ($asaddr=="") $asaddr="-";
		if ($opcode==1)
		{
        	if (!AppSvrsExists($proxytype,$proxyname,$svrsport,0,6))
          	{
            	$addtime=date('Y/m/d--H:i:s');
              
              	if (($proxytype=="forward") or ($proxytype=="Forward"))
              	{
              		$Msecstag="ssl";
              	} else
              	{
                	$Msecstag="plain";
              	}
              	$ret=AddServs($proxyname,$proxytype,$svrstype,$svrsport,$Msecstag,$asaddr,$asport,$openstat,$addtime,$denypage1);
              	if ($ret)
              	{
              		if (($proxytype=="forward") or ($proxytype=="Forward"))
						echo "代理服务已添加成功！"."<br>\n";
                  	else
                  		echo "反向代理服务已添加成功！"."<br>\n";
                  	//------------20060101;by yt;增加“添加应用服务操作簇”；
                  	AosProxyAddCli($proxyname,$proxytype,$svrstype,$svrsport,$Msecstag,$asaddr,$asport,$openstat,$addtime,$denypage1);
              	} 
              	else
              	{
                	echo "代理服务添加没能成功，读写文件可能失败！"."<br>\n";
              	}
          	} 
          	else
          	{
            	echo "代理名称为<font color=\"#0000FF\"> $proxyname </font><br> 或 <br>\n";
              
              	if (($proxytype=="forward") or ($proxytype=="Forward"))
              	{
                	echo "<font color=\"#0000FF\"><正向></font>代理类型中，\n";
              	}
              	if (($proxytype=="backward") or ($proxytype=="Backward"))
              	{
                	echo "<font color=\"#0000FF\"><反向></font>代理类型中，\n";
              	}
              	echo "服务端口为<font color=\"#0000FF\"> $svrsport </font><br>的代理服务已经存在，\n";
              	echo "<br>请重新填写代理名称或服务端口！"."<br>\n";
          	}
      	} 
      	else if($opcode==2)
      	{
        	if (!AppSvrsExists($proxytype,$proxyname,$svrsport,$RuleID,2))
        	{
            	if (($proxytype=="forward") or ($proxytype=="Forward"))
            	{
               		$Msecstag="ssl";
            	} else
            	{
               		$Msecstag="plain";
            	}
            	//$addtime=date('Y/m/d--H:i:s');
            	//ModiServs($PRID,$PProxyname,$PProxytype,$PSvrtype,$PSvrport,$PSecutag,$PSname,$PSport,$POpensw,$PAddtime,$Pdenypages,$MType)
            	$ret=ModiServs($RuleID,$proxyname,$proxytype,$svrstype,$svrsport,$Msecstag,$asaddr,$asport,$openstat,$adtime,$denypage1,1);
            	if ($ret)
            	{
            		if (($proxytype=="forward") or ($proxytype=="Forward"))
              			echo "第 $RuleID 条代理服务已修改成功！"."<br>";
              		else
                		echo "第 $RuleID 条反向代理服务已修改成功！"."<br>";
                
              		//exec the Cli.exe commands -- add by ytao
              		$cmd=MYSUDO.MYCLI."\\\"app proxy set status $proxyname off\\\"";
              		exec($cmd,$a);
              
              		$cmd=MYSUDO.MYCLI."\\\"app proxy remove $proxyname\\\"";
              		exec($cmd,$a);
                
                  	//------------20060101;by yt;增加“添加应用服务操作簇”；
                  	AosProxyAddCli($proxyname,$proxytype,$svrstype,$svrsport,$Msecstag,$asaddr,$asport,$openstat,$addtime,$denypage1);

            	} else
            	{
            		if (($proxytype=="forward") or ($proxytype=="Forward"))
                		echo "代理服务修改没能成功，读写文件可能失败！"."<br>";
             		else
             			echo "反向代理服务修改没能成功，读写文件可能失败！"."<br>";
            	}
        	} else
        	{
            	if (($proxytype=="forward") or ($proxytype=="Forward"))
            	{
                	echo "<font color=\"#0000FF\"><正向></font>代理类型中，";
            	}
            	if (($proxytype=="backward") or ($proxytype=="Backward"))
            	{
                	echo "<font color=\"#0000FF\"><反向></font>代理类型中，";
            	}
            	echo "服务端口<font color=\"#0000FF\"> $svrsport </font>已经存在，";
            	echo "<br>请重新填写服务端口！"."<br>";
        	}
      	}
    }

	if (($proxytype=="forward") or ($proxytype=="Forward"))
		echo "<p></b></font><a href=\"server.php?numb=$numb&apptype=$proxytype\">";
	else
		echo "<p></b></font><a href=\"revserver.php?numb=$numb&apptype=$proxytype\">";
	echo "<img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\">";
	echo "</a><br></center></body></html>";
?>