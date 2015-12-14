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
<!--
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
-->
<title>管理员管理</title>
</head>

<body bgcolor="#eef4f4">

<FORM name="manamanage" METHOD="POST" ACTION="check_manager.php">

<table border="0" cellspacing="1" width="100%" id="AutoNumber1" height="480">
  <tr>

    <td width="100%" height="480">　

 <table border="0" cellspacing="0" width="100%" id="AutoNumber2" height="44" bgcolor="#eef4f4">
  <tr>
    <td width="50%" height="39">
    <img border="0" src="image/mareg1.gif" width="283" height="57"><br>&nbsp;
    <font face="楷体_GB2312">管理员姓名：</font><input type="text" name="mananame1" style="position: relative; width: 120; height: 20">&nbsp;
    <br>&nbsp;<font face="楷体_GB2312"> 密&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
    码：</font><input type="password" name="password1" style="position: relative; width: 120; height: 20"><br>&nbsp;
    <font face="楷体_GB2312">确 认 密码：</font><input type="password" name="password2" style="position: relative; width: 120; height: 20">
    <p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
    <input type="button" value="添 加" name="B1"  onclick="set()" ></td>
    <td width="50%" height="39">
    <img border="0" src="image/mareg2.gif" width="283" height="57"><br>
    <font face="楷体_GB2312">请选择注销管理员名：</font>
    <select size="1" name="mananame2"  style="position: relative; width: 120; height: 20">
      <?php
        include "pmi_db.php";
        //----------------20051210;by yy;用户注册方式由数据库改为文件读取
        /*
        $db = mysql_connect(C_DB_HOST, "root");
      	mysql_select_db(C_DB_NAME,$db);
     	$result = mysql_query("SELECT * FROM ManagerTable",$db);
     	if(!$result)
     	{
     		echo '没有满足条件的纪录';
    		mysql_close($db);
 	    	return 0;
 	    }
        while($row =mysql_fetch_row($result))
        {
          	echo "<option value=$row[0]>$row[0]</option>";
        }
        mysql_close($db);
        */
        //----------------20051210;by yy;用户注册方式由数据库改为文件读取

        //----------------20051213;by yy;新增用户文件注册方式；
        $users=GetManagerList();
        foreach ($users as $name) {

            echo "<option value=$name>$name</option>";
        }
        //----------------20051213;by yy;新增用户文件注册方式；
     ?>
        </select><p>
        <font face="楷体_GB2312">输 &nbsp;&nbsp; 入&nbsp;&nbsp;  密&nbsp;&nbsp;  码： </font>
        <input type="password" name="password3"  style="position: relative; width: 120; height: 20">
        <p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
        <input type="button" value="删 除" name="logout"  onclick="set1()" >
        </td>
  </tr>
</table>
<br><br>
<table border="0" cellspacing="0" width="100%" id="AutoNumber3">
  <tr>
    <td width="100%">
    <p align="center">
    <img border="0" src="image/mareg3.gif" width="283" height="57">
    <br><font face="楷体_GB2312">请选择修改的管理员名：
	</font>

        <select size="1" name="mananame3"  style="position: relative; width: 120; height: 20">
          <?php
            //----------------20051210;by yy;用户注册方式由数据库改为文件读取
            /*
            $db = mysql_connect(C_DB_HOST, "root");
         	mysql_select_db(C_DB_NAME,$db);
        	$result = mysql_query("SELECT * FROM ManagerTable",$db);
        	if(!$result)
 	        {
 	        	echo '没有满足条件的纪录';
	        	mysql_close($db);
 	        	return 0;
 	        }
            while($row =mysql_fetch_row($result))
            {
              	echo "<option value=$row[0]>$row[0]</option>";
            }
            mysql_close($db);
            */
            //----------------20051210;by yy;用户注册方式由数据库改为文件读取

            //----------------20051213;by yy;新增用户文件注册方式；
            $users=GetManagerList();
            foreach ($users as $name)
            {
                echo "<option value=$name>$name</option>";
            }
            //----------------20051213;by yy;新增用户文件注册方式；
          ?>
        </select>
        <br><font face="楷体_GB2312">输 入 旧 密 码：&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font><input type="password" name="password4"  style="position: relative; width: 120; height: 20">
        <br><font face="楷体_GB2312">输 入 新 密 码：&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font><input type="password" name="password5"  style="position: relative; width: 120; height: 20">
        <br><font face="楷体_GB2312">确 认 新 密 码：&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</font><input type="password" name="password6"  style="position: relative; width: 120; height: 20">
        <br><br>
        <input type="button" value="修 改" name="change"  onclick="set2()" >
        <INPUT type=hidden value="0" name=islogin><br></td>
  </tr>
</table>

    </td>
  </tr>
</table>
</form>
</body>

<script language="javaScript">

function set(){

    var tlen;
	tlen=document.manamanage.mananame1.value;
	p1len=document.manamanage.password1.value;
	p2len=document.manamanage.password2.value;

	if (tlen.length<1 || p1len.length<1)
        alert("管理员姓名和密码不能为空！");
    else if (tlen.indexOf(" ")!= -1)
        alert("管理员姓名中不能有空格！");
    else if(tlen.length>20)
		alert("管理员姓名过长！");
	else if(p1len.length>15)
		alert("密码过长！");
	else if(p2len.length>15)
		alert("密码过长！");
	else if(p1len!=p2len)
		alert("两密码不符！");
    else if (p1len.indexOf(" ")!= -1)
        alert("密码中不能有空格！");
	else
	{
		document.manamanage.islogin.value=0;
		document.manamanage.action="check_manager.php";
		document.manamanage.submit();
	}
}

function set1(){
	var tlen;
	p1len=document.manamanage.password3.value;
	if(p1len.length>15)
		alert("密码过长！");
    else if(p1len.length<1)
		alert("密码不能为空！");
    else if (p1len.indexOf(" ")!= -1)
        alert("密码中不能有空格！");
	else
	{
		document.manamanage.islogin.value=1;
		document.manamanage.action="check_manager.php";
		document.manamanage.submit();
	}
}

function set2(){
	var tlen;
	p3len=document.manamanage.password6.value;
	p1len=document.manamanage.password4.value;
	p2len=document.manamanage.password5.value;
	if(p1len.length>15)
		alert("旧密码过长！");
    else if(p1len.length<1)
		alert("密码不能为空！");
	else if(p2len.length>15)
		alert("新密码过长！");
	else if(p2len.length<1)
		alert("新密码不能为空！");
	else if(p2len!=p3len)
		alert("两个新密码不符！");
    else if (p2len.indexOf(" ")!= -1)
        alert("密码中不能有空格！");
    else
	{
		document.manamanage.islogin.value=2;
		document.manamanage.action="check_manager.php";
		document.manamanage.submit();
	}
	}

</script>
</html>