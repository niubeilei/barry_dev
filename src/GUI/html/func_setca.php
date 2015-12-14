<?php

function GetCaTxt(&$ca_pol,
	&$ca_ldapgap,
	&$ca_ldapport,
	&$ca_ocspport,
	&$ca_ocsponldapsuc,
	&$ca_ldapipstr,
	&$ca_ocspipstr,
	&$ca_ldapport2,
	&$ca_ldapipstr2)
{
include("constant.php");
	$fcont=file($CATXTFILE);
	list($key_pro,$catxtstr) = each($fcont);
	
	$ca_pol=strtok($catxtstr," ");
//	print($ca_pol.$ln);
	$ca_ldapgap=strtok(" ");
//	print($ca_ldapgap.$ln);
	$ca_ldapport=strtok(" ");
	$ca_ocspport=strtok(" ");
//	print($ca_ocspport.$ln);
	$ca_ocsponldapsuc=strtok(" ");
	$ca_ldapipstr=strtok(" ");
	$ca_ocspipstr=strtok(" ");
//	print($ca_ocspipstr.$ln);
	$ca_ldapport2=strtok(" ");
	$ca_ldapipstr2=strtok(" ");
}

function SetCaTxt($ca_pol,
	$ca_ldapgap,
	$ca_ldapport,
	$ca_ocspport,
	$ca_ocsponldapsuc,
	$ca_ldapipstr,
	$ca_ocspipstr,
	$ca_ldapport2,
	$ca_ldapipstr2)
{
include("constant.php");
	$fp=fopen($CATXTFILE,"wb");
	if ($fp == FALSE)
		return FALSE;
	$catxtstr=$ca_pol." ".$ca_ldapgap." ".$ca_ldapport." ".$ca_ocspport." ".$ca_ocsponldapsuc." ".$ca_ldapipstr." ".$ca_ocspipstr." ".$ca_ldapport2." ".$ca_ldapipstr2;
	print($catxtstr.$ln);
	fwrite($fp,$catxtstr,strlen($catxtstr));
	fclose($fp);
	return TRUE;
}

?>