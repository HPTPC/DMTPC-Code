#! /usr/bin/perl
use CGI ':standard';
use CGI::Carp qw(fatalsToBrowser);
use DM; 
my $tmpdir="$DM::APACHE_ROOT/$DM::HTMLDIR/tmp"; 
print header();

#exec "rm -f $tmpdir/*.gif";
exec 'find '.$tmpdir.' -name *gif -mmin +1 -exec rm {} \;'; 
