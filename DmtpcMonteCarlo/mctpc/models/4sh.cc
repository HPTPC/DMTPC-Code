#include "CollisionReader.hh"
#include "TableReader.hh"
#include "DatasetReadout.hh"
#include "ScalarDistribution.hh"
#include "ParticleDistribution.hh"
#include "VectorDistribution.hh"
#include "TrackMaker.hh"
#include "MultiTRIMGenerator.hh"
#include "SimpleDrift.hh"
#include "SimpleAvalanche.hh"
#include "SimpleCamera.hh"
#include "IonizationModel.hh"
#include "FileReadout.hh"
#include "Driver.hh"
#include "InteractiveReadout.hh"
#include "TApplication.h"

int main(int nargs, char** args) 
{

  const char * out = nargs > 1 ? args[1] : "."; 
//  float energy = nargs > 2 ? atof(args[2]) : 200.00; 
  int ntracks = nargs > 2 ? atoi(args[2]) : 10; 
  int P = nargs > 3 ? atoi(args[3]) : 60; 

  const dmtpc::mc::retrim::TableReader * tables[2]; 
  tables[0] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/f_in_cf4_%dtorr.txt", P)); 
  tables[1] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/c_in_cf4_%dtorr.txt", P)); 

  dmtpc::mc::retrim::SimpleIonizationModel m; 

  dmtpc::mc::mctpc::MultiTRIMGenerator * g = dmtpc::mc::mctpc::MultiTRIMGenerator::makeFromDir(TString::Format("../retrim/data/f/%dtorr/coll/bin/",P), &m, 2, tables, 0.98 ); 

  dmtpc::mc::mctpc::UniformScalarDistribution Edist(10,250); 
  dmtpc::mc::mctpc::UniformScalarDistribution tdist(0,100); 
  dmtpc::mc::mctpc::IsotropicVectorDistribution dirdist; 
  dirdist.setSeed(0); 
  dmtpc::mc::mctpc::UniformBox posdist(-80,80,-80,80,0,300); 
  posdist.setSeed(0); 

  dmtpc::mc::mctpc::IndependentDistribution pdist(&posdist, &dirdist, &Edist, &tdist); 

  g->setParticleDistribution(&pdist); 


  dmtpc::mc::mctpc::SimpleAvalanche::Params sap; 

  double geometric_acceptance = 7.4e-4; 

  dmtpc::mc::mctpc::SimpleDrift d (0,300,0.0056, 0.0056, 0.1); 
  sap.acceptance = 2e-3; 
  dmtpc::mc::mctpc::SimpleAvalanche a(sap); 
  dmtpc::mc::mctpc::SimpleCamera cam(1024,1024,4,"4shcam"); 

  double cathode_transparency = 0.9; 
  double window_transparency = 0.75; 
  double fudge_factor = 0.3; 
  double acceptance = cathode_transparency * window_transparency * fudge_factor * geometric_acceptance / sap.acceptance; 

  cam.setAcceptance(acceptance); 

  //make track
  g->setCurrentTrack(0); 

  dmtpc::mc::mctpc::Driver driver(500); 
  dmtpc::mc::mctpc::DatasetReadout r(out); 

  r.defineCamera("4shcam","4shcam_bias"); 

  driver.plug(g,&d); 
  driver.plug(&d,&a); 
  driver.plug(&a,&cam); 
  driver.plug(&cam,&r); 
  driver.plug(&a,&r); 
  driver.plug(&d,&r); 

  driver.run(ntracks); 

  delete g; 

}
