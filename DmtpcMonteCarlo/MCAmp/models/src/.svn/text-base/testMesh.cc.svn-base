#include "Model.hh" 
#include "SolverElmer.hh"


using namespace dmtpc::mc::amp; 

int main(int nargs, char ** args) 
{

  const char * outputdir = nargs > 1 ? args[1] : "."; 

  Model m("mesh",outputdir); 

  SurfaceProperties none; 
  none.isElectrode = false; 
  none.voltageDefined = true; 
  none.voltage = 0; 

  SurfaceProperties mesh; 
  none.isElectrode = false; 
  none.voltageDefined = true; 
  none.voltage = 1; 

  VolumeProperties steel; 
  steel.description = "steel"; 
  steel.epsilon = 1; 

  VolumeProperties cf4; 
  cf4.description = "air"; 
  cf4.epsilon = 1; 


//  m.add("mesh", new WovenMesh(0,0.5,0.1,0,4,4), &mesh, &steel); 
  m.add("mesh", new SimpleMesh(new TVector3(0,0,0),0.75,0.1,0,4,4), &mesh, &steel); 
  m.add("bbox", new Box(0,new TVector3(30,30,30)), &none, &cf4,true); 
  m.saveGeo(); 
//  SolverElmer solver; 
//  solver.makeFieldSif(&m); 
//  m.makeMesh(3,2); 
}
  
