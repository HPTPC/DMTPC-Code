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
#include "TrackMaker.hh"
#include "CollisionReader.hh"
#include "TableReader.hh"

using namespace dmtpc::mc::amp; 

int main(int nargs, char ** args) 
{

  bool force_mesh= nargs > 1 ? atoi(args[1]) : false; 
  bool force_elmer= nargs > 2 ? atoi(args[2]) : false; 

  //Set up the model, first argument is id, second is output directory
  Model m("driftfield","driftfield"); 
  m.enableElectrodes(false); 
  m.enableIonDrift(false); 

  double rotation = 0; 
  double v = nargs > 3 ? atof(args[3]) : 500; 
  double drift_length = nargs > 4 ? atof(args[4]) : 50; 
  double radius = nargs > 5 ? atof(args[5]) : 100; 
  double pressure = nargs > 6 ? atof(args[6]) : 30; 
  double trimE = nargs > 7 ? atof(args[7]) : 0; 

  m.addUserParam("v",v,false); 
  m.addUserParam("d",drift_length); 
  m.addUserParam("r",radius); 
  m.addUserParam("p", pressure,false,false); 
  m.addUserParam("E", trimE,false,false); 
  m.setCameraPosition(0,0,1500); 
 
  //Set up the surface and volume properties
  SurfaceProperties cathode; 
  cathode.isElectrode = false; 
  cathode.voltageDefined = true; 
  cathode.voltage = -v; 


  SurfaceProperties ground; 
  ground.isElectrode = true; 
  ground.voltageDefined = true; 
  ground.voltage = 0; 

  VolumeProperties steel; 
  steel.description = "steel"; 
  steel.epsilon = 1; 
  steel.conductivity = 1.4e6; 

  GasLoader gas("../gas");  //directory that has gas files 

  VolumeProperties cf4; 
  cf4.description = "cf4"; 
  cf4.epsilon = 1; 
  cf4.medium = gas.getCF4(pressure); //get 60Torr cf4 

  //Add the geometry. 
  //In addition to Box and WovenMesh, Torus, Ring and Disc are supported
 
  // Create cathode and anodes as box. First argument is center, second is extents. It is also possible to change orientation and rotation. 
  // The surface and boundary properties are also passed here
  m.add("cathode", new Disc(new TVector3(0,0,drift_length + 0.5),radius,1), &cathode, &steel); 
  m.add("ground_mesh", new Disc(new TVector3(0,0,-0.5),radius,1), &ground, &steel); 
  m.add("ground_ring", new Ring(new TVector3(0,0,3.25),radius, radius + 25.4, 6), &ground, &steel); 


  //the last boolean argument makes this a bounding box
  m.add("bbox", new Disc(new TVector3(0,0,drift_length/2),12*25.4, drift_length + 10), &ground,&cf4,true); 

  //Create the mesh, 3D, order 2, optimized
  bool redo = force_mesh || !m.checkMesh();
  if (redo) m.makeMesh(3,2,true); 

  //Solve for the field
  SolverElmer solver; 
  redo = force_elmer || !solver.solved(&m); 
  redo ? solver.solve(&m) : solver.loadSolution(&m);  

  Avalanche av(&solver,&m); 
  av.setBounds(-radius/sqrt(2),radius/sqrt(2),-radius/sqrt(2),radius/sqrt(2),1,drift_length); 
  av.setSeed(100); 
  if (trimE>0)
  {
    dmtpc::mc::retrim::TableReader ** tables = new dmtpc::mc::retrim::TableReader* [2]; 
    dmtpc::mc::retrim::CollisionReader *coll = new dmtpc::mc::retrim::CollisionReader(TString::Format("../../retrim/data/f/%dtorr/coll/COLLISON_%.2fkeV.txt",int(pressure),trimE)); 
    tables[0] = new dmtpc::mc::retrim::TableReader(TString::Format("../../retrim/data/srim/f_in_cf4_%dtorr.txt",int(pressure))); 
    tables[1] = new dmtpc::mc::retrim::TableReader(TString::Format("../../retrim/data/srim/c_in_cf4_%dtorr.txt",int(pressure))); 
    dmtpc::mc::retrim::TrackMaker* tmaker = new dmtpc::mc::retrim::TrackMaker(coll,2,(const dmtpc::mc::retrim::TableReader**)tables); 
    dmtpc::mc::retrim::SimpleIonizationModel * simp = new dmtpc::mc::retrim::SimpleIonizationModel; 
    av.setTrackMaker(tmaker); 
    av.setMode(Avalanche::RETRIM); 
    av.setIonizationModel(simp); 
    av.setTrackDirection(1,0,0); 
  }

  av.setSeed(100); 
  av.run(trimE > 0 ? 100 : 10000); 

  
  
}
  
