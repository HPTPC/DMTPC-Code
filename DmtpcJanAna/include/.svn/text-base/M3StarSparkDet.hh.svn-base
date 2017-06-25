#ifndef M3StarSparkDet_HH
#define M3StarSparkDet_HH

class TH1I;
class TFile;

#include "../DmtpcJanEve/include/M3Event.hh"

namespace dmtpc {
  namespace core  {    class Event; class Dataset;}  
  namespace jana  {
    class M3StarSparkDet  : public TObject     {
    public: 
      int runId,  quadId, mxFrame;  // to share with others      
      ~M3StarSparkDet(){};
      M3StarSparkDet();
      void initDims(const dmtpc::core::Dataset *ds , int quadId, int camDaq);
      int  getQuadId() { return quadId;} 
      void evalQuad(const dmtpc::core::Event * eve );      
      int  isFrameBad() { return nHotPix >par_nHotPixL; }
      void print();
      void clear();
      TH1I *hHotPix;

    private:
      TString coreName, camSN;
      int camDaq;
      int nHotPix;
      TH2I * h2GarbCol; // garbage collector for re-formated images

      float par_aduH;
      float par_nHotPixL;
      ClassDef(M3StarSparkDet,1);       
    }; 
  }
}
#endif
