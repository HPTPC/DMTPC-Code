#include "Angular.hh" 
#include "TH1.h" 
#include <assert.h>
#include "TMath.h"
#include <iostream>
#include <gsl/gsl_poly.h>
#include "Math/ProbFunc.h"
#include <cmath>



double dmtpc::math::angle::normalize(double phi, double center)
{
  return phi - (2*M_PI) * floor((phi + M_PI - center)/(2*M_PI)); 
}

double dmtpc::math::angle::normalize_axial(double phi, double center)
{
  double norml = normalize(phi,center); 
  if (norml - center > M_PI/2) norml-= M_PI; 
  if (norml - center < -M_PI/2) norml+= M_PI; 
  return norml; 
}

double dmtpc::math::angle::delta(double phi0, double phi1)
{
  return normalize(phi0,phi1) - phi0; 
}



double dmtpc::math::angle::delta_axial(double phi0, double phi1)
{
  return normalize_axial(normalize(phi0,phi1) - phi0); 
}


double dmtpc::math::angle::avg(size_t nang, const double * ang, const double * weights)
{
  double cos_sum = 0; 
  double sin_sum = 0; 

  for (size_t i = 0; i < nang; i++)
  {
    double w = weights == 0 ? 1. : weights[i]; 
    w/=nang; 
    cos_sum += w*cos(ang[i]); 
    sin_sum += w*sin(ang[i]); 
  }
  return atan2(sin_sum, cos_sum); 
}

double dmtpc::math::angle::avg_axial(size_t nang, const double * ang, const double * weights)
{
  double cos_sum = 0; 
  double sin_sum = 0; 

  for (size_t i = 0; i < nang; i++)
  {
    double w = weights == 0 ? 1. : weights[i]; 
    w/=nang; 
    cos_sum += w*cos(normalize_axial(ang[i])*2); 
    sin_sum += w*sin(normalize_axial(ang[i])*2); 
  }
  return atan2(sin_sum, cos_sum)/2; 
}


double dmtpc::math::angle::avg(const TH1* hist)
{
  double cos_sum = 0; 
  double sin_sum = 0; 

  for (int i = 1; i <= hist->GetNbinsX(); i++)
  {
    double w = hist->GetBinContent(i) ; 
    w/=hist->GetNbinsX(); 
    double ang = hist->GetBinCenter(i); 
    cos_sum += w*cos(ang); 
    sin_sum += w*sin(ang); 
  }
  return atan2(sin_sum, cos_sum); 
}


double dmtpc::math::angle::avg_axial(const TH1* hist)
{
  double cos_sum = 0; 
  double sin_sum = 0; 

  for (int i = 1; i <= hist->GetNbinsX(); i++)
  {
    double w = hist->GetBinContent(i) ; 
    w/=hist->GetNbinsX(); 
    double ang = normalize_axial(hist->GetBinContent(i))*2; 
    cos_sum += w*cos(ang); 
    sin_sum += w*sin(ang); 
  }
  return atan2(sin_sum, cos_sum)/2; 
}


double dmtpc::math::angle::resultant(size_t nang, const double * ang, const double * weights)
{
  double cos_sum = 0; 
  double sin_sum = 0; 

  for (size_t i = 0; i < nang; i++)
  {
    double w = weights == 0 ? 1. : weights[i]; 
    cos_sum += w*cos(ang[i]); 
    sin_sum += w*sin(ang[i]); 
  }

  return sqrt(cos_sum*cos_sum + sin_sum*sin_sum); 
}

double dmtpc::math::angle::resultant_axial(size_t nang, const double * ang, const double * weights)
{
  double cos_sum = 0; 
  double sin_sum = 0; 

  for (size_t i = 0; i < nang; i++)
  {
    double w = weights == 0 ? 1. : weights[i]; 
    double angle = normalize_axial(ang[i],M_PI/2)*2;    
    cos_sum += w*cos(angle); 
    sin_sum += w*sin(angle); 
  }

  return sqrt(cos_sum*cos_sum + sin_sum*sin_sum); 
}


double dmtpc::math::angle::resultant(const TH1* hist)
{
  double cos_sum = 0; 
  double sin_sum = 0; 

  for (int i = 1; i <= hist->GetNbinsX(); i++)
  {
    double w = hist->GetBinContent(i) ; 
    double ang = hist->GetBinCenter(i); 
    cos_sum += w*cos(ang); 
    sin_sum += w*sin(ang); 
  }
  return sqrt(cos_sum*cos_sum + sin_sum*sin_sum); 
}

double dmtpc::math::angle::resultant_axial(const TH1* hist)
{
  double cos_sum = 0; 
  double sin_sum = 0; 

  for (int i = 1; i <= hist->GetNbinsX(); i++)
  {
    double w = hist->GetBinContent(i) ; 
    double ang = normalize_axial(hist->GetBinCenter(i))*2; 
    cos_sum += w*cos(ang); 
    sin_sum += w*sin(ang); 
  }
  return sqrt(cos_sum*cos_sum + sin_sum*sin_sum); 
}

double dmtpc::math::angle::a1inv(double x) 
{
  if (x < 0) 
  {
     std::cerr << "Warning in a1inv: negative input, abs taken" << std::endl; 
     x = -x; 
  }

  //from a rational fit
  double p0 = -0.0257814;
  double p1 = 0.780033;
  double p2 = -0.299871;
  double p3 = 0.945254;
  double q1 = 1.04616;
  double q2 = 0.156743;
  double q3 = 0.94556;

  double norm = x*q3-p3;
  double a = (x*q2-p2) / norm; 
  double b = (x*q1-p1) / norm; 
  double c = (x-p0) / norm; 

  double x0,x1,x2; 
  gsl_poly_solve_cubic(a,b,c,&x0,&x1,&x2); 

  return x0; 
}

double dmtpc::math::angle::von_mises_kappa_estimate(double resultant, size_t n) 
{
  double nn = double(n); 
  double r = resultant / nn; 
  double re = sqrt(nn / (nn-1) * (r*r - 1/nn)); 
  return a1inv(re); 
}

double dmtpc::math::angle::rayleigh(double resultant, size_t n) { return 2* resultant*resultant / double(n); }
double dmtpc::math::angle::var(double resultant, size_t n) { return 1 - resultant / double(n);} 

double 
dmtpc::math::angle::von_mises_dist(double *x, double *par)
{
  double A = par[0]; 
  double u = par[1]; 
  double k = fabs(par[2]); 

  if (k==0) return A/(2.*M_PI); 
  if (k>700) return 0; //avoid overflow 

  double val =  A*exp(k * cos(*x-u)) / (2*M_PI * TMath::BesselI0(k));  
#ifdef NAN_CHECKS
  if (isnan(val))
  {
    fprintf(stderr, "Warning in dmtpc::math::angle::von_mises_dist, val is NaN. A= %f, u = %f, k = %f\n", A,u,k); 
    return 0; 
  }
#endif 
  return val; 
}


double 
dmtpc::math::angle::von_mises_dist_ht(double *x, double *par)
{
  double f = par[0]; 
  double par2[3]; 
  par2[0] = par[1];
  par2[1] = par[2] + M_PI;
  par2[2] = par[3]; 
  return von_mises_dist(x,par+1) * f + (1-f) * von_mises_dist(x,par2); 
}


double 
dmtpc::math::angle::wrapped_normal_dist(double *x, double *par)
{

  double phi = normalize(*x); 
  double A = par[0]; 
  double u = par[1]; 
  double w = par[2]; 

  int max_k = 3+int(ceil(2 * w / (2*M_PI))); 

  double ans = 0; 
  for (int k = -max_k; k <= max_k; k++)
  {
    ans += exp(-pow(phi - u + 2 * M_PI * k,2)/(2*w*w)); 
  }

  ans *= A/(w * sqrt(2*M_PI)); 

  return ans; 
}


double 
dmtpc::math::angle::wrapped_normal_dist_ht(double *x, double *par)
{
  double f = par[0]; 
  double par2[3]; 
  par2[0] = par[1];
  par2[1] = par[2] + M_PI;
  par2[2] = par[2]; 
  return wrapped_normal_dist(x, par+1) * f + (1-f) * wrapped_normal_dist(x,par2);
}

static void build_rank_uns(unsigned n, const double * unsorted_array, double * ranks)
{
  std::vector<unsigned> indices(n); 
  TMath::Sort(n,unsorted_array,&(indices[0]),false); 
  for (unsigned i = 0; i < n; i++)
  {
    int nwithsamerank = 0; 
    double rank = indices[i]; 
    while (unsorted_array[indices[i+nwithsamerank + 1]] == unsorted_array[indices[i]])
    {
      nwithsamerank++; 
    }

    if (nwithsamerank) 
    {
      rank +=  0.5* nwithsamerank; 
    }


    ranks[i] = rank; 

    for (int j = 0; j < nwithsamerank; j++)
    {
      ranks[i+1+j] = rank; 
    }

    i+= nwithsamerank; 
  }
}


static void build_rank(unsigned n, const double * sorted_array, double * ranks)
{
  for (unsigned i = 0; i < n; i++)
  {
    int nwithsamerank = 0; 
    double rank = i; 
    while (sorted_array[i+nwithsamerank + 1] == sorted_array[i])
    {
      nwithsamerank++; 
    }

    if (nwithsamerank) 
    {
      rank +=  0.5* nwithsamerank; 
    }


    ranks[i] = rank; 

    for (int j = 0; j < nwithsamerank; j++)
    {
      ranks[i+1+j] = rank; 
    }

    i+= nwithsamerank; 
  }
}



//see Hinkley: on the ratio of two correlated normal random variables 

double 
dmtpc::math::angle::ratio_distribution(double *x, double *par)
{

  double w = *x; 

  double A = par[0]; 
  double ux = par[1]; 
  double sx = fabs(par[2]); 
  double uy = par[3]; 
  double sy = fabs(par[4]); 
  double p = par[5]; 
   
  double a = sqrt(w*w/(sx*sx) + 1./(sy*sy) - 2 *p *w / (sx*sy)); 
  double b = ux*w / (sx*sx) + uy/(sy*sy) - p *(ux + uy*w)/(sx*sy); 
  double c = ux*ux/(sx*sx) - 2*p*ux*uy/(sx*sy) + uy*uy/(sy*sy); 
  double d = exp((b*b - c * a*a)/(2*(1-p*p)*a*a)); 

  double Phi = ROOT::Math::normal_cdf(b/a/sqrt(1-p*p)) - ROOT::Math::normal_cdf(-b/a/sqrt(1-p*p)); 

  return A*(b*d/(sqrt(2*TMath::Pi()) * sx * sy * a *a * a) * Phi + sqrt(1-p*p)/(TMath::Pi() * sx * sy * a*a) *exp(-c/(2*(1-p*p)))  ); 
}

double 
dmtpc::math::angle::kuipers_test(unsigned m, const double *A, unsigned n, const double *B, bool sorted)
{


  std::vector<unsigned> iA (m);
  std::vector<unsigned> iB (n); 

  if (sorted)
  {
    for (unsigned i = 0; i <m;i++)
      iA[i] =i; 
    for (unsigned i = 0; i <n;i++)
      iB[i] =i; 
  }
  else
  {
    TMath::Sort(m,A,&iA[0],false); 
    TMath::Sort(n,B,&iB[0],false); 
  }

  double Dplus = 0; 
  double Dminus = 0; 

  double cdf_A; 
  double cdf_B; 

  unsigned i = 0; 
  unsigned j = 0;
  double vA,vB; 

  /* algorithm based on NR K-S test */ 
  while(i < m && j < n)
  {

    vA = A[iA[i]]; 
    vB = B[iB[j]]; 
    if (vA  < m && vA <=  vB )
    {
      do
      {
        cdf_A = (++i) / (double(m));  
      }
      while  ( i < m && vA == A[iA[i]]); 
    }

    if (vB <= vA)
    {

      do 
        cdf_B = (++j) / (double(n)); 
      while (j < n && vB == B[iB[j]]); 
    }

    Dplus = TMath::Max(Dplus, cdf_A - cdf_B); 
    Dminus = TMath::Max(Dminus, -cdf_A + cdf_B); 
  }


  return Dplus + Dminus; 
}

double 
dmtpc::math::angle::kuipers_p(double V, int m, int n) 
{
  double N = double(m)*double(n) / (m+n); 
  double lambda = V*(sqrt(N) + 0.155 + 0.24 / sqrt(N)); 


  if (lambda < 0.4) return 1.; 

  double sum = 0; 
  int j = 1; 

  while(true) 
  {
    double term =  2 *( 4 * j * j * lambda * lambda - 1) * exp(-2*j*j*lambda*lambda);
    sum += term; 
    j++; 

    if (term < 1e-6)
      break; 

  }

  assert(sum <=1); 
  assert(sum >=0); 

  return sum; 
}

double 
dmtpc::math::angle::kuipers_test(const TH1 * A, const TH1 * B) 
{

  double Dplus = 0, Dminus = 0; 
  double cdf_A, cdf_B; 
  unsigned i = 0;

  if (A->GetNbinsX() != B->GetNbinsX() || A->GetXaxis()->GetXmin() != B->GetXaxis()->GetXmin() || A->GetXaxis()->GetXmax())
  {
    fprintf(stderr,"kuipers_test: WARNING: Histograms don't have same binning! returning 0\n"); 
    return 0; 
  }
  double IA = A->Integral(); 
  double IB = B->Integral(); 


  if (!IA || IB) 
  {

    fprintf(stderr,"kuipers_test: WARNING: One histogram is empty.\n"); 
    return 1; 
  }


  for (int i = 1 ; i <= A->GetNbinsX(); i++)  
  {
    cdf_A += A->GetBinContent(i) / IA; 
    cdf_B += B->GetBinContent(i) / IB; 

    Dplus = TMath::Max(Dplus, cdf_A - cdf_B); 
    Dminus = TMath::Max(Dminus, -cdf_A + cdf_B); 
  }

  return Dplus + Dminus; 

}


