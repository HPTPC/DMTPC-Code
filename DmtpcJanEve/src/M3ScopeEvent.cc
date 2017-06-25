#include "M3ScopeEvent.hh"

ClassImp(M3ScopePulse)
//==================================
M3ScopePulse::M3ScopePulse(){ clear();  } // 

//==================================
void  M3ScopePulse::clear() { 
  id=0;
  ampl=  raise50=timeInFrame=0;
}

//==================================
void  M3ScopePulse::print(int flag) {
  printf("    pulse id=%d  ampl=%.1f (mV), R50=%.3e(ns), delT=%.2f (sec)\n",id, ampl,  raise50, timeInFrame);
}


//==================================
//==================================
//==================================

ClassImp(M3ScopeEvent)
//==================================
M3ScopeEvent::M3ScopeEvent(){ clear();  } // 

//==================================
void  M3ScopeEvent::clearHeader() { 
 runId=0;
}

//==================================
void  M3ScopeEvent::clearFrame() { // frame elements
 frameId=-1;
 timeStamp = TTimeStamp(0,0);
 for(int ca=0; ca<M3Event::mxCam; ca++)
   pulseV[ca].clear();
}
//==================================
void  M3ScopeEvent::print(int flag) {
  printf("m3scopeEve dump : frameId=%d,  runId=%d,  cam:nPulse  ", frameId, runId);

  for(int ca=0; ca<M3Event::mxCam; ca++) 
    printf("%d:%d  ",ca,pulseV[ca].size());
  printf("\n  recorded at: ");
  //off timeStamp.Print();
  if(flag<=0) return;
  for(int ca=0; ca<M3Event::mxCam; ca++){ 
    if(pulseV[ca].size()<=0) continue;
    printf("  cam%d pulses:\n",ca);
    for( int ip=0;ip<pulseV[ca].size(); ip++)
      pulseV[ca][ip].print(flag-1);
  }
	 
}


