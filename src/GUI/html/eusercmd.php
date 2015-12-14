<?php
	include("constant.php");
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>µÇÂ½Ê§°Ü</title></head><body  background="image/backgroud.gif"><b>' ;
	    echo 'ÇëÏÈµÇÂ½£¡<p>';
	    echo '<a href="login.html">·µ»Ø</a>';
            echo '</b></body></html>';
            exit();			
	}
//	echo $cmdline;
	ExecUserCmd($cmdline);
?>