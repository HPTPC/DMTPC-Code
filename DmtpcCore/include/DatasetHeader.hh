#ifndef DMTPC_CORE_DATASET_HEADER_HH
#define DMTPC_CORE_DATASET_HEADER_HH

#include "TObject.h" 
#include "TObjString.h"
#include "TTimeStamp.h"

class TH2; 


namespace dmtpc
{
namespace core
{
  class DatasetHeader : public TObject
  {
    public: 

      DatasetHeader(); 
      virtual ~DatasetHeader();

      int ncameras; 
      TTimeStamp * starttime; 
      TTimeStamp * endtime; 
      TObjString config; 


      ClassDef(DatasetHeader,1); 
  }; 
}
}

#endif
