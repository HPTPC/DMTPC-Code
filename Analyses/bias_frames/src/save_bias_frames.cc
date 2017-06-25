#include <Dataset.hh>
#include <Image.hh>


#include <vector>
#include <iostream>
#include <cstdlib>

#include <TFile.h>
#include <TH2.h>
#include <TTree.h>
#include <TCanvas.h>

#include <TString.h>

using namespace std;

int main(int argc, char ** argv){

  dmtpc::core::Dataset * d = new dmtpc::core::Dataset;
  TString runnum = argv[1];
  d->open(Form("/scratch1/darkmatter/dmtpc/data/m3/dmtpc_m3_%05d.root",atoi(argv[1])));
  int ncam = 4;

  //  int cam = atoi(argv[2]);
  //  int frame = atoi(argv[3]);

  std::vector<dmtpc::core::Image*> * biasPtr = 0;

  TTree * btree = d->getBiasTree();
  btree->SetBranchAddress("biasFrames",&biasPtr);

  TFile save(Form("output/bframes_%d.root",runnum.Atoi()), "recreate");

  std::cout<<"btree entries: "<<btree->GetEntries()<<std::endl;
  
  for(int i = 0; i < btree->GetEntries(); i++){
    btree->GetEntry(i);

    std::cout<<"biasPtr size: "<<biasPtr->size()<<std::endl;

    for(int j = 0; j < biasPtr->size(); j++){
      std::cout<<"hist type: "<<biasPtr->at(j)<<std::endl;
      TH2I * hist = const_cast<TH2I*>((TH2I*)biasPtr->at(j)->getHist()->Clone("hist"));
      hist->SetNameTitle(Form("bframe_%d_%d",j,i),Form("Bias Frame %d, Cam %d",i,j));
    }
  }
  save.Write();
  save.Close();
}
