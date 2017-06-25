#include "ViewerImageTransform.hh"
#include "ImageCalculations.hh"
#include "ClusterAlgo.hh"
#include "GainMap.hh"
#include "TROOT.h"
#include "TFile.h"
#include <iostream>
#include "ClusterImage.hh"
#include "ClusterFinding.hh"
#include <stdio.h>
#include <cstdlib>
#include "TPoints.h"
#include <map>
#include "TGMsgBox.h"
#include "TH2.h"
#include "ImageTransforms.hh"
#include "LensCorrection.hh"
#include "ImageProcessing.hh"
#include "TGComboBox.h"
#include "TGFileDialog.h"
#include "TTimeStamp.h" 
#include "TGraph.h"

ClassImp(::dmtpc::viewer::ViewerImageTransform); 
ClassImp(::dmtpc::viewer::ViewerImageTransform::Parameter); 

// this file needs comments 

static char * edge_detect_types[] = {"sobel","prewitt","scharr"}; 
static char * interpolation_types[] = {"bilinear","bicubic","nearest"}; 
static char * aniso_diff_types[] = {"gaussian","lorentzian","tukey"}; 
static char * bilateral_fn_types[] = {"gaussian","lorentzian","box","triangle","tukey"}; 


static char * houghTransformLinearFn(const TH2 * img, const std::vector<void*> * params)
{
  int pi = 0; 
  
  int nrbins = *((int*) params->at(pi++)); 
  int nthetabins = *((int*) params->at(pi++)); 
  double valmin = *((double*) params->at(pi++)); 
  TH2 * hough = dmtpc::image::transform::houghTransformLine((TH2*)img,0,valmin,nthetabins,nrbins);
  hough->DrawCopy("colz"); 
  hough->Delete(); 

  return 0; 
}


static char * medianFn(const TH2 * img, const std::vector<void*> * params)
{
  int pi = 0; 
  int radius = *((int*) params->at(pi++)); 
  int iter = *((int*) params->at(pi++)); 

  TH2 * median = dmtpc::image::processing::medianFilter(img,radius,iter); 
  median->DrawCopy("colz"); 
  median->Delete(); 

  return 0; 
}

static char * lensFn(const TH2 * img, const std::vector<void*> * params)
{
  double polyn[5]; 
  int max_order = 0; 
  for (int i = 0; i < 5; i++)
  {
    polyn[i] = *((double*) params->at(i)); 
    if (polyn[i] !=0) max_order = i; 
  }
  bool distort = *((bool*) params->at(5)); 

  dmtpc::image::LensCorrection corr("lenscorr",max_order);
  for (int i = 0; i <= max_order; i++)
  {
//    std::cout << i << " " <<polyn[i] << std::endl; 
    corr.setParameter(i,polyn[i]); 
  }
  TH2 * corrected = corr.correctDistortion(img, 0,"bicubic",0,distort); 

  corrected->DrawCopy("colz"); 
  corrected->Delete(); 

  return 0; 
}

static char * radonTransformFn(const TH2 * img, const std::vector<void*> * params)
{
  int nthetabins = *((int*) params->at(0));  
  TH2 * radon = dmtpc::image::transform::radonTransform(img, nthetabins); 
  radon->DrawCopy("colz"); 
  radon->Delete(); 
  return 0; 
}

static char * rotateFn(const TH2 * img, const std::vector<void*> * params)
{
  double theta = *((double*) params->at(0));  
  double xorig = *((double*) params->at(1));  
  double yorig = *((double*) params->at(2));  
  int type = *((int*) params->at(3)) ;
  TPoints pts(xorig,yorig); 
  TH2 * rotated = dmtpc::image::transform::rotateInterpolate(img, theta,&pts,0,interpolation_types[type]); 
  rotated->DrawCopy("colz"); 
  rotated->Delete(); 
  return 0; 
}


static char * anisotropicDiffusionFn(const TH2 * img, const std::vector<void*> * params)
{
  int pi = 0; 
  double lambda = *((double*) params->at(pi++)); 
  double K = *((double*) params->at(pi++)); 
  dmtpc::image::processing::ANISOTROPIC_DIFFUSION_FN f = *((dmtpc::image::processing::ANISOTROPIC_DIFFUSION_FN *) params->at(pi++)); 
  double gradient_t = *((double*) params->at(pi++)); 
  dmtpc::image::processing::GRADIENT_OPERATOR gradop = *((dmtpc::image::processing::GRADIENT_OPERATOR*)params->at(pi++)); 
  int repeat = *((int*) params->at(pi++)); 
  bool use_diagonals = *((bool*) params->at(pi++)); 
  bool zero_out = *((bool*) params->at(pi++)); 
  bool animate = *((bool*) params->at(pi++)); 
  TCanvas * canvas = (TCanvas*) params->at(pi+1);

  TH2 * diffused; 
  if (!zero_out) 
  {
    diffused = dmtpc::image::processing::anisotropicDiffusion(img, lambda, K, f, gradient_t, gradop, use_diagonals); 
  }
  else
  {
    TH2 * copy = (TH2*) img->Clone("copy"); 
    for (int i = 1; i < copy->GetNbinsX(); i++)
    {
      for (int j = 1; j < copy->GetNbinsY(); j++)
      {
        if (copy->GetBinContent(i,j) < 0) copy->SetBinContent(i,j,0); 
      }
    }
    diffused = dmtpc::image::processing::anisotropicDiffusion(copy,lambda,K,f,gradient_t,gradop, use_diagonals); 
  }
  for (int i = 0; i < repeat -1; i++)
  {
    if (animate)
    {
      diffused->DrawCopy("colz"); 
      diffused->GetZaxis()->UnZoom(); 
      canvas->Update(); 
    }
    TH2 * next =  dmtpc::image::processing::anisotropicDiffusion(diffused, lambda, K, f, gradient_t, gradop, use_diagonals); 
    diffused->Delete(); 
    diffused = next; 
  }
  diffused->GetZaxis()->UnZoom(); 
  diffused->DrawCopy("colz"); 
  diffused->Delete(); 
  return 0; 
}

static char * gradientFn(const TH2 * img, const std::vector<void*> * params)
{
 
  double sigma = *((double*)params->at(0)); 
  int nblur = *((int*)params->at(1)); 
  dmtpc::image::processing::GRADIENT_OPERATOR type = *((dmtpc::image::processing::GRADIENT_OPERATOR*)params->at(2)); 
  bool show_orient = *((bool*) params->at(3)); 

  int nx = img->GetNbinsX(); 
  int ny = img->GetNbinsY(); 
  int xmin = (int) img->GetXaxis()->GetXmin();
  int ymin = (int) img->GetYaxis()->GetXmin();
  int xmax = (int) img->GetXaxis()->GetXmax();
  int ymax = (int) img->GetYaxis()->GetXmax();

  TH2 * M2 = new TH2F("gMag2","gMag2",nx,xmin,xmax,ny,ymin,ymax); 
  TH2S * A = new TH2S("gAng","gAng",nx,xmin,xmax,ny,ymin,ymax); 

  dmtpc::image::processing::gradient(img,M2,A,sigma,type,nblur);   

  if (show_orient) A->DrawCopy("colz"); 
  else M2->DrawCopy("colz"); 

  A->Delete();
  M2->Delete(); 

  return 0; 
}

static char * fastBilateralFilterFn(const TH2 * img, const std::vector<void*> * params) 
{

  double space_sigma = *((double *)params->at(0));
  double value_sigma = *((double *)params->at(1));
  double nsigma= *((double*)params->at(2)); 
  dmtpc::image::processing::BILATERAL_VALUE_FN type = *((dmtpc::image::processing::BILATERAL_VALUE_FN*)params->at(3)); 
  int scale_exp = *((int*) params->at(4)); 
  bool cache = *((bool*) params->at(5)); 
  if (nsigma <0) nsigma = -nsigma; 
  TH2 * blurred = dmtpc::image::processing::fastBilateralFilter(img, space_sigma, value_sigma, nsigma,type,scale_exp,cache); 
  blurred->GetZaxis()->UnZoom(); 
  blurred->DrawCopy("colz"); 
  blurred->Delete(); 
  return 0; 
}


static char * bilateralFilterFn(const TH2 * img, const std::vector<void*> * params) 
{

  double space_sigma = *((double *)params->at(0));
  double value_sigma = *((double *)params->at(1));
  double nsigma= *((double*)params->at(2)); 
  dmtpc::image::processing::BILATERAL_VALUE_FN type = *((dmtpc::image::processing::BILATERAL_VALUE_FN*)params->at(3)); 
  if (nsigma <0) nsigma = -nsigma; 
  TH2 * blurred = dmtpc::image::processing::bilateralFilter(img, space_sigma, value_sigma, nsigma,type); 
  blurred->GetZaxis()->UnZoom(); 
  blurred->DrawCopy("colz"); 
  blurred->Delete(); 
  return 0; 
}

static char * neighborRatioFn(const TH2 * img, const std::vector<void*> * params)
{
  bool abs = *((bool *) params->at(0)); 
  bool median = *((bool *) params->at(1)); 
  bool difference = *((bool *) params->at(2)); 
  TH2 * ratio = dmtpc::image::processing::neighborRatio(img,abs,median,difference); 
  ratio->GetZaxis()->UnZoom(); 
  ratio->DrawCopy("colz"); 
  ratio->Delete(); 
  return 0; 
}

static char * laplacianFn(const TH2 * img, const std::vector<void*> * params)
{
  (void) params; 
  TH2 * laplaced = dmtpc::image::processing::laplacian(img); 
  laplaced->GetZaxis()->UnZoom(); 
  laplaced->DrawCopy("colz"); 
  laplaced->Delete(); 
  return 0; 
}
static char * laplaceOfGaussianFn(const TH2 * img, const std::vector<void*> * params) 
{

  double sigma = *((double *)params->at(0));
  int nblur = *((int*)params->at(1)); 
  if (nblur <=1 || sigma <=0)
    return strdup("Positive Parameters please"); 
  TH2 * blurred = (TH2*) dmtpc::image::processing::laplacianOfGaussian(img, sigma, nblur); 
  blurred->GetZaxis()->UnZoom(); 
  blurred->DrawCopy("colz"); 
  blurred->Delete(); 
  return 0; 
}


static char * gaussianBlurFn(const TH2 * img, const std::vector<void*> * params) 
{

  double sigma = *((double *)params->at(0));
  int nblur = *((int*)params->at(1)); 
  if (nblur <=1 || sigma <=0)
    return strdup("Positive Parameters please"); 
  TH2 * blurred = (TH2*) dmtpc::image::processing::gaussianBlur(img, nblur, sigma); 
  blurred->GetZaxis()->UnZoom(); 
  blurred->DrawCopy("colz"); 
  blurred->Delete(); 
  return 0; 
}

static char * blurFn(const TH2 * img, const std::vector<void*> * params)
{
  int blurn = *((int*) params->at(0));  
  double blurfrac = *((double*) params->at(1));  
  int rebin = *((int*) params->at(2)); 
  TH2 * copy = (TH2*) img->Clone("copy"); 
  if (rebin >1) copy->Rebin2D(rebin,rebin); 
  TH2 * blurred = dmtpc::image::processing::topHatBlur(copy, blurn, blurfrac); 
  blurred->GetZaxis()->UnZoom(); 
  blurred->DrawCopy("colz"); 
  blurred->Delete(); 
  copy->Delete(); 
  return 0; 
}

static char * edgeDetectFn(const TH2 * img, const std::vector<void*> *  params) 
{
  double blur_amt = *((double*)params->at(0)); 
  if (blur_amt <=0)
  {
    return strdup("Blur must be positive"); 
  }
  double low = *((double*)params->at(1)); 
  double high = *((double*)params->at(2)); 
  dmtpc::image::processing::GRADIENT_OPERATOR type = *((dmtpc::image::processing::GRADIENT_OPERATOR*)params->at(3)); 
  int nblur = *((int*)params->at(4)); 
  TH2C * edge = dmtpc::image::processing::edgeDetect(img,blur_amt,low,high,type,nblur); 
  edge->DrawCopy("colz"); 
  edge->Delete(); 
  return 0; 
}

/*
static char * findClustersCIfn(const TH2 * img, const std::vector<void *> * params)
{
  double minsig = *((double*) params->at(0)); 
  double maxsig = *((double*) params->at(1)); 
  int minsize = *((int*) params->at(2)); 
  double mindist = *((double*) params->at(3)); 
  double blur_amt = *((double*) params->at(4)); 
  std::list<TObject*> * auxlist = (std::list<TObject*> *) params->at(5); 

  img->DrawCopy("colz"); 

  TH2 * cpy = (TH2*) img->Clone("cpy"); 
  double mean, rms_nooutliers; 
  dmtpc::image::calculations::meanRMSNoOutliers(cpy, mean, rms_nooutliers); 
  cpy->Rebin2D(2,2); 
  cpy = dmtpc::image::processing::topHatBlur(cpy,1,blur_amt); 
  
  dmtpc::analysis::ClusterImage ci(cpy, new TTimeStamp()); 
  TString ret = ""; 

  int nfound= dmtpc::analysis::findClustersLegacy(cpy,&ci, minsig, maxsig, minsize, mindist); 
  ret += nfound; 
  ret += " clusters found"; 


  ci.changeImageWithThreshold((TH2*) img->Clone("clustimg"), minsig*rms_nooutliers); 

  for (int c = 0; c < ci.getNCluster(); c++)
  {
    std::list<TGraph *> edges = ci.getClusterBoundary(c, c+2, 2, true); 
    for (std::list<TGraph*>::iterator i = edges.begin(); i!= edges.end(); i++)
    {
       auxlist->push_back(dynamic_cast<TObject*>(*i)); 
    }
  }

  cpy->Delete(); 

  return strdup(ret.Data()); 
}


static char * findClustersADfn(const TH2 * img, const std::vector<void *> * params)
{
  TString ret = "";     
  dmtpc::analysis::ClusterImage ci((TH2*) img->Clone("cpy"), new TTimeStamp()); 

  int param = 0; 
  double high = *((double*) params->at(param++)); 
  double low = *((double*) params->at(param++)); 
  double lambda = *((double*) params->at(param++)); 
  double K = *((double*) params->at(param++)); 
  dmtpc::image::processing::ANISOTROPIC_DIFFUSION_FN f = * ((dmtpc::image::processing::ANISOTROPIC_DIFFUSION_FN *) params->at(param++)); 
  double gradient_t = *((double*) params->at(param++)); 
  dmtpc::image::processing::GRADIENT_OPERATOR g = * ((dmtpc::image::processing::GRADIENT_OPERATOR *) params->at(param++)); 
  int niter = *((int*) params->at(param++)); 
  int min_neighbors = *((int*)  params->at(param++)); 
  int min_size = *((int*) params->at(param++)); 
  double min_distance = *((double*) params->at(param++)); 
  double min_rxy_global = *((double*) params->at(param++)); 
  double min_rxy_cluster = *((double*) params->at(param++)); 
  double max_join_residual = *((double*) params->at(param++)); 
  double ls_weight = *((double*) params->at(param++)); 
  char * map_key = (char *) params->at(param++); 
  char * map_file = (char *) params->at(param++); 
  double spacer_width = *((double*) params->at(param++)); 

  std::list<TObject*> * auxlist = (std::list<TObject*> *) params->at(param++); 

  dmtpc::analysis::GainMap * map = 0; 
  if (strcmp(map_file,""))
  {
    TFile fi(map_file); 
    gROOT->cd(); 
    if (!fi.IsOpen())
    {
      ret += "Bad Gainmap File, not using one\r\n"; 
    }
    else
    {
      if (fi.Get(map_key))
      {
        map = (dmtpc::analysis::GainMap*) (fi.Get(map_key))->Clone(); 
      }
      else
      {
        ret += "Key not found in gain map file, not using gain map.\r\n";
      }
    }
    fi.Close();
  }

  int nfound = dmtpc::analysis::findClustersADHysteresisGM(img,&ci, K, lambda, f, niter, gradient_t,g, 
                                                    high, low, min_neighbors,
                                                    min_size, min_distance, min_rxy_global, 
                                                    min_rxy_cluster, max_join_residual, ls_weight, map, spacer_width);
  ret += nfound; 
  ret += " clusters found"; 
  if (map) map->Delete(); 

  img->DrawCopy("colz"); 
  for (int c = 0; c < ci.getNCluster(); c++)
  {
    std::list<TGraph> edges = ci.getClusterBoundary(c, c+2, 2, true); 
    for (std::list<TGraph*>::iterator i = edges.begin(); i!= edges.end(); i++)
    {
       auxlist->push_back(dynamic_cast<TObject*>(*i)); 
    }
  }

  return strdup(ret.Data()); 
}


static char * ringClusterFindFn(const TH2 * img, const std::vector<void*> * params)
{

  MaxCamClusterImage ci((TH2*) img->Clone("cpy"), new TTimeStamp); 

  TString ret = ""; 
  int i = 0; 
  double space_sigma = *((double*) params->at(i++)); 
  double rms_sigma = *((double*) params->at(i++)); 
  double core_high = *((double*) params->at(i++)); 
  double core_low = *((double*) params->at(i++)); 
  double ring_thresh = *((double*) params->at(i++)); 
  double ring_sigma = *((double*) params->at(i++)); 
  int min_size = *((int*) params->at(i++)); 
  MaxCamImageTools::BILATERAL_VALUE_FN type = *((MaxCamImageTools::BILATERAL_VALUE_FN*)params->at(i++)); 
  int ncleanup = *((int*) params->at(i++)); 
  double min_distance = *((double*) params->at(i++)); 
  double min_rxy_global = *((double*) params->at(i++)); 
  double min_rxy_cluster = *((double*) params->at(i++)); 
  double max_join_residual = *((double*) params->at(i++)); 
  double ls_weight = *((double*) params->at(i++)); 
  char * map_key = (char *) params->at(i++); 
  char * map_file = (char *) params->at(i++); 
  double spacer_width = *((double*) params->at(i++)); 

  std::list<TObject*> * auxlist = (std::list<TObject*> *) params->at(i); 
  DmtpcGainMap * map = 0; 
  if (strcmp(map_file,""))
  {
    TFile f(map_file); 
    gROOT->cd(); 
    if (!f.IsOpen())
    {
      ret += "Bad Gainmap File, not using one\r\n"; 
    }
    else
    {
      if (f.Get(map_key))
      {
        map = (DmtpcGainMap*) (f.Get(map_key))->Clone(); 
      }
      else
      {
        ret += "Key not found in gain map file, not using gain map.\r\n";
      }
    }
    f.Close();
  }


  double mean, rms; 
  MaxCamImageTools::meanRMSNoOutliers(img, mean, rms); 

  int nfound = MaxCamImageTools::findClustersGMRing(img, &ci, 0, &rms, &mean, space_sigma, 
                                                    rms_sigma, core_high, core_low, ring_thresh,
                                                    ring_sigma, min_size, type, ncleanup, min_distance, min_rxy_global, 
                                                    min_rxy_cluster, max_join_residual, ls_weight,map,spacer_width,0); 


  ret += nfound; 
  ret += " clusters found"; 
  if (map) map->Delete(); 

  img->DrawCopy("colz"); 
  for (int c = 0; c < ci.getNCluster(); c++)
  {
    std::list<TGraph *> edges = ci.getClusterBoundary(c, c+2, 2, true); 
    for (std::list<TGraph*>::iterator i = edges.begin(); i!= edges.end(); i++)
    {
       auxlist->push_back(dynamic_cast<TObject*>(*i)); 
    }
  }

  return strdup(ret.Data()); 

}

// TODO 

*/

static char * seedClusterFindFn(const TH2 * img, const std::vector<void*> * params)
{



  TString ret = ""; 
  double seed_thresh = *((double*) params->at(0)); 
  double thresh_pct = *((double*) params->at(1)); 
  double max_wrong_p = *((double*) params->at(2)); 
  double min_thresh = *((double*) params->at(3)); 
  int blur_n = *((int*) params->at(4)); 
  double blur_sigma = *((double*) params->at(5)); 
  int neighbors_thresh = *((int*) params->at(6)); 
  int min_neigh = *((int*) params->at(7)); 
  int min_size = *((int*) params->at(8)); 
  double min_distance = *((double*) params->at(9)); 
  double min_rxy_global = *((double*) params->at(10)); 
  double min_rxy_cluster = *((double*) params->at(11)); 
  double max_join_residual = *((double*) params->at(12)); 
  double ls_weight = *((double*) params->at(13)); 
  char * map_key = (char *) params->at(14); 
  char * map_file = (char *) params->at(15); 
  double spacer_width = *((double*) params->at(16)); 
  bool reproduce_bug = *((bool*) params->at(17)); 

  std::list<TGraph> * edgelist = (std::list<TGraph> *) params->at(18); 
  dmtpc::analysis::GainMap * map = 0; 
  if (strcmp(map_file,""))
  {
    TFile f(map_file); 
    gROOT->cd(); 
    if (!f.IsOpen())
    {
      ret += "Bad Gainmap File, not using one\r\n"; 
    }
    else
    {
      if (f.Get(map_key))
      {
        map = (dmtpc::analysis::GainMap*) (f.Get(map_key))->Clone(); 
      }
      else
      {
        ret += "Key not found in gain map file, not using gain map.\r\n";
      }
    }
    f.Close();
  }

  std::vector<std::vector<int> > clusters; 
  dmtpc::analysis::clusterfinding::GMMergeParams merge; 
  merge.min_dist = min_distance; 
  merge.min_rxy_global = min_rxy_global; 
  merge.min_rxy_cluster = min_rxy_cluster; 
  merge.max_join_residual = max_join_residual; 
  merge.least_squares_weight = ls_weight;
  merge.spacer_width = spacer_width; 
  merge.map = map; 



  int nfound = dmtpc::analysis::clusterfinding::findClustersGMSeed(img,&clusters, seed_thresh, thresh_pct, max_wrong_p,
                                                    min_thresh, blur_n, blur_sigma, neighbors_thresh, 
                                                    min_neigh, min_size, &merge, reproduce_bug);
  ret += nfound; 
  ret += " clusters found"; 
  if (map) map->Delete(); 

  img->DrawCopy("colz"); 
  for (int c = 0; c < nfound; c++)
  {
    dmtpc::analysis::Cluster clust; 
    clust.pixels = clusters[c]; 
    clust.img = img; 

    dmtpc::analysis::clusteralgo::makeBoundary(&clust, edgelist, false, true, "lsame", c+2, 2);  
  }

  return strdup(ret.Data()); 
}



dmtpc::viewer::ViewerImageTransform * dmtpc::viewer::ViewerImageTransform::Factory(char * name, 
                                                                       std::list<dmtpc::viewer::ViewerImageTransform*> * store) 
{
  ViewerImageTransform * t = 0; 

  if (!strcasecmp(name,"gaussianBlur"))
  {
    std::vector<Parameter> params; 
    params.push_back(Parameter::makeDoubleParameter("Sigma",1,TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeIntParameter("Blur Kernel Radius",2,TGNumberFormat::kNEAPositive,2)); 
    t = new ViewerImageTransform(gClient->GetRoot(),800,600,"gaussianBlur", gaussianBlurFn, params, store); 
  }

  if (!strcasecmp(name,"LoG"))
  {
    std::vector<Parameter> params; 
    params.push_back(Parameter::makeDoubleParameter("Sigma",1,TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeIntParameter("Kernel Radius",2,TGNumberFormat::kNEAPositive,2)); 
    t = new ViewerImageTransform(gClient->GetRoot(),800,600,"laplacianOfGaussian", laplaceOfGaussianFn, params, store); 
  }
  if (!strcasecmp(name,"Laplacian"))
  {
    std::vector<Parameter> params; 
    t = new ViewerImageTransform(gClient->GetRoot(),800,600,"laplacian", laplacianFn, params, store); 
  }

  if (!strcasecmp(name,"neighborRatio"))
  {
    std::vector<Parameter> params; 
    params.push_back(Parameter::makeBoolParameter("Abs? ")); 
    params.push_back(Parameter::makeBoolParameter("Median? ")); 
    params.push_back(Parameter::makeBoolParameter("Difference? ")); 
    t = new ViewerImageTransform(gClient->GetRoot(),800,600,"neighborRatio", neighborRatioFn, params, store); 
  }

  if (!strcasecmp(name,"bilateralFilter"))
  {
    std::vector<Parameter> params; 
    params.push_back(Parameter::makeDoubleParameter("Space Sigma",2,TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeDoubleParameter("Value Sigma",20,TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeDoubleParameter("Kernel Size (in #sigma)",3,TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeEnumParameter("Value Function",5,bilateral_fn_types,0)); 
    t = new ViewerImageTransform(gClient->GetRoot(),800,600,"bilateralFilter", bilateralFilterFn, params, store); 
  }

  if (!strcasecmp(name,"fastBilateralFilter"))
  {
    std::vector<Parameter> params; 
    params.push_back(Parameter::makeDoubleParameter("Space Sigma",2,TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeDoubleParameter("Value Sigma",20,TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeDoubleParameter("Kernel Size (in #sigma)",3,TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeEnumParameter("Value Function",5,bilateral_fn_types,0)); 
    params.push_back(Parameter::makeIntParameter("Scale Exponent",8)); 
    params.push_back(Parameter::makeBoolParameter("Cache")); 
    t = new ViewerImageTransform(gClient->GetRoot(),800,600,"fastBilateralFilter", fastBilateralFilterFn, params, store); 
  }


  if (!strcasecmp(name,"houghTransformLine"))
  {
    std::vector<Parameter> params; 
    params.push_back(Parameter::makeIntParameter("Num r Bins",512,TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeIntParameter("Num #theta Bins",512,TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeDoubleParameter("Min value for vote",1)); 
    t = new ViewerImageTransform(gClient->GetRoot(),800,600,"houghTransformLine",houghTransformLinearFn, params, store); 
  }

  if (!strcasecmp(name,"radonTransform"))
  {
    std::vector<Parameter> params; 
    params.push_back(Parameter::makeIntParameter("Num #theta Bins",180, TGNumberFormat::kNEAPositive)); 
    t = new ViewerImageTransform(gClient->GetRoot(),800,600,"radonTransform",radonTransformFn, params, store); 
  }

  if (!strcasecmp(name,"lens"))
  {
    std::vector<Parameter> params; 
    params.push_back(Parameter::makeDoubleParameter("k0",1)); 
    params.push_back(Parameter::makeDoubleParameter("k1",0)); 
    params.push_back(Parameter::makeDoubleParameter("k2",1e-7)); 
    params.push_back(Parameter::makeDoubleParameter("k3",0)); 
    params.push_back(Parameter::makeDoubleParameter("k4",0)); 
    params.push_back(Parameter::makeBoolParameter("Distort?")); 
    t = new ViewerImageTransform(gClient->GetRoot(), 800,600,"lens",lensFn,params,store); 
  }

  if (!strcasecmp(name,"rotate"))
  {
    std::vector<Parameter> params; 
    params.push_back(Parameter::makeDoubleParameter("theta",0, TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeDoubleParameter("x0",512, TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeDoubleParameter("y0",512, TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeEnumParameter("interpolation",3,interpolation_types,0)); 
    t = new ViewerImageTransform(gClient->GetRoot(),800,600,"rotate",rotateFn, params, store); 
  }

  if (!strcasecmp(name,"blur"))
  {
    std::vector<Parameter> params; 
    params.push_back(Parameter::makeIntParameter("Blur N",1,TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeDoubleParameter("Blur Fraction" ,0.8,TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeIntParameter("Rebin NxN Before: " ,1,TGNumberFormat::kNEAPositive)); 
    t = new ViewerImageTransform(gClient->GetRoot(),800,600,"blur", blurFn, params, store); 
  }

  if (!strcasecmp(name,"edgeDetect"))
  {
    std::vector<Parameter> params; 
    params.push_back(Parameter::makeDoubleParameter("Blur Level",2,TGNumberFormat::kNEANonNegative));
    params.push_back(Parameter::makeDoubleParameter("Low Threshold",10)); 
    params.push_back(Parameter::makeDoubleParameter("High Threshold",20)); 
    params.push_back(Parameter::makeEnumParameter("Gradient Operator",3,edge_detect_types,0)); 
    params.push_back(Parameter::makeIntParameter("Blur Kernel Radius",7,TGNumberFormat::kNEAPositive,2)); 
    t = new ViewerImageTransform(gClient->GetRoot(),800,600,"edgeDetect", edgeDetectFn, params, store); 
  }

  if (!strcasecmp(name,"anisotropicDiffusion"))
  {
    std::vector<Parameter> params; 
    params.push_back(Parameter::makeDoubleParameter("Lambda",1,TGNumberFormat::kNEANonNegative)); 
    params.push_back(Parameter::makeDoubleParameter("K",25,TGNumberFormat::kNEANonNegative)); 
    params.push_back(Parameter::makeEnumParameter("Edge Function",3,aniso_diff_types,2)); 
    params.push_back(Parameter::makeDoubleParameter("Gradient Blur",1.6,TGNumberFormat::kNEANonNegative)); 
    params.push_back(Parameter::makeEnumParameter("Gradient Operator",3,edge_detect_types,0)); 
    params.push_back(Parameter::makeIntParameter("N Times",30, TGNumberFormat::kNEAPositive,2)); 
    params.push_back(Parameter::makeBoolParameter("Use Diagonals")); 
    params.push_back(Parameter::makeBoolParameter("Zero Negatives")); 
    params.push_back(Parameter::makeBoolParameter("Animate")); 
    t = new ViewerImageTransform(gClient->GetRoot(),800,600,"anisotropicDiffusion", anisotropicDiffusionFn, params, store); 
  }

  /*
  if (!strcasecmp(name, "findClustersAD")) 
  {
    std::vector<Parameter> params; 

    params.push_back(Parameter::makeDoubleParameter("High Threshold",7.3)); 
    params.push_back(Parameter::makeDoubleParameter("Low Threshold",4.5)); 
    params.push_back(Parameter::makeDoubleParameter("Lambda",1,TGNumberFormat::kNEANonNegative)); 
    params.push_back(Parameter::makeDoubleParameter("K",25,TGNumberFormat::kNEANonNegative)); 
    params.push_back(Parameter::makeEnumParameter("Edge Function",3,aniso_diff_types,2)); 
    params.push_back(Parameter::makeDoubleParameter("Gradient Blur",1.6,TGNumberFormat::kNEANonNegative)); 
    params.push_back(Parameter::makeEnumParameter("Gradient Operator",3,edge_detect_types,0)); 
    params.push_back(Parameter::makeIntParameter("NIterations",30, TGNumberFormat::kNEAPositive,2)); 
    params.push_back(Parameter::makeIntParameter("Neighbor Thresh",5, TGNumberFormat::kNEAPositive,2)); 
    params.push_back(Parameter::makeIntParameter("Minimum Size",16, TGNumberFormat::kNEANonNegative)); 
    params.push_back(Parameter::makeDoubleParameter("Minimum Distance^2",3600, TGNumberFormat::kNEANonNegative)); 
    params.push_back(Parameter::makeDoubleParameter("Min Rxy Global", 0.65)); 
    params.push_back(Parameter::makeDoubleParameter("Min Rxy Cluster", 0.8)); 
    params.push_back(Parameter::makeDoubleParameter("Max Join Residual", 5)); 
    params.push_back(Parameter::makeDoubleParameter("Least Squares Weight", 2)); 
    params.push_back(Parameter::makeStringParameter("DmtcGainMap Key", "")); 
    params.push_back(Parameter::makeFileParameter("Gain Map File", "/net/zwicky/dmtpc","*.root")); 
    params.push_back(Parameter::makeDoubleParameter("Spacer Width (in #sigma)",2)); 

    t = new ViewerImageTransform(gClient->GetRoot(),800,600,"findClustersAD", findClustersADfn, params, store); 
  }

  if (!strcasecmp(name,"findClustersCI"))
  {

    std::vector<Parameter> params; 
    params.push_back(Parameter::makeDoubleParameter("Minimum Sigma",3.7)); 
    params.push_back(Parameter::makeDoubleParameter("Maximum Sigma",300)); 
    params.push_back(Parameter::makeIntParameter("Minimum Size",5,TGNumberFormat::kNEANonNegative)); 
    params.push_back(Parameter::makeDoubleParameter("Minimum Distance^2",3600)); 
    params.push_back(Parameter::makeDoubleParameter("Blur Amount",0.8)); 
    t = new ViewerImageTransform(gClient->GetRoot(), 800,600,"findClustersCI",findClustersCIfn, params, store); 
  }


  if (!strcasecmp(name,"ringClusterFind"))
  {
    std::vector<Parameter> params; 
    params.push_back(Parameter::makeDoubleParameter("Space Sigma",2)); 
    params.push_back(Parameter::makeDoubleParameter("RMS Sigma",1)); 
    params.push_back(Parameter::makeDoubleParameter("Core High",4)); 
    params.push_back(Parameter::makeDoubleParameter("Core Low",2)); 
    params.push_back(Parameter::makeDoubleParameter("Ring Thresh",0.5)); 
    params.push_back(Parameter::makeDoubleParameter("Ring Sigma",2.5)); 
    params.push_back(Parameter::makeIntParameter("Minimum Size",16, TGNumberFormat::kNEANonNegative)); 
    params.push_back(Parameter::makeEnumParameter("Value Function",5,bilateral_fn_types,0)); 
    params.push_back(Parameter::makeIntParameter("NCleanup",1, TGNumberFormat::kNEANonNegative)); 
    params.push_back(Parameter::makeDoubleParameter("Minimum Distance^2",3600, TGNumberFormat::kNEANonNegative)); 
    params.push_back(Parameter::makeDoubleParameter("Min Rxy Global", 0.65)); 
    params.push_back(Parameter::makeDoubleParameter("Min Rxy Cluster", 0.8)); 
    params.push_back(Parameter::makeDoubleParameter("Max Join Residual", 10)); 
    params.push_back(Parameter::makeDoubleParameter("Least Squares Weight", 2)); 
    params.push_back(Parameter::makeStringParameter("DmtcGainMap Key", "")); 
    params.push_back(Parameter::makeFileParameter("Gain Map File", "/net/zwicky/dmtpc","*.root")); 
    params.push_back(Parameter::makeDoubleParameter("Spacer Width (in #sigma)",2)); 

    t = new ViewerImageTransform(gClient->GetRoot(),800,600,"ringClusterFind", ringClusterFindFn, params, store); 


  }

  */
  if (!strcasecmp(name,"seedClusterFind"))
  {

    std::vector<Parameter> params; 
    params.push_back(Parameter::makeDoubleParameter("Seed Threshold",5.2)); 
    params.push_back(Parameter::makeDoubleParameter("Threshold Reduction %",0.75)); 
    params.push_back(Parameter::makeDoubleParameter("Max Wrong P",1e-33)); 
    params.push_back(Parameter::makeDoubleParameter("Minimum Threshold ",0.75)); 
    params.push_back(Parameter::makeIntParameter("Blur Kernel Size",4,TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeDoubleParameter("Blur Sigma",2,TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeIntParameter("Neighbor Threshold",4, TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeIntParameter("Minimum Neighbors",2, TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeIntParameter("Minimum Size",16, TGNumberFormat::kNEANonNegative)); 
    params.push_back(Parameter::makeDoubleParameter("Minimum Distance^2",3600, TGNumberFormat::kNEANonNegative)); 
    params.push_back(Parameter::makeDoubleParameter("Min Rxy Global", 0.65)); 
    params.push_back(Parameter::makeDoubleParameter("Min Rxy Cluster", 0.8)); 
    params.push_back(Parameter::makeDoubleParameter("Max Join Residual", 10)); 
    params.push_back(Parameter::makeDoubleParameter("Least Squares Weight", 2)); 
    params.push_back(Parameter::makeStringParameter("DmtcGainMap Key", "")); 
    params.push_back(Parameter::makeFileParameter("Gain Map File", "/net/zwicky/dmtpc","*.root")); 
    params.push_back(Parameter::makeDoubleParameter("Spacer Width (in #sigma)",2)); 
    params.push_back(Parameter::makeBoolParameter("Reproduce v4 Bug",true)); 

    t = new ViewerImageTransform(gClient->GetRoot(), 800,600, "seedClusterFind", seedClusterFindFn, params, store); 

  }

  /*
  if (!strcasecmp(name, "seedClusterFindStitch"))
  {

    std::vector<Parameter> params; 
    params.push_back(Parameter::makeDoubleParameter("Seed Threshold",5.2)); 
    params.push_back(Parameter::makeDoubleParameter("Threshold Reduction %",0.75)); 
    params.push_back(Parameter::makeDoubleParameter("Max Wrong P",1e-33)); 
    params.push_back(Parameter::makeDoubleParameter("Minimum Threshold ",0.75)); 
    params.push_back(Parameter::makeIntParameter("Blur Kernel Size",4,TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeDoubleParameter("Blur Sigma",2,TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeIntParameter("Neighbor Threshold",4, TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeIntParameter("Minimum Neighbors",2, TGNumberFormat::kNEAPositive)); 
    params.push_back(Parameter::makeIntParameter("Minimum Size",16, TGNumberFormat::kNEANonNegative)); 
    params.push_back(Parameter::makeDoubleParameter("Minimum Distance^2",3600, TGNumberFormat::kNEANonNegative)); 
    params.push_back(Parameter::makeDoubleParameter("Min Rxy Global", 0.65)); 
    params.push_back(Parameter::makeDoubleParameter("Min Rxy Cluster", 0.8)); 
    params.push_back(Parameter::makeDoubleParameter("Max Join Residual", 10)); 
    params.push_back(Parameter::makeDoubleParameter("Least Squares Weight", 2)); 
    params.push_back(Parameter::makeStringParameter("DmtcGainMap Key", "")); 
    params.push_back(Parameter::makeFileParameter("Gain Map File", "/net/zwicky/dmtpc","*.root")); 
    params.push_back(Parameter::makeDoubleParameter("Spacer Width (in #sigma)",2)); 



  }

  */

  if (!strcasecmp(name,"gradient"))
  {
    std::vector<Parameter> params; 
    params.push_back(Parameter::makeDoubleParameter("Blur Level",2,TGNumberFormat::kNEANonNegative));
    params.push_back(Parameter::makeIntParameter("Blur Kernel Radius",7,TGNumberFormat::kNEAPositive,2)); 
    params.push_back(Parameter::makeEnumParameter("Gradient Operator",3,edge_detect_types,0)); 
    params.push_back(Parameter::makeBoolParameter("Show Orientation Instead")); 
    t = new ViewerImageTransform(gClient->GetRoot(),800,600,"gradient", gradientFn, params, store); 
  }

  if (!strcasecmp(name,"median"))
  {
    std::vector<Parameter> params; 
    params.push_back(Parameter::makeIntParameter("radius",3,TGNumberFormat::kNEANonNegative));
    params.push_back(Parameter::makeIntParameter("niter",1,TGNumberFormat::kNEANonNegative));
    t = new ViewerImageTransform(gClient->GetRoot(),800,600,"median", medianFn, params,store); 
  }
  return t; 
}

void dmtpc::viewer::ViewerImageTransform::Update()
{
   if (!current_image) return; 

   //Clear the edgelist
   _edgelist.clear(); 

   std::vector<void*> topass;  


   for (unsigned int i = 0; i < _params.size(); i++)
   {
      Parameter p = _params[i]; 
      switch(p.type)
      {
        case Parameter::param_double:
          {
            TGNumberEntry * input = (TGNumberEntry*) inputs->At(i); 
            double * d = new double[1]; 
            *d = input->GetNumber(); 
            topass.push_back(d); 
            break; 
          }
        case Parameter::param_int:
          {
            TGNumberEntry * input = (TGNumberEntry*) inputs->At(i); 
            int * d = new int[1]; 
            *d = input->GetIntNumber(); 
            topass.push_back(d); 
            break; 
          }
        case Parameter::param_string:
        case Parameter::param_file:
          {
            TGTextEntry * input = (TGTextEntry*) inputs->At(i); 
            char * s = strdup(input->GetText()); 
            topass.push_back(s); 
            break; 
          }
        case Parameter::param_bool:
          {
            TGCheckButton * input = (TGCheckButton*) inputs->At(i); 
            bool * b = new bool[1]; 
            b[0] = input->IsDown(); 
            topass.push_back(b); 
            break;
          }
        case Parameter::param_enum:
          {
            TGComboBox * input = (TGComboBox*) inputs->At(i);
            int * d = new int[1]; 
            *d = input->GetSelected(); 
            topass.push_back(d); 
            break;
          }
         default:
          break; 
      }
      //std::cout << params[i] << std::endl;
   }

   /** Always push back auxlist and canvas **/  
   topass.push_back(&_edgelist); 
   topass.push_back(canvas->GetCanvas()); 
   canvas->GetCanvas()->cd();
   watch.Start(); 
   char * result = _fp(current_image,&topass); 
   watch.Stop(); 
   std::cout << _name <<":: realtime: " << watch.RealTime() << " cputime: " << watch.CpuTime() << std::endl; 
   if (result)
   {
     TGText * text = new TGText(); 
     text->LoadBuffer(result); 
     output->SetText(text); 
     output->Layout(); 
     output->Update(); 
     free(result); 
   }

   canvas->GetCanvas()->Update();

   // free everything except auxlist 
   for (unsigned int i = 0; i < topass.size()-2; i++)
   {
      free(topass[i]); 
   }
   
}

dmtpc::viewer::ViewerImageTransform::ViewerImageTransform(const TGWindow * p, UInt_t w, UInt_t h, const char * name,
                                                             char * (*fp) (const TH2*, const std::vector<void*> *),
                                                             std::vector<Parameter> params, 
                                                             std::list<ViewerImageTransform*> * store ) 
  : TGMainFrame(p,w,h) 
{
  _params = params; 
  _fp = fp; 
  _name = strdup(name); 
  current_image = 0; 

  _store = store; 
  inputs = new TObjArray(params.size()); 

  TGHorizontalFrame * hframe = new TGHorizontalFrame(this, w,h); 

  /* left part*/ 
  TGVerticalFrame * leftframe = new TGVerticalFrame(hframe, 200, 600); 

  std::vector<Parameter>::iterator it; 
  int counter = 0; 
  for (it = params.begin(); it!= params.end(); it++)
  {
   Parameter p = *it; 
   TGHorizontalFrame * thisFrame = new TGHorizontalFrame(leftframe,200,40);  

   if (p.type!=Parameter::param_bool)
   {
     thisFrame->AddFrame(new TGLabel(thisFrame,TString(p.name) + TString(":")),
                         new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 5, 0,3,4)); 
   }

   TGFrame * thisInput; 
   switch(p.type)
   {
     case Parameter::param_double:
       thisInput = new TGNumberEntry(thisFrame,p.default_value.d,p.aux2.field_size,-1,TGNumberFormat::kNESReal, p.aux.number_type); 
       break;
     case Parameter::param_int:
       thisInput = new TGNumberEntry(thisFrame,p.default_value.i,p.aux2.field_size,-1,TGNumberFormat::kNESInteger, p.aux.number_type); 
       break;
     case Parameter::param_bool:
       thisInput = new TGCheckButton(thisFrame,p.name,-1); 
       ((TGCheckButton*)thisInput)->SetState(p.default_value.b ? kButtonDown:kButtonUp); 
       break;
     case Parameter::param_string:
       thisInput = new TGTextEntry(thisFrame,p.default_value.str,-1);
       break;
     case Parameter::param_enum:
       
       thisInput = new TGComboBox(thisFrame); 
       for (int i = 0; i < p.aux2.nchoices; i++)
       {
          ((TGComboBox*)thisInput)->AddEntry(p.aux.enum_choices[i],i);  
       }
       ((TGComboBox*)thisInput)->Select(p.default_value.i); 
       ((TGComboBox*)thisInput)->Resize(100,20); 
       break;
     case Parameter::param_file:
     { 
       TGTextButton * file_select_button = new TGTextButton(thisFrame,"..."); 
       TString fileChooseString = "FileChoose(="; 
       fileChooseString+=counter; 
       fileChooseString+=")"; 
       file_select_button->Connect("Clicked()","ViewerImageTransform",this,fileChooseString.Data()); 
       thisFrame->AddFrame(file_select_button, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY,5,0,3,4));
       thisInput = new TGTextEntry(thisFrame,"",-1); 
       break; 
     }
     default:
       thisInput = 0; 
   }

   inputs->Add(thisInput); 
   thisInput->Layout(); 
   thisFrame->AddFrame(thisInput, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 5,0,3,4));
   leftframe->AddFrame(thisFrame, new TGLayoutHints(kLHintsExpandX,3,3,3,3)); 
   counter++; 
  }

  TGTextButton * update = new TGTextButton(leftframe,"Update"); 
  update->Connect("Clicked()","dmtpc::viewer::ViewerImageTransform",this,"Update()"); 
  leftframe->AddFrame(update, new TGLayoutHints(kLHintsCenterX , 4,4,4,4)); 

  leftframe->AddFrame(new TGLabel(leftframe,"Output:"), new TGLayoutHints(kLHintsCenterX,4,4,4,4)); 
  output = new TGTextView(leftframe,200,200); 
  leftframe->AddFrame(output, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY , 4,4,4,4));;  

  TGVerticalFrame * rightframe = new TGVerticalFrame(hframe, 600,600); 

  canvas = new TRootEmbeddedCanvas("ITCanvas",rightframe,550,550);
  rightframe->AddFrame(canvas, new TGLayoutHints(kLHintsExpandY | kLHintsExpandX,4,4,4,4)); 
  hframe->AddFrame(leftframe, new TGLayoutHints(kLHintsExpandY,2,2,2,2)); 
  hframe->AddFrame(rightframe, new TGLayoutHints(kLHintsExpandY | kLHintsExpandX ,2,2,2,2)); 
  AddFrame(hframe, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2)); 
  SetWindowName(name); 
  MapSubwindows(); 
  Resize(GetDefaultSize()); 
  SetIconPixmap("/net/zwicky/dmtpc/software/viewer/icon.png");
  MapWindow(); 
}

void dmtpc::viewer::ViewerImageTransform::FileChoose(int id)
{
  TGFileInfo fi;             
  if (_params[id].aux.file_suffix)  
  {
     char * types[] = { "",_params[id].aux.file_suffix,0,0} ;
     fi.fFileTypes = const_cast<const char **>(types); 
  }
  
  fi.fIniDir = strdup(_params[id].default_value.start_dir); 
  new TGFileDialog(gClient->GetRoot(),this,kFDOpen, &fi); 
  if (fi.fFilename == NULL || !strcmp(fi.fFilename,"")) return; 
  ((TGTextEntry*)inputs->At(id))->SetText(fi.fFilename); 
}

dmtpc::viewer::ViewerImageTransform::~ViewerImageTransform()
{
  if (_store) _store->remove(this);  
  free(_name); 
}

dmtpc::viewer::ViewerImageTransform::Parameter 
dmtpc::viewer::ViewerImageTransform::Parameter::makeDoubleParameter(char * name, double default_value, TGNumberFormat::EAttribute attr, int field_size)
{
  Parameter dbl; 
  dbl.type = param_double;
  dbl.name = name; 
  dbl.default_value.d = default_value; 
  dbl.aux.number_type = attr; 
  dbl.aux2.field_size = field_size; 
  return dbl; 
}

dmtpc::viewer::ViewerImageTransform::Parameter
dmtpc::viewer::ViewerImageTransform::Parameter::makeIntParameter(char * name, int default_value, TGNumberFormat::EAttribute attr, int field_size)
{
  Parameter i; 
  i.type = param_int;
  i.name = name; 
  i.default_value.i = default_value; 
  i.aux.number_type = attr; 
  i.aux2.field_size = field_size; 
  return i; 
}

dmtpc::viewer::ViewerImageTransform::Parameter
dmtpc::viewer::ViewerImageTransform::Parameter::makeBoolParameter(char * name, bool default_value)
{
  Parameter b; 
  b.type = param_bool ;
  b.name = name; 
  b.default_value.b = default_value; 
  return b; 
}

dmtpc::viewer::ViewerImageTransform::Parameter
dmtpc::viewer::ViewerImageTransform::Parameter::makeStringParameter(char * name, char * default_value)
{
  Parameter s;
  s.type = param_string;
  s.name = name; 
  s.default_value.str = default_value; 
  return s; 
}

dmtpc::viewer::ViewerImageTransform::Parameter
dmtpc::viewer::ViewerImageTransform::Parameter::makeFileParameter(char * name, char * start_dir, char * file_suffix)  
{
  Parameter f;
  f.type = param_file;
  f.name = name; 
  f.default_value.start_dir = start_dir; 
  f.aux.file_suffix = file_suffix; 
  return f; 
}

dmtpc::viewer::ViewerImageTransform::Parameter
dmtpc::viewer::ViewerImageTransform::Parameter::makeEnumParameter(char * name, int n_choices, char ** choices, int default_choice)
{
  Parameter e;
  e.type = param_enum;
  e.name = name; 
  e.default_value.i = default_choice; 
  e.aux2.nchoices = n_choices; 
  e.aux.enum_choices = choices; 
  return e; 
}


