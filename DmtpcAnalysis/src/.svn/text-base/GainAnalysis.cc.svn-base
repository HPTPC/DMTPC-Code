//Base classes
#include "WaveformVector.hh"
#include "SkimWaveform.hh"
#include "Pulse.hh"
//Charge sensitive preamp
#include "CspWfVector.hh"
#include "CspWaveform.hh"
#include "CspPulse.hh"
//Fast preamp
#include "FastWfVector.hh"
#include "FastWaveform.hh"
#include "FastPulse.hh"
//PMT
#include "PMTWfVector.hh"
#include "PMTWaveform.hh"
#include "PMTPulse.hh"

#include "SkimDataset.hh"
#include "SkimEvent.hh"
#include "TObjArray.h"
#include <iostream>

#include "GainTree.hh"


void loopOverEvents(dmtpc::analysis::SkimDataset& d, dmtpc::analysis::GainTree& tree, bool firstPass=true)
{
  static const unsigned int N_EV = d.nevents();
  
  for (unsigned int ev=0; ev<N_EV; ev++)
    {
      if (ev%(int)(N_EV/10)==0) std::cout << "Event " << ev << "/" << N_EV << std::endl;
      //Get event
      d.getEvent(ev);
      //Get array of waveform data:
      const vector<dmtpc::waveform::CspWfVector*> * arr = d.event()->cspwf_vectors();
      
      // test if waveform data is saved in this file
      if (!arr || arr->size()==0) {
	std::cout <<"No waveform data found!"<<std::endl;
	break;
      }
      
      const unsigned int N_CH = arr->size();
      //      const unsigned int N_TRIG = ((dmtpc::waveform::CspWfVector*)arr->At(0))->size();
      const unsigned int N_TRIG = arr->at(0)->size();

      // loop over triggers
      for (unsigned t=0; t<N_TRIG; t++)
	{
	  // loop over channels
	  for (unsigned ch=0; ch<N_CH; ch++)
	    {
	      //	      dmtpc::waveform::CspWfVector* wfv = (dmtpc::waveform::CspWfVector*)arr->At(ch);
	      dmtpc::waveform::CspWfVector* wfv = arr->at(ch);

	      int detCh = wfv->getChan();
	      int cam=detCh/2; // mapping to cam 0,1,2,3
	      
	      dmtpc::waveform::CspWaveform wf = wfv->at(t);
	      dmtpc::waveform::CspPulse pulse = wf[0]; // only look at primary (maxbin) pulses
	      if (pulse.getPeak() < 1.) continue;

	      if (firstPass) 
		{
		  tree.fillWfHistos(wf,cam,false);
		  tree.fillPulseHistos(pulse,cam,false);

		  for (unsigned t2=t; t2<N_TRIG; t2++)
		    {
		      for (unsigned ch2=ch+1; ch2<N_CH; ch2++)
			{
			  // dmtpc::waveform::CspWaveform wf2 = ((dmtpc::waveform::CspWfVector*)arr->At(ch2))->at(t2);
			  dmtpc::waveform::CspWaveform wf2 = arr->at(ch2)->at(t2);
			  dmtpc::waveform::CspPulse pulse2 = wf2[0];
			  tree.fillXtalkHistos(pulse,pulse2,ch,ch2,dmtpc::analysis::getTimeDiff(wf,wf2), false);
			}
		    }
		}
	      else
		{
		  bool wfPass = tree.fillWfHistos(wf,cam,true);
		  double peak = pulse.getPeak();

		  double startbin = pulse.getStartBin();
		  double peakbin = pulse.getPeakBin();
		  double diff = peakbin-startbin;

		  // loop over all other pulses in the event                                           
		  if (wfPass)
		    {
		      bool xtalk=false;
		      for (int t2=t; t2>=0; t2--)
			{
			  for (unsigned ch2=0; ch2<N_CH; ch2++)
			    {
			      if (ch==ch2) continue;

			      dmtpc::waveform::CspWaveform wf0 = arr->at(ch2)->at(t2);
			      dmtpc::waveform::CspPulse pulse0 = wf0[0];
			      if ((peak<2.) || (pulse0.getPeak()<2.)) continue;
			      bool xtalk_pair = tree.fillXtalkHistos(pulse0,pulse,ch2,ch,dmtpc::analysis::getTimeDiff(wf0,wf), true);
			      
			      //if (xtalk_pair && ch!=ch2) xtalk=true;
			      if (xtalk_pair) { 
				xtalk=true;

				//cout << "Found xtalk pair with deltaT = " << deltaT << ", peakRatio = " << peakRatio << ", ch==ch2: " << (ch==ch2) << endl;
			      }
			      
			    }
			}
		      if (!xtalk) {
			//if ((fabs(peak-8.) < 0.25) || (fabs(peak-12.) < 0.25) || (fabs(peak-16.) < 0.25)) cout << "peak = " << peak << " at event " << ev << ", trigger = " << t << ", ch = " << ch << endl;
			//			if ((peak>3.) && (diff>3000) && (diff<4200) && (peakbin > 10000.)) cout << "event = " << ev << ", trigger = " << t << ", ch = " << ch << ", peak = " << pulse.getPeak() << ", peakBin = " << pulse.getPeakBin() << ", startBin = " << pulse.getStartBin() << ", R10 = " << pulse.getRise10() << ", R90 = " << pulse.getRise90() << ", baselin = " << wf.getBase() << endl;
			bool pass = tree.fillPulseHistos(pulse,cam,true);
			
			//if (pass & ((fabs(peak-8.) < 0.25) || (fabs(peak-12.) < 0.25) || (fabs(peak-16.) < 0.25))) cout << "peak = " << peak << " at event " << ev << ", trigger = " << t << ", ch = " << ch << endl;
		      }
		    }
		}
	    }
	}
    }
  if (firstPass) {
    tree.fitBaseline();
    tree.fitRMS();
    tree.fitPeakBin();
    tree.fitStartBin();
    tree.fitPeakStartDiff();
  }
}



int main(int argc, char ** argv)
{
  int runNo = (argc > 1) ? atoi(argv[1]) : 381;

  //Open file
  dmtpc::analysis::SkimDataset d;
  //d.openRootFile(Form("/net/zwicky/dmtpc/data/m3/GainMap/skim/dmtpc_m3_gain_%05dskim.root",runNo));
  //d.openRootFile(Form("/net/zwicky/dmtpc/data/m3/AlphaData/skim/dmtpc_m3_alpha_%05dskim.root",runNo));
  d.openRootFile(Form("/scratch1/darkmatter/dmtpc/production/skimout/skim/m3/skim/AlphaData/dmtpc_m3_alpha_%05dskim.root",runNo));
  
  dmtpc::analysis::GainTree myGainTree;
  loopOverEvents(d,myGainTree);

  loopOverEvents(d,myGainTree,false);

  myGainTree.printPeakInfo();

  myGainTree.writeHistos(runNo);

  return 0;
}




