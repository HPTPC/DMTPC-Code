#ifndef DMTPC_MC_AMP_SHAPE_HH
#define DMTPC_MC_AMP_SHAPE_HH


#include "Representation.hh"
#include "TVector3.h" 
#include <vector> 

class GModel; 
class GVertex; 
class GRegion; 
class GFace; 
class GEdge; 
class TPolyLine3D; 


namespace dmtpc
{
  namespace mc
  {
    namespace amp
    {
      class Shape 
      {
        public: 
          virtual int build(GModel * model,bool isBounding=false, std::vector<Shape*> *extant_shapes = 0) = 0; 
          int addToPhysicalVolume(int id); 
          int addToPhysicalSurface(int id); 

          const std::vector<GRegion*> * getVolumes() const { return &vols;}
          const std::vector<std::vector<GFace*> > * getFaceLoops() const { return &faces;}
          const std::vector<GEdge*> * getEdges() const { return &edges;}
          const std::vector<GVertex*> * getPoints() const { return &points;}
          void setElementSize(double s) { size = s;} 
          virtual bool intersectsSegment(double x0,double y0, double z0, double x1, double y1, double z1) const; 
          Representation * getRepr() { return &repr; }

        protected:
          std::vector<GEdge*> edges; 
          std::vector<GVertex*> points; 
          void newFaceLoop() { faces.push_back(std::vector<GFace*>()); }
          std::vector<GFace*>  * newestFaceLoop() { return &faces[faces.size()-1]; }
          std::vector<std::vector<GFace*> > faces;  // face loops
          double size;
          std::vector<GRegion*> vols; 
          Representation repr; 
      };

      class Box  : public Shape
      {
        public:
          Box(TVector3 * center = 0, TVector3* widths = 0, TVector3 * orientation = 0, double rotation = 0); 
          int build(GModel * model,bool bounding, std::vector<Shape*> *extant_shapes); 

        private: 
          TVector3 center; 
          TVector3 widths; 
          TVector3 orientation; 
          double rotation; 
      };

      class Disc  : public Shape
      {
        public:
          Disc(TVector3 * center = 0, double radius=1, double height=1, TVector3 * orientation = 0); 
          int build(GModel * model,bool bounding, std::vector<Shape*> *extant_shapes); 

        private: 
          TVector3 center; 
          TVector3 orientation; 
          double radius; 
          double height; 
      };

      class Ring  : public Shape
      {
        public:
          Ring(TVector3 * center = 0, double inner_radius=1, double outer_radius=2, double height=1, TVector3 * orientation = 0); 
          int build(GModel * model,bool bounding, std::vector<Shape*> *extant_shapes); 

        private: 
          TVector3 center; 
          TVector3 orientation; 
          double inner_radius; 
          double outer_radius; 
          double height; 
      };

      class Torus  : public Shape
      {
        public:
          Torus(TVector3 * center = 0, double big_radius=10, double small_radius=1, TVector3 * orientation = 0); 
          int build(GModel * model,bool bounding, std::vector<Shape*> *extant_shapes); 

        private: 
          TVector3 center; 
          TVector3 orientation; 
          double big_radius; 
          double small_radius; 
          double rotation;
      };

      class Mesh : public Shape
      {
        public: 
          virtual bool intersectsSegment(double x0,double y0, double z0, double x1, double y1, double z1) const; 
          int build(GModel * model,bool bounding, std::vector<Shape*> *extant_shapes){ return 1;} 

        protected:
          Mesh(TVector3 * center, double wire_pitch, double wire_radius, TVector3 * orientation, unsigned nwiresx, unsigned nwiresy , double rotation); 
          TVector3 orientation; 
          TVector3 center; 
          double wire_pitch;
          double wire_radius; 
          int nwiresx; 
          int nwiresy; 
          TVector3 dirx, diry; 
          double rotation;

      }; 

      class WovenMesh : public Mesh
      {
        public:

          WovenMesh(TVector3 * center = 0, double wire_pitch=0.254, double wire_radius=0.03, TVector3 * orientation = 0, unsigned nwiresx = 10, unsigned nwiresy = 10, double rotation = 0, double separation_eps = 0.1, double spline_dist = 1); 
          int build(GModel * model,bool bounding, std::vector<Shape*> *extant_shapes); 
        private: 
          int addBraid(GModel* m, TVector3 start, TVector3 dir, int nperiods,bool parity); 
          double eps;
          double splinedist;
          unsigned nsegs; 
      }; 


      class SimpleMesh : public Mesh
      {
        public:
          SimpleMesh(TVector3 * center = 0, double wire_pitch=0.254, double wire_radius=0.03, TVector3 * orientation = 0, unsigned nwiresx = 10, unsigned nwiresy = 10, double rotation = 0); 
          int build(GModel * model,bool bounding, std::vector<Shape*> *extant_shapes); 
      };


    }

  }
}

#endif
