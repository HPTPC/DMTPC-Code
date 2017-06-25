#include "MultiTRIMGenerator.hh" 
#include "CollisionReader.hh"
#include "TrackMaker.hh"
#include "TRandom.h"
#include "ParticleDistribution.hh" 
#include "IonizationModel.hh"
#include "TMath.h"
#include <cstdio>
#include <dirent.h>
#include <sys/types.h>



dmtpc::mc::mctpc::MultiTRIMGenerator * dmtpc::mc::mctpc::MultiTRIMGenerator::makeFromDir(const char * collision_dir, dmtpc::mc::retrim::IonizationModel * ioniz_model, int nsrimtables, 
                                                                                         const dmtpc::mc::retrim::TableReader** tables, double srim_fract, int species, double P)
{

  DIR * d; 
  struct dirent *dir; 

  printf("Making Tracks from %s\n", collision_dir); 
  d = opendir(collision_dir); 

  std::vector<dmtpc::mc::retrim::TrackMaker*> makers; 

  if (d) 
  {
    while ((dir = readdir(d))) 
    {
      if (dir->d_type == DT_REG && strstr(dir->d_name, "COLLISON"))
      {
        dmtpc::mc::retrim::CollisionReader * c = new dmtpc::mc::retrim::CollisionReader(TString::Format("%s/%s",collision_dir,dir->d_name)); 
        makers.push_back(new dmtpc::mc::retrim::TrackMaker(c, nsrimtables, tables, srim_fract)); 
      }
    }

    closedir(d); 
  }

  return new MultiTRIMGenerator(makers.size(), &(makers[0]), ioniz_model, species, P); 
}


dmtpc::mc::mctpc::MultiTRIMGenerator::MultiTRIMGenerator(int n, dmtpc::mc::retrim::TrackMaker ** track_maker, dmtpc::mc::retrim::IonizationModel * ioniz_model, int species, double P)
  : vx(1,0,0), v(0,0,0), species(species)
{

  pressure = P; 
  std::vector<double> Es; 
  std::vector<int> indices(n); 
  current_trim = 0; 

  for (int i = 0; i < n; i++)
  {
    trim.push_back(track_maker[i]); 
    Es.push_back(track_maker[i]->E()); 
    current_track.push_back(0); 
  }

  TMath::Sort(n, &(Es[0]), &(indices[0]), false); 

  std::vector<double> bins(n+1); 

  bins[0] = 0; //no underflow

  for (int i = 1; i < n; i++)
  {
    bins[i] = 0.5 *( Es[indices[i]] + Es[indices[i-1]]); 
  }

  bins[n] = Es[indices[n-1]]; 

  energy_indices.SetBins(n, &bins[0]); 

  for (int i = 1; i <= n;i++)
  {
    energy_indices.SetBinContent(i, indices[i-1]); 
  }


  ioniz = ioniz_model; 
  current_trim = 0; 
  dist = 0; 
}

void dmtpc::mc::mctpc::MultiTRIMGenerator::setE(double E)
{
  int bin = energy_indices.FindFixBin(E); 
  //avoid over/underflow
  if (bin == 0) bin++; 
  if (bin > energy_indices.GetNbinsX()) bin--; 
  current_trim =  (int) energy_indices.GetBinContent(bin); 
}

void dmtpc::mc::mctpc::MultiTRIMGenerator::setPosition(TVector3 pos)
{

  v = pos; 

}

void dmtpc::mc::mctpc::MultiTRIMGenerator::setProjectileDirection(TVector3 dir)
{
  pvx = dir; 
}

void dmtpc::mc::mctpc::MultiTRIMGenerator::setSourceDirection(TVector3 dir)
{
  svx = dir; 
}
void dmtpc::mc::mctpc::MultiTRIMGenerator::setDirection(TVector3 dir)
{
  vx = dir; 
}

int dmtpc::mc::mctpc::MultiTRIMGenerator::doGenerate(dmtpc::analysis::MCTruth * truth) 
{

  if (dist) 
  {
    dist->draw(); 

    setPosition(*dist->getPosition()); 
    setDirection(*dist->getDirection()); 
    setProjectileDirection(*dist->getProjectileDirection()); 
    setSourceDirection(*dist->getSourceDirection()); 
    setE(dist->getE()); 
    setTime(dist->getTime()); 
    truth->pE = dist->getProjectileE(); 
    printf("Generating track with E= %f, Position = (%f,%f,%f), Direction = (%f,%f,%f), Time = %f\n", trim[current_trim]->E(), v.x(), v.y(), v.z(), vx.x(), vx.y(), vx.z(), time); 
  }

  truth->species = species; 
  truth->recoilEnergy = trim[current_trim]->E(); 
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
  truth->pressure = pressure; 

  current_track[current_trim] = gRandom->Integer(trim[current_trim]->ntracks()); 
  trim[current_trim]->makeTrack(current_track[current_trim]++, &v,&vx); 

  int ne = trim[current_trim]->makeElectrons(ioniz); 
  truth->ionization = trim[current_trim]->getTotalIonization(); 
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
    trim[current_trim]->getElectron(i, xd,yd,zd,td, E,dx,dy,dz); 

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
