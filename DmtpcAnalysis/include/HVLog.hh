#ifndef DMTPC_ANALYSIS_HV_LOG_HH
#define DMTPC_ANALYSIS_HV_LOG_HH

#include <vector>
class TChain; 

/* This class facilitates spark finding using the current */ 

namespace dmtpc
{
  namespace analysis
  {

    class HVLog
    {

      public: 

        HVLog( unsigned n_imon, const double * imon, const double * imon_t,
               unsigned n_vmon, const double * vmon, const double * vmon_t,  
               unsigned n_vset, const double * vset, const double * vset_t); 

        HVLog(const char * file_name, const char * imon_tree, const char * vmon_tree, const char * vset_tree, const char * valname = "value", const char * timename = "time"); 


        double getImon(double  t, double * dt = 0) const;  
        double getMaxImon(double t0, double e1) const;  
        double getAvgImon(double t0, double t1) const;  
        double getVmon(double t, double * dt = 0) const;  
        double getAvgVmon(double t0, double t1) const;  
        double getVset(double t, double * dt = 0) const;  


        /* Build a spark friend tree for a chain of events based on the information in this log. 
         *
         * Note that early versions of the DAQ software had a bug in the timestamp handling such that the first 100 events had the timestamps of the bias frames instead.  
         * A non-zero value of max_run_with_bias_frame_timestamp_bug will try to compensate for that 
         *
         * **/ 
        int makeSparkFriend(TChain *c, const char * outputfile, const char * tree_name = "spark", double t_before = 5,
                            double t_after = 3, bool overwrite=false, int max_run_with_bias_frame_timestamp_bug=0, double extra_padding_for_bug = 1); 
        

      private: 

        std::vector<double>  imon; 
        std::vector<double>  vmon; 
        std::vector<double>  vset; 
        std::vector<double> imon_time; 
        std::vector<double> vmon_time; 
        std::vector<double> vset_time; 

    }; 

  }
}

#endif
