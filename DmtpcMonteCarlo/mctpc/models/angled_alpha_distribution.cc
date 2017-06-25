#include "ParticleDistribution.hh"
#include "CollisionReader.hh" 
#include "ScalarDistribution.hh"
#include "ToyGenerator.hh"
#include "TRandom3.h"


#include "TFile.h"
#include "TTree.h" 

double quenching(double E) 
{

  double p0 = -0.31; 
  double p1 = 32.6347; 

  return (p0 + p1*E)/(1+p1*E); 
}


int main(int nargs, char** args) 
{

  const char * out = nargs > 1 ? args[1] : "aa_distribution.root"; 
  int ntracks = nargs > 2 ? atoi(args[2]) : 10000; 
  const char * in = nargs > 3 ? args[3] : "alpha_999999_4500keV.coll"; 


  dmtpc::mc::retrim::CollisionReader coll(in); 


  TVector3 source_dir (0.97009,0.091188,-0.22495); 
  TVector3 source_position(-185.9,38.1,300.1); 

  dmtpc::mc::mctpc::AngledAlphaDistribution pdist(&coll, &source_position, &source_dir); 
  pdist.setELimits(40,450,0); 

  pdist.setZPlaneHeight(267) ; 

  dmtpc::mc::mctpc::ToyGenerator g(&pdist,2); 
  g.setQuenching(&quenching); 
  dmtpc::analysis::MCTruth *truth = new dmtpc::analysis::MCTruth; 
  TFile fout(out,"RECREATE");

  TTree * tree = new TTree("Simulation","Simulation"); 


  TRandom3 rand; 


  tree->Branch("truth",&truth);

  for (int i = 0; i < ntracks; i++)
  {
    g.generate(truth); 
    tree->Fill() ; 
  }


  tree->Write(); 
  fout.Close(); 


}
