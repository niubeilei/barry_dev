<!--
**************************************************************************
                  《 PLW517服务器密码机管理系统Web站点》
                        版权所有 (C) 2005,2006
**************************************************************************
******************************************************
项目:《 PLW517服务器密码机管理系统Web站点》
模块:AppProxy反向代理服务列表管理
描述:
版本:1.0.1.0
日期:2006-01-08
作者:Stephen
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
<title>反向代理服务设置</title>
</head>

<body bgcolor="#eef4f4">
<form name="server" method="post">
<table border="0" cellspacing="1" width="100%" id="AutoNumber2" height="400">
  <tr>

    <td width="100%" height="400">
    <img border="0" src="image/revser.gif" width="283" height="57">
    <b><font face="楷体_GB2312"  color="#006699">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;页次:
<?php
	$apptype = "backward";
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
			"<th width=\"2%\"></th>\n".
			"<th width=\"28%\"><center>反向代理<br>服务名称</center></th>\n".
  			"<th width=\"6%\"><center>协议<br>类型</center></th>\n".
  			"<th width=\"7%\"><center>服务<br>端口</center></th>\n".
   			"<th width=\"17%\"><center>对方PWL517<br>外口IP</center></th>\n".
  			"<th width=\"9%\"><center>后台服务<br>端口号</center></th>\n".
      		"<th width=\"6%\"><center>开启<br>状态</center></th>\n".
  			"<th width=\"24%\"><center>加入时间</center></th>\n".
			"</tr>\n<font size=\"1\">");

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
	 
	        if($prtype != $apptype)	continue;
	 
	        if ($modienable==1)
	          echo "<tr><th><input type=\"radio\" name=\"RadioGroupID\" value=$rid></th>\n";
	        else
	         	echo "<tr><th> </th>\n";
	        echo "<th>$prname</th>";
              
	        if (($svrtype=="tcp") or ($svrtype=="TCP"))
	          echo "<th>其它</th><th>$svrport</th>\n";
	        else
	          echo "<th>$svrtype</th><th>$svrport</th>\n";
	      
	       	if ($seraddr=="-")
	          echo "<th></th>";
	        else
	          echo "<th>$seraddr</th>";
 
	        if ($serport=="-")
	          echo "<th></th>";
					else
	          echo "<th>$serport</th>\n";
	 
	        if (($opensw=="on") or ($opensw=="ON"))
	          echo "<th>开</th>";
	        else
	          echo "<th>关</th>";
	        echo "<th>$addtime</th>";
	       	echo "\n</tr>";             
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
	 	echo "<a href=\"revserver.php?numb=$temp&apptype=$apptype\">上一页</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	}
	else
	{
		echo "上一页&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	}
	
	if($nr>$numb*10)
	{
  		$temp=$numb+1;
	 	echo "<a href=\"revserver.php?numb=$temp&apptype=$apptype\">下一页</a>";
	}
	else
	{
		echo "下一页";
	}

	print("</font><br><br><center>");
  	print("<table border=\"0\" cellspacing=\"1\" width=\"50%\" style=\"border-collapse: collapse\">
        	<tr><td><font face=\"楷体_GB2312\">反向代理服务名称</font></td>
	      		<td width=\"54%\" valign=\"top\">
        			<input type=\"TEXT\" name=\"proxyname\" size=%20 maxlength=32>
  						<input name=\"proxytype\" style=\"Display:none\" value = $apptype>
  				</td>
  			</tr>");

	print("  <tr id=\"SvrTxtSel\" style=\"DISPLAY:\"><td><font face=\"楷体_GB2312\">协议类型</font></td>
             <td>
        	   	<select name=\"svrstypef\" style=\"position: relative; width: 152; height: 20\" onchange=\"changedenyp()\" onclick=\"changedenyp()\">
                 <option value=\"http\" selected>http</option>
                 <option value=\"ftp\">ftp</option>
                 <option value=\"smtp\">smtp</option>
                 <option value=\"pop3\">pop3</option>
                 <option value=\"yzw\">yzw</option>
                 <option value=\"dhttp\">dhttp</option>
                 <option value=\"tcp\">其它</option>
              </select>
             </td>
            </tr>");
    
    print("  <tr><td><font face=\"楷体_GB2312\">服务端口</font></td>
                 <td><input type=\"TEXT\" name=\"svrsport\" size=%20 maxlength=5>
                 			<input type=\"hidden\" name=\"secstag\" value=\" \">
                 </td>
             </tr>");
         
    print("<tr><td><font face=\"楷体_GB2312\">对方PWL517外口IP</font></td>
               <td><input type=\"TEXT\" name=\"asaddr\" size=%20 maxlength=20></td></tr>
           <tr><td><font face=\"楷体_GB2312\">后台服务端口号</font></td>
               <td><input type=\"TEXT\" name=\"asport\" size=%20 maxlength=5></td></tr>
           <tr><td><font face=\"楷体_GB2312\">开启状态</font></td>
               <td>
          	   <select name=\"openstat\"  style=\"position: relative; width: 152; height: 20\">
                   <option value=\"on\">开</option>
                   <option value=\"off\">关</option>
                 </select></td></tr>");
    
    print("<tr><td colspan=\"2\" align=\"center\"><br>
           			<input type=\"button\" value=\"添加服务\" name=\"addSvrs\" onClick=\"check()\">
               </td>
           </tr>
          </table>");
    echo "<input type=\"hidden\" name=\"svrstype\" value=\"http\">";
?>
</td>
  </tr>
</table>
</form>
</body>
<script language="javaScript">
function set()
{
	if(window.confirm("您确定删除此反向应用服务吗？"))
	{
		document.server.action="revserver_del.php?delall=0";
		document.server.submit();
	}
}

function delall()
{
	if(window.confirm("您确定删除全部反向应用服务吗？"))
	{
		document.server.action="revserver_del.php?delall=1";
		document.server.submit();
	}
}

function modi()
{
	document.server.action="revserver_modi.php?";
	document.server.submit();
}

function check()
{
	var appnamelen=document.server.proxyname.value;
	if(appnamelen.length>32)
		alert("代理名称过长！");
	else if(appnamelen=="")
		alert("代理名称不能为空！");
  /*
    else if(pronamelen.length>20)
		alert("代理协议名称过长！");
		else if(pronamelen=="")
		alert("代理协议名称不能为空！");
  */
  else if(document.server.svrsport.value>65535)
		alert("服务端口过大！");
	else if(document.server.svrsport.value==0)
		alert("服务端口不能为零或空");
	else if(document.server.asport.value>65535)
		alert("实际服务端口号过大！");
	else if((document.server.asport.value<=0) && (document.server.svrstype.value!="dhttp") && (document.server.svrstype.value!="yzw"))
		alert("实际服务端口号不能为零或空！");
	else if((document.server.asaddr.value=="") && (document.server.svrstype.value!="dhttp") && (document.server.svrstype.value!="yzw"))
		alert("实际服务器IP地址不能为空！");
	else{
		document.server.action="server_modiok.php?opcode=1";
		document.server.submit();
	}
}

function changedenyp() 
{
	document.server.svrstype.value=document.server.svrstypef.value;
}

</script>
</html>