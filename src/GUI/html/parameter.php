<?php
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>登陆失败</title></head><body  background="image/pmiback.gif"><b>' ;
	    echo '<p><br><br><center><font face="楷体_GB2312"><b>请先登陆！</font></b><p>';
	    echo '<a href="login.html"><img border="0" src="image/goback.gif" width="40" height="40"></a>';
            echo '</center></body></html>';
            exit();
	}
?>
<html>

<head>
<meta name="GENERATOR" content="Microsoft FrontPage 5.0">
<meta name="ProgId" content="FrontPage.Editor.Document">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>网络参数设置</title>
<STYLE>
   .userData {behavior:url(#default#userdata);}
</STYLE>
</head>

<body bgcolor="#eef4f4">


<table border="0" cellspacing="1" width="100%" id="AutoNumber2" height="480">
  <tr>
   <td width="100%" height="480">

<table border="0" cellspacing="0" width="100%" style="border-collapse: collapse">
  <tr>
    <td width="50%">
    <div align="center"><img src="image/ocsphead.gif" width="283" height="57" border="0" align="top">
	<?php
	    include("constant.php");
	    GetOCSP($ocsp_onoff,$ocsp_addr,$ocsp_port);

	    echo "<form name=\"setocsp\" method=post action=\"setcaip_ocsp.php\">";
		if ($ocsp_onoff==true)
		{
		    echo "<label><input type=radio name=ocspRadioGroup value=on checked><font face=\"楷体_GB2312\">开启</font></label>";
    		echo "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	    	echo "<label><input type=radio name=ocspRadioGroup value=off><font face=\"楷体_GB2312\">关闭</font></label>";
		} else
		{
		    echo "<label><input type=radio name=ocspRadioGroup value=on><font face=\"楷体_GB2312\">开启</font></label>";
    		echo "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	    	echo "<label><input type=radio name=ocspRadioGroup value=off checked><font face=\"楷体_GB2312\">关闭</font></label>";
		}
		echo "<br><br><font face=\"楷体_GB2312\">地址</font>";
		echo "<input name=ocsp_addr type=text value=$ocsp_addr size=20 maxlength=18><br>";
		echo "<font face=\"楷体_GB2312\">端口</font><input name=ocsp_port type=text value=$ocsp_port size=20 maxlength=6><p>";
		echo "<input type=submit name=ocsp_setip value=修改>&nbsp;&nbsp;&nbsp;<input type=reset name=ocsp_reset value=取消>";
		echo "<br><br><br><br><br><br><br></form>";
	?>

    </td>
    <td width="50%"><div align="center"><img border="0" src="image/pmiblhead.gif" width="283" height="57" align="middle">
        　
    <?php
	    //include("constant.php");
	
	    //20051217;增加Onoff;
	    GetPMICfg($pmicfg_ldap_port,$pmicfg_subtree,$pmicfg_ip,$pmicfg_port,$pmicfg_dn,$pmicfg_ca_addr,$pmicfg_ca_port,$pmicfg_ca_dn);
	    AosGetPMI($pmi_onoff,$pmi_addr,$pmi_port,$pmi_org);
		
	    echo "<form name=\"setPMI\" method=post action=\"setpminet.php\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	    if ($pmi_onoff=="on")
		{
		    echo "<label><input type=radio name=pmiRadioGroup value=on checked><font face=\"楷体_GB2312\">开启</font></label>";
			echo "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	    	echo "<label><input type=radio name=pmiRadioGroup value=off><font face=\"楷体_GB2312\">关闭</font></label>";
		} else
		{
		    echo "<label><input type=radio name=pmiRadioGroup value=on><font face=\"楷体_GB2312\">开启</font></label>";
			echo "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	    	echo "<label><input type=radio name=pmiRadioGroup value=off checked><font face=\"楷体_GB2312\">关闭</font></label>";
		}
		echo "<br><font face=\"楷体_GB2312\">LDAP服务器端口</font><input type=text name=pmi_ldap_port value=\"$pmicfg_ldap_port\" size=30 maxlength=6><br>";
		echo "<font face=\"楷体_GB2312\">PMISubTree&nbsp;&nbsp;&nbsp;&nbsp;</font><input type=text name=pmi_subtree value=\"$pmicfg_subtree\" size=30 maxlength=256><br>";
	    echo "<font face=\"楷体_GB2312\">PMI服务器IP&nbsp;&nbsp;&nbsp;</font><input type=text name=pmi_addr value=\"$pmi_addr\" size=30 maxlength=18><br>";
	    echo "<font face=\"楷体_GB2312\">PMI服务器端口&nbsp;</font><input type=text name=pmi_port value=\"$pmi_port\" size=30 maxlength=6><br>";
	    echo "<font face=\"楷体_GB2312\">PMIBaseDN&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font><input type=text name=pmi_org value=\"$pmi_org\" size=30 maxlength=256><br>";
	    echo "<font face=\"楷体_GB2312\">CA服务器IP&nbsp;&nbsp;&nbsp;&nbsp;</font><input type=text name=pmi_ca_addr value=\"$pmicfg_ca_addr\" size=30 maxlength=18><br>";
	    echo "<font face=\"楷体_GB2312\">CA服务器端口&nbsp;&nbsp;</font><input type=text name=pmi_ca_port value=\"$pmicfg_ca_port\" size=30 maxlength=6><br>";
	    echo "<font face=\"楷体_GB2312\">CABaseDN&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font><input type=text name=pmi_ca_dn value=\"$pmicfg_ca_dn\" size=30 maxlength=256><p>";
	    
	    
	    echo "<input type=submit name=pmi_setip value=修改>";
	    echo "&nbsp;&nbsp;&nbsp;<input type=reset name=pmi_reset value=取消></form>";
	?>
    </td>
  </tr>
  <tr>
    <!--//*************20051230;by cw;增加CRL服务器设置功能-->
    <td width="50%"><div align="center"><img border="0" src="image/urlshead.gif" width="283" height="57" align="top">
        　
        <?php
	    //include("constant.php");

        //20051217;增加Onoff;
	    GetCRL($crl_onoff,$crl_addr,$crl_port,$crl_url,$crl_time);

	    echo "<form name=\"setCRL\" method=post action=\"setcrl.php\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        if ($crl_onoff=="on")
		{
		    echo "<label><input type=radio name=crl_onoff value=\"on\" checked><font face=\"楷体_GB2312\">开启</font></label>";
    		echo "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	    	echo "<label><input type=radio name=crl_onoff value=\"off\"><font face=\"楷体_GB2312\">关闭</font></label>";
		} else
		{
		    echo "<label><input type=radio name=crl_onoff value=\"on\"><font face=\"楷体_GB2312\">开启</font></label>";
    		echo "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	    	echo "<label><input type=radio name=crl_onoff value=\"off\" checked><font face=\"楷体_GB2312\">关闭</font></label>";
		}
        echo "<br><font face=\"楷体_GB2312\">地址&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font><input type=text name=crl_addr value=\"$crl_addr\" size=20 maxlength=18><br>";
        echo "<font face=\"楷体_GB2312\">端口&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font><input type=text name=crl_port value=\"$crl_port\" size=20 maxlength=6><br>";
	    echo "<font face=\"楷体_GB2312\">URL&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font><input type=text name=crl_url value=\"$crl_url\" size=20 maxlength=512><br>";
	    echo "<font face=\"楷体_GB2312\">超时时间(分钟)</font><input type=text name=crl_time value=\"$crl_time\" size=20 maxlength=4><p>";
	    echo "<input type=submit name=crl_setip value=修改>";
	    echo "&nbsp;&nbsp;&nbsp;<input type=reset name=crl_reset value=取消></form>";
        ?>
      </div></td>
    <!--*************20051230;by cw;增加CRL服务器设置功能-->
    
    <td width="50%"><div align="center"><img border="0" src="image/para1.gif" width="283" height="57" align="top">
        
        <?php
	    //include("constant.php");
     	GetIPAddr(1,$ip_eth1,$mask_eth1);

    	echo "<form name=\"seteth1\" method=post >";
    	echo "<font face=\"楷体_GB2312\">IP地址&nbsp;&nbsp;</font><input type=text name=ip_eth1 value=$ip_eth1 size=20 maxlength=18><br>";
    	echo "<font face=\"楷体_GB2312\">子网掩码</font><input type=text name=mask_eth1 value=$mask_eth1 size=20 maxlength=18><p>";
    	echo "<input type=submit name=eth1_setip value=修改 onclick=\"reboot()\">";
    	echo "&nbsp;&nbsp;&nbsp;<input type=reset name=eth1_reset value=取消></form>";
        ?>
        <br><br><br><br>
      </div></td>
  </tr>
  <tr >
    <td width="50%"><div align="center"><img border="0" src="image/para2.gif" width="283" height="57" align="top">
      </div>
      <center>
        <?php
	    GetIPAddr(0,$ip_eth0,$mask_eth0);

    	echo "<form name=\"seteth0\" method=post action=\"setproxynet.php?opcode=2\">";
      	echo " <font face=\"楷体_GB2312\">IP地址&nbsp;&nbsp;</font><input type=text name=ip_eth0 value=$ip_eth0 size=20 maxlength=18><br>";
     	echo "<font face=\"楷体_GB2312\">子网掩码</font><input type=text name=mask_eth0 value=$mask_eth0 size=20 maxlength=18><p>";
     	echo "<input type=submit name=eth0_setip value=修改>";
     	echo "&nbsp;&nbsp;&nbsp;<input type=reset name=eth0_reset value=取消></form>";
        ?>
  　
      </center></td>
    <td width="50%"><div align="center"><img border="0" src="image/para3.gif" width="283" height="57" align="top">
       </div><center>
       <?php
	GetGateway($ip_gateway,$host_name);
	if($ip_gateway==NULL)
		$ip_gateway="空";
	echo "<form name=\"setgateway\" method=post action=\"setproxynet.php?opcode=3\">";
	echo "<font face=\"楷体_GB2312\">网关IP&nbsp;&nbsp;</font><input type=text name=ip_gateway value=".$ip_gateway." size=20 maxlength=18><br>";
	echo "<input type=hidden name=host_name value=$host_name size=20 maxlength=18><p>";
	echo "<input type=submit name=gateway_setip value=修改>";
	echo "&nbsp;&nbsp;&nbsp;<input type=reset name=gateway_reset value=取消></form>";
       ?>
  　    <br><br>
        </center>
    </td>
    </tr>

</table>


</td>
  </tr>
</table>


</body>
<script language="javaScript">
function reboot()
{
	if(window.confirm("请务必记住此地址，系统将重新启动，启动后请用新地址连接配置界面!!!\n\n\n 您确定要修改内网卡地址吗?"))
        {
                document.seteth1.action="setproxynet.php?opcode=1";
                document.seteth1.submit();
       }
}
</script>

</html>
