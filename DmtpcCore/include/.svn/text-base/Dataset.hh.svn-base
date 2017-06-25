#ifndef DMTPC_CORE_DATASET_HH
#define DMTPC_CORE_DATASET_HH

/** Raw dataset helper class */

#include <vector>
#include "TTree.h" 

class TH2; 
class TFile; 

namespace dmtpc
{
  namespace core
  {
    class Event; 
    class DatasetHeader; 
    class Image; 
    class Dataset
    {

      public: 
        static const char * getBiasTreeName();
        static const char * getDataTreeName();

        Dataset(const char * file = 0); 
        virtual ~Dataset(); 
        int open(const char * file); 
        int close(); 

        int getEvent(int event_i) { return eventTree->GetEntry(event_i) ; } 
        int getBiasRaw(int bias_i) { return biasTree->GetEntry(bias_i);  } 

        int nevents() const { return eventTree->GetEntries(); } 
        int nbias() const { return biasTree->GetEntries(); } 


        const DatasetHeader * getHeader() const { return header; } 


        /** Get the current event (select events with getEvent) **/ 
        const Event * event() const {return eventPtr; }

        const Image * biasRaw(int icam)  { return biasPtr->at(icam); } 
        const Image * biasOS(int icam)  { return biasOsPtr->at(icam); } 

        /** Get the averaged bias image for a camera. This shouldn't 
         * be used for real calculations in most cases, but is useful for a
         * quick background subtraction **/ 
        const TH2 * biasAvg(int icam);
        const TH2 * biasOSAvg(int icam);
        TTree * getBiasTree() { return biasTree; }
        TTree * getEventTree() { return eventTree; }
        const TFile * getFile() { return file; }

      protected: 
        TTree * biasTree; 
        TTree * eventTree; 
        DatasetHeader *header; 
        TFile * file; 
        Event * eventPtr; //! 
        std::vector<Image*> *biasPtr; //!
        std::vector<Image*> *biasOsPtr; //!
        std::vector<TH2*> bias_avg; //! 
        std::vector<TH2*> bias_os_avg; //! 
        const TH2* computeAvg(int icam, bool os = false); 

        void clearFile(); 


    }; 
  }

}


#endif



