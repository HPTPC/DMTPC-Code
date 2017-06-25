#ifndef DMTPC_ANALYSIS_MCTRUTH_ALGO_HH
#define DMTPC_ANALYSIS_MCTRUTH_ALGO_HH

class TVector3;
class TRandom; 

class TH1; 
class TF1; 
class TH2; 

namespace dmtpc
{
  namespace math
  {
    class PrincipalComponentFinder;
  }

  namespace analysis
  {
      class GainMap;
      class MCTPCDataset;
      class MCTruth;

      namespace truthalgo 
      {
        
        int maincam(const dmtpc::analysis::MCTruth * truth); 
        bool edge(const dmtpc::analysis::MCTruth * truth, int cam); 
        double getGain(dmtpc::analysis::MCTPCDataset * d, int cam, const char * cut = "", dmtpc::analysis::GainMap * gm = 0, double mindist = 2); 

        dmtpc::math::PrincipalComponentFinder *  pca_electrons(const dmtpc::analysis::MCTruth * truth, dmtpc::math::PrincipalComponentFinder * pca = 0, bool diffused = false); 
        dmtpc::math::PrincipalComponentFinder *  pca_electrons_flat(const dmtpc::analysis::MCTruth * truth, dmtpc::math::PrincipalComponentFinder * pca = 0, bool diffused = false); 
        double pca_hist(const TH2 * hist, double * xmean = 0, double * ymean = 0); 

        double electron_asymmetry(const dmtpc::analysis::MCTruth * truth, const TVector3 * axis, bool diffuse = false, bool flat = false, int npercentile = 0, double *percentile  = 0, int * asymmetries = 0, TF1 * debug = 0, TH1 * debugh = 0); 
        double hist_asymmetry(const TH2 * hist, double phi, int npercentile = 0, double *percentile = 0,  int *asymmetries = 0 , TRandom * rand = 0, double acceptance = 1,  TF1 * debug = 0, TH1 * debug_h = 0); 

        double max_displacement(const dmtpc::analysis::MCTruth * truth, bool flat= true); 


      }
  }
}
      

#endif
