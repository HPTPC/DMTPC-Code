#ifndef DMTPC_IMAGE_CLUSTER_FINDING
#define DMTPC_IMAGE_CLUSTER_FINDING

#include <vector>
class TH2; 
#include "ImageProcessing.hh"



namespace dmtpc
{
  namespace image
  {
    class StitchInfo; 
  }
  namespace analysis
  {
    class GainMap; 

    namespace clusterfinding
    {

      /** parameters for gainmap-based track merging algorithm */ 
      struct GMMergeParams
      {
        double min_dist; 
        double min_rxy_global; 
        double min_rxy_cluster; 
        double max_join_residual; 
        double least_squares_weight; 
        double spacer_width; 
        const GainMap * map; 
      };
      
      /** Legacy cluster finding algorithm. 
       *
       * Image is rebinned then blurred using a tophat blur. Bins above minsig above noise but below maxsig and their immediate neigbhors are added to the cluster provided they exceed minsize. 
       * Clusters closer than mindist are merged. 
       *
       */
      int findClustersLegacy(TH2* image, std::vector<std::vector<int> > * clusters, 
           double minsig, double maxsig, int minsize, double mindist);
      

      /** Legacy cluster finding algorithm modified to use gain map for cluster merging. 
       *  
       */
      int findClustersGMLegacy(const TH2* image, std::vector<std::vector<int> > * clusters,
          double minsig, double maxsig, unsigned int minsize, const GMMergeParams * merge_params); 
 


      int findClustersGMHysteresis(const TH2 *image,  std::vector<std::vector<int> > * clusters, 
                               double high_thresh, double low_thresh, 
                               unsigned int min_neighbors, unsigned int minsize, 
                               const GMMergeParams * params, 
                               bool allow_edge_seed = false, bool allow_outliers = true); 

      int findClustersGMSeed(const TH2 * image, std::vector<std::vector<int> > * clust,
                               double seed_thresh, double thresh_pct,
                               double max_wrong_p, double min_thresh,
                               int blur_n, double blur_sigma, 
                               unsigned int neighbors_thresh, int min_neigh, 
                               unsigned int minsize, const GMMergeParams * merge_params, 
                               bool reproduce_v4_bug = true); 
      
      int findClustersGMSeedStitch(const TH2 * image, std::vector<std::vector<int> >* clusters,
                              std::vector<std::vector<int> > * reduced_clusters, 
                              const dmtpc::image::StitchInfo * sinfo, 
                              const double * image_mean, const double * blurred_mean, 
                              const double * image_rms, const double * blurred_rms,  
                              double seed_thresh, double red_thresh, 
                              double thresh_pct,
                              double max_wrong_p, double min_thresh,
                              int blur_n, double blur_sigma, 
                              unsigned int neighbors_thresh, int min_neigh, 
                              unsigned int minsize, const GMMergeParams * merge_params,
                              const char * debug_outfile = 0, bool reproduce_v4_bug = false ); 
 
      int findClustersGMRing( const TH2 * image, std::vector<std::vector<int> > * clusters, std::vector<std::vector<int> > * reduced_clusters,
                             const dmtpc::image::StitchInfo * sinfo, 
                             const double * image_rms, const double * image_means, double space_sigma, 
                             double rms_sigma, double core_thresh_high, double core_thresh_low,  
                             double ring_thresh, double ring_nsigma, unsigned int min_size, dmtpc::image::processing::BILATERAL_VALUE_FN fn, 
                             unsigned ncleanup, const GMMergeParams * merge_params, 
                             const char * debug = 0, int fast_bilateral_scale_exp = 7); 

      int findClustersADHysteresisGM(const TH2 *image, std::vector<std::vector<int> >  * clusters, 
                             double K, double Lambda, dmtpc::image::processing::ANISOTROPIC_DIFFUSION_FN f, int ntimes, double gradient_sigma, 
                             dmtpc::image::processing::GRADIENT_OPERATOR g, double high_thresh, double low_thresh, 
                             unsigned int min_neighbors, unsigned int minsize, 
                             const GMMergeParams * merge_parmas); 


      

//      TH2 * rmsScale(const TH2* image, const dmtpc::image::StitchInfo * info); 

    
    }
      
  }

  
}


#endif
