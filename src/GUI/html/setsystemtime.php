<?php
include "constant.php";
@session_start();
	if($manager_ID==NULL)
	{
            echo '<html><head><title>µÇÂ½Ê§°Ü</title></head><body  background="image/backgroud.gif"><b>' ;
	    echo 'ÇëÏÈµÇÂ½£¡<p>';
	    echo '<a href="login.html">·µ»Ø</a>';
            echo '</b></body></html>';
            exit();			
	}

	echo '<html>';
	echo '<head>';
	echo '<meta http-equiv="refresh" content="1;url=settime.php">';
	echo '<head>';
	echo '<body>';

/*	$tok = strtok($timetemp," ");
	$tok=strtok(" ");
	switch ($tok)
	{
		case "Jan":
			$month="01";
			break;
		case "Feb":
			$month="02";
			break;
		case "Mar":
			$month="03";
			break;
		case "Apr":
			$month="04";
			break;
		case "May":
			$month="05";
			break;
		case "Jun":
			$month="06";
			break;
		case "Jul":
			$month="07";
			break;
		case "Aug":
			$month="08";
			break;
		case "Sep":
			$month="09";
			break;
		case "Oct":
			$month="10";
			break;
		case "Nov":
			$month="11";
			break;
		case "Dec":
			$month="12";
			break;
	}
	$tok=strtok(" ");
	if(strlen($tok)==1)
		$day="0".$tok;
	else
		$day=$tok;
	$tok=strtok(" ");
	$tok=strtok(" ");
	$tok=strtok(" ");
	$year=$tok;
	$hour=$hourlist;
	$minu=$minulist;
	$cmd=SUDO."date ".$month.$day.$hour.$minu.$year;
*/

	if (strlen($monthlist) == 1) $monthlist="0".$monthlist;
	if (strlen($daylist) == 1) $daylist="0".$daylist;
	if (strlen($hourlist) == 1) $hourlist="0".$hourlist;
	if (strlen($minulist) == 1) $minulist="0".$minulist;
	
	//-------------------------20060102;by yt; set sys time	
	$cmd=MYSUDO." date ".$monthlist.$daylist.$hourlist.$minulist.$yearlist;	
	$ret=exec($cmd,$a);	
	//-------------------------20060102;by yt; set sys time	
	
	echo '</body>';
	echo '</html>';

?>
