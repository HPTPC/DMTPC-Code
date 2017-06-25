#include "Model.hh" 
#include "TCanvas.h" 
#include "SolverElmer.hh" 
#include "ViewField.hh"
#include "TCanvas.h" 


using namespace dmtpc::mc::amp; 

int main(int nargs, char ** args) 
{

  const char * outputdir = nargs > 1 ? args[1] : "."; 

  Model m("compound",outputdir); 

  SurfaceProperties none; 
  none.isElectrode = false; 
  none.voltageDefined = false; 

  SurfaceProperties anode; 
  anode.isElectrode = true; 
  anode.voltageDefined = true; 
  anode.voltage = 100; 

  SurfaceProperties ground; 
  ground.isElectrode = true; 
  ground.voltageDefined = true; 
  ground.voltage = 0; 

  VolumeProperties steel; 
  steel.description = "steel"; 
  steel.epsilon = 1; 
  steel.conductivity = 1.4e6; 

  VolumeProperties air; 
  air.description = "air"; 
  air.epsilon = 1; 

  m.add("disctest", new Disc(new TVector3(0,0,7),3,0.25), &ground, &steel); 
  m.add("ringtest", new Ring(new TVector3(0,0,-7),2.5,3,0.25), &ground, &steel); 
  m.add("torustest", new Torus(0,2,0.25, new TVector3(1,1,0)), &anode, &steel); 
  m.add("boxtest", new Box(0,new TVector3(20,20,20)), &none, &air,true); 
  m.saveGeo(); 
  m.makeMesh(3,2,0); 
  SolverElmer solver; 
  solver.solve(&m); 


 // Garfield::Sensor* sensor = new Garfield::Sensor();
 // sensor->AddComponent(solver.fieldMap());
 // sensor->SetArea(-20,-20,-20,20,20,20);

  TCanvas c; 


  // Set up the object for field visualization.
  Garfield::ViewField * vf = new Garfield::ViewField();
  vf->SetComponent(solver.fieldMap());
  vf->SetCanvas(&c);
  vf->SetArea(-2,-2,2,2);
  vf->SetNumberOfContours(20);
  vf->SetNumberOfSamples2d(30,30);
  vf->SetPlane(-1,-1,0,0,0,0);

  vf->PlotContour("v"); 
  c.SaveAs(TString::Format("%s/field.pdf",outputdir)); 
}
  
