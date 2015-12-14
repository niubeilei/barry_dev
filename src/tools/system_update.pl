#!/usr/bin/perl -w

use strict;
use Fcntl qw(:DEFAULT :flock); #File locking

my $echo="/bin/echo";
my $who="/usr/bin/who";
my $awk="/usr/bin/awk";
my $tee="/usr/bin/tee";
my $fetch="/usr/bin/wget -q";
my $mystro_run="/usr/local/AOS/Bin/mystrorun";

my $url_timeout=300; # timeout to give up on url (seconds)
############# Program Area ##############

#Solve the concurancy issue with simple record locking
my $lock_file="/tmp/.update.lock";
sysopen(LOCKFILE,$lock_file,O_RDONLY|O_CREAT);
# dont block for the lock, just error out
flock(LOCKFILE,LOCK_EX|LOCK_NB) || (
	print "there is a update already in progess\n" &&
	die "cant get LOCK_EX for /tmp/.update.lock :$!\n"
);

my $url = $ARGV[0];
my $dst_pak = "/tmp/mystro_install";

if (-e $dst_pak) {
	`rm -rf $dst_pak`;
}

my $ttys = `$who | $awk '{printf "/dev/%s ", \$2}'`;
my $teecmd = "$tee ".$ttys;

# download the url with fetch
`$echo "Now download the packetage..." | $teecmd`;
`$mystro_run $fetch -T $url_timeout -O $dst_pak $url | $teecmd`;

if ( ! -e $dst_pak ) {
	die "Error downloading package, Upgrade aborted\n";
}

`$echo "Download done. Now applying update..." | $teecmd`;
if (! -x $dst_pak) {
	`chmod 755 $dst_pak`;
}

my $ret = `$dst_pak`;

unlink $dst_pak;

if ($ret !~ /install ok/){
	`$echo "Update failed: $ret" | $teecmd`;
	exit(-1);
}

`echo "Starting Reboot ...\n" | $teecmd`;

`/bin/kill -INT 1`;

# Release the lockfile
close(LOCKFILE);
exit (0);

