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

  const char * out = nargs > 1 ? args[1] : "m3wimp_out/" ; 
  int ntracks = nargs > 2 ? atoi(args[2]) : 1000; 
  double wimpmass = nargs > 3 ? atof(args[3]) : 100; 
  int P = nargs > 4 ? atoi(args[4]) : 30; 
  TChain avch("avalanche"); 
  dmtpc::core::common::addFilesToChain(&avch, nargs > 5 ? args[5] :"avalanche_lists/3m_avalanches.txt"); 

  const char * retrim_data_dir = getenv("RETRIM_DATA_DIR") ? getenv("RETRIM_DATA_DIR") : "/net/zwicky/esata01/dmtpc/production/retrim_data";
  gStyle->SetOptStat(0); 
  gStyle->SetNumberContours(255); 
  TApplication app("app",0,0); 


  dmtpc::mc::mctpc::CachedMCAmpAvalanche::Params sap; 
  sap.gain = 150e3; 
  sap.acceptance = 5e-3; 
  dmtpc::mc::mctpc::CachedMCAmpAvalanche a(&avch,"",sap); 

  const dmtpc::mc::retrim::TableReader * tables[2]; 
  tables[0] = new dmtpc::mc::retrim::TableReader(TString::Format("%s/srim/f_in_cf4_%dtorr.txt",retrim_data_dir, P)); 
  tables[1] = new dmtpc::mc::retrim::TableReader(TString::Format("%s/srim/c_in_cf4_%dtorr.txt",retrim_data_dir, P)); 

  dmtpc::mc::retrim::SimpleIonizationModel m; 

  dmtpc::mc::wimpspectrum::WimpGen wimpgen; 
  wimpgen.setWimpMass(wimpmass); 
  dmtpc::mc::mctpc::MultiTRIMGenerator * g = dmtpc::mc::mctpc::MultiTRIMGenerator::makeFromDir(TString::Format("%s/f/%dtorr/coll/",retrim_data_dir,P), &m, 2, tables, 0.98,9,P); 

  
  TTimeStamp t0(2015,1,1,0,0,0); 
  TTimeStamp t1(2016,1,1,0,0,0); 

  dmtpc::mc::mctpc::UniformScalarDistribution tdist(t0.AsDouble(),t1.AsDouble()); 
  dmtpc::mc::mctpc::FixedVectorDistribution dirdist(0,1,0); 
  dmtpc::mc::mctpc::UniformBox posdist(-50,50,-50,50,0,300); 

  dmtpc::mc::mctpc::WIMPDistribution pdist(&wimpgen, &dirdist, &tdist,&posdist); 
  pdist.setELimits(25,250); 
  g->setParticleDistribution(&pdist); 

  dmtpc::mc::mctpc::SimpleDrift d (0,300,0.0107, 0.0107, 0.1); 
  dmtpc::mc::mctpc::SimpleCamera cam(764,764,4,"m3cam"); 

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
