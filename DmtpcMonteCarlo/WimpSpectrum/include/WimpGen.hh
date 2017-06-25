#ifndef __DMTPC_MC_SPECTRUM_WIMPGEN_HH__
#define __DMTPC_MC_SPECTRUM_WIMPGEN_HH__

class TRandom; 
#include "TVector3.h"

namespace dmtpc
{
  namespace mc
  {
    namespace wimpspectrum
    {
      class VelocityDistribution; 
      class WimpGen
      {
        public:
          WimpGen();
          virtual ~WimpGen();

          void setWimpDensity(double wd){fWimpDensity = wd;}
          float getWimpDensity() const{return fWimpDensity;}
        
          void setTotalTargetMass(double tm){fTotalMass = tm;}
          float getTotalTargetMass() const{return fTotalMass;}

          void setCrossSection(double cs){fCS = cs;}
          float getCrossSection() const{return fCS;}

          void setWimpMass(double wm){fWimpMass = wm*1e6;}
          float getWimpMass() const{return fWimpMass;}

          void setTargetMass(double tm){fTargetMass = tm*1e6*0.931494;}
          float getTargetMass() const{return fTargetMass;}

          void setV0(double v0){fV0 = v0;}
          float getV0() const{return fV0;}

          void setVE(double vE){fVE = vE;}
          float getVE() const{return fVE;}

          void setVesc(double vesc){fVesc = vesc;}
          float getVesc() const{return fVesc;}

          void generate();
          void makeTree(long N,const char* file);
          void setWindDirection(const TVector3 * w) { wind = TVector3(*w); }

          float getE() const{return fE;}
          float getTheta() const{return fTheta;}
          float getPhi() const{return fPhi;}
          float getThetaW() const{return fThetaW;}
          float getPhiW() const{return fPhiW;}
          float getVw() const{return fVw;}
          float getEmax() const{return fEmax;}
          float getVgal() const{return fVgal;}
          float getOpening() const { return fOpeningAngle; } 
          float getOpeningW() const { return fWimpOpeningAngle; } 

          double getEventRate(long Ntrials = 1000000) const;

        protected:
          //Properties
          float fCS;
          float fTotalMass;
          float fWimpDensity;
          float fWimpMass;
          float fTargetMass;
          float fV0;
          float fVE;
          float fVesc;
          float fWimpOpeningAngle; 
          TVector3 wind; 

          //Recoil properties
          float fE;
          float fTheta;
          float fPhi;
          float fThetaW;
          float fPhiW;
          float fVw;
          float fEmax;
          float fVgal;
          float fOpeningAngle; 
          float fLabScatteringCosTheta; 
          //Random gen
          TRandom* fRandom;
          VelocityDistribution * dist; 
      };
    }
  }
}

#endif
