#include "ConvolvedSegment.hh" 
#include <assert.h>
#include "Geometry.hh" 
#include <cmath>
#include "TMath.h"

double dmtpc::math::integralOfLineSegmentConvolvedWithGaussian(double y0, double y1, double * par) 
{

 double m = par[0]; 
 double b = par[1]; 
 double x0 = par[2]; 
 double x1 = par[3]; 
 double w = par[4]; 

 //thank Mathematica CForm for this 
 return (sqrt(2/M_PI)*w*(2*b*(-exp(-pow(x0 - y0,2)/(2.*pow(w,2))) + exp(-pow(x1 - y0,2)/(2.*pow(w,2))) + 
         exp(-pow(x0 - y1,2)/(2.*pow(w,2))) - exp(-pow(x1 - y1,2)/(2.*pow(w,2)))) + 
         m*(-((x0 + y0)/exp(pow(x0 - y0,2)/(2.*pow(w,2)))) + (x1 + y0)/exp(pow(x1 - y0,2)/(2.*pow(w,2))) + 
        (x0 + y1)/exp(pow(x0 - y1,2)/(2.*pow(w,2))) - (x1 + y1)/exp(pow(x1 - y1,2)/(2.*pow(w,2))))) + 
        (2*b*(-x0 + y0) + m*(pow(w,2) - pow(x0,2) + pow(y0,2)))*TMath::Erf((x0 - y0)/(sqrt(2)*w)) - 
        (2*b*(-x1 + y0) + m*(pow(w,2) - pow(x1,2) + pow(y0,2)))*TMath::Erf((x1 - y0)/(sqrt(2)*w)) - 
        (2*b*(-x0 + y1) + m*(pow(w,2) - pow(x0,2) + pow(y1,2)))*TMath::Erf((x0 - y1)/(sqrt(2)*w)) + 
        (2*b*(-x1 + y1) + m*(pow(w,2) - pow(x1,2) + pow(y1,2)))*TMath::Erf((x1 - y1)/(sqrt(2)*w)))/4. ;
}


double dmtpc::math::lineSegmentConvolvedWithGaussian2D(double *xx, double * par)
{
  double x = xx[0]; 
  double y = xx[1]; 

  double m = par[0]; 
  double z0 = par[1]; 
  double x0 = par[2]; 
  double y0 = par[3]; 
  double x1 = par[4]; 
  double y1 = par[5]; 
  double w = fabs(par[6]); 

  //calculate intersection onto line: 
  double xint, yint; 
  
  double slope_proj = y1==y0 ? 1 : (x0-x1)/(y1-y0); 
  double inc = y1==y0? 0 : 1; 

  lineLineIntersection(x0,y0,x1,y1,x,y, x + inc, y + slope_proj,&xint,&yint); 

  double distance_to_line = sqrt(pow(x-xint,2) + pow(y-yint,2)); 
  double distance_from_start =  sqrt(pow(xint-x0,2) + pow(yint-y0,2)); 
  double segment_length = sqrt(pow(x1-x0,2) + pow(y1-y0,2)); 


  //value along line: 
  double par1d[5]; 
  par1d[0] = m; 
  par1d[1] = z0; 
  par1d[2] = 0; 
  par1d[3] = segment_length; 
  par1d[4] = w; 

  double x1d = ( xint > x0 == x1 > x0  || (xint == x0 && yint > y0)) ? distance_from_start : -distance_from_start; 
  double val1d = lineSegmentConvolvedWithGaussian(&x1d, par1d); 

  //value perpendicular to line
  
  double val2d = val1d * TMath::Gaus(distance_to_line,0,w,true); 
  //printf("(x,y,xint,yint,d2l,d2smval1d,val2d): (%f,%f,%f,%f,%f,%f,%f,%f)\n", x,y,xint,yint,distance_to_line,distance_from_start,val1d,val2d); 


  return val2d; 

}

double dmtpc::math::lineSegmentConvolvedWithCauchy2D(double *xx, double * par)
{
  double x = xx[0]; 
  double y = xx[1]; 

  double m = par[0]; 
  double z0 = par[1]; 
  double x0 = par[2]; 
  double y0 = par[3]; 
  double x1 = par[4]; 
  double y1 = par[5]; 
  double w = fabs(par[6]); 

  //calculate intersection onto line: 
  double xint, yint; 
  
  double slope_proj = y1==y0 ? 1 : (x0-x1)/(y1-y0); 
  double inc = y1==y0? 0 : 1; 

  lineLineIntersection(x0,y0,x1,y1,x,y, x + inc, y + slope_proj,&xint,&yint); 

  double distance_to_line = sqrt(pow(x-xint,2) + pow(y-yint,2)); 
  double distance_from_start =  sqrt(pow(xint-x0,2) + pow(yint-y0,2)); 
  double segment_length = sqrt(pow(x1-x0,2) + pow(y1-y0,2)); 


  //value along line: 
  double par1d[5]; 
  par1d[0] = m; 
  par1d[1] = z0; 
  par1d[2] = 0; 
  par1d[3] = segment_length; 
  par1d[4] = w; 

  double x1d = ( xint > x0 == x1 > x0  || (xint == x0 && yint > y0)) ? distance_from_start : -distance_from_start; 
  double val1d = lineSegmentConvolvedWithCauchy(&x1d, par1d); 

  //value perpendicular to line
  
  double val2d = val1d * TMath::CauchyDist(distance_to_line,0,w); 
  //printf("(x,y,xint,yint,d2l,d2smval1d,val2d): (%f,%f,%f,%f,%f,%f,%f,%f)\n", x,y,xint,yint,distance_to_line,distance_from_start,val1d,val2d); 


  return val2d; 

}

double dmtpc::math::lineSegmentConvolvedWithCauchy(double *xx, double * par)
{
   double x = *xx; 
   double m = par[0]; 
   double b = par[1]; 
   double x0 = par[2]; 
   double x1 = par[3]; 
   double w = par[4]; 

   double X0 = (x-x0)/w;
   double X1 = (x-x1)/w;

   return (m*x+b)/M_PI*(TMath::ATan(X0)-TMath::ATan(X1)) - w*m/(2*M_PI)* TMath::Log(  (1 + X0*X0)/(1+X1*X1));

}


double dmtpc::math::lineSegmentConvolvedWithGaussian(double * xx, double * par)
{
   double x = *xx; 
   double m = par[0]; 
   double b = par[1]; 
   double x0 = par[2]; 
   double x1 = par[3]; 
   double w = par[4]; 

   if (w == 0)
   {
     if (x < x0) return 0; 
     if (x > x1) return 0; 
     return (m * x + b); 
   }


   double val =   (exp((2.*x0*x - x0*x0 - x*x)/(w*w*2.)) - exp((2.*x1*x - x1*x1 - x*x) / (2.*w*w))) * m * w / sqrt(2.*M_PI)  
                  + 0.5 * (b+m*x) * ( -TMath::Erf((x0-x)/(sqrt(2) * w)) + TMath::Erf((x1-x)/(sqrt(2) * w)) ); 


   return val; 

      
}

