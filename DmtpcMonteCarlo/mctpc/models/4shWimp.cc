#include "CollisionReader.hh"
#include "TableReader.hh"
#include "DatasetReadout.hh"
#include "TStyle.h"
#include "WimpGen.hh"
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
  int ntracks = nargs > 2 ? atoi(args[2]) : 10; 
  double Emin = nargs > 3 ? atof(args[3]) : 0; 
  double wimpmass = nargs > 4 ? atof(args[4]) : 100; 
  int P = nargs > 5 ? atoi(args[5]) : 60; 
  gStyle->SetOptStat(0); 
  gStyle->SetNumberContours(255); 
  TApplication app("app",0,0); 

  const dmtpc::mc::retrim::TableReader * tables[2]; 
  tables[0] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/f_in_cf4_%dtorr.txt", P)); 
  tables[1] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/c_in_cf4_%dtorr.txt", P)); 

  dmtpc::mc::retrim::SimpleIonizationModel m; 

  dmtpc::mc::wimpspectrum::WimpGen wimpgen; 
  wimpgen.setWimpMass(wimpmass); 
  dmtpc::mc::mctpc::MultiTRIMGenerator * g = dmtpc::mc::mctpc::MultiTRIMGenerator::makeFromDir(TString::Format("../retrim/data/f/%dtorr/coll/",P), &m, 2, tables, 0.98 ); 

  
  TTimeStamp t0(2015,1,1,0,0,0); 
  TTimeStamp t1(2016,1,1,0,0,0); 

  dmtpc::mc::mctpc::UniformScalarDistribution tdist(t0.AsDouble(),t1.AsDouble()); 
  dmtpc::mc::mctpc::FixedVectorDistribution dirdist(0,1,0); 
  dmtpc::mc::mctpc::UniformBox posdist(-80,80,-80,80,0,300); 

  dmtpc::mc::mctpc::WIMPDistribution pdist(&wimpgen, &dirdist, &tdist,&posdist); 
  pdist.setELimits(Emin,250); 
  g->setParticleDistribution(&pdist); 
  dmtpc::mc::mctpc::SimpleAvalanche::Params sap; 

  dmtpc::mc::mctpc::SimpleDrift d (0,300,0.0056, 0.0056, 0.1); 
  dmtpc::mc::mctpc::SimpleAvalanche a(sap); 
  dmtpc::mc::mctpc::SimpleCamera cam(1024,1024,4,"4shcam"); 

  double geometric_acceptance = 7.4e-4; 
  double cathode_transparency = 0.9; 
  double window_transparency = 0.75; 
  double fudge_factor = 0.3; 
  double acceptance = geometric_acceptance * cathode_transparency * window_transparency * fudge_factor; 

  cam.setAcceptance(acceptance); 

  //make track
  g->setCurrentTrack(0); 

  dmtpc::mc::mctpc::Driver driver(2000); 
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
