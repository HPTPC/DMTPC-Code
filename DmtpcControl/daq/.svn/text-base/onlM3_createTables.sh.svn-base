#!/bin/bash
dbName=onlM3
#hostN=m3slow.lns.mit.edu ; dbUser=balewski ;dbPass=jan
hostN=localhost ; dbUser=dmatter ;dbPass=seedark

echo  create all onlM3 tables
MYSQL="mysql -h $hostN -u $dbUser -p$dbPass -D ${dbName}"

# mysql -u balewski -pjan -h m3slow.lns.mit.edu
# mysql -u dmatter -pseedark
# use onlM3;

echo list all tables currently existing
$MYSQL  -BNe "show tables; "

if [ 0 -eq 1 ] ; then  # run_lock
    $MYSQL  -BNe "CREATE TABLE run_lock (runId INT, operator VARCHAR(20), timeStamp TIMESTAMP, remark VARCHAR(140));"
fi 



if [ 0 -eq 1 ] ; then 
    echo "--------- authorize dmatter from remote machines  ---"
    $MYSQL  -BNe " SELECT user, host FROM mysql.user;"
    #$MYSQL  -BNe "  "
    $MYSQL  -BNe "create user dmatter2@m3daq.lns.mit.edu identified by 'seedark'; "
    $MYSQL  -BNe "grant all privileges on *.* to dmatter2@m3daq.lns.mit.edu  with grant option;  "
    $MYSQL  -BNe " FLUSH PRIVILEGES; " 
fi

if [ 0 -eq 1 ] ; then 
    tableName=run_start
    echo "--------- initializing  ${tableName}  ---"
    $MYSQL  -BNe "CREATE TABLE ${tableName} (runId INT, setNumBias INT, setNumExpo INT, expoTime4sh INT, expoTime1sh INT, timeStamp TIMESTAMP , outName VARCHAR(140)) ;"
    $MYSQL  -BNe "INSERT INTO ${tableName} (runId,setNumBias,setNumExpo,timeStamp,outName) VALUES ( 111000,7,11,2000,0NOW(),'fake-m3-out.root') ;"
    $MYSQL  -BNe "select * from ${tableName} order by timeStamp desc limit 3;"

    unset tableName  
#to flush whole table:   delete from ${tableName} ;
#to delete table:  drop table  ${tableName} ;
fi


if [ 0 -eq 1 ] ; then 
    tableName=run_end
    echo "--------- initializing  ${tableName}  ---"
    $MYSQL  -BNe "CREATE TABLE ${tableName} (runId INT, timeStart TIMESTAMP , timeStop TIMESTAMP, quality  VARCHAR(20) , numExposures INT ,timeStamp TIMESTAMP  , remark  VARCHAR(140) ) ;"
    $MYSQL  -BNe "INSERT INTO ${tableName} (runId, timeStart,  timeStop, quality, numExposures, timeStamp, remark) VALUES ( 112000,NOW(),NOW(),'void', 33,NOW(),'fake-end-run') ;"
    $MYSQL  -BNe "select * from ${tableName} order by timeStamp desc limit 3;"

    unset tableName  

fi


if [ 0 -eq 1 ] ; then 
    tableName=avr_occupancy_scope0
    echo "--------- initializing  ${tableName}  ---"
    $MYSQL  -BNe "CREATE TABLE ${tableName} ( timeStamp TIMESTAMP , value FLOAT) ;"
    $MYSQL  -BNe "INSERT INTO ${tableName} (timeStamp, value  ) VALUES ( NOW(), 1.56) ;"
    $MYSQL  -BNe "select * from ${tableName} order by timeStamp desc limit 3;"
    unset tableName  
fi


if [ 0 -eq 1 ] ; then 
    tableName=caesar
    
    echo "--------- initializing  ${tableName}  ---"
    $MYSQL  -BNe "CREATE TABLE ${tableName} ( timeStamp TIMESTAMP , clientIp VARCHAR(20), operator VARCHAR(20), geoLocation VARCHAR(120), gpsLocation VARCHAR(120)) ;"
    $MYSQL  -BNe "INSERT INTO ${tableName} (timeStamp,clientIp,operator,geoLocation,gpsLocation   ) VALUES ( NOW(),'123.123.10.10' , 'bird','poznan.poland','42.4247,-71.1112' ) ;"
    $MYSQL  -BNe "select * from ${tableName} order by timeStamp desc limit 3;"
    unset tableName  
fi

if [ 0 -eq 1 ] ; then 
    tableName=frame_image_count
    echo "--------- initializing  ${tableName}  ---"
    $MYSQL  -BNe "CREATE TABLE ${tableName} ( timeStamp TIMESTAMP , count INT) ;"
    $MYSQL  -BNe "INSERT INTO ${tableName} (timeStamp, count  ) VALUES ( NOW(), 12) ;"
    $MYSQL  -BNe "select * from ${tableName} order by timeStamp desc limit 3;"
    unset tableName  
fi


if [ 0 -eq 1 ] ; then 

    for i in `seq 0 -1`; do
	echo  drop $i
	$MYSQL  -BNe "DROP TABLE dc_residue_scope0_ch$i ;"
    done    
    
    tableName=dc_residue_scope0_ch0
    echo "--------- initializing  ${tableName}  ---"
    $MYSQL  -BNe "CREATE TABLE ${tableName} ( runId INT, value FLOAT, sigma FLOAT, status INT, timeStamp TIMESTAMP );"
    $MYSQL  -BNe "INSERT INTO ${tableName} (runId,value,sigma,status,timeStamp)  VALUES ( 222001, 66, 10, 1, NOW());"
    $MYSQL  -BNe "select * from ${tableName} order by runId desc limit 3;"

  for i in `seq 1 7`; do
	#echo add similar table for ch$i
	 tableName1=dc_residue_scope0_ch$i
	 echo $i $tableName1
	 #$MYSQL  -BNe " DROP TABLE  ${tableName1};" ; continue
	 $MYSQL  -BNe "CREATE TABLE   ${tableName1} LIKE ${tableName};"
	 $MYSQL  -BNe "INSERT INTO ${tableName1} (runId,value,sigma,status,timeStamp)  VALUES ( 222001, 6$i, 1$i, 0, NOW());"
	 $MYSQL  -BNe "select * from ${tableName1} order by runId desc limit 3;"
   done
    
    unset tableName  
fi


if [ 0 -eq 1 ] ; then
    tableName=run_startX
    echo "--------- add new column  to  ${tableName}  ---"
    $MYSQL  -BNe "ALTER TABLE ${tableName}  ADD expoTime1sh INT AFTER setNumExpo; "
    $MYSQL  -BNe "ALTER TABLE ${tableName}  ADD expoTime4sh INT AFTER setNumExpo; "
    $MYSQL  -BNe "INSERT INTO ${tableName} (runId,setNumBias,setNumExpo,expoTime4sh,expoTime1sh,timeStamp,outName) VALUES ( 111000,9,19,2000,0,NOW(),'fake-m5-out.root') ;"

    $MYSQL  -BNe "select * from ${tableName} order by timeStamp desc limit 3;"
    unset tableName  
fi







