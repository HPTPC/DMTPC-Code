#ifndef __DMTPC_MC_WIMPSPECTRUM_VELOCITY_DIST_HH
#define __DMTPC_MC_WIMPSPECTRUM_VELOCITY_DIST_HH

class TVector3; 


namespace dmtpc
{
  namespace mc
  {
    namespace wimpspectrum
    {
      class VelocityDistribution
      {

        public: 
          /* Fills vector with a velocity in the galactic frame according to this distribution */ 
          //by default, this uses accept reject to pick something from f using g as bounding distribution. 
          virtual void v(TVector3 * v, const TVector3 *vE); 
          virtual ~VelocityDistribution() { ; }

        protected:
          virtual double f(double v) const = 0; 
          double vEsc; 

          //bounding function used for accept reject. Must bound vf(v). 
          virtual double g(double v, double vE) const = 0; 
          bool gDependsOnv; 
      }; 


      class MaxwellBoltzmannDistribution : public VelocityDistribution
      {

        public:
          MaxwellBoltzmannDistribution(double vesc = 2300, double v0 = 230); 
          virtual ~MaxwellBoltzmannDistribution() {;}

        private:
          double v02inv; 
          double v0; 
          virtual double f(double v) const; 
          virtual double g(double v, double vE) const; 

      };

    }
  }
}


#endif
