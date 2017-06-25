#ifndef DMTPC_MC_AMP_REPR_HH
#define DMTPC_MC_AMP_REPR_HH

#include "TObject.h"
#include <vector>
#include "TPolyLine3D.h" 
class GVertex; 

namespace dmtpc
{
  namespace mc
  {
    namespace amp
    {
      class Representation : public TObject 
      {
        public:
          Representation() { ; } 
          void addLine(const GVertex * v1, const GVertex * v2); 
          void addCircleArc(const GVertex * v1,const GVertex * c1, const GVertex * v2); 
          void addBSpline(std::vector<GVertex*> * cpts); 
          void add(TPolyLine3D * addme) { repr.push_back(addme); } 
          void draw(const char * opt); 
          void setLineColor(int color); 
          unsigned n() const { return repr.size(); } 
          const TPolyLine3D * l(int i) const { return repr[i]; } 

        private: 
             std::vector<TPolyLine3D*> repr; 



          ClassDefNV(Representation,1); 


      };

    }
  }
} 


#endif
