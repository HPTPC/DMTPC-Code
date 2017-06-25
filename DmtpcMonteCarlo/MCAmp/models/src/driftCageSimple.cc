#include "Model.hh" 
#include <cstdlib>
#include "SolverElmer.hh" 
#include "Amplifier.hh"
#include "TH2.h"
#include "TTree.h"
#include "Avalanche.hh"
#include "TFile.h" 
#include "TCanvas.h"
#include "TRandom3.h"
#include "AvalancheMicroscopic.hh"
#include "Sensor.hh"
#include "Gas.hh" 
#include "ViewField.hh"

using namespace dmtpc::mc::amp; 

int main(int nargs, char ** args) 
{


  //Set up the model, first argument is id, second is output directory
  Model m("driftcage","driftcage"); 
  m.enableElectrodes(false); 
  m.enableIonDrift(false); 

  double rotation = 0; 
  double v = nargs > 3 ? atof(args[3]) : 700; 
  double gap = nargs > 4 ? atof(args[4]) : 0.5; 
  double pitch = nargs > 5 ? atof(args[5]) : 0.254; 
  double radius = nargs > 6 ? atof(args[6]) : 0.015; 
  double pressure = nargs > 7 ? atof(args[7]) : 40; 
  int deexc = nargs> 8? atoi(args[8]) : 1; 
  int aval_id = nargs > 9 ? atoi(args[9]) : 1; 

  double w = 5; 

  m.addUserParam("v",v,false); 
  m.addUserParam("gap",gap); 
  m.addUserParam("pitch",pitch); 
  m.addUserParam("w",w); 
  m.addUserParam("radius",radius); 
  m.addUserParam("rotation",rotation); 
  m.addUserParam("pressure", pressure,false,false); 
  m.addUserParam("deexc", deexc,false,false); 
  m.setCameraPosition(0,0,500); 
 
  //Set up the surface and volume properties
  SurfaceProperties cathode; 
  cathode.isElectrode = false; 
  cathode.voltageDefined = true; 
  cathode.voltage = -40; 

  ChargeSensitiveAmplifier cr112(0.013,50e3,50e6); //13 mV/pC, 50 us falloff, 500 MHz bandwidth
  FastAmplifier r2e; //gain of 80, 500 MHz bandwidth

  SurfaceProperties anode; 
  anode.isElectrode = true;
  anode.voltageDefined = true; 
  anode.voltage = v; 
  anode.amp = &cr112; 

  SurfaceProperties ground; 
  ground.isElectrode = true; 
  ground.voltageDefined = true; 
  ground.voltage = 0; 
  ground.amp = &r2e; 

  VolumeProperties steel; 
  steel.description = "steel"; 
  steel.epsilon = 1; 
  steel.conductivity = 1.4e6; 

  GasLoader gas(getenv("MCAMP_GAS_DIR")); 

  VolumeProperties cf4; 
  cf4.description = "air"; 
  cf4.epsilon = 1; 
  cf4.medium = gas.getCF4(pressure); //get 60Torr cf4 
  ((Garfield::MediumMagboltz*) cf4.medium)->EnableCF4Photons(deexc); 

  //Add the geometry. 
  //In addition to Box and WovenMesh, Torus, Ring and Disc are supported
 
  // Create cathode and anodes as box. First argument is center, second is extents. It is also possible to change orientation and rotation. 
  // The surface and boundary properties are also passed here
  m.add("cathode", new Box(new TVector3(0,0,3.125),new TVector3(w,w,0.25)), &cathode, &steel); 
  m.add("anode", new Box(new TVector3(0,0,-0.125),new TVector3(w,w,0.25)), &anode, &steel); 

  // Create a woven mesh. The first argument is position, followed by wire pitch, radius, normalization vector (NULL for z), 
  // number of x braids, number of y braids and rotation around normalization vector
  m.add("mesh", new SimpleMesh(new TVector3(0,0,gap), pitch, radius,NULL,int(w/pitch),int(w/pitch),rotation), &ground, &steel,false,true); 

  //the last boolean argument makes this a bounding box
  m.add("bbox", new Box(new TVector3(0,0,1),new TVector3(1.5*w,1.5*w,5)), 0, &cf4,true); 

  if (getenv("_CONDOR_SCRATCH_DIR"))
    m.copyFromRemote(getenv("MCAMP_INIT_DIR")); 

  bool force_mesh= nargs > 1 ? atoi(args[1]) : false; 
  bool force_elmer= nargs > 2 ? atoi(args[2]) : false; 
  printf("force_elmer is %d\n",force_elmer); 
  //Create the mesh, 3D, order 2, optimized
  bool redo = force_mesh || !m.checkMesh();
  if (redo) m.makeMesh(3,2,true); 

  //Solve for the field
  SolverElmer solver; 
  redo = force_elmer || !solver.solved(&m); 
  printf("Redo is: %d\n", redo); 
  redo ? solver.solve(&m) : solver.loadSolution(&m);  

  Avalanche av(&solver,&m); 
  if (deexc) 
  {
    av.enablePhotonTransport(true); 
  }
  av.setBounds(-pitch,pitch,pitch,pitch,pitch+gap,1 + pitch + gap); 
  av.setSeed(100); 
//  av.enablePlotting(true); 
//  av.setDebug(1); 
  
 // av.run(500); 
  av.run(100,aval_id); 

  if (getenv("_CONDOR_SCRATCH_DIR"))
    m.copyToRemote(getenv("MCAMP_INIT_DIR")); 


}
  
