#!/usr/bin/perl -wW  
use strict;  

my $query;
my $rslt;
my $time;
my $fd;

$fd=shift;

############################
# stat_test_qry_1
############################
$rslt=`grep -ir stat_test_qry_1 $fd`;
if ($rslt =~ /time="(.*)ms"/)
{
	$time = $1;
	print "Query time is ${time}ms.\n";
}

if ($rslt =~ /f21="key_field21_1" key_field8="14621" sum0x28key_field70x29="516"/)
{
	print "stat_test_qry_1 query OK.\n";
}
else
{
	print "stat_test_qry_1 query failed!\n";
}


1;
