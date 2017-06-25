#include "ImageCalculations.hh"
#include "TCutG.h" 
#include "TF1.h"
#include <set>
#include "TH1.h" 
#include <vector>
#include "TH2.h" 
#include "TMath.h"
#include "TPoints.h" 
#include "TMatrixD.h"
#include "TMatrixDEigen.h"
#include "TCanvas.h"

/** Here we calculate things about images */ 

/** USE THIS INSTEAD OF TMath::IsInside in 5.24 since 5.24 IsInside requires repeating last vertex. Source: 
 * http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html  */ 

int pnpoly(int nvert, double *vertx, double *verty, double testx, double testy)
{
    int i, j, c = 0;
      for (i = 0, j = nvert-1; i < nvert; j = i++) {
            if ( ((verty[i]>testy) != (verty[j]>testy)) &&
                   (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
                     c = !c;
              }
        return c;
}


double dmtpc::image::calculations::maxProjectedStep(const TH2 * img, char axis)
{
  if (axis < 0 || axis > 1) return -1;  

  TH1D * proj = axis ? img->ProjectionX() : img->ProjectionY(); 
 
  double max= 0; 
  int nx = proj->GetNbinsX(); 
  for (int i =1; i < nx; i++) 
  {
    double val = TMath::Abs(proj->GetBinContent(i)- proj->GetBinContent(i+1));
    max = val > max ? val : max; 
  }

  return max; 
}

bool
dmtpc::image::calculations::hasNeighbor(const TH2* image, int i, int j, float threshold, int minNeighbors) {
  // Check if pixel has neighbor(s) that are above threshold.
    
    int countNeighbors=0;
    for (int ii=i-1; ii<=i+1; ii++) {
        for (int jj=j-1; jj<=j+1; jj++) {
            if (ii==i && jj==j) continue;
            if (image->GetBinContent(ii,jj)<threshold) continue;
            countNeighbors++;
        }
    }
    return countNeighbors>=minNeighbors;
}

bool
dmtpc::image::calculations::hasNeighbor(const TH1* image, int i, float threshold, int minNeighbors) {
    // Check if pixel has neighbor(s) that are above threshold.
    
    int countNeighbors=0;
    for (int ii=i-1; ii<=i+1; ii++) {
        if (ii==i || ii<1 || ii>image->GetNbinsX()) continue;
        if (image->GetBinContent(ii)<threshold) continue;
        countNeighbors++;
    }
    return countNeighbors>=minNeighbors;
}


double
dmtpc::image::calculations::calcPixelDistance(const TH2* image, int i, int j) {
    // Find distance (in bins) between the first bin i and
    // the last bin j.
    
    int xi = i%(image->GetNbinsX()+2);
    int yi = i/(image->GetNbinsX()+2);
    int xj = j%(image->GetNbinsX()+2);
    int yj = j/(image->GetNbinsX()+2);    
    return sqrt( (xi-xj)*(xi-xj) + (yi-yj)*(yi-yj) );
}





double
dmtpc::image::calculations::calcSkewness(TH1* strip, int minx, int maxx, double bkg) {
        // Calculate skewness in the given bin range.

        if (maxx<=minx) { return -666; }

        int n=strip->GetNbinsX();
        for (int i=1; i<=n; i++) strip->SetBinContent(i, strip->GetBinContent(i)-bkg);
        strip->GetXaxis()->SetRange(minx,maxx);
        return strip->GetSkewness(1);
}

double
dmtpc::image::calculations::calcSkewness2D(TH2* image, int minbin, int maxbin, double bkg) {
    // Calculate skewness in the given bin range in 2d image.
    // Assume only counts above threshold are used
	
    minbin=maxbin; // dummy to avoid compilation warnings

    int nx=image->GetNbinsX();
    int ny=image->GetNbinsY();
    for (int i=1; i<=nx; i++) {
        for (int j=1; j<ny; j++) {
            if (image->GetBinContent(i,j)>0)
                image->SetBinContent( i, j, image->GetBinContent(i,j)-bkg );
        }
    }
    TH1D* proj=image->ProjectionX("_tmp_proj");
    double skewness = proj->GetSkewness(1); proj->DrawCopy();
    delete proj;
                                   
    return skewness;
}


int
dmtpc::image::calculations::findMaximum(const TH1 *project, float threshold, int minNeighbors) {
    // Find maximum that has neighbor(s).
        
    int imax = project->GetMaximumBin(); 
    float max = project->GetMinimum();
    int nbin = project->GetNbinsX()+1;
    std::vector<int> rejected;
    while (1) {
        int countNeighbors=0;
        for (int i=imax-2; i<=imax+2; i++) {
            if (i!=imax && project->GetBinContent(i)>threshold) countNeighbors++;
        }
        //cout << imax << "  " << max<<"   "<<threshold<<"   "<<countNeighbors << endl;
        if ( countNeighbors<minNeighbors ) {
            rejected.push_back(imax);
            imax= 0;
            max = project->GetMinimum();
            for (int i=1; i<nbin; i++) {
                if ( project->GetBinContent(i)>max &&
                     find( rejected.begin(), rejected.end(), i)==rejected.end()  ) {
                    imax=i; 
                    max=project->GetBinContent(i); 
                }
            }
            if (!imax) break;
            continue;
        }
        break;
    }
    return imax;
}


void
dmtpc::image::calculations::countSegmentLength(const TH1 *project, float threshold, int pixelsBelowThreshold, int &iR, int &iL) {
    // Estimate the width of signal by finding the number of pixels around the maximum
    // bin that are above the threshold. The maximum is required to have contiguous pixels above threshold.
    //
    
    // Find maximum bin
    //cout <<"CHANGED FINDMAXIMUM CALL"<<endl;
    int imax =findMaximum(project, threshold, 4);
    if (!imax) { iR=iL=0; }
    int nbin=project->GetNbinsX();
    //cout << "TEST:"<< threshold<<"  "<< imax<<"   "<< pixelsBelowThreshold<<endl;
    
    // Count pixels to the left and right that are above threshold
    int countEmpty=0;
    for (iR=imax; iR<=nbin; iR++) {
        if (project->GetBinContent(iR)<threshold) {
            countEmpty++;
           if (countEmpty>pixelsBelowThreshold) {countEmpty--; break;} // drop out when you find discontinuity
        }
        else countEmpty=0;
    }        
    iR -= countEmpty+1; // last filled bin
    
    countEmpty=0;
    for (iL=imax; iL>=1; iL--) {
        if (project->GetBinContent(iL)<threshold) {
            countEmpty++;
            if (countEmpty>pixelsBelowThreshold) {countEmpty--; break;} // drop out when you find discontinuity
        }
        else countEmpty=0;
    }        
    iL += countEmpty+1; // last filled bin
}


int
dmtpc::image::calculations::findMaximum2D(const TH2 *image, float threshold, int minNeighbors) {
    // Find maximum that has neighbor(s).

    int maxbin = image->GetMaximumBin(); 
    int imax = maxbin/(image->GetNbinsX()+2); 
    int jmax = maxbin/(image->GetNbinsY()+2); 
    float max = image->GetMinimum();
    int nbin = image->GetNbinsX()*image->GetNbinsY()+1;
    std::vector<int> rejected;
    while (1) {
        int countNeighbors=0;
        for (int i=imax-1; i<=imax+1; i++) {
            for (int j=jmax-1; j<=jmax+1; j++) {
                if (i!=imax && j!=jmax && image->GetBinContent(i,j)>threshold) countNeighbors++;
            }
            //cout << imax << "  " << max<<"   "<<threshold<<"   "<<countNeighbors << endl;
        }
        if ( countNeighbors<minNeighbors ) {
            rejected.push_back(maxbin);
            maxbin = 0;
            max = image->GetMinimum();
            for (int i=1; i<nbin; i++) {
                if ( image->GetBinContent(i)>max &&
                     find( rejected.begin(), rejected.end(), i)==rejected.end()  ) {
                    maxbin=i; 
                    max=image->GetBinContent(i); 
                }
            }
            if (!maxbin) break;
            continue;
        }
        break;
    }
    return maxbin;
}
    
void
dmtpc::image::calculations::countSegmentLength2D(const TH2 *image, float threshold, int &imax, int &jmax) {
    // Estimate the width of signal by counting the number of pixels above around the maximum
    // bin that are above the threshold. All bins are required to have neighbor(s) above threshold.
    //

    int nbin = (image->GetNbinsX()+2)*(image->GetNbinsY()+2)+1;
    float xi,yi,xj,yj,d2,d2max=-1;
    imax=jmax=0;
    for (int i=1; i<nbin; i++) {
        
        if (image->GetBinContent(i)<=threshold) continue;
        xi = i%(image->GetNbinsX()+2);
        if (xi==0 || xi==image->GetNbinsX()+1) continue;
        yi = i/(image->GetNbinsX()+2);
        if (yi==0 || yi==image->GetNbinsY()+1) continue;
        
        for (int j=i+1; j<nbin; j++) {
            
            if (image->GetBinContent(j)<=threshold) continue;
            xj = j%(image->GetNbinsX()+2);
            if (xj==0 || xj==image->GetNbinsX()+1) continue;
            yj = j/(image->GetNbinsX()+2);
            if (yj==0 || yj==image->GetNbinsY()+1) continue;
            
            d2 = (xi-xj)*(xi-xj) + (yi-yj)*(yi-yj);
            //cout << xi<<","<<yi<<"   "<<xj<<","<<yj<<"  d2="<<d2<<endl;
            if (d2>d2max) {
                d2max=d2;
                imax=i;
                jmax=j;
                //cout<<"max"<<endl;
            }
            
        }
    }    
}
double
dmtpc::image::calculations::calcIntensity2D(const TH2 *image, int imax, int jmax, int sidebandBins, float &sumSB ) {

    int xi = imax%(image->GetNbinsX()+2);
    int yi = imax/(image->GetNbinsX()+2);
    int xj = jmax%(image->GetNbinsX()+2);
    int yj = jmax/(image->GetNbinsX()+2);    

    // redefine points: (minx,miny)  and  (maxx,maxy)
    if (xi>xj) { int tmp=xi; xi=xj; xj=tmp; }
    if (yi>yj) { int tmp=yi; yi=yj; yj=tmp; }
    xi-=3;
    yi-=3;
    xj+=3;
    yj+=3;
    if (xi<1) xi=1;
    if (xj>image->GetNbinsX()) xj=image->GetNbinsX();
    if (yi<1) yi=1;
    if (yj>image->GetNbinsY()) yj=image->GetNbinsY();
    
    
    // define sidebands
    int x0 = xi-sidebandBins;
    int x1 = xj+sidebandBins;
    int y0 = yi-sidebandBins;
    int y1 = yj+sidebandBins;    
    if (x0<1) x0=1;
    if (x1>image->GetNbinsX()) x1=image->GetNbinsX();
    if (y0<1) y0=1;
    if (y1>image->GetNbinsY()) y1=image->GetNbinsY();


        
    float sumSR=0;
    sumSB=0;
    for (int i=x0; i<=x1; i++) {
        for (int j=y0; j<=y1; j++) {
            if (i>=xi && i<=xj && j>=yi && j<=yj) { sumSR+=image->GetBinContent(i,j); continue; }
            else sumSB += image->GetBinContent(i,j);
        }
    }
    float aSR=(xj-xi+1)*(yj-yi+1);
    float aSB=(x1-x0+1)*(y1-y0+1);

    //cout << aSR<<"  "<<aSB<<endl;
    
    sumSB /= (aSB-aSR); // background per pixel
    
    sumSR -= sumSB*aSR; // signal = tot. counts - background in signal region

    std::cout << xi<<"-"<<xj<<"  "<<yi<<"-"<<yj<<"  "
         << x0<<"-"<<x1<<"  "<<y0<<"-"<<y1<< " counts: "
         << sumSR << "  " << sumSB << std::endl;
    return sumSR;
}

void
dmtpc::image::calculations::rotateTCutG(TCutG *cut, float ang_rad, TPoints *pt) {
  // rotate the TCutG object

  int ncut = cut->GetN();
//  int nx = anodeImage_D->GetNbinsX();
//  float xmin  = anodeImage_D->GetXaxis()->GetBinLowEdge(1);
//  float xmax  = anodeImage_D->GetXaxis()->GetBinLowEdge(nx)+anodeImage_D->GetXaxis()->GetBinWidth(nx);
//
//  int ny = anodeImage_D->GetNbinsY();
//  float ymin  = anodeImage_D->GetYaxis()->GetBinLowEdge(1);
//  float ymax  = anodeImage_D->GetYaxis()->GetBinLowEdge(ny)+anodeImage_D->GetYaxis()->GetBinWidth(ny);
//
//  float x0 = 0.5*(xmax+xmin);
//  float y0 = 0.5*(ymax+ymin);
//  cout << "rotAng_deg, x0, y0 = " << rotAng_deg << ", " << x0 << ", " << y0 << endl;
//
  // if a pivot point is specified then use it
  // otherwise, calculate the pivot point to be the average of the extreme
  // x and y values in the TCutG object

  if (pt == 0) pt = getCenter(cut);
  Double_t x0 = pt->GetX();
  Double_t y0 = pt->GetY();

  Double_t xx, yy;
  for (int ic=0; ic<ncut; ic++) {
    cut->GetPoint(ic, xx, yy);
    cut->SetPoint(ic, 
		  x0+(TMath::Cos(ang_rad)*(xx-x0)-TMath::Sin(ang_rad)*(yy-y0)), 
		  y0+(TMath::Sin(ang_rad)*(xx-x0)+TMath::Cos(ang_rad)*(yy-y0)));
  }

}

void
dmtpc::image::calculations::getExtent(TCutG* cut, TPoints* xrng, TPoints* yrng) {

  int npt = cut->GetN();
  Double_t xx, yy;
  Double_t xmin, xmax, ymin, ymax;
  xmin = ymin = 9e10;
  xmax = ymax = -xmin;
  for (int ipt=0; ipt<npt; ipt++) {
    cut->GetPoint(ipt, xx, yy);
    xmin = (xx < xmin) ? xx : xmin;
    xmax = (xx > xmax) ? xx : xmax;
    ymin = (yy < ymin) ? yy : ymin;
    ymax = (yy > ymax) ? yy : ymax;
  }

  xrng->SetX(xmin); xrng->SetY(xmax);
  yrng->SetX(ymin); yrng->SetY(ymax);
  
}


TPoints*
dmtpc::image::calculations::getCenter(TCutG* cut) {
  
  int npt = cut->GetN();
  Double_t xx, yy;
  Double_t xmin, xmax, ymin, ymax;
  xmin = ymin = 9e10;
  xmax = ymax = -xmin;
  for (int ipt=0; ipt<npt; ipt++) {
    cut->GetPoint(ipt, xx, yy);
    xmin = (xx < xmin) ? xx : xmin;
    xmax = (xx > xmax) ? xx : xmax;
    ymin = (yy < ymin) ? yy : ymin;
    ymax = (yy > ymax) ? yy : ymax;
  }

  TPoints* pt = new TPoints(0.5*(xmin+xmax), 0.5*(ymin+ymax));
  return pt;
}

TPoints*
dmtpc::image::calculations::getCenter(const TH2* img) {

  int nx = img->GetNbinsX();
  int ny = img->GetNbinsY();
  TPoints *pt = new TPoints(0.5*(img->GetXaxis()->GetBinUpEdge(nx) - img->GetXaxis()->GetBinLowEdge(1)), 0.5*(img->GetYaxis()->GetBinUpEdge(ny) - img->GetYaxis()->GetBinLowEdge(1)));

  return pt;
}

Int_t
dmtpc::image::calculations::countPix(const TH2* img, TCutG* cut, bool cutint) {

   int nbinsx = img->GetNbinsX();
   int nbinsy = img->GetNbinsY();

   TAxis *xaxis = img->GetXaxis();
   TAxis *yaxis = img->GetYaxis();
   int npix = 0;
   for(int i=1; i<=nbinsx; i++) {
     Double_t x = xaxis->GetBinCenter(i);
     for(int j=1; j<=nbinsy; j++) {
       Double_t y = yaxis->GetBinCenter(j);
       bool isInRegion = (cut !=0) && (cut->IsInside(x,y));
       if ((cut!=0 && !isInRegion &&  cutint) || 
	   (cut!=0 &&  isInRegion && !cutint)) continue;
       npix++;
     }
   }
   return npix;
}


TCutG* 
dmtpc::image::calculations::makeTCutGRect(TString name, float x0, float y0, float x1, float y1) {
  TCutG *cut = new TCutG(name, 0); int ipt=0;
  cut->SetPoint(ipt++, x0, y0);
  cut->SetPoint(ipt++, x0, y1);
  cut->SetPoint(ipt++, x1, y1);
  cut->SetPoint(ipt++, x1, y0);
  cut->SetPoint(ipt++, x0, y0);  // close the figure
  return cut;
}


// @param[in] r1   -- Inner radius
// @param[in] r2   -- Outer radius
TCutG* 
dmtpc::image::calculations::makeAnnulusSector(TString name, Float_t x0, Float_t y0, Float_t r1, Float_t r2, TH2* img) {

  // initial point on outer radius
  // assumes here that outer radius hits top of image
  Float_t xmin = img->GetXaxis()->GetBinLowEdge(1);
  Float_t xmax = img->GetXaxis()->GetBinUpEdge(img->GetNbinsX());
  Float_t ymin = img->GetYaxis()->GetBinLowEdge(1);
  Float_t ymax = img->GetYaxis()->GetBinUpEdge(img->GetNbinsY());
  /*
  std::cout << "xmin, max = " << xmin << ", " << xmax << std::endl;
  std::cout << "ymin, max = " << ymin << ", " << ymax << std::endl;
  */

  Float_t phi = TMath::ASin((ymax-y0)/r2);
  /*
  cout << "phi = " << phi << endl;
  */

  Float_t dphi = 1.0*TMath::DegToRad();
  TCutG* reg = new TCutG(name, 0); 

  // start on the outer track
  Int_t phidir = 1; // +1 or -1
  Float_t rr = r2; // start on the outer radius

  int ipt=0;
  while (1) {
    // check which path you're on...

    if (ipt>0) phi += dphi*phidir;

    Float_t dx=rr*TMath::Cos(phi);
    Float_t dy=-rr*TMath::Sin(phi);
    //cout << "phi, dx, dy, x, y = " << phi*TMath::RadToDeg() << ", " << dx << ", " << dy << ", " << x0+dx << ", " << y0+dy << endl;
    Float_t xx = x0+dx;
    Float_t yy = y0+dy;

    // check to see if you exited the image (on either the left or bottom)
    if ((yy < ymin) || (xx < xmin)) {
      rr = r1; // swap to the inner radius
      phidir = -1;  // go back to ccw motion
      // truncate the region to the image boundary
      if (xx<xmin) xx=xmin;
      if (yy<ymin) yy=ymin;
      // deal with the edges
      reg->SetPoint(ipt++, xx, yy);
      reg->SetPoint(ipt++, xx, yy+(r2-r1));
      continue;
    } 
    // check to see if you have reached the top or right of the image
    if ((yy>ymax) || (xx>xmax)) {
      // truncate the region to the image boundary
      if (xx>xmax) xx=xmax;
      if (yy>ymax) yy=ymax;

      reg->SetPoint(ipt++, xx, yy);

      // close the figure
      Double_t xfirst, yfirst;
      reg->GetPoint(0, xfirst, yfirst);
      reg->SetPoint(ipt, xfirst,  yfirst);
      // all done!

      return reg;
    }
    reg->SetPoint(ipt++, xx, yy);
  }

}



void
dmtpc::image::calculations::principalAxes(const TH2* image, float threshold, float &Ix, float &Iy) {

    TMatrixD I(2,2);
    int nx = image->GetNbinsX(); 
    int ny = image->GetNbinsY();
    float meanx=0, meany=0, sum=0;
    for (int i=1; i<=nx; i++) {
        for (int j=1; j<=ny; j++) {
            float Y=image->GetBinContent(i,j);
            if (Y<threshold) continue;
            meanx+=i*Y; meany+=j*Y; sum+=Y; 
        }
    }
    meanx/=sum; meany/=sum;
//    std::cout << "means" << meanx << "," << meany << std::endl;
    for (int i=1; i<=nx; i++) {
        for (int j=1; j<=ny; j++) {
            float Y=image->GetBinContent(i,j);
            if (Y<threshold) continue;
            I(0,0)+=(j-meany)*(j-meany)*Y; I(0,1)+=-(i-meanx)*(j-meany)*Y;
            I(1,0)=I(0,1); I(1,1)+=(i-meanx)*(i-meanx)*Y;
        }
    }
    I *= 1./sum;
    //I.Print();
    
    TMatrixDEigen eig(I);
    TMatrixD dI = eig.GetEigenValues();

    dI.Print();

    Ix=dI(0,0); Iy=dI(1,1);
}


double
dmtpc::image::calculations::cosRecoil2D( const TH2 *image, int imax, int jmax) {
    // cosine of angle between two pixels (2D)
    
    
    int xi = imax%(image->GetNbinsX()+2);
    int yi = imax/(image->GetNbinsX()+2);
    int xj = jmax%(image->GetNbinsX()+2);
    int yj = jmax/(image->GetNbinsX()+2);

    double tgrec = abs(xj-xi)>0 ? double(yj-yi)/double(xj-xi) : 0;
    
    return 1./sqrt(1+tgrec*tgrec);
}

TH1F*
dmtpc::image::calculations::makeProfile(TGraph *graph, int nx, float minx, float maxx, float miny, float maxy, TCanvas *c) {
    
    TF1 ftmp("ftmp","gaus");
    //TF1 ftmp("ftmp","[0]*exp(0.5*((x-[1])/[2])**2)+[3]");
    TH1F *hpr = new TH1F("hpr","",nx, minx, maxx);

    for (int i=0; i<nx; i++) {
        TH1F htmp("htmp","",100, miny, maxy); 
        for (int j=0; j<graph->GetN(); j++) {
            if ( graph->GetX()[j]>hpr->GetXaxis()->GetBinLowEdge(i+1) &&
                 graph->GetX()[j]<hpr->GetXaxis()->GetBinUpEdge(i+1) ) {
                htmp.Fill( graph->GetY()[j] );
//                std::cout << "fill " << graph->GetY()[j] << "  for " <<  graph->GetX()[j] << endl;
            }
        }
        if (htmp.GetEntries()<1) continue;
        ftmp.SetParameters( htmp.GetMaximum(), htmp.GetBinCenter( htmp.GetMaximumBin() ),  htmp.GetRMS(), 0);
        htmp.Fit("ftmp","LL");
        htmp.DrawCopy();
        hpr->SetBinContent(i+1, htmp.GetFunction("ftmp")->GetParameter(1) );
        hpr->SetBinError( i+1,  htmp.GetFunction("ftmp")->GetParError(1) );
        if (c) { c->Update(); getchar(); }
    }
    return hpr;
}

TH1F* 
dmtpc::image::calculations::pixelHist(const TH2* image, int nbins, Float_t minVal, Float_t maxVal, TCutG* cut, bool cutint) {
  /* produce a histogram of pixel values in the image */

  if (minVal !=  9999) minVal = image->GetMinimum();
  if (maxVal != -9999) maxVal = image->GetMaximum();
//  std::cout << "minVal, maxVal = " << minVal << ", " << maxVal << std::endl;

  TAxis *xaxis = image->GetXaxis();
  TAxis *yaxis = image->GetYaxis();

  TH1F *hpix = new TH1F("hpix","",nbins, minVal, maxVal);
  for (Int_t ix=1; ix<=image->GetNbinsX(); ix++) {
    Double_t x = xaxis->GetBinCenter(ix);
    for (Int_t iy=1; iy<=image->GetNbinsY(); iy++) {
      Double_t y = yaxis->GetBinCenter(iy);
      bool isInRegion = (cut !=0) && (cut->IsInside(x,y));
      if ((cut!=0 && !isInRegion &&  cutint) || 
	  (cut!=0 &&  isInRegion && !cutint)) continue;
      hpix->Fill(image->GetBinContent(ix, iy));
    }
  }
  return hpix;
}


double
dmtpc::image::calculations::getMean(const TH2* image, TCutG* cut, bool cutint)
{
   double sum=0;
   int nbinsx = image->GetNbinsX();
   int nbinsy = image->GetNbinsY();

   TAxis *xaxis = image->GetXaxis();
   TAxis *yaxis = image->GetYaxis();
   int npix = 0;
   for(int i=1; i<=nbinsx; i++)
   {
     Double_t x = xaxis->GetBinCenter(i);
      for(int j=1; j<=nbinsy; j++)
      {
	int bin = image->GetBin(i,j);
	Double_t y = yaxis->GetBinCenter(j);
	bool isInRegion = (cut !=0) && (cut->IsInside(x,y));
	if ((cut!=0 && !isInRegion &&  cutint) || 
	    (cut!=0 &&  isInRegion && !cutint)) continue;
	sum+=image->GetBinContent(bin);
	npix++;
      }
   }

   sum = sum/double(npix);
   //sum = sum/double(nbinsx*nbinsy);
   return sum;
}

double
dmtpc::image::calculations::getRMS(const TH2* image, TCutG* cut, bool cutint)
{
  double mean = getMean(image, cut, cutint);
  double sum=0;
  int nbinsx = image->GetNbinsX();
  int nbinsy = image->GetNbinsY();
  
  TAxis *xaxis = image->GetXaxis();
  TAxis *yaxis = image->GetYaxis();
  int npix=0;
  
  for(int i=1; i<=nbinsx; i++)
    {
      Double_t x = xaxis->GetBinCenter(i);
      for(int j=1; j<=nbinsy; j++)
	{
	  int bin = image->GetBin(i,j);
	  Double_t y = yaxis->GetBinCenter(j);
	  bool isInRegion = (cut !=0) && (cut->IsInside(x,y));
	  if ((cut!=0 && !isInRegion &&  cutint) || 
	      (cut!=0 &&  isInRegion && !cutint)) continue;
	  sum+=pow(image->GetBinContent(bin)-mean,2);
	  npix++;
	}
    }
  
  sum=sum/(npix-1);
  sum=sqrt(sum);
  return sum;
}

void
dmtpc::image::calculations::meanRMSNoOutliers(const TH2* image, double& mean, double& rms)
{

  mean = getMean(image);
  rms = getRMS(image);
  bool done = false;
  //cout << "mean, rms = " << mean << ", " << rms << endl;

  int nbinsx=image->GetNbinsX();
  int nbinsy=image->GetNbinsY();
  const int npx = nbinsx*nbinsy;
  //cout << "nbinsx, y = " << nbinsx << ", " << nbinsy << endl;

  Double_t *original = new Double_t[ npx ];
  Double_t *good = new Double_t[ npx ];

  int ngood=0;
  int nbad=0;
  int nbadlast=0;

  for(int i=1; i<=nbinsx; i++)
    {
      for(int j=1; j<=nbinsy; j++)
	{
	  original[(i-1)+nbinsx*(j-1)] = image->GetBinContent(i,j);
	}
    }

  while(!done)
    {
      nbadlast = nbad;
      ngood=0;
      nbad=0;
      
      for(int i=0; i<npx; i++)
	{
	   if(original[i] < mean+5*rms && original[i] > mean-5*rms)
	    {
	      good[ngood]=original[i];
	      ngood++;
	    }
	  else
	    nbad++;
	}
      mean = TMath::Mean(ngood,good);
      rms = TMath::RMS(ngood,good);
      
      //cout << "ngood = " << ngood << " nbad = " << nbad <<
      //	" mean = " << mean << " rms = " << rms << endl;

      if(nbad-nbadlast==0)
	done=true;
    }

  delete original;
  delete good;
}



int 
dmtpc::image::calculations::distanceToImageEdge(const TH2 *image, int ibin) {
  // Find pixel distance to image edge. 
  //

  int xmax=image->GetNbinsX();
  int ymax=image->GetNbinsY();

  int xi = ibin%(image->GetNbinsX()+2);
  int yi = ibin/(image->GetNbinsX()+2);

  if (xi>(xmax-xi)) xi=xmax-xi;
  if (yi>(ymax-yi)) yi=ymax-yi;
  
  return xi>yi ? yi : xi;
}

double dmtpc::image::calculations::rectIntersectionArea(double * xcoords1, double * ycoords1, double * xcoords2, double * ycoords2) 
{
  std::vector<double> xs; 
  std::vector<double> ys; 
  
  //Loop through all edges in 1 
  for (int i1 = 0; i1 < 4; i1++)
  {
    double x1 = xcoords1[i1]; 
    double x2 = xcoords1[(i1+1) % 4]; 
    double y1 = ycoords1[i1]; 
    double y2 = ycoords1[(i1+1) % 4]; 

    if (pnpoly(4,xcoords2,ycoords2,x1,y1)) 
    {
      xs.push_back(x1); 
      ys.push_back(y1); 
    }

    //Loop through all edges in 2 
    for (int i2 = 0; i2 <4; i2++)
    {

      double x3 = xcoords2[i2]; 
      double x4 = xcoords2[(i2+1) % 4]; 
      double y3 = ycoords2[i2]; 
      double y4 = ycoords2[(i2+1) % 4]; 

      if (i1 == 0 && pnpoly(4, xcoords1, ycoords1,x3,y3))  
      {
        xs.push_back(x3); 
        ys.push_back(y3); 
      }

      double x = ((x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4)) / ((x1-x2)*(y3-y4)  - (y1-y2)*(x3-x4)); 
      double y = ((x1*y2-y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4)) / ((x1-x2)*(y3-y4)  - (y1-y2)*(x3-x4)); 
         
      if (!TMath::Finite(x)  || !TMath::Finite(y) || isnan(x) || isnan(y)) continue; 


      if ( (x1 < x2) && (x < x1 || x > x2)) continue; 
      else if ((x1 > x2) &&  (x > x1 || x < x2)) continue; 

      if ( (x3 < x4) && (x < x3 || x>x4)) continue; 
      else if ((x3 > x4) && (x > x3 || x < x4)) continue; 

      if ( (y1 < y2) && (y < y1 || y>y2)) continue; 
      else if  ((y1 > y2) && (y > y1 || y < y2)) continue; 

      if ( (y3 < y4) && (y < y3 || y>y4)) continue; 
      else if ((y3 > y4) && (y > y3 || y < y4 )) continue; 

      xs.push_back(x);
      ys.push_back(y);
    }
  }

  if (xs.size() < 3) return 0; 
  
  double ret =  polygonArea(xs.size(), &(xs[0]), &(ys[0]), false); 

//  if ( fabs(ret - 1.) > 0.01)
//  {
//    printf("vertices: "); 
//    for (unsigned i =0; i < xs.size(); i++)
//    {
//      printf(" %f,%f ",xs[i],ys[i]); 
//    }
//    printf("\n"); 
 // }

  return ret; 
}


double dmtpc::image::calculations::median(const TH2 * img)
{
  int n = img->GetNbinsX() * img->GetNbinsY(); 
  std::vector<double> pix(n); 
  int i = 0; 
  for (int x = 1; x <= img->GetNbinsX(); x++)
  {
    for (int y = 1; y <= img->GetNbinsY(); y++)
    {
        pix[i++] = img->GetBinContent(x,y); 
    }
  }

  std::nth_element(pix.begin(), pix.begin() + n/2, pix.end()); 

  return pix[n/2]; 
}

double dmtpc::image::calculations::polygonArea(int n, const double * x, const double * y, bool ordered)  
{

  int * indices = (int *) alloca(sizeof(*indices) * n);  
  if (!ordered)
  {
    double * thetas = (double *) alloca(sizeof(*thetas) * n); 
    float centerx = 0; 
    float centery = 0; 

    for (int i = 0; i < n;  i++)
    {
      centerx += x[i]; 
      centery += y[i]; 
    }

    centerx/=n; 
    centery/=n; 

    for (int i = 0; i < n;  i++)
    {
      thetas[i] = atan2(y[i] - centery, x[i] - centerx); 
    }
    TMath::Sort(n,thetas,indices); 
  }

  double area = 0; 

  for (int j = 0; j < n; j++)
  {
    int i = ordered ? j : indices[j]; 
    int ii = ordered ? (j+1) % n: indices[(j+1) % n]; 
    area += x[i]*y[ii] - x[ii]*y[i]; 
  }

  area = fabs(area)/2.; 
  return area; 
}


int dmtpc::image::calculations::countUniqueValues(const TH1* in)
{

  std::set<double> unique; 

  for (int i = 1; i <= in->GetNbinsX(); i++)
  {
    for (int j = 1; j <= in->GetNbinsY(); j++)
    {
      for (int k = 1; k <= in->GetNbinsZ(); k++)
      {
        unique.insert(in->GetBinContent(i,j,k)); 
      }
    }
  }

  return unique.size(); 

}






