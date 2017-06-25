#ifndef M3CcdImageUtil_HH
#define M3CcdImageUtil_HH

#include "TObjString.h" 

class TH1F;
class TH2I;
class TH1;
class TFile;
class TCanvas;

namespace dmtpc {
  namespace core  {    class Event; class Dataset;}
  namespace ped  {
    class M3CcdImageUtil  : public TObject     {
    public:       
      ~M3CcdImageUtil();
      M3CcdImageUtil();
      // retro-fixing old data formats
      static TH2I *  convert764toOS(TH2I * h2raw, int camId);
      ClassDef(M3CcdImageUtil,1);       
    }; 
  }
}
#endif
