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


using namespace dmtpc::mc::amp; 

int main(int nargs, char ** args) 
{

  bool force_mesh=  false ;//nargs > 1 ? atoi(args[1]) : false; 
  bool force_elmer= false ;//nargs > 2 ? atoi(args[2]) : false; 

  double v = nargs > 1 ? atof(args[1]) : 385; 
  double v2 = nargs > 2 ? atof(args[2]) : 770; 
  double rotation0=nargs > 3 ? atof(args[3]) : 0.0; 
  double rotation1=nargs > 4 ? atof(args[4]) : 0.0; 
  double gap = nargs > 5 ? atof(args[5]) : 0.5; 
  double radius =nargs > 6 ? atof(args[6]) :  0.015; 
  double pitch = nargs > 7 ? atof(args[7]) : 0.254; 
  double pressure =nargs > 8 ? atof(args[8]) :  30; 
  int nwires = nargs > 9 ? atoi(args[9]) : 10; 
  double offset0 = nargs > 10 ? atof(args[10]) : 0.5; 
  double offset1 = nargs > 11 ? atof(args[11]) : 0.5; 
  int aval_id = nargs > 12 ? atoi(args[12]) : 1; 
  int deexc = nargs> 13? atoi(args[13]) : 1; 

  Model m("doubleamp","doubleamp"); 
  m.enableElectrodes(false); 
  m.enableIonDrift(false); 

  m.addUserParam("v",v,false); 
  m.addUserParam("v2",v2,false); 
  m.addUserParam("g",gap); 
  m.addUserParam("wp",pitch); 
  m.addUserParam("wr",radius); 
  m.addUserParam("r0",rotation0); 
  m.addUserParam("r1",rotation1); 
  m.addUserParam("P",pressure,false,false); 
  m.addUserParam("n",nwires); 
  m.addUserParam("o0",offset0); 
  m.addUserParam("o1",offset1); 
  m.addUserParam("dxc", deexc,false,false); 

  SurfaceProperties none; 
  none.isElectrode = false; 
  none.voltageDefined = false; 

  SurfaceProperties cathode; 
  cathode.isElectrode = false; 
  cathode.voltageDefined = true; 
  cathode.voltage = -200; 


  SurfaceProperties anode; 
  anode.isElectrode = true; 
  anode.voltageDefined = true; 
  anode.voltage = v; 

  SurfaceProperties anode2; 
  anode2.isElectrode = true; 
  anode2.voltageDefined = true; 
  anode2.voltage = v2; 


  SurfaceProperties ground; 
  ground.isElectrode = false; 
  ground.voltageDefined = true; 
  ground.voltage = 0; 

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




  m.add("top_cathode", new Box(new TVector3(0,0,2),new TVector3(1.25*nwires*pitch,1.25*nwires*pitch,0.25)), &cathode, &steel); 
  m.add("top_mesh", new SimpleMesh(new TVector3(gap * offset0,gap * offset0,gap), pitch, radius,0,nwires,nwires,rotation0), &ground, &steel, false,true); 
  m.add("center_mesh", new SimpleMesh(new TVector3(0,0,0), pitch, radius,0,nwires,nwires,0.0), &anode, &steel,false,true); 
  m.add("bottom_mesh", new SimpleMesh(new TVector3(gap * offset1,gap * offset1,-gap), pitch, radius,0,nwires,nwires,rotation1), &anode2, &steel); 
  m.add("bbox", new Box(new TVector3(0,0,1),new TVector3(1.5 *nwires * pitch,1.5*nwires*pitch,3.5)), 0, &cf4,true); 

  if (getenv("_CONDOR_SCRATCH_DIR"))
    m.copyFromRemote(getenv("MCAMP_INIT_DIR")); 

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

  av.setBounds(-0.25,0.25,-0.25,0.25,gap+pitch,2*gap+pitch); 
  av.setSeed(100); 
//  av.setDebug(1); 
   av.run(getenv("MCAMP_N") ? atoi(getenv("MCAMP_N")) : 10,aval_id); 
  
  if (getenv("_CONDOR_SCRATCH_DIR"))
    m.copyToRemote(getenv("MCAMP_INIT_DIR")); 



}
  
