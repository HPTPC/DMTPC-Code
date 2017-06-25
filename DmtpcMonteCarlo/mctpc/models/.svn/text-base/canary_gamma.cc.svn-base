#include "CollisionReader.hh"
#include "TableReader.hh"
#include "HEEDGenerator.hh"
#include "CommonTools.hh"
#include "MediumMagboltz.hh"
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


  int P = 30; 




  dmtpc::mc::retrim::SimpleIonizationModel m; 

  dmtpc::mc::mctpc::UniformCylinder posdist(0,0,50,0,50); 
  dmtpc::mc::mctpc::IsotropicVectorDistribution isodir; 
  dmtpc::mc::mctpc::ConstantScalarDistribution gamma_e(5.9); 
  dmtpc::mc::mctpc::ConstantScalarDistribution tdist(0); 

  dmtpc::mc::mctpc::IndependentDistribution gamma_pdist(&posdist, &isodir, &gamma_e, &tdist); 


  Garfield::MediumMagboltz * gas = new Garfield::MediumMagboltz; 
  TString gasfile = TString::Format("../MCAmp/gas/cf4_%d_298.gas", P); 
  gas->LoadGasFile(gasfile.Data()); 
  
  gas->SetW(34); 
  gas->SetFanoFactor(0.18); 
  gas->Initialise(true); 


  TVector3 center(0,0,3); 
  TVector3 halflengths(10,10,3); 
  TVector3 efield(0,0,100); 

  dmtpc::mc::mctpc::HEEDGenerator gamma_g(gas, &center, &halflengths, &efield); 

  gamma_g.setParticleDistribution(&gamma_pdist); 


  /* Set up avalanche*/

  double cathode_transparency = 0.9; 
  double window_transparency = 0.25; 
  double fudge_factor = 1; 
  dmtpc::mc::mctpc::CachedMCAmpAvalanche::Params sap; 
  sap.acceptance = 1.e-3; // guess for now 
  sap.acceptance *= cathode_transparency * window_transparency * fudge_factor; 
  sap.gain = V == 770 ? 4.4e5 : V == 800  ? 8.5e5 : 0; 


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


  /* Set up sensors */
  dmtpc::mc::mctpc::Driver driver(ntracks); 
  dmtpc::mc::mctpc::DatasetReadout r(out); 

  r.defineCamera("10435","10435_bias"); 


  driver.plug(&gamma_g,&d); 
  driver.plug(&d,&a); 
  driver.plug(&a,&cam); 
  driver.plug(&cam,&r); 
  driver.plug(&a,&r); 
  driver.plug(&d,&r); 

  driver.run(ntracks); 





}



