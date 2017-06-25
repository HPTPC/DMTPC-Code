#include "Model.hh" 
#include "SolverElmer.hh" 
#include "ViewField.hh"
#include "TCanvas.h" 


using namespace dmtpc::mc::amp; 

int main(int nargs, char ** args) 
{

  const char * outputdir = nargs > 1 ? args[1] : "."; 

  Model m("wire",outputdir); 

  SurfaceProperties none; 
  none.isElectrode = false; 
  none.voltageDefined = false; 

  VolumeProperties steel; 
  steel.description = "steel"; 
  steel.epsilon = 1; 
  steel.conductivity = 1.4e6; 

  VolumeProperties air; 
  air.description = "air"; 
  air.epsilon = 1; 

  SurfaceProperties ground; 
  ground.isElectrode = true; 
  ground.voltageDefined = true; 
  ground.voltage = 0; 



  SurfaceProperties cathode; 
  cathode.isElectrode = true; 
  cathode.voltageDefined = true; 
  cathode.voltage = -1000; 

 
  double wire_heights[9] = {45,40,35,30,25,20,15,10,5};
  double wire_voltages[9] = {-900,-800,-700,-600,-500,-400,-300,-200,-100};
  m.add("cathode", new Disc(new TVector3(0,0,50),50,0.25), &cathode, &steel); 
  m.add("ground", new Disc(new TVector3(0,0,0),50,0.25), &ground, &steel); 

  for (int i = 0; i < 8; i++)
  {
    SurfaceProperties *wire = new SurfaceProperties; 
    wire->isElectrode = false; 
    wire->voltageDefined = true; 
    wire->voltage = wire_voltages[i]; 
    m.add(TString::Format("wire%d",i), new Torus(new TVector3(0,0,wire_heights[i]),49.0,1), wire, &steel); 
  }

  m.add("bbox", new Disc(new TVector3(0,0,27.5), 60,60), &ground, &air,true); 

  m.saveGeo(); 
  m.makeMesh(3,2,1); 
  SolverElmer solver; 
  solver.solve(&m); 
  TCanvas c; 


  // Set up the object for field visualization.
  Garfield::ViewField * vf = new Garfield::ViewField();
  vf->SetComponent(solver.fieldMap());
  vf->SetCanvas(&c);
  vf->SetArea(-6,-3,6,3);
  vf->SetNumberOfContours(20);
  vf->SetNumberOfSamples2d(30,30);
  vf->SetPlane(-1,-1,0,0,0,0);
  vf->PlotContour("v"); 
  c.SaveAs(TString::Format("%s/field.pdf",outputdir)); 

}
  
