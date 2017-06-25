#include "Mollweide.hh"
#include "TChain.h"
#include "TTree.h"
#include "TH2.h"
#include "TH2Poly.h"
#include "TF1.h"
#include "TLine.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
using namespace std;
using namespace dmtpc::mc::wimpspectrum; 

double PI = TMath::Pi();

Mollweide::Mollweide()
{
 fName = "MollweideProj";
 fBinning = 25;
 fNpts = 10;
 fScale = 90;
 fTol = 1e-5;
 fNorm = true;
 fHist = 0;
 fPoly = 0;
 fMulti = 0;
// fPrimeMeridian = 0;
}

Mollweide::~Mollweide()
{
/*
  delete fHist;
  delete fPoly;
  delete fMulti;
*/
//  for (int i = 0; i < (int) fFunVec.size();i++)
//    delete fFunVec[i];
}

void Mollweide::projection(const char* file, const char* tree, const char* theta, const char* phi,const char* opt)
{
  TChain ch(tree);
  ch.AddFile(file);
  projection(&ch,theta,phi,opt);
}

void Mollweide::projection(TTree* tree, const char* thetastr, const char* phistr, const char* opt)
{
  if (!fHist)
    fHist = new TH2F(TString(fName.c_str())+"hist","",2*fBinning,-2*fScale,2*fScale,50,-fScale,fScale);

  float phi,theta;
  tree->SetBranchAddress(phistr,&phi);
  tree->SetBranchAddress(thetastr,&theta);
  long evts = tree->Draw("Entry$",opt,"goff");
  if (evts>tree->GetEstimate()) {
    tree->SetEstimate(evts+1);
    evts = tree->Draw("Entry$",opt,"goff");
  }
  double x=0,y=0;
  for (int i = 0 ; i < evts; i++)
  {
    long entry = (long) tree->GetV1()[i];    
    tree->GetEntry( entry );
    
    coords(theta,phi,x,y);
    fHist->Fill(x,y);
  }
  makePoly(); 
}

void 
Mollweide::coords(double theta, double phi,
     double& x, double& y)
{
  double lon = atan2(sin(phi)*sin(theta),cos(phi)*sin(theta));
  double lat = atan2(cos(theta),hypot(cos(phi)*sin(theta),sin(phi)*sin(theta)) );
  double psi = lat;
  if (fabs(psi - PI/2.)>fTol ) 
  {
    double old_psi;
    do{
      old_psi = psi;
      psi = old_psi - (2*old_psi+sin(2*old_psi)-PI*sin(lat))/(2+2*cos(2*old_psi));
    }while(fabs(psi-old_psi)>fTol);
  }

  x = 2*fScale * lon/PI * cos(psi);
  y = fScale*sin(psi);
}

void 
Mollweide::makePoly()
{
  int N = fNpts;
  if (fPoly) delete fPoly;
  fPoly=new TH2Poly(fName.c_str(),fName.c_str(),-2*fScale,2*fScale,-fScale,fScale);
  
  for (int i = 1; i <= fHist->GetNbinsX(); i++)
  {
    for (int j = 1; j <= fHist->GetNbinsY(); j++)
    {
      //Find if corners are inside 
      double xlow = fHist->GetXaxis()->GetBinLowEdge(i)/fScale;
      double xhigh = fHist->GetXaxis()->GetBinUpEdge(i)/fScale;
      double ylow = fHist->GetYaxis()->GetBinLowEdge(j)/fScale;
      double yhigh = fHist->GetYaxis()->GetBinUpEdge(j)/fScale;
      double size = 1;
      if (fNorm) {
        size = binsize(xlow,xhigh,ylow,yhigh);
        size /= (xhigh-xlow)*(yhigh-ylow);
      }
      //cout <<"Size: "<<size<<endl;
      bool inTL = xlow*xlow/4 + yhigh*yhigh < 1;
      bool inTR = xhigh*xhigh/4 + yhigh*yhigh < 1;
      bool inBL = xlow*xlow/4 + ylow*ylow < 1;
      bool inBR = xhigh*xhigh/4 + ylow*ylow < 1;

      if (!inTL && !inTR && !inBL&&!inBR) continue;
      else if (inTL&&inTR&&inBL&&inBR){


        double x[] = {fScale*xlow,fScale*xlow,fScale*xhigh,fScale*xhigh};
        double y[] = {fScale*ylow,fScale*yhigh,fScale*yhigh,fScale*ylow};
        int bin = fPoly->AddBin(4,x,y);
        fPoly->SetBinContent(bin,fHist->GetBinContent(i,j));
      }else if (inTL && !inTR && !inBL &&!inBR){
        vector<double> x;
        vector<double> y;
        x.push_back(xlow*fScale);
        y.push_back(yhigh*fScale);
        double x1 = xlow;
        double x2 = 2 * sqrt(1-yhigh*yhigh);
        for (int k = 0; k <=N; k++)
        {
          double x0 = x1 + k/N * (x2-x1);
          //Find point at same x as corner:
          double y0 = -sqrt(1.-x0*x0/4);
          x.push_back(x0*fScale); y.push_back(y0*fScale);
        }
        int bin = fPoly->AddBin(x.size(),&x[0],&y[0]);
        fPoly->SetBinContent(bin,fHist->GetBinContent(i,j)/size);
      }else if (inTL && inTR && !inBL &&!inBR){
        vector<double> x;
        vector<double> y;
        x.push_back(fScale*xlow);
        y.push_back(fScale*yhigh);
        double x1 = xlow;
        double x2 = xhigh;
        for (int k = 0; k <=N; k++)
        {
          double x0 = x1 + k/N * (x2-x1);
          //Find point at same x as corner:
          double y0 = -sqrt(1.-x0*x0/4);
          x.push_back(x0*fScale); y.push_back(y0*fScale);
        }
        x.push_back(xhigh*fScale); y.push_back(yhigh*fScale);
        int bin = fPoly->AddBin(x.size(),&x[0],&y[0]);
        fPoly->SetBinContent(bin,fHist->GetBinContent(i,j)/size);
      }else if (!inTL && inTR && !inBL &&!inBR){
        vector<double> x;
        vector<double> y;
        x.push_back(fScale*xhigh);
        y.push_back(fScale*yhigh);
        double x1 = xhigh;
        double x2 = -2 * sqrt(1-yhigh*yhigh);
        for (int k = 0; k <=N; k++)
        {
          double x0 = x1 + k/N * (x2-x1);
          //Find point at same x as corner:
          double y0 = -sqrt(1.-x0*x0/4);
          x.push_back(fScale*x0); y.push_back(fScale*y0);
        }
        int bin = fPoly->AddBin(x.size(),&x[0],&y[0]);
        fPoly->SetBinContent(bin,fHist->GetBinContent(i,j)/size);
      }else if (!inTL && !inTR && inBL &&!inBR){
        vector<double> x;
        vector<double> y;
        x.push_back(fScale*xlow);
        y.push_back(fScale*ylow);
        double x1 = xlow;
        double x2 = 2 * sqrt(1-ylow*ylow);
        for (int k = 0; k <=N; k++)
        {
          double x0 = x1 + k/N * (x2-x1);
          //Find point at same x as corner:
          double y0 = sqrt(1.-x0*x0/4);
          x.push_back(fScale*x0); y.push_back(fScale*y0);
        }
        int bin = fPoly->AddBin(x.size(),&x[0],&y[0]);
        fPoly->SetBinContent(bin,fHist->GetBinContent(i,j)/size);
      }else if (!inTL && !inTR && !inBL &&inBR){
        vector<double> x;
        vector<double> y;
        x.push_back(fScale*xhigh);
        y.push_back(fScale*ylow);
        double x1 = xhigh;
        double x2 = -2 * sqrt(1-ylow*ylow);
        for (int k = 0; k <=N; k++)
        {
          double x0 = x1 + k/N * (x2-x1);
          //Find point at same x as corner:
          double y0 = sqrt(1.-x0*x0/4);
          x.push_back(fScale*x0); y.push_back(fScale*y0);
        }
        int bin = fPoly->AddBin(x.size(),&x[0],&y[0]);
        fPoly->SetBinContent(bin,fHist->GetBinContent(i,j)/size);
      }else if (!inTL && !inTR && inBL &&inBR){
        vector<double> x;
        vector<double> y;
        x.push_back(fScale*xlow);
        y.push_back(fScale*ylow);
        double x1 = xlow;
        double x2 = xhigh;
        for (int k = 0; k <=N; k++)
        {
          double x0 = x1 + k/N * (x2-x1);
          //Find point at same x as corner:
          double y0 = sqrt(1.-x0*x0/4);
          x.push_back(fScale*x0); y.push_back(fScale*y0);
        }
        x.push_back(fScale*xhigh); y.push_back(fScale*ylow);
        int bin = fPoly->AddBin(x.size(),&x[0],&y[0]);
        fPoly->SetBinContent(bin,fHist->GetBinContent(i,j)/size);
      }else if (!inTL && inTR && !inBL &&inBR){
        vector<double> x;
        vector<double> y;
        x.push_back(fScale*xhigh);
        y.push_back(fScale*ylow);
        double y1 = ylow;
        double y2 = yhigh;
        for (int k = 0; k <=N; k++)
        {
          double y0 = y1 + k/N * (y2-y1);
          //Find point at same x as corner:
          double x0 = -2*sqrt(1.-y0*y0);
          x.push_back(fScale*x0); y.push_back(fScale*y0);
        }
        x.push_back(fScale*xhigh); y.push_back(fScale*yhigh);
        int bin = fPoly->AddBin(x.size(),&x[0],&y[0]);
        fPoly->SetBinContent(bin,fHist->GetBinContent(i,j)/size);
      }else if (inTL && !inTR && inBL &&!inBR){
        vector<double> x;
        vector<double> y;
        x.push_back(fScale*xlow);
        y.push_back(fScale*ylow);
        double y1 = ylow;
        double y2 = yhigh;
        for (int k = 0; k <=N; k++)
        {
          double y0 = y1 + k/N * (y2-y1);
          //Find point at same x as corner:
          double x0 = 2*sqrt(1.-y0*y0);
          x.push_back(fScale*x0); y.push_back(fScale*y0);
        }
        x.push_back(fScale*xlow); y.push_back(fScale*yhigh);
        int bin = fPoly->AddBin(x.size(),&x[0],&y[0]);
        fPoly->SetBinContent(bin,fHist->GetBinContent(i,j)/size);
      }else if (inTL && !inTR && inBL &&inBR){
        vector<double> x;
        vector<double> y;
        x.push_back(fScale*xlow);
        y.push_back(fScale*yhigh);
        x.push_back(fScale*xlow);
        y.push_back(fScale*ylow);
        x.push_back(fScale*xhigh);
        y.push_back(fScale*ylow);
      
        double x1 = xhigh;
        double x2 = 2*sqrt(1-yhigh*yhigh);
        for (int k = 0; k <=N; k++)
        {
          double x0 = x1 + k/N * (x2-x1);
          //Find point at same x as corner:
          double y0 = sqrt(1.-x0*x0/4.);
          x.push_back(fScale*x0); y.push_back(fScale*y0);
        }
        int bin = fPoly->AddBin(x.size(),&x[0],&y[0]);
        fPoly->SetBinContent(bin,fHist->GetBinContent(i,j)/size);
      }else if (!inTL && inTR && inBL &&inBR){
        vector<double> x;
        vector<double> y;
        x.push_back(fScale*xhigh);
        y.push_back(fScale*yhigh);
        x.push_back(fScale*xhigh);
        y.push_back(fScale*ylow);
        x.push_back(fScale*xlow);
        y.push_back(fScale*ylow);
      
        double x1 = xlow;
        double x2 = -2*sqrt(1-yhigh*yhigh);
        for (int k = 0; k <=N; k++)
        {
          double x0 = x1 + k/N * (x2-x1);
          //Find point at same x as corner:
          double y0 = sqrt(1.-x0*x0/4.);
          x.push_back(fScale*x0); y.push_back(fScale*y0);
        }
        int bin = fPoly->AddBin(x.size(),&x[0],&y[0]);
        fPoly->SetBinContent(bin,fHist->GetBinContent(i,j)/size);
      }else if (inTL && inTR && !inBL &&inBR){
        vector<double> x;
        vector<double> y;
        x.push_back(fScale*xhigh);
        y.push_back(fScale*ylow);
        x.push_back(fScale*xhigh);
        y.push_back(fScale*yhigh);
        x.push_back(fScale*xlow);
        y.push_back(fScale*yhigh);
      
        double x1 = xlow;
        double x2 = -2*sqrt(1-ylow*ylow);
        for (int k = 0; k <=N; k++)
        {
          double x0 = x1 + k/N * (x2-x1);
          //Find point at same x as corner:
          double y0 = -sqrt(1.-x0*x0/4.);
          x.push_back(fScale*x0); y.push_back(fScale*y0);
        }
        int bin = fPoly->AddBin(x.size(),&x[0],&y[0]);
        fPoly->SetBinContent(bin,fHist->GetBinContent(i,j)/size);
      }else if (inTL && inTR && inBL &&!inBR){
        vector<double> x;
        vector<double> y;
        x.push_back(fScale*xlow);
        y.push_back(fScale*ylow);
        x.push_back(fScale*xlow);
        y.push_back(fScale*yhigh);
        x.push_back(fScale*xhigh);
        y.push_back(fScale*yhigh);
      
        double x1 = xhigh;
        double x2 = 2*sqrt(1-ylow*ylow);
        for (int k = 0; k <=N; k++)
        {
          double x0 = x1 + k/N * (x2-x1);
          //Find point at same x as corner:
          double y0 = -sqrt(1.-x0*x0/4.);
          x.push_back(fScale*x0); y.push_back(fScale*y0);
        }
        int bin = fPoly->AddBin(x.size(),&x[0],&y[0]);
        fPoly->SetBinContent(bin,fHist->GetBinContent(i,j)/size);
      }
      if (fNorm) fHist->SetBinContent(i,j,fHist->GetBinContent(i,j)/size);
    }//y bins
  }//x bins

}

double 
Mollweide::binsize(double xmin,double xmax,double ymin, double ymax)
{
  int inTL = xmin*xmin/4 + ymax*ymax < 1;
  int inTR = xmax*xmax/4 + ymax*ymax < 1;
  int inBL = xmin*xmin/4 + ymin*ymin < 1;
  int inBR = xmax*xmax/4 + ymin*ymin < 1;
  int value = 1;
  value = value << inTL;value= value<<inTR; value =value <<inBL;value= value<<inBR;
  if (value==1) return 0;
  else if (value==16) return (xmax-xmin)*(ymax-ymin);
  else if (value==2)//one corner
  {
    double xm = TMath::Min(fabs(xmin),fabs(xmax)); 
    double xM = TMath::Max(fabs(xmin),fabs(xmax)); 
    double ym = TMath::Min(fabs(ymin),fabs(ymax)); 
    //double yM = TMath::Max(fabs(ymin),fabs(ymax));

    TF1 f("tempfun","sqrt(1-0.25*x^2)-[0]",xm,xM);
    f.SetParameter(0,ym);
    return f.Integral(xm,2*sqrt(1-ym*ym));
  }else if (value==4)//two corners
  {
    double xm = TMath::Min(fabs(xmin),fabs(xmax)); 
//    double xM = TMath::Max(fabs(xmin),fabs(xmax)); 
    double ym = TMath::Min(fabs(ymin),fabs(ymax)); 
//    double yM = TMath::Max(fabs(ymin),fabs(ymax));
    if ((inTL&&inTR)||(inBL&&inBR) ){
      TF1 f("tempfun","sqrt(1-0.25*x^2)-[0]");
      f.SetParameter(0,ym);
      return f.Integral(xmin,xmax);    
    }else if ( (inTL&&inBL) || (inTR&&inBR) ){
      TF1 f("tempfun","2*sqrt(1-x^2)-[0]");
      f.SetParameter(0,xm);
      return f.Integral(ymin,ymax);
    }else return 0;

  }else if (value==8)//three corners
  {
    double xm = TMath::Min(fabs(xmin),fabs(xmax)); 
    double xM = TMath::Max(fabs(xmin),fabs(xmax)); 
    double ym = TMath::Min(fabs(ymin),fabs(ymax)); 
    double yM = TMath::Max(fabs(ymin),fabs(ymax));
    double x1 = 2*sqrt(1-yM*yM);
    TF1 f("tempfun","sqrt(1-0.25*x^2)-[0]",xm,xM);
    f.SetParameter(0,ym);
    return (yM-ym)*(x1-xm) + f.Integral(x1,xM);
  }
  return -1;//error
}

void Mollweide::clear()
{ if (fHist) fHist->Reset();}

void
Mollweide::Draw(int nlong, int nlat)
{
  fPoly->Draw("colz");
//  gPad->SetLeftMargin(0.07);
//  gPad->SetRightMargin(0.07);


  if (fMulti) delete fMulti;
  fMulti = new TMultiGraph;
  for (int i = 0 ; i <2*nlat-1; i++)
  {
    double lat = (i - nlat +1)*PI / (2*nlat);
    double phi=PI, theta=0.5*PI-lat,x=0,y=0;
    coords(theta,phi,x,y);
    double xx[] = {-x,x};
    double yy[] = {y,y};

    TString name = fName.c_str();
    name +="lat";
    name+=i;
    TGraph* gr = new TGraph(2,xx,yy);
    gr->SetName(name);
    gr->SetLineColor(kBlack);
    gr->SetLineWidth(1);
//    cout <<"Drawing lat "<<i<<endl;
//    gr->Draw("l");
    fMulti->Add(gr);
  }
  for (int i = 0; i<nlong;i++)
  {
    double lon = 1./nlong*i-1;
    
//    double invlon = 1./lon;
//    double lim = 2*fScale * fabs(lon);

    int n = 200;
    double y[n+1],x1[n+1],x2[n+1];
    for (int j = 0;j<=n; j++)
    {
      y[j]= -fScale + 2*fScale/n * j;
      x1[j]=2*lon*sqrt(fScale*fScale-y[j]*y[j]);
      x2[j] = -x1[j];
    }
    TGraph* gr1 = new TGraph(n+1,x1,y);
    TGraph* gr2 = new TGraph(n+1,x2,y);
    gr1->SetName( TString(fName.c_str())+"lonN"+TString(i));
    gr2->SetName( TString(fName.c_str())+"lonP"+TString(i));
    gr1->SetLineColor(kBlack);
    gr2->SetLineColor(kBlack);
    gr1->SetLineWidth(1);
    gr2->SetLineWidth(1);
//    cout <<"Drawing long "<<i<<endl;
//    gr1->Draw("l"); gr2->Draw("l");
    fMulti->Add(gr1);
    fMulti->Add(gr2);

  }
  double x[] = {0,0}; double y[] = {-fScale,fScale};
  TGraph* gr = new TGraph( 2,x,y);
  gr->SetLineColor(kBlack);
  gr->SetLineWidth(1);
  gr->SetName("PrimeMeridian");
  fMulti->Add(gr);
  fMulti->Draw("l");
  gPad->GetCanvas()->SetLeftMargin(0.07);
  gPad->GetCanvas()->SetRightMargin(0.07);
  gPad->GetCanvas()->SetCanvasSize(800,400);
/*
  for (int i = 0 ;i <(int) fFunVec.size(); i++)
    delete fFunVec[i];
  fFunVec.clear();

  for (int i = 0 ; i <2*nlat-1; i++)
  {
    double lat = (i - nlat +1)*PI / (2*nlat);
    cout <<"Latitude: "<<lat*180/PI<<endl;

    double phi=PI, theta=0.5*PI-lat,x=0,y=0;
    cout <<"Theta: "<<theta<<" Phi: "<<phi<<endl;
    coords(theta,phi,x,y);
    cout <<"X: "<<x<<" Y: "<<y<<endl;
    TString name = fName.c_str();
    name +="lat";
    name+=i;
    TF1* f = new TF1( name,"[0]",-x,x);
    f->SetParameter(0,y);
    fFunVec.push_back(f);
    f->Draw("same");
  }

  for (int i = 0; i<nlong;i++)
  {
    double lon = 1./nlong*i-1;
    
    double invlon = 1./lon;
    double lim = 2*fScale * fabs(lon);
    TF1* f0 = new TF1(TString(fName.c_str())+"lonP"+TString(i),"sqrt([0]^2-0.25*x^2*[1]^2)",-lim,lim);
    TF1* f1 = new TF1(TString(fName.c_str())+"lonP"+TString(i),"-sqrt([0]^2-0.25*x^2*[1]^2)",-lim,lim);
    f0->SetParameters(fScale,invlon); 
    f1->SetParameters(fScale,invlon);
    f0->SetNpx(10000);
    f1->SetNpx(10000);
    fFunVec.push_back(f0);
    fFunVec.push_back(f1);
    f0->Draw("same");
    f1->Draw("same");
  }
  if (fPrimeMeridian) delete fPrimeMeridian;
  fPrimeMeridian = new TLine(0,-fScale,0,fScale);
  fPrimeMeridian->Draw();
*/
}
