#include "Pixel.hh"
#include <queue>
#include <cmath>
#include <iostream>
#include "ImageTools.hh"
#include "TH2.h"

/** In this file, put operations on groups of pixels. Things which modify the image
 * or create a new image should go in ImageProcessing instead.  */ 

std::vector<int> 
dmtpc::image::pixel::findNeighbors(TH2 *image, float threshold, int ibin) {
    // Check if pixels in segment between imax and jmax are touching border
    // of image frame.

  std::vector<int> pixels;
    if (image->GetBinContent(ibin)<threshold) return pixels; 
    pixels.push_back(ibin);

    
    for (unsigned int ip=0; ip<pixels.size(); ip++) {
        int xi = pixels[ip]%(image->GetNbinsX()+2);
        int yi = pixels[ip]/(image->GetNbinsX()+2);
        //cout << "checking bin " << xi<<","<<yi << endl;
        for (int i=xi-1; i<=xi+1; i++) {
            for (int j=yi-1; j<=yi+1; j++) {
                if (j==yi && i==xi) continue;
                if (image->GetBinContent(i,j)<threshold) continue;
                int newbin = image->GetBin(i,j);
                unsigned int k=0;
                for (; k<pixels.size(); k++) {
                    if (pixels[k]==newbin) break;
                }
                if (k==pixels.size()) pixels.push_back( image->GetBin(i,j) );
                //cout << "adding pixel " << i<<","<<j<<endl;
            }
        }
    }

    return pixels;
}

int dmtpc::image::pixel::clustWidth(std::vector<int> clusta, TH2* image)
{
  int nbinsx = image->GetNbinsX()+2;
  int nbinsy = image->GetNbinsY()+2;
  double ySuma=0.0;
  int count=0;
   
  for(int a = 0; a<int(clusta.size()); a++)
    {
      int xbina = int(clusta[a])%nbinsx;
      int ybina = ((int(clusta[a])-xbina)/nbinsx)%nbinsy;	 
      ySuma = ySuma + ybina;
      count++;
    }
  int Y = int(ySuma/count);

  int xmin = nbinsx+1;
  int xmax = 0;
  for(int a = 0; a<int(clusta.size()); a++)
  {
    int xbina = int(clusta[a])%nbinsx;
    int ybina = ((int(clusta[a])-xbina)/nbinsx)%nbinsy;
    if (ybina==Y)
    {
        if(xbina<xmin) xmin=xbina;
        if(xbina>xmax) xmax=xbina;
    }
  }

  return xmax-xmin;
}


std::vector<int> dmtpc::image::pixel::halfway(std::vector<int> clusta, std::vector<int> clustb, TH2* image, double mindist)
{
   double dist = 10000000;
   int X;
   int Y;

   int nbinsx = image->GetNbinsX()+2;
   int nbinsy = image->GetNbinsY()+2;
   double xSuma=0.0;
   double ySuma=0.0;
   double xSumb=0.0;
   double ySumb=0.0;
   int asize=int(clusta.size());
   int bsize=int(clustb.size());
   
   // finding shortest separation
   for(int a = 0; a<asize; a++)
   {
      for(int b = 0; b<bsize; b++)
      {	 
         int xbina = int(clusta[a])%nbinsx;
         int ybina = ((int(clusta[a])-xbina)/nbinsx)%nbinsy;
         int xbinb = int(clustb[b])%nbinsx;
         int ybinb = ((int(clustb[b])-xbinb)/nbinsx)%nbinsy;
         
         double xa = image->GetXaxis()->GetBinCenter(xbina);
         double ya = image->GetYaxis()->GetBinCenter(ybina);
         double xb = image->GetXaxis()->GetBinCenter(xbinb);
         double yb = image->GetYaxis()->GetBinCenter(ybinb);
         
         double testdist = pow(xa-xb,2.)+pow(ya-yb,2.);
         if(testdist < dist)
         {
             dist = testdist;
             X=(xbina+xbinb)/2;
             Y=(ybina+ybinb)/2;
          }
      }
   }

   int acount=0;
   int bcount=0;
   // finding center of clusters to refine halfway point
   for(int a = 0; a<asize; a++)
   {
       int xbina = int(clusta[a])%nbinsx;
       int ybina = ((int(clusta[a])-xbina)/nbinsx)%nbinsy;	 
	 
       double testdist = pow(xbina-X,2.)+pow(ybina-Y,2.);
       if(testdist < mindist)
       {
         xSuma = xSuma + xbina;
         ySuma = ySuma + ybina;
         acount++;
       }
   }
   for(int b = 0; b<bsize; b++)
   {	 
       int xbinb = int(clustb[b])%nbinsx;
       int ybinb = ((int(clustb[b])-xbinb)/nbinsx)%nbinsy;

       double testdist = pow(xbinb-X,2.)+pow(ybinb-Y,2.);
       if(testdist < mindist)
       {
         xSumb = xSumb + xbinb;
         ySumb = ySumb + ybinb;
         bcount++;
       }
    }

   double Xa = xSuma/acount;
   double Xb = xSumb/bcount;
   double Ya = ySuma/acount;
   double Yb = ySumb/bcount;
   X=int((Y-Ya)/(Yb-Ya)*(Xb-Xa)+Xa);
   
   std::vector<int> v;
   v.push_back(X);
   v.push_back(Y);
   return v;
}


double dmtpc::image::pixel::minDist(std::vector<int>&  clusta, std::vector<int> & clustb, const TH2* image)
{
   double dist = DBL_MAX;

   
   int bx,by,bz; 
   for(int a = 0; a<int(clusta.size()); a++)
   {
      for(int b = 0; b<int(clustb.size()); b++)
      {
	 
        image->GetBinXYZ(clusta[a], bx,by,bz); 
        double xa = image->GetXaxis()->GetBinCenter(bx); 
        double ya = image->GetYaxis()->GetBinCenter(by); 

        image->GetBinXYZ(clustb[b], bx,by,bz); 
        double xb = image->GetXaxis()->GetBinCenter(bx);
        double yb = image->GetYaxis()->GetBinCenter(by);
         
       double testdist = pow(xa-xb,2)+pow(ya-yb,2);
         
       if(testdist < dist) dist = testdist;
	 
      }
   }

   return dist;
}

int
dmtpc::image::pixel::countPixelsAboveThreshold(TH2* image, float threshold) {

    int n=0;
    int nx = image->GetNbinsX(); 
    int ny = image->GetNbinsY();
    for (int i=1; i<=nx; i++) {
        for (int j=1; j<=ny; j++) {
            if (image->GetBinContent(i,j)>threshold) n++;
        }
    }
    return n;
}

int dmtpc::image::pixel::selectPixelsAbove(const TH2 * img, std::set<int> * pixels, double thresh)
{
  int init = pixels->size();   
  for (int i = 1; i <= img->GetNbinsX(); i++)
  {
    for (int j = 1; j <= img->GetNbinsY(); j++)
    {
      if (img->GetBinContent(i,j) > thresh)
      {
        pixels->insert(img->GetBin(i,j)); 
      }
    }
  }

  return pixels->size() - init; 
}

int dmtpc::image::pixel::selectPixelsBelow(const TH2 * img, std::set<int> * pixels, double thresh)
{
  int init = pixels->size();   
  for (int i = 1; i <= img->GetNbinsX(); i++)
  {
    for (int j = 1; j <= img->GetNbinsY(); j++)
    {
      if (img->GetBinContent(i,j) < thresh)
      {
        pixels->insert(img->GetBin(i,j)); 
      }
    }
  }

  return pixels->size() - init; 
}

int dmtpc::image::pixel::selectPixelsBetween(const TH2 * img, std::set<int> * pixels, double thresh_low, double thresh_high)
{
  int init = pixels->size();   
  for (int i = 1; i <= img->GetNbinsX(); i++)
  {
    for (int j = 1; j <= img->GetNbinsY(); j++)
    {
      if (img->GetBinContent(i,j) < thresh_high && img->GetBinContent(i,j) > thresh_low)
      {
        pixels->insert(img->GetBin(i,j)); 
      }
    }
  }

  return pixels->size() - init; 
}


int dmtpc::image::pixel::outerBorder(const TH2 *img,  const std::set<int> * in, std::set<int>  * out) 
{
  for (std::set<int>::const_iterator it = in->begin(); it!=in->end(); it++)
  {
    int bx,by,bz; 
    img->GetBinXYZ(*it,bx,by,bz); 

    for (int i = -1; i <=1; i++)
    {
      for (int j = -1; j <=1; j++)
      {
          if (!i && !j) continue;       
          int testbin = img->GetBin(bx+i, by+j);  
          if (testbin >= 0  && !in->count(testbin))
          {
              out->insert(testbin); 
          }
      }
    }
  }

  return out->size(); 
}

int dmtpc::image::pixel::innerBorder(const TH2 *img,  const std::set<int> * in, std::set<int>  * out) 
{
  int init = out->size();
  for (std::set<int>::const_iterator it = in->begin(); it!=in->end(); it++)
  {
    int bx,by,bz; 
    img->GetBinXYZ(*it,bx,by,bz); 

    for (int i = -1; i <=1; i++)
    {
      for (int j = -1; j <=1; j++)
      {
          if (!i && !j) continue;       
          int testbin = img->GetBin(bx+i, by+j);  
          if (testbin >= 0  && !in->count(testbin))
          {
              out->insert(*it); 
              i=2; 
              break; 
          }
      }
    }
  }
  return out->size() - init; 
}


int dmtpc::image::pixel::erode(const TH2 * img, std::set<int> * pixels)
{
  std::set<int> killpix;     
  int n = innerBorder(img,pixels,&killpix);

  for (std::set<int>::const_iterator it = killpix.begin(); it!=killpix.end(); it++)
  {
    pixels->erase(*it); 
  }
  return n; 
}

int dmtpc::image::pixel::dilate(const TH2 * img, std::set<int> * pixels)
{
  std::set<int> newpix; 
  int n = outerBorder(img,pixels,&newpix);  

  for (std::set<int>::const_iterator it = newpix.begin(); it!=newpix.end(); it++)
  {
    pixels->insert(*it);
  }

  return n; 
}

void dmtpc::image::pixel::fillPixels(TH2* img, const std::set<int> * pixels, double val)
{
  for (std::set<int>::iterator it = pixels->begin(); it!=pixels->end(); it++)
  {
    img->SetBinContent(*it,val); 
  }
}

int dmtpc::image::pixel::getInversePixels(const TH2 * img, const std::set<int> * in, std::set<int> * out) 
{
  int n = 0; 
  for (int x = 1; x<= img->GetNbinsX(); x++)
  {
    for (int y = 1; y<= img->GetNbinsY(); y++)
    {
      int bin = img->GetBin(x,y); 
      if (!(in->count(bin)))
      {
        out->insert(bin); 
        n++; 
      }
    }
  }

  return n; 
}

void 
dmtpc::image::pixel::findHotCold(TH2* image, float low, float high, int* cold, int* hot, int &ncold, int &nhot)
{

    int nx = image->GetNbinsX();
    int ny = image->GetNbinsY();
    
    ncold =0;
    nhot=0;
    
    for (int i=1; i<=nx; i++) {
        for (int j=1; j<=ny; j++) {

	   if(image->GetBinContent(i,j) > high)
	   { 
	      cold[ncold] = image->GetBin(i,j);
	      ncold++;
	   }
	   if(image->GetBinContent(i,j) < low)
	   {
	      hot[nhot]=image->GetBin(i,j);
	      nhot++;
	   }
	   
        }
    }

}

int dmtpc::image::pixel::selectPixelsInsideBox(const TH2 * img, std::set<int> * pix, double xmin, double ymin, double xmax, double ymax) 
{
       
  int xbinmin = img->GetXaxis()->FindBin(xmin); 
  int ybinmin = img->GetYaxis()->FindBin(ymin); 
  int xbinmax = img->GetXaxis()->FindBin(xmax); 
  int ybinmax = img->GetYaxis()->FindBin(ymax); 

  int n = 0; 
  for (int x = xbinmin; x <= xbinmax; x++)
  {
    for (int y = ybinmin; y <= ybinmax; y++)
    {
        pix->insert(img->GetBin(x,y)); 
        n++; 
    }
  }

  return n;
}


TH2I * dmtpc::image::pixel::distanceTransform(const TH2*in, const std::set<int> * pixels)
{
  std::set<int> workspace(pixels->begin(),pixels->end()); 
  TH2I * out = (TH2I*)  dmtpc::image::tools::newTH2StealSize(in,'I',"dist","Distance Transform"); 

  while (workspace.size() > 0)
  {
    for (std::set<int>::iterator it = workspace.begin(); it!=workspace.end(); it++)
    {
      out->SetBinContent(*it, out->GetBinContent(*it)+1); 
    }

    erode(in,&workspace); 
  }

  return out; 
}


int dmtpc::image::pixel::selectPixelsInsideCircle(const TH2* img, std::set<int> * pix, double x0, double y0, double r)
{
  //First select bins inside bounding box

  std::set<int> boxpix; 
  selectPixelsInsideBox(img,&boxpix, x0 -r, y0-r, x0+r,y0+r); 

  //Then add anything inside the circle
  double r2 = r*r; 
  int n = 0; 
  for (std::set<int>::iterator it = boxpix.begin(); it != boxpix.end(); it++)
  {
    int bx,by,bz; 
    img->GetBinXYZ(*it,bx,by,bz); 
    double x = img->GetXaxis()->GetBinCenter(bx); 
    double y = img->GetYaxis()->GetBinCenter(by); 

    double dx = x -x0; 
    double dy = y-y0; 

    if (dx*dx + dy*dy <= r2) 
    {
      pix->insert(*it);  
      n++; 
    }
  }

  return n; 
}


int dmtpc::image::pixel::selectPixelsAboveWithAtLeastNNeighbors(const TH2 * img, std::set<int> * pix, double thresh, int n)
{
  

  std::set<int> newpix; 
  std::set<int> rejectpix; 

  for (int x = 1; x <= img->GetNbinsX(); x++)
  {
    for (int y = 1; y <= img->GetNbinsY(); y++)
    {
      int bin = img->GetBin(x,y); 

      if (img->GetBinContent(bin) < thresh || newpix.count(bin) || rejectpix.count(bin)) continue; 

      std::set<int> neighbors; 
      int nneigh = getNeighborsAboveThreshold(img,bin,&neighbors,thresh); 
      bool reject = nneigh < n; 
      for (std::set<int>::iterator it = neighbors.begin(); it != neighbors.end(); it++)
      {
        (reject ? rejectpix : newpix).insert(*it); 
      }
    }
  }


  for (std::set<int>::iterator it = newpix.begin(); it != newpix.end(); it++)
  {
    pix->insert(*it); 
  }

  std::cout << newpix.size() << std::endl; 
  return newpix.size(); 
}


int dmtpc::image::pixel::getNeighborsAboveThreshold(const TH2 * img, int bin, std::set<int> * pixels, double thresh) 
{
  if (img->GetBinContent(bin) < thresh) return 0;  

  std::queue<int> consider; 
  std::set<int> newpix; 

  consider.push(bin); 
  newpix.insert(bin); 


  while (consider.size() > 0) 
  {
      int cbin =  consider.front(); 
      consider.pop(); 
      //get neigbhbors
      int bx, by, bz; 
      img->GetBinXYZ(cbin,bx,by,bz); 

      for (int i = -1; i<=1; i++)
      {
        for (int j = -1; j <= 1; j++)
        {
           if (!i && !j) continue; 
           if (bx + i == 0 || bx + i ==img->GetNbinsX() + 1) continue; 
           if (by + j == 0 || by + j ==img->GetNbinsY() + 1) continue; 

           int nbin = img->GetBin(bx+i, by+j); 

           if (newpix.count(nbin)) continue; 

           if (img->GetBinContent(nbin) >= thresh)
           {
             newpix.insert(nbin); 
             consider.push(nbin); 
           }
        }
     }
  }

  for (std::set<int>::iterator it = newpix.begin(); it != newpix.end(); it++)
  {
    pixels->insert(*it); 
  }

  return newpix.size(); 

}


bool dmtpc::image::pixel::fullyConnected(const std::set<int> * all, const TH2 * img)
{
  std::set<int> connected; 

  connected.insert(*(all->begin())); 
  int nadded = 1; 
  int nbinsx = img->GetNbinsX() + 2; 
  while (nadded)
  {
    nadded = 0; 
    for (std::set<int>::iterator it = connected.begin(); it!= connected.end(); it++)
    {
      int px = *it; 

      for (int x = -1; x <=1; x++)
      {
        for (int y = -1; y <=1; y++)
        {
          if (!x && !y) continue; 
          int testpx = px + x + y * nbinsx; 

          if (all->count(testpx) && !connected.count(testpx))
          {
            nadded++; 
            connected.insert(testpx); 
          }
        }
      }
    }
  }

  return connected.size() == all->size(); 

}

