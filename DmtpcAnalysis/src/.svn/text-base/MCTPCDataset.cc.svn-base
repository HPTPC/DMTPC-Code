#include "MCTPCDataset.hh" 

#include <iostream>
#include "TFile.h"
#include "TTree.h"



dmtpc::analysis::MCTPCDataset::MCTPCDataset(const char * file) 
{
  f = 0; 
  tree = 0; 
  truthptr = new MCTruth;

  if (file) open(file); 
}

void dmtpc::analysis::MCTPCDataset::close()
{
  f->Close(); 
  delete f; 

}


bool dmtpc::analysis::MCTPCDataset::open(const char * file)
{
  if (f) delete f; 
  f = new TFile(file); 
  if (!f->IsOpen()) return false; 
  tree = (TTree*) f->Get("Simulation"); 
  if (!tree) return false; 

  tree->SetBranchAddress("truth",&truthptr); 
  tree->GetEntry(0); 
  return true; 
}



TH2 * dmtpc::analysis::MCTPCDataset::getTruthImage(int i) 
{
  return (TH2*) truthptr->truth_images[i]; 
}


dmtpc::analysis::MCTruth * dmtpc::analysis::MCTPCDataset::getEvent(int i) 
{
  tree->GetEntry(i); 
  return truthptr; 
}


dmtpc::analysis::MCTPCDataset::~MCTPCDataset() 
{
  close(); 
  delete truthptr; 
}
