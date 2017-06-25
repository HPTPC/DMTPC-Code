//
//Basic program to access both databases and get the power-supply current-draw
//Needs modifying to be of use, currently not saved (flexibilty for save option)
//Only channel 3 in use at time of writing, loop required when others operational
//
//Author: Richard Eggleston Feb 2016

#include <mysql/mysql.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>


#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TTimeStamp.h>

#include "db_connect.hh"

int main(int argc, char ** argv){

  //argv[1] = list_of_run_nums.txt

  double min_current = 0;

  //set up sql db access
  db_access slow_access, run_access;
  char * slow_server = "m3slow.lns.mit.edu";
  char * slow_user = "dmatter";
  char * slow_pass = "";
  //TEST - control info
  slow_access.server = slow_server;
  slow_access.user = slow_user;
  slow_access.pass = slow_pass;
  slow_access.database = "DMTPC_TEST";
  //onlM3 - run info
  run_access.server = slow_server;
  run_access.user = slow_user;
  run_access.pass = slow_pass;
  run_access.database = "onlM3";
  
  //connect to databases
  std::cout<<"connecting to databases"<<std::endl;
  MYSQL * slow_db = initialiseDB(slow_access);
  std::cout<<"slow db connected"<<std::endl;
  MYSQL * run_db = initialiseDB(run_access);
  std::cout<<"run db connected"<<std::endl;

  //read in run numbers
  std::cout<<"reading in runs"<<std::endl;
  std::ifstream run_file(argv[1]);
  std::vector<TString> run_nums;
  std::string line;
  if(run_file.is_open()){
    while(getline(run_file,line)){
      run_nums.push_back(line);
    }
  }

  //current channels for the power supply
  TString A4shtli = "caen_brd1_ch0_i";
  TString A4shtri = "caen_brd1_ch1_i";
  TString A4shbri = "caen_brd1_ch2_i";
  TString A4shbli = "caen_brd1_ch3_i";

  TString channel[4] = {A4shtli,A4shtri,A4shbri,A4shbli};

  //currently only one channel in use:
  int ch = 3;
  //add loop for all channels

  std::cout<<"querying databases"<<std::endl;
  std::vector<int> tstamps;
  std::vector<double> currents;
  for(int run = 0; run < run_nums.size(); run++){
    std::cout<<"run: "<<run_nums.at(run)<<std::endl;
    //get run start timestamp
    TString runCondition = "runId = " + run_nums.at(run);
    std::cout<<"condition: "<<runCondition<<std::endl;
    int run_start = (int)getValue(run_db,"run_start","unix_timestamp(timestamp)",runCondition);
    int run_end = (int)getValue(run_db,"run_end","unix_timestamp(timestamp)",runCondition);
    std::cout<<"run: "<<run_nums.at(run)<<" time: "<<TTimeStamp::TTimeStamp(run_start)<<std::endl;
    //time-value condition
    TString timeCondition = Form("unix_timestamp(timestamp) > %d && unix_timestamp(timestamp) < %d ",run_start,run_end);
    TString valueCondition = Form("value > %f ",min_current);
    TString timeValueCondition = timeCondition + " && " + valueCondition;
    std::cout<<"timeValueCondition: "<<timeValueCondition<<std::endl;
    currents = getValues(slow_db,channel[ch],"value",timeValueCondition,false,tstamps);
  }

  std::cout<<"completed"<<std::endl;
  

  mysql_close(slow_db);
  mysql_close(run_db);

  

}
