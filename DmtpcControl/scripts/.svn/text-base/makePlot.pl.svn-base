#!/usr/bin/perl

use DM;
use GetTime;
use CGI ":standard";
use CGI::Carp qw(fatalsToBrowser);
use Chart::Graph::Gnuplot qw(&gnuplot);
$Chart::Graph::Gnuplot::gnuplot = "/usr/bin/gnuplot";
$Chart::Graph::Gnuplot::ppmtogif = "/usr/bin/ppmtogif";

print header();




my $minTime = GetTime->getTime(6);
my $maxTime = GetTime->getTime;

if (param("START_TIME")) {$minTime=param("START_TIME");}
if (param("END_TIME")) {$maxTime=param("END_TIME");}
#print $minTime." ".$maxTime;
if ($maxTime < $minTime){
($maxTime, $minTime) = ($minTime, $maxTime);
}


my $WhichParam = param("PARAM") or die "No parameter defined!";
my $WhichTable = param("TABLE") or die "No table defined!";
my $plotWithLines = param("PLOTWLINES");

$db_handle = DM->connect
    or die("Could not connect! \n");

$tmpdir="$DM::APACHE_ROOT/$DM::HTMLDIR/tmp";
$url="/tmp";

my @pdata;
my @tsdata;
$sql = "SELECT * FROM $WhichTable WHERE timestamp>'$minTime' && timestamp<'$maxTime'";
if (param(MINVAL)){my $minVal = param(MINVAL); $sql = $sql." && $WhichParam > $minVal"; }
if (param(MAXVAL)){my $maxVal = param(MAXVAL); $sql = $sql." && $WhichParam < $maxVal"; }
$sql.=" ORDER BY timestamp ASC";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
while ($row_ref = $statement->fetchrow_hashref())
{
  push( @tsdata, $row_ref->{timestamp});
  push( @pdata, $row_ref->{$WhichParam});
}

#Get rough estimates of median and standard deviation, remove outliers
my $count = @pdata;
if ($count > 0){
#$ave = 0;
#$valsq = 0;
#@sorted = sort {$a<=>$b}@pdata;

#my $median;
#if ($count % 2 == 0){
# $median = ($sorted[$count/2]+$sorted[$count/2-1])/2;
#}else{
# $median = $sorted[($count-1)/2];
#}
#for($i=0; $i<$count; $i++){
#  $ave = $ave+$pdata[$i];
#  $valsq = $valsq+$pdata[$i]^2;
#}
#for ($i=0; $i<$count; $i++){
#  if (abs($pdata[$i]-$median)>10*$stddev){
#    delete $pdata[$i];
#    delete $tsdata[$i];
#  }
#}
    
    # default is to plot without lines
    @plotoptions=[{"title"=>"$WhichParam of $WhichTable", "type"=>"columns","using"=>"1:3"},\@tsdata,\@pdata];
    if($plotWithLines eq 'on'){
	@plotoptions=[{"title"=>"$WhichParam of $WhichTable", "type"=>"columns","style"=>"lines","using"=>"1:3"},\@tsdata,\@pdata];
    }
    
   my $imageName="${WhichParam}_in_${WhichTable}_tmp".time().".gif";
   # my $imageName="${WhichParam}_in_${WhichTable}_tmp".gif;
    gnuplot({"title"=>"$WhichTable Chart","output file"=>"$tmpdir/$imageName","output type"=>"gif",
	     "x-axis label"=>"Time Stamp","y-axis label"=>"$WhichParam",
	     "xdata"=>"time","timefmt"=>"%Y-%m-%d %H:%M:%S",
	     "extra_opts"=>join("\n", "set grid") },
	    @plotoptions
	    ) or die "YOU SUCK, BAYES\n";
    
    print "$url/$imageName";
}
else{
print "gfx/norecent.png"; 
}
$db_handle->disconnect();
