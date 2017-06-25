#include "TrackFitter.hh" 
#include "TStyle.h"
#include "TPoints.h"
#include "ImageTransforms.hh"
#include <assert.h>
#include "TMath.h"
#include "SRIMTable.hh"
#include "TGraphAsymmErrors.h"
#include "Angular.hh"
#include "ConvolvedSegment.hh" 
#include "ImageProcessing.hh"
#include "TPaveText.h"
#include "Math/Factory.h"
#include "Math/GSLMCIntegrator.h"
#include "Minuit2/Minuit2Minimizer.h"
#include "TRandom3.h"
#include "limits.h"

template <class T>
void printVec(size_t N, const T * v) 
  {
    std::cout << "[ "; 
    for (size_t i = 0; i < N; i++)
    {
      std::cout << v[i];   
      std::cout << (i < N-1 ? " , " : " ]"); 
    }
    std::cout << std::endl; 
}

template<class T>
bool checkNaN(size_t N, const T * v) 
{
    for (size_t i = 0; i < N; i++)
    {
      if (isnan(v[i]))
      {
        return true; 
      }
    }
    return false; 
}


static __thread bool second_width_is_cauchy; 


inline double sumError(double a, double b) 
{
  return sqrt(a*a + b*b); 
}


static unsigned nfitter = 0; 

ClassImp(dmtpc::analysis::TrackFitter); 
ClassImp(dmtpc::analysis::TrackFitter::Result); 
ClassImp(dmtpc::analysis::TrackFitter::Param); 

struct srimLineFitIntegralParams
{
   const dmtpc::analysis::TrackFitter * fitter; 
   enum which {ALL, SAME,OPPOSITE} which; 
   double phi; 
   double x0; 
   double y0; 
   double sigma; 
   double E; 
   double scale;
   double second_width; 
   double second_width_ratio; 
   bool flip; 
}; 

static double SRIMLineFitFnIntegral(double *x, size_t ndim, void * p)
{
  (void) ndim; 
  srimLineFitIntegralParams * params = (srimLineFitIntegralParams*) p; 
  double phi = params->phi; 
  double z1 = x[2]; 
  double z0 = x[1]; 
  double dz = z1-z0; 
  double offset = x[0]; 
  double scale = params->scale;


  double fpar[dmtpc::analysis::TrackFitter::NPAR]; 
  fpar[dmtpc::analysis::TrackFitter::PAR_E] = params->E;
  fpar[dmtpc::analysis::TrackFitter::PAR_X0] = params->x0 + offset * cos(phi); 
  fpar[dmtpc::analysis::TrackFitter::PAR_Y0] = params->y0 + offset * sin(phi); 
  fpar[dmtpc::analysis::TrackFitter::PAR_PHI] = params->phi; 
  fpar[dmtpc::analysis::TrackFitter::PAR_Z0] = z0; 
  fpar[dmtpc::analysis::TrackFitter::PAR_DELTA_Z] = dz; 
  fpar[dmtpc::analysis::TrackFitter::PAR_SIGMA2] = params->sigma*params->sigma; 
  fpar[dmtpc::analysis::TrackFitter::PAR_SECOND_WIDTH] = params->second_width; 
  fpar[dmtpc::analysis::TrackFitter::PAR_SECOND_WIDTH_RATIO] = params->second_width_ratio; 


  bool same_side = (params->flip == (dz > 0));
  if (params->which == srimLineFitIntegralParams::SAME && !same_side) return 0; 
  else if (params->which == srimLineFitIntegralParams::OPPOSITE && same_side) return 0; 
  double ret =  exp(-(params->fitter->SRIMLineFit2DFn(fpar)-scale)/2); 

  if (isnan(ret))
  {
    std::cout << "nan in integral" << std::endl; 
    printVec(dmtpc::analysis::TrackFitter::NPAR, fpar); 
  }

//  cout << ret << endl; 
  return ret; 
}



void dmtpc::analysis::TrackFitter::fillResultParams(bool from_alt)
{

  const double * vals = min->X(); 
  const double * errors = min->Errors(); 

  //set values



  if(!from_alt)
  {
    result->fitE.val = vals[PAR_E]; 
    result->E.val = 0; 
    result->E.err = 0; 
  }

  result->second_width_is_cauchy = enable_cauchy; 

  result->x0.val = vals[PAR_X0]; 
  result->y0.val = vals[PAR_Y0]; 
  if (!from_alt) result->phi.val = dmtpc::math::angle::normalize(vals[PAR_PHI]); 
  result->z0.val = vals[PAR_Z0]; 
  result->delta_z.val = vals[PAR_DELTA_Z]; 
  result->z1.val= vals[PAR_Z0] + vals[PAR_DELTA_Z]; 
  if (!from_alt) result->sigma2.val = vals[PAR_SIGMA2]; 
  if (!from_alt) result->sigma.val = sqrt(result->sigma2.val); 
  if (!from_alt) result->second_width.val = vals[PAR_SECOND_WIDTH]; 
  if (!from_alt) result->second_width_ratio.val = vals[PAR_SECOND_WIDTH_RATIO]; 

  if (!from_alt) result->fitE.err = errors[PAR_E]; 
  result->x0.err = errors[PAR_X0]; 
  result->y0.err = errors[PAR_Y0]; 
  if (!from_alt) result->phi.err = errors[PAR_PHI]; 
  result->z0.err = errors[PAR_Z0]; 
  result->delta_z.err = errors[PAR_DELTA_Z]; 
  result->z1.err= sqrt(pow(errors[PAR_Z0],2) + pow(errors[PAR_DELTA_Z],2)); 
  if (!from_alt) result->sigma2.err = errors[PAR_SIGMA2]; 
  if (!from_alt) result->sigma.err = 0.5 * result->sigma.val * result->sigma2.err / result->sigma2.val; 
  if (!from_alt) result->second_width.err = errors[PAR_SECOND_WIDTH]; 
  if (!from_alt) result->second_width_ratio.err = errors[PAR_SECOND_WIDTH_RATIO]; 

  if (!from_alt) result->fitE.zeroErr(); 
  result->x0.zeroErr(); 
  result->y0.zeroErr(); 
  if (!from_alt) result->phi.zeroErr(); 
  result->z0.zeroErr(); 
  result->z1.zeroErr(); 
  result->z1.zeroErr(); 
  if (!from_alt) result->sigma.zeroErr(); 
  if (!from_alt) result->second_width.zeroErr(); 
  if (!from_alt) result->second_width_ratio.zeroErr(); 

  //do minos if always minos or z0, z1 close enough to limits 
  result->minos = always_minos 
    || fabs(result->z0.val-minheight)/result->z0.err < minos_thresh
    || fabs(result->z1.val-minheight)/result->z1.err < minos_thresh
    || fabs(result->z0.val-maxheight)/result->z0.err < minos_thresh
    || fabs(result->z1.val-maxheight)/result->z1.err < minos_thresh;

  if (result->minos)
  {
  if (!from_alt)  min->GetMinosError(PAR_E,result->fitE.errDn, result->fitE.errUp); 
  //  min->GetMinosError(PAR_X0,result->x0.errDn, result->x0.errUp); 
  //  min->GetMinosError(PAR_Y0,result->y0.errDn, result->y0.errUp); 
    min->GetMinosError(PAR_Z0,result->z0.errDn, result->z0.errUp); 
    min->GetMinosError(PAR_DELTA_Z,result->delta_z.errDn, result->delta_z.errUp); 
    result->z1.errDn = -sumError(result->delta_z.errDn, result->z0.errDn); 
    result->z1.errUp = -sumError(result->delta_z.errUp, result->z0.errUp); 
 //   min->GetMinosError(PAR_PHI,result->phi.errDn, result->phi.errUp); 
 //   min->GetMinosError(PAR_SIGMA,result->sigma.errDn, result->sigma.errUp); 
  }


  result->fitE.fixErr(); 
  result->x0.fixErr(); 
  result->y0.fixErr(); 
  result->phi.fixErr(); 
  result->z0.fixErr(); 
  result->z1.fixErr(); 
  result->delta_z.fixErr(); 
  result->sigma.fixErr(); 
  result->sigma2.fixErr(); 
  result->second_width.fixErr(); 
  result->second_width_ratio.fixErr(); 

  result->range.val = 2 * binwidth * result->fitE.val / (2*result->z0.val + result->delta_z.val); 
  //incorporate covariance of z0 and z1... but not with E since E error is probably relatively small 
  result->range.err = result->range.val *  2 * sqrt( pow(result->fitE.err,2) / pow(result->fitE.val,2) + (2*pow(result->z0.err,2) + pow(result->delta_z.err,2) + 2*min->CovMatrix(PAR_Z0,PAR_DELTA_Z)) / pow(2*result->z0.val + result->delta_z.val,2)); 

  //TODO: figure out a better way to set these
  result->range.errDn = result->range.err; 
  result->range.errUp = result->range.err; 



  result->chisq = min->MinValue(); 
  result->ndof = fithist->GetNbinsX()*fithist->GetNbinsY() - min->NFree(); 
  result->gain = gain; 


  result->flip = result->delta_z.val > 0; 

  result->htphi.err = result->phi.err; 
  result->htphi.errUp = result->phi.errUp; 
  result->htphi.errDn = result->phi.errDn; 
  result->htphi.val = result->flip ? dmtpc::math::angle::normalize(result->phi.val + M_PI) : result->phi.val; 


  for (unsigned i = 0; i < NPAR; i++)
  {
    for (unsigned j = 0; j < NPAR; j++)
    {
      result->covar[i][j] = min->CovMatrix(i,j); 
    }
  }

  double m = (result->delta_z.val*binwidth) / (result->range.val); 
  double x1 = result->x0.val + result->range.val * cos(result->phi.val); 
  double y1 = result->y0.val + result->range.val * sin(result->phi.val); 
  double pars[] = {m,result->z0.val*binwidth,result->x0.val,result->y0.val,x1,y1,result->sigma.val, result->second_width.val, result->second_width_ratio.val}; 
  fitfn->SetParameters(pars); 

  if (checkNaN<double>(NPAR, pars))
  {
    result->success = false; 
    return; 
  }
  double bin2 = binwidth * binwidth; 
  for (int i = 1; i <=fithist->GetNbinsX(); i++)
  {
    for (int j = 1; j <=fithist->GetNbinsY(); j++)
    {
      double xlow = fithist->GetXaxis()->GetBinLowEdge(i); 
      double ylow = fithist->GetYaxis()->GetBinLowEdge(j); 
      double xhigh = xlow + binwidth;
      double yhigh = ylow + binwidth; 
      double vlow[2] = {xlow,ylow}; 
      double vhigh[2] = {xhigh,yhigh}; 
      double valfn = (quickeval ? fitfn->Eval((xlow+xhigh)/2,(ylow+yhigh)/2) : fit_integrator->Integral(vlow,vhigh)/bin2) * local_gainmap->GetBinContent(i,j); 
      result->E.val+= valfn; 
    }
  }

  result->Ephys.val = result->E.val / gain; 
  result->E.err = result->fitE.err * result->E.val / result->fitE.val; 
  result->Ephys.err = result->E.err/gain; 
  result->E.errDn =result->fitE.errDn * result->E.val / result->fitE.val; 
  result->E.errUp =result->fitE.errUp * result->E.val / result->fitE.val; 
  result->Ephys.errDn =result->E.errDn / gain;
  result->Ephys.errUp =result->E.errUp / gain; 



  


  //compute quick probability estimate based on overlap of z0, z1 multiplied by probability that phi is on same side
  double zscore_z = fabs(result->delta_z.val); 
  zscore_z /=  (result->delta_z.val > 0 ? -result->delta_z.errDn : result->delta_z.errUp); 
  std::cout <<"zscore_z: " << zscore_z << std::endl; 
  double zscore_phi = fabs(M_PI/result->phi.err); 
  std::cout <<"zscore_phi: " << zscore_phi << std::endl; 

  double p_phi = 0.5  + 0.5*TMath::Erf(zscore_phi/sqrt(2)); 
  double p_z = 0.5 + 0.5*TMath::Erf(zscore_z/sqrt(2)); 

  result->probFast = p_phi * p_z + (1-p_phi) * (1-p_z) ; 




}


dmtpc::analysis::TrackFitter::~TrackFitter()
{
  if (fitfn) delete fitfn; 
  if (wrapped_fitfn) delete wrapped_fitfn; 
  if (fit_integrator) delete fit_integrator; 
  if (result) delete result; 
  if (arrow) delete arrow; 
  if (canvas) delete canvas; 
  if (fithist) delete fithist; 
  if (local_gainmap) delete local_gainmap; 
  if (local_gainmap_e) delete local_gainmap_e; 
  if (gainMap_blurred) delete gainMap_blurred; 
  delete min; 
}
static double fn1d_impl( double *x, double *pars)
{


  double ratio = pars[6]; 

  if (!ratio)  
  {
    return dmtpc::math::lineSegmentConvolvedWithGaussian(x,pars); 
  }

  int npar_line = dmtpc::math::npar_lineSegmentConvolvedWithGaussian; 
  double pars_segment_a[npar_line]; 
  double pars_segment_b[npar_line]; 

  memcpy(pars_segment_a, pars, npar_line*sizeof(double));
  memcpy(pars_segment_b, pars, npar_line*sizeof(double));
  pars_segment_b[npar_line-1] = pars[5]; 

  double a_coeff = 1./(1+ratio); 
  double b_coeff = 1 - a_coeff; 


  if (second_width_is_cauchy)
  {
    return a_coeff * dmtpc::math::lineSegmentConvolvedWithGaussian(x, pars_segment_a) + 
           b_coeff * dmtpc::math::lineSegmentConvolvedWithCauchy(x, pars_segment_b); 
  }
  else
  {
    return a_coeff * dmtpc::math::lineSegmentConvolvedWithGaussian(x, pars_segment_a) + 
           b_coeff * dmtpc::math::lineSegmentConvolvedWithGaussian(x, pars_segment_b); 
  }

}

static double fitfn_impl( double *x, double *pars)
{


  double ratio = pars[dmtpc::analysis::TrackFitter::PAR_SECOND_WIDTH_RATIO]; 
  if (!ratio)  
  {
    return dmtpc::math::lineSegmentConvolvedWithGaussian2D(x,pars); 
  }

  int npar_line = dmtpc::math::npar_lineSegmentConvolvedWithGaussian2D; 
  double pars_segment_a[npar_line]; 
  double pars_segment_b[npar_line]; 

  memcpy(pars_segment_a, pars, npar_line*sizeof(double));
  memcpy(pars_segment_b, pars, npar_line*sizeof(double));
  pars_segment_b[npar_line-1] = pars[dmtpc::analysis::TrackFitter::PAR_SECOND_WIDTH]; 


  double a_coeff = 1./(1+ratio); 
  double b_coeff = 1 - a_coeff; 

  if (second_width_is_cauchy) 
  {
    return a_coeff * dmtpc::math::lineSegmentConvolvedWithGaussian2D(x, pars_segment_a) + 
           b_coeff * dmtpc::math::lineSegmentConvolvedWithCauchy2D(x, pars_segment_b); 
  }
  else
  {

    return a_coeff * dmtpc::math::lineSegmentConvolvedWithGaussian2D(x, pars_segment_a) + 
           b_coeff * dmtpc::math::lineSegmentConvolvedWithGaussian2D(x, pars_segment_b); 

  }


}

const double avg_kernel[9] = { 1./9,1./9,1./9,
                               1./9,1./9,1./9, 
                               1./9,1./9,1./9 }; 


dmtpc::analysis::TrackFitter::TrackFitter(dmtpc::physics::SRIMTable * sr, double rangecal, double g, const TH2* gainmap, double min_g, double rn, double cg)
  : minfn(this, &dmtpc::analysis::TrackFitter::SRIMLineFit2DFn,NPAR) 
{
  gain = g; 
  gainMap = gainmap;
  
  gainMap_blurred = gainmap ? dmtpc::image::processing::convolve(gainmap, avg_kernel, 3,3) : 0; 
  lengthcal = rangecal; 
  fitter_id = nfitter++; 
  srim = sr; 
  result = 0; 
  min_gain = min_g; 
  read_noise = rn; 
  conversion_gain = cg; 

  fitfn = new TF2(TString::Format("trackfitfn%u",fitter_id),&fitfn_impl,0,1,0,1,NPAR); 
  wrapped_fitfn = new ROOT::Math::WrappedMultiFunction<TF2&> (*fitfn,2); 
  fit_integrator = new ROOT::Math::AdaptiveIntegratorMultiDim(*wrapped_fitfn, 1e-5,1e-4,100); 
  arrow = new TArrow(); 
  zero_out = false; 
  padding = 3; 
  fithist = 0; 
  force_decreasing = false; 
  local_gainmap = 0; 
  use_gainmap_error = false; 
  local_gainmap_e = 0; 
  integral_nsigma=3; 
  integral_reltol=0.1; 
  integral_abstol = 0; 
  integral_ncalls = 10000; 
  canvas = 0; 
  do_integral = false; 
  always_minos = false; 
  minos_thresh = 1; 
  quickeval = false; 
  min = new ROOT::Minuit2::Minuit2Minimizer; 
  min->SetFunction(minfn); 
  min->SetPrintLevel(10); 
  enable_cauchy= false; 
  enable_second_gaus = false; 
  likelihood_flip = false; 
}




double dmtpc::analysis::TrackFitter::SRIMLineFit2DFn(const double * p) const
{

  double E =fabs( p[PAR_E] * binwidth * binwidth); 
  double x0 = p[PAR_X0]; 
  double y0 = p[PAR_Y0]; 
  double phi =p[PAR_PHI];
  double raw_z0 = p[PAR_Z0]; 
  double raw_z1 = p[PAR_Z0] + p[PAR_DELTA_Z]; 
  double z0 = p[PAR_Z0] * binwidth; 
  double z1 = p[PAR_DELTA_Z] * binwidth + z0; 


  double chisq = 0; 

  //penalize for minheight, maxheight
  
  if (raw_z0 < minheight)
  {
    chisq += pow(raw_z0-minheight,2) / fabs(raw_z0); 
    if (raw_z0 < 0) 
    {
      chisq+= 100 * fabs(raw_z0); 
    }
//    std::cout << raw_z0 << std::endl; 
  }

  if (raw_z1 < minheight)
  {
    chisq += pow(raw_z1-minheight,2) / fabs(raw_z1); 
    if (raw_z1 < 0) 
    {
      chisq += 100 * fabs(raw_z1); 
    }
//    std::cout << raw_z1 << std::endl; 
  }

  if (raw_z1 > maxheight)
  {
    chisq += pow(raw_z1-maxheight,2) / fabs(raw_z1);
//    std::cout << raw_z1 << std::endl; 
  }

  if (raw_z0 > maxheight)
  {
    chisq += pow(raw_z0-maxheight,2) / fabs(raw_z0) ;
//    std::cout << raw_z0 << std::endl; 
  }


  double sigma = sqrt(p[PAR_SIGMA2]); 
  double second_width = p[PAR_SECOND_WIDTH]; 
  double second_width_ratio = p[PAR_SECOND_WIDTH_RATIO]; 

  double range =  2*E/(z0+z1);  
  double m =  (z1 - z0)/range; 
  double x1 = x0 + range * cos(phi); 
  double y1 = y0 + range * sin(phi); 

  double pars[] = {m,z0,x0,y0,x1,y1,sigma,second_width, second_width_ratio}; 

  if (checkNaN(NPAR,p)) 
  {
    std::cout << "p is NaN: ";  
    printVec(NPAR,p);
    return 1e99; 
  }

  if (checkNaN(sizeof(pars)/sizeof(*pars),pars)) 
  {
    std::cout << "pars_a is NaN: ";  
    printVec(sizeof(pars)/sizeof(*pars),pars);
    return 1e99 ; 
  }


  if (E<=0) 
  {
    std::cout << "E is Non-Positive: ";  
    printVec(NPAR,pars);
    return 1e99 ; 
  }




  fitfn->SetRange(fithist->GetXaxis()->GetXmin(), fithist->GetXaxis()->GetXmax(), fithist->GetYaxis()->GetXmin(), fithist->GetYaxis()->GetXmax()) ; 
  fitfn->SetParameters(pars); 

  double bin2 = binwidth*binwidth; 
  for (int i = 1; i <= fithist->GetNbinsX(); i++)
  {
    for (int j = 1; j <= fithist->GetNbinsY(); j++)
    {

      double valhist = fithist->GetBinContent(i,j); 
      
      double xlow = fithist->GetXaxis()->GetBinLowEdge(i); 
      double ylow = fithist->GetYaxis()->GetBinLowEdge(j); 
      double xhigh = xlow + binwidth;
      double yhigh = ylow + binwidth; 
      
 //     double valfn = fitfn->Integral(xlow,xhigh,ylow,yhigh); 
      //double valfn = fitfn->Eval((xlow+xhigh)/2, (ylow+yhigh)/2); 
      double vlow[2] = {xlow,ylow}; 
      double vhigh[2] = {xhigh,yhigh}; 
     // double valfn = fit_integrator->Integral(vlow,vhigh)/bin2 * local_gainmap->GetBinContent(i,j); 
     
      double valfn = (quickeval ? fitfn->Eval((xlow+xhigh)/2,(ylow+yhigh)/2) : fit_integrator->Integral(vlow,vhigh)/bin2) * local_gainmap->GetBinContent(i,j); 
//      double err2 = read_noise * read_noise + fabs(valfn/conversion_gain) + pow(local_gainmap_e->GetBinContent(i,j)*valfn,2); 
      double err2 = read_noise * read_noise + fabs(valhist/conversion_gain) + pow(local_gainmap_e->GetBinContent(i,j)*valhist,2); 


      //add further correction for large negative values? 
//      if (valhist < -10 * read_noise) err2 += valhist*valhist; 

     //cout <<valfn << endl; 
      chisq += pow(valfn-valhist,2)/err2; 
    }
  }

  if (isnan(chisq))
  {
    std::cout <<" chisq is nan" << std::endl; 
    printVec(NPAR,pars);

  }
  return chisq; 
}


int dmtpc::analysis::TrackFitter::fit(const SkimEvent * ev, int c, int t) 
{
  const Cluster * clust = ev->cluster(c)->getCluster(t); 
  return fit(clust->img, &clust->pixels, ev->phi(c,t), ev->range(c,t), sqrt(ev->transverse_moment(c,2,t)));
}

int dmtpc::analysis::TrackFitter::fit(const TH2 * img, const std::vector<int> * pix, double phi_guess, double range_guess, double sigma_guess)
{
  min->Clear(); 
  binwidth = img->GetXaxis()->GetBinWidth(1); 

  //assign not ridiculous values if nothing specified 
  if (phi_guess == DBL_MAX || isnan(phi_guess))
  {
    static TRandom3 rand; 
    phi_guess =rand.Rndm() * 2 * TMath::Pi(); 
  }
  if (range_guess == DBL_MAX || isnan(range_guess))
  {
    range_guess = binwidth * sqrt(pix->size())+1; 
  }
  if (sigma_guess == DBL_MAX || isnan(sigma_guess))
  {
    sigma_guess = 2 * binwidth; 
  }


  double x0 = 0,y0 = 0; 

  double max = -DBL_MAX; 
  double E = 0;

  if (enable_cauchy && enable_second_gaus)
  {
    fprintf(stderr,"Can't enable both cauchy and second gaus at same time... doing cachy only\n"); 
    enable_second_gaus = false; 
  }


  second_width_is_cauchy = enable_cauchy; 

  double xmin=0,xmax=0,ymin=0,ymax=0; 
  int xminbin = INT_MAX, yminbin = INT_MAX, xmaxbin = -INT_MAX,ymaxbin = -INT_MAX; 

  for (unsigned i = 0; i < pix->size(); i++)
  {
    int b = pix->at(i); 
    double val = img->GetBinContent(b);
    int bx,by,bz; 
    img->GetBinXYZ(b,bx,by,bz); 

    if (bx < xminbin)
    {
      xminbin = bx; 
      xmin = img->GetXaxis()->GetBinLowEdge(bx); 
    }
    if (bx > xmaxbin)
    {
      xmaxbin = bx; 
      xmax = img->GetXaxis()->GetBinLowEdge(bx)+binwidth; 
    }
    if (by < yminbin)
    {
      yminbin = by; 
      ymin = img->GetYaxis()->GetBinLowEdge(by); 
    }
    if (by > ymaxbin)
    {
      ymaxbin = by; 
      ymax = img->GetYaxis()->GetBinLowEdge(by)+binwidth; 
    }


    if (val > max)
    {
      max = val; 
      img->GetBinXYZ(b,bx,by,bz); 
      x0 = img->GetXaxis()->GetBinCenter(bx); 
      y0 = img->GetYaxis()->GetBinCenter(by); 
    }

    /*
    E +=  !gainMap ? val 
          : gainMap->GetBinContent(b) > min_gain ? val / gainMap->GetBinContent(b) 
          : 0; 
          */
    E+= val; 
  }

  if (xminbin - padding >= 1) 
  {
    xminbin -= padding; 
    xmin -= padding * binwidth; 
  }
  else 
  {
    xmin -= (xminbin-1)*binwidth;
    xminbin = 1; 
  }

  if (yminbin - padding >= 1) 
  {
    yminbin -= padding; 
    ymin -= padding * binwidth; 
  }
  else 
  {
    ymin -= (yminbin-1)*binwidth;
    yminbin = 1; 
  }

  if (xminbin + padding <= img->GetNbinsX()) 
  {
    xmaxbin += padding; 
    xmax += padding * binwidth; 
  }
  else 
  {
    xmax += (img->GetNbinsX() - xmaxbin)*binwidth;
    xmaxbin = img->GetNbinsX(); 
  }

  if (yminbin + padding <= img->GetNbinsY()) 
  {
    ymaxbin += padding; 
    ymax += padding * binwidth; 
  }
  else 
  {
    ymax += (img->GetNbinsY() - ymaxbin)*binwidth;
    ymaxbin = img->GetNbinsY(); 
  }

  if (fithist) delete fithist; 

  if (local_gainmap) delete local_gainmap; 
  if (local_gainmap_e) delete local_gainmap_e; 

  fithist = new TH2F(TString::Format("fithist_%u",fitter_id),"fithist", xmaxbin-xminbin + 1, xmin,xmax, ymaxbin-yminbin +1,ymin,ymax); 
  local_gainmap = new TH2F(TString::Format("gm_local_%u",fitter_id),"gm_local", xmaxbin-xminbin + 1, xmin,xmax, ymaxbin-yminbin +1,ymin,ymax); 
  local_gainmap_e = new TH2F(TString::Format("e_gm_local_e_%u",fitter_id),"e_gm_local_e", xmaxbin-xminbin + 1, xmin,xmax, ymaxbin-yminbin +1,ymin,ymax); 


  if (zero_out)
  {
    for (unsigned i = 0; i < pix->size(); i++)
    {
        int bin = pix->at(i); 
        int ix,iy,iz; 
        img->GetBinXYZ(bin,ix,iy,iz); 

        fithist->SetBinContent(ix-xminbin+1, iy-yminbin+1, img->GetBinContent(ix,iy)); 
        local_gainmap->SetBinContent(ix-xminbin+1, iy-yminbin+1, !gainMap ? 1 : gainMap->GetBinContent(ix,iy)); 
        local_gainmap_e->SetBinContent(ix-xminbin+1, iy-yminbin+1, (!gainMap|| !use_gainmap_error) ? 0 : fabs(gainMap->GetBinContent(ix,iy) - gainMap_blurred->GetBinContent(ix,iy))); 


    }

  }
  else
  {
    for (int ix = xminbin; ix <=xmaxbin; ix++)
    {
      for (int iy = yminbin; iy <=ymaxbin; iy++)
      {
        fithist->SetBinContent(ix-xminbin+1, iy-yminbin+1, img->GetBinContent(ix,iy)); 
        local_gainmap->SetBinContent(ix-xminbin+1, iy-yminbin+1, !gainMap ? 1 : gainMap->GetBinContent(ix,iy)); 
        local_gainmap_e->SetBinContent(ix-xminbin+1, iy-yminbin+1, (!gainMap|| !use_gainmap_error) ? 0 : fabs(gainMap->GetBinContent(ix,iy) - gainMap_blurred->GetBinContent(ix,iy))); 
      }
    }
  }




//  double x1 = x0 + range_guess * cos(phi_guess); 
//  double y1 = x0 + range_guess * cos(phi_guess); 

  srim->setIonizationFractions(1,0); 
  srim->interpolate(lengthcal * binwidth, dmtpc::physics::SRIMTable::RANGE); 


  double srimheight = srim->ionization() * gain; 
  maxheight = E/2; 
  srim->interpolate(E/gain, dmtpc::physics::SRIMTable::IONIZATION); 
  double startheight = srim->ionizationDensity()*gain*lengthcal*binwidth; 
  double start_delta = srimheight * gain - startheight; 
  if (start_delta > 0) start_delta = -startheight/2; 

  srim->interpolate(lengthcal * binwidth/2, dmtpc::physics::SRIMTable::RANGE); 
  minheight = srim->ionization() * gain * 0.75 ; 

  if (startheight < minheight) startheight = srimheight; 


//  min->SetVariable(PAR_E,"E",E,sqrt(E)); 
  std::  cout <<"start E: " << E << std::endl; 
  min->SetLimitedVariable(PAR_E,"E", E, sqrt(E),0,2*E); 
  min->SetLimitedVariable(PAR_X0,"x0",x0, 0.5 * sigma_guess, xmin - sigma_guess, xmax + sigma_guess); 
  min->SetLimitedVariable(PAR_Y0,"y0",y0, 0.5 * sigma_guess, ymin - sigma_guess, ymax + sigma_guess); 
  min->SetVariable(PAR_PHI,"phi",phi_guess,TMath::Pi()/16); 

  if (force_decreasing)
  {
    min->SetLimitedVariable(PAR_Z0,"z0",startheight,minheight/4, minheight, maxheight); 
    min->SetLimitedVariable(PAR_DELTA_Z,"delta_z",start_delta,minheight/4, minheight-maxheight, 0); 
  }
  else 
  {
    min->SetVariable(PAR_Z0,"z0",startheight,minheight/4); 
    min->SetVariable(PAR_DELTA_Z,"delta_z",start_delta,minheight/4); 
  }
  min->SetLimitedVariable(PAR_SIGMA2,"sigma2",pow(sigma_guess,2), sigma_guess,0, binwidth *binwidth* (pow(fithist->GetNbinsX(),2.) + pow(fithist->GetNbinsY(),2.)) ); 


  if (enable_cauchy)
  {
    min->SetLimitedVariable(PAR_SECOND_WIDTH,"second_width",sigma_guess/4,sigma_guess/8, 0, 0.5*binwidth *binwidth* (pow(fithist->GetNbinsX(),2.) + pow(fithist->GetNbinsY(),2.)) ); 
    min->SetLimitedVariable(PAR_SECOND_WIDTH_RATIO,"second_width_ratio",0.1, 0.1,0, 10); 
  }
  else if (enable_second_gaus)
  {
    min->SetLimitedVariable(PAR_SECOND_WIDTH,"second_width",2*sigma_guess,sigma_guess/2, 0, 0.5*binwidth *binwidth* (pow(fithist->GetNbinsX(),2.) + pow(fithist->GetNbinsY(),2.)) ); 
    min->SetLimitedVariable(PAR_SECOND_WIDTH_RATIO,"second_width_ratio",0.1, 0.1,0, 10); 
  }
  else
  {
    min->SetFixedVariable(PAR_SECOND_WIDTH,"second_width",0);
    min->SetFixedVariable(PAR_SECOND_WIDTH_RATIO,"second_width_ratio",0); 
  }


  if (!result) result = new Result; 
  result->success = min->Minimize(); 
  min->Hesse(); 

  if (verbose)
  {
    min->PrintResults(); 
  }


  fillResultParams(); 

  result->integralProb = do_integral; 
  result->likelihoodProb= do_likelihood; 

  if (do_integral)
  {

//#ifdef HAVE_MATHMORE
    ROOT::Math::GSLMCIntegrator gator(ROOT::Math::MCIntegration::kVEGAS,integral_abstol,integral_reltol,integral_ncalls);  

    // parameters for integral: 

    // xoffset 
    // z0 
    // z1
     
    double gator_maxes[3] = 
    {
      2*result->sigma.val, 
      maxheight, 
      maxheight

    }; 

    double gator_mins[3] = 
    {
      -2*result->sigma.val, 
      minheight,
      minheight
    }; 



    srimLineFitIntegralParams gator_pars; 
    gator_pars.fitter = this; 
    gator_pars.phi = result->phi.val;
    gator_pars.flip = result->flip; 
    gator_pars.x0 = result->x0.val; 
    gator_pars.y0 = result->y0.val; 
    gator_pars.sigma = result->sigma.val; 
    gator_pars.E = result->E.val; 
    gator_pars.scale = result->chisq; 


    if (verbose)
    {
      for (size_t par = 0; par < 3; par++)
      {
        std::cout << "par " << par <<" integration limits:  (" << gator_mins[par] << " , " << gator_maxes[par] << ")"  <<std::endl; 
      }
    }

    //do  total likely integral 
    bool q = quickeval; 
    quickeval = true; 
    gator_pars.which = srimLineFitIntegralParams::SAME;  
    result->likelyIntegralSame = gator.Integral(&SRIMLineFitFnIntegral,3,gator_mins, gator_maxes, (void *) &gator_pars); 
    result->likelyIntegralSameError = gator.Sigma(); 
    gator_pars.which = srimLineFitIntegralParams::ALL; 
    result->likelyIntegralTotal = gator.Integral(&SRIMLineFitFnIntegral,3,gator_mins, gator_maxes, (void *) &gator_pars); 
    result->likelyIntegralTotalError = gator.Sigma(); 
    result->prob = result->likelyIntegralSame / result->likelyIntegralTotal; 
//    result->probError = DmtpcMath::quotientError(result->likelyIntegralSame,result->likelyIntegralTotal,result->likelyIntegralSameError, result->likelyIntegralTotalError); 
    quickeval = q; 

//#else
//    std::cerr <<"Integration not supported without HAVE_MATHMORE" <<std::endl; 
//    assert(0); 
//#endif
  }
  else
  {
    result->likelyIntegralTotal = 0; 
    result->likelyIntegralTotalError = 0; 
    result->likelyIntegralSame = 0; 
    result->likelyIntegralSameError = 0; 
    result->prob = -1; 
    result->probError = 0; 
  }

  if (do_likelihood && result->success)
  {
   while(true)
   {
     min->SetFixedVariable(PAR_E,"E", result->fitE.val);
     min->SetVariable(PAR_X0,"x0",result->x0.val,result->x0.err); 
     min->SetVariable(PAR_Y0,"y0",result->y0.val,result->y0.err); 
     min->SetFixedVariable(PAR_PHI,"phi",result->phi.val); 
     min->SetLimitedVariable(PAR_Z0,"z0",result->z0.val,minheight/4, minheight, maxheight); 
     min->SetLimitedVariable(PAR_DELTA_Z,"delta_z",-result->delta_z.val, result->delta_z.err,
               result->delta_z.val > 0 ? minheight-maxheight : 0 , result->delta_z.val > 0 ? 0 : maxheight-minheight); 

     min->SetFixedVariable(PAR_SIGMA2,"sigma2",result->sigma2.val); 
     min->SetFixedVariable(PAR_SECOND_WIDTH,"second_width",result->second_width.val); 
     min->SetFixedVariable(PAR_SECOND_WIDTH_RATIO,"second_width_ratio",result->second_width_ratio.val); 

     bool alt_success=  min->Minimize(); 
     min->Hesse(); 
     if (verbose)
     {
       min->PrintResults(); 
     }


     result->alt_chisq = min->MinValue(); 


     // ruh roh 
     if (likelihood_flip && alt_success && result->alt_chisq < result->chisq)
     {
       printf("FOUND BETTER MINIMUM!!! \n"); 
       result->alt_chisq = result->chisq; 
       fillResultParams(true); 
     }
     else
     {
       break; 
     }

   }


   result->prob2 =  1./ (1 + exp((-result->alt_chisq + result->chisq)/2.)); 
   if (isnan(result->prob2))
   {
     result->prob2 = result->alt_chisq > result->chisq ? 1 : 0; 
   }

  }
  else
  {

     result->alt_chisq = 0; 
     result->prob2 = 0; 
  }


  if (draw)
  {

//    fitfn->SetParameters(pars); 

    gStyle->SetOptStat(0); 
    //if (!canvas) canvas = new TCanvas("trackfitter_c","Track Fitter Canvas", 1800,1000); 
    if (!canvas) canvas = new TCanvas(TString::Format("trackfitter_c_%u",fitter_id),"Track Fitter Canvas", 1800,1000); 
    canvas->Clear(); 
    canvas->Divide(3,2); 

    canvas->cd(1); 
    fithist->SetTitle("Data"); 
    fithist->GetXaxis()->SetTitle("pixels"); 
    fithist->GetYaxis()->SetTitle("pixels"); 
    fithist->Draw("colz"); 

    canvas->cd(2); 
    TH2 * fnhist = (TH2*) fithist->Clone(); 

    fnhist->GetXaxis()->SetTitle("pixels"); 
    fnhist->GetYaxis()->SetTitle("pixels"); 


    TH2 * residualhist = (TH2*) fithist->Clone(); 

    double bin2 = binwidth * binwidth; 
    for (int i = 1; i <=fithist->GetNbinsX(); i++)
    {
      for (int j = 1; j <=fithist->GetNbinsY(); j++)
      {
        double xlow = fithist->GetXaxis()->GetBinLowEdge(i); 
        double ylow = fithist->GetYaxis()->GetBinLowEdge(j); 
        double xhigh = xlow + binwidth;
        double yhigh = ylow + binwidth; 
//        fnhist->SetBinContent(i,j,fitfn->Integral(xlow,xhigh,ylow,yhigh)); 
//        fnhist->SetBinContent(i,j,fitfn->Eval((xlow+xhigh)/2,(ylow+yhigh)/2)); 
        double vlow[2] = {xlow,ylow}; 
        double vhigh[2] = {xhigh,yhigh}; 
        double valfn = (quickeval ? fitfn->Eval((xlow+xhigh)/2, (ylow+yhigh)/2) : fit_integrator->Integral(vlow,vhigh)/bin2) * local_gainmap->GetBinContent(i,j); 
        fnhist->SetBinContent(i,j,valfn); 
        //double err2 = read_noise * read_noise + fabs(valfn/conversion_gain) + pow(local_gainmap_e->GetBinContent(i,j)*valfn,2); 
        double err2 = read_noise * read_noise + fabs(fithist->GetBinContent(i,j)/conversion_gain) + pow(local_gainmap_e->GetBinContent(i,j)*fithist->GetBinContent(i,j),2); 
        //add further correction for large negative values? 
        double valhist = fithist->GetBinContent(i,j); 
//        if (valhist < -10 * read_noise) err2 += valhist*valhist; 

        residualhist->SetBinContent(i,j,pow(valfn-valhist,2)/err2); 
      }
    }

    new (arrow) TArrow( result->x0.val, result->y0.val, result->x0.val + result->range.val * cos(result->phi.val), result->y0.val + result->range.val * sin(result->phi.val), 0.1, result->flip ? "<" : ">"); 

    fnhist->SetTitle("Best-Fit Model"); 
    fnhist->SetMaximum(fithist->GetMaximum()); 
    fnhist->DrawCopy("colz"); 
    arrow->Draw(); 

    canvas->cd(3); 

    residualhist->SetTitle("Per-bin #chi^{2} value"); 
    residualhist->SetName("chisq"); 
    residualhist->DrawCopy("colz"); 

    delete fnhist; 
    delete residualhist; 

    canvas->cd(4); 

    TPoints ctr(result->x0.val, result->y0.val); 
    
    TH2 * rotated = dmtpc::image::transform::rotateInterpolate(fithist, result->phi.val, &ctr,0, "bicubic"); 
    TH2 * rotated_gm = dmtpc::image::transform::rotateInterpolate(local_gainmap, result->phi.val, &ctr,0, "bicubic"); 
    TH2 * norml =  (TH2*) local_gainmap->Clone("norml"); 

    for (int i = 1; i <= norml->GetNbinsX(); i++)
    {
      for (int j = 1; j <= norml->GetNbinsY(); j++)
      {
        norml->SetBinContent(i,j,1); 
      }
    }
 
    TH2 * rotated_norml = dmtpc::image::transform::rotateInterpolate(norml,result->phi.val, &ctr ,0,"bicubic"); 

   
    TH1 * longi = rotated->ProjectionX("long"); 
    TH1 * longi_gm = rotated_gm->ProjectionX("long_gm"); 
    TH1 * longi_norml = rotated_norml->ProjectionX("long_norml"); 
//    longi_gm->Divide(longi_norml); 
    double average_gain = longi_gm->Integral()/longi_norml->Integral(); 
    std::cout << "average gain: " << average_gain << std::endl; 

    TF1 func1d("func1d", &fn1d_impl, longi->GetXaxis()->GetXmin(), longi->GetXaxis()->GetXmax(), 7); 

    double m = (result->delta_z.val*binwidth) / (result->range.val); 
    double b = result->z0.val- m*result->x0.val/binwidth;  
    double line_params[]  = {m/binwidth*average_gain,b*average_gain,result->x0.val,result->x0.val+result->range.val,result->sigma.val, result->second_width.val, result->second_width_ratio.val}; 
    func1d.SetParameters(line_params); 

    longi->SetMaximum(TMath::Max(result->z0.val,result->z1.val) * 1.25*average_gain); 
    longi->SetTitle("Longitudinal Projection"); 
    longi->DrawCopy(); 
    func1d.DrawCopy("lpsame"); 
    TF1 funcline("funcline","pol1",result->x0.val, result->x0.val+result->range.val); 
    funcline.SetParameters(b*average_gain,m/binwidth*average_gain); 
    funcline.SetLineColor(3); 
    funcline.DrawCopy("lpsame"); 
    double * pdfx = new double[longi->GetNbinsX()]; 
    double * pdfy = new double[longi->GetNbinsX()]; 

    double a_coeff = 1./(1.+result->second_width_ratio.val); 
    double b_coeff = 1 - a_coeff; 
    for (int ii = 1; ii <= longi->GetNbinsX(); ii++)
    {
      pdfx[ii-1] = longi->GetBinCenter(ii);  
      pdfy[ii-1] = func1d.Integral(longi->GetBinLowEdge(ii), longi->GetBinLowEdge(ii+1))/longi_gm->GetBinContent(ii) * gain * longi->GetBinWidth(ii); 
                  
    }
      
    TGraph  * g = new TGraph(longi->GetNbinsX(), pdfx, pdfy); 
    g->SetMarkerColor(4); 
    g->Draw("*psame"); 


    double pts_x[2]={result->x0.val, result->x0.val + result->range.val}; 
    double pts_y[2]={result->z0.val*average_gain, result->z1.val*average_gain}; 
    double pts_x_err[2]={result->x0.err, sqrt(pow(result->x0.err,2) + pow(result->range.err,2))}; 
    double pts_y_err_dn[2]={fabs(result->z0.errDn)*average_gain, fabs(result->z1.errDn)*average_gain}; 
    double pts_y_err_up[2]={result->z0.errUp*average_gain, result->z1.errUp*average_gain}; 

    TGraphAsymmErrors * pts = new TGraphAsymmErrors(2,pts_x,pts_y,pts_x_err,pts_x_err,pts_y_err_dn,pts_y_err_up); 
    pts->SetLineColor(8); 
    pts->SetMarkerColor(3); 
    pts->Draw("psame"); 

    canvas->cd(5); 


    TH1 * tranv = rotated->ProjectionY("tranv"); 
    tranv->SetTitle("Transverse Projection"); 
    TH1 * tranv_gm = rotated_gm->ProjectionY("tranv_gm"); 
    TH1 * tranv_norml = rotated_norml->ProjectionY("tranv_normal"); 
    tranv_gm->Divide(tranv_norml); 
    double * pdf_t_x = new double [tranv->GetNbinsX()]; 
    double * pdf_t_y = new double [tranv->GetNbinsX()]; 

    TF1 gaus("gaus2", enable_cauchy ? "gaus(0) + [3] * TMath::CauchyDist(x,[4],[5])" : "gaus(0) + gaus(3)", tranv->GetXaxis()->GetXmin(), tranv->GetXaxis()->GetXmax()); 
    gaus.SetParameters(a_coeff * average_gain*min->X()[PAR_E] *binwidth / sqrt(2*TMath::Pi()) / result->sigma.val,  result->y0.val, result->sigma.val,
                       b_coeff == 0 ? 0 : b_coeff  * average_gain*min->X()[PAR_E] *binwidth * (enable_cauchy ? 1. : (1./sqrt(2*TMath::Pi()) / result->second_width.val)),  result->y0.val, result->second_width.val); 
        

    for (int ii = 1; ii <= tranv->GetNbinsX(); ii++)
    {
      pdf_t_x[ii-1] = tranv->GetBinCenter(ii); 
      pdf_t_y[ii-1] = a_coeff* (TMath::Erf((tranv->GetBinLowEdge(ii+1) - result->y0.val)/(result->sigma.val * sqrt(2))) - 
                                  TMath::Erf((tranv->GetBinLowEdge(ii) - result->y0.val)/(result->sigma.val * sqrt(2))) ) * min->X()[PAR_E] / (sqrt(2*TMath::Pi())) * tranv_gm->GetBinContent(ii) +  
                                    (enable_cauchy ? b_coeff* (TMath::ATan((tranv->GetBinLowEdge(ii+1) - result->y0.val)/(result->second_width.val))) - 
                                            TMath::ATan((tranv->GetBinLowEdge(ii) - result->y0.val)/(result->second_width.val))  * min->X()[PAR_E] * tranv_gm->GetBinContent(ii)
                                     : b_coeff* (TMath::Erf((tranv->GetBinLowEdge(ii+1) - result->y0.val)/(result->second_width.val * sqrt(2))) - 
                                       TMath::Erf((tranv->GetBinLowEdge(ii) - result->y0.val)/(result->second_width.val * sqrt(2)))) );  
    }

    TGraph * gt = new TGraph(tranv->GetNbinsX(), pdf_t_x, pdf_t_y); 
    gt->SetMarkerColor(4); 
    tranv->DrawCopy(); 
    gt->Draw("psame"); 

    gaus.Print(); 
    gaus.DrawCopy("lpsame"); 

    canvas->cd(6); 

    TPaveText * pt = new TPaveText(0.05,0.2,0.95,0.95); 
    pt->AddText( TString::Format("#chi^{2}/ndof : %g/%d  = %g", result->chisq, result->ndof, result->chisq/result->ndof)); 
    //pt->AddText( TString::Format("x0: %g +/-%g (%g,%g)", result->x0.val, result->x0.err, result->x0.errDn, result->x0.errUp)); 
    pt->AddText( TString::Format("x_{0}: %g +/-%g", result->x0.val, result->x0.err)); 
    //pt->AddText( TString::Format("y0: %g +/-%g (%g,%g)", result->y0.val, result->y0.err, result->y0.errDn, result->y0.errUp)); 
    pt->AddText( TString::Format("y_{0}: %g +/-%g", result->y0.val, result->y0.err)); 
    pt->AddText( TString::Format("s_{0}: %g +/-%g (%g,%g)", result->z0.val, result->z0.err, result->z0.errDn, result->z0.errUp)); 
    pt->AddText( TString::Format("#Delta s: %g +/-%g (%g,%g)", result->delta_z.val, result->delta_z.err, result->delta_z.errDn, result->z1.errUp)); 
    pt->AddText( TString::Format("s_{1}: %g +/-%g (%g,%g)", result->z1.val, result->z1.err, result->z1.errDn, result->z1.errUp)); 
    //pt->AddText( TString::Format("phi: %g +/-%g (%g,%g)", result->htphi.val, result->phi.err, result->phi.errDn, result->phi.errUp)); 
    pt->AddText( TString::Format("#phi: %g +/-%g", result->htphi.val, result->phi.err)); 
   // pt->AddText( TString::Format("#sigma: %g +/-%g (%g,%g)", result->sigma.val, result->sigma.err, result->sigma.errDn, result->sigma.errUp)); 
    pt->AddText( TString::Format("#sigma: %g +/-%g pixels", result->sigma.val, result->sigma.err)); 
    pt->AddText( TString::Format("#%s: %g +/-%g pixels", enable_cauchy ? "gamma" : "sigma_{b}", result->second_width.val, result->second_width.err)); 
    pt->AddText( TString::Format("#%s/#sigma: %g +/-%g", enable_cauchy ? "gamma"  :"sigma_{b}", result->second_width_ratio.val, result->second_width_ratio.err)); 
    pt->AddText( TString::Format("E: %g +/- %g adu (%g +/- %g keVee)", result->E.val, result->E.err, result->Ephys.val, result->Ephys.err)); 
    pt->AddText( TString::Format("Range: %g +/- %g pixels (%g +/- %g mm)", result->range.val, result->range.err, result->range.val * lengthcal, result->range.err * lengthcal)); 
    pt->AddText( TString::Format("Minos?: %s  LikelihoodIntegral?: %s", result->minos ? "true" : "false", do_integral ? "true" : "false"));
//     pt->AddText(TString::Format("Z-test Probability: %g", result->probFast));  
    if (do_integral)
    {
      pt->AddText(TString::Format("Implied Probability (Integral): %g +/- %g", result->prob, result->probError));  
    }
    if (do_likelihood)
    {
      pt->AddText(TString::Format("Implied Probability (Likelihood ratio): %g", result->prob2));  
    }


    pt->Draw(); 

    delete pdfx;
    delete pdfy;
    delete pdf_t_x;
    delete pdf_t_y;
    delete tranv;
    delete longi; 
    delete longi_gm; 
    delete longi_norml; 
    delete tranv_gm; 
    delete tranv_norml; 
    delete rotated; 
    delete rotated_gm; 
    //delete g; 
  }


  if (verbose)
  {
    std::cout <<  "Chisq/ndof: " << result->chisq / result->ndof << std::endl <<
                  "x0: " << result->x0.val << " +/-" << result->x0.err << std::endl <<
                  "y0: " << result->y0.val << " +/-" << result->y0.err << std::endl <<
                  "z0: " << result->z0.val << " +/-" << result->z0.err << std::endl <<
                  "deltaz: " << result->delta_z.val << " +/-" << result->delta_z.err << std::endl <<
                  "z1: " << result->z1.val << " +/-" << result->z1.err << std::endl <<
                  "range: " << result->range.val << " +/-" << result->range.err << std::endl <<
                  "E: " << result->E.val << std::endl <<
                  "phi " << result->htphi.val << std::endl <<
                  "fast probability " << result->probFast << std::endl; 

    if (do_integral)
    {

     std::cout << " total integral: " << result->likelyIntegralTotal << " +/- " << result->likelyIntegralTotalError << 
                  " same side integral: " << result->likelyIntegralSame << " +/- " << result->likelyIntegralSameError << 
                  " implied probability: " << result->prob <<  " +/- " << result->probError << std::endl; 
    }

    if (do_likelihood)
    {

     std::cout << " alt chisq: " << result->alt_chisq << 
                  " implied probability (likelihood): " << result->prob2 << std::endl; 
    }


 }



  return 0; 

}
