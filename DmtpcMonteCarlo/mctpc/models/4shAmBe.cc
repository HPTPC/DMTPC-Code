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

  const char * out = nargs > 1 ? args[1] : "ambe_out/"; 
  int ntracks = nargs > 2 ? atoi(args[2]) : 1000; 
  int igal = nargs > 3 ? atoi(args[3]) : 0; 
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

  dmtpc::mc::mctpc::IsotropicVectorDistribution dirdist; 
  dmtpc::mc::mctpc::UniformCylinder posdist(0,0,150,0,270); 

  //dmtpc::mc::mctpc::AmBeEnergyDistribution edist; 
  
  TFile ambehist("AmBe_energy_spectrum_from_Igal.root"); 
  TH1 * ambe_edist = (TH1*) ambehist.Get(igal ? "EnergyDis2" : "EnergyDis"); 
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

  pdist_F.setRecoilELimits(50,250); 
  pdist_C.setRecoilELimits(50,250); 

  g_F->setParticleDistribution(&pdist_F); 
  g_C->setParticleDistribution(&pdist_C); 


  double geometric_acceptance = 7.4e-4; 
  double cathode_transparency = 0.9; 
  double window_transparency = 0.75; 
  double fudge_factor = 0.25 *0.97; 

  dmtpc::mc::mctpc::SimpleAvalanche::Params sap; 
  sap.acceptance = 1e-3; 
  double acceptance =  cathode_transparency * window_transparency * fudge_factor  * geometric_acceptance/ sap.acceptance; 
//  sap.xycut = true; 

  dmtpc::mc::mctpc::SimpleDrift d (0,267,0.0063, 0.0063, 0.1); 
  d.setXYCut(true,0,0,150); 
  dmtpc::mc::mctpc::SimpleAvalanche a(sap); 


  dmtpc::mc::mctpc::SimpleCamera cam0(1024,1024,4,"A80334"); 
  dmtpc::mc::mctpc::SimpleCamera cam1(1024,1024,4,"100534"); 
  dmtpc::mc::mctpc::SimpleCamera cam2(1024,1024,4,"110121"); 
  dmtpc::mc::mctpc::SimpleCamera cam3(1024,1024,4,"A80333"); 
  TFile gmfile("data/4sh_gainmaps_ambe.root"); 

  cam0.setAcceptance(acceptance *  10/18.6 * 1.65/1.4); 
  cam0.setLocation(82,82); 
  cam0.setGain(1.65); 
  cam0.setNoise(8.6); 
  cam0.setRotation(-3.2-TMath::Pi()/2); 
  cam0.setGainMap((TH2F*) gmfile.Get("gm_A80334")); 

  cam1.setAcceptance(acceptance *  18.4/18.6 * 1.3/1.4); 
  cam1.setLocation(82,-82); 
  cam1.setGain(1.3); 
  cam1.setNoise(9.9); 
  cam1.setRotation(-1.64+TMath::Pi()/2); 
  cam1.setGainMap((TH2F*) gmfile.Get("gm_100534")); 

  cam2.setAcceptance(acceptance * 1.3/1.4); 
  cam2.setLocation(-82,82); 
  cam2.setNoise(10.8); 
  cam2.setGain(1.3); 
  cam2.setRotation(1.53+TMath::Pi()/2); 
  cam2.setGainMap((TH2F*) gmfile.Get("gm_110121")); 

  cam3.setAcceptance(acceptance * 16.4 / 18.6 * 1.3/1.4); 
  cam3.setLocation(-82,-82); 
  cam3.setRotation(0.007-TMath::Pi()/2); 
  cam3.setGain(1.3); 
  cam3.setNoise(11.1); 
  cam3.setGainMap((TH2F*) gmfile.Get("gm_A80333")); 


  //make track
  g_F->setCurrentTrack(0); 
  g_C->setCurrentTrack(0); 

  dmtpc::mc::mctpc::Driver driver(2500); 
  dmtpc::mc::mctpc::DatasetReadout r(out); 

  r.defineCamera("A80334","A80334_bias"); 
  r.defineCamera("100534","100534_bias"); 
  r.defineCamera("110121","110121_bias"); 
  r.defineCamera("A80333","A80333_bias"); 

  double F_weight = pdist_F.getXSIntegral() * 4; 
  double C_weight = pdist_C.getXSIntegral(); 
  driver.plug(g_F,&d, F_weight); 
  driver.plug(g_C,&d, C_weight); 
  driver.plug(&d,&a); 
  driver.plug(&a,&cam0); 
  driver.plug(&cam0,&r); 
  driver.plug(&a,&cam1); 
  driver.plug(&cam1,&r); 
  driver.plug(&a,&cam2); 
  driver.plug(&cam2,&r); 
  driver.plug(&a,&cam3); 
  driver.plug(&cam3,&r); 
  driver.plug(&a,&r); 
  driver.plug(&d,&r); 

  driver.run(ntracks); 

  delete g_F; 
  delete g_C; 

}
