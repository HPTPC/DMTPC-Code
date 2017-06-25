#ifndef DMTPC_MC_MCTPC_MCAMP_AVALANCHE_CACHE_HH
#define DMTPC_MC_MCTPC_MCAMP_AVALANCHE_CACHE_HH

#include <vector>
class TChain;
#include "TTree.h"
class TH2; 
class TH1; 
class TFile; 

namespace dmtpc
{
  namespace mc
  {
    namespace mctpc
    {

      class MCAmpAvalancheCache
      {

        public: 

          /* Create a cache of a sampled avalanche from the chain c, with cut
           * cut applied to the results * The cache consists of histogrammed
           * versions of the x-y distributions of the avalanche. The
           * half-length of the avalanches is size and the binsize is the size
           * of each bin (both in mm). 
           *
           * The cache will be automagically created inside the cachedir, which
           * will be created if necessary. The cache will be loaded
           * automatically if it has been previously generated with the same
           * settings (same files in TChain in same order, same cut, same size,
           * same binsize). 
           *
           * The file name will be stored in the form of an md5sum and an
           * incrementing integer to handle the rare case of collisions.  */ 

          struct Params
          {
            double size; //half-size of histogram cache
            double bin_size; //bin size of histogram cache
            double max_t; //maximum t value
            double tbin; //tbin
           

            //all of the avalanches are such that they start in the center of a mesh hole, which will be at 0,0
            // To estimate these things, the following are necessary: 
            double top_mesh_pitch;
            double top_mesh_offset_x;  //ofset defined such that an offset of zeo means the mesh hole center is at zero
            double top_mesh_offset_y; 
            int nzbins; 
            double ztop; 
            double zbot; 
            int from_photon;

            Params()
            {
              size = 2; 
              bin_size = 0.01; 
              max_t = 10; 
              tbin = 0.1 ;
              top_mesh_pitch = 0.254; 
              top_mesh_offset_x = 0; 
              top_mesh_offset_y = 0; 
              nzbins = 50; 
              zbot = -1; 
              ztop = 1; 
              from_photon = 1; 
            }

          };
          


          MCAmpAvalancheCache(TChain *c, const char * cut, Params &p, 
                                     const char* cachedir = "cache"); 

          /*The number of avalanches in the cache */ 
          int n() const { return cache_tree->GetEntries(); } 
          /*Returns the ith avalanche in the cache, or if i is negative, gRandom is used to generate one */ 
          int drawAvalanche(int i = -1); 
          TH2 * xyhist() { return h; } 
          void xyhist_rand(double *x, double *y) const; 
          TH2 * xy_visible_hist() { return hvis; } 
          double avg() const { return avg_gain; }
          TH1 * thist() { return t; } 
          double thist_rand() const; 
          TH1 * zhist() { return z; } 
          double zhist_rand() const;
          double pitch() const { return mesh_pitch; } 

        private:

          double avg_gain; 
          double mesh_pitch; 
          TTree * cache_tree; 
          TH2 * h; 
          TH2 * hvis; 
          TH1 * t; 
          TH1 * z; 
          TFile * f; 
          std::vector<double>* h_int; 
          std::vector<double>* t_int; 
          std::vector<double>* z_int; 

      }; 


    }
  }
}



#endif
