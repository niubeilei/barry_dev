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

<title>其他状态</title>
</head>

<body bgcolor="#eef4f4">

<form name="mother_stat" METHOD="POST" ACTION="">
<img border="0" src="image/motherhead.gif" width="283" height="57">
<?php
include ("constant.php");

	echo '<br>';
	$a="";
	$cmd=MYSUDO.MYCLI."\\\"ocsp show config\\\"";
	$buffer=exec($cmd,$a);
	//print_r($a);
	//echo $a;

	    	foreach($a as $buffer)
		    {
		        $lines=htmlspecialchars("$buffer",ENT_QUOTES);
		        $lines=str_replace(" ","&nbsp;",$lines);
		        echo "$lines".'<br>';
		    }

	    
	echo '<hr>';
	$a="";
	$cmd=MYSUDO.MYCLI."\\\"amm show config\\\"";
	$buffer=exec($cmd,$a);
	//print_r($a);
	//echo $a;
	
	    	foreach($a as $buffer)
		    {
		        $lines=htmlspecialchars("$buffer",ENT_QUOTES);
		        $lines=str_replace(" ","&nbsp;",$lines);
		        echo "$lines".'<br>';
		    }

	    
	echo '<hr>';
	$a="";
	$cmd=MYSUDO.MYCLI."\\\"crl show config\\\"";
	$buffer=exec($cmd,$a);
	//print_r($a);
	//echo $a;

	    	foreach($a as $buffer)
		    {
		        $lines=htmlspecialchars("$buffer",ENT_QUOTES);
		        $lines=str_replace(" ","&nbsp;",$lines);
		        echo "$lines".'<br>';
		    }

	    
	echo '<hr>';
	$a="";
	$cmd=MYSUDO.MYCLI."\\\"deny page show\\\"";
	$buffer=exec($cmd,$a);
	//print_r($a);
	//echo $a;

	    	foreach($a as $buffer)
		    {
		        $lines=htmlspecialchars("$buffer",ENT_QUOTES);
		        $lines=str_replace(" ","&nbsp;",$lines);
		        echo "$lines".'<br>';
		    }
	echo '<hr>';
	$a="";
	$cmd=MYSUDO.MYCLI."\\\"usbkey show\\\"";
	$buffer=exec($cmd,$a);
   	foreach($a as $buffer)
    {
		$lines=htmlspecialchars("$buffer",ENT_QUOTES);
		$lines=str_replace(" ","&nbsp;",$lines);
		echo "$lines".'<br>';
	}

?>
</form>
</body>
</html>
