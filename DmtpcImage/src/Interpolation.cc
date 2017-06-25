#include "Interpolation.hh" 
#include "TH2.h"
#include <iostream>

static double bicubicFunction(double x, double * params)
{
  double * a = params+1;  //hack to use same notation as in wikipedia :)
  return 0.5 * (2*a[0] + x*(-a[-1] + a[1]) + x*x*(2*a[-1]-5*a[0]+4*a[1] -a[2]) + x*x*x*(-a[-1] + 3*a[0] - 3*a[1] + a[2])); 
}


static double interpolateBicubic(const TH2* in, double x, double y)
{

  double xmin = in->GetXaxis()->GetXmin(); 
  double xmax = in->GetXaxis()->GetXmax(); 
  double ymin = in->GetYaxis()->GetXmin(); 
  double ymax = in->GetYaxis()->GetXmax(); 
  double xwidth = (xmax-xmin) / (in->GetNbinsX()); 
  double ywidth = (ymax-ymin) / (in->GetNbinsY()); 

  //If at an edge, use bilinear 
  if (x < xmin+1.5*xwidth || x > xmax - 1.5*xwidth 
     || y < ymin + 1.5*ywidth || y > ymax - 1.5*ywidth)
  {
    return ((TH2*)in)->Interpolate(x,y); 
  }
  
  int binx0 = (int) ((x - xmin) / xwidth + 0.5); 
  int biny0 = (int) ((y - ymin) / ywidth + 0.5); 


  double b[4]; 
  for (int iy = -1; iy <3; iy++)
  {
    double a[4]; 
    for (int ix = -1; ix <3; ix++)
    {
      a[ix+1] = in->GetBinContent(binx0 + ix, biny0 + iy); 
    }
    b[iy+1] = bicubicFunction( (x - in->GetXaxis()->GetBinCenter(binx0)) / xwidth,a); 
  }
  
  return bicubicFunction((y - in->GetYaxis()->GetBinCenter(biny0)) / ywidth,b);  

}





double dmtpc::image::interpolation::interpolate(const TH2* in, double x, double y, const char * method)
{

    if (!strcasecmp(method,"bilinear")) 
    {
      return ((TH2*)in)->Interpolate(x,y); 
    }
    else if (!strcasecmp(method,"bicubic"))
    {
      return interpolateBicubic(in, x, y); 
    }
    else if (!strcasecmp(method,"nearest"))
    {

      #if ROOT_VERSION_CODE >= ROOT_VERSION(5,28,0)
         return in->GetBinContent(in->FindFixBin(x,y)); 
      #else
         return in->GetBinContent(((TH2*)in)->FindBin(x,y)); 
      #endif

    }
    std::cerr << "Bad interpolation method: " << method << std::endl; 
    return 0; 
}


bool dmtpc::image::interpolation::testInterpolate(const char * method)
{
  return !(strcasecmp(method,"bilinear")
         && strcasecmp(method,"bicubic")
         && strcasecmp(method,"nearest")); 
}


