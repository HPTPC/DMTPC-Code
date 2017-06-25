#include "SolverElmer.hh" 
#include <string.h>
#include "ComponentElmer.hh"
#include <fstream>
#include <istream>
#include <iostream>
#include "Model.hh"
#include <cstdlib>


dmtpc::mc::amp::SolverElmer::SolverElmer(const char * path)
{

  elmerpath = path ? strdup(path) : 0; 
  fm = 0; 
}

dmtpc::mc::amp::SolverElmer::~SolverElmer()
{
  free(elmerpath); 
  if (fm) delete fm; 
}


bool dmtpc::mc::amp::SolverElmer::solved(const Model * m) const
{
  std::string str(TString::Format("%s/soln/%s/%s.result",m->getOutputDir(),m->getSolnName(),m->getId())); 
  std::ifstream ifs (str.c_str()); 
  if (!ifs.good()) 
  {
    std::cout <<str  << " not found, ElmerSolver not yet run for this configuration. " << std::endl;
    return false;
  }


  if (m->electrodesEnabled())
  {
    for (unsigned i = 0; i < m->nElectrodes(); i++)
    {
      std::string strw(TString::Format("%s/soln/%s/%s_wgt_%u.result",m->getOutputDir(),m->getSolnName(),m->getId(),i)); 
      std::ifstream ifsw (strw.c_str()); 
      if(!ifsw.good())
      {
        std::cout <<strw  << " not found, ElmerSolver not yet run for this configuration. " << std::endl;
        return false; 
      }
    }

  }

  return true; 


} 

int dmtpc::mc::amp::SolverElmer::solve(const Model * m)
{
  makeFieldSif(m); 
  if (m->nElectrodes()) makeWeightingSifs(m); 
  
  //convert mesh 


  printf("Running ElmerGrid (output to %s/log/ElmerGrid_%s.log)\n",m->getOutputDir(), m->getSolnName()); 
  if (int ret = system(TString::Format("%s%sElmerGrid 14 2 %s/mesh/%s.msh -out %s/soln/%s -autoclean> %s/log/ElmerGrid_%s.log 2>&1", elmerpath ? elmerpath : "", elmerpath ? "/" : "", m->getOutputDir(), m->getMeshName(), m->getOutputDir(),  m->getSolnName(),m->getOutputDir(),m->getSolnName() ) ) )
  {
    return ret; 
  }

  printf("Running ElmerSolver for field (output to %s/log/ElmerSolver_%s.log)\n", m->getOutputDir(),m->getSolnName()); 
  if (int ret= system(TString::Format("%s%sElmerSolver %s/soln/%s.sif > %s/log/ElmerSolver_%s.log 2>&1", elmerpath ? elmerpath : "", elmerpath ? "/" : "", m->getOutputDir(), m->getSolnName(), m->getOutputDir(), m->getSolnName() ) ) )
  {
    return ret; 
  }

  
  if (m->electrodesEnabled())
  {
    for (unsigned i = 0 ; i < m->nElectrodes(); i++)
    {
      printf("Running ElmerSolver for weighting field %u (output to %s/log/ElmerSolver_%s_wgt_%u.log)\n",i, m->getOutputDir(),m->getSolnName(),i); 
      if (int ret= system(TString::Format("%s%sElmerSolver %s/soln/%s_wgt_%u.sif > %s/log/ElmerSolver_%s_wgt_%u.log 2>&1", elmerpath ? elmerpath : "", elmerpath ? "/" : "", m->getOutputDir(), m->getSolnName(),i, m->getOutputDir(), m->getSolnName(),i ) ) )
      {
        return ret; 
      }
    }
  }

  printf("ElmerSolver Done\n"); 


  makeDielectrics(m); 
  return loadSolution(m); 
}


int dmtpc::mc::amp::SolverElmer::loadSolution(const Model * m)
{
  Garfield::ComponentElmer *em = new Garfield::ComponentElmer(
                            TString::Format("%s/soln/%s/mesh.header",m->getOutputDir(),m->getSolnName()).Data(),
                            TString::Format("%s/soln/%s/mesh.elements",m->getOutputDir(),m->getSolnName()).Data(),
                            TString::Format("%s/soln/%s/mesh.nodes",m->getOutputDir(),m->getSolnName()).Data(),
                            TString::Format("%s/soln/%s/dielectrics.dat",m->getOutputDir(),m->getSolnName()).Data(),
                            TString::Format("%s/soln/%s/%s.result",m->getOutputDir(),m->getSolnName(),m->getId()).Data(),
                            m->getUnit() 
                          ); 


  // if a medium is defined, load it
  const std::vector<VolumeProperties*> * volprops = m->getVolumeProperties(); 
  unsigned idx = 0; 
  for (unsigned i = 0; i < volprops->size(); i++)
  {
    if (volprops->at(i)) 
    {
      if (volprops->at(i)->medium)
        em->SetMedium(idx,volprops->at(i)->medium); 
      idx++; 
    }
  }


  //load weighting fields 


  if (m->electrodesEnabled())
  {
    unsigned surface_index = 0; 
    for (unsigned i = 0; i < m->nElectrodes(); i++)
    {
        char * fname = strdup(TString::Format("%s/soln/%s/%s_wgt_%u.result",m->getOutputDir(),m->getSolnName(),m->getId(), i)); 
        while (!m->getSurfaceProperties()->at(surface_index)->isElectrode) surface_index++; 
        em->SetWeightingField(fname, m->getNames()->at(surface_index++)); 
    }
  }

  fm = em; 

}



int dmtpc::mc::amp::SolverElmer::makeWeightingSifs(const Model * m)
{


  const std::vector<VolumeProperties*> * volprops = m->getVolumeProperties(); 
  const std::vector<SurfaceProperties*> * surfprops = m->getSurfaceProperties() ; 
  const std::vector<const char*> * names = m->getNames(); 


  for (unsigned ii = 0; ii < m->nElectrodes(); ii++)
  {

    char * fname = strdup(TString::Format("%s/soln/%s_wgt_%u.sif",m->getOutputDir(),m->getSolnName(),ii)); 

    printf("Writing weighting sif to %s\n",fname); 

    FILE* f=fopen(fname,"w"); 
    fprintf(f,"!   %s.sif \n",m->getSolnName()); 
    fprintf(f,"!\n"); 
    fprintf(f,"!  Auto generated by MCAmp\n"); 
    fprintf(f,"!\n\n"); 

    fprintf(f,"Check Keywords Warn\n\n"); 

    fprintf(f,"Header\n  Mesh DB \"%s/soln\" \"%s\"\nEnd\n\n",m->getOutputDir(), m->getSolnName()); 

    fprintf(f, "Simulation\n  Coordinate System = Cartesian 3D\n  Simulation Type = Steady State\n"); 
    fprintf(f, "  Steady State Max Iterations = 1\n  Output File = \"%s_wgt_%u.result\"\n  Post File = \"%s_wgt_%u.ep\"\nEnd\n\n", m->getId(),ii,m->getId(),ii); 

    fprintf(f,"Constants\n  Permittivity of Vacuum = 8.8542e-12\nEnd\n\n"); 

    unsigned bodycount = 1; //keep track of bodies and materials = physical volumes
    unsigned surfcount = 1; //keep track of surfaces = physical surfaces
    unsigned boundcount = 1; //keep track of surfaces with potentials

    const std::vector<VolumeProperties*> * volprops = m->getVolumeProperties(); 
    const std::vector<SurfaceProperties*> * surfprops = m->getSurfaceProperties(); 
    const std::vector<const char*> * names = m->getNames(); 
    unsigned electrode_count = 0; 

    for (unsigned i = 0; i < names->size(); i++)
    {
      if (volprops->at(i))
      {
        fprintf(f,"\n\n! VolumeName: %s -> %d\n", names->at(i),bodycount); 
        fprintf(f,"Body %d\n  Equation = 1\n  Material = %d\nEnd\n\n",bodycount,bodycount,bodycount); 
        fprintf(f,"Material %d\n  Relative Permittivity = %f\n  Electric Conductivity = %f\nEnd\n\n",bodycount,volprops->at(i)->epsilon,volprops->at(i)->conductivity); 
        bodycount++; 
      }

      if (surfprops->at(i))
      {
        fprintf(f,"\n\n! SurfName: %s -> %d\n", names->at(i),surfcount); 
        if (surfprops->at(i)->voltageDefined)
        {
          double v = 0; 
          if (surfprops->at(i)->isElectrode)
          {
            if (ii == electrode_count) v = 1;   
            electrode_count++; 
          }

          fprintf(f,"Boundary Condition %d\n  Target Boundaries = %d\n  Potential = %f\nEnd\n\n",boundcount,surfcount,v); 
          boundcount++; 
        }
        surfcount++; 
      }
    }

    fprintf(f,"Equation 1\n  Active Solvers(1) = 1\n\n Calculate Electric Energy = True\nEnd\n\n"); 

    fprintf(f,"Solver 1\n  Equation = Stat Elec Solver\n  Variable = Potential\n  Variable DOFs = 1\n");
    fprintf(f,"  Procedure = \"StatElecSolve\" \"StatElecSolver\"\n  Calculate Electric Field = False\n");  
    fprintf(f,"  Calculate Electric Flux = False\n  Linear System Solver = Iterative\n  Linear System Iterative Method = BiCGStab\n"); 
    fprintf(f,"  Linear System Max Iterations = 1000\n  Linear System Abort Not Converged = True\n  Linear System Convergence Tolerance = 1.0e-10\n"); 
    fprintf(f,"  Linear System Preconditioning = ILU1\n  Steady State Convergence Tolerance = 5.0e-7\n End\n\n");

    fclose(f); 
    free(fname); 

    }
}

int dmtpc::mc::amp::SolverElmer::makeDielectrics(const Model * m)
{
  //count the number of physical volumes
  
  const std::vector<VolumeProperties*> *volprop = m->getVolumeProperties(); 

  unsigned nphys = 0; 

  for (unsigned i = 0; i < volprop->size(); i++)
  {
    if (volprop->at(i)) nphys++; 
  }

  char * fname = strdup(TString::Format("%s/soln/%s/dielectrics.dat",m->getOutputDir(),m->getSolnName())); 
  printf("Writing dielectrics.dat to %s\n",fname); 
  FILE * f = fopen(fname,"w"); 

  fprintf(f,"%u\n", nphys); 

  unsigned idx = 1;
  for (unsigned i = 0; i < volprop->size(); i++)
  {
    if (volprop->at(i)) fprintf(f,"%u %f\n",idx++, volprop->at(i)->conductivity==0 ? volprop->at(i)->epsilon : -volprop->at(i)->conductivity); 
  }

  fclose(f); 
  free(fname); 

}


int dmtpc::mc::amp::SolverElmer::makeFieldSif(const Model * m)
{
  char * fname = strdup(TString::Format("%s/soln/%s.sif",m->getOutputDir(),m->getSolnName())); 
  printf("Writing field sif to %s\n",fname); 
  FILE * f = fopen(fname,"w"); 

  fprintf(f,"!   %s.sif \n",m->getSolnName()); 
  fprintf(f,"!\n"); 
  fprintf(f,"!  Auto generated by MCAmp\n"); 
  fprintf(f,"!\n\n"); 

  fprintf(f,"Check Keywords Warn\n\n"); 

  fprintf(f,"Header\n  Mesh DB \"%s/soln\" \"%s\"\nEnd\n\n",m->getOutputDir(), m->getSolnName()); 

  fprintf(f, "Simulation\n  Coordinate System = Cartesian 3D\n  Simulation Type = Steady State\n"); 
  fprintf(f, "  Steady State Max Iterations = 1\n  Output File = \"%s.result\"\n  Post File = \"%s.ep\"\nEnd\n\n", m->getId(),m->getId()); 

  fprintf(f,"Constants\n  Permittivity of Vacuum = 8.8542e-12\nEnd\n\n"); 

  unsigned bodycount = 1; //keep track of bodies and materials = physical volumes
  unsigned surfcount = 1; //keep track of surfaces = physical surfaces
  unsigned boundcount = 1; //keep track of surfaces with potentials

  const std::vector<VolumeProperties*> * volprops = m->getVolumeProperties(); 
  const std::vector<SurfaceProperties*> * surfprops = m->getSurfaceProperties(); 
  const std::vector<const char*> * names = m->getNames(); 
  for (unsigned i = 0; i < names->size(); i++)
  {
    if (volprops->at(i))
    {
      fprintf(f,"\n\n! VolumeName: %s -> %d\n", names->at(i),bodycount); 
      fprintf(f,"Body %d\n  Equation = 1\n  Material = %d\nEnd\n\n",bodycount,bodycount,bodycount); 
      fprintf(f,"Material %d\n  Relative Permittivity = %f\n  Electric Conductivity = %f\nEnd\n\n",bodycount,volprops->at(i)->epsilon,volprops->at(i)->conductivity); 
      bodycount++; 
    }

    if (surfprops->at(i))
    {
      fprintf(f,"\n\n! SurfName: %s -> %d\n", names->at(i),surfcount); 
      if (surfprops->at(i)->voltageDefined)
      {
        fprintf(f,"Boundary Condition %d\n  Target Boundaries = %d\n  Potential = %f\nEnd\n\n",boundcount,surfcount,surfprops->at(i)->voltage); 
        boundcount++; 
      }
      surfcount++; 
    }
  }

  fprintf(f,"Equation 1\n  Active Solvers(1) = 1\n\n Calculate Electric Energy = True\nEnd\n\n"); 

  fprintf(f,"Solver 1\n  Equation = Stat Elec Solver\n  Variable = Potential\n  Variable DOFs = 1\n");
  fprintf(f,"  Procedure = \"StatElecSolve\" \"StatElecSolver\"\n  Calculate Electric Field = False\n");  
  fprintf(f,"  Calculate Electric Flux = False\n  Linear System Solver = MultiGrid\n  Linear System Preconditioning = None\n"); 
  fprintf(f,"  Linear System Max Iterations = 1000\n  Linear System Abort Not Converged = True\n  Linear System Convergence Tolerance = 1.0e-10\n"); 
  fprintf(f,"  MG Method = Cluster\n  MG Levels = 10\n Steady State Convergence Tolerance = 5.0e-7\n End\n\n");

  fclose(f); 
  free(fname); 
  return 0; 
}


