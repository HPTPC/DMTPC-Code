#include "MCTruthAlgo.hh"
#include "MCTruth.hh"
#include "MCTPCDataset.hh"
#include "TTree.h"
#include "Geometry.hh" 
#include "Math/WrappedTF1.h"
#include "Math/Minimizer.h"
#include "Fit/UnBinData.h"
#include "Fit/Fitter.h"
#include "TF1.h"
#include "PrincipalComponentFinder.hh" 
#include "TRandom.h" 
#include "TRandom3.h" 
#include "ConvolvedSegment.hh" 
#include "ClusterAlgo.hh" 
#include "ClusterImage.hh" 
#include <algorithm>
#include <vector>
#include <cstdio>
#include "TVector3.h" 
#include "TVector2.h" 
#include "TH2.h" 
#include "GainMap.hh" 


double dmtpc::analysis::truthalgo::max_displacement(const dmtpc::analysis::MCTruth * truth, bool flat)
{

  if (truth->primary_electron_x.size() <=1) return 0; 

  double max_dist = 0; 

  double x0 = truth->primary_electron_x[0]; 
  double y0 = truth->primary_electron_y[0]; 
  double z0 = truth->primary_electron_z[0]; 


  for (unsigned i = 1; i < truth->primary_electron_x.size(); i++)
  {

    double x = truth->primary_electron_x[i]; 
    double y = truth->primary_electron_y[i]; 
    double z = truth->primary_electron_z[i]; 
    double dist = pow(x-x0,2) + pow(y-y0,2);
    if (!flat) dist+= pow(z-z0,2); 


    if (dist > max_dist) max_dist = dist; 

  }

  return sqrt(max_dist); 

}

int dmtpc::analysis::truthalgo::maincam(const dmtpc::analysis::MCTruth * truth)
{

  int cam = -1; 
  int max = 0; 

  for (unsigned i = 0; i < truth->nadu.size(); i++)
  {
    if (truth->nadu[i] > max)
    {
      cam = i; 
      max = truth->nadu[i];
    }
  }
  return cam; 
}

bool dmtpc::analysis::truthalgo::edge(const dmtpc::analysis::MCTruth * truth, int cam)
{
  int xmax = truth->truth_images[cam]->GetNbinsX();  
  int ymax = truth->truth_images[cam]->GetNbinsY();  

  for (int i = 1; i < xmax; i++)
  {
    if (truth->truth_images[cam]->GetBinContent(i,1)) return true; 
    if (truth->truth_images[cam]->GetBinContent(i,ymax)) return true; 
  }

  for (int i = 2; i < ymax-1; i++)
  {
    if (truth->truth_images[cam]->GetBinContent(1,i)) return true; 
    if (truth->truth_images[cam]->GetBinContent(xmax,i)) return true; 
  }
  
  return false; 
}

dmtpc::math::PrincipalComponentFinder* dmtpc::analysis::truthalgo::pca_electrons_flat(const dmtpc::analysis::MCTruth * truth, dmtpc::math::PrincipalComponentFinder * pca, bool diffuse)
{
  if (pca && pca->ndim() != 2)
  {
    fprintf(stderr,"WARNING: passed PCA has wrong dimension, creating new one instead\n");
    pca = 0; 
  }

  if (!pca)
  {
    pca = new dmtpc::math::PrincipalComponentFinder(2); 
  }

  pca->clearPoints(); 

  unsigned n = diffuse ? truth->diffused_electron_x.size() : truth->primary_electron_x.size(); 
  pca->setPointsInDim(0,n, diffuse ? &truth->diffused_electron_x[0] : &truth->primary_electron_x[0]) ;
  pca->setPointsInDim(1,n, diffuse ? &truth->diffused_electron_y[0] : &truth->primary_electron_y[0]) ;

  pca->solve(); 
  return pca; 
}

dmtpc::math::PrincipalComponentFinder* dmtpc::analysis::truthalgo::pca_electrons(const dmtpc::analysis::MCTruth * truth, dmtpc::math::PrincipalComponentFinder * pca, bool diffuse)
{
  if (pca && pca->ndim() != 3)
  {
    fprintf(stderr,"WARNING: passed PCA has wrong dimension, creating new one instead\n");
    pca = 0; 
  }

  if (!pca)
  {
    pca = new dmtpc::math::PrincipalComponentFinder(3); 
  }

  pca->clearPoints(); 

  unsigned n = diffuse ? truth->diffused_electron_x.size() : truth->primary_electron_x.size(); 
  pca->setPointsInDim(0,n, diffuse ? &truth->diffused_electron_x[0] : &truth->primary_electron_x[0]) ;
  pca->setPointsInDim(1,n, diffuse ? &truth->diffused_electron_y[0] : &truth->primary_electron_y[0]) ;
  pca->setPointsInDim(2,n, diffuse ? &truth->diffused_electron_z[0] : &truth->primary_electron_z[0]) ;

  pca->solve(); 
  return pca; 
}

double  dmtpc::analysis::truthalgo::pca_hist(const TH2 * hist, double * xmean, double * ymean)
{

  if (!hist)
    return NAN; 

  dmtpc::analysis::Cluster c; 
  c.img = hist; 
  for (int i = 1; i <= hist->GetNbinsX(); i++)
  {
    for (int j = 1; j <= hist->GetNbinsY(); j++)
    {
      if (hist->GetBinContent(i,j) > 0) 
      {
        c.pixels.push_back(hist->GetBin(i,j)); 
      }
    }
  }

  dmtpc::analysis::clusteralgo::getXY(&c, xmean, ymean); 
  return dmtpc::analysis::clusteralgo::getPhiPCA(&c); 
}

double asymmetry_line_fit_sigma(double *xx, double *p)
{
  double E = p[0]; 
  double x0 = p[1]; 
  double ymax = p[2]; 
  double delta_y = p[3]; 

  double y0 = delta_y < 0 ? ymax : ymax - delta_y; 
  double y1 = delta_y < 0 ? ymax + delta_y : ymax; 

  if (y0 < 0 || y1 < 0) return -1000 * (y0*y0 + y1*y1); 

  double sigma = p[4]; 

  double range = 2*E / (y0 + y1); 
  double x1 = x0 + range; 
  double m = delta_y/range; 
  double b = y0 - m * x0; 



  double pars[5]; 
  pars[0] = m; 
  pars[1] = b; 
  pars[2] = x0; 
  pars[3] = x1; 
  pars[4] = sigma; 
  
  return dmtpc::math::lineSegmentConvolvedWithGaussian(xx, pars);  
}

double asymmetry_line_fit_nosigma(double *xx, double *p)
{
  double x = *xx; 
  double E = p[0]; 
  double x0 = p[1]; 
  double x1 = p[2]; 
  double delta_y = p[3]; 

  double range = fabs(x1-x0); 
  double sum_y = 2.*E/range; 

  double y0 = 0.5* (sum_y -  delta_y); 


  if (x <  x0 ) return 0; 
  if (x > x1) return 0; 

  double m = delta_y/range; 
  return m * (x - x0) + y0; 
}


static double asymmetry(std::vector<double> & zs, int np, double * percentiles, int * asymmetries, double sigma, TF1 * debug, TH1 * debug_h)
{
  unsigned n = zs.size(); 
  if (!np || !percentiles || !asymmetries)
  {
    np = 1; 
    percentiles = 0; 
    asymmetries = 0; 
  }

  std::sort (zs.begin(), zs.end()); 

  for (int p = 0; p < np; p++)
  {
    double percentile = percentiles ? percentiles[p] : 0; 
    int bound_low =(int) round(percentile * n);
    int bound_high = (int) round((1.-percentile) * n) - 1; 

    double min = zs[bound_low]; 
    double max = zs[bound_high]; 
    double middle = (min + max) /2. ; 

    unsigned where = std::lower_bound(zs.begin() + bound_low, zs.begin() + bound_high+1, middle) - zs.begin(); 

    int nequal = 0; 

    if (zs[where] == middle)
    {
      unsigned i = 1+where;
      while(zs[i++] == zs[where]) nequal++; 
    }

    int nfirsthalf = where + nequal/2; 
    int nsecondhalf = n - nfirsthalf; 
    int answer = nfirsthalf - nsecondhalf; 
    if (asymmetries)
    {
      asymmetries[p] = answer; 
    }
    else
    {
      return answer; 
    }
  }




  /* now do the line fit asymmetry */ 

  double asym = 0; 

  double min = zs[0]; 
  double max = zs[n-1]; 
  double range = fabs(max-min);
  printf("%f %f %f\n", min, max, range); 

  TF1 f_high(sigma ? "f_asymmetry_linefit_sigma_high" : "f_asymmetry_linefit_high", sigma ? asymmetry_line_fit_sigma : asymmetry_line_fit_nosigma, min - range/2, max + range/2, sigma ? 5 : 4) ;
  TF1 f_low(sigma ? "f_asymmetry_linefit_sigma_low" : "f_asymmetry_linefit_low", sigma ? asymmetry_line_fit_sigma : asymmetry_line_fit_nosigma, min - range/2, max + range/2, sigma ? 5 : 4) ;

  ROOT::Fit::UnBinData data(n,&zs[0]); 
 
  double fcn_low = 0; 
  double fcn_high = 0 ; 
  double asym_low = 0; 
  double asym_high = 0; 

  for (int i = 0; i < 2; i++)
  {
    TF1 & f = i == 0 ? f_high : f_low; 
    f.SetParameter(0, n); 
    f.FixParameter(0,n);


    f.SetParameter(3,0); 
    f.SetParError(3,0.1 * n/range); 


    double dx = 0;
    if (sigma)
    {
      dx = range < 2*sigma ? range/4 : sigma/2; 
      f.SetParameter(1,min + dx); 
      f.SetParError(1,range/10); 
      f.SetParameter(2,1.25*n/range); 
      f.SetParError(2,0.2 * n/range); 
      f.SetParameter(4,sigma); 
      f.SetParError(4,sigma/10); 
    }
    else
    {

      f.SetParameter(1,min); 
      f.SetParError(1,range/5); 
      f.SetParError(1,0); 
      f.SetParameter(2,max); 
      f.SetParError(2,0); 
    }

    ROOT::Math::WrappedTF1 wf(f); 
    ROOT::Fit::Fitter fit; 
    fit.Config().SetMinimizer("Minuit2","MigradImproved"); 
    fit.SetFunction(wf,false); 


    fit.Config().SetParamsSettings(f.GetNpar(), f.GetParameters(), f.GetParErrors()); 
    fit.Config().ParSettings(0).Fix(); 
    if (!sigma)
    {
      fit.Config().ParSettings(1).Fix(); 
      fit.Config().ParSettings(2).Fix(); 
    }
    else
    {
      fit.Config().ParSettings(4).SetLimits(0.5*sigma,2*sigma); 
      fit.Config().ParSettings(1).SetLimits(min,min+dx+range/2); 
      fit.Config().ParSettings(2).SetLimits(0,20*n/range); 
    }


    if (i == 0)
    {
      fit.Config().ParSettings(3).SetValue( -(n/(4.*range))); 
      fit.Config().ParSettings(3).SetLimits(-10.*n/range,0); 
    }
    else
    {
      fit.Config().ParSettings(3).SetValue(n/(4.*range)); 
      fit.Config().ParSettings(3).SetLimits(0,10*n/range); 
    }


    for (int j = 0; j < f.GetNpar(); j++)
    {

      TString lower_string = TString::Format("%f", fit.Config().ParSettings(j).LowerLimit()); 
      TString upper_string = TString::Format("%f", fit.Config().ParSettings(j).UpperLimit()); 

      printf("par %d: v=%f, stepsize = %f, limits = [%s, %s]\n", j, fit.Config().ParSettings(j).Value(), fit.Config().ParSettings(j).StepSize(), 
                                                                        fit.Config().ParSettings(j).HasLowerLimit() ? lower_string.Data(): "none",
                                                                        fit.Config().ParSettings(j).HasUpperLimit() ? upper_string.Data(): "none"); 


    }
    fit.Config().MinimizerOptions().SetPrintLevel(0); 
    fit.Config().MinimizerOptions().SetMaxFunctionCalls(2000); 
    fit.Config().MinimizerOptions().SetStrategy(1); 

    fit.LikelihoodFit(data); 
   
    if ( i == 0)
    {
      asym_high = -fit.Result().Parameter(3); 
      fcn_high = fit.Result().MinFcnValue(); 
    }
    else
    {
      asym_low = -fit.Result().Parameter(3); 
      fcn_low = fit.Result().MinFcnValue(); 
    }

    for (int j = 0; j < f.GetNpar(); j++)
    {
      f.SetParameter(j, fit.Result().Parameter(j)); 

    }
  }

  
  asym =  fcn_high < fcn_low ? asym_high : asym_low;
  if (debug) *debug =  fcn_high < fcn_low ? f_high : f_low; 
  if (debug_h)
  {
    debug_h->SetBins(n/5+1, min-range/2,max  + range/2); 
    for (unsigned z = 0; z < n; z++) debug_h->Fill(zs[z]);
  }
 

  return asym; 



}

double dmtpc::analysis::truthalgo::electron_asymmetry(const dmtpc::analysis::MCTruth * truth, const TVector3 * axis, bool diffuse, bool flat,   int npercentile, double * percentile, int * asymmetries, TF1 * debug, TH1 * debug_h)
{

  unsigned n = diffuse ? truth->diffused_electron_x.size() : truth->primary_electron_x.size(); 
  std::vector<double> zs(n);

  TVector3 vec;
  
  double tran_mean = 0; 
  double tran_mean2 = 0; 

  for (unsigned i = 0; i < n; i++) 
  {
    if (diffuse) vec.SetXYZ(truth->diffused_electron_x[i], truth->diffused_electron_y[i], flat ? 0 :  truth->diffused_electron_z[i]); 
    else  vec.SetXYZ(truth->primary_electron_x[i], truth->primary_electron_y[i], flat ? 0 : truth->primary_electron_z[i]); 
        
    if (debug|| debug_h) 
    {
//      printf("(%f %f %f) ->   ", vec.x(),vec.y(),vec.z() ); 
    }

    double projected = dmtpc::math::scalarProjection(&vec,axis); 
    double rejected = dmtpc::math::scalarRejection(&vec,axis); 

    if (debug|| debug_h) 
    {
 //     printf("%f %f\n", projected, rejected); 
    }

    zs[i] = projected; 
    tran_mean2 += rejected * rejected;
    tran_mean += rejected; 
  }
  tran_mean/=n; 

  double sigma = diffuse ? sqrt(tran_mean2/n - tran_mean*tran_mean) : 0; 

  return asymmetry(zs, npercentile, percentile, asymmetries,sigma,debug, debug_h); 
}



double dmtpc::analysis::truthalgo::hist_asymmetry(const TH2* hist, double phi,  int npercentile, double * percentile, int * asymmetries, TRandom * rand, double acceptance , TF1 * debug, TH1 * debug_h) 
{
  if (!hist) 
  {
    for (int i = 0; i < npercentile; i++) asymmetries[i] = 0; 
    return NAN; 
  }
  unsigned n = (int) hist->Integral(); 
  std::vector<double> zs; 
  zs.reserve(unsigned(n * acceptance)); 

  TVector2 vec; 
  TVector2 center; 
  double xmean = 0;//hist->GetMean(1); 
  double ymean = 0;//hist->GetMean(2); 
  double tran_mean = 0; 
  double tran_mean2 = 0; 

  TRandom3 rand2; 
  for (int i = 1; i <= hist->GetNbinsX(); i++) 
  {
    for (int j = 1; j <= hist->GetNbinsY(); j++) 
    {
      int content = (int) hist->GetBinContent(i,j); 
      if (!content) continue; 

      double x = hist->GetXaxis()->GetBinCenter(i); 
      double y = hist->GetYaxis()->GetBinCenter(j); 

      if (rand)
      {
        double xbin = hist->GetXaxis()->GetBinWidth(i); 
        double ybin = hist->GetYaxis()->GetBinWidth(j); 

        for(int c = 0; c < content; c++)
        {

          x += rand->Uniform(-xbin/2, xbin/2); 
          y += rand->Uniform(-ybin/2, ybin/2); 

          vec.Set(x-xmean,y-ymean); 
          TVector2 rotated = vec.Rotate(-phi); 
          tran_mean += rotated.Y(); 
          tran_mean2 += rotated.Y()*rotated.Y(); 

          if (acceptance < 1 && rand->Uniform() > acceptance) continue; 
          zs.push_back(rotated.X());   
        }
      }


      else
      {
        vec.Set(x-xmean,y-ymean); 
        TVector2 rotated = vec.Rotate(-phi); 
        tran_mean += content * rotated.Y(); 
        tran_mean2 += content * rotated.Y()*rotated.Y(); 

        for (int c = 0; c < content; c++)
        {
          if (acceptance < 1 && rand2.Uniform() > acceptance ) continue; 
          zs.push_back(rotated.X());   
        }
      }
    }
  }
  tran_mean/= n;

  double sigma = sqrt(tran_mean2/n - tran_mean*tran_mean); 
  return asymmetry(zs, npercentile, percentile, asymmetries,sigma, debug, debug_h); 
}


double dmtpc::analysis::truthalgo::getGain(dmtpc::analysis::MCTPCDataset * d, int cam, const char * cut, dmtpc::analysis::GainMap * gm, double mindist) 
{
  TTree * tree = d->getTree(); 
  int current_event = tree->GetReadEntry();


  int ncut = tree->Draw("Entry$",cut,"goff"); 

  double gain = 0; 
  int n = 0; 
  for (int i = 0; i < ncut; i++)
  {
    tree->GetEntry((int)tree->GetV1()[i]);
    int nadu = d->truth()->nadu_raw[cam]; 

    bool notonly = false;
    for (unsigned j = 0; j < d->truth()->nadu_raw.size(); j++)
    {

      if (int(j) != cam && d->truth()->nadu_raw[j] > 0) 
      {
        notonly = true; 
        break;
      }
    }

    if (notonly) continue; 
    if (edge(d->truth(), cam)) continue; 

    if (gm)
    {
      //check for overlap with spacers
     
      int nearest_spacer; 

      bool too_close = false; 

      for (int bx = 1; bx <= d->truth()->getTruthImage(cam)->GetNbinsX(); bx++) 
      {
        for (int by = 1; by <= d->truth()->getTruthImage(cam)->GetNbinsY(); by++) 
        {
          if (d->truth()->getTruthImage(cam)->GetBinContent(bx,by))
          {
            double x = d->truth()->getTruthImage(cam)->GetXaxis()->GetBinCenter(bx); 
            double y = d->truth()->getTruthImage(cam)->GetYaxis()->GetBinCenter(by); 
            if (gm->distanceToNearestSpacer(x,y,nearest_spacer) < mindist * gm->getSpacerWidth(nearest_spacer))
            {
              too_close = true; 
              break;
            }



          }
        }
        if (too_close) break;
      }

      if (too_close) continue; 
    }

    
    double this_gain = nadu/d->truth()->ionization;
    std::cout << this_gain << std::endl; 
    gain += this_gain;
    n++; 
  }

  gain/= n; 

  tree->GetEntry(current_event); 
  return gain; 
}

