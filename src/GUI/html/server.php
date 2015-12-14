<!--
**************************************************************************
                  《 PLW517服务器密码机管理系统Web站点》
                        版权所有 (C) 2005,2006
**************************************************************************
******************************************************
项目:《 PLW517服务器密码机管理系统Web站点》
模块:AppProxy代理服务列表管理
描述:
版本:1.0.1.0
日期:2005-12-22
作者:Rechard
更新:
	1.2005-12-24
    	代理类型增加“JNS”类型；
        增加全部清空功能；
  2.2006-1-8
TODO:
*******************************************************
-->
<?php
	include("constant.php");
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
<!--
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
-->
<title>代理服务设置</title>
</head>

<body bgcolor="#eef4f4">
<form name="server" method="post">
<table border="0" cellspacing="1" width="100%" id="AutoNumber2" height="400">
  <tr>

    <td width="100%" height="400">
    <img border="0" src="image/apphead.gif" width="283" height="57">
    <b><font face="楷体_GB2312"  color="#006699">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;页次:
<?php

	$apptype = "forward";
    $proxyservs=GetAppSvrList($apptype);
    $nr=count($proxyservs);
	if($nr%10==0)
		$tmp=floor($nr/10);
	else
		$tmp=floor($nr/10)+1;
    if ($numb<=0)
        $numb=1;
	print("$numb/$tmp&nbsp;&nbsp;&nbsp;每页最多显示10条&nbsp;&nbsp;&nbsp;记录数:$nr</font></b>");
	print("<table border=\"2\" cellspacing=\"1\" bordercolor=\"#55847e\" width=\"100%\" style=\"border-collapse: collapse\">".
					"<tr bgcolor=\"#95bdbf\">\n".
						"<th width=\"3%\"></th>\n".
						"<th width=\"22%\"><center>代理服务名称</center></th>\n".
					"<th width=\"8%\"><center>协议<br>类型</center></th>\n".
					"<th width=\"8%\"><center>服务<br>端口</center></th>\n".
					"<th width=\"18%\"><center>后台服务器<br>地址</center></th>\n".
					"<th width=\"10%\"><center>后台服务<br>端口号</center></th>\n".
        	"<th width=\"5%\"><center>开启<br>状态</center></th>\n".
					"<th width=\"15%\"><center>加入时间</center></th>\n".
					"<th width=\"10%\"><center>DenyPage</center></th>\n");
//		"<th width=\"11%\"><center>安全用户<br>帐号关联</center></th>\n".
	print("</tr>\n<font size=\"1\">");

    $aaa=($numb-1)*10+1;             //该页的第一个记录的rid
    if (($numb*10)<=$nr)
    	$bbb=$numb*10;                   //该页的最后一个记录的rid
    else
      $bbb=$nr;

    $rs=$bbb-$aaa;                       //该页显示的记录数-1；
    $ri=0;

	if ($nr != 0)
	{
		while ($ri<=$rs)
		{

	        $rid=$proxyservs[$aaa-1][0];
	        $modienable=$proxyservs[$aaa-1][1];
	        $prname=$proxyservs[$aaa-1][2];
	        $prtype=$proxyservs[$aaa-1][3];
	        $svrtype=$proxyservs[$aaa-1][4];
	        $svrport=$proxyservs[$aaa-1][5];
	        $secutag=$proxyservs[$aaa-1][6];
	        $seraddr=$proxyservs[$aaa-1][7];
	        $serport=$proxyservs[$aaa-1][8];
	        $opensw=$proxyservs[$aaa-1][9];
	        $addtime=$proxyservs[$aaa-1][10];
	        $denypage=$proxyservs[$aaa-1][11];
	        if($prtype != $apptype)
	        	continue;
	        if ($modienable==1)
	        {
	        	echo "<tr><th><input type=\"radio\" name=\"RadioGroupID\" value=$rid></th>\n";
	        } else
	        {
	        	echo "<tr><th> </th>\n";
	        }
        	echo "<th>$prname</th>";
              
        	if (($svrtype=="tcp") or ($svrtype=="TCP"))
        	{
            	echo "<th>其它</th><th>$svrport</th>\n";
        	} else
        	{
           		echo "<th>$svrtype</th><th>$svrport</th>\n";
        	}
        
       		if ($seraddr=="-")
	        {
	            echo "<th></th>";
	        } else
	        {
	            echo "<th>$seraddr</th>";
	        }
	        if ($serport=="-")
	        {
	            echo "<th></th>";
	        } else
	        {
	            echo "<th>$serport</th>\n";
	        }
	
	        if (($opensw=="on") or ($opensw=="ON"))
	        {
	            echo "<th>开</th>";
	        } else
	        {
	            echo "<th>关</th>";
	        }
	        echo "<th>$addtime</th>";
	        
	        if (($denypage == "-") || ($svrtype != "http"))
	        	echo "<th>无</th>\n";
	        else
	            echo "<th>$denypage</th>\n";
	        echo "</tr>";
	        $aaa=$aaa+1;
		    $ri+=1;
		}
	}
    $temp=$numb;

	print("</font></table><br>");
	echo "<input type=\"hidden\" name=\"numb\" value=\"$numb\">";

	print("<input type=\"button\" value=\"删除服务\" name=\"del\" onclick=\"set()\">");
	print("&nbsp;&nbsp;&nbsp;");
	print("<input type=\"submit\" value=\"修改服务\" name=\"edit\" onclick=\"modi()\">");
	print("&nbsp;&nbsp;&nbsp;");
	print("<input type=\"button\" value=\"全部删除\" name=\"deleall\" onclick=\"delall()\">");

	print("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
	print("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
	print("<font face=\"楷体_GB2312\" color=\"#C0C0C0\"><b>");
	if($numb>1)
   	{
  		$temp=$numb-1;
		echo "<a href=\"server.php?numb=$temp&apptype=$apptype\">上一页</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	}else{
		echo "上一页&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	}
	if($nr>$numb*10)
	{
  		$temp=$numb+1;
	 	echo "<a href=\"server.php?numb=$temp&apptype=$apptype\">下一页</a>";
	}else{
		echo "下一页";
	}

	print("</font><br><br><center>");
	print("<table id=\"moditable\" border=\"0\" cellspacing=\"1\" width=\"50%\" style=\"border-collapse: collapse\">
	             <tr><td><font face=\"楷体_GB2312\">代理服务名称</font></td>
		             <td width=\"54%\" valign=\"top\">
	                     <input type=\"TEXT\" name=\"proxyname\" size=%20 maxlength=32>
	  						       <input name=\"proxytype\" style=\"Display:none\" value = $apptype></td></tr>");
	    //--------------------20060101;by yy;根据代理类型变化服务类型               
  	print("  <tr id=\"SvrTxtSel\" style=\"DISPLAY:\"><td><font face=\"楷体_GB2312\">协议类型</font></td>
               <td>
          	   <select name=\"svrstypef\" style=\"width: 152; height: 20\" onchange=\"changedenyp()\" onclick=\"changedenyp()\">
                   <option value=\"http\" selected>http</option>
                   <option value=\"ftp\" >ftp</option>
                   <option value=\"smtp\">smtp</option>
                   <option value=\"pop3\">pop3</option>
                   <option value=\"tcp\">其它</option>
                 </select></td></tr>");
    
    print("  <tr><td><font face=\"楷体_GB2312\">服务端口</font></td>
                 <td><input type=\"TEXT\" name=\"svrsport\" size=%20 maxlength=5>
                 <input type=\"hidden\" name=\"secstag\" value=\" \"></td></tr>");
         
    print("<tr><td><font face=\"楷体_GB2312\">后台服务器地址</font></td>
               <td><input type=\"TEXT\" name=\"asaddr\" size=%20 maxlength=20></td></tr>
           <tr><td><font face=\"楷体_GB2312\">后台服务端口号</font></td>
               <td><input type=\"TEXT\" name=\"asport\" size=%20 maxlength=5></td></tr>
           <tr><td><font face=\"楷体_GB2312\">开启状态</font></td>
               <td><select name=\"openstat\"  style=\"width: 152; height: 20\">
                   <option value=\"on\">开</option>
                   <option value=\"off\">关</option>
               </select></td></tr>");
    
  	print("<tr id=\"denyptxtbox\" style=\"DISPLAY:\">
    		<td><font face=\"楷体_GB2312\">DenyPage</font></td>
            <td><select name=\"denypage1\"  style=\"position: relative; width: 152; height: 20\">");
          	   
    //----------------20051216;by yy;读取/usr/SSLMPS/config/denypages.conf中的应用服务列表；
    $denypages=GetDenysList();
    print("<option value=\"-\">无</option>");
    foreach ($denypages as $denyname) {

        print("<option value=\"$denyname\">$denyname</option>");
    }
    print("</select></td></tr>
             <tr><td colspan=\"2\" align=\"center\"><br>
                   <input type=\"button\" value=\"添加服务\" name=\"addSvrs\" onClick=\"check()\">
                 </td>
             </tr>
           </table>");
           
    print("<input type=\"hidden\" name=\"svrstype\" value=\"http\">");
?>
</td>
  </tr>
</table>
</form>
</body>
<script language="javaScript">
function set()
{
	if(window.confirm("若删除此应用，则该应用所对应的PMI访问\n控制规则也将一同删除!\n\n\n     确定删除此应用吗？"))
	{
		document.server.action="server_del.php?delall=0&apptype=forward";
		document.server.submit();
	}
}

function delall()
{
	if(window.confirm("若删除全部应用服务，则所有PMI访问控制规则也将一同删除!!!\n\n\n           您确定删除全部应用服务吗？"))
	{
		document.server.action="server_del.php?delall=1&apptype=forward";
		document.server.submit();
	}
}

function modi()
{
	document.server.action="server_modi.php?apptype=forward";
	document.server.submit();
}

function check()
{
	var appnamelen=document.server.proxyname.value;
	if(appnamelen.length>32)
		alert("代理名称过长！");
	else if(appnamelen=="")
		alert("代理名称不能为空！");
  	else if(document.server.svrsport.value>65535)
		alert("服务端口过大！");
	else if(document.server.svrsport.value==0)
		alert("服务端口不能为零或空");
	else if(document.server.asport.value>65535)
		alert("后台服务端口号过大！");
	else if(document.server.asport.value<=0)
		alert("后台服务端口号不能为零或空！");
	else if(document.server.asaddr.value=="")
		alert("后台服务器IP地址不能为空！");
	else
	{
		document.server.action="server_modiok.php?opcode=1";
		document.server.submit();
	}
}

function changedenyp() 
{
	document.server.svrstype.value=document.server.svrstypef.value;
	if (document.server.svrstype.value == "http")
	{
		denyptxtbox.style.display=""
		denyptxtbox.style.position="relative";
	}
 	else
 	{
		denyptxtbox.style.display="none";
	}
}

</script>
</html>
