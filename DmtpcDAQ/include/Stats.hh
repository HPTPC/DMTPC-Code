#ifndef DMTPC_DAQ_STATS_HH
#define DMTPC_DAQ_STATS_HH


namespace dmtpc
{
  namespace core
  {
    class DetectorConfig; 
    class RunConfig;
  }

  namespace control 
  {
    class Store; 
  }

  namespace daq
  {
    class DAQConfig; 
    class CameraHandler; 

    class Stats 
    {

      public: 
      Stats(const dmtpc::core::RunConfig * runCfg, const CameraHandler * ccd_handler); 
        virtual int fillDetectorConfig(dmtpc::core::DetectorConfig * fillme)  {;} 
      private:
        const CameraHandler * ccd_handler; 
        const dmtpc::control::Store * store; 
    }; 

  }
}

#endif
