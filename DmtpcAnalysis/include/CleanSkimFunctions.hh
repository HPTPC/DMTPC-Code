#ifndef DMTPC_ANALYSIS_CLEANSKIM_FUNCTIONS_HH
#define DMTPC_ANALYSIS_CLEANSKIM_FUNCTIONS_HH

class TH2; 
class TH2F; 
class TTree; 
class TObjArray; 

#include <vector>
#include <list>
#include <utility>
#include <sys/stat.h>
#include "SkimEvent.hh"
#include "SkimDataset.hh"
#include "RunQuality.hh"

namespace dmtpc
{
  namespace core
  {
    class Event; 
    class Dataset; 
  }
  namespace image
  {
    class StitchInfo; 
  }
  namespace waveform
  {
    class CspWfVector; 
    class PMTWfVector; 
    class FastWfVector; 
    class WaveformVector;  // wtf is this shit 
  }

  namespace analysis
  {
    class GainMap; 
    class ClusterImage; 
    class Stitcher; 

    namespace cleanskim
    {

       class Config; 

       struct Setup
       {
         const char * datadir; 
	 const char * dbdir;
	 const char * dbname;
         const char * outdir; 
         const char * cfgfile; 
         std::vector<const char*> files; 
         std::vector<std::string> string_container;  //holds strings from parsing file to avoid memory leak
       }; 


       /* class for friendliness. ugly. */ 
       class Functions
       {
         public:

	 static inline bool fexists(const char* name){
	   struct stat buffer;
	   return (stat (name,&buffer) == 0);
	 }

         static int cleanSkim(dmtpc::core::Dataset &d, int runnum, SkimDataset &sd, bool delete_intermediate, const Config * conf, unsigned algo_index, dmtpc::Quality::RunQuality &runQ); 

         static bool checkSpark(int ev, const char * camid, double mean, double last_mean, double os_spark_mean, const Config * conf, dmtpc::Quality::RunQuality &runQ); 

	 //	 static double getMaxHVCurrent(dmtpc::analysis::MySQLRead * mysql, const char * table, TTimeStamp * start, TTimeStamp * end, const Config * conf);

         static int loadGainMaps(GainMap ** gainmaps, SkimDataset *d, int n, const Config * conf, const Stitcher * stitch, std::string * camera_ids);

         static TH2 * cleanImage(const TH2 * raw_img, const TH2 * bias, int & nkilled, double raw_rms, double raw_mean, const TH2 * raw_overscan, double raw_os_mean, double os_bias_mean, const Config * conf); 


         static TH2 * cleanBiasFrame(const TH2 * in, const Config * conf); //done

//         static TH2 * cleanBiasFrameStack(TTree * in, Config * conf); 
         
         static Stitcher * loadStitch(int run, const Config * conf); 


         static bool checkPartialSpark(const TH2 * img, const Config * conf); 


         static void populateSparkRef(const TH2 * raw_img, const TH2 * biases, const Config * conf, std::vector<std::pair<int,int> > * sparkref_running) ; 

         static int fillCspWfVector(dmtpc::core::Dataset & d, dmtpc::waveform::CspWfVector *cspwv, int ich, int nch); 
         static int fillPMTWfVector(dmtpc::core::Dataset & d, dmtpc::waveform::PMTWfVector *pmtwv, int ich, int nch); 
         static int fillFastWfVector(dmtpc::core::Dataset & d, dmtpc::waveform::FastWfVector *fastwv, int ich, int nch); 

         static bool burnin_test( double x_delta, double y_delta, const Config * conf); 

         static int parse_args(int nargs, const char ** args, Setup* setup); 

         static std::vector<BurninEncoded_t> computeTrackBurnin(std::list<std::vector<std::vector<std::pair<double,double> > > > * positions,
                                                                       std::list<std::vector<std::vector<std::vector<BurninEncoded_t> > >* > * tmpburnin, 
                                                                       int position_offset, double x, double y, int cam, int track, const Config * ); 

         // THIS IS THE PLACE TO ADD MOST NEW VARIABLES //
         static void fillTrackInfo(SkimEvent * ev, int u, int v, const Cluster* clust, const Config * conf, const GainMap * gm, const Stitcher * stitch, const char * cam_id, const dmtpc::core::Event * orig_ev); 



         static int clusterFind(ClusterImage * clusti, const GainMap * maps, 
                                const Config * cfg, const dmtpc::image::StitchInfo * stitch, const double * image_rms,
                                const double * blurred_image_rms,  const double * image_mean, const double * blurred_image_means,  int indx, int cam); 


         static void updateGlobalSparkRef(std::vector<std::pair<int,int> > **sparkref_running, std::list<std::vector<std::pair<int,int> > * > * sparkef, int ncamera); 

         static void updateGlobalBurnin( std::list<std::vector<std::vector<std::pair<double,double> > > > * positions, std::list<std::vector<std::vector<std::vector<BurninEncoded_t> > > *> * burnin_temp, 
                                                    std::list<std::vector<std::vector<std::vector<BurninEncoded_t> > > *> * burnin, SkimEvent * tmp_event, const Config * conf, int ncamera, std::vector<std::vector<std::vector<BurninEncoded_t> > > *  this_event_burnin) ; 



         static  TH2 * medianBiasFrameStack(TTree * tree, int frame, const Config * conf, bool os = false); 
       };
    }
  }
}


#endif
