#ifndef DMTPC_WAVEFORM_PMT_WAVEFORM
#define DMTPC_WAVEFORM_PMT_WAVEFORM

#include "TObject.h"
#include <vector>
#include <stdint.h>

namespace dmtpc
{
namespace waveform
{

class PMTPulse;

class PMTWaveform : public TObject
{

  public:
    PMTWaveform(int n=0);
    PMTWaveform(const PMTWaveform& w);
    virtual ~PMTWaveform(){;}

    void add(const PMTPulse& p);
    void insert(int i, const PMTPulse& p);
    void swap(int i, const PMTPulse& p);
    void rm(int i);
    void clearPulse();
    void clear();
    void resize(int i);

    int size() const {return N;}

    uint32_t getTimeSecs() const {return secs;}
    uint32_t getTimeNsecs() const { return nsecs;}

    double getBase() const {return base;}
    double getRMS() const {return rms;}
    double getWfMax() const {return wfMax;}
    double getWfMaxTime() const {return wfMaxTime;}
    int getWfMaxBin() const {return wfMaxBin;}
    double getWfMin() const {return wfMin;}
    double getWfMinTime() const {return wfMinTime;}
    int getWfMinBin() const {return wfMinBin;}

    void setTimeSecs(uint32_t x) {secs = x;}
    void setTimeNsecs(uint32_t x) {nsecs = x;}

    void setBase(double x){base = x;}
    void setRMS(double x){rms=x;}
    void setWfMax(double x){wfMax=x;}
    void setWfMaxTime(double x){wfMaxTime=x;}
    void setWfMaxBin(int x){wfMaxBin=x;}
    void setWfMin(double x){wfMin=x;}
    void setWfMinTime(double x){wfMinTime=x;}
    void setWfMinBin(int x){wfMinBin=x;}

    PMTWaveform& operator=(const PMTWaveform& w);

    const PMTPulse& at(int i) const;
    const PMTPulse& operator()(int i) const;
    const PMTPulse& operator[](int i) const;
    PMTPulse& at(int i);
    PMTPulse& operator()(int i);
    PMTPulse& operator[](int i);
    

  protected:
    int N;
    std::vector<PMTPulse> pulse;
    uint32_t secs;
    uint32_t nsecs;
    double base;
    double rms;
    double wfMax;
    double wfMaxTime;
    int wfMaxBin;
    double wfMin; 
    double wfMinTime; 
    int wfMinBin;

    ClassDef(PMTWaveform,1)
};
}
}


#endif
