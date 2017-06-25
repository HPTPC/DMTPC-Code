#ifndef DMTPC_MONTECARLO_AMP_MODEL_HH
#define DMTPC_MONTECARLO_AMP_MODEL_HH


#include "Shape.hh"
#include "UserParam.hh" 
#include "Properties.hh"
#include <vector>
#include "TVector3.h" 
class GModel; 
namespace dmtpc
{
namespace mc
{
namespace amp
{

  class Model  
  {
    public: 
      Model(const char * id ="model", const char * output_dir = ".", const char *unit = "mm"); 
      ~Model(); 

      int add(const char * name, Shape * shape, SurfaceProperties * surf_prop = 0, VolumeProperties * vol_prop = 0, bool bounding = false, bool opaque = false, int color = 1); 
      double setCameraPosition(double x,double y, double z){ campos[0] = x; campos[1] = y; campos[2] = z; } 

      int saveGeo(); 
      bool checkMesh(); 
      bool checkGeo(); 
      int makeMesh(int dim =3, int order =2, bool optimize = false, bool refine = false); 
      const char * getId() const {return id;}
      const char * getOutputDir() const {return outputdir;}

      void addUserParam(const char * name, double value, bool needs_remesh = true, bool needs_resolve = true) { addUserParam(new UserParam(name,value,needs_remesh, needs_resolve)) ; }
      void addUserParam(const char * name, TVector3* value, bool needs_remesh = true, bool needs_resolve = true)  { addUserParam(new UserParam(name,value,needs_remesh, needs_resolve)); } 
      void addUserParam(UserParam* param); 

      void copyFromRemote(const char * remote, const char * copy_cmd = "rsync -avh") const ; 
      void copyToRemote(const char * remote, const char * copy_cmd = "rsync -avh") const; 

      bool occluded(double x, double y, double z) const; 

      const std::vector<Shape*> * getShapes() const { return &shapes;}
      const std::vector<SurfaceProperties*> * getSurfaceProperties() const { return &surfprops;}
      const std::vector<VolumeProperties*> * getVolumeProperties() const { return &volprops;}
      const std::vector<const char*> * getNames() const { return &names;}
      std::vector<UserParam*> * getUserParams()const { return &user_params; }

      unsigned nElements() const { return names.size(); }
      unsigned nElectrodes() const { return nelectrodes; }
      const char * getUnit() const { return unit; } 
      const char * getFullName() const; 
      const char * getMeshName() const; 
      const char * getSolnName() const; 
      
      void enableElectrodes(bool use) { use_electrodes = use; } 
      bool electrodesEnabled() const { return use_electrodes; } 

      void enableIonDrift(bool use) { ion_drift = use; } 
      bool ionDriftEnabled() const { return ion_drift; } 

    private: 
      GModel * m; 
      std::vector<Shape*> shapes; 
      std::vector<SurfaceProperties*> surfprops; 
      std::vector<VolumeProperties*> volprops; 
      std::vector<int> colors; 
      std::vector<bool> bounding; 
      const char * getSuffix() const; 
      std::vector<const char *> names; 
      mutable std::vector<UserParam*> user_params; 
      const char * id;
      const char * unit; 
      const char * outputdir; 
      unsigned nelectrodes; 
      double campos[3]; 
      std::vector<int> opaque; 
      mutable TString fullname; 
      mutable TString meshname; 
      mutable TString solnname; 
      bool isBuilt; 
      bool use_electrodes;
      bool ion_drift;
      void doBuild(); 

  }; 
}
}
}

#endif
