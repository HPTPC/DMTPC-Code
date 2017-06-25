#!/usr/bin/perl

use DM;

use CGI ':standard';
use CGI::Carp qw(fatalsToBrowser);

print "Content-type:text/html\n\n";

#if (!param("table")) {
#  die "Unknown table: $table \n";
#}

# first check if script is already running to reduce CPU load
$count=`ps -A | grep printpars.pl | wc -l`;
if ($count>1) { die "printpars.pl already running"; }

$db_handle = DM->connect
    or die("Could not connect! \n");

print '<h3>Status reports</h3>';
print '<small>';
print '<table border="1">'."\n";
my $datetime = localtime();
my $utctime = gmtime();

print "<tr><td> Item </td><td>Status</td></tr>";
print "<tr><td>Local time </td><td> <SMALL>$datetime</SMALL> </td></tr> \n\n";
print "<tr><td>UTC time </td><td> <SMALL>$utctime</SMALL> </td></tr>\n\n";

$slowControlStatus="<span style='background-color:red'>OFF</span>";
@ARGV = ("ps -A | grep DMSlow |");
while (<>) {
  $slowControlStatus="<span style='background-color:green'>ON</span>";
}
print "<tr><td>Slow control</td><td>$slowControlStatus</td></tr>\n";


$pressureControlStatus="<span style='background-color:red'>OFF</span>";
@ARGV = ("ps -A | grep DMPressure |");
while (<>) {
  $pressureControlStatus="<span style='background-color:green'>ON</span>";
}
print "<tr><td>Pressure control</td><td>$pressureControlStatus</td></tr>\n";

$sql = "select * from hv_status ORDER BY timestamp DESC LIMIT 1";
$statement = $db_handle->prepare($sql)
    or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute()
    or die "Couldn't execute query '$sql': $DBI::errstr\n";
$row_ref = $statement->fetchrow_hashref();
if ($row_ref->{value} > 0) {
	print "<tr><td>HV</td><td> <span style='background-color:green'>ON</span></td></tr>\n";
}
else {
	print "<tr><td>HV </td><td><span style='background-color:red'>OFF</span></td></tr>\n";
}

$sparkMonitorStatus="<span style='background-color:red'>OFF</span>";
@ARGV = ("ps -A | grep sparkMonitor |");
while (<>) {
    $sparkMonitorStatus="<span style='background-color:green'>ON</span>";
}
print "<tr><td>Spark monitor</td><td>$sparkMonitorStatus</td></tr>\n";

print "</table>\n\n";
print '</small>';

#Environmental parameters
print "<h3>Environmental data<\h3><table border='1'>";
print '<small>';
print "<tr><td> Item </td><td> Value </td></tr>";

#@environ_params = qw(set_pressure chamber_pressure_cdg turbo_pressure_cdg chamber_pressure_bpg temp0 temp1);
#@environ_titles = ("Set pressure (Torr)", "Chamber pressure CDG (Torr)", "Pump-line pressure CDG (Torr)", "Chamber pressure BPG (Torr)", "Temperature 0 (C)", "Temperature 1 (C)");
#@environ_format = qw(%.1f %.2f %.2f %.2e %.0f %.2f);

@environ_params = qw(chamber_pressure_cdg turbo_pressure_cdg chamber_pressure_bpg temp0 temp1);
@environ_titles = ("Chamber pressure CDG (Torr)", "Pump-line pressure CDG (Torr)", "Chamber pressure BPG (Torr)", "Ambient temperature (C)");
@environ_format = qw(%.2f %.2f %.2e %.0f);

my $count=0;
foreach $param (@environ_params) {

    #Query database and print table
    $sql = "select * from " . $param . " ORDER BY timestamp DESC LIMIT 1";
    $statement = $db_handle->prepare($sql)
	or die "Couldn't prepare query '$sql': $DBI::errstr\n";
    $statement->execute()
	or die "Couldn't execute query '$sql': $DBI::errstr\n";
    $row_ref = $statement->fetchrow_hashref();
    printf  "<tr><td>" . $environ_titles[$count] . "</td><td>" . $environ_format[$count] . "</td></tr>\n",$row_ref->{value};

    $count++;
}

print "</table>";
print '</small>';

#Control voltages and read currents
print "<h3>Control voltages<\h3><table border='1'>";
print '<small>';
print "<tr><td> Item </td><td>Voltage [V]</td><td>Current [uA]</td><td>Set voltage [V] </td><td>Status</td></tr>";

@control_params = qw(caen_brd1_ch0 caen_brd1_ch1 caen_brd1_ch2 caen_brd1_ch3 caen_brd0_ch3 caen_brd0_ch1);
@control_titles = ("Anode 4sh-TL", "Anode 4sh-TR", "Anode 4sh-BR", "Anode 4sh-BL", "Cathode 4sh", "PMTs");
#@control_format = qw(%.4f %4f %4f %4f %4f);

my $count=0;
foreach $param (@control_params) {

    #Query database for latest set value
    $set_volt = $param . "_set_hv";
    $sql = "select * from " . $set_volt . " ORDER BY timestamp DESC LIMIT 1";
    $statement = $db_handle->prepare($sql)
	or die "Couldn't prepare query '$sql': $DBI::errstr\n";
    $statement->execute()
	or die "Couldn't execute query '$sql': $DBI::errstr\n";
    $set_row_ref = $statement->fetchrow_hashref(); 

    #Query database for latest status value
    $read_status = $param . "_status";
    $sql = "select * from " . $read_status . " ORDER BY timestamp DESC LIMIT 1";
     $statement = $db_handle->prepare($sql)
	or die "Couldn't prepare query '$sql': $DBI::errstr\n";
    $statement->execute()
	or die "Couldn't execute query '$sql': $DBI::errstr\n";
    $status_row_ref = $statement->fetchrow_hashref();

    $stat = $status_row_ref->{value};

    if    ($stat & 8192) { $status = "NOCAL"; }
    elsif ($stat & 4096) { $status = "ITLK";  }
    elsif ($stat & 2048) { $status = "KILL";  }
    elsif ($stat & 1024) { $status = "DIS";   }
    elsif ($stat & 512)  { $status = "OVT";   }
    elsif ($stat & 128)  { $status = "TRIP";  }
    elsif ($stat & 256)  { $status = "OVP";   }
    elsif ($stat & 64)   { $status = "MAXV";  }
    elsif ($stat & 32)   { $status = "UNV";   }
    elsif ($stat & 16)   { $status = "OVV";   }
    elsif ($stat & 8)    { $status = "OVC";   }
    elsif ($stat & 4)    { $status = "RDWN";  }
    elsif ($stat & 2)    { $status = "RUP";   }
    elsif ($stat & 1)    { $status = "ON";    }
    else                 { $status = "OFF";   }

    #Query database for latest voltage value
    $read_volt = $param . "_hv";
    $sql = "select * from " . $read_volt . " ORDER BY timestamp DESC LIMIT 1";
    $statement = $db_handle->prepare($sql)
	or die "Couldn't prepare query '$sql': $DBI::errstr\n";
    $statement->execute()
	or die "Couldn't execute query '$sql': $DBI::errstr\n";
    $volt_row_ref = $statement->fetchrow_hashref();
    
    #Query database for latest current value
    $read_curr = $param . "_i";
    $sql = "select * from " . $read_curr . " ORDER BY timestamp DESC LIMIT 1";
    $statement = $db_handle->prepare($sql)
	or die "Couldn't prepare query '$sql': $DBI::errstr\n";
    $statement->execute()
	or die "Couldn't execute query '$sql': $DBI::errstr\n";
    $curr_row_ref = $statement->fetchrow_hashref();

    #Query database for average current value (for spark monitoring)
    $sql = "select AVG(a.value) as avg_current from (select value from " . $read_curr . " ORDER BY timestamp DESC LIMIT 15) a";
    $statement = $db_handle->prepare($sql)
	or die "Couldn't prepare query '$sql': $DBI::errstr\n";
    $statement->execute()
	or die "Couldn't execute query '$sql': $DBI::errstr\n";
    $avg_curr = $statement->fetchrow_array;

    #if ($sparkMonitor && $avg_curr > 10.0 && $volt_row_ref->{value} > 0.0) 


    #Print table
    printf  "<tr><td>" . $control_titles[$count] . "</td><td>%.1f</td><td>%.2f</td><td>%.1f</td><td><b>$status<b></td></tr> \n",
    $volt_row_ref->{value},$curr_row_ref->{value},$set_row_ref->{value};

    $count++;
}

print "</table>";
print "</small>";
print end_html();
$statement->finish();
$db_handle->disconnect();


