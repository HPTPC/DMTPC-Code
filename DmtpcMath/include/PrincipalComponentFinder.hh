#ifndef DMTPC_MATH_PRINCIPAL_COMPONENT_FINDER_HH
#define DMTPC_MATH_PRINCIPAL_COMPONENT_FINDER_HH

#include <vector>
#include "TVector3.h"

namespace dmtpc
{
  namespace math
  {
    class PrincipalComponentFinder
    {
      public:
        PrincipalComponentFinder (int ndim, bool fast=false);  
        void addPoint(const double *x); 
        void reserve (unsigned t); 
        void setPointsInDim(int dim, int n, const double *x);
        void setPointInDim(int dim, int i, double x);
        void setX(int n, const double *x) { setPointsInDim(0,n,x); }
        void setY(int n, const double *y) { setPointsInDim(1,n,y); }
        void setZ(int n, const double *z) { setPointsInDim(2,n,z); }
        void clearPoints(); 
        unsigned ndim() const { return means.size(); } 
        bool isSolved() const { return solved; } 

        int solve(); 
        double * getComponent(int component = 0, double * dest = 0); 
        double getWeight(int component = 0); 
        double getMean(int dim = 0); 

        TVector3 asVec(int component = 0) const; 
        double asPhi(int component) const; 


      private:
        std::vector<std::vector<double> > points; 
        std::vector<std::vector<double> > components; 
        std::vector<double> means; 
        std::vector<double> weights; 
        int solveFast(); 
        bool solved; 
        bool fast; 

    }; 
  }
}

#endif
