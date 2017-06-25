#include "TF1.h"
#include "GainTree.hh"
#include <stdint.h>
#include <math.h>

ClassImp(dmtpc::analysis::GainTree);

dmtpc::analysis::GainTree::GainTree()
{
  initHistos();
  BaseMean.clear();
  BaseSigma.clear();
  PeakBinMean.clear();
  PeakBinSigma.clear();
  StartBinMean.clear();
  StartBinSigma.clear();
  PeakStartDiffMean.clear();
  PeakStartDiffSigma.clear();
}

dmtpc::analysis::GainTree::~GainTree()
{
}


bool dmtpc::analysis::GainTree::fillWfHistos(const dmtpc::waveform::CspWaveform& wf, int ch, bool withCuts)
{
  double baseline = wf.getBase();
  double rms = wf.getRMS();

  if (withCuts)
    {
      //if ((fabs(baseline-BaseMean[ch])<3.*BaseSigma[ch]) && (fabs(rms-RmsMean[ch])<3.*RmsSigma[ch]))  
      if ((fabs(baseline-0.145)<(3.*1.161)) && (rms > 0.2) && (rms < 0.5))
	{ 
	  vhBase_cuts[ch]->Fill(baseline);
	  vhRMS_cuts[ch]->Fill(rms);

	  return true;
	}
    }
  else 
    {
      hBase->Fill(baseline);
      hRMS->Fill(rms);

      vhBase[ch]->Fill(baseline);
      vhRMS[ch]->Fill(rms);
    }

  return false;
}

bool dmtpc::analysis::GainTree::fillPulseHistos(dmtpc::waveform::CspPulse& wfp, int ch, bool withCuts)
{
  double peak = wfp.getPeak();
  double peakbin = wfp.getPeakBin();
  double startbin = wfp.getStartBin();
  double endbin = wfp.getEndBin();
  double integral = wfp.getIntegral();
  double diff = peakbin-startbin;
  double R10 = wfp.getRise10();
  double R25 = wfp.getRise25();
  double R50 = wfp.getRise50();
  double R75 = wfp.getRise75();
  double R90 = wfp.getRise90();
  double F10 = wfp.getFall10();
  double F25 = wfp.getFall25();
  double F50 = wfp.getFall50();
  double F75 = wfp.getFall75();
  double F90 = wfp.getFall90();
  double integralOverR10 = (R10==0.) ? integral/1.E-6 : integral/R10; 
  
  if (withCuts)
    {                          

      if (// was 2. for Fe-55
	  // (fabs(diff-PeakStartDiffMean[ch])<3.*PeakStartDiffSigma[ch]) && used for Fe-55
	  //(fabs(peakbin-PeakBinMean[ch])>3.*PeakBinSigma[ch]) && 
	  (peakbin>3000.) && (peakbin<9500)) // was 49000 for Fe-55 
	{
	  hPeak_nobias->Fill(peak);
          hPeakBin_nobias->Fill(peakbin);
          hStartBin_nobias->Fill(startbin);
          hEndBin_nobias->Fill(endbin);
          hIntegral_nobias->Fill(integral);
          hPeakStartDiff_nobias->Fill(diff);
	  hIntegralOverR10_nobias->Fill(integralOverR10);

	  vhPeak_nobias[ch]->Fill(peak);
          vhPeakBin_nobias[ch]->Fill(peakbin);
          vhStartBin_nobias[ch]->Fill(startbin);
          vhEndBin_nobias[ch]->Fill(endbin);
          vhIntegral_nobias[ch]->Fill(integral);
          vhPeakStartDiff_nobias[ch]->Fill(diff);
	  vhIntegralOverR10_nobias[ch]->Fill(integralOverR10);
	} 

      //if ((peak > 1.) && (fabs(peakbin-PeakBinMean[ch])<3.*PeakBinSigma[ch]) && (peakbin>2000.) && (endbin>9500))// && (endbin>49000)
      if ((peakbin>2000.) && (peakbin<4100.) && (endbin>9380)) // && (fabs(R10-3178.) < (3.*387)))
	{
	  hPeak_cuts->Fill(peak);
	  hPeakBin_cuts->Fill(peakbin);
	  hStartBin_cuts->Fill(startbin);
	  hEndBin_cuts->Fill(endbin);
	  hIntegral_cuts->Fill(integral);
	  hPeakStartDiff_cuts->Fill(diff);
	  hIntegralOverR10_cuts->Fill(integralOverR10);
	  hR10_cuts->Fill(R10);
	  hR25_cuts->Fill(R25);
	  hR50_cuts->Fill(R50);
	  hR75_cuts->Fill(R75);
	  hR90_cuts->Fill(R90);
	  hF10_cuts->Fill(F10);
	  hF25_cuts->Fill(F25);
	  hF50_cuts->Fill(F50);
	  hF75_cuts->Fill(F75);
	  hF90_cuts->Fill(F90);
	
	  hPeakBin_Peak_cuts->Fill(peakbin,peak);

	  vhPeak_cuts[ch]->Fill(peak);
	  vhPeakBin_cuts[ch]->Fill(peakbin);
	  vhStartBin_cuts[ch]->Fill(startbin);
	  vhEndBin_cuts[ch]->Fill(endbin);
	  vhIntegral_cuts[ch]->Fill(integral);
	  vhPeakStartDiff_cuts[ch]->Fill(diff);
	  vhIntegralOverR10_cuts[ch]->Fill(integralOverR10);
	  vhR10_cuts[ch]->Fill(R10);
	  vhR25_cuts[ch]->Fill(R25);
	  vhR50_cuts[ch]->Fill(R50);
	  vhR75_cuts[ch]->Fill(R75);
	  vhR90_cuts[ch]->Fill(R90);
	  vhF10_cuts[ch]->Fill(F10);
	  vhF25_cuts[ch]->Fill(F25);
	  vhF50_cuts[ch]->Fill(F50);
	  vhF75_cuts[ch]->Fill(F75);
	  vhF90_cuts[ch]->Fill(F90);

	  return true;
	}
    }
  else 
    {
      hPeak->Fill(peak);
      hPeakBin->Fill(peakbin);
      hStartBin->Fill(startbin);
      hEndBin->Fill(endbin);
      hIntegral->Fill(integral);
      hPeakStartDiff->Fill(diff);
      hIntegralOverR10->Fill(integralOverR10);
      hR10->Fill(R10);
      hR25->Fill(R25);
      hR50->Fill(R50);
      hR75->Fill(R75);
      hR90->Fill(R90);
      hF10->Fill(F10);
      hF25->Fill(F25);
      hF50->Fill(F50);
      hF75->Fill(F75);
      hF90->Fill(F90);
      
      hPeakBin_Peak->Fill(peakbin,peak);
      
      vhPeak[ch]->Fill(peak);
      vhPeakBin[ch]->Fill(peakbin);
      vhStartBin[ch]->Fill(startbin);
      vhEndBin[ch]->Fill(endbin);
      vhIntegral[ch]->Fill(integral);
      vhPeakStartDiff[ch]->Fill(diff);
      vhIntegralOverR10[ch]->Fill(integralOverR10);
      vhR10[ch]->Fill(R10);
      vhR25[ch]->Fill(R25);
      vhR50[ch]->Fill(R50);
      vhR75[ch]->Fill(R75);
      vhR90[ch]->Fill(R90);
      vhF10[ch]->Fill(F10);
      vhF25[ch]->Fill(F25);
      vhF50[ch]->Fill(F50);
      vhF75[ch]->Fill(F75);
      vhF90[ch]->Fill(F90);
      
    }
  
  return false;
}

bool dmtpc::analysis::GainTree::fillXtalkHistos(dmtpc::waveform::CspPulse& wfp, dmtpc::waveform::CspPulse& wfp2,
						unsigned int ch1, unsigned int ch2,
						double deltaTwf = 0., bool withCuts=false)
{
  double peak1 = wfp.getPeak();
  double peak2 = wfp2.getPeak();

  double peakbin1 = wfp.getPeakBin();
  double peakbin2 = wfp2.getPeakBin();
  
  double startbin1 = wfp.getStartBin();
  double startbin2 = wfp2.getStartBin();

  double ratio = peak2/peak1;
  double deltaT = deltaTwf + dmtpc::analysis::getTimeDiff(wfp,wfp2); 
  if (withCuts)
    {
      //hXtalk_Peak_Peak_cuts->Fill(peak1,peak2);
      //hXtalk_DeltaT_PeakRatio_cuts->Fill(deltaT,ratio);
      //ak
      //std::cout << "inside fillxtalkhistos: deltaT = " << deltaT << ",ratio = " << ratio << std::endl;

      
      if ((deltaT > 0.) && (deltaT < 4.) && (ratio < 2.)) {
	//std::cout << "xtalk: deltaT = " << deltaT << ", ratio = " << ratio << ", peak1 = " << peak1 << ", peak2 = " << peak2 << std::endl;
	return true; 
      }
      /*
      if ((deltaT > 0.) && (deltaT < 120.) && (ratio < 0.2)) {
	//std::cout <<"xtalk: deltaT = " << deltaT << ", ratio = " << ratio << ", peak1 = " << peak1 << ", peak2 = " << peak2<< std::endl;
	return true;
      }
      if ((deltaT > 0.) && (deltaT < 500.) && (ratio < 0.1)) {
	//std::cout <<"xtalk: deltaT = " << deltaT << ", ratio = " << ratio << ", peak1 = " << peak1 << ", peak2 = " << peak2<< std::endl;
	return true;
      }
      */

      if (1) 
	//(fabs(peakbin1-PeakBinMean[ch1])<3.*PeakBinSigma[ch1]) && 
	// (fabs(peakbin2-PeakBinMean[ch2])<3.*PeakBinSigma[ch2]) && 
	// (fabs(startbin1-StartBinMean[ch1])<3.*StartBinSigma[ch1]) && 
	//1(fabs(startbin2-StartBinMean[ch2])<3.*StartBinSigma[ch2]))
	{
	  /*
	  if ((deltaT > -5.) && (deltaT < 5.)) {
	    std::cout << "xtalk: deltaT = " << deltaT << ", ratio = " << ratio << ", peak1 = " << peak1 << ", peak2 = " << peak2 << std::endl;
	    return true;
	  }
	  */
	  hXtalk_Peak_Peak_cuts->Fill(peak1,peak2);
	  hXtalk_DeltaT_PeakRatio_cuts->Fill(deltaT,ratio);   
	}
    }
    else {

      hXtalk_Peak_Peak->Fill(peak1,peak2);
      hXtalk_DeltaT_PeakRatio->Fill(deltaT,ratio);
    }
  return false;
}


void dmtpc::analysis::GainTree::fitBaseline()
{
  BaseMean.clear();
  BaseSigma.clear();

  if (vhBase.size()==0) std::cout << "ERROR: vhBase not initialized!" << std::endl;

  for (unsigned i=0; i<MAX_NCH; i++)
    {
      if (vhBase[i]->GetEntries()>0)
        {
	  std::pair<double,double> pars = fitGaussian(vhBase[i], -8., 8.);
          BaseMean.push_back(pars.first);
          BaseSigma.push_back(pars.second);
        }
      else
        {
          BaseMean.push_back(0.);
          BaseSigma.push_back(0.);
        }
    }
}


void dmtpc::analysis::GainTree::fitRMS()
{
  RmsMean.clear();
  RmsSigma.clear();

  if (vhRMS.size()==0) std::cout << "ERROR: vhRMS not initialized!" << std::endl;

  for (unsigned i=0; i<MAX_NCH; i++)
    {
      if (vhRMS[i]->GetEntries()>0)
        {
	  std::pair<double,double> pars = fitGaussian(vhRMS[i], 0.2, 0.6);
          RmsMean.push_back(pars.first);
          RmsSigma.push_back(pars.second);
        }
      else
        {
          RmsMean.push_back(0.);
          RmsSigma.push_back(0.);
        }
    }
}


void dmtpc::analysis::GainTree::fitPeakBin()
{
  PeakBinMean.clear();
  PeakBinSigma.clear();
  
  if (vhPeakBin.size()==0) std::cout << "ERROR: vhPeakBin not initialized!" << std::endl;

  for (unsigned i=0; i<MAX_NCH; i++)
    {
      if (vhPeakBin[i]->GetEntries()>0)
	{
	  //std::pair<double,double> pars = fitGaussian(vhPeakBin[i], 7000.,10000.); // used for Fe-55 analysis
	  std::pair<double,double> pars = fitGaussian(vhPeakBin[i], 1000., 3000.);
	  PeakBinMean.push_back(pars.first);
	  PeakBinSigma.push_back(pars.second);
	}
      else
	{
	  PeakBinMean.push_back(0.);
	  PeakBinSigma.push_back(0.);
	}
    }
}

void dmtpc::analysis::GainTree::fitStartBin()
{
  StartBinMean.clear();
  StartBinSigma.clear();

  if (vhStartBin.size()==0) std::cout << "ERROR: vhStartBin not initialized!" << std::endl;
  if (PeakBinMean.size()==0) std::cout << "ERROR: PeakBinMean not yet calculated!" << std::endl;

  for (unsigned i=0; i<MAX_NCH; i++)
    {
      if (vhStartBin[i]->GetEntries()>0)
        {
	  //	  std::pair<double,double> pars = fitGaussian(vhStartBin[i],5000.,9000.); // used for Fe-55 analysis
	  std::pair<double,double> pars = fitGaussian(vhStartBin[i], 1000., 3000.);
	  StartBinMean.push_back(pars.first);
          StartBinSigma.push_back(pars.second);
        }
      else
        {
          StartBinMean.push_back(0.);
          StartBinSigma.push_back(0.);
	}
    }
}

void dmtpc::analysis::GainTree::fitPeakStartDiff()
{
  PeakStartDiffMean.clear();
  PeakStartDiffSigma.clear();

  if (vhPeakStartDiff.size()==0) std::cout << "ERROR: vhPeakStartDiff not initialized!" << std::endl;
  if (PeakBinMean.size()==0) std::cout << "ERROR: PeakBinMean not yet calculated!" << std::endl;

  for (unsigned i=0; i<MAX_NCH; i++)
    {
      if (vhPeakStartDiff[i]->GetEntries()>0)
        {
	  std::pair<double,double> pars = fitGaussian(vhPeakStartDiff[i],1000.,3000.);
          PeakStartDiffMean.push_back(pars.first);
          PeakStartDiffSigma.push_back(pars.second);
        }
      else
        {
          PeakStartDiffMean.push_back(0.);
          PeakStartDiffSigma.push_back(0.);
        }
    }
}

std::pair<double,double> dmtpc::analysis::GainTree::fitGaussian(TH1* h, double rangeMin=-1., double rangeMax=-1.)
{
  int maxbin=h->GetMaximumBin();
  if (rangeMin==rangeMax) h->Fit("gaus","","",h->GetBinCenter(maxbin-(int)rangeMin),h->GetBinCenter(maxbin+(int)rangeMin));
  else h->Fit("gaus","","",rangeMin,rangeMax);
  TF1* myfunc = h->GetFunction("gaus");
  if (myfunc)
    {
      std::cout << "Gaussian fit for " << h->GetName() << ": " << myfunc->GetParameter(1) <<
	" +/-" << myfunc->GetParameter(2) << std::endl;
      return std::make_pair(myfunc->GetParameter(1),myfunc->GetParameter(2));
    }
  else return std::make_pair(0.0,9.9E9);
}

void dmtpc::analysis::GainTree::printPeakInfo()
{
  std::cout << "----------------------------------" << std::endl;
  for (unsigned i=0; i<MAX_NCH; i++)
    {
      std::cout << "Channel " << i << " maximum: " <<
        vhPeak_cuts[i]->GetBinCenter(vhPeak_cuts[i]->GetMaximumBin()) << std::endl;
    }
  std::cout << "----------------------------------" << std::endl;
}


void dmtpc::analysis::GainTree::cloneHisto(TH1* histo, std::vector<TH1*>& vector)
{
  TString hName = histo->GetName();
  for (unsigned i=0; i<MAX_NCH; i++)
    {
      vector.push_back((TH1*)histo->Clone(hName+Form("_ch%i",i)));
      vector[i]->SetTitle(hName+Form("_ch%i",i));
    }
}

void dmtpc::analysis::GainTree::initHistos()
{
  hPeak = new TH1D("hPeak", "hPeak", nbins_peak, min_peak, max_peak);
  hPeak->Sumw2();
  cloneHisto(hPeak, vhPeak);
  hPeak_cuts = (TH1D*)hPeak->Clone("hPeak_cuts"); hPeak_cuts->SetTitle("hPeak_cuts");
  hPeak_cuts->Sumw2();
  cloneHisto(hPeak_cuts, vhPeak_cuts);
  hPeak_nobias = (TH1D*)hPeak->Clone("hPeak_nobias"); hPeak_nobias->SetTitle("hPeak_nobias");
  hPeak_nobias->Sumw2();
  cloneHisto(hPeak_nobias, vhPeak_nobias);

  hRMS = new TH1D("hRMS", "hRMS", nbins_rms, min_rms, max_rms);
  cloneHisto(hRMS, vhRMS);
  hRMS_cuts = (TH1D*)hRMS->Clone("hRMS_cuts"); hRMS_cuts->SetTitle("hRMS_cuts");
  cloneHisto(hRMS_cuts, vhRMS_cuts);
  hRMS_nobias = (TH1D*)hRMS->Clone("hRMS_nobias"); hRMS_nobias->SetTitle("hRMS_nobias");
  cloneHisto(hRMS_nobias, vhRMS_nobias);

  hBase = new TH1D("hBase", "hBase", nbins_base, min_base, max_base);
  cloneHisto(hBase, vhBase);
  hBase_cuts = (TH1D*)hBase->Clone("hBase_cuts"); hBase_cuts->SetTitle("hBase_cuts");
  cloneHisto(hBase_cuts, vhBase_cuts);
  hBase_nobias = (TH1D*)hBase->Clone("hBase_nobias"); hBase_nobias->SetTitle("hBase_nobias");
  cloneHisto(hBase_nobias, vhBase_nobias);

  hPeakBin = new TH1D("hPeakBin", "hPeakBin", nbins_trace, min_trace, max_trace);
  cloneHisto(hPeakBin, vhPeakBin);
  hPeakBin_cuts = (TH1D*)hPeakBin->Clone("hPeakBin_cuts"); hPeakBin_cuts->SetTitle("hPeakBin_cuts");
  cloneHisto(hPeakBin_cuts, vhPeakBin_cuts);
  hPeakBin_nobias = (TH1D*)hPeakBin->Clone("hPeakBin_nobias"); hPeakBin_nobias->SetTitle("hPeakBin_nobias");
  cloneHisto(hPeakBin_nobias, vhPeakBin_nobias);

  hStartBin = new TH1D("hStartBin", "hStartBin", nbins_trace, min_trace, max_trace);
  cloneHisto(hStartBin, vhStartBin);
  hStartBin_cuts = (TH1D*)hStartBin->Clone("hStartBin_cuts"); hStartBin_cuts->SetTitle("hStartBin_cuts");
  cloneHisto(hStartBin_cuts, vhStartBin_cuts);
  hStartBin_nobias = (TH1D*)hStartBin->Clone("hStartBin_nobias"); hStartBin_nobias->SetTitle("hStartBin_nobias");
  cloneHisto(hStartBin_nobias, vhStartBin_nobias);

  hEndBin = new TH1D("hEndBin", "hEndBin", nbins_trace, min_trace, max_trace);
  cloneHisto(hEndBin, vhEndBin);
  hEndBin_cuts = (TH1D*)hEndBin->Clone("hEndBin_cuts"); hEndBin_cuts->SetTitle("hEndBin_cuts");
  cloneHisto(hEndBin_cuts, vhEndBin_cuts);
  hEndBin_nobias = (TH1D*)hEndBin->Clone("hEndBin_nobias"); hEndBin_nobias->SetTitle("hEndBin_nobias");
  cloneHisto(hEndBin_nobias, vhEndBin_nobias);

  hIntegral = new TH1D("hIntegral", "hIntegral", nbins_integral, min_integral, max_integral);
  cloneHisto(hIntegral, vhIntegral);
  hIntegral_cuts = (TH1D*)hIntegral->Clone("hIntegral_cuts"); hIntegral_cuts->SetTitle("hIntegral_cuts");
  cloneHisto(hIntegral_cuts, vhIntegral_cuts);
  hIntegral_nobias = (TH1D*)hIntegral->Clone("hIntegral_nobias"); hIntegral_nobias->SetTitle("hIntegral_nobias");
  cloneHisto(hIntegral_nobias, vhIntegral_nobias);

  hPeakStartDiff = new TH1D("hPeakStartDiff", "hPeakStartDiff", nbins_trace, min_trace, max_trace);
  cloneHisto(hPeakStartDiff, vhPeakStartDiff);
  hPeakStartDiff_cuts = (TH1D*)hPeakStartDiff->Clone("hPeakStartDiff_cuts"); hPeakStartDiff_cuts->SetTitle("hPeakStartDiff_cuts");
  cloneHisto(hPeakStartDiff_cuts, vhPeakStartDiff_cuts);
  hPeakStartDiff_nobias = (TH1D*)hPeakStartDiff->Clone("hPeakStartDiff_nobias"); hPeakStartDiff_nobias->SetTitle("hPeakStartDiff_nobias");
  cloneHisto(hPeakStartDiff_nobias, vhPeakStartDiff_nobias);

  hIntegralOverR10 = new TH1D("hIntegralOverR10", "hIntegralOverR10", nbins_integral, min_integral/1000., max_integral/1000.);
  cloneHisto(hIntegralOverR10, vhIntegralOverR10);
  hIntegralOverR10_cuts = (TH1D*)hIntegralOverR10->Clone("hIntegralOverR10_cuts"); hIntegralOverR10_cuts->SetTitle("hIntegralOverR10_cuts");
  cloneHisto(hIntegralOverR10_cuts, vhIntegralOverR10_cuts);
  hIntegralOverR10_nobias = (TH1D*)hIntegralOverR10->Clone("hIntegralOverR10_nobias"); hIntegralOverR10_nobias->SetTitle("hIntegralOverR10_nobias");
  cloneHisto(hIntegralOverR10_nobias, vhIntegralOverR10_nobias);

  hR10 = new TH1D("hR10", "hR10", nbins_trace, min_trace, max_trace);
  cloneHisto(hR10, vhR10);
  hR10_cuts = (TH1D*)hR10->Clone("hR10_cuts"); hR10_cuts->SetTitle("hR10_cuts");
  cloneHisto(hR10_cuts, vhR10_cuts);

  hR25 = new TH1D("hR25", "hR25", nbins_trace, min_trace, max_trace);
  cloneHisto(hR25, vhR25);
  hR25_cuts = (TH1D*)hR25->Clone("hR25_cuts"); hR25_cuts->SetTitle("hR25_cuts");
  cloneHisto(hR25_cuts, vhR25_cuts);

  hR50 = new TH1D("hR50", "hR50", nbins_trace, min_trace, max_trace);
  cloneHisto(hR50, vhR50);
  hR50_cuts = (TH1D*)hR50->Clone("hR50_cuts"); hR50_cuts->SetTitle("hR50_cuts");
  cloneHisto(hR50_cuts, vhR50_cuts);

  hR75 = new TH1D("hR75", "hR75", nbins_trace, min_trace, max_trace);
  cloneHisto(hR75, vhR75);
  hR75_cuts = (TH1D*)hR75->Clone("hR75_cuts"); hR75_cuts->SetTitle("hR75_cuts");
  cloneHisto(hR75_cuts, vhR75_cuts);

  hR90 = new TH1D("hR90", "hR90", nbins_trace, min_trace, max_trace);
  cloneHisto(hR90, vhR90);
  hR90_cuts = (TH1D*)hR90->Clone("hR90_cuts"); hR90_cuts->SetTitle("hR90_cuts");
  cloneHisto(hR90_cuts, vhR90_cuts);

  hF10 = new TH1D("hF10", "hF10", nbins_trace, min_trace, max_trace);
  cloneHisto(hF10, vhF10);
  hF10_cuts = (TH1D*)hF10->Clone("hF10_cuts"); hF10_cuts->SetTitle("hF10_cuts");
  cloneHisto(hF10_cuts, vhF10_cuts);

  hF25 = new TH1D("hF25", "hF25", nbins_trace, min_trace, max_trace);
  cloneHisto(hF25, vhF25);
  hF25_cuts = (TH1D*)hF25->Clone("hF25_cuts"); hF25_cuts->SetTitle("hF25_cuts");
  cloneHisto(hF25_cuts, vhF25_cuts);

  hF50 = new TH1D("hF50", "hF50", nbins_trace, min_trace, max_trace);
  cloneHisto(hF50, vhF50);
  hF50_cuts = (TH1D*)hF50->Clone("hF50_cuts"); hF50_cuts->SetTitle("hF50_cuts");
  cloneHisto(hF50_cuts, vhF50_cuts);

  hF75 = new TH1D("hF75", "hF75", nbins_trace, min_trace, max_trace);
  cloneHisto(hF75, vhF75);
  hF75_cuts = (TH1D*)hF75->Clone("hF75_cuts"); hF75_cuts->SetTitle("hF75_cuts");
  cloneHisto(hF75_cuts, vhF75_cuts);

  hF90 = new TH1D("hF90", "hF90", nbins_trace, min_trace, max_trace);
  cloneHisto(hF90, vhF90);
  hF90_cuts = (TH1D*)hF90->Clone("hF90_cuts"); hF90_cuts->SetTitle("hF90_cuts");
  cloneHisto(hF90_cuts, vhF90_cuts);


  hPeakBin_Peak = new TH2F("hPeakBin_Peak", "hPeakBin_Peak",
			   nbins_trace, min_trace, max_trace, nbins_peak, min_peak, max_peak);
  hPeakBin_Peak_cuts = (TH2F*)hPeakBin_Peak->Clone("hPeakBin_Peak_cuts");
  hPeakBin_Peak_cuts->SetTitle("hPeakBin_Peak_cuts");

  hXtalk_Peak_Peak = new TH2F("hXtalk_Peak_Peak", "hXtalk_Peak_Peak", 
			      nbins_peak, min_peak, max_peak, nbins_peak, min_peak, max_peak);
  hXtalk_Peak_Peak_cuts = (TH2F*)hXtalk_Peak_Peak->Clone("hXtalk_Peak_Peak_cuts");
  hXtalk_Peak_Peak_cuts->SetTitle("hXtalk_Peak_Peak_cuts");

  hXtalk_DeltaT_PeakRatio = new TH2F("hXtalk_DeltaT_PeakRatio", "hXtalk_DeltaT_PeakRatio",
				     nbins_time, min_time, max_time, nbins_peakRatio, min_peakRatio, max_peakRatio);
  hXtalk_DeltaT_PeakRatio_cuts = (TH2F*)hXtalk_DeltaT_PeakRatio->Clone("hXtalk_DeltaT_PeakRatio_cuts");
  hXtalk_DeltaT_PeakRatio_cuts->SetTitle("hXtalk_DeltaT_PeakRatio_cuts");

}


void dmtpc::analysis::GainTree::writeHistos(int run)
{
  TFile *fout=new TFile(Form("/home/mleyton/svn/DmtpcAnalysis/trees/ChargePulses_cuts_run%03d.root",run),"recreate");

  hPeak->Write();
  hRMS->Write();
  hBase->Write();
  hPeakBin->Write();
  hStartBin->Write();
  hEndBin->Write();
  hIntegral->Write();
  hPeakStartDiff->Write();
  hIntegralOverR10->Write();
  hR10->Write();
  hR25->Write();
  hR50->Write();
  hR75->Write();
  hR90->Write();
  hF10->Write();
  hF25->Write();
  hF50->Write();
  hF75->Write();
  hF90->Write();

  hPeak_cuts->Write();
  //hRMS_cuts->Write();
  //hBase_cuts->Write();
  hPeakBin_cuts->Write();
  hStartBin_cuts->Write();
  hEndBin_cuts->Write();
  hIntegral_cuts->Write();
  hPeakStartDiff_cuts->Write();
  hIntegralOverR10_cuts->Write();
  hR10_cuts->Write();
  hR25_cuts->Write();
  hR50_cuts->Write();
  hR75_cuts->Write();
  hR90_cuts->Write();
  hF10_cuts->Write();
  hF25_cuts->Write();
  hF50_cuts->Write();
  hF75_cuts->Write();
  hF90_cuts->Write();

  hPeak_nobias->Write();
  //  hRMS_nobias->Write();                                                               
  //hBase_nobias->Write();                                                                                                                  
  hPeakBin_nobias->Write();
  hStartBin_nobias->Write();
  hEndBin_nobias->Write();
  hIntegral_nobias->Write();
  hPeakStartDiff_nobias->Write();
  hIntegralOverR10_nobias->Write();

  hPeakBin_Peak->Write();
  hPeakBin_Peak_cuts->Write();

  hXtalk_Peak_Peak->Write();
  hXtalk_DeltaT_PeakRatio->Write();
  hXtalk_Peak_Peak_cuts->Write();
  hXtalk_DeltaT_PeakRatio_cuts->Write();

  for (unsigned i=0; i<MAX_NCH; i++)
    {
      vhPeak[i]->Write();
      vhRMS[i]->Write();
      vhBase[i]->Write();
      vhPeakBin[i]->Write();
      vhStartBin[i]->Write();
      vhEndBin[i]->Write();
      vhIntegral[i]->Write();
      vhPeakStartDiff[i]->Write();
      vhIntegralOverR10[i]->Write();
      vhR10[i]->Write();
      vhR25[i]->Write();
      vhR50[i]->Write();
      vhR75[i]->Write();
      vhR90[i]->Write();
      vhF10[i]->Write();
      vhF25[i]->Write();
      vhF50[i]->Write();
      vhF75[i]->Write();
      vhF90[i]->Write();
      
      vhPeak_cuts[i]->Write();
      vhRMS_cuts[i]->Write();
      vhBase_cuts[i]->Write();
      vhPeakBin_cuts[i]->Write();
      vhStartBin_cuts[i]->Write();
      vhEndBin_cuts[i]->Write();
      vhIntegral_cuts[i]->Write();
      vhPeakStartDiff_cuts[i]->Write();
      vhIntegralOverR10_cuts[i]->Write();
      vhR10_cuts[i]->Write();
      vhR25_cuts[i]->Write();
      vhR50_cuts[i]->Write();
      vhR75_cuts[i]->Write();
      vhR90_cuts[i]->Write();
      vhF10_cuts[i]->Write();
      vhF25_cuts[i]->Write();
      vhF50_cuts[i]->Write();
      vhF75_cuts[i]->Write();
      vhF90_cuts[i]->Write();

      vhPeak_nobias[i]->Write();
      vhRMS_nobias[i]->Write();
      vhBase_nobias[i]->Write();
      vhPeakBin_nobias[i]->Write();
      vhStartBin_nobias[i]->Write();
      vhEndBin_nobias[i]->Write();
      vhIntegral_nobias[i]->Write();
      vhPeakStartDiff_nobias[i]->Write();
      vhIntegralOverR10_nobias[i]->Write();
    }

  fout->Close();
}
