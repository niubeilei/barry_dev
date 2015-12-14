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
    include "constant.php";
?>
<html>

<head>
<meta name="GENERATOR" content="Microsoft FrontPage 5.0">
<meta name="ProgId" content="FrontPage.Editor.Document">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<title>PMI访问控制配置</title>
</head>

<body bgcolor="#eef4f4">

<FORM name="PMIcontrol" METHOD="POST" ACTION="pmi_condel.php">
<table border="0" cellspacing="1" width="100%" id="AutoNumber2" height="480">
  <tr>

    <td width="100%" height="480">
     <img border="0" src="image/pmihead.gif" width="283" height="57">
     <b><font face="楷体_GB2312" color="#006699">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;页次
    <?php
    //临时使用
    //$nr=99;
    //$aaa=floor($nr/10);
    //echo $aaa;

    $pmiservs=GetPMIRules();
    $nr=count($pmiservs);
    //print_r( $pmiservs);
	if($nr%10==0)
		$tmp=floor($nr/10);
	else
		$tmp=floor($nr/10)+1;
    if ($numb<=0)
        $numb=1;
 	print(":$numb/$tmp&nbsp;&nbsp;&nbsp;每页最多显示10条&nbsp;&nbsp;&nbsp;记录数:$nr</font></b>");

	print("<table border=\"2\" cellspacing=\"1\" bordercolor=\"#55847e\" width=\"100%\" style=\"border-collapse: collapse\">".
			"<tr bgcolor=\"#95bdbf\">\n".
			"<th width=4%></th>\n".
			"<th width=%10><center>应用服务名称</center></th>\n".
            //*********20051221;by cw;修改显示顺序
  			"<th width=%8><center>组织名称</center></th>\n".
            "<th width=%10><center>系统名称</center></th>\n".
  	//		"<th width=%15>证书验证设置</th>\n".
  			"<th width=%35><center>开关</center></th>\n".
 			"</tr>\n");
		$aaa=($numb-1)*10+1;             //该页的第一个记录的rid
        if (($numb*10)<=$nr)
        {
            $bbb=$numb*10;                   //该页的最后一个记录的rid
        } else
        {
            $bbb=$nr;
        }
        $rs=$bbb-$aaa;                       //该页显示的记录数-1；
        //$count=0;
        $ri=0;

		if ($nr != 0)
		{
            while ($ri<=$rs)
			{
                $rid=$pmiservs[$aaa-1][0];
                $appname=$pmiservs[$aaa-1][1];
                $sysname=$pmiservs[$aaa-1][3];
                $orgname=$pmiservs[$aaa-1][2];
                $onoff=$pmiservs[$aaa-1][4];
                echo "<tr><th><input type=\"radio\" name=\"RadioGroupID\" value=$rid></th>\n";
                //echo "<input type=\"hidden\" name=\"RuleID\" value=$rid>";
                echo "<th>$appname</th><th>$orgname</th><th>$sysname</th>\n";
                /*
                if ($onoff=="on")
                {
                    echo "<th><label>开启<input type=\"radio\" name=\"RGSW$rid\" value=\"on\" checked></label>\n";
                    echo "&nbsp;&nbsp;&nbsp;<label>关闭<input type=\"radio\" name=\"RGSW$rid\" value=\"off\"></label></th></tr>\n";
                } else
                {
                    echo "<th><label>开启<input type=\"radio\" name=\"RGSW$rid\" value=\"on\"></label>\n";
                    echo "&nbsp;&nbsp;&nbsp;<label>关闭<input type=\"radio\" name=\"RGSW$rid\" value=\"off\" checked></label></th></tr>\n";
                }
                */
                if ($onoff=="on")
                {
                    echo "<th>开启</th></tr>\n";
                } else
                {
                    echo "<th>关闭</th></tr>\n";
                }
                $aaa=$aaa+1;
			    $ri+=1;
			}

		}
		//$dblink->close();
        $temp=$numb;

		print("</table><br>");
        //echo "radioid=".$RadioGroupID;
        echo "<input type=\"hidden\" name=\"numb\" value=\"$numb\">";
		 // action=\"setprot.php?opcode=$opcode\">;

        echo "<input type=\"button\" value=\"删除规则\" name=\"delrule\" onclick=\"del()\">";
        print("&nbsp;&nbsp;&nbsp;");
        echo "<input type=\"submit\" value=\"修改规则\" name=\"editrule\" onclick=\"modify()\"\n>";

	print("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");
	print("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");
	print("<font face=\"楷体_GB2312\" color=\"#C0C0C0\"><b>");

    if($numb>1)
   	{
  		$temp=$numb-1;
	 	echo "<a href=\"pmi_control.php?numb=$temp\">上一页</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	}else{
		echo "上一页&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	}
	if($nr>$numb*10)
	{
  		$temp=$numb+1;
	 	echo "<a href=\"pmi_control.php?numb=$temp\">下一页</a>";
	}else{
		echo "下一页";
	}

	print("</font><br><br><center>");

    print("<table border=\"0\" cellspacing=\"1\" width=\"50%\" style=\"border-collapse: collapse\">
             <tr><td width=\"35%\"><font face=\"楷体_GB2312\">应用服务名称</font></td>
                 <td width=\"65%\" valign=\"top\">
                   <select name=\"appselect\" style=\"position: relative; width: 152; height: 20\">");

        //----------------20051216;by yy;读取/usr/SSLMPS/config/app_proxy.conf中的应用服务列表；
        $apps=GetAppSvrNameList(2);
        /*
        $n0=$apps[0];
        if (strlen($n0)<18)
        {
            $nl=18-strlen($n0);
            for ($i=0;$i<$nl;$i++)
            {
                $n0.="&nbsp;";
            }
        }
        $apps[0]=$n0;
        */
        foreach ($apps as $name) {

            echo "<option value=\"$name\">$name</option>";
        }
        AosGetPMI($pmionoff,$pmiaddr,$pmiport,$pmiorg);
        //----------------20051216;by yy;

    print("               </select></td></tr>
             <tr><td><font face=\"楷体_GB2312\">组织名称</font></td>
                 <td><input type=\"TEXT\" name=\"orgname\" size=%20 maxlength=20 value=\"$pmiorg\" readonly=\"true\"></td></tr>
             <tr><td><font face=\"楷体_GB2312\">系统名称</font></td>
	             <td width=\"54%\" valign=\"top\"><input type=\"TEXT\" name=\"sysname\" size=%20 maxlength=255></td></tr>
             <tr><td><font face=\"楷体_GB2312\">开关</font></td>
                 <td>
            	   <select name=\"swselect\" size=%20>
                     <option value=\"on\">开&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</option>
                     <option value=\"off\">关</option>
                   </select></td></tr>
             <tr><td colspan=\"2\" align=\"center\"><br>
                   <input type=\"button\" value=\"添加规则\" name=\"addRule\" onClick=\"check()\">
                 </td></tr>
           </table>");
    ?>
     </center>
     </td>
  </tr>

</table>
</form>
</body>

<script language="javaScript">
/*
function set(){
	if(document.control.R1[0].checked)
	{
		document.control.action="pmirole.php?numb=1&rolenumb=1";
		document.control.submit();
	}else if(document.control.R1[1].checked)
	{
		document.control.action="pmi1.php?numb=1&rolenumb=1";
		document.control.submit();
	}else if(document.control.R1[2].checked)
	{
		document.control.action="pmi_all.php?numb=1&rolenumb=1";
		document.control.submit();
	}
}
*/

function del()
{   //var arra=document.PMIcontrol.RadioGroupID.value;
    //arra=2;
    {   //alert(arra);
        if(window.confirm("确定删除PMI访问控制规则吗？"))
	    {
            document.PMIcontrol.action="pmi_condel.php";
     	    document.PMIcontrol.submit();
        }
     }
}

function modify()
{
    document.PMIcontrol.action="pmi_conmodi.php";
    document.PMIcontrol.submit();

}

function check(){
    /*
    var appnamelen=document.server.appname.value;
	var pronamelen=document.server.svname.value;


	if(appnamelen.length>20)
		alert("应用服务名称过长！");
	else if(appnamelen=="")
		alert("应用服务名称不能为空！");
	else if(pronamelen.length>20)
		alert("代理协议名称过长！");
	else if(pronamelen=="")
		alert("代理协议名称不能为空！");
	else if(document.server.svport.value>65535)
		alert("服务端口过大！");
	else if(document.server.svport.value==0)
		alert("服务端口不能为空");
	else if(document.server.svrport.value>65535)
		alert("后台服务端口号过大！");
	else if(document.server.svrport.value==0)
		alert("后台服务端口号不能为空！");
	else if(document.server.svrip.value=="")
		alert("后台服务器IP地址不能为空！");
	else{  }
    */
	document.PMIcontrol.action="pmi_conmodiok.php?opcode=1";
	document.PMIcontrol.submit();
}

</script>
</html>
