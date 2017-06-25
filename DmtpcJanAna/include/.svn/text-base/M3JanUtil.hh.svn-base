#ifndef M3JanUtil_HH
#define M3JanUtil_HH
#include "TString.h"
class TH2I;

namespace dmtpc {
  namespace jana  {
    class M3FrameQA;
    class M3JanUtil      { 
    public:       
      ~M3JanUtil();
      M3JanUtil();
      static void verifyQuadId(int quadId, TString camSN); 
      static TH2I * convert764toOS(TH2I * h2inp, int quadId);
      static void testRicSparkTool(int runId, TString dbName);
    };
  }
}
#endif
