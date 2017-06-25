#include <sqlite3.h>
#include "SQLite.hh"

#include <TString.h>
#include <TTimeStamp.h>

#include <iostream>
#include <cstdlib>
#include <cstdio>

#include "Dataset.hh"
#include "Event.hh"

using namespace dmtpc::core;

int main(int argc, char ** argv){

  //  TString db_name = "spark.db";
  //argv[1] = database
  //argv[2] = run
  //  char * data_dir = "/scratch1/darkmatter/dmtpc/data/m3/2016/";
  char * data_dir = "/data/2016/02/raw/";

  TString db_name = argv[1];
  int run = atoi(argv[2]);
  int exp = 30;
  int nchan = 4;

  dmtpc::analysis::SQLite db;
  db.open(db_name);
  
  //create sql statement
  TString table = " sparks ";

  //open dataset
  Dataset d; 
  d.open(Form("%s/m3_Michael_R%d.raw.root",data_dir,run));

  //retrieve values
  std::cout<<"getting values"<<std::endl;
  for(int ev = 0; ev < d.nevents(); ev++){
    d.getEvent(ev);
    int ev_time = d.event()->getTimestamp()->GetSec();
    //    std::cout<<"ev_time: "<<ev_time<<std::endl;
    for(int i = 0; i < nchan; i++){
      TString condition = "channel = " + TString::Itoa(i,10);
      condition += " and timestamp >" + TString::Itoa(ev_time,10);
      condition += " and timestamp < " + TString::Itoa(ev_time+exp,10);
      TString column = " current ";
      std::vector<double> currents = db.getDValues(table,column,condition);
      column = " timestamp ";
      std::vector<int> times = db.getIValues(table,column,condition);
      
      //for now, couting, can use how you like
      for(int i = 0; i < currents.size(); i++){
	//    std::cout<<"time: "<<times.at(i)<<", current: "<<currents.at(i)<<std::endl;
	//std::cout<<"Event: "<<ev<<" has an associated power-supply current-draw"<<std::endl;
	
	std::cout<<"Event: "<<ev<<" has current: "<<currents.at(i)<<std::endl;
      }
    }
  }

  db.close();
  
  return 0;
  
}
