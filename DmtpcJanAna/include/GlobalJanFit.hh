#ifndef GLOBAL_JAN_FIT_HH
#define GLOBAL_JAN_FIT_HH
// global variables used by Minuit to fit M3-tracks

#include <TVector2.h>


extern std::vector <CcdBin> *ccdBinVp;
extern TH2F *hUtil2p, *hUtil3p; // only monitoring

void dumpFitPar();
Double_t fitFunc(TVector2 &posUV, Double_t *par);
void chi2Func(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag);

#endif
