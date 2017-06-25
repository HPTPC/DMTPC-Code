#include "WaveformProcessing.hh"
#include "FirFilter.hh"
#include "IirFilter.hh"

#include "TClass.h"
#include "TH1.h"
#include "TH2.h"
#include "TObjArray.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TMath.h"

#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <complex> 
#include <stdlib.h>
using namespace std;
#define PI 3.1415926535897932384626433832795029L

#define get(h,i) h->GetBinContent(i) 
#define set(h,i,x) h->SetBinContent(i,x) 

TH1* newTH1StealType(const TH1* h,const char* name, const char* title,int nbins, double xmin, double xmax,void* ptr=0)
{
  TH1* newhist=0;
  TString clname = h->ClassName();
  if (clname=="TH1D")
    newhist = ptr ? new(ptr) TH1D(name,title,nbins,xmax,xmin) : new TH1D(name,title,nbins,xmax,xmin);
  else if (clname=="TH1F")
    newhist = ptr ? new(ptr) TH1F(name,title,nbins,xmax,xmin) : new TH1F(name,title,nbins,xmax,xmin);
  else if (clname=="TH1I")
    newhist = ptr ? new(ptr) TH1I(name,title,nbins,xmax,xmin) : new TH1I(name,title,nbins,xmax,xmin);
  else if (clname=="TH1S")
    newhist = ptr ? new(ptr) TH1S(name,title,nbins,xmax,xmin) : new TH1S(name,title,nbins,xmax,xmin);
  else if (clname=="TH1C")
    newhist = ptr ? new(ptr) TH1C(name,title,nbins,xmax,xmin) : new TH1C(name,title,nbins,xmax,xmin);
  else
    cout <<"newTH1StealType: Unsupported histogram type!"<<endl;
  return newhist;
}

TH1* 
dmtpc::waveform::medianFilter(const TH1* hist, int size, const char* name)
{

  //Create the filtered histogram
  int nbins = hist->GetNbinsX();
  double xmax = hist->GetXaxis()->GetXmax();
  double xmin = hist->GetXaxis()->GetXmin();
  double xwidth = hist->GetXaxis()->GetBinWidth(1);
  TH1* filter = newTH1StealType(hist,name,"Median Filter",nbins-2*(size+1),xmin+(size+1)*xwidth,xmax-(size+1)*xwidth);

  vector<double> v(2*size+1);

  for (int i = 1; i<=2*size+1; i++){
    v[i-1] = hist->GetBinContent(i);;
  }

 
  sort(v.begin(),v.end());
  double value;
  vector<double>::iterator iter;
  for (int i = size+2; i<= nbins-size-1; i++)
  {

    filter->SetBinContent(i-size,v[size]);

    //Find and erase first element

    iter = lower_bound(v.begin(),v.end(),hist->GetBinContent(i-size));

    v.erase(iter);
    //Add new element in its place
    if (i==nbins-size)break;
 
    value = hist->GetBinContent(i+size+1);

    iter = lower_bound(v.begin(),v.end(),value);

    if (iter-v.end()==0) v.push_back(value);
    else v.insert(iter,value);

  }//loop over all non-edge bins 

  return filter;

}

/*
======================================================
Other useful functions and transforms
(Integrals, derivatives, etc)
Plan to add FFTW3 support for FFT, reverse FFT and autocorrelation
====================================================
*/
TH1* 
dmtpc::waveform::integral(const TH1* hist, bool useXwidth, const char* name)
  {


    int nbins = hist->GetNbinsX();
    double xmax = hist->GetXaxis()->GetXmax();
    double xmin = hist->GetXaxis()->GetXmin();
    TH1* integ;
    if (hist->ClassName()=="TH1D")
      integ = new TH1D(name,"Integral",nbins, xmin, xmax);
    else 
      integ = new TH1F(name,"Integral",nbins, xmin, xmax);
 
    if (useXwidth){
      double xwidth = hist->GetXaxis()->GetBinWidth(1);

      integ->SetBinContent(1,0.5*hist->GetBinContent(1)*xwidth);
      for (int i = 2; i<=nbins; i++)
        integ->SetBinContent(i, integ->GetBinContent(i-1)+0.5 * xwidth *  
           (hist->GetBinContent(i-1)+hist->GetBinContent(i))  
           );

    } else{

      integ->SetBinContent(1,0.5*hist->GetBinContent(1));
      for (int i = 2; i<=nbins; i++)
        integ->SetBinContent(i, integ->GetBinContent(i-1)+0.5 * 
           (hist->GetBinContent(i-1)+hist->GetBinContent(i))  
           );

    }
   return integ;
  }
    

//================================= FIR and IIR filters ===================================//


TH1 *
dmtpc::waveform::FIRFilter(const TH1*hist, unsigned int order, const double * bcoeffs, TH1 * out)
{
  if (order < 1) return 0; 

  if (!out)
  {
    TH1 * out = (TH1*) hist->Clone(hist->GetName() + TString("_FIR")); 
    out->SetTitle(out->GetName()); 
  }

  for (int i = 0; i < out->GetNbinsX(); i++)
  {
    double ynew = 0; 
    for (unsigned int j =0; j <= order; j++)
    {
      if (i <(int) j) break; 
      ynew +=  bcoeffs[j] * hist->GetBinContent(i+1-j); 
    }

    out->SetBinContent(i+1,ynew); 
  }

  return out; 

}

TH1 *
dmtpc::waveform::IIRFilter(const TH1* hist, unsigned int order, const double * acoeffs, const double * bcoeffs, TH1 * out)
{
  if (order < 1) return 0; 

  if (!out)
  {
    out = (TH1*) hist->Clone(hist->GetName() + TString("_IIR")); 
    out->SetTitle(out->GetName()); 
  }
  for (int i = 0; i < out->GetNbinsX(); i++)
  {
    double ynew = 0; 

    for (unsigned int j =0; j <= order; j++)
    {
      if (i < (int)j) break; 
      ynew +=  bcoeffs[j] * hist->GetBinContent(i+1-j); 
      if (j>0 && acoeffs[j] != 0) ynew-= acoeffs[j] * out->GetBinContent(i+1-j); 
    }

    ynew /= acoeffs[0]; 

    out->SetBinContent(i+1,ynew); 
  }

  return out; 
}


//Sorta equivalent to matlab poly. (except indices are reversed so that the index matches the order)
static std::complex<double> * poly(unsigned int n, std::complex<double> * zeros)
{

  for (unsigned int i = 0; i < n; i++)
  {
 //   std::cout << zeros[i] << " " ; 
  }

  std::complex<double> * pol = new std::complex<double>[n+1]; 

  //Start out with (x - zero_0)
  pol[0] = -zeros[0]; 
  pol[1] = std::complex<double>(1,0); 


  for (unsigned int i = 1; i < n; i++)
  {

    //Multiply  by each (x - zero_i)
    for (unsigned int j = i+1;;  j--)
    {
      pol[j] = (j > 0 ? pol[j-1] : 0) - (j < i+1 ? pol[j] * zeros[i] : 0); 
      if (j == 0) break; 
    }

  }

  
  return pol; 
}

double
butterworthBand(double w0, double delta, unsigned int order, double ** acoeffs, double ** bcoeffs, bool stop)
{
  *(acoeffs) = new double[2*order+1] ;
  *(bcoeffs) = new double[2*order+1] ;

  std::complex<double> * poles = new std::complex<double>[order]; 
  std::complex<double> * tmp = new std::complex<double>[order]; 
  std::complex<double> pole_product(1,0); 

  //std::cout << "Here" << std::endl; 
  //This code is based on the octave-forge implementation of butter.m
  // There are certainly simplifications possible, but I wanted something that worked...  


  //Compute edges
  double wlow = w0 - delta; 
  double whigh = w0 + delta; 

  //Convert frequency to s space 
  double Wlow = TMath::Tan(TMath::Pi() * wlow / 2.); 
  double Whigh = TMath::Tan(TMath::Pi() * whigh / 2.); 

  //std::cout << " Wlow: " << Wlow << std::endl; 
  //std::cout << " Whigh: " << Whigh << std::endl; 

  //Compute Butterworth poles for given order
  for (unsigned int i = 0; i < order; i++)
  {
    poles[i] = std::exp(std::complex<double>(0, TMath::Pi()* (2*(i+1)+order - 1)/(2*order))); 
    tmp[i] = stop ? (Whigh - Wlow) / poles[i] / 2. : poles[i] * (Whigh - Wlow) / 2. ; 
    pole_product*= -poles[i]; 
  }

  //std::cout << "initial poles: "; 
//  for (unsigned int i = 0; i <order; i++) std::cout << poles[i] << " "; 
  //std::cout << std::endl; 

  //std::cout << "tmp: "; 
//  for (unsigned int i = 0; i <order; i++) std::cout << tmp[i] << " "; 
  //std::cout << std::endl; 
  
  //Transform to band stop/pass filter 
  
  double gain = stop ? 1./std::real(pole_product) : pow(Whigh-Wlow,double(order)) ; 
 
  std::complex<double> * transformed_poles = new std::complex<double>[order*2]; 
  std::complex<double> * transformed_zeros = new std::complex<double>[order*2]; 
  for (unsigned int i = 0; i < order; i++)
  {
    transformed_poles[i] = tmp[i] + std::sqrt(std::pow(tmp[i],2) - Wlow*Whigh); 
    transformed_poles[order+i] = tmp[i] - std::sqrt(std::pow(tmp[i],2) - Wlow*Whigh); 
  }  

  for (unsigned int i = 0; i < order*2; i++)
  {
    transformed_zeros[i] = stop ?  -std::sqrt(std::complex<double>(-Whigh*Wlow,0)) : 0 ; 
    if (i &1) transformed_zeros[i] *= -1; 
  }

  //std::cout << "sz: "; 
  //for (unsigned int i = 0; i < order*2; i++) std::cout << stop_zeros[i] << " "; 
  //std::cout << std::endl; 
  //std::cout << "sp: "; 
//  for (unsigned int i = 0; i < order*2; i++) std::cout << stop_poles[i] << " "; 
  //std::cout << std::endl; 
  //std::cout << "sg: " << gain << std::endl; 



  //Perform bilinear transform to convert to digital 
  
  std::complex<double> * digi_poles = new std::complex<double>[order*2]; 
  std::complex<double> * digi_zeros = new std::complex<double>[order*2]; 

  std::complex<double> digi_gain = std::complex<double>(gain,0);  

  std::complex<double> one(1,0); 
  for (unsigned int i = 0; i < order*2; i++)
  {
    digi_gain *=  (one -transformed_zeros[i]) / (one - transformed_poles[i]); 

    digi_poles[i] = (one + transformed_poles[i]) / (one - transformed_poles[i]); 
    if (stop) 
      digi_zeros[i] = (one + transformed_zeros[i]) / (one - transformed_zeros[i]); 
    else
      digi_zeros[i] = i < order ? 1 : -1; 
  }

  digi_gain = std::real(digi_gain); 

 // std::cout << "z: "; 
//  for (unsigned int i = 0; i < order*2; i++) std::cout << digi_zeros[i] << " "; 
 // std::cout << std::endl; 
 // std::cout << "p: "; 
//  for (unsigned int i = 0; i < order*2; i++) std::cout << digi_poles[i] << " "; 
 // std::cout << std::endl; 
 // std::cout << "g: " << digi_gain << std::endl; 

  //Now, get coefficients from poles and zeroes
  std::complex<double> * bpoly = poly(2*order, digi_zeros); 
  std::complex<double> * apoly = poly(2*order, digi_poles); 

  for (unsigned int i = 0; i < 2*order+ 1; i++)
  {
       (*bcoeffs)[i] = (double) std::real(digi_gain * bpoly[2*order - i]);             
       (*acoeffs)[i] = (double) std::real(apoly[2*order - i]);             
  }

  delete poles; 
  delete digi_poles; 
  delete digi_zeros; 
  delete transformed_poles; 
  delete transformed_zeros; 
  delete apoly; 
  delete bpoly; 
  delete tmp; 
  return std::real(digi_gain); 
}

double
dmtpc::waveform::butterworthBandStop(double w0, double delta, unsigned int order, double ** acoeffs, double ** bcoeffs)
{
  return butterworthBand(w0,delta,order,acoeffs,bcoeffs,true); 
}
double 
dmtpc::waveform::butterworthBandPass(double w0, double delta, unsigned int order, double ** acoeffs, double ** bcoeffs)
{
  return butterworthBand(w0,delta,order,acoeffs,bcoeffs,false); 
}


TH1*
dmtpc::waveform::createFirOutputHist(const TH1* in, int nkernel)
{
  //Look for class:
  char c = 'F';
  if (in->IsA()->GetBaseClass("TH1D"))
    c = 'D';

  TH1* h = createFirOutputHist(in->GetNbinsX(),nkernel,c);
  if (!h) return 0;
  h->SetXTitle(in->GetXaxis()->GetTitle());
  h->SetYTitle(in->GetYaxis()->GetTitle());

  double binW = in->GetXaxis()->GetBinWidth(1);
  double xmin, xmax;
  xmin = in->GetXaxis()->GetXmin() + binW * (nkernel/2);
  xmax = in->GetXaxis()->GetXmax() - binW * (nkernel/2);
  if (nkernel%2==0)
    xmax += binW;
  h->GetXaxis()->SetLimits(xmin,xmax);
  return h;
}

TH1*
dmtpc::waveform::createFirOutputHist(int nin, int nkernel, char type)
{
  if (nin-nkernel<=0) return 0;
  TH1* h;

  switch (type){
    case 'C':
    case 'c':
      h = new TH1C("FirC","",nin-nkernel+1,0,nin-nkernel+1);
      break;
    case 'S':
    case 's':
      h = new TH1S("FirS","",nin-nkernel+1,0,nin-nkernel+1);
      break;
    case 'I':
    case 'i':
      h = new TH1I("FirI","",nin-nkernel+1,0,nin-nkernel+1);
      break;
    case 'F':
    case 'f':
      h = new TH1F("FirF","",nin-nkernel+1,0,nin-nkernel+1);
      break;
    case 'D':
    case 'd':
      h = new TH1D("FirD","",nin-nkernel+1,0,nin-nkernel+1);
      break;
    default:
      return 0;
  }

  return h;

}

TH1*
dmtpc::waveform::runFirFilter(const TH1* in, TH1* out, FirFilter<double>* fir)
{
  if (out==0) out = createFirOutputHist(in,fir->firCoeffsSize());
  if (   in->IsA()->GetBaseClass("TH1F"))
    fir->apply(in->GetNbinsX(),( (TH1F*)in )->GetArray()+1);
  else if ( in->IsA()->GetBaseClass("TH1D"))
    fir->apply(in->GetNbinsX(),( (TH1D*)in )->GetArray()+1);
  else {
    float x[in->GetNbinsX()];
    for (int i = 1; i <= in->GetNbinsX(); i++)
      x[i-1] = in->GetBinContent(i);
    fir->apply(in->GetNbinsX(),x);
  }
  for (int i = 0; i <(int) fir->outputSize(); i++)
    out->SetBinContent(i+1,fir->output()[i]);
  return out;
}

TH1*
dmtpc::waveform::createIirOutputHist(const TH1* in, int nkernel)
{
  //Look for class:
  char c = 'F';
  if (in->IsA()->GetBaseClass("TH1D"))
    c = 'D';

  TH1* h = createFirOutputHist(in->GetNbinsX(),nkernel,c);
  if (!h) return 0;
  h->SetXTitle(in->GetXaxis()->GetTitle());
  h->SetYTitle(in->GetYaxis()->GetTitle());

  double binW = in->GetXaxis()->GetBinWidth(1);
  double xmin, xmax;
  xmin = in->GetXaxis()->GetXmin() + binW * (nkernel/2);
  xmax = in->GetXaxis()->GetXmax() - binW * (nkernel/2);
  if (nkernel%2==0)
    xmax += binW;
  h->GetXaxis()->SetLimits(xmin,xmax);
  return h;
}

TH1*
dmtpc::waveform::createIirOutputHist(int nin, int nkernel, char type)
{
  if (nin-nkernel<=0) return 0;
  TH1* h;

  switch (type){
    case 'C':
    case 'c':
      h = new TH1C("IirC","",nin-nkernel+1,0,nin-nkernel+1);
      break;
    case 'S':
    case 's':
      h = new TH1S("IirS","",nin-nkernel+1,0,nin-nkernel+1);
      break;
    case 'I':
    case 'i':
      h = new TH1I("IirI","",nin-nkernel+1,0,nin-nkernel+1);
      break;
    case 'F':
    case 'f':
      h = new TH1F("IirF","",nin-nkernel+1,0,nin-nkernel+1);
      break;
    case 'D':
    case 'd':
      h = new TH1D("IirD","",nin-nkernel+1,0,nin-nkernel+1);
      break;
    default:
      return 0;
  }

  return h;

}

TH1*
dmtpc::waveform::runIirFilter(const TH1* in, TH1* out, IirFilter<double>* iir)
{
  if (!out) out = createIirOutputHist(in,iir->firCoeffsSize());
  if (   in->IsA()->GetBaseClass("TH1F"))
    iir->apply(in->GetNbinsX(),( (TH1F*)in )->GetArray()+1);
  else if ( in->IsA()->GetBaseClass("TH1D"))
    iir->apply(in->GetNbinsX(),( (TH1D*)in )->GetArray()+1);
  else {
    float x[in->GetNbinsX()];
    for (int i = 1; i <= in->GetNbinsX(); i++)
      x[i-1] = in->GetBinContent(i);
    iir->apply(in->GetNbinsX(),x);
  }

  for (size_t i = 0; i < iir->outputSize(); i++)
    out->SetBinContent(i+1,iir->output()[i]);
  return out;
}

unsigned int
dmtpc::waveform::zeroSupress(TH1* in, double zero_level,  double noise_ceiling, unsigned min_samples, unsigned window_size, double * mean, double * rms)
{
  vector<int> kill_list;

  //this is very slow way of doing this... probably using a buffer would be way faster... 
  for (unsigned i = 1; i <= (unsigned) in->GetNbinsX(); i++)
  {
    int nabove = 0;
    for (unsigned j = i-window_size; j <= i+window_size; j++)
    {
      if (j < 1) continue;
      if (j >(unsigned) in->GetNbinsX()) break;
      if (fabs(in->GetBinContent(j) - zero_level) >  noise_ceiling)
      {
        nabove++;
      }
    }
    if ((unsigned)nabove < min_samples)
    {
      kill_list.push_back(i);
    }
  }

  double sum = 0;
  double sum2 = 0;

  for (unsigned i = 0; i < kill_list.size(); i++)
  {
    if (mean || rms)
    {
      sum+= in->GetBinContent(kill_list[i]);
    }
    if (rms)
    {
      sum2+= in->GetBinContent(kill_list[i]) * in->GetBinContent(kill_list[i]);
    }

    in->SetBinContent(kill_list[i],zero_level);
  }

  if (mean)
  {
    *mean = sum / kill_list.size();
  }

  if (rms)
  {
    *rms = sqrt(sum2/kill_list.size() - sum*sum/kill_list.size()/kill_list.size());
  }

  return kill_list.size();
}


TH1 *
dmtpc::waveform::cropZeros(const TH1 * in, TH1 * outptr)
{
  int start_bin = 1, end_bin = in->GetNbinsX();

  for (int i = 1; i <= in->GetNbinsX(); i++)
  {
    if (in->GetBinContent(i) == 0)
      start_bin++;
    else
      break;
  }

  if (start_bin == in->GetNbinsX())
  {
    //empty hist! 
    return 0;
  }

  for (int i = in->GetNbinsX(); i >=1; i--)
  {
    if (in->GetBinContent(i) == 0)
      end_bin--;
    else
      break;
  }

  TH1 * out = newTH1StealType(in, TString(in->GetName())+TString("_cropped"), TString(in->GetTitle()) +TString( " (cropped)"),
                              end_bin - start_bin + 1, in->GetXaxis()->GetBinLowEdge(start_bin), in->GetXaxis()->GetBinLowEdge(end_bin+1),outptr);
  int j = 1;
  for (int i = start_bin; i <= end_bin; i++)
  {
    out->SetBinContent(j++, in->GetBinContent(i));
  }

  return out;
}

TH1* 
dmtpc::waveform::gausConv(const TH1* hist, double sigma, bool rmEdge,double cut,const char* name)
{

  //Set the convolution kernel
  int N = (int) (sigma*sqrt(-2*log(cut)));
  double kernel[1+2*N];
  for (int i = -N; i<=N; i++){
      kernel[i+N] = 
	erf((i+0.5)/(sqrt(2)*sigma)) - erf((i-0.5)/(sqrt(2)*sigma));

  }  
  
  return convolution(hist,N,kernel,rmEdge,name);
}

TH1*
dmtpc::waveform::convolution(const TH1* hist, int N, const double* arr, bool removeEdges,const char* name)
{
  int nbins = hist->GetNbinsX();
  double xmax = hist->GetXaxis()->GetXmax();
  double xmin = hist->GetXaxis()->GetXmin();
  
  TH1F* filter = 0;
  if (removeEdges)
  {
    double xsize = hist->GetXaxis()->GetBinWidth(1); 
    filter = new TH1F(name,name,nbins-2*N-2, xmin+xsize*(N+1),xmax-xsize*(N-1));
    double sum = 0;

    for (int i = 0; i<2*N+1; i++){
      sum+= arr[i];
    }

    double value;
    for (int i = N+2; i<=nbins-N-1; i++)
    {
      value = 0;
      for (int j = i-N; j<=i+N; j++)
        value+= arr[j-i+N] * get(hist,j);

      set(filter,i-N-1,value/sum);
    }

  }else{
   
    filter = new TH1F(name,name,nbins, xmin,xmax);

    double value, sum;
    int minBin, maxBin;
    for (int i = 1; i<= nbins; i++)
    {
      value = 0;
      minBin = max(2,i-N);
      maxBin = min(nbins-1,i+N);
      sum = 0;
      for (int j = minBin; j<=maxBin; j++){
        value += arr[j-i+N]*get(hist,j);
        sum += arr[j-i+N];
      }
      if (sum==0) set(filter,i,0);
      else set(filter,i,value/sum);
    }



  }
  return filter;
}
