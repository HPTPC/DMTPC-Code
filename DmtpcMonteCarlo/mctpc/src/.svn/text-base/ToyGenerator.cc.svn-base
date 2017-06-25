#include "ToyGenerator.hh"
#include "ParticleDistribution.hh"


int dmtpc::mc::mctpc::ToyGenerator::doGenerate(dmtpc::analysis::MCTruth * truth) 
{
    dist->draw(); 
    truth->recoilEnergy = dist->getE(); 
    truth->time = dist->getTime(); 
    truth->pE = dist->getProjectileE(); 
    truth->pM = dist->getProjectileM(); 
    truth->cosThetaCM = dist->getCosThetaCM(); 
    truth->x0 = dist->getPosition()->x(); 
    truth->y0 = dist->getPosition()->y(); 
    truth->z0 = dist->getPosition()->z(); 

    truth->dx = dist->getDirection()->x(); 
    truth->dy = dist->getDirection()->y(); 
    truth->dz = dist->getDirection()->z(); 

    truth->pdx = dist->getProjectileDirection()->x(); 
    truth->pdy = dist->getProjectileDirection()->y(); 
    truth->pdz = dist->getProjectileDirection()->z(); 

    truth->sdx = dist->getSourceDirection()->x(); 
    truth->sdy = dist->getSourceDirection()->y(); 
    truth->sdz = dist->getSourceDirection()->z(); 
    truth->species = species; 

    if (quenching) truth->ionization = quenching(truth->recoilEnergy)* truth->recoilEnergy; 

    return 0; 
}
