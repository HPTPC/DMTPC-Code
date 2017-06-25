#ifndef DMTPC_ANALYSIS_MCTRUTH_HH
#define DMTPC_ANALYSIS_MCTRUTH_HH

#include <cmath>
#include <vector>
#include "TObject.h"
#include <stdint.h>

class TH2I; 
class TH2; 
class TH1I; 
class TH1S; 


namespace dmtpc
{
  namespace analysis
  {
      class MCTruth 
      {

        public:
          MCTruth(); 

          double phi() { return atan2(dy,dx);} 
          double opening() { return acos( dx * pdx + dy * pdy + dz * pdz); }
          double theta() { return atan2(dz, sqrt(dx*dx + dy*dy)) ; }
          double xcam(double x, double y, int cam = 0);
          double ycam(double x, double y, int cam = 0);
          double x0cam(int cam = 0) ;
          double y0cam(int cam = 0);
          TH2* getTruthImage(int cam = 0) { return (TH2*) truth_images[cam]; } 

          double recoilEnergy; 
          double x0, y0, z0; 
          double time; // unix time + fraction
          double dx,dy,dz;  // recoil direction
          int species;  //Z  of element (He = 2, C = 6, F = 9, 0 = gamma, -1 = electron)

          double pdx,pdy,pdz; // incident particle direction, if any
          double pE; // incident particle energy, if any 
          double cosThetaCM; // center of mass scattering angle, if any  (-2 if not set) 
          double sdx,sdy,sdz; // source direction, if any
          
          double ionization; 

          int nprimary; 
          std::vector<double> primary_electron_x; 
          std::vector<double> primary_electron_y;
          std::vector<double> primary_electron_z;

          std::vector<double> diffused_electron_x; 
          std::vector<double> diffused_electron_y;
          std::vector<double> diffused_electron_z;

          TH2I * avalanche_visible_photon_xy; 
          TH1I * avalanche_visible_photon_t; 

          uint64_t ne_in_avalanche, nphotons_from_avalanche, nvisible_photons_from_avalanche;; 

          std::vector<TH2I*> truth_images; 
          std::vector<TH1S*> truth_traces; 

          //These have to do with the truth images 
          std::vector<int> nphoton; 
          std::vector<int> nadu; 
          std::vector<int> nadu_raw;  //nadu with no gain map 
          std::vector<double> rotation;
          std::vector<double> xoffsets;
          std::vector<double> yoffsets;
          std::vector<double> xscale;
          std::vector<double> yscale;
          std::vector<double> noise; 
          std::vector<double> conversion_gain; 


          double pressure; 
          bool edge; 
          double pM; //incident particle mass

          std::vector<uint32_t> individual_avalanche_gain; 


          void clear(); 

          virtual ~MCTruth() { clear(); } 

          ClassDef(MCTruth,13); 
      };
  }
}

#endif
