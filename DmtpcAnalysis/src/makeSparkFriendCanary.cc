#include "HVLog.hh"
#include "TChain.h" 
#include <cstdlib>


int main(int nargs, char ** args)
{



  int min = nargs > 1 ? atoi(args[1]) : 23; 
  int max = nargs > 2 ? atoi(args[2]) : 1300; 
  const char * outfile = nargs > 3 ? args[3]: "dd_sparktree.root"; 
  const char * indir = nargs > 4 ? args[4] :"/net/zwicky/esata04/dmtpc/data/canary/dd_run/"; 
  int min_for_fix = nargs > 5 ? atoi(args[5]) : 605; 


  dmtpc::analysis::HVLog hvl("dbdump.root", "caen_ch3_imon","caen_ch3_vmon","caen_ch3_set"); 

  TChain c("data"); 
  for (int i = min; i <= max; i++)
  {
    c.Add(TString::Format("%s/dmtpc_can_%05d.root",indir,i)); 
  }

  hvl.makeSparkFriend(&c,outfile,"spark",15,5,true,min_for_fix,0.25); 

  return 0; 

}
