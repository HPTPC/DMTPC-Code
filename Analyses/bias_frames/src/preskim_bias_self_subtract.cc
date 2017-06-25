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

  TFile save(Form("output/selfsub_bframes_%d.root",runnum.Atoi()), "recreate");

  std::cout<<"btree entries: "<<btree->GetEntries()<<std::endl;

  //loop over bias frames
  for(int i = 0; i < btree->GetEntries(); i += 2){
    btree->GetEntry(i);

    std::cout<<"biasPtr size: "<<biasPtr->size()<<std::endl;
    //loop over cameras
    for(int j = 0; j < biasPtr->size(); j++){
      std::cout<<"hist type: "<<biasPtr->at(j)<<std::endl;
      //get first bias frame
      TH2I * b0 = const_cast<TH2I*>((TH2I*)biasPtr->at(j)->getHist()->Clone("b0"));
      //get second bias frame
      btree->GetEntry(i+1);
      TH2I * b1 = const_cast<TH2I*>((TH2I*)biasPtr->at(j)->getHist()->Clone("b1"));
      //subtract first from second
      b1->Add(b0,-1);
      b1->SetNameTitle(Form("bframe_%d_%d",j,i),Form("Bias Frame %d, Cam %d",i,j));
      //get first bias frame for next cam
      btree->GetEntry(i);
    }
  }
  save.Write();
  save.Close();
}
