#ifndef DMTPC_FAST_PULSE
#define DMTPC_FAST_PULSE

#include "Pulse.hh"

namespace dmtpc
{

namespace waveform
{


/** Class to hold informatslow about pulses from a fast preamplifier
where a typical nuclear recoil shows both a fast electron peak and
a slower slow peak.
*/
class 
FastPulse : public Pulse
{

  public:

    /** Default constructor.  Sets rise and fall time variables to -1, 
    all else to 0.  
    */
    FastPulse() : Pulse(), 
      fastPeak(0), slowPeak(0), fastPeakTime(0), slowPeakTime(0),
      fastPeakBin(0), slowPeakBin(0),
      troughHeight(0), troughTime(0), troughBin(0),
      fR0(-1),fR10(-1),fR25(-1),fR50(-1),fR75(-1),fR90(-1),
      sF0(-1),sF10(-1),sF25(-1),sF50(-1),sF75(-1),sF90(-1),
      R0(-1),R10(-1),R25(-1),R50(-1),R75(-1),R90(-1),
      F0(-1),F10(-1),F25(-1),F50(-1),F75(-1),F90(-1){;}


    FastPulse(int nb) : Pulse(nb),
      fastPeak(0), slowPeak(0), fastPeakTime(0), slowPeakTime(0),
      fastPeakBin(0), slowPeakBin(0),
      troughHeight(0), troughTime(0), troughBin(0),
      fR0(-1),fR10(-1),fR25(-1),fR50(-1),fR75(-1),fR90(-1),
      sF0(-1),sF10(-1),sF25(-1),sF50(-1),sF75(-1),sF90(-1),
      R0(-1),R10(-1),R25(-1),R50(-1),R75(-1),R90(-1),
      F0(-1),F10(-1),F25(-1),F50(-1),F75(-1),F90(-1){;}

    FastPulse(const Pulse& p) : Pulse(p),
      fastPeak(0), slowPeak(0), fastPeakTime(0), slowPeakTime(0),
      fastPeakBin(0), slowPeakBin(0),
      troughHeight(0), troughTime(0), troughBin(0),
      fR0(-1),fR10(-1),fR25(-1),fR50(-1),fR75(-1),fR90(-1),
      sF0(-1),sF10(-1),sF25(-1),sF50(-1),sF75(-1),sF90(-1),
      R0(-1),R10(-1),R25(-1),R50(-1),R75(-1),R90(-1),
      F0(-1),F10(-1),F25(-1),F50(-1),F75(-1),F90(-1){;}



    /**Destructor */
    virtual ~FastPulse(){;}


    virtual const char* GetName()const{return "FastPulse";}

    //Heights, times and bins of fast, slow peaks
    double getFastPeak()const{return fastPeak;}
    double getSlowPeak()const{return slowPeak;}
    double getFastPeakTime()const{return fastPeakTime;}
    double getSlowPeakTime()const{return slowPeakTime;}
    int getFastPeakBin()const{return fastPeakBin;}
    int getSlowPeakBin()const{return slowPeakBin;}
    //Height, time, bin of trough between two peaks
    double getTroughHeight()const{return troughHeight;}
    double getTroughTime()const{return troughTime;}
    int getTroughBin()const{return troughBin;}
    //Rise time to fast peak
    double getFastRise0()const{return fR0;}
    double getFastRise10()const{return fR10;}
    double getFastRise25()const{return fR25;}
    double getFastRise50()const{return fR50;}
    double getFastRise75()const{return fR75;}
    double getFastRise90()const{return fR90;}
    //Fall time from slow peak
    double getSlowFall0()const{return sF0;}
    double getSlowFall10()const{return sF10;}
    double getSlowFall25()const{return sF25;}
    double getSlowFall50()const{return sF50;}
    double getSlowFall75()const{return sF75;}
    double getSlowFall90()const{return sF90;}
    //Rise time to overall peak
    double getRise0()const{return R0;}
    double getRise10()const{return R10;}
    double getRise25()const{return R25;}
    double getRise50()const{return R50;} 
    double getRise75()const{return R75;}
    double getRise90()const{return R90;}
    //Fall time to overall peak
    double getFall0()const{return F0;}
    double getFall10()const{return F10;}
    double getFall25()const{return F25;}
    double getFall50()const{return F50;}
    double getFall75()const{return F75;}
    double getFall90()const{return F90;}


    //Heights, times and bins of fast, slow peaks
    void setFastPeak(double v){fastPeak = v;}
    void setSlowPeak(double v){slowPeak =v;}
    void setFastPeakTime(double v){fastPeakTime = v;}
    void setSlowPeakTime(double v){slowPeakTime = v;}
    void setFastPeakBin(int b){fastPeakBin=b;}
    void setSlowPeakBin(int b){slowPeakBin = b;}
    //Height, time, bin of trough between two peaks
    void setTroughHeight(double v){troughHeight = v;}
    void setTroughTime(double v){troughTime = v;}
    void setTroughBin(int b){troughBin = b;}
    //Rise time to fast peak
    void setFastRise(double* v);  
    void setFastRise0(double v){fR0 = v;}
    void setFastRise10(double v){fR10 = v;}
    void setFastRise25(double v){fR25 = v;}
    void setFastRise50(double v){fR50 = v;}
    void setFastRise75(double v){fR75 = v;}
    void setFastRise90(double v){fR90 = v;}
    //Fall time from slow peak
    void setSlowFall(double* v);
    void setSlowFall0(double v){sF0 = v;}
    void setSlowFall10(double v){sF10 = v;}
    void setSlowFall25(double v){sF25 = v;}
    void setSlowFall50(double v){sF50 = v;}
    void setSlowFall75(double v){sF75 = v;}
    void setSlowFall90(double v){sF90 = v;}
    //Rise time to overall peak
    void setRise(double* v);
    void setRise0(double v){R0=v;}
    void setRise10(double v){R10=v;}
    void setRise25(double v){R25 = v;}
    void setRise50(double v){R50=v;}
    void setRise75(double v){R75=v;}
    void setRise90(double v){R90=v;}
    //Fall time to overall peak
    void setFall(double* v);
    void setFall0(double v){F0=v;}
    void setFall10(double v){F10=v;}
    void setFall25(double v){F25=v;}
    void setFall50(double v){F50=v;}
    void setFall75(double v){F75=v;}
    void setFall90(double v){F90=v;}


  protected: 

    //Heights, times and bins of fast, slow peaks
    double fastPeak;
    double slowPeak;
    double fastPeakTime;
    double slowPeakTime;
    int fastPeakBin;
    int slowPeakBin;
    //Height, time, bin of trough between two peaks
    double troughHeight;
    double troughTime;
    int troughBin;
    //Rise time to fast peak
    double fR0;
    double fR10;
    double fR25;
    double fR50;
    double fR75;
    double fR90;
    //Fall time from slow peak
    double sF0;
    double sF10;
    double sF25;
    double sF50;
    double sF75;
    double sF90;
    //Rise time to overall peak
    double R0;
    double R10;
    double R25;
    double R50; 
    double R75;
    double R90;
    //Fall time to overall peak
    double F0;
    double F10;
    double F25;
    double F50;
    double F75;
    double F90;

    ClassDef(FastPulse,1)

};
}
}


#endif
