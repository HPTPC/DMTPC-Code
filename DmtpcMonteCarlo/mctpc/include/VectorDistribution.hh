#ifndef DMTPC_MC_MCTPC_VECTOR_DISTRIBUTION_HH
#define DMTPC_MC_MCTPC_VECTOR_DISTRIBUTION_HH

#include "TRandom3.h"
#include "TVector3.h"

namespace dmtpc
{
  namespace mc
  {
    namespace mctpc
    {

      class VectorDistribution
      {

        public: 
          TVector3 * fillVector(TVector3 * fillme = 0);
          void fillVector(double *x=0, double*y=0, double *z=0);
          void setSeed(long seed) {rng.SetSeed(seed); } 
          virtual void setTime(double time) { (void) time; }  //time is in seconds in unix epoch, most vector distributions are time-independent
          virtual ~VectorDistribution() {;}
          
        protected:
          double x,y,z; 
          virtual void makeVector() = 0;  
          TRandom3 rng; 

      };

      class FixedVectorDistribution : public VectorDistribution
      {
        public:

        FixedVectorDistribution(double xx, double yy, double zz) 
        { x = xx; y = yy; z = zz;  }

        FixedVectorDistribution(const TVector3 * vec) 
        { x = vec->x(); y = vec->y(), z = vec->z();  }

        protected:
          virtual void makeVector() {;} 
      }; 



      class IsotropicVectorDistribution : public VectorDistribution
      {

        public:
          IsotropicVectorDistribution(double r  = 1) : r(r) {;} 

        protected:
          virtual void makeVector(); 
       
        private: 
          double r; 
      }; 


      class CygnusVectorDistribution : public VectorDistribution
      {

        public: 
          // For detector axis, x = north, y = east, z = up 
          CygnusVectorDistribution(double latitude, double longitude, const TVector3 *detector_z_axis, const TVector3 * detector_x_axis) 
          : latitude(latitude), longitude(longitude), z_axis(detector_z_axis), x_axis(detector_x_axis)  {; } 
          virtual void setTime(double t) {time = t;}  // unix time

        protected:
          virtual void makeVector(); 
          
        private:
          double latitude; 
          double longitude; 
          const TVector3  *z_axis; 
          const TVector3  *x_axis; 
          double time; 

      }; 


      class CollimatedDistribution : public VectorDistribution
      {
        public: 
          CollimatedDistribution(const TVector3 * dir, double coll)  : direction(dir), collimation_angle (coll) { ; } 

        protected:
          virtual void makeVector(); 
        private: 
          const TVector3 * direction; 
          double collimation_angle; 

      }; 

      class UniformCylinder : public VectorDistribution
      {
        public:
          UniformCylinder(double x0, double y0, double r, double z0, double z1) 
            : x0(x0), y0(y0),r(r),z0(z0),z1(z1) {};

        protected:
          virtual void makeVector(); 
        private:
          double x0,y0,r,z0,z1; 
      }; 


      class UniformBox : public VectorDistribution
      {
        public:
          UniformBox(double x0,double x1, double y0, double y1, double z0, double z1) 
            : x0(x0), x1(x1), y0(y0),y1(y1),z0(z0),z1(z1) {};

        protected:
          virtual void makeVector(); 
        private:
          double x0,x1,y0,y1,z0,z1; 
      }; 



    }
  }
}


#endif
