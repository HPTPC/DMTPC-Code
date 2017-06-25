/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * Operations on Wevent2011
 *********************************************************************/

#include <assert.h>
#include <set>
#include "TRandom3.h"
#include "../DmtpcJanEve/include/M3Event.hh"

class TH1F;
class WeventIO : public TObject{
 private:
  TRandom3 rnd;
 public:
  WeventIO(){ rnd.SetSeed(123);}
  ~WeventIO(){}
  void fillEve(M3Event *eve, int k, int runId);
  void unpackEve(M3Event *eve, TH1F *h, int camId);
  std::set<int>  testRicSparkTool(int runId, TString dbName);
  ClassDef(WeventIO,1) 
};

