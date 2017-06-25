#include "Model.hh" 
#include "TCanvas.h"
#include "SolverElmer.hh" 
#include "ViewField.hh" 


using namespace dmtpc::mc::amp; 

int main(int nargs, char ** args) 
{

  const char * outputdir = nargs > 1 ? args[1] : "cage"; 

  Model m("cage",outputdir); 

  SurfaceProperties none; 
  none.isElectrode = false; 
  none.voltageDefined = false; 

  VolumeProperties steel; 
  steel.description = "steel"; 
  steel.epsilon = 1; 

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
  m.add("cathode", new Disc(new TVector3(0,0,50),200,2), &cathode, &steel); 

  for (int i = 0; i < 9; i++)
  {
    SurfaceProperties *wire = new SurfaceProperties; 
    wire->isElectrode = false; 
    wire->voltageDefined = true; 
    wire->voltage = wire_voltages[i]; 
    m.add(strdup(TString::Format("wire%d",i)), new Ring(new TVector3(0,0,wire_heights[i]),180,190,1), wire, &steel); 
  }


  m.add("anode", new Disc(new TVector3(0,0,0),200,2), &ground, &steel); 
  m.add("bbox", new Disc(new TVector3(0,0,27.5), 210,60), &none, &air,true); 

  m.makeMesh(3,2,true); 
  SolverElmer solver; 
  solver.solve(&m); 

  TCanvas c; 
  Garfield::ViewField * vf = new Garfield::ViewField();
  vf->SetComponent(solver.fieldMap());
  vf->SetNumberOfContours(50);
  vf->SetNumberOfSamples2d(120,120);
  vf->SetPlane(0,1,0,0,0,0);
  vf->SetArea(-5,-5,5,5); // in cm!!!! 
  vf->SetCanvas(&c);
  vf->PlotContour("e"); //plot electric field magnitude 

  c.SaveAs(TString::Format("%s/aval/field_%s.pdf",m.getOutputDir(), m.getFullName())); 



}
  
