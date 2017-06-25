#ifndef DMTPC_ANALYSIS_CLEAN_SKIM_CONFIG_HH
#define DMTPC_ANALYSIS_CLEAN_SKIM_CONFIG_HH

/* CleanSkimConfig.hh 
  
   author: Cosmin Deaconu <cozzyd@mit.edu> 
   
   This file (and the associated source file) are responsible
   for parsing the configuration file for cleanSkim and providing
   cleanSkim with the configuration information. 
*/

#include "Config.hh" 

namespace dmtpc
{
  namespace analysis
  {
    namespace cleanskim
    {


      typedef enum
      {
      BIAS_CLEAN_TRADITIONAL, 
      BIAS_CLEAN_MEDIAN_STACK 
      }bias_clean_method_t; 


      typedef enum
      {
      IMAGE_CLEAN_TRADITIONAL, 
      IMAGE_CLEAN_BIAS_SUBTRACT_FIRST, 
      IMAGE_CLEAN_MEDIAN_DIFFERENCE 
      }image_clean_method_t; 


      typedef enum
      {
      IMAGE_RATIO_MEAN, 
      IMAGE_RATIO_MEDIAN 

      }image_ratio_method_t; 

      typedef enum 
      {
        HIST_SAVE_TYPE_FLOAT, 
        HIST_SAVE_TYPE_SHORT, 
        HIST_SAVE_TYPE_INT
      }hist_save_type_t; 



      class Config :  public dmtpc::core::Config
      {
        public:
          Config();
          Config(const char *f);
        private:
          void setDefaults(); 

      };
    }
  }
}

#endif

