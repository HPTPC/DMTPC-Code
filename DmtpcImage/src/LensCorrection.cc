#include "LensCorrection.hh"
#include "TH2.h"
#include <gsl/gsl_poly.h>
#include "Interpolation.hh"
#include "ImageTools.hh"
#include <stdlib.h>
#include <vector> 


ClassImp(dmtpc::image::LensCorrection); 


dmtpc::image::LensCorrection::~LensCorrection()
{
  free(_coeffs); 
}

dmtpc::image::LensCorrection::LensCorrection(const char * name, unsigned order, double * coef)
{
  fName = strdup(name); 

  _order = order; 
  _coeffs = (double*) malloc(sizeof (*_coeffs) * (order+1)); 
  if (coef)
  {
    memcpy (_coeffs,coef, sizeof (*_coeffs) * (order+1)); 
  }
}

void dmtpc::image::LensCorrection::setParameters(const double * vals)
{
  memcpy(_coeffs,vals, sizeof (*vals) * (_order+1));   
}

int dmtpc::image::LensCorrection::distortCoords(const double * center, const double * in, double * out) const 
{
  if (!center || !in || !out) return 1; 

  double x0 = center[0]; 
  double y0 = center[1]; 
  double x = in[0]; 
  double y = in[1]; 

  double r = sqrt(pow((x-x0),2) + pow((y-y0),2));

  if (r==0) 
  {
    out[0] = x; 
    out[1] = y; 
    return 0; 
  }

  double f = _coeffs[0] ; 
  double rr = r; 
  for (unsigned i = 1; i <= _order; i++)
  {
    f+= _coeffs[i] * rr; 
    rr*=r; 
  }
  out[0] = x0 + f * (x-x0); 
  out[1] = y0 + f * (y-y0); 
  return 0; 
}

int dmtpc::image::LensCorrection::unDistortCoords(const double * center, const double * in, double * out) const
{
  
  /*Solve inverse problem:
   *
   *
   *  \vec{r_{distorted}} = g(\mathbf{r_{corrected}}) \vec{r_{corrected}}
   *  \vec{r_{corrected}} = f(\mathbf{r_{distorted}}) \vec{r_{distorted}}
   *
   *  \to 
   *
   *  \vec{r_{corrected}} = f(g(\mathbf{r_{corrected}}) \mathbf{r_{corrected}}) g(\mathbf{r_{corrected}}) \vec{r_{corrected}}
   *
   *  \to 
   *
   *  1 - f(g(\mathbf{r_{corrected}}) \mathbf{r_{corrected}}) g(\mathbf{r_{corrected}})  = 0; 
   *
   *  so g is a root of the polynomial 
   *
   *  1 - f(x\mathbf{r_{corrected}}) x 
   *
   *
   */

  if (!center || !in || !out) return 1; 

  double x0 = center[0]; 
  double y0 = center[1]; 
  double x = in[0]; 
  double y = in[1]; 
  double r = sqrt(pow((x-x0),2) + pow((y-y0),2));

  if (r==0) 
  {
    out[0] = x; 
    out[1] = y; 
    return 0; 
  }

  double * params = (double *) alloca(sizeof(double) * (_order + 2)) ; 

  params[0] = 1; 

  double rr = 1; 
  for (unsigned i = 0; i <= _order; i++)
  {
    params[i+1] = -_coeffs[i] * rr; 
    rr *= r; 
  }


  double g = DBL_MAX; 
  int nr = 0; 
  double * roots = (double*) alloca(sizeof(double) * (_order + 1)) ; 
  switch (_order + 1)
  {
    case 1: 
      nr = 1; 
      roots[0] = -params[0]/params[1]; 
      break;
    case 2: 
      nr = gsl_poly_solve_quadratic(params[2],params[1],params[0],&roots[0],&roots[1]); 
      break;
    case 3: 
      nr = gsl_poly_solve_cubic(params[2]/params[3],params[1]/params[3],params[0]/params[3],&roots[0],&roots[1],&roots[2]); 
      break; 
    default:
    {
      gsl_poly_complex_workspace * w = gsl_poly_complex_workspace_alloc(_order + 2); 
      double * complex_roots = (double *) alloca(2* sizeof(double) * (_order+1));
      gsl_poly_complex_solve(params,_order+2,w,complex_roots); 
      for (unsigned i = 0; i < _order+1; i++)
      {
        if (fabs(complex_roots[2*i+1]) < 1e-12)
        {
          roots[nr++] = complex_roots[2*i]; 
        }
      }
      gsl_poly_complex_workspace_free(w); 
    }
  }

  if (nr ==0)
  {
    return 2; 
  }

  for (int i = 0; i < nr; i++)
  {
    if (fabs(roots[i] - 1) < fabs(g-1))
    {
      g = roots[i];
    }
  }

  out[0] = x0 + g * (x-x0); 
  out[1] = y0 + g * (y-y0); 
  return 0; 

}


TH2 * dmtpc::image::LensCorrection::correctDistortion(const TH2 * in, TH2 * out, const char * interpolation, const double * camera_center, bool backwards) const
{


  double r0[2]; 
  if (!camera_center)
  {
    double xmin = in->GetXaxis()->GetXmin(); 
    double xmax = in->GetXaxis()->GetXmax(); 
    double ymin = in->GetYaxis()->GetXmin(); 
    double ymax = in->GetYaxis()->GetXmax(); 
    r0[0] = (xmin + xmax)/2; 
    r0[1] = (ymin + ymax)/2; 
  }
  else
  {
    r0[0] = camera_center[0];
    r0[1] = camera_center[1];
  }

  if (!out)
  {
    
    double bwidth = in->GetXaxis()->GetBinWidth(1); 

    double xmin = in->GetXaxis()->GetXmin(); 
    double xmax = in->GetXaxis()->GetXmax(); 
    double ymin = in->GetYaxis()->GetXmin(); 
    double ymax = in->GetYaxis()->GetXmax(); 

    double r[2]; 
    double rout[2]; 
    double new_xmin;
    double new_xmax;
    double new_ymin;
    double new_ymax;


    r[0] = xmax; 
    r[1] = ymax; 
    !backwards ? distortCoords(r0, r, rout) : unDistortCoords(r0,r,rout) ; 
    bool bigger = rout[0] > xmax ;  

//    printf("bigger: %d\n",bigger); 
    
    if (!bigger)
    {
      r[0] = xmin; 
      r[1] = r0[1]; 
      !backwards ? distortCoords(r0, r, rout) : unDistortCoords(r0,r,rout) ; 
      new_xmin = rout[0]; 

      r[0] = xmax; 
      r[1] = r0[1]; 
      !backwards ? distortCoords(r0, r, rout) : unDistortCoords(r0,r,rout) ; 
      new_xmax = rout[0]; 

      r[0] = r0[0]; 
      r[1] = ymin; 
      !backwards ? distortCoords(r0, r, rout) : unDistortCoords(r0,r,rout) ; 
      new_ymin = rout[1]; 

      r[0] = r0[0]; 
      r[1] = ymax; 
      !backwards ? distortCoords(r0, r, rout) : unDistortCoords(r0,r,rout) ; 
      new_ymax = rout[1]; 
    }

    else
    {
      r[0] = xmin; 
      r[1] = ymin; 
      !backwards ? distortCoords(r0, r, rout) : unDistortCoords(r0,r,rout) ; 
      new_xmin = rout[0]; 
      new_ymin = rout[1]; 

      r[0] = xmax; 
      r[1] = ymax; 
      !backwards ? distortCoords(r0, r, rout) : unDistortCoords(r0,r,rout) ; 
      new_xmax = rout[0]; 
      new_ymax = rout[1]; 
    }
    
    static int hist_counter = 0; 
    TString name = TString::Format("%s_%sdistorted_by_%s_%d", in->GetName(),backwards ? "" : "un", fName.Data(), hist_counter++); 
    int nbinsx = int((new_xmax - new_xmin) / bwidth + 1); 
    int nbinsy = int((new_ymax - new_ymin) / bwidth + 1); 

    //printf("%f,%f  %f,%f\n",new_xmin,new_ymin,new_xmax, new_ymax); 
    out = dmtpc::image::tools::newTH2StealType(in, name.Data(),name.Data(), nbinsx, new_xmin, new_xmax, nbinsy, new_ymin, new_ymax); 
//    out->SetDirectory(0); 
  }


  double rin[2];   
  double rout[2];   
  for (int i = 1; i <= out->GetNbinsX(); i++)
  {
    rin[0] = out->GetXaxis()->GetBinCenter(i); 
    for (int j = 1; j <= out->GetNbinsY(); j++)
    {
      rin[1] = out->GetYaxis()->GetBinCenter(j); 
      backwards ? distortCoords(r0,rin,rout) : unDistortCoords(r0,rin,rout); 

      if (rout[0] >= in->GetXaxis()->GetBinCenter(1)  && rout[0] <= in->GetXaxis()->GetBinCenter(in->GetNbinsX()) 
          && rout[1] >= in->GetYaxis()->GetBinCenter(1)  && rout[1] <= in->GetYaxis()->GetBinCenter(in->GetNbinsY()))
      {
        out->SetBinContent(i,j, dmtpc::image::interpolation::interpolate(in, rout[0],rout[1],interpolation)); 
      }
      else
      {
        out->SetBinContent(i,j,0); 
      }
    }
  }


  return out; 
}

