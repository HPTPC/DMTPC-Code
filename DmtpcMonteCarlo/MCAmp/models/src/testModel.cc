#include "Model.hh" 
#include "SolverElmer.hh" 


using namespace dmtpc::mc::amp; 

int main(int nargs, char ** args) 
{

  const char * outputdir = nargs > 1 ? args[1] : "."; 

  Model m("test",outputdir); 

  SurfaceProperties none; 
  none.isElectrode = false; 
  none.voltageDefined = false; 

  VolumeProperties air; 
  air.description = "air"; 
//  air.density = 1.2e-3; 
  air.epsilon = 1; 
//  m.add("boxtest", new Box, &none, &air); 
//   m.add("disctest", new Disc, &none, &air); 
  m.add("ringtest", new Ring, &none, &air); 
//  m.add("torustest", new Torus, &none, &air); 
//  m.saveGeo(); 
  m.makeMesh(); 
  m.saveGeo(); 
//  SolverElmer solver; 
//  solver.makeFieldSif(&m); 
//  m.makeMesh(); 
}
  
