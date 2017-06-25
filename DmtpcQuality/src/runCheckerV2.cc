#include "RunQuality.hh"

#include "TString.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

int main(int argc, char ** argv){

  //user input
  //argv[1] = runs.txt
  //argv[2] = year e.g. 2016
  //argv[3] = month e.g. 03
 
  //---user modifications (?)
  int nchan = 4;
  //---end user modifications

  //---read in run numbers
  std::fstream infile(argv[1]);
  std::string line;
  std::vector<TString> run_nums;
  if(infile.is_open()){
    while(getline(infile, line)){
      //std::cout<<"adding run "<<line<<std::endl;
      run_nums.push_back(TString(line));
    }
  }

  //---get sqlite database file
  int nruns = run_nums.size();
  TString run_start = run_nums.at(0);
  TString run_end = run_nums.at(nruns-1);
  //TString db_dir = "/scratch1/darkmatter/dmtpc/Calibration/2016/02/Quality/";//RHUL
  TString db_dir = Form("/Calibration/%s/%s/Quality/",argv[2],argv[3]);
  TString dbName;
  std::string auto_db = "y";
  //  std::cout<<"auto-load DB? (y/n)"<<std::endl;//mainly for testing
  //  std::cin>>auto_db;
  if(strcmp(auto_db.c_str(),"y") == 0){
    dbName = db_dir + run_start + "-" + run_end + ".db";
  }
  else{
    std::cout<<"Enter DB name: "<<std::endl; 
    std::cin>>dbName;
  }
  
  //---loop over runs
  for(int run = 0; run < nruns; run++){

    //---setup RunQuality
    std::cout<<"Checking run: "<<run_nums.at(run)<<std::endl;
    dmtpc::Quality::RunQuality runQ = dmtpc::Quality::RunQuality(run_nums.at(run),dbName);
    runQ.setNChannels(nchan);
    //runQ.setDebug();//currently very verbose!
    //---end setup RunQuality

    //---get only spark events
    std::vector< std::vector< int > > sparkEvents = runQ.getSparkTaggedEvents();

    //---print out results
    //---replace with desired output or further use sparkEvents vector
    /*
    for(unsigned int ch = 0; ch < sparkEvents.size(); ch++){
      for(unsigned int ev = 0; ev < sparkEvents.at(ch).size(); ev++){
	if(sparkEvents.at(ch).at(ev) == 1){
	  double spark_value = runQ.getLastCurrent(ev,ch);
	  int spark_time = runQ.getLastCurrentTime(ev,ch);
	  std::cout<<"run: "<<runQ.runid()<<", ev: "<<ev<<", channel: "<<ch<<", spark: "<<sparkEvents.at(ch).at(ev)<<", spark value: "<<spark_value<<", spark_time: "<<spark_time<<std::endl;
	  
	}
      }
    }
    */
    std::cout<<"-----------"<<std::endl;//for clarity
    //---Alternative method calling isSpark method on all events (mainly for debugging but just as useable)
    for(int ch = 0; ch < 4; ch++){
      for(int ev = 0; ev < runQ.nevents(); ev++){
	//--check event and channel
	int spark = runQ.isSpark(ev,ch);
	if(spark == 1){
	  double spark_value = runQ.getLastCurrent(ev,ch);
	  int spark_time = runQ.getLastCurrentTime(ev,ch);
	  std::cout<<"run: "<<runQ.runid()<<", ev: "<<ev<<", channel: "<<ch<<", spark: "<<spark<<", spark_value: "<<spark_value<<", spark_time: "<<spark_time<<std::endl;
	}
      }
    }
  }
  return 0;
  
}
