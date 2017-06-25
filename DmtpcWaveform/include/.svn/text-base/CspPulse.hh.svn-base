#ifndef DMTPC_WAVEFORM_CSP_PULSE_HH
#define DMTPC_WAVEFORM_CSP_PULSE_HH

#include "Pulse.hh"

/** Class to hold information about pulses from a fast preamplifier
where a typical nuclear recoil shows both a fast electron peak and
a slower slow peak.
*/
namespace dmtpc
{
namespace waveform
{

class CspPulse : public Pulse
{

  public:

    /** Default constructor.  Sets rise and fall time variables to -1, 
    all else to 0.  
    */
    CspPulse() :
      Pulse(), 
      R0(-1),R10(-1),R25(-1),R50(-1),R75(-1),R90(-1),
      F0(-1),F10(-1),F25(-1),F50(-1),F75(-1),F90(-1){;}


    CspPulse(int nb) : Pulse(nb), 
      R0(-1),R10(-1),R25(-1),R50(-1),R75(-1),R90(-1),
      F0(-1),F10(-1),F25(-1),F50(-1),F75(-1),F90(-1){;}

    CspPulse(const Pulse& p) : Pulse(p), 
      R0(-1),R10(-1),R25(-1),R50(-1),R75(-1),R90(-1),
      F0(-1),F10(-1),F25(-1),F50(-1),F75(-1),F90(-1){;}



    /**Destructor */
    virtual ~CspPulse(){;}

    virtual const char* GetName() const {return "CspPulse";}

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

    ClassDef(CspPulse,1)

};
}
}


#endif
