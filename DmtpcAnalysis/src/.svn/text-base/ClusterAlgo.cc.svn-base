#include "ClusterAlgo.hh"
#include "ImageTools.hh"
#include <limits.h>
#include "TPoints.h"
#include "ClusterImage.hh"
#include "ImageCalculations.hh"
#include "ImageTransforms.hh"
#include "TGraph.h"
#include "GainMap.hh"
#include "Pixel.hh" 
#include <cmath>

#include "TMath.h"

double dmtpc::analysis::clusteralgo::getIntegral(const Cluster *c, const GainMap * g, bool red )
{
  const std::vector<int> & pix = red ? c->reduced_pixels : c->pixels; 

  double answer = 0; 

  for (unsigned i = 0; i < pix.size(); i++)
  {
    double val = c->img->GetBinContent(pix[i]); 

    if (g) val *= g->getGainMap()->GetBinContent(pix[i]); 
    answer+= val;
  }

  return answer; 
}

double dmtpc::analysis::clusteralgo::getLength(const Cluster * c, double *x0 , double *x1, double *y0, double *y1, bool red ) 
{

   const std::vector<int> & px = red ? c->reduced_pixels : c->pixels; 

   int nbinsx = c->img->GetNbinsX() + 2; 
   int nbinsy = c->img->GetNbinsY() + 2; 
   
   double length = 0;
   for(int k=0; k<int(px.size()); k++)
   {
      for(int j=k+1; j<int(px.size());j++)
      {
        int ibinx = px[k]%nbinsx;
        int ibiny = ((px[k]-ibinx)/nbinsx)%nbinsy;
        int jbinx = px[j]%nbinsx;
        int jbiny = ((px[j]-jbinx)/nbinsx)%nbinsy;

        double ix = c->img->GetXaxis()->GetBinCenter(ibinx);
        double iy = c->img->GetYaxis()->GetBinCenter(ibiny);
        double jx = c->img->GetXaxis()->GetBinCenter(jbinx);
        double jy = c->img->GetYaxis()->GetBinCenter(jbiny);

        double testlen = (ix-jx)*(ix-jx)+(iy-jy)*(iy-jy);

        if(testlen > length)
        {
          length=testlen;
          if (x0) *x0=ix;
          if (x1) *x1=jx;
          if (y0) *y0=iy;
          if (y1) *y1=jy;
        }

      }
   }

   return sqrt(length);
}


double dmtpc::analysis::clusteralgo::getLengthAlongAngle(const Cluster * c, double phi, bool red) 
{

   const std::vector<int> & px = red ? c->reduced_pixels : c->pixels; 
   int nbinsx = c->img->GetNbinsX() + 2; 

   double minx = c->img->GetXaxis()->GetBinLowEdge(1);
   double miny = c->img->GetYaxis()->GetBinLowEdge(1);

   double mindist = 0;
   double maxdist = 0;

   for (unsigned i = 0; i < px.size(); i++)
   {
      //save only the two extreme values
      int xbin = px[i] % nbinsx;
      int ybin = px[i] / nbinsx;
      double dist = (c->img->GetXaxis()->GetBinCenter(xbin) - minx)*cos(phi)+ 
        (c->img->GetYaxis()->GetBinCenter(ybin) - miny)*sin(phi);

      if (dist < mindist || i == 0) mindist = dist;
      if (dist > maxdist || i == 0) maxdist = dist;
    }

    double length = maxdist - mindist;

    return length;
}


double dmtpc::analysis::clusteralgo::getPhiFromLength(const Cluster * c, bool reduced)  
{
  double x,y, x1,x2,y1,y2; 

  getLength(c,&x1,&x2,&y1,&y2,reduced);
  getXY(c,&x,&y); 

  //check which side centroid is closer to 

  if ( pow(x-x1,2) + pow(y-y1,2) < pow(x-x2,2) + pow(y-y2,2))
  {
    return atan2(y2-y1,x2-x1);
  }

  return atan2(y1-y2,x1-x2);
}


bool dmtpc::analysis::clusteralgo::edge(const Cluster * c)  
{
  const std::vector<int> & px = c->pixels; 

  int nx = c->img->GetNbinsX()+2; 
  int ny = c->img->GetNbinsY()+2; 

  for (unsigned i = 0; i < px.size(); i++)
  {
      int xbin = px[i] % nx;
      if (xbin == 1 || xbin == nx-2) return true; 
      int ybin = px[i] / nx;
      if (ybin == 1 || ybin == ny-2) return true; 
  }
  return false; 
}

double dmtpc::analysis::clusteralgo::getPhiPCA(const Cluster * c)  
{

  const std::vector<int> & px = c->pixels;
  int nbinsx = c->img->GetNbinsX() + 2; 
//  int nbinsy = c->img->GetNbinsY() + 2; 
  int size = (int) px.size();

  double meanx = 0; 
  double meany = 0; 
  double xsq = 0; 
  double ysq = 0; 
  double xy = 0; 
  double sum = 0; 
  for (int j = 0; j < size; j++){

    int xbin = px[j]%nbinsx;
    int ybin = px[j]/nbinsx;
    double content = c->img->GetBinContent(xbin,ybin);
    double xpos = c->img->GetXaxis()->GetBinCenter(xbin);
    double ypos = c->img->GetYaxis()->GetBinCenter(ybin);

    meanx += content*xpos;
    meany += content*ypos;
    xsq += content*xpos*xpos;
    ysq   += content*ypos*ypos;
    xy    += content*xpos*ypos;
    sum   += content;

  }
  meanx /= sum; meany /= sum; xsq /= sum; ysq /= sum; xy /= sum;

  double varx = xsq - meanx*meanx;//x-variance
  double vary = ysq - meany*meany;//y-variance
  double cov  = xy - meanx*meany;//covariance
  
  //unnormalized eigenvectors of the matrix I = [vary, - cov; - cov, varx] are in the form (A,B+-)
  double A      = 2 * cov;
  double Bplus  = vary - varx - sqrt((varx - vary)*(varx - vary) + 4.*cov*cov);
  double Bminus = vary - varx + sqrt((varx - vary)*(varx - vary) + 4.*cov*cov);
  double phi1 = atan2(Bplus,A);
  double phi2 = atan2(Bminus,A);


  double var1 = varx * cos(phi1)*cos(phi1) + vary * sin(phi1)*sin(phi1) + cov * sin(2.*phi1);
  double var2 = varx * cos(phi2)*cos(phi2) + vary * sin(phi2)*sin(phi2) + cov * sin(2.*phi2);

  double phi;
  if (var1 < var2) phi = phi2;
  else phi = phi1;

  if(phi > TMath::Pi()/2) phi = phi-TMath::Pi();


  double skewness = getAsymmetry(c,phi);

  if(skewness > 0)
  {
     if(phi <= 0)				
     {
        phi=TMath::Pi()+phi;
     }
     else if(phi > 0)
     {
        phi=phi-TMath::Pi();
     }
  }

  return phi;

}

double dmtpc::analysis::clusteralgo::getPhiMaxSigma(const Cluster * c) 
{
   const std::vector <int> & px = c->pixels; 
   int size     = (int) px.size();
   int nbinsx = c->img->GetNbinsX() + 2; 
//   int nbinsy = c->img->GetNbinsY() + 2; 

   double meanx = 0, meany = 0, xsq = 0, ysq = 0, xy = 0, sum = 0;
   
   for (int i = 0; i < size; i++){
      
      int xbin       = px[i]%(nbinsx);
      int ybin       = px[i]/(nbinsx);
      double content = c->img->GetBinContent(xbin,ybin);
      double xpos    = c->img->GetXaxis()->GetBinCenter(xbin);
      double ypos    = c->img->GetYaxis()->GetBinCenter(ybin);
      
      meanx += content*xpos;
      meany += content*ypos;
      xsq   += content*xpos*xpos;
      ysq   += content*ypos*ypos;
      xy    += content*xpos*ypos;
      sum   += content;
   }
   
   meanx /= sum; meany /= sum; xsq /= sum; ysq /= sum; xy /= sum;
   
   double varx = xsq - meanx*meanx;//x-variance
   double vary = ysq - meany*meany;//y-variance
   double cov  = xy - meanx*meany;//covariance
   double phi1 = 0.5*atan2(2.*cov,varx-vary);//see getPhi() for explanation of this
   double phi2 = phi1 + 0.5*TMath::Pi();
   double var1 = varx * cos(phi1)*cos(phi1) + vary * sin(phi1)*sin(phi1) + cov * sin(2.*phi1);
   double var2 = varx * cos(phi2)*cos(phi2) + vary * sin(phi2)*sin(phi2) + cov * sin(2.*phi2);
   
   double phi;
   if (var1 < var2){
      phi       = phi2;
      //    rmswidth  = sqrt(var1);
      //   rmslength = sqrt(var2);
   }
   else {
      phi       = phi1;
      //    rmswidth  = sqrt(var2);
      //    rmslength = sqrt(var1);
   }
   
   return phi;
}

double dmtpc::analysis::clusteralgo::getPhiLineFit(const Cluster * c ) 
{
  const std::vector <int> & px = c->pixels; 
  int size     = (int) px.size();
  int nbinsx = c->img->GetNbinsX() + 2; 
  int nbinsy = c->img->GetNbinsY() + 2; 

  TGraph g(size); 
  for (int j = 0; j < size; j++)
  {
    int xj = px[j]%nbinsx;
    int yj = ((px[j]-xj)/nbinsx)%nbinsy;
    g.SetPoint(j, c->img->GetXaxis()->GetBinCenter(xj), c->img->GetYaxis()->GetBinCenter(yj)); 
  }

  TF1 l("tmppol1","pol1"); 
  g.Fit(&l); 
  return atan(l.GetParameter(1)); 
}

double dmtpc::analysis::clusteralgo::getXY(const Cluster *c, double *xptr, double *yptr, const GainMap * g)
{

  double x = 0; 
  double y = 0; 

  const std::vector <int> & px = c->pixels; 
  int nx=c->img->GetNbinsX()+2;
  int ny=c->img->GetNbinsY()+2;
  double total = 0; 

  for(int j=0; j<int(px.size()); j++)
  {
     int xx = px[j]%nx;
     int yy = ((px[j]-xx)/nx)%ny;
     double xw=c->img->GetXaxis()->GetBinCenter(xx);
     double yw=c->img->GetYaxis()->GetBinCenter(yy);
     double content = c->img->GetBinContent(px[j]);

     if (g) content *= g->getGainMap()->GetBinContent(px[j]); 

     total += content; 
     x+=content*xw;
     y+=content*yw;
  }

  if (xptr) *xptr = x/total; 
  if (yptr) *yptr = y/total; 

  return total; 
}


bool dmtpc::analysis::clusteralgo::inCluster(const Cluster *c, int bin, bool red ) 
{

  const std::vector <int> & px = red ? c->reduced_pixels : c->pixels; 

  for (unsigned i = 0; i < px.size(); i++)
  {
    if (px[i] == bin) return true; 
  }

  return false; 
}
void dmtpc::analysis::clusteralgo::getEllipseAxes(const Cluster* c, double* a, double* b)
{
  const std::vector <int> & px = c->pixels; 
  int size = (int) px.size();
  int nbinsx = c->img->GetNbinsX(); 
  double meanx = 0, meany = 0, xsq = 0, ysq = 0, xy = 0, sum = 0;

  for (int j = 0; j < size; j++){

    int xbin = px[j]%nbinsx;
    int ybin = px[j]/nbinsx;
    double content = c->img->GetBinContent(xbin,ybin);
    double xpos = c->img->GetXaxis()->GetBinCenter(xbin);
    double ypos = c->img->GetYaxis()->GetBinCenter(ybin);

    meanx += content*xpos;
    meany += content*ypos;
    xsq += content*xpos*xpos;
    ysq   += content*ypos*ypos;
    xy    += content*xpos*ypos;
    sum   += content;

  }
  meanx /= sum; meany /= sum; xsq /= sum; ysq /= sum; xy /= sum;

  double varx = xsq - meanx*meanx;//x-variance
  double vary = ysq - meany*meany;//y-variance
  double cov  = xy - meanx*meany;//covariance
  
  if (a) 
  {
    *a = (varx+vary)/2 + sqrt((varx-vary)*(varx-vary)/4-cov*cov);
    *a = sqrt(*a);
  }
  if (b) 
  {
    *b = (varx+vary)/2 - sqrt((varx-vary)*(varx-vary)/4-cov*cov);
    *b = sqrt(*b);
  }
}

double dmtpc::analysis::clusteralgo::getRMS(const Cluster *c)
{

  double x = 0; 
  double xx = 0; 

  for (unsigned i = 0; i < c->pixels.size(); i++)
  {
    x += c->img->GetBinContent(c->pixels[i]); 
    xx += pow(c->img->GetBinContent(c->pixels[i]),2); 
  }

  xx/=c->pixels.size();
  x/=c->pixels.size();

  return sqrt(xx - x*x); 
}

double dmtpc::analysis::clusteralgo::getMean(const Cluster *c)
{
  return getIntegral(c) / c->pixels.size(); 
}

double dmtpc::analysis::clusteralgo::getSkewness(const Cluster *c, double phi)
{
   const std::vector <int> & px = c->pixels;
   int size = (int) px.size();

   double mean = 0;
   double total = 0;
   double offset = 0;
  
   for (int j = 0; j < size; j++){
      
      int xbin = px[j] % c->img->GetNbinsX();
      int ybin = px[j] / c->img->GetNbinsX();

      float distance=xbin*cos(phi)+ybin*sin(phi);
      
      float content = c->img->GetBinContent(px[j])+offset;

      if(content >0)
      {
        mean += content*distance;
        total += content;
      }
   }

   //cout << " w*x: " << mean << endl;
   //cout << " w: " << total << endl;
   mean /=total;
   
   //cout << "Mean: " << mean << endl;
   
   double moment3 = 0;
   double moment2 = 0;

   for (int j = 0; j < size; j++){
      
      int xbin = px[j] % c->img->GetNbinsX(); 
      int ybin = px[j]/ c->img->GetNbinsX(); 
      
      double dist = xbin*cos(phi) + ybin*sin(phi);

      double content = c->img->GetBinContent(px[j])+offset;
      if(content > 0)
      {	 
        moment3 += content*(dist-mean)*(dist-mean)*(dist-mean);
        //moment3 += content*(dist)*(dist)*(dist);
        moment2 += content*(dist-mean)*(dist-mean); 
      }
   }

   moment3 /= total;
   moment2 /= total;
   //cout << "second moment around mean: " << moment2 << endl;
   //cout << "sigma: " << sqrt(moment2) << endl;

   double skewness = moment3 / pow(moment2,1.5);

   //cout << "Moment 2: " << moment2 << "\t Moment 3: " 
	//<< moment3 << "\t Skewness: " << skewness << endl;
      
   return skewness;
}

double dmtpc::analysis::clusteralgo::getAsymmetry(const Cluster *c, double phi)
{
   const std::vector <int> & px = c->pixels; 
   int size = (int) px.size();

   double min = DBL_MAX;
   double max = -DBL_MAX;

   for(int j=0; j<size; j++)
   {

      int xbin = px[j] % c->img->GetNbinsX(); 
      int ybin = px[j] / c->img->GetNbinsX(); 

      double distance=xbin*cos(phi)+ybin*sin(phi);
      if(distance < min)
      {
        min = distance;
      }
      if(distance > max)
      {
        max = distance;
      }
   }

   double midpoint = (min+max)/2;


   double above=0;
   double below=0;
   
   
   for(int j=0; j<size; j++)
   {

      int xbin = px[j] % c->img->GetNbinsX(); 
      int ybin = px[j] / c->img->GetNbinsX(); 

      double distance=xbin*cos(phi)+ybin*sin(phi);
      double content=c->img->GetBinContent(px[j]);
      
      if(distance < midpoint)
      {
         below+=content;
      }
      if(distance > midpoint)
      {
         above+=content;
      }
   }
   
   
   if(below > above)
      return -1;
   else if (above > below)
      return 1;
   else
      return 0;

}

void dmtpc::analysis::clusteralgo::morphologicalOperation(Cluster * c, int nerode, int ndilate, bool red)
{

  std::vector <int> & px = red ? c->pixels : c->reduced_pixels; 

  if (nerode || ndilate)
  {
    std::set<int> moar_pix; 

    moar_pix.insert(px.begin(), px.end()); 

    for (int i = 0; i < nerode; i++)
    {
      dmtpc::image::pixel::erode(c->img, &moar_pix);
    }

    for (int i = 0; i < ndilate; i++)
    {
      dmtpc::image::pixel::dilate(c->img, &moar_pix);
    }

    px.clear();

    for (std::set<int>::iterator it = moar_pix.begin(); it!= moar_pix.end(); it++)
    {
      px.push_back(*it); 
    }
  }
}

double dmtpc::analysis::clusteralgo::getMax(const Cluster *c, int *maxpix)
{
  double max = -DBL_MAX; 
  int maxpixel = 0; 

  for (unsigned i = 0; i < c->pixels.size(); i++)
  {
    int pix = c->pixels[i]; 
    double val = c->img->GetBinContent(pix); 
    if (val > max)
    {
      maxpixel = pix; 
      max = val;
    }
  }

  if (maxpix) *maxpix = maxpixel; 

  return max; 
}


int dmtpc::analysis::clusteralgo::getNeighborsAboveThresh(const Cluster * c, double thresh, int bin)
{

  int bin_x, bin_y, bin_z; 
  c->img->GetBinXYZ(bin, bin_x, bin_y, bin_z); 

  int count = 0; 

  for (int i = bin_x -1; i <= bin_x + 1; i++)
  {
      for (int j = bin_y - 1; j <= bin_y + 1; j++)
      {
          if (i==bin_x && j==bin_y) continue; 
          if (c->img->GetBinContent(i,j) > thresh) count++; 
      }
  }
 
  return count; 

}


int dmtpc::analysis::clusteralgo::makeBoundary(const Cluster *c, std::list<TGraph> * list, bool red, bool draw, const char * drawopt, int color, int width)
{


  int nbinsx = c->img->GetNbinsX() + 2; 
  const std::vector<int>  & pix = red ? c->reduced_pixels : c->pixels; 

  int n = 0; 

  for (unsigned int i = 0; i < pix.size(); i++)
  {
    int bin = pix[i]; 
    //Loop over directions
    for (int dir = 0; dir < 4; dir++)
    {
      int bindir = 0; 
      if (dir == 0)
      {
       bindir = bin-1; 
      }
      else if (dir ==1)
     {
       bindir = bin - nbinsx; 
      }
      else if (dir == 2)
      {
       bindir = bin+1; 
      }
      else if (dir ==3)
      {
       bindir = bin + nbinsx; 
      }
 
      if (!inCluster(c,bindir, red))
      {
        n++; 
        list->push_back(TGraph(2)); 

        list->back().SetEditable(0); 
        list->back().SetBit(kCannotPick); 
        int bx,by; 
        bx = bin % nbinsx; 
        by = bin / nbinsx; 
        double x0 = c->img->GetXaxis()->GetBinLowEdge(bx); 
        double x1 = c->img->GetXaxis()->GetBinLowEdge(bx+1); 
        double y0 = c->img->GetYaxis()->GetBinLowEdge(by); 
        double y1 = c->img->GetYaxis()->GetBinLowEdge(by+1); 
//        double x0 = xmin + (bx-1) *binsize;  
//        double x1 = xmin + (bx) *binsize;
//        double y0 = xmin + (by-1) *binsize;  
//        double y1 = xmin + (by) *binsize; 
        if (dir == 0)
        {
          list->back().SetPoint(0, x0,y0); 
          list->back().SetPoint(1, x0,y1); 
        }
        else if (dir == 1)
        {
          list->back().SetPoint(0, x0,y0); 
          list->back().SetPoint(1, x1,y0); 
        }
        else if (dir == 2)
        {
          list->back().SetPoint(0, x1,y0); 
          list->back().SetPoint(1, x1,y1); 
        }
        else if (dir == 3)
        {
          list->back().SetPoint(0, x0,y1); 
          list->back().SetPoint(1, x1,y1); 
        }
 
        list->back().SetLineColor(color); 
        list->back().SetLineWidth(width); 
 

        if (draw) list->back().Draw(drawopt); 

      }
    }
  }
 


  return n;

}

void dmtpc::analysis::clusteralgo::clusterBinBounds(const Cluster *c, int * xmin, int *xmax, int *ymin, int * ymax) 
{
  int minx, maxx, miny, maxy; 
  int x,y;
  maxx = 0;
  maxy = 0; 
  minx = 1<<30;
  miny = 1<<30;  
  
  for (unsigned int j = 0; j < c->pixels.size(); j++)
  {
    int bin = c->pixels[j]; 

    int bz;
    c->img->GetBinXYZ(bin,x,y,bz); 
     
    minx = x < minx ? x : minx;
    miny = y < miny ? y : miny;
    maxx = x > maxx ? x : maxx;
    maxy = y > maxy ? y : maxy;
  }
 
  if (xmin) *xmin = minx; 
  if (xmax) *xmax = maxx; 
  if (ymin) *ymin = miny; 
  if (ymax) *ymax = maxy; 
}

void dmtpc::analysis::clusteralgo::clusterBounds(const Cluster *c, double * xmin, double *xmax, double *ymin, double * ymax, double border_px) 
{
  int minx, maxx, miny, maxy; 
  clusterBinBounds(c,&minx,&maxx,&miny, &maxy); 
 
  //convert to physical units 
  
  if (xmin) *xmin = c->img->GetXaxis()->GetBinCenter(minx) - border_px; 
  if (xmax) *xmax = c->img->GetXaxis()->GetBinCenter(maxx) + border_px; 
  if (ymin) *ymin = c->img->GetYaxis()->GetBinCenter(miny) - border_px; 
  if (ymax) *ymax = c->img->GetYaxis()->GetBinCenter(maxy) + border_px; 
}

TH2 * dmtpc::analysis::clusteralgo::clusterHist(const Cluster *c, int padding, bool red, GainMap * map, double mingain, bool setzero ) 
{
  int minbinx =  INT_MAX; 
  int maxbinx =  -INT_MAX; 
  int minbiny =  INT_MAX; 
  int maxbiny =  -INT_MAX; 
  double xmax = -DBL_MAX, ymax = -DBL_MAX, xmin = DBL_MAX,ymin = DBL_MAX; 

  int binx,biny, binz; 
  double x,y; 
  const std::vector<int> & pix = red ? c->reduced_pixels : c->pixels; 
  for (std::vector<int>::const_iterator it = pix.begin(); it !=pix.end(); it++)
  {
      c->img->GetBinXYZ(*it,binx,biny,binz);     
      x = c->img->GetXaxis()->GetBinCenter(binx); 
      y = c->img->GetYaxis()->GetBinCenter(biny); 

      if (binx < minbinx) minbinx = binx; 
      if (binx > maxbinx) maxbinx = binx; 
      if (biny < minbiny) minbiny = biny; 
      if (biny > maxbiny) maxbiny = biny; 
      if (x < xmin) xmin = x; 
      if (x > xmax) xmax = x; 
      if (y < ymin) ymin = y; 
      if (y > ymax) ymax = y; 
  }
 
  TString n (TString::Format("%s_clust%i",c->img->GetName(),c->i).Data()); 

  int nbinsx = maxbinx - minbinx + 1 + 2 * padding; 
  int nbinsy = maxbiny - minbiny + 1 + 2 * padding; 

  xmin -= padding * c->img->GetXaxis()->GetBinWidth(1); 
  xmax += padding * c->img->GetXaxis()->GetBinWidth(1); 
  ymin -= padding * c->img->GetYaxis()->GetBinWidth(1); 
  ymax += padding * c->img->GetYaxis()->GetBinWidth(1); 

  TH2 * ret = dmtpc::image::tools::newTH2StealType(c->img, n,n,nbinsx, xmin,xmax, nbinsy, ymin,ymax); 

  for (int ix = 0; ix < nbinsx; ix++)
  {
    for (int iy = 0; iy < nbinsy; iy++)
    {
      if (!setzero || dmtpc::analysis::clusteralgo::inCluster(c,c->img->GetBin(ix+minbinx,iy+minbiny), red))
      {
        double g = map ?  map->getGainMap()->GetBinContent(ix + minbinx, iy + minbiny) : 1; 
        if (ix+minbinx == 0 || ix+minbinx > c->img->GetNbinsX()) continue; 
        if (iy+minbiny == 0 || iy+minbiny > c->img->GetNbinsY()) continue; 
        if (g > mingain) 
          ret->SetBinContent(padding +ix+1,padding +iy+1, c->img->GetBinContent(ix+minbinx, iy+minbiny)/g); 
      }
    }
  }

  return ret; 


}

void dmtpc::analysis::clusteralgo::minmax(const Cluster * c, double phi, double * minval, double * maxval, bool red)
{

  const std::vector<int> & pix = red ? c->reduced_pixels : c->pixels; 

  int nbinsx = c->img->GetNbinsX(); 
  std::vector<int>::const_iterator iter =pix.begin();
  for (; iter < pix.end(); iter++){
    int xbin = (*iter) % nbinsx;
    int ybin = (*iter) / nbinsx;
    //getXYFromBinNo(*iter,&xbin,&ybin,false);
    double x = c->img->GetXaxis()->GetBinCenter(xbin);
    double y = c->img->GetYaxis()->GetBinCenter(ybin);
    double dist = x*cos(phi)+y*sin(phi);
    if (iter == pix.begin()){
      if (minval) *minval = dist;
      if (maxval) *maxval = dist;
    }
    else{
      if (minval) *minval = TMath::Min(*minval,dist);
      if (maxval) *maxval= TMath::Max(*maxval,dist);
    }
  }
}

TH1 * dmtpc::analysis::clusteralgo::projectLegacy(const Cluster *c, double phi, int binning, bool binningIspixPerBin, bool red)
{

  double minVal =0, maxVal = 0;
  minmax(c,phi,&minVal,&maxVal);
  int nbins;
//Set bins by either setting total bins or pixels per bin in projection
//Endpoints of cluster are set to the center of the end bins.
  if (binningIspixPerBin) {
    //nbins = static_cast<int>((maxVal - minVal) / binning);
    //maxVal = minVal+binning*tempSize;
    //maxVal += 0.5 * binning;
    //minVal -= 0.5 * binning;
    maxVal += 0.5*binning;
    minVal -= 0.5*binning;
    nbins = static_cast<int>((maxVal-minVal)/binning);
  }else {
    nbins = binning;
    double binsize = (maxVal - minVal) / binning;
    minVal -= 0.5*binsize * binning / (binning - 1.);
    maxVal += 0.5*binsize * binning / (binning - 1.);
  }


  const std::vector<int> &  pix = red ? c->reduced_pixels : c->pixels; 
  int nbinsx = c->img->GetNbinsX(); 

  TH1F *proj = new TH1F("projHist","projection",nbins,minVal,maxVal);
  for(std::vector<int>::const_iterator iter = pix.begin(); iter < pix.end(); iter++){
    int xbin = (*iter) % nbinsx;
    int ybin = (*iter) / nbinsx;

    double x = c->img->GetXaxis()->GetBinCenter(xbin);
    double y = c->img->GetYaxis()->GetBinCenter(ybin);
    proj->Fill(x*cos(phi)+y*sin(phi),c->img->GetBinContent(*iter));  
  }

  return proj;
}

TH1 * dmtpc::analysis::clusteralgo::project(const Cluster *c, double  phi, const char * interpolation_method , GainMap * map ,
                           double min_gain, bool reset_xaxis, const char * name, double * startpos, double mean, double readnoise)  
{

  //pad with 2 should be safe for rotation, I think
  TH2 * temp = clusterHist(c,2,false,map, min_gain,true); 
  TH2 * err_temp = (TH2*) temp->Clone("err_temp"); 


  for (int ix = 1; ix <= err_temp->GetNbinsX(); ix++)
  {
    for (int iy = 1; iy <= err_temp->GetNbinsY(); iy++)
    {
      
      //Start with readnoise
      double err2 = readnoise*readnoise; 
      //Add estimate  of poisson noise (WARNING THIS IS AN OVERESTIMATE)
      err2 += temp->GetBinContent(ix,iy) > mean ? temp->GetBinContent(ix,iy) - mean : 0 ; 
      err_temp->SetBinContent(ix,iy,err2); 
    }
  }
    

  TPoints pts; 
  if (startpos)
  {
    pts.SetX(startpos[0]);
    pts.SetY(startpos[0]);
  }

  TH2 * rotated = dmtpc::image::transform::rotateInterpolate(temp, phi,startpos ? &pts : 0 ,0,interpolation_method); 
  TH2 * rotated_err = dmtpc::image::transform::rotateInterpolate(err_temp, phi,startpos ? &pts : 0 ,0,interpolation_method); 

  int nbinsx = rotated->GetNbinsX(); 
  int nbinsy = rotated->GetNbinsY(); 

  double xmin=0,xmax=0; 
  int minbin = 1; 
  int maxbin = nbinsx; 

  //find minimum bins... 
  bool done = false; 
  for (int ix = 1; ix <= nbinsx; ix++)
  {
    for (int iy = 1; iy <= nbinsy; iy++)
    {
      if (rotated->GetBinContent(ix,iy) > 0)
      {
        minbin = ix;  
        xmin = rotated->GetXaxis()->GetBinLowEdge(ix); 
        done = true; 
        break; 
      }
    }
    if (done) break; 
  }

  done = false; 

  for (int ix = nbinsx; ix >= 1; ix--)
  {
    for (int iy = 1; iy <= nbinsy; iy++)
    {
      if (rotated->GetBinContent(ix,iy) > 0)
      {
        maxbin = ix;  
        xmax = rotated->GetXaxis()->GetBinLowEdge(ix) + rotated->GetXaxis()->GetBinWidth(ix); 
        done = true; 
        break; 
      }
    }
    if (done) break; 
  }
 



  TString n(name ? name : TString::Format("%s_proj%d_%f",c->img->GetName(),c->i,phi).Data()); 

  TH1F * projection = new TH1F(n,n, maxbin-minbin+1, reset_xaxis ? 0 : xmin, reset_xaxis ? xmax-xmin : xmax); 

//  std::cout << projection->GetNbinsX() << std::endl; 

  for (int ix = minbin; ix <= maxbin; ix++)
  {
    for (int iy = 1; iy <= nbinsy; iy++)
    {
        projection->SetBinContent(ix - minbin + 1,projection->GetBinContent(ix - minbin + 1) + rotated->GetBinContent(ix,iy)); 
        projection->SetBinError(ix - minbin + 1,projection->GetBinError(ix - minbin + 1)+ rotated_err->GetBinContent(ix,iy)); //Fill error with number of things projected
    }
  }

  for (int ix = 1; ix <= projection->GetNbinsX(); ix++)
  {
    projection->SetBinError(ix, sqrt(projection->GetBinError(ix))); 
  }


  temp->Delete(); 
  err_temp->Delete(); 
  rotated->Delete(); 
  rotated_err->Delete(); 

  return projection; 
}


double dmtpc::analysis::clusteralgo::moment(const Cluster *c, int n, double phi, int binning, bool binningIsPixPerBin , bool red)
{
  const std::vector<int> & pix = red ? c->reduced_pixels : c->pixels;
  std::vector<int>::const_iterator iter = pix.begin();
  double ave =0, moment = 0, total = 0;
  int nbinsx = c->img->GetNbinsX(); 

  if (binning == 0){
    std::vector<double> dist;
    std::vector<double> counts;
    for (; iter < pix.end(); iter++){
      int xbin = (*iter) % nbinsx;
      int ybin = (*iter) / nbinsx;
      double x = c->img->GetXaxis()->GetBinCenter(xbin);
      double y = c->img->GetYaxis()->GetBinCenter(ybin);
      double tempCount = c->img->GetBinContent(*iter);
      total += tempCount;
      counts.push_back(tempCount);
      dist.push_back(x*cos(phi)+y*sin(phi));
      ave += counts[counts.size()-1]*dist[dist.size()-1];
    }
    ave /= total;
    if (n != 1){
      for (unsigned int ii = 0; ii < dist.size(); ii++){
        moment += counts[ii]*pow(dist[ii]-ave,n);
      }
      moment /= total;
    }else moment = ave;
  }else{//project onto an axis

    TH1* hist = projectLegacy(c,phi,binning,binningIsPixPerBin);
    ave = 0;
    for (int ii=1; ii<=hist->GetNbinsX(); ii++){
      ave+= hist->GetBinContent(ii)*hist->GetXaxis()->GetBinCenter(ii);
    }
    ave/=hist->Integral();

    if (n!=1){
      total = hist->Integral();
      for (int i = 1; i <= hist->GetNbinsX(); i++){
        //prevent moment from being negative 
        moment += TMath::Max(0.,hist->GetBinContent(i)*pow(hist->GetXaxis()->GetBinCenter(i)-ave,n));
      }
      moment /= total;
    }else moment = ave;    
    delete hist;
  }

  return moment;
}

double dmtpc::analysis::clusteralgo::maxDerivativeX(const Cluster * c) 
{

  int xmax,ymax,xmin,ymin; 
  clusterBinBounds(c, &xmin,&xmax, &ymin,&ymax); 
  double max_deriv = 0; 
  double col_sum = 0; 
  double last_col_sum = 0; 
  double max_height = 0; 

  for (int bx = xmin-1; bx <= xmax+1; bx++)
  {
    last_col_sum = col_sum; 
    col_sum = 0; 
    for (int by = ymin; by <= ymax; by++)
    {
      col_sum += c->img->GetBinContent(bx,by); 
    }

    if (bx >=xmin)
    {
      double deriv = fabs(col_sum - last_col_sum); 
      if (deriv > max_deriv) max_deriv = deriv; 
    }

    if (col_sum > max_height) max_height = col_sum;

  }

  return max_deriv / max_height; 
}

double dmtpc::analysis::clusteralgo::minDistance(const Cluster *c, double x, double y) 
{
  const std::vector<int> & pix = c->pixels; 
  double min = DBL_MAX; 
  int bx,by,bz;
  double xx,yy;
  for (unsigned i =0; i < pix.size(); i++)
  {
    c->img->GetBinXYZ(pix[i],bx,by,bz); 
    double xx = c->img->GetXaxis()->GetBinCenter(bx);
    double yy = c->img->GetYaxis()->GetBinCenter(by);
 
    double dist =  (xx-x)*(xx-x) + (yy-y)*(yy-y) ;
    if ( dist < min)
      min = dist; 

  }

  return sqrt(min); 
}

double dmtpc::analysis::clusteralgo::maxDistance(const Cluster *c, double x, double y) 
{
  const std::vector<int> & pix = c->pixels; 
  double max = 0; 
  int bx,by,bz;
  double xx,yy;
  for (unsigned i =0; i < pix.size(); i++)
  {
    c->img->GetBinXYZ(pix[i],bx,by,bz); 
    double xx = c->img->GetXaxis()->GetBinCenter(bx);
    double yy = c->img->GetYaxis()->GetBinCenter(by);
 
    double dist =  (xx-x)*(xx-x) + (yy-y)*(yy-y) ;
    if ( dist > max)
      max = dist; 

  }

  return sqrt(max); 

}

double dmtpc::analysis::clusteralgo::nearestSpacerDistance(const Cluster *c, const GainMap * gm, int & nearest)
{
  if (!gm) return -1; 
  if (gm->getNSpacers() == 0) return -1; 


  const std::vector<int> & pix = c->pixels;
  std::vector<int>::const_iterator iter = pix.begin();
  int nearest_spacer = -1; 
  double nearest_distance = -1; 

  while (iter != pix.end())
  {

    int bx,by,bz; 
    c->img->GetBinXYZ(*iter,bx,by,bz); 

    double x = c->img->GetXaxis()->GetBinCenter(bx);
    double y = c->img->GetYaxis()->GetBinCenter(by);
    int spacer; 
    double dist = gm->distanceToNearestSpacer(x,y,spacer); 

    if (nearest_distance < 0 || dist > nearest_distance)
    {
      nearest_spacer = spacer; 
      nearest_distance = dist; 
    }

    iter++; 
  }


  nearest = nearest_spacer; 
  return nearest_distance; 
}

bool dmtpc::analysis::clusteralgo::fullyConnected(const Cluster *c )
{


  const std::vector<int> & pix = c->pixels;
  std::set<int> all(pix.begin(), pix.end()); 
  return dmtpc::image::pixel::fullyConnected(&all, c->img); 

}



