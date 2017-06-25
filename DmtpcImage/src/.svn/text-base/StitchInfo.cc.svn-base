#include "StitchInfo.hh"
#include "ImageCalculations.hh"
#include "Interpolation.hh"
#include <cmath>

ClassImp(dmtpc::image::StitchInfo);


dmtpc::image::StitchInfo::~StitchInfo()
{
  for (unsigned i =0; i < frac.size(); i++)
  {
      frac[i]->Delete();  
  }

  if (weight_sum)
  {
    weight_sum->Delete(); 
  }
}
 



dmtpc::image::StitchInfo::StitchInfo (const char * name, unsigned nimages, const dmtpc::image::LensCorrection * lens, const double * xmins, const double * ymins, const double * xmaxs, const double * ymaxs,
                     const int * nbinsx,  const int * nbinsy,  const  double * xorigin, const double * yorigin, const double * rotation, 
                     const double * scale,  const double * weight, unsigned binning) 
: corners(nimages,std::vector<std::vector<double> >(2, std::vector<double>(4))), 
  xmins(nimages), xmaxs(nimages), ymins(nimages), ymaxs(nimages), nbinsx(nimages), nbinsy(nimages), 
  xorigin(nimages), yorigin(nimages), rotation(nimages), sintheta(nimages), costheta(nimages), 
  scale(nimages), weight(nimages), xwidths(nimages), ywidths(nimages), frac(nimages)
{

    weight_sum = 0; 
    this->nimages = nimages; 
    fName = strdup(name); 

    std::vector<TH2*> distimages(nimages); 

    for (unsigned int i = 0; i < nimages; i++)
    {

      TH2F temp (TString::Format("tmp%d",i), TString::Format("tmp%d",i), nbinsx[i], xmins[i], xmaxs[i], nbinsy[i], ymins[i], ymaxs[i]); 

      for (int x = 1; x <= temp.GetNbinsX(); x++)
      {
        for (int y = 1; y <= temp.GetNbinsY(); y++)
        {
          temp.SetBinContent(x,y,1); 
        }
      }
    
      distimages[i] =  lens ? lens->correctDistortion(&temp) : (TH2*) temp.Clone("dist"); 

      this->xmins[i] = distimages[i]->GetXaxis()->GetXmin() ;
      this->xmaxs[i] = distimages[i]->GetXaxis()->GetXmax() ;
      this->ymins[i] = distimages[i]->GetYaxis()->GetXmin() ;
      this->ymaxs[i] = distimages[i]->GetYaxis()->GetXmin() ;
      this->nbinsx[i] = distimages[i]->GetNbinsX(); 
      this->nbinsy[i] = distimages[i]->GetNbinsY(); 
      this->xorigin[i] = xorigin[i]; 
      this->yorigin[i] = yorigin[i]; 
      this->rotation[i] = rotation[i]; 
      this->costheta[i] = cos(rotation[i]); 
      this->sintheta[i] = sin(rotation[i]); 
      this->scale[i] = scale[i]; 
      this->weight[i] = weight[i]; 
      xwidths[i] = (xmaxs[i]-xmins[i])/(nbinsx[i]); 
      ywidths[i] = (ymaxs[i]-ymins[i])/(nbinsy[i]); 
    }

    stitched_xmin = DBL_MAX; 
    stitched_ymin = DBL_MAX; 
    stitched_xmax = -DBL_MAX; 
    stitched_ymax = -DBL_MAX; 

    double xtry, ytry; 
    for (unsigned i = 0; i < nimages; i++)
    {
       //check the corners of the image 
       double theta = rotation[i];
       double c = scale[i]; 
       for (unsigned corner = 0; corner < 4; corner++)
       {
         double x,y; 
         switch (corner)
         {
           case 3: 
             x = xmins[i]; 
             y = ymins[i];
             break;
           case 2: 
             x = xmaxs[i]; 
             y = ymins[i]; 
             break;
           case 1: 
             x = xmaxs[i];
             y = ymaxs[i]; 
             break;
           case 0: 
             x = xmins[i]; 
             y = ymaxs[i]; 
             break;
         }

         x-= xorigin[i]; 
         y-= yorigin[i]; 
         xtry = c*(x * cos(theta) + y*sin(theta));
         ytry = c*(-x * sin(theta) + y*cos(theta)); 


         corners[i][0][corner] = xtry; 
         corners[i][1][corner] = ytry; 

         if (xtry < stitched_xmin) stitched_xmin = xtry; 
         if (ytry < stitched_ymin) stitched_ymin = ytry; 
         if (xtry > stitched_xmax) stitched_xmax = xtry; 
         if (ytry > stitched_ymax) stitched_ymax = ytry; 
       }

    }


  if (!binning) binning = (unsigned) xwidths[0]; 

  //the image with largest bins and we upsample everything else. 
  stitched_nbinsx = (unsigned) ((stitched_xmax - stitched_xmin)/binning +0.5); 
  stitched_nbinsy = (unsigned) ((stitched_ymax - stitched_ymin)/binning +0.5); 



  char hist_name[200];  

  for (unsigned i = 0; i < nimages; i++)
  {
    sprintf(hist_name,"frac%s_%d",name,i); 
    frac[i] = new TH2F(hist_name,hist_name,stitched_nbinsx,stitched_xmin,stitched_xmax,stitched_nbinsy,stitched_ymin,stitched_ymax); 
    frac[i]->SetDirectory(0); 
  }



  sprintf(hist_name,"weight_sum_%s",name); 
  weight_sum = new TH2F(hist_name, hist_name, stitched_nbinsx, stitched_xmin, stitched_xmax, stitched_nbinsy, stitched_ymin, stitched_ymax); 
  weight_sum->SetDirectory(0); 

  for (unsigned xi = 1; xi <= stitched_nbinsx; xi++)
  {
    double x = weight_sum->GetXaxis()->GetBinCenter(xi); 
    for (unsigned yi = 1; yi <= stitched_nbinsy; yi++) 
    {
      double y = weight_sum->GetYaxis()->GetBinCenter(yi); 
      double binsx[4]; 
      double binsy[4]; 


      binsx[0] = x - binning/2. ;
      binsx[1] = x + binning/2. ;
      binsx[2] = x + binning/2. ;
      binsx[3] = x - binning/2. ;
      binsy[0] = y - binning/2. ;
      binsy[1] = y - binning/2. ;
      binsy[2] = y + binning/2. ;
      binsy[3] = y + binning/2. ;

      for (unsigned img = 0; img < nimages; img++)
      {
         double theta = rotation[img]; 
         double xtemp=  1./(scale[img]) * x;
         double ytemp=  1./(scale[img]) * y; 
         double ximg = xtemp * cos(theta) - ytemp*sin(theta) + xorigin[img]; 
         double yimg = xtemp * sin(theta) + ytemp*cos(theta) + yorigin[img]; 


         double fraction = dmtpc::image::calculations::rectIntersectionArea(&corners[img][0][0], &corners[img][1][0], binsx, binsy) / (binning * binning); 

         if (ximg < xmins[img] + 0.5 * xwidths[img]  || 
             ximg > xmaxs[img] - 0.5 * xwidths[img]  || 
             yimg < ymins[img] + 0.5 * ywidths[img]  || 
             yimg > ymaxs[img] - 0.5 * ywidths[img]) 
         {
           fraction = 0; 
         }
         else
         {
           double lens_factor = dmtpc::image::interpolation::interpolate(distimages[img], ximg, yimg); 
           fraction *= lens_factor; 
         }

         frac[img]->SetBinContent(xi,yi,fraction); 
         weight_sum->SetBinContent(xi,yi,weight_sum->GetBinContent(xi,yi) + fraction * weight[img]);
      }
    }
  }

  for (unsigned i = 0; i < nimages; i++)
  {
    delete distimages[i]; 
  }
  distimages.clear(); 
}
