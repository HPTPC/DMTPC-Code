#include <Dataset.hh>
#include <Image.hh>


#include <vector>
#include <iostream>

#include <TFile.h>
#include <TH2.h>
#include <TTree.h>
#include <TCanvas.h>

int main(int argc, char ** argv){

  dmtpc::core::Dataset * d = new dmtpc::core::Dataset;
  d->open("/data/GainMap/dmtpc_m3_00170.root");

  std::vector<dmtpc::core::Image*> * biasPtr = 0;

  TTree * btree = d->getBiasTree();
  btree->SetBranchAddress("biasFrames",&biasPtr);

  btree->GetEntry(0);

  std::cout<<"biasPtr: "<<biasPtr->at(0)<<std::endl;

  TCanvas c1;
  c1.cd();
  //  biasPtr->at(0)->Draw();
  const TH2F * hist = (TH2F*)biasPtr->at(0)->getHist()->Clone("hist");
  
  TFile save("test.root", "recreate");
  save.cd();
  hist->Write();
  save.Write();
  save.Close();
}
