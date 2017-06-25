#include "CollisionReader.hh"
#include "TableReader.hh"
#include "DatasetReadout.hh"
#include "TStyle.h"
#include "ScalarDistribution.hh"
#include "ParticleDistribution.hh"
#include "VectorDistribution.hh"
#include "TrackMaker.hh"
#include "MultiTRIMGenerator.hh"
#include "SimpleDrift.hh"
#include "SimpleVUVAvalanche.hh"
#include "SimpleCamera.hh"
#include "IonizationModel.hh"
#include "FileReadout.hh"
#include "Driver.hh"
#include "TApplication.h"

int main(int nargs, char** args) 
{

  const char * out = nargs > 1 ? args[1] : "hide_ambe_simple"; 
//  float energy = nargs > 2 ? atof(args[2]) : 200.00; 
  int ntracks = nargs > 2 ? atoi(args[2]) : 1000; 
  int P = 30; 

  const dmtpc::mc::retrim::TableReader * tables_f[2]; 
  // I don't remmeber if the order was important, but if it was, then the primary ion should go first
  tables_f[0] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/f_in_cf4_%dtorr.txt", P)); 

  tables_f[1] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/c_in_cf4_%dtorr.txt", P)); 


  dmtpc::mc::retrim::SimpleIonizationModel m; 



  dmtpc::mc::mctpc::MultiTRIMGenerator * g= dmtpc::mc::mctpc::MultiTRIMGenerator::makeFromDir(TString::Format("../retrim/data/f/%dtorr/coll/",P), &m, 2, tables_f, 0.98,9,P ); 


  dmtpc::mc::mctpc::FixedVectorDistribution dirdist(1,0,0); 
  dmtpc::mc::mctpc::UniformCylinder posdist(0,0,2,0,20); 
  dmtpc::mc::mctpc::UniformScalarDistribution edist(0,250); 
  dmtpc::mc::mctpc::ConstantScalarDistribution tdist(0); 
  dmtpc::mc::mctpc::IndependentDistribution pdist (&posdist, &dirdist, &edist, &tdist); 
 


  g->setCurrentTrack(0); 
  g->setParticleDistribution(&pdist); 

  dmtpc::mc::mctpc::SimpleVUVAvalanche::Params sap; 

  double geometric_acceptance = 1e-3; 
  double cathode_transparency = 0.9; 
  double window_transparency = 0.75; 
  double fudge_factor = 0.25; 

  sap.gain=3e5;
  sap.gap = 1; 
  sap.vuv_photon_lambda=0.3; 
  sap.vuv_photon_p = 1e-5; 
  sap.avalanche_width=0.1;
  sap.acceptance = geometric_acceptance; 
  dmtpc::mc::mctpc::SimpleDrift d (0,20,0.005, 0.005, 0.1); 
  d.setXYCut(true,0,0,5); 
  dmtpc::mc::mctpc::SimpleVUVAvalanche a(sap); 
  dmtpc::mc::mctpc::SimpleCamera cam(512,512,4,"10435"); 

  double acceptance =  cathode_transparency * window_transparency * fudge_factor; 

  cam.setAcceptance(acceptance); 
  cam.setNoise(1); 
  cam.setQE(0.9); 
  cam.setBias(300); 
  cam.setScale(0.042); 


  //make track
  g->setCurrentTrack(0); 

  dmtpc::mc::mctpc::Driver driver(ntracks); 
  dmtpc::mc::mctpc::DatasetReadout r(out); 

  r.defineCamera("10435","10435_bias"); 


  driver.plug(g,&d); 
  driver.plug(&d,&a); 
  driver.plug(&a,&cam); 
  driver.plug(&a,&r); 
  driver.plug(&d,&r); 
  driver.plug(&cam,&r); 

  driver.run(ntracks); 


}
