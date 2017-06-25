#ifndef __MOLLWEIDE_HH__
#define __MOLLWEIDE_HH__
#include <string>
#include <vector>
class TH2;
class TH2Poly;
class TLine;
class TF1;
class TTree;
class TMultiGraph;
namespace dmtpc
{
  namespace mc
  {
    namespace wimpspectrum
    {
      class Mollweide 
      {
        public:
          Mollweide();
          virtual ~Mollweide();

          void setBinning(int n){fBinning = n;}

          void projection(const char* fn,const char* tname, const char* theta,const char* phi,const char* opt="");
          void projection(TTree* tr,const char* theta, const char* phi,const char* opt="");
      //    void project(int n,double* theta, double* phi);

          void clear();

          void setNpts(int npt){fNpts = npt;}
          void setTolerance(double t){fTol=t;}
          void setScale(double s){fScale = s;}

          void setName(const char* name){fName = name;}
          void setNormalize(bool n){fNorm = n;}

          void Draw(int nlon=6,int nlat=3);//every 30 deg
          TH2Poly* getProj(){return fPoly;}
          const TH2Poly* getProj() const {return fPoly;}
          TH2* getHist(){return fHist;}
          const TH2* getHist() const {return fHist;}

          void coords(double theta, double phi,double& x, double& y);
          double binsize(double xmin, double xmax, double ymin, double ymax);

        private:  
          
          void makePoly();
          std::string fName;
          int fBinning;
          int fNpts;
          double fScale;
          double fTol;
          bool fNorm;
          TH2* fHist;
          TH2Poly* fPoly;
          TMultiGraph* fMulti;
      //    TLine* fPrimeMeridian;
      //    std::vector<TF1*> fFunVec;
      };

    }
  }
}

#endif
