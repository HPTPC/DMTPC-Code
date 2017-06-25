#include "Dataset.hh"
#include "Event.hh"
#include "Waveform.hh"
#include "WaveformAnalysis.hh"
#include "ScopeInfo.hh"
#include "M3GlobalData.hh"
#include "TString.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TF1.h"
#include <iostream>
#include <assert.h>

static const int NBINS=800;
static const int MINMAX=100; // in mV
static const int MAX_NO_SCOPE_CHANNELS=8;
TH1F* hBase[MAX_NO_SCOPE_CHANNELS];
TString chNames[MAX_NO_SCOPE_CHANNELS];
int runNo;

void printHisto(TCanvas* c, TH1F* h, TString chName, int ch)
{
  c->cd();
  TString name = h->GetName();
  double mean=-1.;
  double sigma=-1.;
  int status=1;
  if (h->GetEntries() > 0)
    {
      h->Fit("gaus");
      h->GetXaxis()->SetTitle("Negative residue (mV)");
      //      h->Draw();
      TF1* fitfunc = h->GetFunction("gaus");
      assert(fitfunc);
      mean = -1.*(fitfunc->GetParameter(1));
      sigma = fitfunc->GetParameter(2);
      if (sigma>15.) status=2;
      else status=0;
      h->SetAxisRange(-mean-20.,-mean+50.);
      c->Update();
      h->Draw();
      printf("Residual bias for %s is %f +/- %f\n", name.Data(),mean,sigma); 
      c->Print(Form("/data/2015/post/dc_residue_scope0_ch%d.png",ch));
    }
  // TODO assumes scope0
  gM3Data.mySqlOnlM3->command(Form("insert into dc_residue_scope0_ch%d (runId,value,sigma,status,timeStamp) VALUES (%d,%f,%f,0,NOW())",ch,runNo,mean,sigma,status));
  return;
}

int main(int argc, char ** argv)
{
  assert(argc>2);
  runNo=atoi(argv[1]);
  TString input_filename = TString(argv[2]);
  // the workhorse
  dmtpc::core::Dataset d;


  //Open file                                                 
  printf("Opening dataset in file %s\n",input_filename.Data());
  d.open(input_filename);
  int nEvents = d.nevents();
  if (!nEvents) {
    std::cout << "ERROR: No events found in file!" << std::endl;
    return -1;
  }

  std::cout << "# events: " << nEvents << std::endl;

  // init baseline histos
  for (int i=0; i<MAX_NO_SCOPE_CHANNELS; i++) {
    chNames[i]="";
    TString hName(Form("dc_residue_R%d_scope0_ch%d",runNo,i));
    hBase[i] = new TH1F(hName,hName,NBINS,-1.*MINMAX,MINMAX);
  }
  
  unsigned int totalTraces=0;
  unsigned int totalEventsRead=0;

  for (int nev=0; nev<nEvents; nev++) {
    if (nev>10) continue;
    d.getEvent(nev);
    totalEventsRead++;
    int nScopes = d.event()->nscopes();
    if (nScopes==0) continue;
    //std::cout << "# scopes = " << nScopes << std::endl;
    int nTraces = d.event()->nScopeData();
    totalTraces+=nTraces;
    std::cout << "# traces = " << nTraces << std::endl;
    int nChannels = 0;
    // should iterate until nScopes, but BUG TODO
    for (int nsc=0; nsc<1; nsc++) nChannels += d.event()->getScopeConfig(nsc)->channel_info.size();
    std::cout << "# channels = " << nChannels << std::endl;

    int nTriggers = nTraces/nChannels;
    std::cout << "# triggers = " << nTriggers << std::endl;

    for (int ich=0; ich<nChannels; ich++) {
      for (int tr=0; tr<nTriggers; tr++) {
	if (tr>2000) continue;
	// for now assume only one scope! TODO                                                                                                    
	int chID = d.event()->getScopeConfig(0)->channel_info[ich].channel;
	chNames[chID] = TString(d.event()->getScopeConfig(0)->channel_info[ich].name);
	TH1D* myTrace = (TH1D*)d.event()->getTrace(ich,tr,nChannels)->getPhysical();
	if (myTrace->GetMaximumBin()<600) continue; // triggered on falling pulse
	double rms=0.;
	double base = dmtpc::waveform::analysis::baseline(myTrace,rms,100,600);
	hBase[chID]->Fill(base);
      }
    }
  }

  gStyle->SetOptFit(1);
  gStyle->SetOptStat(1111110);
  gM3Data.mySqlOnlM3=new TinyMySqlInterface("m3slow.lns.mit.edu", "onlM3", "dmatter2","seedark");

  TCanvas *c = new TCanvas("c","c",0,0,400,400);
  for (int i=0; i<MAX_NO_SCOPE_CHANNELS; i++) printHisto(c,hBase[i],chNames[i],i);

  if (totalEventsRead==0) std::cout << "ERROR in CalcScopeBias(): no events read from dataset!" << std::endl;
  if (totalTraces==0) std::cout << "ERROR in CalcScopeBias(): no traces found! Are scopes on/enabled?" << std::endl; 

  std::cout << "# average_multiplicity " << totalTraces/totalEventsRead << std::endl;
  return 0;
}
