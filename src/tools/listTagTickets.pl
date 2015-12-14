#!/usr/bin/perl -w
##########################################i####
# find tickets between two tags
# E.g. 
#   $perl listTickets.pl rel_3_1 03e398 366797
################################################
use strict;

my $rel = $ARGV[0];
my $tag1 = $ARGV[1];
my $tag2 = $ARGV[2];

my $logMsg = `git log $rel $tag1...$tag2 --oneline | grep -ir jimodb-`;
print $logMsg;

1;
