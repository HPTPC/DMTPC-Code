#include "SkimDataset.hh"
#include "SkimEvent.hh"

#include <TFile.h>
#include <TH2.h>

#include <cstdlib>


int main(int argc, char ** argv){

  
  dmtpc::analysis::SkimDataset d;
  d.openRootFile(Form("/scratch1/darkmatter/dmtpc/production/skimout/skim/m3/dmtpc_m3_%05dskim.root",atoi(argv[1])));
  d.getEvent(0);
  int ncam = d.event()->ncamera();

  d.loadBiasFrames(true,Form("/scratch1/darkmatter/dmtpc/production/skimout/skim/m3/dmtpc_m3_%05dbias.root",atoi(argv[1])));

  TFile save(Form("output/skim_bframes_%d.root",atoi(argv[1])),"recreate");
  
  for(int c = 0; c < ncam; c++){
    TH2I * hist = (TH2I*)d.getBiasFrame(c)->Clone();
    hist->SetName(Form("bframe_%d",c));
  }

  save.Write();
  save.Close();
}
