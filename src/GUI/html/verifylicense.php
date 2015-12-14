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





<title>License注册结果</title>


</head>





<body bgcolor="#eef4f4">





<form name="mserver_resource" METHOD="POST" ACTION="">




<?php


include ("constant.php");

	echo '<br>';


	echo '<font face="楷体_GB2312"><b>您注册的结果为：</font></b>';

	echo '<hr>';
	$a="";   
        //add write option to the file   --by ytao
        $filename=LICENSETXT;        
	AosAddWriteOption($filename);
	  
	$cmd=MYSUDO."echo ".$verifystring." \>".LICENSETXT;
        //del write option to the file   --by ytao
    	AosDelWriteOption($filename);
        $buffer=exec($cmd,$a);
        
	write_ed();
        
        $cmd=MYSUDO.LICENSE_VERIFY;
        //echo $cmd;
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

?>
<p>
<center><a href='license.php'><img border="0" src="image/goback.gif" width="40" height="40"></a>
</center>


</form>


</body>


</html>


