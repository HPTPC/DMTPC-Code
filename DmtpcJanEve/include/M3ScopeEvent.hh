//*-- Author : Jan Balewski, MIT
//*-- February 2016
#ifndef M3ScopeEvent_HH
#define M3ScopeEvent_HH
#include <assert.h>
#include <TObject.h>
#include <TTimeStamp.h>

#include "M3Event.hh"

//---------------
class M3ScopePulse : public TObject { 
 public:
  Int_t id; // assigned during reco
  Float_t ampl, raise50, timeInFrame; //(mV), (ns), (sec)
  M3ScopePulse();
  ~M3ScopePulse(){};
  void clear();
  void print(int flag=0);

 private:
  ClassDef(M3ScopePulse,1);
};


//---------------
//---------------
//---------------
class M3ScopeEvent : public TObject { 
 public:
  Int_t runId;// elements not changing withn a run
  Int_t frameId;// elements changing with each frame
  TTimeStamp timeStamp;
  std::vector <M3ScopePulse> pulseV[M3Event::mxCam];
   
  M3ScopeEvent();
  ~M3ScopeEvent(){};
  void clear() { clearHeader();clearFrame(); }
  void clearHeader();
  void clearFrame();
 
  void print(int flag=0);

 private:
  ClassDef(M3ScopeEvent,1);
};


#endif
