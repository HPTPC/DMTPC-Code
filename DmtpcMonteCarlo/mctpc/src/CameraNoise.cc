#include "CameraNoise.hh" 
#include "TTree.h" 
#include "Event.hh"
#include "DmtpcEvent.hh"


dmtpc::mc::mctpc::CosMixCameraNoise::CosMixCameraNoise(const char * serial, int run, const char * det, const char * dir)
{

  f.Open(TString::Format("%s/dmtpc_%s_%05d.root", dir,det,run)); 

  compat = f.Get("dmtpc"); 



  if (compat) 
    tree = (TTree*) f.Get("dmtpc"); 
  else
    tree = (TTree*) f.Get(dmtpc::core::getDataTreeName()); 
   

  compat_event = 0; 
  event = 0; 
    
  tree->SetBranchAddress("event", compat ? &compat_event : event); 

  tree->GetEntry(0); 





}
