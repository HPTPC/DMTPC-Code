#ifndef M3FrameQA_HH
#define M3FrameQA_HH
#include <set>
class TH1I;
class TFile;


namespace dmtpc {
  namespace jana  {
    class  M3StarSparkDet;
    class M3FrameQA  : public TObject     {
    public:       
      ~M3FrameQA(){};
      M3FrameQA();
      void createStatus(int runIdX, int quadIdX, int mxFrameX);
      void createStatus( dmtpc::jana::M3StarSparkDet *spDet);
      void saveHisto(TFile *);
      int loadFrameQA(int runIdX, int quadIdX,TString fName);  // ret numFrames   
      void maskFrame( int frameId, float val=1);
      int  isBad(int runIdX, int frameId);
      int  isGood(int runIdX, int frameId) {return !isBad(runIdX, frameId);}
      void print();
      void printCSV();
      void producePatcher(TString fName, TString badFrameST);      
      void addTo( std::set<int>  * );

    private:
      int runId, quadId, mxFrame;
      TString coreName, inpFname;
      TH1I *hFrameStat;
      ClassDef(M3FrameQA,1);       
    }; 
  }
}
#endif
