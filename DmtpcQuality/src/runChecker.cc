#include "RunQuality.hh"

#include "TString.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

int main(int argc, char ** argv){

  //user input
  //argv[1] = runs.txt
  //argv[2] = tag (hardcoded for now)
 
  //--user modifications
  bool use_raw_file = false;//to use raw data file or pure database
  int nchan = 4;
  //--end user modifications

  //read in run numbers
  std::fstream infile(argv[1]);
  std::string line;
  std::vector<TString> run_nums;
  if(infile.is_open()){
    while(getline(infile, line)){
      std::cout<<"adding run "<<line<<std::endl;
      run_nums.push_back(TString(line));
    }
  }
  int nruns = run_nums.size();

  //-deprecated but kept for possible future use
  //  TString data_file = "/scratch1/darkmatter/dmtpc/data/m3/2016/02/m3_";//for RHUL
  TString data_file;
  TString tag;//=argv[2];
  if(use_raw_file){
    data_file = "/data/2016/02/raw/m3_";
    tag = "Michael";
    data_file += tag;
    data_file += "_R";
  }
  //------


  TString run_start = run_nums.at(0);
  TString run_end = run_nums.at(nruns-1);
  TString data_dir;
  if(use_raw_file){
    //    data_dir = "/scratch1/darkmatter/dmtpc/data/m3/2016/";
    data_dir = "/data/2016/02/raw/";
  }

  //  TString db_dir = "/scratch1/darkmatter/dmtpc/Calibration/2016/02/Quality/";//RHUL
  TString db_dir = "/Calibration/2016/02/Quality/";
  TString dbName = db_dir + run_start + "-" + run_end + ".db";

  dmtpc::Quality::RunQuality runQ;

  std::cout<<"run quality made"<<std::endl;

  for(int run = 0; run < nruns; run++){


    //setup RunQuality
    std::cout<<"instantiating RunQuality"<<std::endl;
    //---deprecated but kept for possible future use--//
    TString dataFileName;
    if(use_raw_file){
      dataFileName = data_dir + "m3_" + tag + "_R" + run_nums.at(run) + ".raw.root";    
      std:: cout<<"data file: "<<dataFileName<<std::endl;
      runQ = dmtpc::Quality::RunQuality(dataFileName,dbName);
    }
    else{
      std::cout<<"run number: "<<run_nums.at(run).Atoi()<<std::endl;
      runQ = dmtpc::Quality::RunQuality(run_nums.at(run),dbName);
    }
    runQ.setNChannels(nchan);
    std::cout<<"runQ setup"<<std::endl;
    //end setup RunQuality

    //get spark events
    std::vector< std::vector< int > > sparkEvents = runQ.getSparkTaggedEvents();

    for(unsigned int ch = 0; ch < sparkEvents.size(); ch++){
      for(unsigned int ev = 0; ev < sparkEvents.at(ch).size(); ev++){
	if(sparkEvents.at(ch).at(ev) == 1){
	  std::cout<<"Event "<<ev<<"::Channel: "<<ch<<"::spark status: "<<sparkEvents.at(ch).at(ev)<<std::endl;
	}
      }
    }
  }
  return 0;
  
}
