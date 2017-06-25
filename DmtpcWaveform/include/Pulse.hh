#ifndef DMTPC_WAVEFORM_PULSE_HH
#define DMTPC_WAVEFORM_PULSE_HH

#include <ostream>
#include <iostream>
#include "TObject.h"

namespace dmtpc
{
namespace waveform
{

class Pulse : public TObject
{

  public: 

    Pulse() : 
      init(false),
      nbin(0),
      peak(0),
      peakTime(0),
      startTime(0),
      startBin(0),
      endTime(0),
      endBin(0),
      integral(0){;}
    virtual ~Pulse(){;}
    Pulse(int nb) : 
      init(true),
      nbin(nb),
      peak(0),
      peakTime(0),
      startTime(0),
      startBin(0),
      endTime(0),
      endBin(0),
      integral(0){;}


    virtual const char* GetName()const{return "Pulse";}
    const char* getName() const {return GetName();}

    /** the bin in which the pulse maximum occurs */
    int getBin()const { return nbin; }
    int getPeakBin() const {return nbin;}  

    /** calculated as the maximum of pulse, minus the baseline*/
    void setPulseHeight(double value) { peak=value; }
    double getPulseHeight()const { return peak; }
    void setPeak(double x){peak=x;}
    double getPeak() const {return peak;}


    /** calculated as the center of the time bin in which the pulse height occurs*/
    void setPulseHeightTime(double value) { peakTime=value; }
    double getPulseHeightTime()const { return peakTime; }
    void setPeakTime(double value) { peakTime=value; }
    double getPeakTime()const { return peakTime; }

    // time center of first bin in pulse 
    void setPulseStartTime(double value) { startTime=value; }
    double getPulseStartTime() const{ return startTime; }
    void setStartTime(double value) { startTime=value; }
    double getStartTime() const{ return startTime; }

    // first bin of pulse 
    void setPulseStartBin(double value) { startBin=value; }
    double getPulseStartBin()const { return startBin; }
    void setStartBin(double value) { startBin=value; }
    double getStartBin()const { return startBin; }

    // time center of last bin of pulse 
    void setPulseEndTime(double value) { endTime=value; }
    double getPulseEndTime()const { return endTime; }
    void setEndTime(double value) { endTime=value; }
    double getEndTime()const { return endTime; }

    // last bin of pulse 
    void setPulseEndBin(double value) { endBin=value; }
    double getPulseEndBin()const { return endBin; }
    void setEndBin(double value) { endBin=value; }
    double getEndBin()const { return endBin; }

    // integral of pulse 
    void setPulseIntegral(double value) { integral=value; }
    double getPulseIntegral()const { return integral; }
    void setIntegral(double value) { integral=value; }
    double getIntegral()const { return integral; }

    /** prints quantities for the pulse */
    virtual void print(ostream & out = ::std::cout);

    protected:

      bool init; 

      int nbin;
      double peak;
      double peakTime;
      double startTime;
      double startBin;
      double endTime;
      double endBin;
      double integral;
   
    ClassDef(Pulse,1); 
    
};
}
}


#endif

