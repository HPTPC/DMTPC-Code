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
#include "TApplication.h"

int main(int nargs, char** args) 
{

  const char * out = nargs > 1 ? args[1] : "ambe_simple"; 
//  float energy = nargs > 2 ? atof(args[2]) : 200.00; 
  int ntracks = nargs > 2 ? atoi(args[2]) : 1000; 
  int P = 60; 
  gStyle->SetOptStat(0); 
  gStyle->SetNumberContours(255); 
  TApplication app("app",0,0); 

  const dmtpc::mc::retrim::TableReader * tables_f[2]; 
  const dmtpc::mc::retrim::TableReader * tables_c[2]; 
  // I don't remmeber if the order was important, but if it was, then the primary ion should go first
  tables_f[0] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/f_in_cf4_%dtorr.txt", P)); 
  tables_f[1] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/c_in_cf4_%dtorr.txt", P)); 
  tables_c[1] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/f_in_cf4_%dtorr.txt", P));  
  tables_c[0] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/c_in_cf4_%dtorr.txt", P)); 


  dmtpc::mc::retrim::SimpleIonizationModel m; 

  dmtpc::physics::endf::CrossSection cs_F("ENDF_CS_n_on_19F","../../DmtpcPhysics/tables/"); 
  dmtpc::physics::endf::CrossSection cs_C("ENDF_CS_n_on_12C","../../DmtpcPhysics/tables/"); 
  cs_F.readFile(); 
  cs_C.readFile(); 


  dmtpc::mc::mctpc::MultiTRIMGenerator * g_F = dmtpc::mc::mctpc::MultiTRIMGenerator::makeFromDir(TString::Format("../retrim/data/f/%dtorr/coll/bin/",P), &m, 2, tables_f, 0.98,9 ); 
  dmtpc::mc::mctpc::MultiTRIMGenerator * g_C = dmtpc::mc::mctpc::MultiTRIMGenerator::makeFromDir(TString::Format("../retrim/data/c/%dtorr/coll/",P), &m, 2, tables_c, 0.98,6 ); 

  dmtpc::mc::mctpc::UniformBox posdist(-160,0,-160,0,1,270); 

  //dmtpc::mc::mctpc::AmBeEnergyDistribution edist; 
  
  TFile ambehist("AmBe_energy_spectrum_from_Igal.root"); 
  TH1 * ambe_edist = (TH1*) ambehist.Get("EnergyDis"); 
  ambe_edist->GetXaxis()->SetLimits(0,ambe_edist->GetXaxis()->GetXmax() * 1e3); //rescale

  dmtpc::mc::mctpc::HistogramScalarDistribution edist(ambe_edist); 
 

  TVector3 source_position(25.4 * 40, 0, 8); 
  TVector3 source_dir(-1,0,0); 


  dmtpc::physics::endf::AngularDistribution dcs_F(100);
  dcs_F.readFile("ENDF_DCS_n_on_19F","../../DmtpcPhysics/tables/"); 
  dmtpc::physics::endf::AngularDistribution dcs_C(100);
  dcs_C.readFile("ENDF_DCS_n_on_12C","../../DmtpcPhysics/tables/"); 

  dmtpc::mc::mctpc::CollimatedNeutronSourceDistribution pdist_F(&source_position, &source_dir, &posdist, 
                                                              &edist, &cs_F, &dcs_F, 40,30.27 * 25.4); 
  dmtpc::mc::mctpc::CollimatedNeutronSourceDistribution pdist_C(&source_position, &source_dir, &posdist, 
                                                              &edist, &cs_C, &dcs_C, 40,30.27 * 25.4); 
  pdist_F.setRecoilELimits(40,250); 
  pdist_C.setRecoilELimits(40,250); 

  g_F->setParticleDistribution(&pdist_F); 
  g_C->setParticleDistribution(&pdist_C); 


   dmtpc::mc::mctpc::SimpleAvalanche::Params sap; 



  double geometric_acceptance = 7.4e-4; 
  double cathode_transparency = 0.9; 
  double window_transparency = 0.75; 
  double fudge_factor = 0.25 * 1.13; 

  sap.acceptance = 1e-3; 
  dmtpc::mc::mctpc::SimpleDrift d (0,267,0.0063, 0.0063, 0.1); 
  dmtpc::mc::mctpc::SimpleAvalanche a(sap); 
  dmtpc::mc::mctpc::SimpleCamera cam(1024,1024,4,"110121"); 
  TFile gmfile("data/4sh_gainmaps_ambe.root"); 

  double acceptance =  cathode_transparency * window_transparency * fudge_factor * geometric_acceptance / sap.acceptance; 

  cam.setAcceptance(acceptance * 1.3/1.4); 
  cam.setLocation(-81,-81); 
  cam.setNoise(10.8); 
  cam.setRotation(0); 
  cam.setGain(1.4); 
//  cam.setGainMap((TH2F*) gmfile.Get("gm_110121")); 



  //make track
  g_F->setCurrentTrack(0); 
  g_C->setCurrentTrack(0); 

  dmtpc::mc::mctpc::Driver driver(2500); 
  dmtpc::mc::mctpc::DatasetReadout r(out); 

  r.defineCamera("110121","110121_bias"); 

  double F_weight = pdist_F.getXSIntegral() * 4; 
  double C_weight = pdist_C.getXSIntegral(); 
  printf("weights: F: %f, C%f\n",F_weight, C_weight); 

  driver.plug(g_F,&d,F_weight); 
  driver.plug(g_C,&d,C_weight); 
  driver.plug(&d,&a); 
  driver.plug(&a,&cam); 
  driver.plug(&cam,&r); 
  driver.plug(&a,&r); 
  driver.plug(&d,&r); 

  driver.run(ntracks); 

  delete g_F; 
  delete g_C; 

}
