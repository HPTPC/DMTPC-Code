#include "TCanvas.h"
#include "WaveformProcessing.hh"
#include "WaveformAnalysis.hh"
#include "CspWaveform.hh"
#include "CspPulse.hh"
#include "PMTWaveform.hh"
#include "PMTPulse.hh"
#include "FastWaveform.hh"
#include "FastPulse.hh"
#include "TH1.h"
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;
#define max(x,y) x>y?x:y
#define min(x,y) x<y?x:y
#define hget(h,i) h->GetBinContent(i)
#define hcenter(h,i) h->GetXaxis()->GetBinCenter(i)
#define verase(v,i) v.erase(v.begin()+i)
double
dmtpc::waveform::analysis::baseline(const TH1* hist, double& rms, int binMin, int binMax)
{

  double base=0;
  rms=0;
  int n = binMax-binMin+1;
  for (int i = binMin; i<=binMax; i++)
  {
    double v = hget(hist,i);
    base+=v;
    rms+=v*v;
  }

  base/=n;
  rms/=n;
  rms -= base*base;
  rms = sqrt(rms);
  return base;

}

int
dmtpc::waveform::analysis::minbin(const TH1* hist)
{
  int minbin=0;
  double minval = DBL_MAX; 
  // neglects the underflow, the overflow, and the last bin

  for (int i = 1; i< hist->GetNbinsX(); i++)
  {
    if (hist->GetBinContent(i) < minval)
    {
      minval = hist->GetBinContent(i); 
      minbin = i; 
    }
  }
  
  return minbin;
} 

int
dmtpc::waveform::analysis::maxbin(const TH1* hist)
{
  int maxbin=0;
  double maxval = -DBL_MAX; 
  // neglects the underflow, the overflow, and the last bin
  for (int i = 1; i< hist->GetNbinsX(); i++)
  {
    if (hist->GetBinContent(i) > maxval)
    {
      maxval = hist->GetBinContent(i); 
      maxbin = i; 
    }
  }
 
  return maxbin;
}


bool 
 dmtpc::waveform::analysis::isPeak(const TH1* hist, int bin, int nbins)
{
  double binval =hget(hist,bin);
  int minBin = max(1,bin-nbins);
  int maxBin = min(hist->GetNbinsX(),bin+nbins);
  for (int i = minBin; i<= maxBin; i++)
    if (hget(hist,i)>binval) return false;

  return true;
}

bool 
dmtpc::waveform::analysis::isTrough(const TH1* hist, int bin, int nbins)
{
  double binval =hget(hist,bin);
  int minBin = max(1,bin-nbins);
  int maxBin = min(hist->GetNbinsX(),bin+nbins);
  for (int i = minBin; i<= maxBin; i++)
    if (hget(hist,i)<binval) return false;

  return true;
}


double
dmtpc::waveform::analysis::integral(const TH1* hist, double start, double end)
{


  start = max(hist->GetXaxis()->GetXmin(),start);
  end = min(hist->GetXaxis()->GetXmax(),end);
  int startBin = hist->GetXaxis()->FindBin(start);
  int endBin = hist->GetXaxis()->FindBin(end);

  double startCenter = hist->GetXaxis()->GetBinCenter(startBin);
  double endCenter = hist->GetXaxis()->GetBinCenter(endBin);
  
  double width = hist->GetXaxis()->GetBinWidth(1);

  double sum = 0;
  sum += hget(hist,startBin) * (start-startCenter+0.5*width)/width;
  sum += hget(hist,endBin) * (endCenter - 0.5*width-end)/width;
  for(int i = startBin+1; i<endBin; i++) sum += hget(hist,i);
  return sum;
}



void 
dmtpc::waveform::analysis::riseTime(const TH1* hist, const vector<double>& list,
                  vector<double>& values, double startTime, double endTime,
                  bool fromStart)
{

  double binWidth = hist->GetXaxis()->GetBinWidth(1);
  int n = list.size();

  values.clear();
  values.resize(n,-1);
  
  int startBin = hist->GetXaxis()->FindBin(startTime);
  int endBin = hist->GetXaxis()->FindBin(endTime);
  startBin = startBin==1?startBin:startBin-1;
  double v;
  if (fromStart){
    double lastV=hget(hist,startBin);
    for (int i = startBin; i<=endBin ;i++)
    {
      v = hget(hist, i);
    
      for (int j = 0; j<n ; j++){
            
        if (values[j]==-1&&lastV<list[j]&&v>=list[j]){
          double binFrac = (list[j] - lastV) / (v-lastV);
          values[j] = binFrac*binWidth + hcenter(hist,i-1);

          if (j==n-1) break;

        }//if reaches value
 

      }//values
      lastV=v;
    }//bins
  }else{
    double lastV=hget(hist,endBin);
    for (int i = endBin; i>=startBin; i--)
    {
       v = hget(hist,i);
       for (int j=n-1; j>=0; j++){

         if (values[j]==-1&&lastV>list[j]&&v<=list[j]){

           double binFrac = (list[j]-lastV)/(v-lastV);
           values[j] = hcenter(hist,i+1)-binFrac*binWidth;

           if (j==0) break;
         }


       }//values
       lastV=v;
    }
  }//which side to start at
  

}

void 
dmtpc::waveform::analysis::fallTime(const TH1* hist,const vector<double>& list,
                  vector<double>& values, double startTime, double endTime,
                  bool fromStart)
{

  double binWidth = hist->GetXaxis()->GetBinWidth(1);
  int n = list.size();
  values.clear();
  values.resize(n,-1);
  int startBin = hist->GetXaxis()->FindBin(startTime);
  int endBin = hist->GetXaxis()->FindBin(endTime);
  endBin = endBin==hist->GetNbinsX()?endBin:endBin+1;
  double v;
  if (fromStart){
    double lastV=hget(hist,startBin);
    for (int i = startBin; i<=endBin ;i++)
    {
      v = hget(hist, i);
      for (int j = 0; j<n ; j++){

        if (values[j]==-1&&lastV>list[j]&&v<=list[j]){
          double binFrac = (list[j] - lastV) / (v-lastV);
          values[j] = binFrac*binWidth + hcenter(hist,i-1);

          if (j==n-1) break;

        }//if reaches value


      }//values
      lastV=v;
    }//bins
  }else{
    double lastV=hget(hist,endBin);
    for (int i = endBin; i>=startBin; i--)
    {
       v = hget(hist,i);
       for (int j=n-1; j>=0; j--){

         if (values[j]==-1&&lastV<list[j]&&v>=list[j]){

           double binFrac = (list[j]-lastV)/(v-lastV);
           values[j] = hcenter(hist,i+1)-binFrac*binWidth;

           if (j==0) break;
         }


       }//values
       lastV=v;
    }
  }//which side to start at


}

double 
dmtpc::waveform::analysis::startTime(const TH1* hist, int bin,
                     double threshold,int& startBin,int minBin)
{

  if (minBin==-1) minBin=1;

  double binWidth = hist->GetXaxis()->GetBinWidth(1);
  startBin = minBin;
  double time = hcenter(hist,minBin);
  double v, lastV = hget(hist,bin);
  for (int i = bin; i>= minBin; i--)
  {
    v = hget(hist,i);
    if (lastV > threshold&&v<=threshold){
      double binFrac = (threshold-lastV)/(v-lastV);
      time = hcenter(hist,i+1)-binWidth*binFrac;
      //      time = hist->GetBinLowEdge(i+1);
      startBin = binFrac>0.5?i:i+1;
      //      startBin = i+1;
      break;
    }
    lastV = v;
  }
  return time;
}

double 
dmtpc::waveform::analysis::endTime(const TH1* hist, int bin,
               double threshold,int& endBin,int maxBin)
{

  if (maxBin==-1) maxBin=hist->GetNbinsX();

  double binWidth = hist->GetXaxis()->GetBinWidth(1);
  endBin = maxBin;
  double time = hcenter(hist,maxBin);
  double v, lastV = hget(hist,bin);
  for (int i = bin; i<= maxBin; i++)
  {
    v = hget(hist,i);
    if (lastV > threshold&&v<=threshold){
      double binFrac = (threshold-lastV)/(v-lastV);
      time = hcenter(hist,i-1)+binWidth*binFrac;
      //      time = hist->GetBinLowEdge(i);
      endBin = binFrac>0.5?i:i-1;
      break;
    }
    lastV = v;
  }
  return time;

}

vector<int> 
dmtpc::waveform::analysis::peaks(const TH1* hist, double threshold,
                          int minBin,int maxBin)
{

  minBin = minBin<0?1:minBin;
  maxBin = maxBin<0?hist->GetNbinsX():maxBin;
  vector<int> p;
  double v, lastV=hget(hist,minBin);
  bool isInPeak = false;
  for (int i = minBin; i<= maxBin; i++)
  {

    v = hget(hist,i);
    if (lastV<threshold&&v>=threshold){
      isInPeak=true;
      p.push_back(i);
    }else if (lastV>= threshold&&v<threshold) isInPeak=false;

    if (isInPeak){
      if (v>hget(hist,p[p.size()-1])) p[p.size()-1] = i;
    }

    lastV=v;
  }
  return p;

}

vector<int> 
dmtpc::waveform::analysis::valleys(const TH1* hist, double threshold,
                            int minBin,int maxBin)
{

  minBin = minBin<0?1:minBin;
  maxBin = maxBin<0?hist->GetNbinsX():maxBin;
  vector<int> p;
  double v, lastV=hget(hist,minBin);
  bool isInPeak = false;
  for (int i = minBin; i<= maxBin; i++)
  {

    v = hget(hist,i);
    if (lastV>threshold&&v<=threshold){
      isInPeak=true;
      p.push_back(i);
    }else if (lastV<= threshold&&v>threshold) isInPeak=false;
    
    if (isInPeak){
      if (v<hget(hist,p[p.size()-1])) p[p.size()-1] = i;
    }
    
    lastV=v;
  }
  return p;

}

void 
dmtpc::waveform::analysis::peaksAndValleys(const TH1* hist,vector<int>& pkBin,
                         vector<int>& valBin, 
                         int minBin,int maxBin,int nbins)
{

  minBin = minBin<0?1:minBin;
  maxBin = maxBin<0?hist->GetNbinsX():maxBin;

  double v;
  double currentMin=0;
  int bin = 0;
  int npeak = 0;
  for (int i = minBin; i<=maxBin; i++)
  {
    v = hget(hist,i);
    if (isPeak(hist,i,nbins))
    {

      pkBin.push_back(i);
      if (npeak>0) valBin.push_back(bin);
      currentMin = v;
      bin = i;
      npeak++;
    }else{

      bin = v<currentMin?i:bin;
      currentMin = v<currentMin?v:currentMin;
      
    }

  }


}

void 
dmtpc::waveform::analysis::mergePeaksByDistance(const TH1* hist,std::vector<int>& pkBin, 
                               std::vector<int>& valBin,int minDist)
{

  if(pkBin.size()<=1) return;

  //First remove all peaks within a certain distance of one another
  vector<int> rmPk(pkBin.size(),0);
  //Find all peaks within this distance from one another
  for (unsigned int i = 1; i<pkBin.size(); i++)
    if (pkBin[i]-pkBin[i-1] <minDist) rmPk[i] = 1;

  //Loop through everything and assign peaks new bins
  for (unsigned int i = 1; i<pkBin.size(); i++)
  {
    if (rmPk[i]==0) continue;
    int bin = hget(hist,pkBin[i])>hget(hist,pkBin[i-1]) ?
              pkBin[i] : pkBin[i-1];

    unsigned int j = i-1;
    while(rmPk[j+1]==1)
    {
      pkBin[j] = bin;
      j--;
    }
  }

  //Eliminate repeated peaks

  for (unsigned int i = 1; i<pkBin.size(); i++)
  {
    if (pkBin[i]==pkBin[i-1]){
      verase(pkBin,i);
      i--;
    }
  }

  //Assign each valley to a possible position

  int minP = pkBin[0], maxP = pkBin[pkBin.size()-1];
  vector<int> valPos;
  for (unsigned int i = 0; i <valBin.size(); i++)
  {
    if (valBin[i]>=maxP||valBin[i]<minP){
       verase(valBin,i);
       i--;
       continue;
    }

    for (unsigned int j = 0; j < pkBin.size()-1; j++)
    {
      if (valBin[i] >= pkBin[j]&&valBin[i]<pkBin[j+1])
      {
        valPos.push_back(j);
        break;
      }

    }
  }

  //cout <<valBin.size()<<endl;                                                            
  if(valBin.size()>1){
    for (unsigned int i = 0; i< valBin.size()-1; i++)
      {
	
	if (valPos[i]!=valPos[i+1]) continue;
	if (hget(hist,valBin[i]) <= hget(hist,valBin[i+1])){
	  verase(valPos,i+1);
	  verase(valBin,i+1);
	}else{
	  verase(valPos,i);
	  verase(valBin,i);
	}
	i--;
      }
  }

}


/*

void 
dmtpc::waveform::analysismergePeaksByDepth(const TH1F* hist,vector<int>& pkBin, 
                               vector<int>& valBin,double minDepth)
{

  if (pkBin.size()<=1) return;

  //First bin:
  if (hget(hist,pkBin[0])-hget(hist,valBin[0])<minDepth ||
      hget(hist,pkBin[1])-hget(hist,valBin[0])<minDepth )
  {

    if (hget(hist,pkBin[0])>hget(hist,pkBin[1]))
    {//Second peak is spurious
      verase(pkBin,1);
      if (valBin.size()==1) verase(valBin,0);
      else{
        if (hget(hist,valBin[0])>hget(hist,valBin[1]))
          verase(valBin,0);
        else verase(valBin,1);
      }
    }else{//First peak is spurious
      verase(pkBin,0);
      verase(valBin,0);
    }
  


  }




  //Remove all very shallow peaks (likely noise)
  vector<int> rmPk(pkBin.size(),0);
  //Find all peaks to remove 
  rmPk[0] = hget(hist,pkBin[i]-hget(hist,valBin[i])) < minDepth;
  rmPk[pkBin.size()-1] = hget(hist,pkBin[i]) - hget(hist,valBin[i-1]) < minDepth;

  for (unsigned int i = 1; i<pkBin.size()-1; i++)
    if (hget(hist,pkBin[i])-hget(hist,valBin[i-1]) <minDepth ||
        hget(hist,pkBin[i])-hget(hist,valBin[i]) <minDepth)
    rmPk[i] = 1;

  //Loop through everything and assign peaks new bins
  for (unsigned int i = 1; i<pkBin.size(); i++)
  {
    if (rmPk[i]==0) continue;
    int bin = hget(hist,pkBin[i])>hget(hist,pkBin[i-1]) ?
              pkBin[i] : pkBin[i-1];

    unsigned int j = i-1;
    while(rmPk[j+1]==1)
    {
      pkBin[j] = bin;
      j--;
    }
  }
  //Eliminate repeated peaks
  for (unsigned int i = 1; i<pkBin.size(); i++)
  {
    if (pkBin[i]==pkBin[i-1]){
      verase(pkBin,i);
      i--;
    }
  }

  //Assign each valley to a possible position
  int minP = pkBin[0], maxP = pkBin[pkBin.size()-1];
  vector<int> valPos;
  for (unsigned int i = 0; i <valBin.size(); i++)
  {
    if (valBin[i]>=maxP||valBin[i]<minP){
       verase(valBin,i);
       i--;
       continue;
    }

    for (unsigned int j = 0; j < pkBin.size()-1; j++)
    {
      if (valBin[i] >= pkBin[j]&&valBin[i]<pkBin[j+1])
      {
        valPos.push_back(j);
        break;
      }
    }
  }
  //Find best match for each position
  for (unsigned int i = 0; i< valBin.size()-1; i++)
  {
    if (valPos[i]!=valPos[i+1]) continue;
    if (hget(hist,valBin[i]) <= hget(hist,valBin[i+1])){
      verase(valPos,i+1);
      verase(valBin,i+1);
    }else{
      verase(valPos,i);
      verase(valBin,i);
    }
    i--;
  }

      
}
*/

void 
dmtpc::waveform::analysis::analyzeCSP(const TH1* h, CspWaveform& wf, Double_t gausConvSigma)
{
  TH1*  htemp;

  double base,
         rms,
         hmax,
	 hmaxTime,
	 hmin,
	 hminTime;
  int    hmaxBin,
         hminBin;

  vector<double> rise(6),
                 fall(6);

  double peak,
	 peakTime,
	 startTime,
	 endTime,
	 integral;
  int    peakBin,
         startBin,
	 endBin;

  double threshold;
  vector<double> riseV(6),
                 fallV(6);

  double rf[6] = {0,0.1,0.25,0.5,0.75,0.9};
  double binWidth = h->GetXaxis()->GetBinWidth(1);

//Clear waveform
  wf.clear();

//Smoothing
  htemp = gausConv(h,gausConvSigma);
  
//General waveform parameters
  // MAL: change to using h instead of htemp for baseline calculation
  base = dmtpc::waveform::analysis::baseline(h,rms,100,600); 
  hmax = htemp->GetMaximum();
  hmaxBin = htemp->GetMaximumBin();
  hmin = htemp->GetMinimum();
  hminBin = htemp->GetMinimumBin();
  hmaxTime = htemp->GetXaxis()->GetBinCenter(hmaxBin);
  hminTime = htemp->GetXaxis()->GetBinCenter(hminBin);

  wf.setBase(base);
  wf.setRMS(rms);
  wf.setWfMax(hmax);
  wf.setWfMaxTime(hmaxTime);
  wf.setWfMaxBin(hmaxBin);
  wf.setWfMin(hmin);
  wf.setWfMinTime(hminTime);
  wf.setWfMinBin(hminBin);

//Pulse parameters: only allow 1 pulse per waveform for CSP

  peak = hmax-base;
  peakBin = hmaxBin;
  peakTime = hmaxTime;

  threshold = base;
  startTime = dmtpc::waveform::analysis::startTime(htemp,peakBin,threshold,startBin);
  endTime = dmtpc::waveform::analysis::endTime(htemp,peakBin,threshold,endBin);

  for (int i = 0; i < 6; i++)
  {
    riseV[i] = rf[i] * peak + base;
    fallV[i] = rf[5-i] * peak + base;
  }
  
  dmtpc::waveform::analysis::riseTime(htemp,riseV,rise,max(startTime-binWidth,htemp->GetXaxis()->GetXmin()),peakTime,true);
  dmtpc::waveform::analysis::fallTime(htemp,fallV,fall,peakTime,min(endTime+binWidth,htemp->GetXaxis()->GetXmax()-binWidth*0.001),false);

  for (int i =0; i<6; i++)
  {
    rise[i] = peakTime - rise[i];
    fall[i] = fall[i]-peakTime;
  }

  //rise[0] = peakTime-startTime;
  integral = dmtpc::waveform::analysis::integral(htemp,startTime,endTime);
  CspPulse p(peakBin);
  p.setRise( &(rise[0]) );
  p.setFall( &(fall[0]) );
  p.setPeak(peak);
  p.setPeakTime(peakTime);
  p.setStartTime(startTime);
  p.setEndTime(endTime);
  p.setStartBin(startBin);
  p.setEndBin(endBin);
  p.setIntegral(integral);
  wf.add(p);

  delete htemp;
}

void 
dmtpc::waveform::analysis::analyzePMT(const TH1* h, PMTWaveform& wf, Double_t gausConvSigma)
{
  TH1F*  htemp;  TH1F*  htempinverted;

  double base,
         rms,
         hmax,
	 hmaxTime,
	 hmin,
	 hminTime;
  int    hmaxBin,
         hminBin;

  vector<double> rise(6),
                 fall(6);

  double peak,
	 peakTime,
	 startTime,
	 endTime,
	 integral;
  int    peakBin,
         startBin,
	 endBin;

  double threshold;
  vector<double> riseV(6),
                 fallV(6);

  double rf[6] = {0,0.1,0.25,0.5,0.75,0.9};
  double binWidth = h->GetXaxis()->GetBinWidth(1);

//Clear waveform
  wf.clear();

//Smoothing
  htemp = ((TH1F*)h->Clone()); 
//  htemp = tools::gausConv(h,gausConvSigma);

//Inversion
  htempinverted = ((TH1F*)htemp->Clone());
  for (int i=1; i<=htempinverted->GetNbinsX(); i++) htempinverted->SetBinContent( i, -htempinverted->GetBinContent(i) );

//General waveform parameters
  // MAL: change to using h instead of htemp for baseline calculation
  base = dmtpc::waveform::analysis::baseline(h,rms,100,600);
  //base = dmtpc::waveform::analysis::baseline(htemp,rms,1,500); 

  hmax = htemp->GetMaximum();
  hmaxBin = htemp->GetMaximumBin();
  hmin = htemp->GetMinimum();
  hminBin = htemp->GetMinimumBin();
  hmaxTime = htemp->GetXaxis()->GetBinCenter(hmaxBin);
  hminTime = htemp->GetXaxis()->GetBinCenter(hminBin);

  wf.setBase(base);
  wf.setRMS(rms);
  wf.setWfMax(hmax);
  wf.setWfMaxTime(hmaxTime);
  wf.setWfMaxBin(hmaxBin);
  wf.setWfMin(hmin);
  wf.setWfMinTime(hminTime);
  wf.setWfMinBin(hminBin);

//Pulse parameters: only allow 1 pulse per waveform for CSP

  peak = hmin-base;
  peakBin = hminBin;
  peakTime = hminTime;

  threshold = base;

  startTime = dmtpc::waveform::analysis::startTime(htempinverted,peakBin,-threshold,startBin);
  endTime = dmtpc::waveform::analysis::endTime(htempinverted,peakBin,-threshold,endBin);

  for (int i = 0; i < 6; i++)
  {
    riseV[i] = rf[i] * peak + base;
    fallV[i] = rf[5-i] * peak + base;
  }
  
  dmtpc::waveform::analysis::riseTime(htemp,riseV,rise,max(startTime-binWidth,htemp->GetXaxis()->GetXmin()),peakTime,true);
  dmtpc::waveform::analysis::fallTime(htemp,fallV,fall,peakTime,min(endTime+binWidth,htemp->GetXaxis()->GetXmax()-binWidth*0.001),false);

  for (int i =0; i<6; i++)
  {
    rise[i] = peakTime - rise[i];
    fall[i] = fall[i]-peakTime;
  }

  //rise[0] = peakTime-startTime;
  integral = dmtpc::waveform::analysis::integral(htemp,startTime,endTime);
  PMTPulse p(peakBin);
  p.setRise( &(rise[0]) );
  p.setFall( &(fall[0]) );
  p.setPeak(peak);
  p.setPeakTime(peakTime);
  p.setStartTime(startTime);
  p.setEndTime(endTime);
  p.setStartBin(startBin);
  p.setEndBin(endBin);
  p.setIntegral(integral);
  wf.add(p);

  delete htemp;
  delete htempinverted;
}

void
dmtpc::waveform::analysis::analyzeFast(const TH1* h, FastWaveform& wf)
{
  double SMOOTH_WIDTH=1.5;
  double MIN_FAST_PEAK=0.65;
  int PEAK_FINDER_MIN_DIST=5;
  int MIN_PEAK_DIST=10;

  TH1*  htemp;
  double base,
         rms,
         hmax,
	 hmaxTime,
	 hmin,
	 hminTime;
  int    hmaxBin,
         hminBin;

  vector<double> rise(6),
                 fall(6);

  double peak,
	 peakTime,
	 startTime,
	 endTime,
	 integral;
  int    peakBin,
         startBin,
	 endBin;

  double fastPeak=0,
	 fastPeakTime=0,
         trough=0,
	 troughTime=0,
	 slowPeak=0,
	 slowPeakTime=0;

  int	 fastPeakBin=0,
	 troughBin=0,
	 slowPeakBin=0;

  double threshold;
  vector<double> riseV(6),
                 fallV(6);

  vector<int> pkBin,
              valBin;

  int minBin, maxBin;
  double rf[6] = {0,0.1,0.25,0.5,0.75,0.9};
  double binWidth = h->GetXaxis()->GetBinWidth(1);

//Clear waveform
  wf.clear();

//Smoothing
  htemp = gausConv(h,SMOOTH_WIDTH);

  //Inversion                                                                                                                                                            
  //htempinverted = ((TH1F*)htemp->Clone());
  for (int i=1; i<=htemp->GetNbinsX(); i++) htemp->SetBinContent( i, -htemp->GetBinContent(i) );

//General waveform parameters
  // MAL: change to using h instead of htemp for baseline calculation
  base = dmtpc::waveform::analysis::baseline(h,rms,100,600);
  //base = dmtpc::waveform::analysis::baseline(htemp,rms,1,500); 
  hmax = htemp->GetMaximum();
  hmaxBin = htemp->GetMaximumBin();
  hmin = htemp->GetMinimum();
  hminBin = htemp->GetMinimumBin();
  hmaxTime = htemp->GetXaxis()->GetBinCenter(hmaxBin);
  hminTime = htemp->GetXaxis()->GetBinCenter(hminBin);

  wf.setBase(base);
  wf.setRMS(rms);
  wf.setWfMax(hmax);
  wf.setWfMaxTime(hmaxTime);
  wf.setWfMaxBin(hmaxBin);
  wf.setWfMin(hmin);
  wf.setWfMinTime(hminTime);
  wf.setWfMinBin(hminBin);

  //Pulse parameters: only allow 1 pulse per waveform for CSP

  peak = hmax-base;
  peakBin = hmaxBin;
  peakTime = hmaxTime;

  threshold = base;
  startTime = dmtpc::waveform::analysis::startTime(htemp,peakBin,threshold,startBin);
  endTime = dmtpc::waveform::analysis::endTime(htemp,peakBin,threshold,endBin);

  for (int i = 0; i < 6; i++)
  {
    riseV[i] = rf[i] * peak + base;
    fallV[i] = rf[5-i] * peak + base;
  }
  
  dmtpc::waveform::analysis::riseTime(htemp,riseV,rise,max(startTime-binWidth,htemp->GetXaxis()->GetXmin()),peakTime,true);
  dmtpc::waveform::analysis::fallTime(htemp,fallV,fall,peakTime,min(endTime+binWidth,htemp->GetXaxis()->GetXmax()-binWidth*0.001),false);
  for (int i = 0; i<6; i++)
  {
    rise[i] = peakTime - rise[i];
    fall[i] = fall[i] - peakTime;
  }

//  rise[0] = peakTime-startTime;
  integral = (startTime-endTime) * base / binWidth +dmtpc::waveform::analysis::integral(htemp,startTime,endTime);
 // cout <<"Peak Bin: "<<peakBin<<endl;
 // cout <<"Peak position "<<peakTime<<endl;
 // cout <<"Peak "<<peak-base<<endl;
  FastPulse p(peakBin);
  p.setRise( &(rise[0]) );
  p.setFall( &(fall[0]) );
  p.setPeak(peak);
  p.setPeakTime(peakTime);
  p.setStartTime(startTime);
  p.setEndTime(endTime);
  p.setStartBin(startBin);
  p.setEndBin(endBin);
  p.setIntegral(integral);

  //Fast preamp parameters
  
  minBin = rise[3]<-0.5?1
	   :htemp->GetXaxis()->FindBin(peakTime-rise[3]);
  maxBin = fall[3]<-0.5?htemp->GetNbinsX()
	   :htemp->GetXaxis()->FindBin(fall[3]+peakTime);
  dmtpc::waveform::analysis::peaksAndValleys(htemp,pkBin,valBin,
		  minBin, maxBin, PEAK_FINDER_MIN_DIST);

//  cout <<"Number of peaks "<< pkBin.size() << endl;
 //// cout <<"Number of valleys "<< valBin.size()<<endl;

  dmtpc::waveform::analysis::mergePeaksByDistance(htemp,pkBin,valBin,MIN_PEAK_DIST);

  if(pkBin.size()>0){
    for (unsigned int i = 0; i < pkBin.size(); i++)
      {
	if (hget(htemp,pkBin[i])-base> MIN_FAST_PEAK*peak)
	  {
	    fastPeakBin = pkBin[i];
	    fastPeakTime = htemp->GetXaxis()->GetBinCenter(pkBin[i]);
	    fastPeak = hget(htemp,pkBin[i])-base;
	    slowPeakBin = pkBin[i];
	    slowPeakTime = htemp->GetXaxis()->GetBinCenter(pkBin[i]);
	    slowPeak = hget(htemp,pkBin[i])-base;
	    troughBin = pkBin[i];
	    troughTime = htemp->GetXaxis()->GetBinCenter(pkBin[i]);
	    trough = hget(htemp,pkBin[i])-base;
	    break;
	  }else{
	  // only erase if there's elements to erase
	  if(pkBin.size()>0)	  pkBin.erase(pkBin.begin());
	  if(valBin.size()>0)     valBin.erase(valBin.begin());
	}
      }
  }

  if( valBin.size()>0 ){
    int curMin = valBin[0];
    // if this is too big, then pkBin[i] overreaches its size
    for (unsigned int i = 1; i<pkBin.size();i++)
      {
	curMin = min(curMin,valBin[i-1]);
	if (pkBin[i]-pkBin[0]<MIN_PEAK_DIST) continue;
	if (hget(htemp,pkBin[i])-base <= slowPeak && slowPeakBin!=fastPeakBin) continue;
	slowPeak = hget(htemp,pkBin[i])-base;
	slowPeakBin = pkBin[i];
	slowPeakTime = htemp->GetXaxis()->GetBinCenter(pkBin[i]);
	trough = hget(htemp,curMin)-base;
	troughBin = curMin;
	troughTime = htemp->GetXaxis()->GetBinCenter(curMin);
      }
  }

  for (int i = 0; i < 6; i++)
  {
    riseV[i] = rf[i] * fastPeak + base;
    fallV[i] = rf[5-i] * slowPeak + base;
  }

  dmtpc::waveform::analysis::riseTime(htemp,riseV,rise,max(startTime-binWidth,htemp->GetXaxis()->GetXmin()),fastPeakTime,true);
  dmtpc::waveform::analysis::fallTime(htemp,fallV,fall,slowPeakTime,min(endTime+binWidth,htemp->GetXaxis()->GetXmax()-binWidth*0.001),false);


  for (int i = 0; i<6; i++)
  {
    
    rise[i] = fastPeakTime - rise[i];
    fall[i] = fall[i] - slowPeakTime;
  }

  p.setFastRise(&rise[0]);
  p.setSlowFall(&fall[0]);
  p.setFastPeak(fastPeak);
  p.setSlowPeak(slowPeak);
  p.setTroughHeight(trough);
  p.setFastPeakTime(fastPeakTime);
  p.setSlowPeakTime(slowPeakTime);
  p.setTroughTime(troughTime);
  p.setFastPeakBin(fastPeakBin);
  p.setSlowPeakBin(slowPeakBin);
  p.setTroughBin(troughBin);
  wf.add(p);

  delete htemp;
}
