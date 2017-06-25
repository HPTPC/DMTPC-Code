#include "Stitcher.hh"
#include "TH2.h"
#include "TPoints.h"
#include <set>
#include "ImageCalculations.hh"
#include "Pixel.hh"
#include "ImageTools.hh"
#include "TH3.h"
#include "ImageProcessing.hh"
//#include "TThread.h"
#include <iostream>
#include "ImageTransforms.hh"
#include "TSpectrum.h"
#include "TMath.h"
#include <list>
#include "TF1.h"


//#define DEBUG

ClassImp(dmtpc::analysis::Stitcher); 

void progress_fn(int row)
{
  if (row % 10)
    std::cout << "." << std::flush; 
}


dmtpc::analysis::Stitcher::~Stitcher()
{
  if (_stitch_info_256) delete _stitch_info_256; 
  if (_stitch_info_1024) delete _stitch_info_1024; 
  if (_stitch_info_512) delete _stitch_info_512; 
  if (_lens) delete _lens; 

  for (unsigned i = 0; i < _nImages; i++)
  {
    if (medianed.size() == 0) break; 
    if (medianed[i]) delete medianed[i];
    if (orig[i]) delete orig[i];
    if (edges[i]) delete edges[i];
    if (polars[i]) delete polars[i];
    if (rprojs[i]) delete rprojs[i];
  }
}

TH2* dmtpc::analysis::Stitcher::stitch(const std::vector<const TH2*> * images, const char * interpolation) const
{
 if (!isInit())
 {
   std::cerr << "Not init! " << std::endl; 
   return 0; 
 }
 if (images->size()!=_nImages)
 {
   std::cerr << "Wrong number of images! " << std::endl; 
   return 0;
 }



 std::vector<const TH2*> imgs; 

 for (unsigned i = 0; i < _nImages; i++)
 {
    imgs.push_back( _lens ? _lens->correctDistortion(images->at(i)) : images->at(i) ); 
 }

 TH2 * ret = 0; 
 if (images->at(0)->GetNbinsX() == 256)
 {
    ret = dmtpc::image::transform::montage(&(imgs[0]), _stitch_info_256, "stitched", interpolation, 'F'); 
 }

 else if (images->at(0)->GetNbinsX() == 512)
 {
    ret = dmtpc::image::transform::montage(&(imgs[0]), _stitch_info_512, "stitched", interpolation, 'F'); 
 }


 else if (images->at(0)->GetNbinsX() == 1024)
 {
    ret = dmtpc::image::transform::montage(&(imgs[0]), _stitch_info_1024, "stitched", interpolation, 'F'); 
 }
 else
 {
    //create a new stitch info just for this 
   std::vector<int> nbinsx(_nImages); 
   std::vector<int> nbinsy(_nImages); 
   std::vector<double> xmins(_nImages); 
   std::vector<double> xmaxs(_nImages); 
   std::vector<double> ymins(_nImages); 
   std::vector<double> ymaxs(_nImages); 

   for (unsigned i = 0; i < _nImages; i++)
   {
      nbinsx[i] = imgs[i]->GetNbinsX(); 
      nbinsy[i] = imgs[i]->GetNbinsY(); 
      xmins[i] =  imgs[i]->GetXaxis()->GetXmin(); 
      ymins[i] =  imgs[i]->GetYaxis()->GetXmin(); 
      xmaxs[i] =  imgs[i]->GetXaxis()->GetXmax(); 
      ymaxs[i] =  imgs[i]->GetYaxis()->GetXmax(); 
   }

   if (!_stitch_info_other ||  !std::equal(nbinsx.begin(), nbinsx.end(), _stitch_info_other->nbinsx.begin()) || !std::equal(nbinsy.begin(), nbinsy.end(), _stitch_info_other->nbinsy.begin())
        || !std::equal(xmins.begin(), xmins.end(), _stitch_info_other->xmins.begin()) || !std::equal(ymins.begin(), ymins.end(), _stitch_info_other->ymins.begin())
        || !std::equal(xmaxs.begin(), xmaxs.end(), _stitch_info_other->xmaxs.begin()) || !std::equal(ymaxs.begin(), ymaxs.end(), _stitch_info_other->ymaxs.begin())
       )
   {
      if (_stitch_info_other) delete _stitch_info_other; 


        _stitch_info_other = new dmtpc::image::StitchInfo("other",_nImages,_lens, &xmins[0],&ymins[0],&xmaxs[0],&ymaxs[0],&nbinsx[0],&nbinsy[0],&_xCenters[0], &_yCenters[0], 
                                                &_rotations[0], &_scales[0], &_weights[0], (int) imgs[0]->GetXaxis()->GetBinWidth(1)); 
   }


    ret = dmtpc::image::transform::montage(&(imgs[0]), _stitch_info_other, "stitched", interpolation, 'F'); 

 }

 if (_lens)
 {
   for (unsigned i = 0; i < _nImages; i++)
   {
      ((TH2*)imgs[i])->Delete(); 
   }
 }

 return ret; 

}

dmtpc::analysis::Stitcher::Stitcher(const char * name)
{
  if (name)
  {
    fName = strdup(name); 
  }


  _lens = 0; 
  _stitch_info_256=0;
  _stitch_info_512=0;
  _stitch_info_1024=0;
  _stitch_info_other=0;
  _isInit = false; 
  _led = 0; 
//  _led = -1; 
  _led_border = 30; 
  _led_thresh = 6000; 
  _edge_blur_level = 2.4;  
  _edge_low_thresh = -.5; 
  _edge_high_thresh = 0.1; 
  _min_edge_neighbors = 100; 
  _linear_hough_r_bins = 1024; 
  _linear_hough_theta_bins = 1024; 
  _linear_hough_min_votes = 400; 
  _spacer_join_theta_thresh = 0.4; 
  _spacer_join_r_thresh = 40; 
  _nbins_first_pass[0] = 40; 
  _nbins_first_pass[1] = 40; 
  _nbins_first_pass[2] = 400; 
  _mins_first_pass[0] = -200; 
  _mins_first_pass[1] = 800; 
  _mins_first_pass[2] = 800; 
  _maxs_first_pass[0] = 200; 
  _maxs_first_pass[1] = 1200; 
  _maxs_first_pass[2] = 1000; 
#ifdef DEBUG
  _nbins_second_pass[0] = 20; 
  _nbins_second_pass[1] = 20; 
#else
  _nbins_second_pass[0] = 200; 
  _nbins_second_pass[1] = 200; 
#endif
  _nbins_second_pass[2] = 2400; 
  _nwidths_second_pass = 1;
  _nspectr_peaks = 4; 
  _median_nbins = 3; 
  _median_niter = 1; 
  _spacer_theta_thresh = 0.4; 
  _img_high_thresh =2;
}


int dmtpc::analysis::Stitcher::getIndex(const char * serial) const
{
  for (unsigned i = 0; i < _serials.size(); i++)
  {
    if (!strcmp(serial, _serials[i].GetString().Data()))
    {
      return (int) i; 
    }
  }

  return -1; 
}


void dmtpc::analysis::Stitcher::calculateAuxHistograms(int i, const std::vector<const TH2*> * images, bool progress)
{
     TH2F * img =  _lens ? (TH2F*) _lens->correctDistortion(images->at(i)) : (TH2F*) images->at(i)->Clone("cl"); 
     if (progress) std::cout << "Working on image " << i << "(" << _serials[i].String() << ")" <<  std::endl; 
     orig.push_back(img); 

     if (_led == int(i))
     {
      if (progress) std::cout << "LED in this image " << std::endl; 

      std::set<int> pix_above; 
      dmtpc::image::pixel::selectPixelsAbove(img, &pix_above,_led_thresh); 

      if (progress) std::cout << "eroding " << std::endl; 

      for (int ii = 0; ii < _led_border; ii++)
      {
//        printf("erode %d\n",ii); 
        dmtpc::image::pixel::erode(img,&pix_above); 
      }

      if (progress) std::cout << "dilating " << std::endl; 

      for (int ii = 0; ii < 2*_led_border; ii++)
      {
        dmtpc::image::pixel::dilate(img,&pix_above); 
      }

      std::set<int> edge; 
      dmtpc::image::pixel::outerBorder(img, &pix_above,&edge); 

      double avg = 0; 
      int navg = 0;
      for (std::set<int>::iterator it = edge.begin(); it!=edge.end(); it++)
      {
        double val = img->GetBinContent(*it); 
        if (isnan(val)) continue; 
        avg+= val; 
        navg++; 
      }

      avg/=navg; 
      std::cout << "filling with " << avg << std::endl; 

      dmtpc::image::pixel::fillPixels(img, &pix_above, avg); 
    
     }


     TH2 * background = dmtpc::image::processing::gaussianBlur(img,60,20); 
     if (progress) std::cout << "Median filtering" << std::endl; 
     TH2F * median = (TH2F*) dmtpc::image::processing::medianFilter(img,_median_nbins,_median_niter);
//     median->SetDirectory(0); 
//     TH2F * median = (TH2F*) MaxCamImageTools::bilateralFilter(img,20,40);
     median->Add(background,-1); 
     background->Delete(); 
//     double img_mean = MaxCamImageTools::getMean(median); 
 //    double img_rms = MaxCamImageTools::getRMS(median); 
     std::set<int> high; 
//     MaxCamImageTools::selectPixelsAbove(median, &high,img_mean + _img_high_thresh * img_rms); 
//     MaxCamImageTools::fillPixels(median, &high, img_mean + _img_high_thresh * img_rms); 
     dmtpc::image::pixel::selectPixelsAbove(median, &high,0); 
     dmtpc::image::pixel::fillPixels(median, &high, 0); 
     medianed.push_back(median); 

     if (progress) std::cout << "Determining edge finding parameters" << std::endl; 

     TH2F * mag = (TH2F*) dmtpc::image::tools::newTH2StealSize(median,'F',"mag","mag"); 
     TH2S * ori = (TH2S*) dmtpc::image::tools::newTH2StealSize(median,'S',"ori","ori"); 
     dmtpc::image::processing::gradient(median,mag,ori,_edge_blur_level); 
     for (int j = 0; j < mag->GetSize(); j++)
     {
       mag->SetBinContent(j, sqrt(mag->GetArray()[j])); 
     }
    

     dmtpc::image::processing::fillEdges(mag,16,0);


     double mean = dmtpc::image::calculations::getMean(mag); 
     double rms = dmtpc::image::calculations::getRMS(mag); 
     double high_thresh = mean + _edge_high_thresh*rms; 
     double low_thresh = mean + _edge_low_thresh * rms;
     ori->Delete(); 
     mag->Delete(); 

     if (progress) std::cout << "Edge finding" << std::endl; 

     TH2C * edge = dmtpc::image::processing::edgeDetect(median, _edge_blur_level, low_thresh,high_thresh); 
//     edge->SetDirectory(0); 

     //filter out lone edges! 

     std::set<int> highpix; 
     dmtpc::image::pixel::selectPixelsAboveWithAtLeastNNeighbors(edge, &highpix, 0.5, _min_edge_neighbors); 
     edge->Reset(); 
     dmtpc::image::pixel::fillPixels(edge,&highpix,1); 

     edges.push_back(edge); 

    
     dmtpc::image::processing::fillEdges(edge,16,0);

 

}

bool dmtpc::analysis::Stitcher::train(const std::vector<const TH2*> *images, const TString * rotation_guess, const TString * serials, bool progress )
{

  if (images->size() < 2)
  {
    std::cerr << "Need > 1 image"; 
    return false; 
  }

  medianed.clear(); 
  edges.clear(); 
  polars.clear(); 
  rprojs.clear(); 
  orig.clear(); 





  for (unsigned i = 0; i < images->size(); i++)
  {
     _serials.push_back(TObjString(serials[i])); 

     calculateAuxHistograms(i, images, progress); 

     TH2 * edge = edges[i]; 
     if (progress) std::cout << "Finding spacers" << std::endl; 
     TH2I * linhough = (TH2I*) dmtpc::image::transform::houghTransformLine(edge,0,1,_linear_hough_r_bins,_linear_hough_theta_bins); 


     //Do nonmaximum suppression 
     
     TH2I * suppressed = (TH2I*) dmtpc::image::processing::nonMaximumSuppress(linhough); 

     //Find lines! 
     
     std::list<std::pair<double,double> > raw_spacers; 

     for (int x = 1 ; x<= suppressed->GetNbinsX(); x++) 
     {
       for (int y = 1 ; y<= suppressed->GetNbinsY(); y++) 
       {
         if (suppressed->GetBinContent(x,y) > _linear_hough_min_votes)
         {
           double theta = suppressed->GetXaxis()->GetBinLowEdge(x); 
           double r = suppressed->GetYaxis()->GetBinLowEdge(y); 
            
           bool push = false; 
           if (rotation_guess[i] == "HALF_TURN" || rotation_guess[i] == "NONE")
           {
              if (fabs(cos(theta)) < _spacer_theta_thresh)
                push = true; 
           }
           else
           {
              if (fabs(sin(theta)) < _spacer_theta_thresh)
                push = true; 
           }

           if (push)
             raw_spacers.push_back(std::pair<double,double>(r,theta)); 
         }
       }
     }


     //Process the raw spacers into bins of similar lines. 

     std::list<std::list<std::pair<double,double> > > spacers_bins; 
     

     double theta_dist = _spacer_join_theta_thresh; 
     double r_dist = _spacer_join_r_thresh; 
     for (std::list<std::pair<double,double> >::iterator rawi = raw_spacers.begin(); rawi!=raw_spacers.end(); rawi++) 
     {
       bool placed = false; 
       for (std::list<std::list<std::pair<double,double> > >::iterator bini = spacers_bins.begin(); bini!= spacers_bins.end(); bini++)
       {
          for( std::list<std::pair<double,double> >::iterator groupi = (*bini).begin(); groupi!=(*bini).end(); groupi++)
          {
            if (fabs((*rawi).first - (*groupi).first) < r_dist && fabs((*rawi).second - (*groupi).second) < theta_dist)
            {
               placed = true;
               (*bini).push_back(*rawi);  
               break; 
            }
          }

          if (placed) break; 
       }

       if (!placed)
       {
          std::list<std::pair<double,double> > new_bin; 
          new_bin.push_back(*rawi); 
          spacers_bins.push_back(new_bin); 
       }
     }

    _spacers_r.push_back(std::vector<double>()); 
    _spacers_theta.push_back(std::vector<double>()); 
    for (std::list<std::list<std::pair<double,double> > >::iterator bini = spacers_bins.begin(); bini!= spacers_bins.end(); bini++)
    {
      int n = 0; 
      double rsum = 0; 
      double sin_sum = 0;
      double cos_sum = 0;
      for( std::list<std::pair<double,double> >::iterator groupi = (*bini).begin(); groupi!=(*bini).end(); groupi++)
      {
         rsum += (*groupi).first;  
         sin_sum += sin((*groupi).second); 
         cos_sum += cos((*groupi).second); 
         n++; 
      }

      double r= rsum/n; 
      double theta = atan2(sin_sum,cos_sum);

      _spacers_r[i].push_back(r); 
      _spacers_theta[i].push_back(theta); 
      double m = -1./tan(theta); 
      double b = r/sin(theta); 
      if (progress) std::cout << m << " " << b << std::endl; 
    }
      

     if (progress)  std::cout << "Finding rings" << std::endl; 
    
     TH3 * coarse_acc= new TH3I("chough","chough",_nbins_first_pass[0],_mins_first_pass[0],_maxs_first_pass[0], 
                                                  _nbins_first_pass[1],_mins_first_pass[1],_maxs_first_pass[1],
                                                  _nbins_first_pass[2],_mins_first_pass[2],_maxs_first_pass[2]);

     if (progress) std::cout << std::endl; 
     dmtpc::image::transform::houghTransformCircle(edge, (TH3*)coarse_acc,1,progress ? &progress_fn : 0); 
     if (progress) std::cout << std::endl; 

     int bx,by,bz;
     coarse_acc->GetMaximumBin(bx,by,bz); 

     double xwidth = coarse_acc->GetXaxis()->GetBinWidth(bx);
     double ywidth = coarse_acc->GetYaxis()->GetBinWidth(by);
     double mins[3]; 
     double maxs[3]; 
     mins[0] = coarse_acc->GetXaxis()->GetBinCenter(bx) - _nwidths_second_pass*xwidth;
     mins[1] = coarse_acc->GetYaxis()->GetBinCenter(by) - _nwidths_second_pass*ywidth;
     maxs[0] = mins[0] + 2*_nwidths_second_pass* xwidth;
     maxs[1] = mins[1] + 2*_nwidths_second_pass* ywidth; 
     mins[2] = _mins_first_pass[2]; 
     maxs[2] = _maxs_first_pass[2]; 
     delete coarse_acc; 
     TH3 * fine_acc= new TH3I("chough","chough",_nbins_second_pass[0],mins[0],maxs[0], 
                                                  _nbins_second_pass[1],mins[1],maxs[1],
                                                  _nbins_second_pass[2],mins[2],maxs[2]);
     if (progress) std::cout << std::endl; 
     dmtpc::image::transform::houghTransformCircle(edge, (TH3*)fine_acc,1,progress ? &progress_fn : 0 ); 
     if (progress) std::cout << std::endl; 

     int allmaxbin = fine_acc->GetMaximumBin(); 
     fine_acc->GetBinXYZ(allmaxbin,bx,by,bz); 
     double centerx = fine_acc->GetXaxis()->GetBinCenter(bx); 
     double centery = fine_acc->GetYaxis()->GetBinCenter(by); 

     TH2 * img = orig[i]; 
     TPoints cpoint(centerx,centery); 
     TH2D * polar = (TH2D*) dmtpc::image::transform::toPolarCoordinates(img, 4096, 4096, &cpoint); 
//     polar->SetDirectory(0); 
     polars.push_back(polar); 
     _xCenters.push_back(centerx); 
     _yCenters.push_back(centery); 

     int centerxbin = fine_acc->GetXaxis()->FindBin(centerx); 
     int centerybin = fine_acc->GetYaxis()->FindBin(centery); 

     char rprojname[8]; 
     sprintf(rprojname,"rproj%d",i); 

     TH1I * rproj = new TH1I(rprojname,rprojname,_nbins_second_pass[2],mins[2],maxs[2]); 
//     rproj->SetDirectory(0); 
     rprojs.push_back(rproj); 


     for (int j = 1; j <= _nbins_second_pass[2]; j++)
     {
       rproj->SetBinContent(j, fine_acc->GetBinContent(centerxbin, centerybin, j));
     }
     
     double outer_r = 0; 
     double inner_r = 0; 

     calcRadii(rproj, inner_r, outer_r, true); 

     _outerRadius.push_back(outer_r); 
     _innerRadius.push_back(inner_r); 

     if (progress)
     {
        std::cout << "Radii: " << inner_r << " " << outer_r << std::endl; 

     }
     delete fine_acc; 

   }

  _nImages = (int) images->size(); 
  for (unsigned i = 0; i < _nImages; i++)
  {
    _weights.push_back(1.); 
  }

  for (unsigned i = 0; i < _nImages; i++)
  {
    switch (scaleMethod)
    {
       case INNER: 
         _scales.push_back(innerRadius(i)); 
         break; 
       case OUTER: 
         _scales.push_back(outerRadius(i)); 
         break; 
       case BOTH: 
         _scales.push_back(outerRadius(i) + innerRadius(i)); 
         break; 
       case FIXED: 
       default:
         _scales.push_back(1.); 
         break; 
    }
  }

  double maxscale = *std::max_element(_scales.begin(), _scales.end());  

  for (unsigned i = 0; i < _nImages; i++)
  {
    _scales[i] = maxscale / _scales[i]; 
    if (progress) std::cout << "scale for " << i << " is " << _scales[i] << std::endl; 
  }


  //Get baseline theta, this lines up the spacers to be horizontal
 

  std::vector<double> baseline_rotations; 
  for (unsigned i =0; i < _nImages; i++)
  {
     double sin_sum = 0;
     double cos_sum = 0;
     for (unsigned s = 0; s < getNSpacers(i); s++)
     {
       sin_sum += sin(getSpacerTheta(i,s)); 
       cos_sum += cos(getSpacerTheta(i,s)); 
     }

     double theta = TMath::Pi()/2. - atan2(sin_sum,cos_sum); 
     double adjust = 0; 
     if (rotation_guess[i] == "HALF_TURN" || rotation_guess[i] == "QUARTER_RIGHT") adjust = TMath::Pi(); 
     baseline_rotations.push_back(adjust - theta); 
     if (progress) std::cout << "baseline rotation for " << i << " is " << baseline_rotations[i] << std::endl; 
  }



 
 // Get pairs for shared spacer configurations (spacers within 30 px of each other)

  /*
 std::vector<std::vector<std::vector<double> > > pairs(_nImages, std::vector<std::vector<double> >(_nImages)); 
 
 for (unsigned i = 0; i < _nImages; i++)
 {
   for (unsigned j = i+1; j < _nImages; j++)
   {
     std::cout << "i,j: " << i << "," << j << std::endl; 
     for (unsigned si = 0; si < getNSpacers(i); si++)
     {
       double globalb_i = _scales[i] * ( getSpacerIntercept(i,si) - yCenter(i) + getSpacerSlope(i,si) * xCenter(i)) / (TMath::Cos(baseline_rotations[i]) + getSpacerSlope(i,si) * TMath::Sin(baseline_rotations[i])); 
       for (unsigned sj = 0; sj < getNSpacers(j); sj++)
       {
         
          double globalb_j = _scales[j] * ( getSpacerIntercept(j,sj) - yCenter(j) + getSpacerSlope(j,sj) * xCenter(j)) / (TMath::Cos(baseline_rotations[j]) + getSpacerSlope(j,sj) * TMath::Sin(baseline_rotations[j])); 

    //      std::cout << fabs(globalb_j - globalb_i) << std::endl; 
          if (fabs(globalb_j - globalb_i) < 30)
          {
              double a1 = baseline_rotations[i] + TMath::Pi()/2. -  getSpacerTheta(i,si); 
              double a2 = baseline_rotations[j] + TMath::Pi()/2. - getSpacerTheta(j,sj); 
              double diff  =  atan((tan(a2) - tan(a1)) / (1 + tan(a1) * tan(a2))); 
              std::cout << diff << std::endl; 
              pairs[i][j].push_back(diff); 
              pairs[j][i].push_back(diff); 

          }
       }
     }
   }
 }
 

 */

 for (unsigned i = 0; i < _nImages; i++)
 {

    double theta =  baseline_rotations[i]; 

    double adjust = 0; 

    /*
    if (i > 0)
    {
      double sum = 0; 
      double denom = 0; 
      
      for (unsigned s = 0; s < pairs[i][i-1].size(); s++)
      {
        sum += pairs[i][i-1][s]; 
        denom += 1; 
      }
      //adjust += sum/denom; 
      std::cout << "adjust:" << adjust << std::endl; 
    }
    */

    _rotations.push_back(theta - adjust); 
    if (progress) std::cout << "rotation for " << i << " is " << _rotations[i] << std::endl; 
 }

  updateStitchInfo(); 

 _isInit = true; 
 return true; 

}

void dmtpc::analysis::Stitcher::updateStitchInfo()
{
 if (_isInit)
 {
    delete _stitch_info_256; 
    delete _stitch_info_512; 
    delete _stitch_info_1024; 
 }

 std::vector<double> xmins(4,0); 
 std::vector<double> ymins(4,0); 
 std::vector<double> xmaxs(4,1024); 
 std::vector<double> ymaxs(4,1024); 
 std::vector<int> nbinsx_1024(4,1024); 
 std::vector<int> nbinsy_1024(4,1024); 
 std::vector<int> nbinsx_256(4,256); 
 std::vector<int> nbinsy_256(4,256); 
 std::vector<int> nbinsx_512(4,512); 
 std::vector<int> nbinsy_512(4,512); 

 _stitch_info_256 = new dmtpc::image::StitchInfo("4sh_256",_nImages, _lens, &xmins[0],&ymins[0],&xmaxs[0],&ymaxs[0],&nbinsx_256[0],&nbinsy_256[0],&_xCenters[0], &_yCenters[0], 
                                          &_rotations[0],  &_scales[0], &_weights[0], 4); 

 _stitch_info_512 = new dmtpc::image::StitchInfo("4sh_512",_nImages, _lens, &xmins[0],&ymins[0],&xmaxs[0],&ymaxs[0],&nbinsx_512[0],&nbinsy_512[0],&_xCenters[0], &_yCenters[0], 
                                          &_rotations[0],  &_scales[0], &_weights[0], 2); 

 _stitch_info_1024 = new dmtpc::image::StitchInfo("4sh_1024",_nImages,_lens, &xmins[0],&ymins[0],&xmaxs[0],&ymaxs[0],&nbinsx_1024[0],&nbinsy_1024[0],&_xCenters[0], &_yCenters[0], 
                                         &_rotations[0], &_scales[0], &_weights[0], 1); 

}


void dmtpc::analysis::Stitcher::setScales(const double * scale, bool update_stitch )
{
    for (unsigned i = 0; i < _nImages; i++)
    {
      _innerRadius[i] *= scale[i]/_scales[i]; 
      _outerRadius[i] *= scale[i]/_scales[i]; 
      _scales[i] = scale[i] ;
    }

    if (update_stitch) 
    {
      updateStitchInfo(); 
    }
}

void dmtpc::analysis::Stitcher::setRotations(const double * rotation, bool update_stitch)
{
    for (unsigned i = 0; i < _nImages; i++)
    {
      _rotations[i] = rotation[i] ; 
    }

    if (update_stitch) 
    {
      updateStitchInfo(); 
    }
}

void dmtpc::analysis::Stitcher::calcRadii(TH1 * rproj, double & inner_r, double & outer_r, bool progress) 
{
     TSpectrum spectr(_nspectr_peaks); 
    // spectr.Search(rproj,2,"goff"); 
     spectr.Search(rproj,2,"goff"); 
     float * radii = spectr.GetPositionX(); 
     int * ri = new int[spectr.GetNPeaks()]; 
     TMath::Sort(spectr.GetNPeaks(),radii, ri); 

     if (progress) std::cout << "determining radii" << std::endl; 

     //Separate the two radii by looking for the maximum gap in the sorted list 
     double max_gap = 0; 
     int max_gap_i = 0; 
     for (int g = 1; g < spectr.GetNPeaks(); g++)
     {
        if (radii[ri[g-1]] - radii[ri[g]] > max_gap)
        {
          max_gap = radii[ri[g-1]] - radii[ri[g]];
          max_gap_i = g; 
        }
     }


     int outer_r_n =0;
     for (int j = 0; j < max_gap_i; j++)
     {
       if (j < max_gap_i - 2) continue;  //Only take the two most inner rings of the outer, or one if there is only one. 
       if (outer_r_n > 0 && outer_r - radii[ri[j]] > 2 * _edge_blur_level)  //unless they're too far apart 
       {
        outer_r = radii[ri[j]]; 
        outer_r_n = 1; 
       }
       else
       {
       outer_r += radii[ri[j]]; 
       outer_r_n++; 
       }
     }

     outer_r /= outer_r_n; 

     for (int j = max_gap_i; j < spectr.GetNPeaks(); j++)
     {
       inner_r += radii[ri[j]]; 
     }
     inner_r /= (spectr.GetNPeaks() - max_gap_i); 

     delete ri; 

}

void dmtpc::analysis::Stitcher::setCenters(const double * xcenter, const double * ycenter, bool update_stitch)
{
    for (unsigned i = 0; i < _nImages; i++)
    {
       
      
      bool changed = _xCenters[i] != xcenter[i] || _yCenters[i] != ycenter[i]; 
      

      if (!changed) continue; 

      double delta_r[2]; 
      delta_r[0] = xcenter[i] - _xCenters[i]; 
      delta_r[1] = ycenter[i] - _yCenters[i]; 

      _xCenters[i] = xcenter[i] ;
      _yCenters[i] = ycenter[i] ;


      // need to recalculate radii assuming new centers 
     
      delete polars[i]; 
      delete rprojs[i]; 

      TPoints center(_xCenters[i], _yCenters[i]); 
      polars[i] = (TH2D*) dmtpc::image::transform::toPolarCoordinates(orig[i], 4096, 4096,&center); 
      TH2* polar_edge = dmtpc::image::transform::toPolarCoordinates(edges[i], 4096, 4096,&center); 

      char rprojname[8]; 
      sprintf(rprojname,"rproj%d",i); 
      rprojs[i] = polar_edge->ProjectionX("px", polar_edge->GetXaxis()->FindBin(_mins_first_pass[2]), polar_edge->GetXaxis()->FindBin(_maxs_first_pass[2])); 
      rprojs[i]->SetName(rprojname); 
      rprojs[i]->SetTitle(rprojname); 


      delete polar_edge; 

      double outer_r = 0; 
      double inner_r = 0; 

      calcRadii(rprojs[i], inner_r, outer_r, true); 

      _outerRadius[i] = outer_r; 
      _innerRadius[i] = inner_r; 


    }

    if (update_stitch) 
    {
      updateStitchInfo(); 
    }
}


void dmtpc::analysis::Stitcher::setWeights(const double * weights)
{

  if (!_isInit) 
  {
    std::cerr << "Not init yet. Doing nothing. " << std::endl; 
  }

  _weights.clear(); 
  for (unsigned i = 0; i < _nImages; i++)
  {
    _weights.push_back(weights[i]); 
  }
}

void dmtpc::analysis::Stitcher::writeOverlayFile(const char * file, int img, int ncirclesegments)
{
  if (!_isInit)
  {
    std::cerr << "Not trained yet, can't write overlay. " <<std::endl; 
    return; 
  }

  FILE * f = fopen(file,"w"); 

  //Code stolen from DmtpcGainMap
  for (unsigned i = 0; i < getNSpacers(img); i++)
  {
    fprintf(f,"#Spacer %d\n",i); 
    double b = getSpacerIntercept(img,i); 
    double m = getSpacerSlope(img,i); 
    double x0 = -b/m; 
    double y0 = 0; 

    if (x0 < 0)
    {
      x0 = 0; 
      y0 = b; 
    }

    if (x0 > 1024)
    {
      x0 = 1024; 
      y0 = m*1024+b; 
    }

    double x1 = (1024-b)/m; 
    double y1 = 1024; 

    if (x1 < 0)
    {
      x1 = 0; 
      y1 = b; 
    }

    if (x1 > 1024)
    {
      x1 = 1024;
      y1 = m*1024+b; 
    }

    fprintf(f, "%f %f %f %f \n\n", x0, y0, x1, y1); 
  }

  //Draw the rings... 
  //We only have to grab the coordinates for the angles -pi/2 to 0 

  fprintf(f,"\n#Inner Ring\n"); 
  for (int i = 0; i < ncirclesegments; i++)
  {
    double ang = -i * TMath::Pi()/(2. * (ncirclesegments-1)); 
    double x = xCenter(img) + innerRadius(img)*TMath::Cos(ang); 
    double y = yCenter(img) + innerRadius(img)*TMath::Sin(ang); 
    fprintf(f, "%f %f ",x,y); 
  }

  fprintf(f,"\n\n#Outer Ring\n"); 
  for (int i = 0; i < ncirclesegments; i++)
  {
    double ang = -i * TMath::Pi()/(2. * (ncirclesegments-1)); 
    double x = xCenter(img) + outerRadius(img)*TMath::Cos(ang); 
    double y = yCenter(img) + outerRadius(img)*TMath::Sin(ang); 
    fprintf(f, "%f %f ",x,y); 
  }


  fclose(f); 

}

bool dmtpc::analysis::Stitcher::setupMCViper(int n, char ** serial_numbers, double * xcenters, 
                                         double * ycenters, double * scales,
                                         double * rotations, double  * innerRadius, double  * outerRadius)
{

  _nImages = n; 
  _lens = 0; 
 

  for (int i = 0; i < n; i++) 
  {
    std::cout << serial_numbers[i] << std::endl; 

    _xCenters.push_back(xcenters[i]);
    _yCenters.push_back(ycenters[i]);
    std::cout << xcenters[i] << " , "<<ycenters[i] << std::endl; 

    _scales.push_back(scales ?  scales[i] :  1);
    _rotations.push_back(rotations ?  rotations[i] : 0);
    _innerRadius.push_back(innerRadius ? innerRadius[i] : 0);
    _outerRadius.push_back(outerRadius ?  outerRadius[i] :  0);
    _serials.push_back (TObjString(serial_numbers[i])); 
    _weights.push_back(1.); 

    std::vector<double> mt; 
    _spacers_r.push_back(mt); 
    _spacers_theta.push_back(mt); 
  }

  std::cout << "about to update stitch info" << std::endl; 
  updateStitchInfo(); 

  _isInit = true; 
  return true; 
}
