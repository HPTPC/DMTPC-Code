#include <mysql/mysql.h>
#include <sqlite3.h>

#include <iostream>
#include <string>

#include <TString.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TTimeStamp.h>

#include "MySQL.hh"
#include "SQLite.hh"

#include <cstdlib>
#include <sstream>
#include <vector>

int main(int argc, char ** argv){

  //argv[1]: run number

  //  bool verbose = true;
  
  //--setup file names and locations
  //this can be moved to user input if more helpful
  //  TString data_dir = "/scratch1/darkmatter/dmtpc/data/m3/2016/";//RHUL
  TString data_dir = "/data/2016/02/";//m3daq
  TString tag = "Michael";
  TString run_number = argv[1];
  TString dataFileName = data_dir + "raw/" + "m3_" + tag + "_R" + run_number + ".raw.root";
  TString sqlite_file = dataFileName;
  sqlite_file.ReplaceAll("raw/","db/");
  sqlite_file.ReplaceAll(".root",".db");

  //minimum current to classify as spark (to account for leakage current)
  double min_current = 0;//database only registering 10s of mA

  //-------------------set up sql db access--------------
  char * server = "m3slow.lns.mit.edu";
  char * user = "dmatter";
  char * pass = "";
  char * slowdb = "DMTPC_TEST";
  char * rundb = "onlM3";
  
  //connect to databases
  std::cout<<"connecting to databases"<<std::endl;
  dmtpc::Quality::MySQL slow_db, run_db;
  slow_db.initialiseDB(server,user,pass,slowdb);
  std::cout<<"slow db connected"<<std::endl;
  run_db.initialiseDB(server,user,pass,rundb);
  std::cout<<"run db connected"<<std::endl;

  //current channels for the power supply
  const int nchan = 4;//modify for adding 1sh channels
  TString A4shtli = "caen_brd1_ch0_i";
  TString A4shtri = "caen_brd1_ch1_i";
  TString A4shbri = "caen_brd1_ch2_i";
  TString A4shbli = "caen_brd1_ch3_i";

  TString channel[nchan] = {A4shtli,A4shtri,A4shbri,A4shbli};


  //------------create sqlite3 database----------
  dmtpc::Quality::SQLite sqlite;
  sqlite.setName(sqlite_file);
  sqlite.createDatabase();

  //create sql statement to generate table
  //  TString cmd = TString("CREATE TABLE SPARKS(run_id INT NOT NULL, channel INT NOT NULL, run_start TIMESTAMP NOT NULL, run_end TIMESTAMP NOT NULL, timestamp TIMESTAMP NOT NULL, current REAL NOT NULL);");

  //  std::cout<<"sqlite table cmd: "<<cmd<<std::endl;
  sqlite.createSparkTable();

  //----------get the values from mysql databases---------
  std::cout<<"querying databases"<<std::endl;
  //get run start and end timestamp
  TString runCondition = "runId = " + run_number;
  //  std::cout<<"condition: "<<runCondition<<std::endl;
  int run_start = (int)run_db.getValue("run_start","unix_timestamp(timestamp)",runCondition);
  //  std::cout<<"run: "<<run_number<<" time: "<<TTimeStamp::TTimeStamp(run_start)<<std::endl;
  int run_end = (int)run_db.getValue("run_end","unix_timestamp(timestamp)",runCondition);
  //setup time and valuecondition
  TString timeCondition = Form("unix_timestamp(timestamp) > %d && unix_timestamp(timestamp) < %d ",run_start,run_end);
  TString valueCondition = Form("value > %f ",min_current);
  TString timeValueCondition = timeCondition + " && " + valueCondition;
  std::cout<<"timeValueCondition: "<<timeValueCondition<<std::endl;
  std::vector< std::pair<uint32_t, uint32_t > > tstamps;
  //--loop over channels
  for(int ch = 0; ch < nchan; ch++){
    std::vector<double> currents = slow_db.getValues(channel[ch],"value",timeValueCondition,true,tstamps);
    //--insert entries into sqlite database
    for(unsigned int i = 0; i < currents.size(); i++){
      //storage command
      TString store_cmd = "INSERT INTO SPARKS (run_id, channel, run_start, run_end, timestamp_s, timestamp_ns current) ";
      store_cmd += Form("VALUES (%d, %d, %d, %d, %d, %f); ", run_number.Atoi(), ch, run_start, run_end, tstamps.at(i).first, tstamps.at(i).second, currents.at(i));
      //store the values
      //  std::cout<<"store cmd: "<<store_cmd<<std::endl;
      sqlite.storeValues(store_cmd);
    }
  }
  std::cout<<"completed"<<std::endl;
  
 //close the database connections
  //not necessary
  //  slow_db.close();
  //  run_db.close();
  //  sqlite.close();

}
