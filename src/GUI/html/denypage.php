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

<title>DenyPage列表配置</title>
</head>

<body bgcolor="#eef4f4">

<FORM name="Denypages" METHOD="POST" action="--WEBBOT-SELF--" enctype="multipart/form-data">
<table border="0" cellspacing="1" width="100%" id="AutoNumber2" height="480">
  <tr>

    <td width="100%" height="480">
     <img border="0" src="image/denyhead.gif" width="283" height="57">
     <b><font face="楷体_GB2312" color="#006699">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;页次
    <?php
    //临时使用
    //$nr=99;
    //$aaa=floor($nr/10);
    //echo $aaa;

    $denyps=GetDenypagesList();
    $nr=count($denyps);
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
			"<th width=5%></th>\n".
			"<th width=%15><center>Denypage名称</center></th>\n".
  			"<th width=%80><center>Denypage文件名</center></th>\n".
  	//		"<th width=%8><center>单位名称</center></th>\n".
  	//		"<th width=%15>证书验证设置</th>\n".
  	//		"<th width=%35><center>开关</center></th>\n".
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
                $rid=$denyps[$aaa-1][0];
                $denyname=$denyps[$aaa-1][1];
                //---------------20060101;by cw;只显示文件名
                //$denypath=DENYPAGEFILEPATH.$denyps[$aaa-1][2];
                $denypath=$denyps[$aaa-1][2];
                //$orgname=$denyps[$aaa-1][3];
                //$onoff=$denyps[$aaa-1][4];
                echo "<tr><th><input type=\"radio\" name=\"RadioGroupID\" value=$rid></th>\n";
                //echo "<input type=\"hidden\" name=\"RuleID\" value=$rid>";
                echo "<th>$denyname</th><th>$denypath</th>\n";
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

                if ($onoff=="on")
                {
                    echo "<th>开启</th></tr>\n";
                } else
                {
                    echo "<th>关闭</th></tr>\n";
                }
                */
                echo "</tr>\n";
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

        echo "<input type=\"button\" value=\"删除Denypage\" name=\"delrule\" onclick=\"del()\">";
        print("&nbsp;&nbsp;&nbsp;");
        echo "<input type=\"button\" value=\"修改Denypage\" name=\"editrule\" onclick=\"modify()\">";

	print("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");
	print("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\n");
	print("<font face=\"楷体_GB2312\" color=\"#C0C0C0\"><b>");

    if($numb>1)
   	{
  		$temp=$numb-1;
	 	echo "<a href=\"denypage.php?numb=$temp\">上一页</a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	}else{
		echo "上一页&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	}
	if($nr>$numb*10)
	{
  		$temp=$numb+1;
	 	echo "<a href=\"denypage.php?numb=$temp\">下一页</a>";
	}else{
		echo "下一页";
	}

	print("</font><br><br><center>");

    print("<table border=\"0\" cellspacing=\"1\" width=\"50%\" style=\"border-collapse: collapse\">
             <tr><td width=\"35%\"><font face=\"楷体_GB2312\">Denypage名称</font></td>
                 <td width=\"65%\" valign=\"top\">
                   <input type=\"TEXT\" name=\"denypname\" size=%30 maxlength=15>
                 </td></tr>
             <tr><td><font face=\"楷体_GB2312\">Denypage文件名</font></td>
	             <td width=\"54%\" valign=\"top\"><input type=\"TEXT\" name=\"denyppath\" size=%30 maxlength=200></td></tr>
             <tr><td><font face=\"楷体_GB2312\">选择Denypage源文件</font></td>
                 <td><input name=\"denypfile\" type=\"file\" size=%32 maxlength=250></td></tr>
             <tr><td colspan=\"2\" align=\"center\"><br>
                   <input type=\"button\" value=\"添加Denypage\" name=\"addRule\" onClick=\"check()\">
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
        if(window.confirm("若删除此Denypage，则该Denypage对应所有\n代理服务的Denypage项将设置为无！\n\n\n确定删除吗？"))
	    {
            document.Denypages.action="denypage_del.php";
     	    document.Denypages.submit();
        }
     }
}

function modify()
{
    document.Denypages.action="denypage_modi.php";
    document.Denypages.submit();

}

function check(){

    var namelen=document.Denypages.denypname.value;
	var pathlen=document.Denypages.denyppath.value;
    var filelen=document.Denypages.denypfile.value;
    //var fso;
    //fso = new ActiveXObject("Scripting.FileSystemObject");
	
	if(namelen.length>20)
		alert("Denypage名称过长！");
	else if(namelen=="")
		alert("Denypage名称不能为空！");
	else if(pathlen.length>200)
		alert("Denypage文件名过长！");
	else if(pathlen=="")
		alert("Denypage文件名不能为空！");
    else if(filelen=="")
		alert("Denypage源文件不能为空！");
    else if(filelen.length>250)
		alert("Denypage源文件名过长！");
   //else if(!htmlcheck(pathlen))
   //		alert("Denypage文件名必须以.html/.htm为后缀！");
		else
    {
        //if (fso.FileExists(filelen)) {
	        document.Denypages.action="denypage_modiok.php?opcode=1";
	        document.Denypages.submit();
        //} else {
        //    alert("Denypage文件不存在！");
        //}
    }
}

function htmlcheck(strlen)
{
		var strpos=strlen.lastIndexOf(".");
		var extstr;
		if (strpos>0) {
		    extstr=strlen.substr(strpos+1);
	  } else {
		    extstr="";
	  }
	  if ((extstr=="htm") || (extstr=="html"))
	  {
	    return(true);
	  } else {
	  	return(false);
	  }
}

</script>
</html>
