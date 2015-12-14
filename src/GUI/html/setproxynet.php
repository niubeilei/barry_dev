<?php
	include ("constant.php");

//echo "opcode=".$opcode."<br>";
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '请先登陆！<p>';
	    echo '<a href="login.html">返回</a>';
            echo '</b></body></html>';
            exit();			
	}
	echo '<html><head><title>信息服务</title></head><body  background="image/pmiback.gif"><center><font face="楷体_GB2312"><b>' ;
	if ($opcode == 1)
	{
		if (IsIPStr($ip_eth1) != 10)
		{
			echo "内网口的IP地址输入有误，请仔细检查！"."<br>";
			echo "<p></b></font><a href=\"parameter.php\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";
			exit();
		}
		if (IsMaskStr($mask_eth1)!=10)
		{
			echo "内网口的IP地址掩码输入有误，请仔细检查！"."<br>";
			echo "<p></b></font><a href=\"parameter.php\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";
			exit();
		}
		$ret=SetIPAddr(1,$ip_eth1,$mask_eth1);
		
		//------------20060101;by yt
	  $cmd=MYSUDO.REBOOTSERV;
		exec($cmd,$a);
		$cmd=MYSUDO . "echo 'Listen 127.0.0.1:5680' \> /etc/apache2/ports.conf";
		exec($cmd);
		$cmd=MYSUDO . "echo 'Listen " . $ip_eth1 . ":5680' \>\> /etc/apache2/ports.conf";
		exec($cmd);
		write_ed();
		$cmd=MYSUDO . " /sbin/reboot -f";
		echo passthru($cmd);
		echo "修改成功<p>\n";
		echo "reboot ......<p>\n";
	  
	}

	if ($opcode == 2)
	{
		if (IsIPStr($ip_eth0) != 10)
		{
			echo "外网口的IP地址输入有误，请仔细检查！"."<br>";
			echo "<p></b></font><a href=\"parameter.php\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";
			exit();
		}
		if (IsMaskStr($mask_eth0)!=10)
		{
			echo "外网口的IP地址掩码输入有误，请仔细检查！"."<br>";
			echo "<p></b></font><a href=\"parameter.php\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";
			exit();
		}
		if (!isset($ensure))
		{
			GetGateway($ip_gateway,$host_name);
			if (InOneSubnet($ip_eth0,$mask_eth0,$ip_gateway,$mask_eth0))
				$ensure=true;
			else
			{
				echo "修改外网口IP地址<br>";
				echo "外网口IP地址".$ip_eth0."与外网口网关IP地址".$ip_gateway."<br>";
				echo "不在同一网段内，可能会影响代理服务器被访问！<br>";
				echo "如果你坚持要进行修改，请点击<a href=\"setproxynet.php?ip_eth0=$ip_eth0&mask_eth0=$mask_eth0&opcode=2&ensure=true\">确认</a><br>";
				echo "<p></b></font><a href=\"parameter.php\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";
				exit();
			}
		}
		if ($ensure)
			$ret=SetIPAddr(0,$ip_eth0,$mask_eth0);
			//------------20060101;by yt
		$cmd=MYSUDO."/etc/init.d/networking restart";
	  	exec($cmd,$a);
	  	$cmd=MYSUDO.REBOOTSERV;
			//echo $cmd;
			exec($cmd,$a);
			write_ed();
		echo "修改成功<p>\n";
	}

	if ($opcode == 3)
	{
		if (IsIPStr($ip_gateway) != 10)
		{
			echo "外网口的网关IP地址输入有误，请仔细检查！"."<br>";
			echo "<p></b></font><a href=\"parameter.php\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";
			exit();
		}
//		echo $ip_gateway,$host_name;
		if (!isset($ensure))
		{
			GetIPAddr(0,$ip_eth0,$mask_eth0);
			if (InOneSubnet($ip_eth0,$mask_eth0,$ip_gateway,$mask_eth0))
				$ensure=true;
			else
			{
				echo "修改网关IP地址<br>";
				echo "外网口IP地址".$ip_eth0."与外网口网关IP地址".$ip_gateway."<br>";
				echo "不在同一网段内，可能会影响代理服务器被访问！<br>";
				echo "如果你坚持要进行修改，请点击<a href=\"setproxynet.php?ip_gateway=$ip_gateway&host_name=$host_name&opcode=3&ensure=true\">确认</a><br>";
				echo "<p></b></font><a href=\"parameter.php\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";
			exit();
			}
		}
		if ($ensure)
			$ret=SetGateway($ip_gateway,$host_name);
			
			//------------20060101;by yt
			//$cmd=MYSUDO."route add default gw $ip_gateway";
		$cmd=MYSUDO."/etc/init.d/networking restart";
	  	exec($cmd,$a);
		echo "修改成功<p>\n";
		/*
		$cmd=MYSUDO."route add default gw ". $ip_gateway;
		exec($cmd,$a);
		*/
			
	}

	echo "<p></b></font><a href=\"parameter.php\"><img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a><br></center></body></html>";
?>
