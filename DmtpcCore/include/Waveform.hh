#ifndef DMTPC_CORE_WAVEFORM_HH
#define DMTPC_CORE_WAVEFORM_HH

#include "ScopeInfo.hh"
#include "TObject.h" 
#include <stdint.h>
#include "TH1.h"

namespace dmtpc
{
  namespace core
  {
    class Waveform : public TObject 
    {

      public: 

        Waveform(); 
        virtual ~Waveform(); 
        Waveform(const char * name, const char * title, const void * raw_data, const ScopeChannelInfo * info,  uint32_t secs, uint32_t nsecs); 

        uint32_t GetBinContent(int i) const; 
        double GetPhysicalBinContent(int i) const; 

        // seamlessly convert to TH1 
        operator TH1*() { return physical(); }  //non-const
        operator TH1() { return *physical(); }  //non-const
    //    operator const *TH1() const { return getPhysical(); }  //const
    //    operator const TH1() const { return *getPhysical(); }  //const

        void Draw(const char * opt = 0) const { physical()->Draw(opt); } 
        const TH1D* getPhysical() const { return (const TH1D*) physical(); } 
        const TH1* getRaw() const { return data; } 
        void destroyPhysical() const; 
        char getType() const { return type; }
        const ScopeTraceInfo * getInfo() const { return &trace_info; } 


      protected: 
        //protected since non-const return 
        TH1D* physical() const; 
        TH1* data; 
        mutable TH1D * physical_data; //! 
        ScopeTraceInfo trace_info; 
        double vmin;
        double vmax; 
        char type; //C,S,I; 
        char nbits; 

        ClassDef(Waveform,1); 
    };
  }
}

#endif
