#include "Model.hh" 
#include <cstdlib>
#include "SolverElmer.hh" 
#include "TH2.h"
#include "TrackHeed.hh"
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

  bool force_mesh= nargs > 1 ? atoi(args[1]) : false; 
  bool force_elmer= nargs > 2 ? atoi(args[2]) : false; 

  //Set up the model, first argument is id, second is output directory
  Model m("driftcagetrk","driftcagetrk"); 

  double rotation = 0; 
  double v = nargs > 3 ? atof(args[3]) : 650; 
  double gap = nargs > 4 ? atof(args[4]) : 0.435; 
  double pitch = nargs > 5 ? atof(args[5]) : 0.254; 
  double radius = nargs > 6 ? atof(args[6]) : 0.015; 
  double pressure = nargs > 7 ? atof(args[7]) : 60; 

  m.addUserParam("v",v,false); 
  m.addUserParam("gap",gap); 
  m.addUserParam("pitch",pitch); 
  m.addUserParam("radius",radius); 
  m.addUserParam("rotation",rotation); 
  m.addUserParam("pressure", pressure,false,false); 
  m.setCameraPosition(0,0,1000); 
 
  ChargeSensitiveAmplifier cr112(0.013,50e3,500e6); //13 mV/pC, 50 us falloff, 500 MHz bandwidth
  FastAmplifier r2e(0.08,500e6); //gain of 80 mV/pC, 500 MHz bandwidth

  //Set up the surface and volume properties
  SurfaceProperties cathode; 
  cathode.isElectrode = false; 
  cathode.voltageDefined = true; 
  cathode.voltage = -30; 

  SurfaceProperties anode; 
  anode.isElectrode = true;  //this doesn't do anything yet
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

  GasLoader gas("../gas");  //directory that has gas files 

  VolumeProperties cf4; 
  cf4.description = "air"; 
  cf4.epsilon = 1; 
  cf4.medium = gas.getCF4(pressure); //get 

  //Add the geometry. 
  //In addition to Box and WovenMesh, Torus, Ring and Disc are supported
 
  // Create cathode and anodes as box. First argument is center, second is extents. It is also possible to change orientation and rotation. 
  // The surface and boundary properties are also passed here
  m.add("cathode", new Box(new TVector3(0,0,2.125),new TVector3(6.5,6.5,0.25)), &cathode, &steel); 
  m.add("anode", new Box(new TVector3(0,0,-0.125),new TVector3(6.5,6.5,0.25)), &anode, &steel); 

  // Create a woven mesh. The first argument is position, followed by wire pitch, radius, normalization vector (NULL for z), 
  // number of x braids, number of y braids and rotation around normalization vector
  m.add("mesh", new WovenMesh(new TVector3(0,0,gap), pitch, radius,NULL,int(6.5/pitch),int(6.5/pitch),rotation), &ground, &steel,false,true); 

  //the last boolean argument makes this a bounding box
  m.add("bbox", new Box(new TVector3(0,0,1),new TVector3(7,7,3)), 0, &cf4,true); 

  //Create the mesh, 3D, order 2, optimized
  bool redo = force_mesh || !m.checkMesh();
  if (redo) m.makeMesh(3,2,true); 

  //Solve for the field
  SolverElmer solver; 
  redo = force_elmer || !solver.solved(&m); 
  redo ? solver.solve(&m) : solver.loadSolution(&m);  

  Avalanche av(&solver,&m); 
  av.setBounds(-4,4,-4,4,1.1,2); 
  av.setIonZRange(0, 2*gap); 
//  av.setTrackDirection(1,1,0); 
  av.setSeed(100); 
 // av.setDebug(1); 
  av.setMode(Avalanche::PHOTON); 
  av.setEnergy(5900); 
  av.run(1); 

  
  TCanvas c; 
  Garfield::ViewField * vf = new Garfield::ViewField();
  vf->SetComponent(solver.fieldMap());
  vf->SetArea(-0.5,-0.15,5.25,0.25); // in cm!!!! 
  vf->SetNumberOfContours(50);
  vf->SetNumberOfSamples2d(120,120);
  vf->SetPlane(-1,-1,0,0,0,0);
  vf->SetCanvas(&c);
  vf->PlotContour("e"); //plot electric field magnitude 

  c.SaveAs(TString::Format("%s/aval/field_%s.pdf",m.getOutputDir(), m.getFullName())); 


}
  
