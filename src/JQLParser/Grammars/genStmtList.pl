#!/usr/bin/perl -wW  
use strict;  

sub printStmt {
	my ($file, %hash) = @_;
	my $hkey;

	open (OUTFILE, ">>" . $file);
	foreach $hkey (keys %hash)
	{
		print OUTFILE $hkey. ":";
		print OUTFILE "\n";
		print OUTFILE $hash{$hkey};
		print OUTFILE "\n";
	}

	print "\n";
}

sub findParamStmts {
	my ($paramHash, $expHash, $paramList, $stmtStr) = @_;
	my @tokens = split(/\W/, $stmtStr);
	my $token;

	foreach $token (@tokens)
	{
		if ( (lc $token) eq "expr" or
			 (lc $token) eq "expr_list" )
		 {
			 next;
		 }

		if (exists($expHash->{$token}) and 
			!exists($paramHash->{$token}) )
		{
			$paramHash->{$token} = $expHash->{$token};
			push(@$paramList, $token);
			findParamStmts($paramHash, $expHash, $paramList, $expHash->{$token});
		}
	}
}

#remove generated files
`rm -rf stmtSimple.txt`;
`rm -rf stmtVerbose.txt`;

#read first level commands from Mid.yy
unless (open (STMTLIST, "<Mid.yy")) {  
    die ("cannot open statement list file Mid.yy \n");  
}

#generate a big file for stmt list
`cat Stmt*.yy > all.yy`;

unless (open (EXPLIST, "<all.yy")) {  
    die ("cannot open statement list file all.yy \n");  
}


my %stmtMap;
my @stmtList;
my $numStmt = 0;
#save stmt list firstly
while(<STMTLIST>)  
{
	if ($_=~/(\w*stmt\w*)/)
	{
		$stmtList[$numStmt] = $1;
		$numStmt++;
	}
}  

my %expMap;
my $key = "";
my $value = "";
my $valueStarted = 0;
my $blockStarted = 0;
#save all the expressions in a hash
while(<EXPLIST>)  
{
	if ($valueStarted == 0)
	{
		if ($_=~/^(\w+):/)
		{
			$key = $1;
			$value = "";
			$valueStarted = 1;
		}
	}
	else
	{
		#inside valule data
		if ($blockStarted == 0)
		{
			if ($_ =~ /\{/)
			{
				$blockStarted = 1;
			}
			else
			{
				$value = $value . $_;
			}
		}
		else
		{
			#inside the block, the data is code, not needed
			if ($_ =~ /^\s*\|\s*$/)
			{
				$blockStarted = 0;
				$value = $value . $_;
			}
		}

		#sometimes, the line is "};" which is end of a key-value pair
		if ($_ =~ /\}\s*\;/ or $_ =~ /^\s*;\s*$/)
		{
			#this is an end of a key-value pair
			$expMap{$key} = $value;
			$valueStarted = 0;
			$blockStarted = 0;
		}
	}
}  

#expand the statement list based on the exp map
#set the statement to be the value of expMap firstly
my $stmt;
open (SIMPLEFILE, ">>stmtSimple.txt");
foreach $stmt (@stmtList)
{
	$stmtMap{$stmt} = $expMap{$stmt};
	print SIMPLEFILE $stmt . ":";
	print SIMPLEFILE "\n";
	print SIMPLEFILE $stmtMap{$stmt};
	print SIMPLEFILE "\n";
}

#collect parameter expr
my %paramMap;
my @paramList;
open (VERBFILE, ">>stmtVerbose.txt");
$numStmt = 0;
foreach $stmt (@stmtList)
{
	%paramMap = ();
	@paramList = ();
	$value = $stmtMap{$stmt};
	findParamStmts(\%paramMap, \%expMap, \@paramList, $value);

	$numStmt++;
	print VERBFILE "****************************************************\n";
	print VERBFILE "**** STATEMENT " . $numStmt . " **********************************\n";
	print VERBFILE "****************************************************\n";

	print VERBFILE $stmt . ":";
	print VERBFILE "\n";
	print VERBFILE $value;
	print VERBFILE "\n";
	
	#print VERBFILE "---- PARAM INFO ---------------------------------------\n";
	foreach $key (@paramList)
	{
		print VERBFILE $key. ":";
		print VERBFILE "\n";
		print VERBFILE $paramMap{$key};
		print VERBFILE "\n";
	}
}

close VERBFILE;
close SIMPLEFILE;

1;
