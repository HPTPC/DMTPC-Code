package GetTime;

#Returns timestamp in the same format used by our mySQL database
#YYYY-MM-DD HH:MM:SS
#subtracts _[0] number of hours from current time.
sub getTime{

my ($sec,$min,$hour,$mday,$month,$year,$wday,$yday,$isdst) = gmtime();
$year = $year+1900;
$month = $month+1;
if ($_[1]){

  if ($_[1] > 24){print "Warning: going back too many hours!! Timestamp will not be correct";}
  if ($hour >= $_[1]) {$hour = $hour - $_[1];}
  else{
    $hour = 24 - $_[1]+$hour;
    if ($mday > 1){
      $mday = $mday - 1;
    }else{
      if ($month > 1){
        $month = $month - 1;
        if ($month==2 && ($year%4)==0){$mday=29;}
        elsif ($month==2 && ($year%4) != 0){$mday=28;}
        elsif ($month==4 || $month==6 ||$month==9 || $month==11) {$mday=30;}
        else {$mday=31;}
      }else{
        $year = $year-1;
        $month = 12;
        $mday = 31;
      }
    }
  }
}
#print $month;
if ($sec < 10) {$sec = "0".$sec;}
if ($min < 10) {$min = "0".$min;}
if ($mday < 10) {$mday = "0".$mday;}
if ($month < 10) {$month = "0".$month;}
"$year-$month-$mday"." "."$hour:$min:$sec";

}

1;
