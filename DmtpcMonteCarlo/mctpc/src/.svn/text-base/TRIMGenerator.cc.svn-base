#include "TRIMGenerator.hh" 
#include "TrackMaker.hh"
#include "IonizationModel.hh"


dmtpc::mc::mctpc::TRIMGenerator::TRIMGenerator(dmtpc::mc::retrim::TrackMaker * track_maker, dmtpc::mc::retrim::IonizationModel * ioniz_model)
  : vx(1,0,0), v(0,0,0), pvx(0,0,0) 
{

  trim = track_maker; 
  ioniz = ioniz_model; 
  current_track = 0; 
}

void dmtpc::mc::mctpc::TRIMGenerator::setPosition(TVector3 pos)
{
  v = pos; 
}

void dmtpc::mc::mctpc::TRIMGenerator::setProjectileDirection(TVector3 dir)
{
  pvx = dir; 
}
void dmtpc::mc::mctpc::TRIMGenerator::setDirection(TVector3 dir)
{
  vx = dir; 
}

int dmtpc::mc::mctpc::TRIMGenerator::doGenerate(dmtpc::analysis::MCTruth * truth) 
{
  truth->recoilEnergy = trim->E(); 
  truth->x0 = v.x(); 
  truth->y0 = v.y(); 
  truth->z0 = v.z(); 

  truth->dx = vx.x(); 
  truth->dy = vx.y(); 
  truth->dz = vx.z(); 

  if (current_track >= trim->ntracks()) return 0; 
  trim->makeTrack(current_track++, &v,&vx); 

  int ne = trim->makeElectrons(ioniz); 
  truth->ionization = trim->getTotalIonization(); 
  truth->nprimary = ne; 
  truth->primary_electron_x.reserve(ne); 
  truth->primary_electron_y.reserve(ne); 
  truth->primary_electron_z.reserve(ne); 

  particles.reserve(ne); 

  for (int i = 0; i < ne; i++)
  {
    Particle p; 
    p.type = Particle::electron; 
    double E,dx,dy,dz; 

    double xd,yd,zd,td;
    trim->getElectron(i, xd,yd,zd,td, E,dx,dy,dz); 

    truth->primary_electron_x.push_back(xd); 
    truth->primary_electron_y.push_back(yd); 
    truth->primary_electron_z.push_back(zd); 
    p.x = xd; 
    p.y = yd; 
    p.z = zd; 
    p.t = td; 

    p.vx = 0; 
    p.vy = 0; 
    p.vz = 0; 
        
    particles.push_back(p); 

  }

  return ne; 

}
