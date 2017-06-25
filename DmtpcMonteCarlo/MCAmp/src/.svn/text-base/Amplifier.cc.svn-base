#include "Amplifier.hh" 
#include "TH1.h" 
#include "WaveformProcessing.hh"
#include "TMath.h"


void applyBandwidth(TH1 * in, double bandwidth, bool invert = false) 
{
  double rc = 1. / (2*TMath::Pi() * bandwidth); 
  double width = in->GetBinWidth(1)*1e-9;  //ns
  double alpha = width / ( rc + width); 

  if (invert) alpha = 1 -alpha; 

  for (int i = 1; i <= in->GetNbinsX(); i++)
  {
    in->SetBinContent(i, in->GetBinContent(i) * alpha + (1-alpha) * in->GetBinContent(i-1)); 
  }
}

TH1 * dmtpc::mc::amp::ChargeSensitiveAmplifier::result(const TH1 * target, TH1 * result) const
{

  if (!result) result = (TH1*) target->Clone(TString::Format("%s_amped", target->GetName())); 
  result->GetYaxis()->SetTitle("Volts"); 
  result->GetXaxis()->SetTitle("ns"); 

  /** warning not thread safe!!! **/ 
  static int last_size = 0; 
  static double * lookup =0; 
  if (last_size!= target->GetNbinsX())
  {
    if (lookup) delete lookup; 
    last_size = target->GetNbinsX(); 
    lookup = new double[last_size]; 

    for (int i = 0; i < last_size; i++) 
    {
      lookup[i] = target->GetBinWidth(1) * gain *  exp(-target->GetBinWidth(1) * i / tc);  
 //     printf("lookup: %d %f\n", i, lookup[i]); 
    }
  }


  for (int i = 1; i<= result->GetNbinsX(); i++)
  {
    double val = 0; 

    for (int j =1 ; j <= i; j++)
    {
      val +=  target->GetBinContent(j) * lookup[i-j]; 
    }

    result->SetBinContent(i,-val); 
 //   printf("%d: %f\n", i, -val); 
  }

  printf("integral before bandwidth: %f\n", result->Integral()); 
  applyBandwidth(result,bandwidth); 

  printf("integral after bandwidth: %f\n", result->Integral()); 

  return result; 
} 

dmtpc::mc::amp::FastAmplifier::FastAmplifier(double gain, double  bandwidth_low, double bandwidth_high, int order)
  : gain(gain), bandwidth_low(bandwidth_low), bandwidth_high(bandwidth_high), order (order) 
{

  last_norml = 0; 
  a=0,b=0; 
}

TH1 * dmtpc::mc::amp::FastAmplifier::result(const TH1 * target, TH1 * result)  const
{
   if (!result) result = (TH1*) target->Clone(TString::Format("%s_amped", target->GetName())); 

  result->GetYaxis()->SetTitle("volts"); 
  result->GetXaxis()->SetTitle("ns"); 



  double norml = 0.5 / (1.e-9*target->GetBinWidth(1)); 

  if (last_norml == 0 || last_norml != norml)
  {
    if (a) free(a); 
    if (b) free(b); 
    double w0 = (bandwidth_low + bandwidth_high)/2. / norml; 
    double delta = (bandwidth_high - bandwidth_low)/2./norml; 
    dmtpc::waveform::butterworthBandPass(w0,delta,order,&a,&b); 
    last_norml = norml; 
  }

  dmtpc::waveform::IIRFilter(target,order*2,a,b,result); 

  for (int i=1; i <= result->GetNbinsX(); i++)
  {
    result->SetBinContent(i, result->GetBinContent(i) * gain * result->GetBinWidth(i) * 5e-8); 
  }

  return result;
}
