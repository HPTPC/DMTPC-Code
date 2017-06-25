/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2016
 *********************************************************************
 * Descripion:
 * simultaneous analysis of CCD & Scope  frames 
 *********************************************************************/

#include <assert.h>
#include "TString.h"

#include "../DmtpcJanEve/include/M3ScopeEvent.hh"
class M3Event;

class TH1F;
class TFile;

class AnaM3EveBlue {
 public:
  void setM3Eve( M3Event *x) {m3Eve=x;}
  AnaM3EveBlue();
  ~AnaM3EveBlue(){};
  void readScopeData(TString x, int runId);
  int  matchScopeFrame(); // true for match
  void printEve(){ m3Eve->print(); scEve->print();}
  void analyze();
  void initHisto( int quadid);
  void saveHisto(TFile *);
  void setPulseCuts(float x,float y,float z) { 
    par_pulseTimeL=x;  par_pulseTimeH=y;  par_pulseAmplL=z;
  }
    
  void setClusterCuts(float y, float z, float u) { 
    cut_clustLightL=y; cut_clustDiagPixL=z; cut_pcaRatioH=u;}
  void clear();

 private:
  M3Event *m3Eve;  
  M3ScopeEvent *scEve;
  std::vector <M3ScopeEvent> scEveV;
  int lastSCEidx;
  int quadId;
  const static int mxH=64;
  TH1 *hA[mxH]; //
  TString coreName;

  
  std::vector < M3ScopePulse *> myPulseV;
  std::vector <CcdCluster *> myClustV;// working variable , reset for every event
  void evalScopeFrameSolo();
  void evalCamFrameSolo();

  // cuts -scope
  float par_pulseTimeL, par_pulseTimeH ; // (sec)  
  float par_pulseAmplL; // (V)

  //cuts ccd cluster
  float  cut_clustLightL, cut_clustDiagPixL, cut_pcaRatioH;
      
};

