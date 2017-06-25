#include "LandGaus.hh"

// from ROOT langaus.C 

#include "TMath.h"
#include "TF1.h" 
#include "TH1.h"

double dmtpc::math::landgaus(Double_t *x, Double_t *par) {

   //Fit parameters:
   //par[0]=Width (scale) parameter of Landau density
   //par[1]=Most Probable (MP, location) parameter of Landau density
   //par[2]=Total area (integral -inf to inf, normalization constant)
   //par[3]=Width (sigma) of convoluted Gaussian function
   //
   //In the Landau distribution (represented by the CERNLIB approximation), 
   //the maximum is located at x=-0.22278298 with the location parameter=0.
   //This shift is corrected within this function, so that the actual
   //maximum is identical to the MP parameter.

      // Numeric constants
      Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
      Double_t mpshift  = -0.22278298;       // Landau maximum location

      // Control constants
      Double_t np = 10000.0;      // number of convolution steps
      Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas

      // Variables
      Double_t xx;
      Double_t mpc;
      Double_t fland;
      Double_t sum = 0.0;
      Double_t xlow,xupp;
      Double_t step;
      Double_t i;


      // MP shift correction
      mpc = par[1] - mpshift * par[0]; 

      // Range of convolution integral
      xlow = x[0] - sc * par[3];
      xupp = x[0] + sc * par[3];

      step = (xupp-xlow) / np;

      // Convolution integral of Landau and Gaussian by sum
      for(i=1.0; i<=np/2; i++) {
         xx = xlow + (i-.5) * step;
         fland = TMath::Landau(xx,mpc,par[0]) / par[0];
         sum += fland * TMath::Gaus(x[0],xx,par[3]);

         xx = xupp - (i-.5) * step;
         fland = TMath::Landau(xx,mpc,par[0]) / par[0];
         sum += fland * TMath::Gaus(x[0],xx,par[3]);
      }

      return (par[2] * step * sum * invsq2pi / par[3]);
}



TF1 * dmtpc::math::doLandGausFit(TH1 * hist , const char * fitopt)
{

  TF1 * f = makeLandGausTF1(TString::Format("landgausfit_of_%s", hist->GetName()), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax()); 
  f->SetParameter(0, hist->GetRMS()/2); 
  f->SetParameter(1, hist->GetMean()); 
  f->SetParameter(2, hist->Integral()); 
  f->SetParameter(3, hist->GetRMS()/2); 
  hist->Fit(f,fitopt); 
  return f; 
}

TF1 * dmtpc::math::makeLandGausTF1(const char * name, double min, double max) 
{
  TF1 * f = new TF1(name, dmtpc::math::landgaus, min, max,4); 
  f->SetParNames("landau_width", "mpv","A","gaussian_sigma"); 
  return f; 
}
