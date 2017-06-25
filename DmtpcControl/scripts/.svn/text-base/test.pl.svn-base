#!/usr/bin/perl

use DM;
use GetTime;
use CGI ":standard";
use CGI::Carp qw(fatalsToBrowser);
use Chart::Graph::Gnuplot qw(&gnuplot);
$Chart::Graph::Gnuplot::gnuplot = "/usr/bin/gnuplot";
$Chart::Graph::Gnuplot::ppmtogif = "/usr/bin/ppmtogif";

print header();
print [{"title"=>"$WhichParam of $WhichTable", "type"=>"columns","style"=>"lines","using"=>"1:3"},\@tsdata,\@pdata]
