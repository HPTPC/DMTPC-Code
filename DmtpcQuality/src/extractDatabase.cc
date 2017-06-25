#include <mysql/mysql.h>
#include <sqlite3.h>

#include <iostream>
#include <string>
#include <fstream>

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
#include <string>

int getExposure(){

  int exp;
  std::cout<<"Please enter the exposure length for the data you are processing (s): ";
  std::cin>>exp;
  std::cout<<"You have entered: "<<exp<<" s"<<std::endl;
  return exp;
}


int main(int argc, char ** argv){

  //  bool verbose = true;
  bool have_sql_exp;//for backwards compatibility with runs that don't have the exposure store
  int exp;
  std::string have_exp;
  std::cout<<"Is the exposure information in the database for these runs? (y/n)"<<std::endl;
  std::cin>>have_exp;
  if(strcmp(have_exp.c_str(),"n")==0){
    have_sql_exp = false;
    exp = getExposure();
  }
  else{
    have_sql_exp = true;
  }
  
  //argv[1] = list_of_runs.txt

  //minimum current to classify as spark (to account for leakage current)
  double min_current = 0;//database only registering 10s of mA

  //----------set up ------
  TString data_dir = "/data/2016/02/raw/";//m3daq
  TString db_dir = "/Calibration/2016/02/Quality/";
  TString tag = "Michael";

  //read in run numbers
  std::cout<<"reading in runs"<<std::endl;
  std::ifstream run_file(argv[1]);
  std::vector<TString> run_nums;
  std::string line;
  if(run_file.is_open()){
    while(getline(run_file,line)){
      std::cout<<"run: "<<line<<std::endl;
      run_nums.push_back(line);
    }
  }
  //output for sqlite
  int nruns = run_nums.size();
  TString sqlite_file = db_dir +  run_nums.at(0) + "-" + run_nums.at(nruns-1) + ".db";

  std::cout<<"sqlite_file: "<<sqlite_file<<std::endl;
  //-------------------set up sql db access--------------
  //  db_access slow_access, run_access;
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
  sqlite.createSparkTable();
  sqlite.createRunTable();
  sqlite.createEventTable();

  //----------get the values from mysql databases---------
  std::cout<<"querying databases"<<std::endl;
  for(unsigned int run = 0; run < run_nums.size(); run++){
    std::cout<<"run: "<<run_nums.at(run)<<std::endl;

    //get run info
    TString runCondition = "runId = " + run_nums.at(run);
    int run_start = (int)run_db.getValue("run_start","unix_timestamp(timestamp)",runCondition);
    int run_end = (int)run_db.getValue("run_end","unix_timestamp(timestamp)",runCondition);
    int nbias = (int)run_db.getValue("run_start","setNumBias",runCondition);
    int nexp = (int)run_db.getValue("run_start","setNumExpo",runCondition);
    //    int exp4;
    //
    int exp4;
    if(have_sql_exp){
      exp4 = (int)run_db.getValue("run_start","expoTime4sh",runCondition);
    }
    else{
      exp4 = exp;
    }
    //sqlite command
    TString store_cmd;
    store_cmd = "INSERT INTO RUN (run_id, run_start_s, run_start_ns, run_end_s, run_end_ns, nbias, nexp, exp4) ";
    store_cmd += Form("VALUES (%d, %d, %d, %d, %d, %d, %d, %d); ", run_nums.at(run).Atoi(), run_start, 0, run_end, 0, nbias, nexp, exp4);
    
    //store
    sqlite.storeValues(store_cmd);

    //setup time and valuecondition
    TString timeCondition = Form("unix_timestamp(timestamp) >= %d && unix_timestamp(timestamp) <= %d ",run_start,run_end);
    TString valueCondition = Form("value > %f ",min_current);
    TString timeValueCondition = timeCondition + " && " + valueCondition;
    std::vector< std::pair<uint32_t, uint32_t> > tstamps;//pair for seconds and ns

    //--loop over channels
    for(int ch = 0; ch < nchan; ch++){
      std::vector<double> currents = slow_db.getValues(channel[ch],"value",timeValueCondition,true,tstamps);
      //--insert entries into sqlite database
      for(unsigned int i = 0; i < currents.size(); i++){
	//spark storage command
	store_cmd = "INSERT INTO SPARKS (run_id, channel, timestamp_s, timestamp_ns, current) ";
	store_cmd += Form("VALUES (%d, %d, %d, %d, %f); ", run_nums.at(run).Atoi(), ch, tstamps.at(i).first, tstamps.at(i).second, currents.at(i));
	//store the values
	sqlite.storeValues(store_cmd);
      }
    }


    //load raw file and store event data
    /*-----
    TString rawname = data_dir + "m3_" + tag + "_R" + run_nums.at(run) + ".raw.root";
    TFile rawfile(rawname,"read");
    TTree * rawtree = (TTree*)rawfile.Get("data");
    TTimeStamp * ev_time = 0;
    rawtree->SetBranchAddress("timestamp",&ev_time);
    //    rawtree->SetBranchStatus("*",0);//switch off all other branches
    //    rawtree->SetBranchStatus("timestamp",1);//switch on timestamp (faster processing)
    for(int ev = 0; ev < rawtree->GetEntries(); ev++){
      rawtree->GetEntry(ev);
      if(ev%20==0) std::cout<<"ev: "<<ev<<", time: "<<ev_time->GetSec()<<std::endl;
      store_cmd = "INSERT INTO EVENTS (run_id, ev, ev_start_s, ev_start_ns) ";
      store_cmd += Form("VALUES (%d, %d, %d, %d); ", run_nums.at(run).Atoi(), ev, ev_time->GetSec(), 0);
      sqlite.storeValues(store_cmd);
    }
    */

    //use frame_image_count table to get event timestamps
    std::vector< std::pair< uint32_t, uint32_t> > e_tstamps;
    std::cout<<"Time condition: "<<timeCondition<<std::endl;
    std::vector<double> im_count = run_db.getValues("frame_image_count","count",timeCondition, true, e_tstamps);
    std::cout<<"im_count size: "<<im_count.size()<<std::endl;
    for(int ev = 0; ev < im_count.size(); ev++){
      if(ev%20==0) std::cout<<"Event: "<<ev<<std::endl;
      store_cmd = "INSERT INTO EVENTS (run_id, ev, ev_start_s, ev_start_ns) ";
      store_cmd += Form("VALUES (%d, %d, %d, %d); ", run_nums.at(run).Atoi(), ev, e_tstamps.at(ev), 0);
      sqlite.storeValues(store_cmd);
    }
    

  }
  
  std::cout<<"completed"<<std::endl;
  
}
