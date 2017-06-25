#ifndef DMTPC_TRACK_FITTER_HH
#define DMTPC_TRACK_FITTER_HH



#include "TF2.h"
#include "TH2.h"
#include "TCanvas.h"
#include "Math/Minimizer.h"
#include "Math/Functor.h"
#include "Math/WrappedFunction.h"
#include "Math/AdaptiveIntegratorMultiDim.h"
#include "SkimEvent.hh"
#include "TCanvas.h"
#include "TArrow.h"
#include <vector>

namespace dmtpc
{
  namespace physics
  {
    class SRIMTable; 
  }

  namespace analysis
  {
      class TrackFitter
      {
        public: 

        static const size_t NPAR = 9; 
        static const int PAR_E = 0; 
        static const int PAR_X0 = 1; 
        static const int PAR_Y0 = 2; 
        static const int PAR_PHI = 3; 
        static const int PAR_Z0 = 4; 
        static const int PAR_DELTA_Z = 5; 
        static const int PAR_SIGMA2 = 6; 
        static const int PAR_SECOND_WIDTH = 7; 
        static const int PAR_SECOND_WIDTH_RATIO = 8; 


          enum PARTICLE
          {
            ALPHA,
            FLUORINE,
            CARBON  
          }; 

          struct Param
          {
            Param(){;} 
            double val; 
            double err; 
            double errUp; 
            double errDn; 

            void zeroErr() {errUp=0;errDn=0;} 
            void fixErr() { if(errUp==0 || isnan(errUp)){errUp = err;}  if(errDn==0 || isnan(errDn)) {errDn = -err;} } 
            ClassDefNV(Param,1); 
          }; 


          struct Result
          {
            Result() {;}
            Param phi; 
            Param htphi; 
            Param fitE; 
            Param E; 
            Param Ephys; 
            Param range; 
            Param sigma; 
            Param sigma2; 
            Param x0; 
            Param y0; 
            Param z0; 
            Param z1; 
            Param delta_z; 

            double chisq; 
            double gain; 
            double likelyIntegralSame; 
            double likelyIntegralSameError; 
            double likelyIntegralTotal; 
            double likelyIntegralTotalError; 
            double prob;
            double probError; 
            double probFast; 
            int ndof; 
            bool success; 
            bool flip; 
            bool minos; 
            bool integralProb; 
            bool likelihoodProb; 
            double alt_chisq; 
            double prob2; 
            double covar[NPAR][NPAR]; 

            Param second_width; 
            Param second_width_ratio; 
            Param unused; // for ABI compatibility... shitty I know

            bool second_width_is_cauchy; 

            ClassDefNV(Result,4); 
          }; 

          TrackFitter( dmtpc::physics::SRIMTable * sr ,  
                          double rangecal=0.16 /* mm/px */,
                          double gain = 18 /* keV/adu */,
                          const TH2 * gainmap=0,
                          double min_gain = 0.2,
                          double read_noise = 10, 
                          double conversion_gain = 1. 
                          );  

          virtual ~TrackFitter(); 

          int fit(const TH2 * img, const std::vector<int> * pix, double phi_guess = DBL_MAX, double range_guess = DBL_MAX, double sigma_guess = DBL_MAX); 
          int fit(const SkimEvent * ev, int c, int t); 

          void setMinimizationMaxCalls(int ncalls){ min->SetMaxFunctionCalls(ncalls); }
          void setIntegrationAbsTol(double val) { integral_abstol = val; } 
          void setIntegrationRelTol(double val) { integral_reltol = val; } 
          void setIntegrationNCalls(int val) { integral_ncalls = val; } 
          void enableCauchy(bool enable) { enable_cauchy = enable; } 
          void enableSecondGaus(bool enable) { enable_second_gaus = enable; } 
          void enableLikelihoodFlip(bool enable) { likelihood_flip = enable; }

          void setUseGainMapError(bool val) { use_gainmap_error = val; } 
          void setClusterPadding(int val) { padding = val; } 
          void setZeroOutsideCluster(bool val) { zero_out = val; } 
          void setDraw(bool val = true) { draw = val; } 
          void setVerbose(bool val = true) { verbose = val; } 
          void setDoIntegral(bool val = true) { do_integral = val; } 
          void setDoLikelihood(bool val = true) { do_likelihood = val; } 
          void setAlwaysDoMinos(bool val = true) { always_minos = val; } 
          void setMinosThresh(double val) { minos_thresh = val; } 
          void setForceDecreasing(bool force = true) { force_decreasing = force; } 
          void setIntegraNSigma(double val) { integral_nsigma = val; } 
          const Result * getResult() const {return result; }  
          void setQuickEval(bool q) { quickeval = q; } 
          TCanvas * getCanvas() {return canvas; } 

          double SRIMLineFit2DFn(const double * xx) const; 
          const ROOT::Math::Minimizer * getMinimizer() const { return min; } 


          private: 
            void fillResultParams(bool from_alt=false); 
            ROOT::Math::Minimizer *min; 
            ROOT::Math::Functor minfn; 
            double gain; 
            double lengthcal; 
            bool use_gainmap_error; 
            const TH2 * gainMap; 
            TH2 * gainMap_blurred; 
            double min_gain;
            Result *result; 
            mutable TF2* fitfn; 
            mutable ROOT::Math::WrappedMultiFunction<TF2&> * wrapped_fitfn; 
            ROOT::Math::AdaptiveIntegratorMultiDim * fit_integrator; 
            dmtpc::physics::SRIMTable *srim; 
            TCanvas * canvas; 
            double minheight; 
            double maxheight; 
            double binwidth; 
            double read_noise; 
            TH2* fithist; 
            TH2* local_gainmap; 
            TH2* local_gainmap_e; 
            int padding; 
            bool force_decreasing; 
            bool zero_out; 
            bool verbose; 
            bool draw; 
            bool do_integral; 
            bool do_likelihood; 
            bool likelihood_flip; 
            double integral_abstol; 
            double integral_reltol; 
            double integral_nsigma; 
            int integral_ncalls; 
            bool always_minos; 
            double minos_thresh; 
            double conversion_gain; 
            TArrow * arrow; 
            unsigned fitter_id; 
            bool quickeval; 
            bool enable_cauchy; 
            bool enable_second_gaus; 

            ClassDef(TrackFitter,0); 
      };

  }
}





#endif
