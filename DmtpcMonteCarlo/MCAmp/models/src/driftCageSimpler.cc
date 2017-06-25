#include "Model.hh" 
#include "SolverElmer.hh" 


using namespace dmtpc::mc::amp; 

int main(int nargs, char ** args) 
{

  const char * outputdir = nargs > 1 ? args[1] : "."; 

  Model m("driftcage","drift_cage_simpler"); 

  SurfaceProperties none; 
  none.isElectrode = false; 
  none.voltageDefined = false; 

  SurfaceProperties cathode; 
  cathode.isElectrode = true; 
  cathode.voltageDefined = true; 
  cathode.voltage = -300; 


  SurfaceProperties anode; 
  anode.isElectrode = true; 
  anode.voltageDefined = true; 
  anode.voltage = 650; 

  SurfaceProperties ground; 
  ground.isElectrode = true; 
  ground.voltageDefined = true; 
  ground.voltage = 0; 

  VolumeProperties steel; 
  steel.description = "steel"; 
  steel.epsilon = 1; 
  steel.conductivity = 1.4e6; 

  VolumeProperties cf4; 
  cf4.description = "air"; 
  cf4.epsilon = 1; 

  m.add("cathode", new Box(new TVector3(0,0,10),new TVector3(2,2,0.25)), &cathode, &steel); 
  m.add("anode", new Box(new TVector3(0,0,-0.125),new TVector3(2,2,0.25)), &anode, &steel); 
  m.add("mesh", new WovenMesh(new TVector3(0,0,0.5), 0.254, 0.03,0,8,8,0), &ground, &steel); 
  m.add("bbox", new Box(new TVector3(0,0,15),new TVector3(20,20,35)), &ground, &cf4,true); 
  m.saveGeo(); 
  SolverElmer s; 
  s.makeFieldSif(&m); 
//  m.makeMesh(); 
}
  
