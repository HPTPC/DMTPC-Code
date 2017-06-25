#ifndef PixelTrace_HH
#define PixelTrace_HH

#include "TObjString.h" 

class TH1I;
class TFile;
class TH2I;

namespace dmtpc {
  namespace ped  {
    class PixelTrace  : public TObject     {
    public:       
      ~PixelTrace();
      PixelTrace(){}; // for CINT
      PixelTrace(TString pixName,int mBin, float valErr, int numFrame);
      void saveHisto(TFile *);
      void fillFrom(TH2I *h2Dlight, int frameId);

    private:
      int mBin;
      float valErr;
      TString coreName;
      TH1I *hT;// trace of ADU for this pixel
      ClassDef(PixelTrace,1);       
    }; 
  }
}
#endif
