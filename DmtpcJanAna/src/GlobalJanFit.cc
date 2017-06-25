#include <assert.h>
#include <math.h>
#include "../DmtpcJanEve/include/M3Event.hh"
#include "GlobalJanFit.hh"

std::vector <CcdBin> *ccdBinVp=0;
TH2F *hUtil2p=0, *hUtil3p=0;

//================================================
void chi2Func(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag){

  /* The convention used by MINUIT is the following:
     - IFLAG=1 when FCN is called for the first time.
     - IFLAG=2 if you have specified the option to calculate GRAD,
     the first derivatives (optional)
     - IFLAG=3 call to FCN when the fit is finished.
     - IFLAG=4 normal calls to FCN during the minimization process
     The Minuit documentation at page 9 shows an example of FCN
     with tests on IFLAG=1,2 or 3. IFLAG=4 is not explicitly quoted,
     but from the simple example, on can deduce that this is the 
     normal call.
  */

  assert(ccdBinVp);

  double sum=0;
  for(int i=0; i< ccdBinVp->size(); i++) { 
    CcdBin &bin=ccdBinVp->at(i);
    double valFit=fitFunc(bin.posPix,par);
    double del=(valFit-bin.light)/bin.sigLight; 
    double del2=del*del;
    sum+=del2; 
    if(iflag==3) hUtil3p->SetBinContent(bin.mBin,valFit);
    //if(iflag==1) hUtil2p->Fill(bin.posPix.X(),bin.posPix.Y(),valFit);
    //if(iflag==3) hUtil3p->Fill(bin.posPix.X(),bin.posPix.Y(),valFit);
    // hUtil2p->Fill(bin.posPix.X(),bin.posPix.Y(),valFit);
    // if(iflag==3) printf(" fitFunc inp UV=%.1f,%.1f val=%.1f func=%.1f del=%.1 mBin=%d\n",bin.posPix.X(),bin.posPix.Y(),bin.light,valFit,del,bin.mBin);
  }
  f=sum;
}

//================================================
Double_t fitFunc(TVector2 &pos, Double_t *par){
  //printf(" fitFunc inp UV=%.2f,%.1f\n",pos.X(),pos.Y());

  TVector2 L(par[0],par[1]);
  TVector2 RmL=(TVector2(par[3],par[4])-L);
  double d=RmL.Mod();
  TVector2 uni=RmL.Unit();
  //printf("uni: %.2f %.2f phi/deg=%.1f d=%.1f\n",uni.X(),uni.Y(),uni.Phi()/3.1416*180,d);

  double sig=par[3];
  TVector2 rpos=(pos-L);
  //printf("rpos: %.2f %.2f phi/deg=%.1f mag=%.1f sig=%.1f\n",rpos.X(),rpos.Y(),rpos.Phi()/3.1416*180,rpos.Mod(),par[2]);

  double u=rpos*uni;
  double v2=rpos.Mod2()-u*u;
  double ampL=par[5]*par[5];
  double ampR=par[6]*par[6];
  double sig2=par[2]*par[2]/2.;
  double ampl=0;

  if(u<=0) {
    ampl=ampL*exp(- (u*u+v2)/sig2);
  } else if (u <=d) {
    double eps=u/d;
    ampl=(ampR*eps + ampL*(1.-eps) ) *  exp(- v2/sig2);   
  } else {
    double xx=u-d;
    ampl=ampR*exp(- (xx*xx+v2)/sig2);
    //   
  } 

  // printf("u=%.1f |v|=%.1f  ampl=%.1e\n",u,sqrt(v2),ampl);
  // hUtil2p->Fill(pos.X(),pos.Y(),ampl);
  return ampl;
}

