#include "RunQuality.hh"

#include "TString.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>

int main(int argc, char ** argv){
  
  //no user input
  int runId=1062026;
  //int runId=1038009;
  //  int nQuad = 4;//defaulted to 5 (4sh + 1sh) no error (currently) for channel = 5
  //TString db_dir = Form("/Calibration/2016/03/Quality/");
  TString db_dir = "/scratch3/darkmatter/dmtpc/Calibration/2016/03/Quality/final/";

  if(argc>1) runId=atoi(argv[1]);

  //  TString dbName=db_dir+"1061001-1064015.db";
  TString dbName = db_dir + "2016_von_03.db";
  if(argc>2) dbName=db_dir+argv[2]; 

  //---setup RunQuality  
  dmtpc::Quality::RunQuality runQ = dmtpc::Quality::RunQuality(runId,dbName);
  int noInfo= runQ.noInfo();
  printf("extract spark list for run=%d from %s, noInfo=%d, wiat ...\n",runQ.runid(),dbName.Data(),noInfo);  
  if(noInfo)return -1;

  //  runQ.setNChannels(nQuad);

  //---get only spark events
  std::vector< std::vector< int > > sparkEvents = runQ.getSparkTaggedEvents();
  
  //---print out results
  for(unsigned int ch = 0; ch < sparkEvents.size(); ch++){
    printf("run=%d quad=%d has %d sparks: ",runId, ch,sparkEvents.at(ch).size());
    for(unsigned int ev = 0; ev < sparkEvents.at(ch).size(); ev++)
      printf("%d, ",sparkEvents.at(ch).at(ev));          
    printf("\n");
  }
}
