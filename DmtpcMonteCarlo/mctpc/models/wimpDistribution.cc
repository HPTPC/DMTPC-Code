#include "ParticleDistribution.hh"
#include "ScalarDistribution.hh"
#include "ToyGenerator.hh"
#include "TRandom3.h"


#include "TFile.h"
#include "TTree.h" 


int main(int nargs, char** args) 
{

  int mass = nargs > 1 ? atoi(args[1]) : 100; 
  TString out = nargs > 2 ? args[2] : TString::Format("wimpdists/%d.root", mass); 
  int ntracks = nargs > 3 ? atoi(args[3]) : int(2e6); 
 
  dmtpc::mc::wimpspectrum::WimpGen wimpgen; 
  wimpgen.setWimpMass(mass); 

  wimpgen.setVE(230); 
  wimpgen.setV0(220); 
  wimpgen.setVesc(3000); 
 
  dmtpc::mc::mctpc::FixedVectorDistribution dirdist(0,0,1); 
  dmtpc::mc::mctpc::FixedVectorDistribution posdist(0,0,0); 
  dmtpc::mc::mctpc::ConstantScalarDistribution tdist(0); 

  dmtpc::mc::mctpc::WIMPDistribution pdist(&wimpgen, &dirdist, &tdist,&posdist); 
  pdist.setELimits(0,500); 


  dmtpc::mc::mctpc::ToyGenerator g(&pdist,9); 

  dmtpc::analysis::MCTruth *truth = new dmtpc::analysis::MCTruth; 
  TFile fout(out,"RECREATE");

  TTree * tree = new TTree("Simulation","Simulation"); 

  tree->Branch("truth",&truth);

  for (int i = 0; i < ntracks; i++)
  {
      if (i % 1000 == 0) printf("%d\n",i); 
      g.generate(truth); 
      tree->Fill() ; 
  }
 
  tree->Write(); 
  fout.Close(); 

  return 0; 
}

