#include "GainMap.hh"
#include "ImageProcessing.hh"
#include "TRandom3.h" 

#include "TH2F.h"
#include "TH1F.h"
#include "TLine.h"

#include <iostream>
#include <math.h>
#include <stdio.h>

using namespace std;



ClassImp(dmtpc::analysis::GainMap);

dmtpc::analysis::GainMap::GainMap()
{
  _error_estimate = 0; 
}

dmtpc::analysis::GainMap::GainMap(TString name)
{

  _error_estimate = 0; 
   SetName(name);
   _spacers = new TObjArray();
   _spacers->SetOwner(kTRUE);
   _nSpacers=0;
//   _gainMap = new TH2F("gainMap","gainMap",10,0,1,10,0,1);
   _gainMap = 0;

}

dmtpc::analysis::GainMap::~GainMap()
{
   _spacers->Delete(); 
   delete _spacers;
   if (_gainMap) delete _gainMap; 
   if (_error_estimate) delete _error_estimate; 
}

void dmtpc::analysis::GainMap::drawWithSpacers()
{
   _gainMap->Draw("colz");
   double ylow = _gainMap->GetYaxis()->GetXmin();
   double yhigh = _gainMap->GetYaxis()->GetXmax();
   for(int i=0; i<_nSpacers; i++)
   {
      TLine* l = new TLine((ylow-getSpacerIntercept(i))/getSpacerSlope(i),ylow,
			   (yhigh-getSpacerIntercept(i))/getSpacerSlope(i),yhigh);
      l->SetLineWidth(2);
      l->SetLineStyle(2);
      l->Draw("SAME");
   }
}

void dmtpc::analysis::GainMap::addSpacer(double slope,double intercept, double width)
{
   TVector3* params = new TVector3(slope,intercept,width);
   _spacers->Add(params);
   _nSpacers++;

}

void dmtpc::analysis::GainMap::addSpacer(TVector3* params)
{
   _spacers->Add(params);
   _nSpacers++;
}

int dmtpc::analysis::GainMap::nCrossingSpacers(double x1, double y1, double x2, double y2, vector<int>* which) const
{
   int ret = 0; 
   for(int i=0; i<_nSpacers; i++)
   {
      double m=getSpacerSlope(i);
      double b=getSpacerIntercept(i);
      bool w = (y1<m*x1+b) ? true : false;
      bool z = (y2<m*x2+b) ? true : false;

      if( (w||z) && !(w && z) )
      {
	 ret++; 
         if (which!=NULL)
         {
            which->push_back(i);
         }
      }
   }
   return ret;
}


bool dmtpc::analysis::GainMap::crossesSpacer(double x1, double y1, double x2, double y2) const
{
   for(int i=0; i<_nSpacers; i++)
   {
      double m=getSpacerSlope(i);
      double b=getSpacerIntercept(i);
      bool w = (y1<m*x1+b) ? true : false;
      bool z = (y2<m*x2+b) ? true : false;

      if( (w||z) && !(w && z) )
      {
	 return true;
      }

   }
   
   return false;
}
double dmtpc::analysis::GainMap::distanceToSpacer(int i, double x, double y) const
{
   double m = getSpacerSlope(i);
   double b = getSpacerIntercept(i);
   return fabs(y-m*x-b)/sqrt(m*m+1);

}

void dmtpc::analysis::GainMap::writeOverlay(const char * outfile) const
{

  FILE * f = fopen(outfile,"w"); 

  for (int i = 0; i < getNSpacers(); i++)
  {
    fprintf(f,"#Spacer  %d\n",i); 
    
    double b = getSpacerIntercept(i); 
    double m = getSpacerSlope(i); 
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

  fclose(f); 

}

double dmtpc::analysis::GainMap::distanceToNearestSpacer(double x, double y, int& imin) const
{
   double mindist=100000;
   for(int i=0; i<_nSpacers; i++)
   {
      double dist = distanceToSpacer(i,x,y);
      if(dist<mindist) {mindist=dist; imin=i;}
      
   }
   return mindist;
}
/*
void dmtpc::analysis::GainMap::calculatePolynomialSpacers(double width,int npoints,  int polyorder, double chisq_cut)
{

  double xmin = _gainMap->GetXaxis()->GetXmin(); 
  double xmax = _gainMap->GetXaxis()->GetXmin(); 
  double ymin = _gainMap->GetYaxis()->GetXmin(); 
  double ymax = _gainMap->GetYaxis()->GetXmin(); 




  for (int i = 0; i < _nSpacers; i++)  
  {
    double x0,y0,x1,y1; 
    double theta = atan(-1./getSpacerSlope(i)); 
    double r = b * sin(theta); 
    
    if (!lineRectIntersectionPoints(theta,r,xmin,xmax,ymin,ymax,&x0,&y0,&x1,&y1)) continue; 
    vector<double> xs; 
    vector<double> ys; 
    vector<double> ss; 

    double dx = (x0 + x1) / (npoints+1) / 2. 
    double dy = (y0 + y1) / (npoints+1) / 2. 

    for (int j = 0; j < npoints; j++)
    {
      double x = (x0 + x1) / (npoints+1) * (j+1); 
      double y = (y0 + y1) / (npoints+1) * (j+1); 

      TH1 * tranv; 
      TH1 * longi; 

      MaxCamImageTools::projectAlongLine(_gainMap,&longi, &tranv, x - dx, y - dy, x + dx, y + dy, width); 

      TF1 f("f", "gaus", tranv->GetXaxis()->GetXmin(), tranv->GetXaxis()->GetXmax()); 
      tranv->Fit(&f); 

      if (f.GetChisquare() / f.GetNDF() < chisq_cut)
      {
        

      }
    }





  }
}
*/

const static double avg_kernel[9] = { 1./9,1./9,1./9,
                                     1./9,1./9,1./9, 
                                     1./9,1./9,1./9 }; 



TH2* dmtpc::analysis::GainMap::getErrorEstimate() const 
{
  if (_error_estimate) return _error_estimate; 

  TH2 * blurred = dmtpc::image::processing::convolve(_gainMap, avg_kernel,3,3); 


  _error_estimate = (TH2F*) _gainMap->Clone(TString::Format("%s_err",_gainMap->GetName())); 


  for (int i =1; i <= _error_estimate->GetNbinsX(); i++)
  {
    for (int j =1; j <= _error_estimate->GetNbinsY(); j++)
    {
      _error_estimate->SetBinContent(i,j, fabs(_error_estimate->GetBinContent(i,j) - blurred->GetBinContent(i,j))); 
    }
  }


  delete blurred; 

  return _error_estimate; 
}


void dmtpc::analysis::GainMap::randomize()
{
  TRandom3 rand(0); 

  for (int i =1; i <= _gainMap->GetNbinsX(); i++)
  {
    for (int j =1; j <= _gainMap->GetNbinsY(); j++)
    {
      _gainMap->SetBinContent(i,j, rand.Gaus(_gainMap->GetBinContent(i,j), getErrorEstimate()->GetBinContent(i,j))); 
    }
  }
}


vector < double > dmtpc::analysis::GainMap::distanceToSpacers(double x, double y) const
{
   vector < double > dists;
   for(int i=0; i<_nSpacers; i++)
   {
      double dist = distanceToSpacer(i,x,y);
      dists.push_back(dist);
   }

   return dists;
}

