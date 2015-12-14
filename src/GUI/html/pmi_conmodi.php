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
	echo '<html><head><title>PMI访问控制配置规则修改</title></head>';
    echo '<body background="image/pmiback.gif"><form name="PMIconmodi" METHOD="POST" ACTION="pmi_conmodiok.php?opcode=2">';
    echo '<center><font face="楷体_GB2312"><b>';
    //echo "radioid=$RadioGroupID";
    if ($RadioGroupID<1)
    {
        echo "请选择一条规则后再修改！"."<br>";
        echo "<p></b>";
        echo "</font><a href=\"pmi_control.php?numb=$numb\">";
        echo "<img border=\"0\" src=\"image/goback.gif\" width=\"40\" height=\"40\"></a>";
        echo "<br></center></body></html>";
        exit();
    }
    print("<table border=\"0\" cellspacing=\"1\" width=\"50%\" style=\"border-collapse: collapse\">
             <tr><td width=\"46%\"><font face=\"楷体_GB2312\">应用服务名称</font></td>
                 <td width=\"54%\" valign=\"top\">");

        //----------------20051216;by yy;读取/usr/SSLMPS/config/app_proxy.conf中的应用服务列表；
        $apps=GetAppSvrNameList(2);
        //$rcc=count($apps);
        $nj=$RadioGroupID-1;

        $pmirules=GetPMIRules();
        //print_r($pmirules);
        $name=$pmirules[$nj][1];
        $asname=$pmirules[$nj][3];
        $pmiorg=$pmirules[$nj][2];
        $onoff=$pmirules[$nj][4];
        //echo "$name,$asname,$pmiorg,$onoff";
        echo "<input type=\"hidden\" name=\"RuleID\" value=$RadioGroupID>";
        echo "<select name=\"appselect\" style=\"position: relative; width: 152; height: 20\">";
        foreach($apps as $apname)
        {
            if (strcmp($name,$apname)==0)
            {
                echo "<option value=\"$apname\" selected>$apname</option>";
            } else
            {
                echo "<option value=\"$apname\">$apname</option>";
            }
        }
        //GetPMI($pmiaddr,$pmiport,$pmiorg);
        //----------------20051216;by yy;

    print("               </select></td></tr>
             <tr><td><font face=\"楷体_GB2312\">组织名称</font></td>
                 <td><input type=\"TEXT\" name=\"orgname\" size=%20 maxlength=20 value=\"$pmiorg\" readonly=\"true\" style=\"background-color: #c0c0c0\"></td></tr>
             <tr><td><font face=\"楷体_GB2312\">系统名称</font></td>
	             <td width=\"54%\" valign=\"top\"><input type=\"TEXT\" name=\"sysname\" size=%20 value=\"$asname\" maxlength=255></td></tr>
             <tr><td><font face=\"楷体_GB2312\">开关</font></td>
                 <td>
            	   <select name=\"swselect\" style=\"position: relative; width: 152; height: 20\">");
        if ($onoff=="on")
        {
            echo "<option value=\"on\" selected>开</option>";
            echo "<option value=\"off\">关</option>";
        } else
        {
            echo "<option value=\"on\">开</option>";
            echo "<option value=\"off\" selected>关</option>";
        }
    print("        </select></td></tr>
             <tr><td colspan=\"2\" align=\"center\"><br>
                   <input type=\"submit\" value=\"修改规则\" name=\"ModiRule\">
                 </td></tr>
           </table>");
    echo "<input type=\"hidden\" name=\"numb\" value=\"$numb\">";
    echo "<p></b></font><br></center></form></body></html>";
?>