#include "HEEDGenerator.hh"
#include "TrackHeed.hh"
#include "Medium.hh"
#include "ParticleDistribution.hh"
#include "MCTruth.hh"
#include "SolidBox.hh"
#include "Sensor.hh"
#include "ComponentConstant.hh"
#include "GeometrySimple.hh"



dmtpc::mc::mctpc::HEEDGenerator::HEEDGenerator(Garfield::Sensor * sensor) 
{

  delete_me = 0;

  t = new Garfield::TrackHeed; 
  t->SetSensor(sensor); 
  t->SetEnergy(5900); 
  this->sensor = sensor; 
  

}


static TVector3 default_center(0,0,0); 
static TVector3 default_bounds(1000,1000,1000); 
static TVector3 default_efield(0,0,0); 


struct delete_struct
{
  Garfield::Solid *solid; 
  Garfield::Sensor * sensor; 
  Garfield::ComponentConstant * component; 
  Garfield::GeometrySimple* geo; 

  ~delete_struct()
  {
    delete solid; 
    delete sensor; 
    delete component; 
    delete geo;
  }
};

dmtpc::mc::mctpc::HEEDGenerator::~HEEDGenerator()
{

  if (delete_me) 
  {
    delete (delete_struct*) delete_me;
  }

}
dmtpc::mc::mctpc::HEEDGenerator::HEEDGenerator(Garfield::Medium * m, const TVector3 * center, const TVector3 * bounds, const TVector3 * efield ) 
{
  if (!center) center = &default_center; 
  if (!bounds) bounds = &default_bounds; 
  if (!efield) efield = &default_efield; 

  delete_struct * ds =  new delete_struct;
  delete_me = (void*) ds; 

  ds->solid = new Garfield::SolidBox(center->x(), center->y(), center->z(), bounds->x(), bounds->y(), bounds->z());
  ds->geo = new Garfield::GeometrySimple();
  ds->geo->AddSolid(ds->solid, m);
  ds->component = new Garfield::ComponentConstant; 
  ds->component->SetGeometry(ds->geo); 
  ds->component->SetElectricField(efield->x(), efield->y(), efield->z()); 

  ds->sensor = new Garfield::Sensor; 
  ds->sensor->AddComponent(ds->component); 

  sensor = ds->sensor; 

  t = new Garfield::TrackHeed(); 
  t->SetSensor(ds->sensor); 

  t->SetEnergy(5900); 

}


int dmtpc::mc::mctpc::HEEDGenerator::doGenerate(dmtpc::analysis::MCTruth * truth) 
{

  if (dist) 
  {
    dist->draw(); 

    setPosition(*dist->getPosition()); 
    setDirection(*dist->getDirection()); 
    setProjectileDirection(*dist->getProjectileDirection()); 
    setSourceDirection(*dist->getSourceDirection()); 
    setE(1000*dist->getE()); 
    setTime(dist->getTime()); 
    truth->pE = dist->getProjectileE(); 
    printf("Generating track with E= %f, Position = (%f,%f,%f), Direction = (%f,%f,%f), Time = %f\n", E, v.x(), v.y(), v.z(), vx.x(), vx.y(), vx.z(), time); 
  }

  truth->recoilEnergy = 0; 
  truth->x0 = v.x(); 
  truth->y0 = v.y(); 
  truth->z0 = v.z(); 

  truth->dx = vx.x(); 
  truth->dy = vx.y(); 
  truth->dz = vx.z(); 

  truth->pdx = pvx.x(); 
  truth->pdy = pvx.y(); 
  truth->pdz = pvx.z(); 

  truth->sdx = svx.x(); 
  truth->sdy = svx.y(); 
  truth->sdz = svx.z(); 


  truth->time = time; 
  Garfield::Medium *m; 
  ((Garfield::Sensor*)sensor)->GetMedium(v.x(), v.y(), v.z(), m); 

  truth->pressure = m->GetPressure(); 


  int ne; 
  t->TransportPhoton(v.x(),v.y(),v.z(),0, E*1e-3, vx.x(), vx.y(), vx.z(), ne); 
  truth->ionization = E; 
  truth->nprimary = ne; 
  truth->primary_electron_x.reserve(ne); 
  truth->primary_electron_y.reserve(ne); 
  truth->primary_electron_z.reserve(ne); 
  truth->species = 0; 

  particles.reserve(ne); 

  for (int i = 0; i < ne; i++)
  {
    Particle p; 
    p.type = Particle::electron; 
    double E,dx,dy,dz; 

    double xd,yd,zd,td;
    t->GetElectron(i, xd,yd,zd,td, E,dx,dy,dz); 

    truth->primary_electron_x.push_back(xd); 
    truth->primary_electron_y.push_back(yd); 
    truth->primary_electron_z.push_back(zd); 
    p.x = xd; 
    p.y = yd; 
    p.z = zd; 
    p.t = td; 

    p.vx = dx; 
    p.vy = dy; 
    p.vz = dz; 
        
    particles.push_back(p); 

  }

  return ne; 

}
