#include "Model.hh" 
#include "TTree.h"
#include "Avalanche.hh"
#include "TRandom3.h"
#include "TH2.h" 
#include "SolverElmer.hh" 
#include "TFile.h"
#include "TCanvas.h"
#include <cstdlib>
#include "ViewField.hh"
#include "Gas.hh" 
#include "AvalancheMicroscopic.hh"
#include "Sensor.hh"
#include "TrackMaker.hh"
#include "CollisionReader.hh"
#include "TableReader.hh"

#define USE_ELECTRODE false 
#define DRIFT_IONS false 

using namespace dmtpc::mc::amp; 

int main(int nargs, char ** args) 
{

  bool force_mesh= nargs > 1 ? atoi(args[1]) : false; 
  bool force_elmer= nargs > 2 ? atoi(args[2]) : false; 

  double v = nargs > 3 ? atof(args[3]) : 700; 
  double gap = nargs > 4 ? atof(args[4]) : 1; 
  double pitch = nargs > 5 ? atof(args[5]) : 0.254; 
  double radius = nargs > 6 ? atof(args[6]) : 0.015; 
  double rotation0 = nargs > 7  ? atof(args[7]) : 0.; 
  double rotation1 = nargs > 8  ? atof(args[8]) : 0; 
  double offset0 = nargs > 9  ? atof(args[9]) : 0.5; 
  double offset1 = nargs > 10  ? atof(args[10]) : 0.5; 
  double pressure = nargs > 11 ? atof(args[11]) : 30; 
  int n = nargs > 12 ? atoi(args[12]) : 10 ; 
  int deexc = nargs> 13? atoi(args[13]) : 1; 
  int aval_id = nargs > 14 ? atoi(args[14]) : 1; 
  int nruns =  getenv("MCAMP_N") ? atoi(getenv("MCAMP_N")) : 50; 

  Model m("3m","3m"); 

  m.enableElectrodes(USE_ELECTRODE); 
  m.enableIonDrift(DRIFT_IONS); 

  m.addUserParam("v",v,false); 
  m.addUserParam("g",gap); 
  m.addUserParam("l",pitch); 
  m.addUserParam("r",radius); 
  m.addUserParam("rt0",rotation0); 
  m.addUserParam("rt1",rotation1); 
  m.addUserParam("o0",offset0); 
  m.addUserParam("o1",offset1); 
  m.addUserParam("P",pressure,false,false); 
  m.addUserParam("n",(double)n); 
  m.addUserParam("deexc", deexc,false,false); 
  m.setCameraPosition(0,0,500); 


  SurfaceProperties none; 
  none.isElectrode = false; 
  none.voltageDefined = false; 

  SurfaceProperties cathode; 
  cathode.isElectrode = false; 
  cathode.voltageDefined = true; 
  cathode.voltage = -300; 


  SurfaceProperties anode; 
  anode.isElectrode = false; 
  anode.voltageDefined = true; 
  anode.voltage = v; 

  SurfaceProperties ground_top; 
  ground_top.isElectrode = false; 
  ground_top.voltageDefined = true; 
  ground_top.voltage = 0; 

  SurfaceProperties ground_bot; 
  ground_bot.isElectrode = false; 
  ground_bot.voltageDefined = true; 
  ground_bot.voltage = 0; 


  VolumeProperties steel; 
  steel.description = "steel"; 
  steel.epsilon = 1; 
  steel.conductivity = 1.4e6; 

  VolumeProperties cf4; 
  cf4.description = "cf4"; 
  cf4.epsilon = 1; 

//  cf4.medium = new Garfield::MediumGas; 


  GasLoader gas(getenv("MCAMP_GAS_DIR")); 
  cf4.medium = gas.getCF4(pressure); 
  ((Garfield::MediumMagboltz*) cf4.medium)->EnableCF4Photons(deexc); 


  double cathode_width = n* pitch; 
  double bbox_width = (n+2) * pitch * sqrt(2); 

  m.add("top_cathode", new Box(new TVector3(0,0,2),new TVector3(cathode_width,cathode_width,0.25)), &cathode, &steel); 

  m.add("top_mesh", new SimpleMesh(new TVector3(pitch*offset0,pitch*offset0,gap), pitch, radius,0,n,n,rotation0), &ground_top, &steel,false,true); 
  m.add("center_mesh", new SimpleMesh(new TVector3(0,0,0), pitch, radius,0,n,n,0.0), &anode, &steel,false,true); 
  m.add("bottom_mesh", new SimpleMesh(new TVector3(pitch*offset1,pitch*offset1,-gap), pitch, radius,0,n,n,rotation1), &ground_bot, &steel,false,true); 

  m.add("bottom_cathode", new Box(new TVector3(0,0,-2),new TVector3(cathode_width,cathode_width,0.25)), &cathode, &steel); 
  m.add("bbox", new Box(0,new TVector3(bbox_width,bbox_width,5)), 0, &cf4,true); 


  if (getenv("_CONDOR_SCRATCH_DIR"))
    m.copyFromRemote(getenv("MCAMP_INIT_DIR")); 

  //m.saveGeo(); 
  bool redo = force_mesh || !m.checkMesh();
  if (redo) m.makeMesh(3,2,true); 

  SolverElmer solver; 
  redo = force_elmer || !solver.solved(&m); 
  redo ? solver.solve(&m) : solver.loadSolution(&m);  

  Avalanche av(&solver,&m); 
  if (deexc) 
  {
    av.enablePhotonTransport(true); 
  }
 
  av.setSeed(100); 
  av.setBounds(-0.25,0.25,-0.25,0.25,gap+pitch,2*gap+pitch); 
  av.run(nruns); 
  

  if (getenv("_CONDOR_SCRATCH_DIR"))
    m.copyToRemote(getenv("MCAMP_INIT_DIR")); 




  return 0; 

}
  
