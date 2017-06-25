#ifndef DMTPC_CORE_DETECTOR_CONFIG
#define DMTPC_CORE_DETECTOR_CONFIG


/** The Detector Config class is to store 
 * parameters related to the detector set up
 * (e.g. voltages, pressures, etc.). 
 *
 * Right now it can only store doubles, which may be
 * insufficient for future use. 
 *
 * For now, the implemntation just uses two vectors, so lookup speed 
 * is linear, although this should be suitable for our purposes. 
 *
 */ 
#include "TObject.h" 
#include <vector>
#include <map>
#include "TString.h"

#ifndef __CINT__
#include <tr1/unordered_map>
#endif


namespace dmtpc
{
  namespace core
  {

    class DetectorConfig  : public TObject
    {

      public: 

        enum type
        {
          Voltage, 
          Pressure,
          Temperature,
          Other
        };


        DetectorConfig() { ; } 
        DetectorConfig(int n, double * values, const char ** desc, type * types); 

        /* get the value and type of the value with the description. Returns non-zero if not found. */ 
        int get(const char * desc, double * value, type * type) const;  


        double getValue(const char * desc) const; 

        /** list all the types */
        int list(std::vector<const char *> * fill) const ; 





      private: 

        std::vector<double> values; 
        std::vector<TString> desc; 
        std::vector<type> types; 


        ClassDef(DetectorConfig,1); 
    }; 
  }
}


#endif
