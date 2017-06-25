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
#include "SimpleAvalanche.hh"
#include "SimpleCamera.hh"
#include "IonizationModel.hh"
#include "FileReadout.hh"
#include "Driver.hh"
#include "InteractiveReadout.hh"
#include "TApplication.h"

int main(int nargs, char** args) 
{

  const char * out = nargs > 1 ? args[1] : "angled_alpha_out"; 
//  float energy = nargs > 2 ? atof(args[2]) : 200.00; 
  int ntracks = nargs > 2 ? atoi(args[2]) : 1000; 
  const char * in = nargs > 3 ? args[3] : "alpha_999999_4500keV.coll"; 
  int P = 60;


  const char * ion = "he"; 
  dmtpc::mc::retrim::CollisionReader coll(in); 

  TVector3 source_dir (0.97009,0.091188,-0.22495); 
  TVector3 source_position(-185.9,3.1,300.1); 
  
  dmtpc::mc::mctpc::AngledAlphaDistribution pdist(&coll, &source_position, &source_dir); 
  pdist.setZPlaneHeight(267) ; 
 

  const dmtpc::mc::retrim::TableReader * tables[3]; 
  tables[1] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/f_in_cf4_%dtorr.txt", P)); 
  tables[2] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/c_in_cf4_%dtorr.txt", P)); 
  tables[0] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/he_in_cf4_%dtorr.txt", P)); 

  dmtpc::mc::retrim::SimpleIonizationModel m;
  dmtpc::mc::mctpc::MultiTRIMGenerator * g =
  dmtpc::mc::mctpc::MultiTRIMGenerator::makeFromDir(TString::Format("../retrim/data/%s/%dtorr/coll/",ion,P),
  &m, 3, tables, 1.00, 2 ,P); 



  pdist.setELimits(25,250,0); 
  g->setParticleDistribution(&pdist); 


  dmtpc::mc::mctpc::SimpleAvalanche::Params sap; 



  TFile gmfile("data/4sh_gainmaps_ambe.root"); 

  double geometric_acceptance = 7.4e-4; 
  sap.acceptance = geometric_acceptance; 
  double cathode_transparency = 0.9; 
  double window_transparency = 0.75; 
  double fudge_factor = 0.25* 16.4 / 18.6*0.97; 
  double acceptance =  cathode_transparency * window_transparency * fudge_factor; 

  dmtpc::mc::mctpc::SimpleDrift d (0,267,0.0063*1.04, 0.0063*1.04, 0.1); 
  dmtpc::mc::mctpc::SimpleAvalanche a(sap); 
  dmtpc::mc::mctpc::SimpleCamera cam(1024,1024,4,"A80333"); 
  cam.setAcceptance(acceptance); 
  cam.setNoise(11.1); 
//  cam.setBlur(8.); 
  cam.setGain(1.3); 
  cam.setGainMap((TH2F*) gmfile.Get("gm_A80333")); 


  //make track
  g->setCurrentTrack(0); 

  dmtpc::mc::mctpc::Driver driver(500); 
  dmtpc::mc::mctpc::DatasetReadout r(out); 

  r.defineCamera("A80333","A80333_bias"); 

  driver.plug(g,&d); 
  driver.plug(&d,&a); 
  driver.plug(&a,&cam); 
  driver.plug(&cam,&r); 
  driver.plug(&a,&r); 
  driver.plug(&d,&r); 

  driver.run(ntracks); 

  delete g; 


}

 
