#include "ImageTransforms.hh"
#include "Interpolation.hh"
#include "ImageTools.hh"
#include "StitchInfo.hh" 
#include "TPoints.h"
#include <iostream>
#include "TMath.h" 
#include "TH3.h" 
#include "THnSparse.h" 
#include "TClass.h"

/** Functions which calculate a transformed version of an existing image go here.
 *  Sometimes the distinction between a transform and a filter isn't so obvious... use your discretion. 
 * */


TH2 * dmtpc::image::transform::houghTransformLine(const TH2* in, TH2* out, double min, unsigned nangbins, unsigned nrbins)
{

  if (out == 0)
  {
    double rmax = TMath::Sqrt(TMath::Power(in->GetXaxis()->GetXmax() - in->GetXaxis()->GetXmin(),2) + 
                                          TMath::Power(in->GetYaxis()->GetXmax() - in->GetYaxis()->GetXmin(),2)); 
    TString name = TString(in->GetName()) + TString("_hough"); 
    if (!strcmp(in->IsA()->GetName(),"TH2F") || !strcmp(in->IsA()->GetName(),"TH2D"))
    {
      out = new TH2D(name, name, nangbins, 0,  TMath::Pi(), nrbins, -rmax,rmax );

    }
    else
    {
      out = new TH2I(name, name, nangbins, 0, TMath::Pi(), nrbins, -rmax,rmax); 

    }

    out->GetXaxis()->SetTitle("#theta"); 
    out->GetYaxis()->SetTitle("r"); 
  }

  //We want to transform the input so that the edge is 0,0 
  double yoff = in->GetYaxis()->GetXmin(); 
  double xoff = in->GetXaxis()->GetXmin(); 
  
  for (int i = 1; i <= in->GetNbinsX(); i++)
  {
    double x = in->GetXaxis()->GetBinCenter(i) - xoff; 
    for (int j = 1; j <= in->GetNbinsY(); j++)
    {
      double c = in->GetBinContent(i,j); 
      if (c < min) continue; 
      double y = in->GetYaxis()->GetBinCenter(j) - yoff;; 
      for (int ii = 1; ii <= out->GetNbinsX(); ii++)
      {
        double theta = out->GetXaxis()->GetBinCenter(ii);  
        double r = x * TMath::Cos(theta) + y*TMath::Sin(theta); 
        out->Fill(theta,r,c); 
      }
    }
  }
  
  return out; 
}

TH3 * dmtpc::image::transform::houghTransformCircle(TH2*in, TH3 *out, double min, void (*progressFn)(int)) 
{
  if(!out) return 0;         

  for (int i = 1; i <= in->GetNbinsX(); i++)
  {
    if (progressFn) progressFn(i); 
    double x = in->GetXaxis()->GetBinCenter(i); 
    for (int j = 1; j <= in->GetNbinsY(); j++)
    {
      double c = in->GetBinContent(i,j);           
      if (c < min) continue; 
      double y = in->GetYaxis()->GetBinCenter(j); 

      //Loop over radii
      for (int x0i = 1; x0i <= out->GetXaxis()->GetNbins(); x0i++)
      {
        double x0 = out->GetXaxis()->GetBinCenter(x0i); 
        for (int y0i = 1; y0i <= out->GetYaxis()->GetNbins(); y0i++)
        {
          double y0 = out->GetYaxis()->GetBinCenter(y0i); 
          double r = TMath::Sqrt( (x-x0)*(x-x0) + (y-y0)*(y-y0));  

          if (r < out->GetZaxis()->GetXmin() || r > out->GetZaxis()->GetXmax()) continue; 

          out->Fill(x0,y0,r, c);  
        }
      }
    }
  }
      
  return out; 

}

THnSparse * dmtpc::image::transform::houghTransformEllipse(TH2* in, THnSparse *out, double min)
{
  if (!out) 
  {
    return 0; 
  }

  for (int i = 1; i <= in->GetNbinsX(); i++)
  {
    double x = in->GetXaxis()->GetBinCenter(i); 
    for (int j = 1; j <= in->GetNbinsY(); j++)
    {
      double c = in->GetBinContent(i,j);           
      if (c < min) continue; 
      double y = in->GetYaxis()->GetBinCenter(j); 
      
      //Loop over centers
      for (int x0i = 1; x0i <= out->GetAxis(ELL_X0)->GetNbins(); x0i++)
      {
        double x0 = out->GetAxis(ELL_X0)->GetBinCenter(x0i); 

        for (int y0i = 1; y0i <= out->GetAxis(ELL_Y0)->GetNbins(); y0i++)
        {
          double y0 = out->GetAxis(ELL_Y0)->GetBinCenter(y0i); 

          for (int phii = 1; phii <= out->GetAxis(ELL_PHI)->GetNbins(); phii++)
          {
            double phi = out->GetAxis(ELL_PHI)->GetBinCenter(phii); 

            for (int ai = 1; ai <=out->GetAxis(ELL_A)->GetNbins(); ai++)
            {
              double a = out->GetAxis(ELL_A)->GetBinCenter(ai); 
              double b = TMath::Sqrt((1 - TMath::Power(((x-x0)*TMath::Cos(phi) - (y - y0)*TMath::Sin(phi))/a,2)) / ( (x-x0)*TMath::Cos(phi) + (y-y0)*TMath::Sin(phi)));

              if (b > out->GetAxis(ELL_B)->GetXmax() || b < out->GetAxis(ELL_B)->GetXmin()) continue; 
              double params[5]; 

              params[ELL_X0] = x0; 
              params[ELL_Y0] = y0; 
              params[ELL_PHI] = phi; 
              params[ELL_A] = a; 
              params[ELL_B] = b; 
              out->Fill(params,in->GetBinContent(i,j)); 
            }
          }
        }
      }
    }
  }
  return out; 
}

TH2 * dmtpc::image::transform::toPolarCoordinates(const TH2 * in, unsigned nrbins, unsigned nthetabins, TPoints * center, char type, const char * interpolation, const char * name)
{

  TH2 * out; 
  
  TString newname = name ? TString(name) : TString(in->GetName()) + TString("_polar"); 

  double rmin = 0; 

  double xmin = in->GetXaxis()->GetXmin(); 
  double xmax = in->GetXaxis()->GetXmax(); 
  double ymin = in->GetYaxis()->GetXmin(); 
  double ymax = in->GetYaxis()->GetXmax(); 

  double theta_min = -TMath::Pi(); 
  double theta_max = TMath::Pi(); 
 

  double centerx, centery;
  if (!center)
  {
    centerx = (in->GetXaxis()->GetXmax() + in->GetXaxis()->GetXmin())/2; 
    centery = (in->GetYaxis()->GetXmax() + in->GetYaxis()->GetXmin())/2; 
  }
  else
  {
    centerx = center->GetX(); 
    centery = center->GetY(); 
  }

  if (centerx < xmin || centerx > xmax || centery < ymin || centery > ymax)
  {

    theta_min = TMath::Pi(); 
    theta_max = -TMath::Pi(); 

    double theta = atan2(ymin-centery,xmin-centerx); 
    if (theta < theta_min) theta_min = theta; 
    if (theta > theta_max) theta_max = theta; 
    theta = atan2(ymin-centery,xmax-centerx); 
    if (theta < theta_min) theta_min = theta; 
    if (theta > theta_max) theta_max = theta; 
    theta = atan2(ymax-centery,xmax-centerx); 
    if (theta < theta_min) theta_min = theta; 
    if (theta > theta_max) theta_max = theta; 
    theta = atan2(ymax-centery,xmin-centerx); 
    if (theta < theta_min) theta_min = theta; 
    if (theta > theta_max) theta_max = theta; 


    if (centery <= ymax && centery >= ymin)
    {
      rmin =  centerx < xmin ? xmin - centerx : centerx - xmax; 
    }
    else if  (centerx <= xmax && centerx >= xmin)
    {
      rmin =  centery < ymin ? ymin - centery : centery - ymax; 
    }
    else
    {
      double x0, y0; 

      if ( centerx < xmin && centery < ymin)
      {
        x0 = xmin; 
        y0 = ymin; 
      }
      else if ( centerx < xmin && centery > ymax)
      {
        x0 = xmin; 
        y0 = ymax; 
      }
      else if ( centerx > xmax && centery > ymax)
      {
        x0 = xmax; 
        y0 = ymax; 
      }
      else if ( centerx > xmax && centery < ymin)
      {
        x0 = xmax; 
        y0 = ymin; 
      }
      else
      {
        std::cerr << "FORGOTTEN CASE... UH OH" << std::endl; 
      }

      rmin = sqrt(pow(centerx - x0,2) + pow(centery - y0,2)); 
    }
  }

  double rmax = -1; 

  double r = sqrt(pow(xmin - centerx,2) + pow(ymin - centery,2)) ; 
  if (r > rmax) rmax = r; 
  r = sqrt(pow(xmax - centerx,2) + pow(ymin - centery,2)) ; 
  if (r > rmax) rmax = r; 
  r = sqrt(pow(xmax - centerx,2) + pow(ymax - centery,2)) ; 
  if (r > rmax) rmax = r; 
  r = sqrt(pow(xmin - centerx,2) + pow(ymax - centery,2)) ; 
  if (r > rmax) rmax = r; 

  switch (type)
  {
    
    case 'C':
      out = new TH2C(newname,newname,nrbins,rmin,rmax,nthetabins,theta_min,theta_max); 
      break; 
    case 'S':
      out = new TH2S(newname,newname,nrbins,rmin,rmax,nthetabins,theta_min,theta_max); 
      break; 
    case 'I':
      out = new TH2I(newname,newname,nrbins,rmin,rmax,nthetabins,theta_min,theta_max); 
      break; 
    case 'F':
      out = new TH2F(newname,newname,nrbins,rmin,rmax,nthetabins,theta_min,theta_max); 
      break; 
    default:
      std::cerr << "Bad type, using TH2D" << std::endl; 
    case 'D':
      out = new TH2D(newname,newname,nrbins,rmin,rmax,nthetabins,theta_min,theta_max); 
      break; 
  }
 
  
  double xminwidth = in->GetXaxis()->GetBinWidth(1);  
  double xmaxwidth = in->GetXaxis()->GetBinWidth(in->GetNbinsX());  
  double yminwidth = in->GetYaxis()->GetBinWidth(1);  
  double ymaxwidth = in->GetYaxis()->GetBinWidth(in->GetNbinsY());  

  for (unsigned i = 1; i <= nrbins; i++)
  {
    double thisr = out->GetXaxis()->GetBinCenter(i); 
    for (unsigned j = 1; j <= nthetabins; j++)
    {
      double thisth = out->GetYaxis()->GetBinCenter(j); 

      double thisx = thisr * cos(thisth) + centerx; 
      double thisy = thisr * sin(thisth) + centery; 

      if (thisx >= xmin + 0.5 * xminwidth && thisx <= xmax - 0.5 * xmaxwidth &&
          thisy >= ymin + 0.5 * yminwidth && thisy <= ymax - 0.5 * ymaxwidth ) 
      {
          out->SetBinContent(i,j, dmtpc::image::interpolation::interpolate(in,thisx,thisy,interpolation)); 
      }
    }
  }
  return out; 
}

TH2* dmtpc::image::transform::montage(const TH2 * const* in, StitchInfo * info, const char * name, 
               const char * interpolation , char histType) 
{

  if (!dmtpc::image::interpolation::testInterpolate(interpolation))
  {
    std::cerr << "Error, unsupported interpolation method" << std::endl; 
    return 0; 
  }

  if (!in)
  {
    std::cerr << "Error, in is null" << std::endl; 
    return 0; 
  }

  if (!info)
  {
    std::cerr << "Error, stitch info is null" << std::endl; 
    return 0; 
  }

  if (!info->nbinsx[0] == in[0]->GetNbinsX())
  {
    std::cerr << "Error, stitch info binning doesn't match" << std::endl; 
    return 0; 
  }


  
  TH2 * out; 

  switch (histType)
  {
    
    case 'C':
      out = new TH2C(name,name,info->stitched_nbinsx,info->stitched_xmin,info->stitched_xmax,info->stitched_nbinsy,info->stitched_ymin,info->stitched_ymax); 
      break; 
    case 'S':
      out = new TH2S(name,name,info->stitched_nbinsx,info->stitched_xmin,info->stitched_xmax,info->stitched_nbinsy,info->stitched_ymin,info->stitched_ymax); 
      break; 
    case 'I':
      out = new TH2I(name,name,info->stitched_nbinsx,info->stitched_xmin,info->stitched_xmax,info->stitched_nbinsy,info->stitched_ymin,info->stitched_ymax); 
      break; 
    case 'F':
      out = new TH2F(name,name,info->stitched_nbinsx,info->stitched_xmin,info->stitched_xmax,info->stitched_nbinsy,info->stitched_ymin,info->stitched_ymax); 
      break; 
    default:
      std::cerr << "Bad type, using TH2D" << std::endl; 
    case 'D':
      out = new TH2D(name,name,info->stitched_nbinsx,info->stitched_xmin,info->stitched_xmax,info->stitched_nbinsy,info->stitched_ymin,info->stitched_ymax); 
      break; 
  }
 

  //Loop over pixels in new image, figure out how to fill them! 
  for (unsigned xi = 1; xi <= info->stitched_nbinsx; xi++) 
  {
    for (unsigned yi = 1; yi <= info->stitched_nbinsy; yi++) 
    {
      double valsum = 0;
      double weightsum = info->weight_sum->GetBinContent(xi,yi); 
      if (weightsum == 0) continue; 

      for (unsigned img = 0; img < info->nimages; img++)
      {
          double weight = info->frac[img]->GetBinContent(xi,yi);
          if (weight > 0)
          {
            double x = out->GetXaxis()->GetBinCenter(xi); 
            double y = out->GetYaxis()->GetBinCenter(yi); 
            weight*= info->weight[img]; 
            double xtemp=  1./(info->scale[img]) * x;
            double ytemp=  1./(info->scale[img]) * y; 
            double ximg = xtemp * info->costheta[img] - ytemp*info->sintheta[img] + info->xorigin[img]; 
            double yimg = xtemp * info->sintheta[img] + ytemp*info->costheta[img] + info->yorigin[img]; 
            double val = dmtpc::image::interpolation::interpolate((TH2*)in[img],ximg,yimg,interpolation);
//            cout << val << endl; 
            valsum += val * weight; 
            //if (val == 0) weightsum -=  info->frac[img]->GetBinContent(xi,yi);
          }
      }
    
//      cout << xi << " " << yi << " " << valsum << " " << weightsum << endl; 
      out->SetBinContent(xi,yi, valsum / weightsum); 
     
    }
  }

 return out;
}
 
TH2* 
dmtpc::image::transform::rotateRight(const TH2* image) {
  return rotatePerfect(image, "QUARTER_RIGHT");
}
TH2* 
dmtpc::image::transform::rotateLeft(const TH2* image) {
  return rotatePerfect(image, "QUARTER_LEFT");
}
TH2* 
dmtpc::image::transform::rotate180(const TH2* image) {
  return rotatePerfect(image, "HALF_TURN");
}

TH2*
dmtpc::image::transform::rotatePerfect(const TH2 *hin, TString dir) {
  // dir = direction
  //       "QUARTER_LEFT"  Rotate counter clockwise 90 degrees
  //       "QUARTER_RIGHT" Rotate clockwise 90 degrees
  //       "HALF_TURN"     Rotate by 180 degrees
  //       "NONE"          Do nothing, just return input histogram
  // 
  //  ... not yet implemented...
  //       "FLIP_VERT"     Flip image so that the first row becomes the last
  //       "FLIP_HORIZ"    Flip image so that the first column becomes the last
  //
  //  This code does NOT manipulate the overflow/underflow bins!
  //
  //  It DOES assume that there are as many x bins as y bins 
  //  (that is the image is square)
  //

  dir.ToUpper();
  // if dir not in valid list of options, return...


  // copy the histogram
  
  TH2 *hout = (TH2*) hin->Clone(); 

  if (dir == "NONE") { return hout; }

  int nx, ny, nn;
  nx = hin->GetNbinsX();
  ny = hin->GetNbinsY();
  if (nx != ny) {
    std::cout << "error: input histogram must have same number of X and Y bins" << std::endl;
    std::cout << "nx, ny = " << nx << ", " << ny <<std::endl;
    return 0;
  }
  nn = nx;

  for (Int_t ix=1; ix<=nx; ix++) {
    for (Int_t iy=1; iy<=ny; iy++) {
      
      if (dir == "QUARTER_LEFT") {
        hout->SetBinContent(ix, iy, hin->GetBinContent(iy, nn-(ix-1)));
      } else if (dir == "QUARTER_RIGHT") {
        hout->SetBinContent(iy, nn-(ix-1), hin->GetBinContent(ix, iy));
      } else if (dir == "HALF_TURN") {
        hout->SetBinContent(nn-(ix-1), nn-(iy-1), hin->GetBinContent(ix, iy));
      } else if (dir == "FLIP_VERT") {
        std::cout << "not yet implemented" << std::endl;
      } else if (dir == "FLIP_HORIZ") {
        std::cout << "not yet implemented" << std::endl;
      }
    }
  }

  return hout;
}


TH2*
dmtpc::image::transform::rotateSubdivide(const TH2* img, double ang, TPoints* pt, Int_t ndiv) 
{

  // if the rotation angle is multiples of 90, use the "perfect" rotation
  // algorithm
  //if (ang / TMath::Pi()) 

  int nx=img->GetNbinsX();
  int ny=img->GetNbinsY();
  Float_t lowX  = img->GetXaxis()->GetBinLowEdge(1);
  Float_t highX = img->GetXaxis()->GetBinUpEdge(nx);
  Float_t lowY  = img->GetYaxis()->GetBinLowEdge(1);
  Float_t highY = img->GetYaxis()->GetBinUpEdge(ny);

  /////////////////////////////////////////////////////////////
  // Set default rotation point to be the image center
  float x0, y0;
  if (pt == 0) {
    x0 = 0.5*(highX-lowX);
    y0 = 0.5*(highY-lowY);
  } else {
    x0 = pt->GetX();
    y0 = pt->GetY();
  }
  /*
  std::cout << "rotateImg():  " << std::endl;
  std::cout << "  x0, y0 = " << x0 << ", " << y0 << std::endl;
  std::cout << "  lowX, highX = " << lowX << ", " << highX << std::endl;
  cstd::out << "  lowY, highY = " << lowY << ", " << highY << std::endl;
  */
  /////////////////////////////////////////////////////////////

  // Create the output image (blank for now)
  TH2* roti = dmtpc::image::tools::newTH2StealType(img,"roti", "title", nx, lowX, highX, ny, lowY, highY);

  int ndiv2 = ndiv*ndiv;
  for (int ix=1; ix<=nx; ix++) {
    float xwid = img->GetXaxis()->GetBinWidth(ix)/ndiv;
    float xup  = img->GetXaxis()->GetBinUpEdge(ix);
    
    for (int iy=1; iy<=ny; iy++) {
      float yield = img->GetBinContent(ix, iy)/ndiv2;
      float ywid  = img->GetYaxis()->GetBinWidth(iy)/ndiv;
      float yup   = img->GetYaxis()->GetBinUpEdge(iy);

      // split each bin into (ndiv X ndiv) smaller bins and rotate separately
      for (float xbin=img->GetXaxis()->GetBinLowEdge(ix); xbin<xup; xbin+=xwid) {
        for (float ybin=img->GetYaxis()->GetBinLowEdge(iy); ybin<yup; ybin+=ywid) {
          
          // angle between the x-axis and the unrotated point
          float angle1 = TMath::ATan2(ybin-y0, xbin-x0);
          // distance from reference point to unrotated point location
          float rr = TMath::Sqrt( (xbin-x0)*(xbin-x0) + (ybin-y0)*(ybin-y0) );
          // angle between the x-axis and the rotated point
          float newangle = angle1 + ang;
          float x2 = x0 + rr*TMath::Cos(newangle);
          float y2 = y0 + rr*TMath::Sin(newangle);
          roti->Fill( x2, y2, yield );
        }
      }
    }
  }
  
  return roti;     
  
}



/*
 *  dead? 
 *
TH2* 
dmtpc::image::transform::rotate(TH2* image, int nrot, TVector2 *xaxis, TVector2 *yaxis, float x0, float y0) {
    // Rotate the image using axes defined by a rectangular cut region (see setRectangularCut 
    // for defining the cut).

    TH2 *roti = dmtpc::image::tools::newTH2StealType(image,"roti","",
        100, 0, 768,
        100, 0, 512);

    int nrot2 = nrot*nrot;
    
    int nx=image->GetNbinsX();
    int ny=image->GetNbinsY();


  for (int i=1; i<=nx; i++) {
    float xwid = image->GetXaxis()->GetBinWidth(i)/nrot;
    float xup  = image->GetXaxis()->GetBinUpEdge(i);
    for (int j=1; j<=ny; j++) {

        float yield = image->GetBinContent(i,j)/nrot2;
        float ywid = image->GetYaxis()->GetBinWidth(j)/nrot;
        float yup  = image->GetYaxis()->GetBinUpEdge(j);
        
        // split each bin into nrot x nrot smaller bins and rotate separately...
        for (float xbin = image->GetXaxis()->GetBinLowEdge(i);  xbin<xup; xbin+=xwid) {
          for (float ybin = image->GetYaxis()->GetBinLowEdge(j); ybin<yup; ybin+=ywid) {  
          float dx = (xbin-x0) * xaxis->X() + (ybin-y0) * xaxis->Y();
          float dy = (xbin-x0) * yaxis->X() + (ybin-y0) * yaxis->Y();
          roti->Fill( dx, dy, yield);
          }
        }
    }
  }
    
  return roti;
}
*/


TH2*
dmtpc::image::transform::resizeImage(const TH2* image, double xmin, double xmax, double ymin, double ymax) {

        int nx=image->GetNbinsX();
        int ny=image->GetNbinsY();
        TString hname=image->GetName();
        hname += "_resize";
        TH2* resize = dmtpc::image::tools::newTH2StealType(image, hname,image->GetTitle(), nx,xmin,xmax, ny,ymin,ymax);
        return (TH2*)copyContent( image, resize);
}


TH1*
dmtpc::image::transform::copyContent(const TH1* in, TH1* out) {
        int nx=in->GetNbinsX();
        int ny=in->GetNbinsY();
        int nz=in->GetNbinsY();

        for (int i=1;i<=nx;i++) {
          for (int j=1; j<=ny; j++)
          {
            for (int k = 1; k <= nz; k++) 
            {
              out->SetBinContent(i,j,k, in->GetBinContent(i,j,k));
            }
          }
        }
        return out;
}

TH2* 
dmtpc::image::transform::padImage(const TH2* image, int padfactor) {

  ///////////////////////////////////////////////////////
  // Get image property info from the input image
  int nx = image->GetNbinsX();   int ny = image->GetNbinsY();
  float xmin = image->GetXaxis()->GetBinLowEdge(1);
  float xmax = image->GetXaxis()->GetBinLowEdge(nx)+image->GetXaxis()->GetBinWidth(nx);
  float dx = xmax-xmin;

  float ymin = image->GetYaxis()->GetBinLowEdge(1);
  float ymax = image->GetYaxis()->GetBinLowEdge(ny)+image->GetYaxis()->GetBinWidth(ny);
  float dy = ymax-ymin;
  ///////////////////////////////////////////////////////

  TString paddedname = image->GetName();
  paddedname += "_pad";
  
  int nbinsx = padfactor*nx;
  int nbinsy = padfactor*ny;
  float padminx=0; float padmaxx=0; float padminy=0; float padmaxy=0;
  int xtrans=0; int ytrans=0;
  if (padfactor == 3) {
    padminx = xmin-dx;
    padmaxx = xmax+dx;
    padminy = ymin-dy;
    padmaxy = ymax+dy;

    // Define the additive translation factor to go from bin number in 
    // the original image to bin number in the padded image
    xtrans = nx;
    ytrans = ny;
  }

  // Create the padded TH2F
  TH2 *padimg = dmtpc::image::tools::newTH2StealType(image,paddedname, paddedname, 
        nbinsx, padminx, padmaxx, 
        nbinsy, padminy, padmaxy);

  // Populate the padded TH2F
  for (int ix=1; ix<=nx; ix++) {
    for (int iy=1; iy<=ny; iy++) {
      padimg->SetBinContent(ix+xtrans, iy+ytrans, image->GetBinContent(ix, iy));
    }
  }
  
  return padimg;
}

TH2 * dmtpc::image::transform:: histApply(TH2* in, double (*fn) (double)) 
{
  for (int i = 1; i < in->GetNbinsX(); i++)
  {
    for (int j = 1; j < in->GetNbinsY(); j++)
    {
      in->SetBinContent(i,j,fn(in->GetBinContent(i,j))); 
    }
  }
  return in; 
}

TH2 * dmtpc::image::transform:: histApply(TH2* in, double (*fn) (double,double), double aux) 
{
  for (int i = 1; i < in->GetNbinsX(); i++)
  {
    for (int j = 1; j < in->GetNbinsY(); j++)
    {
      in->SetBinContent(i,j,fn(in->GetBinContent(i,j),aux)); 
    }
  }
  return in; 
}

TH2 * dmtpc::image::transform::histSqrt(TH2 * in)
{
  return histApply(in,sqrt);  
}


TH2 * dmtpc::image::transform::histAbs(TH2 * in)
{
  return histApply(in,fabs);  
}

TH2 * dmtpc::image::transform::histPow(TH2 * in, double b)
{
  return histApply(in,pow,b);  
}

TH2 * 
dmtpc::image::transform::rotateInterpolate(const TH2 * img, double ang, TPoints * pt, 
                                          TH2 * out, const char * interpolation)
{
  if (!dmtpc::image::interpolation::testInterpolate(interpolation)) return 0; 

  double binning = img->GetXaxis()->GetBinWidth(1); 
  double cang = cos(ang); 
  double sang = sin(ang); 
  double xmin = img->GetXaxis()->GetXmin(); 
  double xmax = img->GetXaxis()->GetXmax(); 
  double ymin = img->GetYaxis()->GetXmin(); 
  double ymax = img->GetYaxis()->GetXmax(); 

  double xorig, yorig; 
  if (pt ==  0)
  {
    xorig = (xmax + xmin) / 2. ;
    yorig = (ymax + ymin) / 2. ;
  }
  else
  {
    xorig = pt->GetX(); 
    yorig = pt->GetY(); 
  }

  if (out == 0)
  {
    TString new_name = TString::Format("%s_rotated",img->GetName()); 

    double new_width = (xmax-xmin) * fabs(cang) + (ymax - ymin) * fabs(sang); 
    double new_height = (xmax-xmin) * fabs(sang) + (ymax - ymin) * fabs(cang); 

    out = dmtpc::image::tools::newTH2StealType(img, new_name,new_name, 
                          int(new_width/binning + 0.5), 
                          xorig - new_width/2, xorig + new_width/2, 
                          int(new_height/binning + 0.5), 
                          yorig- new_height/2, yorig + new_height/2); 
                               
  }


  for (int bx = 1; bx <= out->GetNbinsX(); bx++)
  {
    for (int by = 1; by <= out->GetNbinsY(); by++)
    {
      double x =  out->GetXaxis()->GetBinCenter(bx) - xorig; 
      double y =  out->GetYaxis()->GetBinCenter(by) - yorig; 

      double xprime = x * cang - y * sang  + xorig;
      double yprime = x * sang + y * cang  + yorig;

      if (xprime < xmin) continue; 
      if (xprime > xmax) continue; 
      if (yprime < ymin) continue; 
      if (yprime > ymax) continue; 

      out->SetBinContent(bx,by, dmtpc::image::interpolation::interpolate(img, xprime,yprime, interpolation)); 
    }
  }

  return out; 
}


TH2D * dmtpc::image::transform::radonTransform(const TH2 * img, int nbinstheta, int projbins) 
{

  if (img->GetNbinsX() != img->GetNbinsY()) 
  {
    std::cerr << "WARNING: nbinsx not = nbinsy for radon transform" << std::endl; 
  }
  double sqrt2 = sqrt(2); 
  double xmin = img->GetXaxis()->GetXmin(); 
  double xmax = img->GetXaxis()->GetXmax(); 
  double ymin = img->GetYaxis()->GetXmin(); 
  double ymax = img->GetYaxis()->GetXmax(); 
  double new_width = (xmax-xmin) * (sqrt2); 
  if (projbins < 0) projbins = int(new_width / img->GetXaxis()->GetBinWidth(1) + 0.5); 

  double new_xmin = (xmin+xmax - new_width)/2; 
  double new_xmax = (xmin +xmax + new_width)/2; 
  double new_ymin = (ymin+ymax - new_width)/2; 
  double new_ymax = (ymin +ymax + new_width)/2; 


  TString name = TString::Format("%s_radon",img->GetName()); 
  TString title = TString::Format("%s (Radon transform)",img->GetTitle()); 
  TH2D * out = new TH2D(name,title, projbins, new_xmin, new_xmax, 
                                    nbinstheta, -M_PI/2, M_PI/2); 


  TH2 * rotated = new TH2D("tmp_rotate","tmp_rotate", projbins, new_xmin, new_xmax, 
                                                       projbins, new_ymin, new_ymax); 



  for (int i = 0; i <= nbinstheta; i++)
  {
    rotated = rotateInterpolate(img, out->GetYaxis()->GetBinLowEdge(1 + i), 0, rotated); 

    TH1D * projx = rotated->ProjectionX(); 

    for (int j = 1; j <= projbins; j++)
    {
      out->SetBinContent(j,i,projx->GetBinContent(j)); 
      std::cout << projx->GetBinContent(j) << " "  ; 
    }
    std::cout << std::endl; 

    delete projx; 
  }

  delete rotated; 

  return out; 

}




void dmtpc::image::transform::projectAlongLine (const TH2 * img, TH1 ** longi, TH1 ** tranv, double x0, double y0, double x1, double y1, double width, const char * method) 
{
  double ang = atan2(y1-y0 ,x1-x0);    
  
  double xmin = std::min(x0 -width, x1 - width); 
  double xmax = std::max(x0 +width, x1 + width); 
  double ymin = std::min(y0 -width, y1 - width); 
  double ymax = std::max(y0 +width, y1 + width); 

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,28,0)
  int minbin = img->FindFixBin(xmin,ymin); 
  int maxbin = img->FindFixBin(xmax,ymax); 
#else

  int minbin = ((TH2*)img)->FindBin(xmin,ymin); 
  int maxbin = ((TH2*)img)->FindBin(xmax,ymax); 

#endif

  int minbx, maxbx, minby, maxby, z; 
  
  img->GetBinXYZ(minbin,minbx,minby,z); 
  img->GetBinXYZ(maxbin,maxbx,maxby,z); 

  xmin = img->GetXaxis()->GetBinLowEdge(minbx); 
  xmax = img->GetXaxis()->GetBinLowEdge(maxbx+1); 
  ymin = img->GetYaxis()->GetBinLowEdge(minby); 
  ymax = img->GetYaxis()->GetBinLowEdge(maxby+1); 
//  std::cout << xmin << " " << xmax << " : " << ymin << " " << ymax << std::endl; 


  double length = sqrt(pow(x1-x0,2) + pow(y1-y0,2)); 


  TH2 * cropped = dmtpc::image::tools::newTH2StealType(img, "croppedtmp","croppedtmp", maxbx - minbx +1, xmin,xmax, maxby - minby + 1, ymin, ymax); 

  for (int x = minbx; x <= maxbx; x++)
  {
    for (int y = minby; y <= maxby; y++)
    {
       cropped->SetBinContent(x-minbx+1, y - minby + 1, img->GetBinContent(x,y)); 
    }
  }

  TH2 * rotated = dmtpc::image::transform::rotateInterpolate(cropped, ang, 0, 0, method); 
  delete cropped; 

  double rotated_y = (rotated->GetYaxis()->GetXmax() + rotated->GetYaxis()->GetXmin())/2;
  double excess = (fabs(rotated->GetXaxis()->GetXmax() - rotated->GetXaxis()->GetXmin()) - length)/2; 
  double rotated_xmin = rotated->GetXaxis()->GetXmin() + excess; 
  double rotated_xmax = rotated->GetXaxis()->GetXmax() - excess; 
  

  for (int x = 1; x <= rotated->GetNbinsX(); x++)
  {
    for (int y = 1; y <= rotated->GetNbinsY(); y++)
    {
      if (rotated->GetXaxis()->GetBinCenter(x) < rotated_xmin)
      {
        rotated->SetBinContent(x,y,0); 
      }
      else if (rotated->GetXaxis()->GetBinCenter(x) > rotated_xmax)
      {
        rotated->SetBinContent(x,y,0); 
      }
      else if (fabs(rotated->GetYaxis()->GetBinCenter(y) - rotated_y) > width)
      {
        rotated->SetBinContent(x,y,0); 
      }
    }
  }

  char  name[128]; 

  sprintf(name,"_p%02f%02f%02f%02f%02fx",x0,x1,y0,y1,width); 
  *longi = rotated->ProjectionX(name); 
  (*longi)->SetAxisRange(rotated_xmin, rotated_xmax); 
  sprintf(name,"_p%02f%02f%02f%02f%02fy",x0,x1,y0,y1,width); 
  *tranv = rotated->ProjectionY(name); 
  (*tranv)->SetAxisRange( rotated_y - width, rotated_y + width); 


  delete rotated; 
}


TH2 * dmtpc::image::transform::cropZero(const TH2 *img)
{

  int xmin = img->GetNbinsX(); 
  int xmax = 1; 
  int ymin = img->GetNbinsY(); 
  int ymax = 1; 


  for (int i = 1; i <= img->GetNbinsX(); i++)
  {
    for (int j = 1; j <= img->GetNbinsY(); j++)
    {
       if (! img->GetBinContent(i,j)) continue; 

       if (i < xmin) xmin = i; 
       if (i > xmax) xmax = i; 
       if (j < ymin) ymin = j; 
       if (j > ymax) ymax = j; 
    }
  }

  return crop(img, xmin, xmax, ymin, ymax); 

}

TH2 * dmtpc::image::transform::crop(const TH2 *img, int xmin, int xmax, int ymin, int ymax)
{

  double xxmin = img->GetXaxis()->GetBinLowEdge(xmin); 
  double yymin = img->GetYaxis()->GetBinLowEdge(ymin); 
  double xxmax = img->GetXaxis()->GetBinLowEdge(xmax) + img->GetXaxis()->GetBinWidth(xmax); 
  double yymax = img->GetYaxis()->GetBinLowEdge(ymax) + img->GetXaxis()->GetBinWidth(ymax); 

  TH2 * cropped = dmtpc::image::tools::newTH2StealType(img,TString::Format("%s_cropped",img->GetName()),TString::Format("%s (cropped)", img->GetTitle()), xmax-xmin + 1, xxmin, xxmax, ymax-ymin+1,yymin,yymax); 

  for (int x = xmin; x <= xmax; x++)
  {
    for (int y = ymin; y <=ymax; y++)
    {
      cropped->SetBinContent(x-xmin+1, y-ymin+1, img->GetBinContent(x,y));
    }
  }

  return cropped; 
}

TH2 * dmtpc::image::transform::fftshift (TH2 * in, double wx, double wy, bool normalize) 
{
  int nx = in->GetNbinsX();
  int ny = in->GetNbinsX();

  in->GetXaxis()->SetLimits(-nx/2/wx,nx/2/wx); 
  in->GetYaxis()->SetLimits(-ny/2/wy,ny/2/wy); 

  for (int i = 0; i <nx; i++)
  {
    for (int j = 0; j <(ny+1)/2; j++)
    {
      int ito = (i + nx/2) % nx + 1; 
      int jto = (j + ny/2) % ny + 1; 
      double to = in->GetBinContent(ito,jto); 

      int ifrom = i+1; 
      int jfrom = j+1; 
      double from = in->GetBinContent(ifrom,jfrom); 

      in->SetBinContent(ito,jto,from); 
      in->SetBinContent(ifrom,jfrom,to); 
    }
  }


  if (normalize)
  {
    in->Scale(1./sqrt(nx*ny)); 
  }


  return in; 
}


TH2 * dmtpc::image::transform::zeroPad(TH2 * in, int newx, int newy, float newxpos, float newypos, const char * name)
{

  int nbinsx = in->GetNbinsX(); 
  int nbinsy = in->GetNbinsY(); 

  
  if (newx < nbinsx || newy < nbinsy)
  {
    std::cerr << "zeroPad(): new hist is smaller than old hist. Returning 0x0" << std::endl; 
    return 0; 
  }

  if (newxpos < -1 || newxpos > 1)
  {
    std::cerr << "zeroPad(): bad x position value. Using 0." << std::endl; 
    newxpos = 0; 
  }

  if (newypos < -1 || newypos > 1)
  {
    std::cerr << "zeroPad(): bad y position value. Using 0." << std::endl; 
    newypos = 0; 
  }

  double xbin = in->GetXaxis()->GetBinWidth(1); 
  double ybin = in->GetYaxis()->GetBinWidth(1); 

  double new_img_x = (newx - nbinsx) * xbin; 
  double new_img_y = (newy - nbinsy) * ybin; 

  double new_xmin = in->GetXaxis()->GetXmin() - (newxpos-1) * new_img_x/2.; 
  double new_xmax = in->GetXaxis()->GetXmax() + (newxpos+1) * new_img_x/2.; 

  double new_ymin = in->GetYaxis()->GetXmin() - (newypos-1) * new_img_y/2.; 
  double new_ymax = in->GetYaxis()->GetXmax() + (newypos+1) * new_img_y/2.; 


  int xbinoffset = (int)  ((newx - nbinsx) * (newxpos+1)/2.); 
  int ybinoffset = (int)  ((newy - nbinsy) * (newypos+1)/2.); 



  TString new_name = name[0]=='_' ?  in->GetName() + TString(name) : TString(name); 


  TH2 * newhist = dmtpc::image::tools::newTH2StealType(in, new_name, new_name, newx, new_xmin, new_xmax, newy, new_ymin, new_ymax); 

  for (int i = 1; i <=newx; i++)
  {  
    for (int j = 1; j <= newy; j++)
    {
       if ((i-1) < xbinoffset) continue; 
       if ((j-1) < ybinoffset) continue; 
       if ((i-1) >= xbinoffset + nbinsx) continue; 
       if ((j-1) >= ybinoffset + nbinsy) continue; 

       newhist->SetBinContent(i,j, in->GetBinContent(i-xbinoffset, j-ybinoffset)); 
    }
  }

  return newhist; 
}

TH2 * dmtpc::image::transform::zeroPadSquare(TH2 * in, float newpos, const char * name)
{

  int new_size = in->GetNbinsX() > in->GetNbinsY() ? in->GetNbinsX() : in->GetNbinsY(); 
  float xpos = in->GetNbinsX() > in->GetNbinsY() ? 0 : newpos; 
  float ypos = in->GetNbinsX() > in->GetNbinsY() ? newpos : 0; 

  return zeroPad(in, new_size, new_size, xpos, ypos, name); 

}
