#ifndef DMTPC_MATH_LANDGAUS_HH
#define DMTPC_MATH_LANDGAUS_HH

//from ROOT langaus.C 
//does simple convolution

class TF1; 
class TH1;

namespace dmtpc
{
  namespace math
  {

   //par[0]=Width (scale) parameter of Landau density
   //par[1]=Most Probable (MP, location) parameter of Landau density
   //par[2]=Total area (integral -inf to inf, normalization constant)
   //par[3]=Width (sigma) of convoluted Gaussian function
   
    double landgaus(double * x, double * par); 
    TF1 * makeLandGausTF1(const char * name, double min, double max); 
    TF1 * doLandGausFit(TH1 * hist, const char * fitopt = "LME"); 

  }
}


#endif
