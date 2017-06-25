package DM;

use DBI;

#use CGI qw(:standard *table);
#use CGI::Carp qw(fatalsToBrowser);
# NOT IMPLEMENTED YET; COSMIN SAYS COMMENT OUT FOR NOW

use Net::SSH::Perl; 
use LWP::Simple;
use Email::Send::SMTP::Gmail;

$HOST = "m3slow.lns.mit.edu";
$DETECTOR = "1m3";
$SYNACCESS = "m3synacc.lns.mit.edu";
$SYNACCESS_IP = "198.125.161.189"; 
@CAMERA_HOST = ("","");
@CAMERA_NAME = ("","");
$DAQ = "m3daq.lns.mit.edu"; 
$HTMLDIR = "html/";
$APACHE_ROOT = "/var/www/"; 
$DAQ_IMG_DIR = "/home/dmatter/images";
$DAQ_WORK_DIR = "/home/dmatter/webdaq";
$USERNAME = "dmatter";
$PASSWORD = "seedark";
$DATABASE = "DMTPC_TEST";


$AUTORUN_COSMIC_LOCATION="WIPP"; 
$AUTORUN_COSMIC_NEVENTS=1000; 
$AUTORUN_COSMIC_EXPOSURE=1000; 

sub ssh_connect
{
  my $ssh = Net::SSH::Perl->new(@_[1],options=>["protocol 2,1"]);   
  $ssh->login($USERNAME,$PASSWORD);
  return $ssh; 
}


sub connect {

    my $database = "DMTPC_TEST";
    my $hostname = $HOST; 
    #my $username = "dmatter";
    #my $password = "seedark";

    my $dsn = "DBI:mysql:database=$DATABASE;host=$hostname";

    my $db_handle = DBI->connect($dsn, $USERNAME, $PASSWORD) 
        or die("Could not connect! \n");

    return $db_handle;
}

#alarm scripts - hold disables webpage
sub check_hold
{
  $db = DM->connect(); 
  $sql = "SELECT slow_hold FROM busy"; 
  $statement = $db->prepare($sql) or die "can't prepare query '$sql': $DBI::errstr\n"; 
  $statement->execute() or die "can't execute query '$sql': $DBI::errstr\n"; 
  $row_ref= $statement->fetchrow_hashref(); 
  return $row_ref->{slow_hold}; 
}


sub check_shutter
{
  $db = DM->connect(); 
  $sql = "SELECT shutter from shutter order by idx desc limit 1";
  $statement = $db->prepare($sql) or die "can't prepare query '$sql': $DBI::errstr\n"; 
  $statement->execute() or die "can't execute query '$sql': $DBI::errstr\n"; 
  $row_ref= $statement->fetchrow_hashref(); 
  return $row_ref->{shutter}; 
}

sub notify_dmtpc
{
    my ($dm, $subject, $message) = @_;
    system("echo '$message' | mail -s '$subject' leyton\@mit.edu");
    system("echo '$message' | mail -s '$subject' balewski\@mit.edu");
    system("echo '$message' | mail -s '$subject' ross.corliss\@gmail.com");

#    system("echo 'Subject: in-subject-22 fsdjfhdj' | msmtp balewski\@mit.edu)");
    # cat msg.txt
    # Subject:
    # <body here>
#my $mail=Email::Send::SMTP::Gmail->new( -smtp=>$SMTP,
#                                        -login=>$EMAIL,
#                                        -pass=>$EMAIL_PW,
#                                        -layer=> 'ssl',
#                                        -port=> 465); #,-debug=> 1);

#$mail->send(-to=>$EMAIL, -subject=>$subject, -body=>$message);
#$mail->bye;

}

1;

