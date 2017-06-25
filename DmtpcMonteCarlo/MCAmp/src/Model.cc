#include "Model.hh"
#include <gmsh/GModel.h> 
#include <gmsh/Context.h> 
#include <cstdlib>
#include "TFile.h" 
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <istream>
#include <fstream>
#include <gmsh/Gmsh.h>

#define EXTERNAL_MESH

dmtpc::mc::amp::Model::Model(const char * mid, const char * dir, const char * unit)
: id(mid), outputdir(dir), unit(unit)
{
  isBuilt = false; 
  GmshInitialize(0,0); 
  m = 0; 
  GmshSetOption("General","Terminal",1.); 
  nelectrodes = 0; 
  mkdir(outputdir,0755); 
  mkdir(TString::Format("%s/geo",outputdir),0755); 
  mkdir(TString::Format("%s/log",outputdir),0755); 
  mkdir(TString::Format("%s/mesh",outputdir),0755); 
  mkdir(TString::Format("%s/soln",outputdir),0755); 
  mkdir(TString::Format("%s/aval",outputdir),0755); 
//  GmshSetOption("General","Verbosity",30.); 

  campos[0] = 0; 
  campos[0] = 0; 
  campos[2] = 1000; 

  use_electrodes = true; 
  ion_drift = false; 
}

bool dmtpc::mc::amp::Model::checkGeo()
{
  std::string str(TString::Format("%s/geo/%s.geo",outputdir,getMeshName())); 
  std::ifstream ifs (str.c_str()); 
  return ifs.good(); 
}

bool dmtpc::mc::amp::Model::checkMesh()
{
  std::string str(TString::Format("%s/mesh/%s.msh",outputdir,getMeshName())); 
  std::ifstream ifs (str.c_str()); 
  return ifs.good(); 

}

dmtpc::mc::amp::Model::~Model()
{
  if (m) delete m; 
  GmshFinalize(); 
}


bool dmtpc::mc::amp::Model::occluded(double x, double y, double z) const
{
  for (unsigned i = 0; i < opaque.size(); i++)
  {
    if (shapes[opaque[i]]->intersectsSegment(x,y,z, campos[0],campos[1], campos[2])) 
      return true; 
  }
  return false; 
}


void dmtpc::mc::amp::Model::doBuild()
{
  if (isBuilt)
  {
    fprintf(stderr,"BUILDING GEOMETRY AGAIN WHEN ALREADY BUILT!!!! ABORTING.\n"); 
    return;

  }
  m= new GModel; 
  m->setFactory("Gmsh"); 

  for (unsigned i = 0; i < shapes.size(); i++)
  {

    Shape * shape = shapes[i]; 
    VolumeProperties * vol_prop = volprops[i]; 
    SurfaceProperties * surf_prop = surfprops[i]; 
    const char * name = names[i]; 
    std::vector<Shape*> extant(shapes.begin(), shapes.begin()+i); 
    
    shape->build(m,bounding[i], &extant);
    if (surf_prop) 
    {
      int id = m->setPhysicalName(std::string(TString::Format("%s_surf",name)), 2); 
      shape->addToPhysicalSurface(id); 
    }
    
    if (vol_prop)
    {
      int id = m->setPhysicalName(std::string(TString::Format("%s_vol",name)), 3); 
      shape->addToPhysicalVolume(id); 
    }

  }

  isBuilt = true; 

}


int dmtpc::mc::amp::Model::add(const char * name, Shape * shape, SurfaceProperties* surf_prop, VolumeProperties * vol_prop, bool isBounding, bool opacity, int color)
{
  if (opacity) 
  {
    opaque.push_back(shapes.size()); 
  }

  colors.push_back(color); 
  shapes.push_back(shape); 
  surfprops.push_back(surf_prop); 
  if (surf_prop && surf_prop->isElectrode) nelectrodes++;
  volprops.push_back(vol_prop); 
  names.push_back(name); 
  bounding.push_back(isBounding); 
}

int dmtpc::mc::amp::Model::saveGeo()
{

  if (!isBuilt) doBuild(); 

  TFile f(TString::Format("%s/geo/%s.root",outputdir,getMeshName()),"RECREATE"); 
  for (unsigned i = 0; i < shapes.size(); i++)
  {
    shapes[i]->getRepr()->setLineColor(colors[i]); 
    shapes[i]->getRepr()->Write(names[i]); 
  }
  f.Close(); 

  std::string str(TString::Format("%s/geo/%s.geo",outputdir,getMeshName())); 
  printf("Writing %s\n",str.c_str());
  return m->writeGEO(str); 
}

int dmtpc::mc::amp::Model::makeMesh(int dim, int order, bool optimize, bool refine)
{
  if (!isBuilt) doBuild(); 
  const char * fname = getMeshName();  
#ifdef EXTERNAL_MESH
  saveGeo(); 
  printf("Meshing... (see %s/log/mesh_%s.log for details)\n",outputdir,fname);
  int ret =  system(TString::Format("gmsh -%d %s/geo/%s.geo -order %d %s -o %s/mesh/%s.msh > %s/log/mesh_%s.log 2>&1", dim, outputdir, fname, order, optimize ? "-optimize" : "", outputdir, fname, outputdir, fname)); 
//  if (refine && !ret) ret = system(TString::Format("gmsh -refine -order %d %s/%s.msh >> %s/%s_mesh.log 2>&1", order, outputdir, id, outputdir, id)); 

  if (refine) printf("refine not supported yet\n"); 
  return ret; 
#else
  printf("Meshing... \n");
  //something wrong here

  CTX::instance()->mesh.order = order; 
  CTX::instance()->mesh.optimize = optimize; 
  m->mesh(dim); 
  std::string str(TString::Format("%s/mesh/%s.msh",outputdir,fname)); 
  printf("Writing %s\n",str.c_str());
  return  m->writeMSH(str); 
#endif


}


void dmtpc::mc::amp::Model::addUserParam(UserParam * param)
{

  //check for duplicate

  for (unsigned i = 0; i < user_params.size(); i++)
  {
    if (!strcmp(user_params[i]->getName(),param->getName()))
    {
      user_params[i] = param; 
      return; 
    }
  }
  user_params.push_back(param); 
}


const char * dmtpc::mc::amp::Model::getMeshName() const 
{
  meshname = id; 

  for (unsigned i = 0; i < user_params.size(); i++)
  {
    if (! user_params[i]->needsRemesh()) continue; 
    meshname += TString::Format("_%s=%s",user_params[i]->getName(),user_params[i]->toString()); 
  }

  return meshname.Data(); 
}

const char * dmtpc::mc::amp::Model::getSolnName() const 
{
  solnname = id; 

  for (unsigned i = 0; i < user_params.size(); i++)
  {
    if (! user_params[i]->needsReSolve()) continue; 
    solnname += TString::Format("_%s=%s",user_params[i]->getName(),user_params[i]->toString()); 
  }

  solnname = solnname.ReplaceAll(".","o"); 
  return solnname.Data(); 
}





const char * dmtpc::mc::amp::Model::getFullName() const 
{
  fullname = id; 

  for (unsigned i = 0; i < user_params.size(); i++)
  {
    fullname += TString::Format("_%s=%s",user_params[i]->getName(),user_params[i]->toString()); 
  }

  return fullname.Data(); 
}

void dmtpc::mc::amp::Model::copyFromRemote(const char * remote, const char * copy_cmd) const
{
  system(TString::Format("mkdir -p %s/geo", getOutputDir())); 
  system(TString::Format("%s %s/%s/geo/%s.geo %s/geo/", copy_cmd, remote, getOutputDir(), getMeshName(), getOutputDir() )); 
  system(TString::Format("mkdir -p %s/mesh", getOutputDir())); 
  system(TString::Format("%s %s/%s/mesh/%s.msh %s/mesh/", copy_cmd, remote, getOutputDir(), getMeshName(), getOutputDir() )); 
  system(TString::Format("mkdir -p %s/soln", getOutputDir())); 
  system(TString::Format("%s %s/%s/soln/%s %s/soln/", copy_cmd, remote, getOutputDir(), getSolnName(), getOutputDir())); 
}

void dmtpc::mc::amp::Model::copyToRemote(const char * remote, const char * copy_cmd) const
{
  system(TString::Format("%s %s %s/", copy_cmd,  getOutputDir(), remote )); 
}







