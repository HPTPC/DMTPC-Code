#include "ParticleDistribution.hh"
#include "ScalarDistribution.hh"
#include "ToyGenerator.hh"
#include "TRandom3.h"


#include "TFile.h"
#include "TTree.h" 


int main(int nargs, char** args) 
{

  const char * out = nargs > 1 ? args[1] : "distribution.root"; 
  int ntracks = nargs > 2 ? atoi(args[2]) : 100000; 
 
  dmtpc::mc::mctpc::UniformBox posdist(-50,50,-50,50,0,5); 
//  TFile ambehist("AmBe_energy_spectrum_from_Igal.root"); 
//  TH1 * ambe_edist = (TH1*) ambehist.Get("EnergyDis"); 
//  ambe_edist->GetXaxis()->SetLimits(0,ambe_edist->GetXaxis()->GetXmax() * 1e3); //rescale to keV
//  dmtpc::mc::mctpc::HistogramScalarDistribution edist(ambe_edist); 
  dmtpc::mc::mctpc::ConstantScalarDistribution edist(2500); 
  TVector3 source_position(25.4 * 10, 0, 8); 
  TVector3 source_dir(-1,0,0); 

  dmtpc::physics::endf::CrossSection cs("ENDF_CS_n_on_19F","../../DmtpcPhysics/tables/"); 
  cs.readFile(); 
  dmtpc::physics::endf::CrossSection cs2("ENDF_CS_n_on_12C","../../DmtpcPhysics/tables/"); 
  cs2.readFile(); 

  printf("Elims: [%f,%f]\n", cs.getMinE(), cs.getMaxE()); 
  dmtpc::physics::endf::AngularDistribution dcs(200);
  dcs.readFile("ENDF_DCS_n_on_19F","../../DmtpcPhysics/tables/"); 

  dmtpc::physics::endf::AngularDistribution dcs2(200);
  dcs2.readFile("ENDF_DCS_n_on_19F","../../DmtpcPhysics/tables/"); 

  dmtpc::mc::mctpc::CollimatedNeutronSourceDistribution pdist(&source_position, &source_dir, &posdist, 
                                                              &edist, &cs, &dcs, 180,100); 

  dmtpc::mc::mctpc::CollimatedNeutronSourceDistribution pdist2(&source_position, &source_dir, &posdist, 
                                                              &edist, &cs2, &dcs2, 180,100); 
  pdist2.setRecoilMass(dmtpc::mc::mctpc::MASS_12C); 

  pdist.setRecoilELimits(0,2500); 
  dmtpc::mc::mctpc::ToyGenerator g(&pdist,9); 

  pdist2.setRecoilELimits(0,2500); 
  dmtpc::mc::mctpc::ToyGenerator g2(&pdist2,6); 

  dmtpc::analysis::MCTruth *truth = new dmtpc::analysis::MCTruth; 
  TFile fout(out,"RECREATE");

  TTree * tree = new TTree("Simulation","Simulation"); 

  double weight = 4*pdist.getXSIntegral(); 
  double weight2 = pdist2.getXSIntegral(); 
  
  double frac = weight / (weight + weight2); 

  TRandom3 rand; 


  tree->Branch("truth",&truth);

  for (int i = 0; i < ntracks; i++)
  {
    

    printf("%d using ",i); 
    if (rand.Uniform() < frac)
    {
      g.generate(truth); 
      printf("F\n"); 
    }
    else
    {
      g2.generate(truth); 
      printf("C\n"); 
    }

    tree->Fill() ; 
  }


  tree->Write(); 
  fout.Close(); 


}
