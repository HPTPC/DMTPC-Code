#include "CollisionReader.hh"
#include "TableReader.hh"
#include "CommonTools.hh"
#include "DatasetReadout.hh"
#include "TChain.h" 
#include "TStyle.h"
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

  const char * out = nargs > 1 ? args[1] : "canary_dd_out/"; 
//  float energy = nargs > 2 ? atof(args[2]) : 200.00; 
  int ntracks = nargs > 2 ? atoi(args[2]) : 1000; 

  int V  = nargs > 3 ? atoi(args[3]) : 770; 
  double dd_weight = 1;//nargs > 4 ? atof(args[4]) : 1. ; 

//  double gamma_weight = nargs > 4 ? atof(args[4]) : 0. ; 
//  double alpha_weight = nargs > 5 ? atof(args[5]) : 0. ; 


  int P = 30; 



  /* Set up DD generators */
  const dmtpc::mc::retrim::TableReader * tables_f[2]; 
  const dmtpc::mc::retrim::TableReader * tables_c[2]; 
  // I don't remember if the order was important, but if it was, then the primary ion should go first
  tables_f[0] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/f_in_cf4_%dtorr.txt", P)); 
  tables_f[1] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/c_in_cf4_%dtorr.txt", P)); 
  tables_c[1] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/f_in_cf4_%dtorr.txt", P));  
  tables_c[0] = new dmtpc::mc::retrim::TableReader(TString::Format("../retrim/data/srim/c_in_cf4_%dtorr.txt", P)); 

  dmtpc::mc::retrim::SimpleIonizationModel m; 

  dmtpc::physics::endf::CrossSection cs_F("ENDF_CS_n_on_19F","../../DmtpcPhysics/tables/"); 
  dmtpc::physics::endf::CrossSection cs_C("ENDF_CS_n_on_12C","../../DmtpcPhysics/tables/"); 
  cs_F.readFile(); 
  cs_C.readFile(); 

  const char * retrim_data_dir = getenv("RETRIM_DATA_DIR") ? getenv("RETRIM_DATA_DIR") : "/net/zwicky/esata01/dmtpc/production/retrim_data";
  dmtpc::mc::mctpc::MultiTRIMGenerator * g_F = dmtpc::mc::mctpc::MultiTRIMGenerator::makeFromDir(TString::Format("%s/f/%dtorr/coll/",retrim_data_dir,P), &m, 2, tables_f, 0.98,9 ); 
  dmtpc::mc::mctpc::MultiTRIMGenerator * g_C = dmtpc::mc::mctpc::MultiTRIMGenerator::makeFromDir(TString::Format("%s/c/%dtorr/coll/",retrim_data_dir,P), &m, 2, tables_c, 0.98,6 ); 


  dmtpc::mc::mctpc::UniformCylinder posdist(0,0,50,0,50); 
  dmtpc::mc::mctpc::ConstantScalarDistribution edist(2450); 
  TVector3 source_position(12 * 25.4,0,5); 
  TVector3 source_dir(-1,0,0); 

  dmtpc::physics::endf::AngularDistribution dcs_F(100);
  dcs_F.readFile("ENDF_DCS_n_on_19F","../../DmtpcPhysics/tables/"); 
  dmtpc::physics::endf::AngularDistribution dcs_C(100);
  dcs_C.readFile("ENDF_DCS_n_on_12C","../../DmtpcPhysics/tables/"); 

  dmtpc::mc::mctpc::CollimatedNeutronSourceDistribution pdist_F(&source_position, &source_dir, &posdist, 
                                                              &edist, &cs_F, &dcs_F); 
  dmtpc::mc::mctpc::CollimatedNeutronSourceDistribution pdist_C(&source_position, &source_dir, &posdist, 
                                                              &edist, &cs_C, &dcs_C); 

  pdist_F.setRecoilELimits(10,250); 
  pdist_C.setRecoilELimits(10,250); 

  g_F->setParticleDistribution(&pdist_F); 
  g_C->setParticleDistribution(&pdist_C); 

  double F_weight = pdist_F.getXSIntegral() * 4 * dd_weight; 
  double C_weight = pdist_C.getXSIntegral() * dd_weight; 


  /* Set up gamma generator */ 

  /*
  dmtpc::mc::mctpc::IsotropicVectorDistribution isodir; 
  dmtpc::mc::mctpc::UniformScalarDistribution gamma_e(1,20); 
  dmtpc::mc::mctpc::ConstantScalarDistribution tdist(0); 

  dmtpc::mc::mctpc::IndependentDistribution gamma_pdist(&posdist, &isodir, &gamma_e, &tdist); 


  HEEDGenerator gamma_g; 


  */

  /* Set up avalanche*/

  double cathode_transparency = 0.9; 
  double window_transparency = 0.25 * 0.75; 
  double fudge_factor = 1;  
  if (V==800) fudge_factor *=  3 / (9.5/4.4); 
  dmtpc::mc::mctpc::CachedMCAmpAvalanche::Params sap; 
  sap.acceptance = 1.e-3; // guess for now 
  sap.acceptance *= cathode_transparency * window_transparency * fudge_factor; 
  sap.gain = V == 770 ? 4.4e5 : V == 800  ? 9.5e5 : 0; 

//  sap.gain *=1; // because of all the extra from the photons... 

//  double pg = 1.e-6;  
//  sap.photon_table.add(pg, 16.5,1); 
//  sap.photon_table.add(pg * 0.067, 18,1); 
//  sap.photon_table.add(pg * 0.067, 20.4,1); 
//  sap.photon_table.add(pg * 0.067, 22.5,1); 
//  sap.photon_table.add(pg, 23,1); 
 // sap.cache_params.from_photon = 0; 

  dmtpc::mc::mctpc::SimpleDrift d (0,50,0.0107, 0.0107, 0.1); 
  d.setXYCut(true,0,0,50); 

  TChain avch("avalanche");
  dmtpc::core::common::addFilesToChain(&avch, nargs > 4 ? args[4] :"avalanche_files.txt"); 
  dmtpc::mc::mctpc::CachedMCAmpAvalanche a(&avch,"",sap); 
  dmtpc::mc::mctpc::SimpleCamera cam(2048,2048,4,"10435"); 
  cam.setAcceptance(1);  // do all acceptance in amplification region for speed
  cam.setNoise(6.6); 
  cam.setQE(0.85); 
  cam.setBias(300); 
  cam.setScale(0.042); 
//  cam.setBlur(1); 

  g_F->setCurrentTrack(0); 
  g_C->setCurrentTrack(0); 

  /* Set up sensors */
  dmtpc::mc::mctpc::Driver driver(ntracks); 
  dmtpc::mc::mctpc::DatasetReadout r(out); 

  r.defineCamera("10435","10435_bias"); 


  driver.plug(g_F,&d, F_weight); 
  driver.plug(g_C,&d, C_weight); 
  driver.plug(&d,&a); 
  driver.plug(&a,&cam); 
  driver.plug(&cam,&r); 
  driver.plug(&a,&r); 
  driver.plug(&d,&r); 

  driver.run(ntracks); 

  delete g_F; 
  delete g_C; 




}



