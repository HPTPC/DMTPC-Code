#include "CollisionReader.hh"
#include "TableReader.hh"
#include "DatasetReadout.hh"
#include "TStyle.h"
#include "ScalarDistribution.hh"
#include "ParticleDistribution.hh"
#include "TrackMaker.hh"
#include "TRIMGenerator.hh"
#include "SimpleDrift.hh"
#include "SimpleAvalanche.hh"
#include "SimpleCamera.hh"
#include "IonizationModel.hh"
#include "FileReadout.hh"
#include "Driver.hh"
#include "InteractiveReadout.hh"
#include "TApplication.h"
#include "MultiTRIMGenerator.hh"

int main(int nargs, char** args) 
{

  const char * out = nargs > 1 ? args[1] : "4sh_simple/"; 
  int P = nargs >2 ? atoi(args[2]) : 60; 
  int ntracks = nargs > 3 ? atoi(args[4]) : 10000; 
  gStyle->SetOptStat(0); 
  gStyle->SetNumberContours(255); 
  TApplication app("app",0,0); 

  const dmtpc::mc::retrim::TableReader * tables_f[2]; 
//  const dmtpc::mc::retrim::TableReader * tables_c[2]; 
  // I don't remmeber if the order was important, but if it was, then the primary ion should go first
  tables_f[0] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/f_in_cf4_%dtorr.txt", P)); 
  tables_f[1] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/c_in_cf4_%dtorr.txt", P)); 

  dmtpc::mc::retrim::SimpleIonizationModel m; 

  dmtpc::physics::endf::CrossSection cs_F("ENDF_CS_n_on_19F","../../DmtpcPhysics/tables/"); 
  cs_F.readFile(); 


  dmtpc::mc::mctpc::MultiTRIMGenerator * g = dmtpc::mc::mctpc::MultiTRIMGenerator::makeFromDir(TString::Format("../retrim/data/f/%dtorr/coll/",P), &m, 2, tables_f, 0.98,9 ); 



  dmtpc::mc::mctpc::SimpleAvalanche::Params sap; 

  dmtpc::mc::mctpc::SimpleDrift d (0,300,0.0056, 0.0056, 0.1); 
  dmtpc::mc::mctpc::SimpleCamera cam(1024,1024,4,"4shcam"); 

  double geometric_acceptance = 7.4e-4; 
  double cathode_transparency = 0.9; 
  double window_transparency = 0.75; 
  double fudge_factor = 0.25; 
  sap.acceptance = 1e-3;
  double acceptance =  cathode_transparency * window_transparency * fudge_factor * geometric_acceptance / sap.acceptance; 

  dmtpc::mc::mctpc::SimpleAvalanche a(sap); 
  cam.setAcceptance(acceptance); 


  dmtpc::mc::mctpc::FixedVectorDistribution dirdist(1,0,0); 
  dmtpc::mc::mctpc::UniformCylinder posdist(0,0,10,0,270); 
  dmtpc::mc::mctpc::UniformScalarDistribution edist(0,250); 
  dmtpc::mc::mctpc::ConstantScalarDistribution tdist(0); 
  dmtpc::mc::mctpc::IndependentDistribution pdist (&posdist, &dirdist, &edist, &tdist); 
  //make track
  g->setCurrentTrack(0); 
  g->setParticleDistribution(&pdist); 

  dmtpc::mc::mctpc::Driver driver(ntracks); 
  dmtpc::mc::mctpc::DatasetReadout r(out); 

  r.defineCamera("4shcam","4shcam_bias"); 

  driver.plug(g,&d); 
  driver.plug(&d,&a); 
  driver.plug(&a,&cam); 
  driver.plug(&d,&r); 
  driver.plug(&a,&r); 
  driver.plug(&cam,&r); 

  driver.run(ntracks); 

}
