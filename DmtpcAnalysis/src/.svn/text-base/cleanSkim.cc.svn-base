#include "CleanSkimFunctions.hh" 
#include "TString.h" 
#include "CleanSkimConfig.hh"
#include "Dataset.hh"
#include "Event.hh"

using namespace std;
using namespace dmtpc::analysis::cleanskim; 
using namespace dmtpc::analysis;

/* Main function */ 

int main(int argn, const char ** argv)
{
   int return_val = 0; 

   Setup setup; 
   if(Functions::parse_args(argn, argv, &setup)) return -1; 
   
   dmtpc::analysis::cleanskim::Config * conf = new dmtpc::analysis::cleanskim::Config(setup.cfgfile); 
    
   /* Loop through input files */
   for (unsigned f = 0; f < setup.files.size(); f++)
   {
                 
      /* Create DMTPC Database and draw out tree */
      dmtpc::core::Dataset d; 
      d.open(TString::Format("%s/%s",setup.datadir, setup.files[f])); 
        
      //creat RunQuality checker
      //      TString db_dir = "/scratch3/darkmatter/dmtpc/Calibraion/2016/03/Quality/final";
      //      TStrin db_name
        

	      //Extract run number from file 
      d.getEvent(0); 
      int runnum = d.event()->run(); 
      TString db = Form("%s%s",setup.dbdir,setup.dbname);
      dmtpc::Quality::RunQuality runQ = dmtpc::Quality::RunQuality(runnum,db);
      if(runQ.noInfo()){
	  std::cout<<"no spark db info for this run"<<std::endl;
	}      
      TString outfilename = TString(setup.files[f]).ReplaceAll(".raw.root","skim.root"); 
      SkimDataset sd; 
      sd.newRootFile(TString::Format("%s/%s",setup.outdir,outfilename.Data())); 
      
      for (unsigned int nprocess = 0; nprocess < conf->getStringList("cluster_find_algo")->size(); nprocess++)
      {
        return_val += Functions::cleanSkim(d,runnum,sd,true,conf,nprocess,runQ); 
      }
   }
   return return_val;
}


