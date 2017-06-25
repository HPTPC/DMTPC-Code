#ifndef DMTPC_ANALYSIS_GAIN_TREE_HH
#define DMTPC_ANALYSIS_GAIN_TREE_HH

#include <vector>
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TString.h"

#include "CspWaveform.hh"
#include "CspPulse.hh"


namespace dmtpc
{
  namespace analysis
  {
    
    // returns waveform time in us
    inline double getTime(dmtpc::waveform::CspWaveform &wf)
    { return (1.E6*(double)wf.getTimeSecs() + 1.E-3*(double)wf.getTimeNsecs()); }

    // returns pulse time in us
    inline double getTime(dmtpc::waveform::CspPulse &wfp)
    { return (1.E-3*wfp.getPeakTime()); }

    // returns time difference between two pulses in us
    inline double getTimeDiff(dmtpc::waveform::CspPulse & wfp, dmtpc::waveform::CspPulse &wfp2) 
    { return (getTime(wfp2)-getTime(wfp)); }

    // returns time difference between two waveforms in us
    inline double getTimeDiff(dmtpc::waveform::CspWaveform& wf, dmtpc::waveform::CspWaveform& wf2)
    { return (getTime(wf2)-getTime(wf)); }

    class GainTree
    {
    public:

      static const unsigned int MAX_NCH = 4; // 4 quadrants
      
      static const int nbins_peak = 6400;
      static const int min_peak = 0;
      static const int max_peak = 1600;
      
      static const int nbins_rms = 400;
      static const int min_rms = 0;
      static const int max_rms = 2;
      
      static const int nbins_base = 480;
      static const int min_base = -30;
      static const int max_base = 30;
      
      static const int nbins_trace = 1100;
      static const int min_trace = -1000;
      static const int max_trace = 10000;
      
      static const int nbins_integral = 5500;
      static const int min_integral = -500000;
      static const int max_integral = 5000000;

      static const int nbins_time = 5000;
      static const int min_time = 0;
      static const int max_time = 500;

      static const int nbins_peakRatio = 400;
      static const int min_peakRatio = 0;
      static const int max_peakRatio = 4;
            
      GainTree();
      virtual ~GainTree();
      
      void cloneHisto(TH1* histo, std::vector<TH1*>& vector);
      void initHistos();
      void writeHistos(int run);

      bool fillWfHistos(const dmtpc::waveform::CspWaveform& wf, int ch, bool withCuts);
      bool fillPulseHistos(dmtpc::waveform::CspPulse& wfp, int ch, bool withCuts);
      std::pair<double,double> fitGaussian(TH1* h, double rangeMin, double rangeMax);
      bool fillXtalkHistos(dmtpc::waveform::CspPulse& wfp, dmtpc::waveform::CspPulse& wfp2, 
			   unsigned int ch1, unsigned int ch2, double deltaTwf, bool withCuts);

      void fitBaseline();
      void fitRMS();
      void fitPeakBin();
      void fitStartBin();
      void fitPeakStartDiff();
      void printPeakInfo();

      std::vector<double> BaseMean, BaseSigma, RmsMean, RmsSigma, PeakBinMean, PeakBinSigma, 
	StartBinMean, StartBinSigma, PeakStartDiffMean, PeakStartDiffSigma;

      TH1D* hPeak;
      TH1D* hRMS;
      TH1D* hBase;
      TH1D* hPeakBin;
      TH1D* hStartBin;
      TH1D* hEndBin;
      TH1D* hIntegral;
      TH1D* hPeakStartDiff;
      TH1D *hIntegralOverR10;
      TH1D* hR10;
      TH1D* hR25;
      TH1D* hR50;
      TH1D* hR75;
      TH1D* hR90;
      TH1D* hF10;
      TH1D* hF25;
      TH1D* hF50;
      TH1D* hF75;
      TH1D* hF90;
      
      TH1D* hPeak_cuts;
      TH1D* hRMS_cuts;
      TH1D* hBase_cuts;
      TH1D* hPeakBin_cuts;
      TH1D* hStartBin_cuts;
      TH1D* hEndBin_cuts;
      TH1D* hIntegral_cuts;
      TH1D* hPeakStartDiff_cuts;
      TH1D* hIntegralOverR10_cuts;
      TH1D* hR10_cuts;
      TH1D* hR25_cuts;
      TH1D* hR50_cuts;
      TH1D* hR75_cuts;
      TH1D* hR90_cuts;
      TH1D* hF10_cuts;
      TH1D* hF25_cuts;
      TH1D* hF50_cuts;
      TH1D* hF75_cuts;
      TH1D* hF90_cuts;
      
      TH1D* hPeak_nobias;
      TH1D* hBase_nobias;
      TH1D* hRMS_nobias;
      TH1D* hPeakBin_nobias;
      TH1D* hStartBin_nobias;
      TH1D* hEndBin_nobias;
      TH1D* hIntegral_nobias;
      TH1D* hPeakStartDiff_nobias;
      TH1D* hIntegralOverR10_nobias;

      std::vector<TH1*> vhPeak;
      std::vector<TH1*> vhRMS;
      std::vector<TH1*> vhBase;
      std::vector<TH1*> vhPeakBin;
      std::vector<TH1*> vhStartBin;
      std::vector<TH1*> vhEndBin;
      std::vector<TH1*> vhIntegral;
      std::vector<TH1*> vhPeakStartDiff;
      std::vector<TH1*> vhIntegralOverR10;
      std::vector<TH1*> vhR10;
      std::vector<TH1*> vhR25;
      std::vector<TH1*> vhR50;
      std::vector<TH1*> vhR75;
      std::vector<TH1*> vhR90;
      std::vector<TH1*> vhF10;
      std::vector<TH1*> vhF25;
      std::vector<TH1*> vhF50;
      std::vector<TH1*> vhF75;
      std::vector<TH1*> vhF90;
      
      std::vector<TH1*> vhPeak_cuts;
      std::vector<TH1*> vhRMS_cuts;
      std::vector<TH1*> vhBase_cuts;
      std::vector<TH1*> vhPeakBin_cuts;
      std::vector<TH1*> vhStartBin_cuts;
      std::vector<TH1*> vhEndBin_cuts;
      std::vector<TH1*> vhIntegral_cuts;
      std::vector<TH1*> vhPeakStartDiff_cuts;
      std::vector<TH1*> vhIntegralOverR10_cuts;
      std::vector<TH1*> vhR10_cuts;
      std::vector<TH1*> vhR25_cuts;
      std::vector<TH1*> vhR50_cuts;
      std::vector<TH1*> vhR75_cuts;
      std::vector<TH1*> vhR90_cuts;
      std::vector<TH1*> vhF10_cuts;
      std::vector<TH1*> vhF25_cuts;
      std::vector<TH1*> vhF50_cuts;
      std::vector<TH1*> vhF75_cuts;
      std::vector<TH1*> vhF90_cuts;

      std::vector<TH1*> vhPeak_nobias;
      std::vector<TH1*> vhBase_nobias;
      std::vector<TH1*> vhRMS_nobias;
      std::vector<TH1*> vhPeakBin_nobias;
      std::vector<TH1*> vhStartBin_nobias;
      std::vector<TH1*> vhEndBin_nobias;
      std::vector<TH1*> vhIntegral_nobias;
      std::vector<TH1*> vhPeakStartDiff_nobias;
      std::vector<TH1*> vhIntegralOverR10_nobias;

      TH2F* hPeakBin_Peak;
      TH2F* hPeakBin_Peak_cuts;
   
      TH2F* hXtalk_Peak_Peak;
      TH2F* hXtalk_Peak_Peak_cuts;
      TH2F* hXtalk_DeltaT_PeakRatio;
      TH2F* hXtalk_DeltaT_PeakRatio_cuts;

      ClassDef(GainTree,1);
    };
  }
}

      
#endif

