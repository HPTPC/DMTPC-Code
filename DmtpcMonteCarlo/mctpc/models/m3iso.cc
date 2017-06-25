#include "CollisionReader.hh"
#include "TableReader.hh"
#include "TChain.h"
#include "CommonTools.hh"
#include "DatasetReadout.hh"
#include "TStyle.h"
#include "WimpGen.hh"
#include "ScalarDistribution.hh"
#include "ParticleDistribution.hh"
#include "VectorDistribution.hh"
#include "TrackMaker.hh"
#include "MultiTRIMGenerator.hh"
#include "SimpleDrift.hh"
#include "CachedMCAmpAvalanche.hh"
#include "SimpleCamera.hh"
#include "IonizationModel.hh"
#include "FileReadout.hh"
#include "Driver.hh"
#include "InteractiveReadout.hh"
#include "TApplication.h"

int main(int nargs, char** args) 
{

  const char * out = nargs > 1 ? args[1] : "m3iso/" ; 
  int ntracks = nargs > 2 ? atoi(args[2]) : 1000; 
  int P = nargs > 3 ? atoi(args[3]) : 30; 
  gStyle->SetOptStat(0); 
  gStyle->SetNumberContours(255); 
  TApplication app("app",0,0); 

  TChain avch("avalanche"); 
  dmtpc::core::common::addFilesToChain(&avch, nargs > 5 ? args[5] :"avalanche_lists/3m_avalanches.txt"); 

  dmtpc::mc::mctpc::CachedMCAmpAvalanche::Params sap; 
  sap.gain = 150e3; 
  sap.acceptance = 1e-3; 
  dmtpc::mc::mctpc::CachedMCAmpAvalanche a(&avch,"",sap); 

  const dmtpc::mc::retrim::TableReader * tables[2]; 
  tables[0] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/f_in_cf4_%dtorr.txt", P)); 
  tables[1] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/c_in_cf4_%dtorr.txt", P)); 

  dmtpc::mc::retrim::SimpleIonizationModel m; 


  dmtpc::mc::mctpc::MultiTRIMGenerator * g = dmtpc::mc::mctpc::MultiTRIMGenerator::makeFromDir(TString::Format("../retrim/data/f/%dtorr/coll/",P), &m, 2, tables, 0.98,9,P); 

  

  dmtpc::mc::mctpc::UniformScalarDistribution Edist(10,250); 
  dmtpc::mc::mctpc::UniformScalarDistribution tdist(0,100); 
  dmtpc::mc::mctpc::IsotropicVectorDistribution dirdist; 
  dirdist.setSeed(0); 
  dmtpc::mc::mctpc::UniformBox posdist(-80,80,-80,80,0,300); 
  posdist.setSeed(0); 




  dmtpc::mc::mctpc::IndependentDistribution pdist(&posdist, &dirdist, &Edist, &tdist); 
  g->setParticleDistribution(&pdist); 



  dmtpc::mc::mctpc::SimpleDrift d (0,300,0.0056, 0.0056, 0.1); 
  dmtpc::mc::mctpc::SimpleCamera cam(1023,1023,3,"m3cam"); 


  cam.setQE(0.5); 
  cam.setScale(0.187); 

  double convgain = 1.5; // ??? 
  cam.setGain(convgain); 
  cam.setNoise(10. / convgain); 

  double geometric_acceptance = 1.6e-4; 
  double cathode_transparency = 0.9; 
  double window_transparency = 0.9; 
  double fudge_factor = 0.5; 
  double acceptance = geometric_acceptance * cathode_transparency * window_transparency * fudge_factor / sap.acceptance; 

  cam.setAcceptance(acceptance); 

  //make track
  g->setCurrentTrack(0); 

  dmtpc::mc::mctpc::Driver driver(ntracks); 
  dmtpc::mc::mctpc::DatasetReadout r(out); 

  r.defineCamera("m3cam","m3cam_bias"); 

  driver.plug(g,&d); 
  driver.plug(&d,&a); 
  driver.plug(&a,&cam); 
  driver.plug(&cam,&r); 
  driver.plug(&a,&r); 
  driver.plug(&d,&r); 

  driver.run(ntracks); 

  delete g; 

}
