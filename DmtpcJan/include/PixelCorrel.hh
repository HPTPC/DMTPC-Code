#ifndef PixelCorrel_HH
#define PixelCorrel_HH

#include "TObjString.h" 

class TH1I;
class TFile;
class TH2I;

namespace dmtpc {
  namespace ped  {
    class PixelCorrel  : public TObject     {
    public:       
      ~PixelCorrel(){};
      PixelCorrel(){}; // for CINT
      PixelCorrel(TString pixName,int bin1, float err1, int bin2, float err2);
      void saveHisto(TFile *);
      void fillFrom(TH2I *h2Dlight);

    private:
      int mBin1, mBin2;
      float valErr1,valErr2 ;
      TString coreName;
      TH2I *hC;//  correlation between pixels
      ClassDef(PixelCorrel,1);       
    }; 
  }
}
#endif
