#include "TH1.h"
#include "TFile.h"
#include "TF1.h"
#include "TStyle.h" 
#include "TCanvas.h"
#include "TTree.h"
#include <cstdio>


const double level_2_volt = 2./4096; 


void printUsage()
{
  printf("Usage: makeTrees in.root out.root [fitdir]\n"); 
}

int main(int nargs, char ** args)
{

  gStyle->SetOptStat(0); 
  gStyle->SetOptFit(1); 
  if (nargs <3)
  {
    printUsage(); 
    return 0; 
  }

  char * fitdir = nargs > 3 ? args[3] : 0; 

  TFile in(args[1]); 

  TTree * intree = (TTree*) in.Get("traces"); 
  TH1S * hist = 0; 
  int timestamp; 
  int offset; 

  intree->SetBranchAddress("waveform",&hist); 
  intree->SetBranchAddress("timestamp",&timestamp); 
  intree->SetBranchAddress("offset",&offset); 


  TFile out(args[2],"recreate"); 

  TTree * outtree = new TTree("analysis","analysis"); 

  double risetime; 
  double height; 
  double heightError; 
  double baseline; 
  double baselineError; 
  double start; 
  double startError; 
  double rc; 
  int status; 
  bool rails; 
  int n_since_last_rail; 
  double chisq; 
  int ndof; 

  outtree->Branch("risetime",&risetime); 
  outtree->Branch("height",&height); 
  outtree->Branch("baseline",&baseline); 
  outtree->Branch("start",&start); 
  outtree->Branch("heightError",&heightError); 
  outtree->Branch("baselineError",&baselineError); 
  outtree->Branch("startError",&startError); 
  outtree->Branch("rc",&rc); 
  outtree->Branch("rails",&rails); 
  outtree->Branch("chisq",&chisq); 
  outtree->Branch("ndof",&ndof); 
  outtree->Branch("status",&status); 
  outtree->Branch("n_since_last_rail",&n_since_last_rail); 


  intree->GetEntry(0); 

  n_since_last_rail = 0; 


  for (int i = 0; i < intree->GetEntries(); i++)
  {
    intree->GetEntry(i); 
    TF1 fitfunc("fitfunc","[4] +  [1] * exp(-(x-[0])/[3]) *  0.5 * (1 + TMath::Erf((x-[0]-[2])/[2]))",hist->GetXaxis()->GetXmin(),hist->GetXaxis()->GetXmax()); 
    double baseline_guess = hist->Integral(1,500)/500; 
    fitfunc.SetParameter(0,hist->GetXaxis()->GetBinCenter(hist->GetMaximumBin())- 1e-6); 
    fitfunc.SetParError(0,1e-6); 
    fitfunc.SetParameter(1,hist->GetMaximum() - baseline_guess); 
    fitfunc.SetParameter(2,5e-7); 
    fitfunc.SetParError(2,1e-7); 
    fitfunc.SetParameter(3,5.2e-5); 
    fitfunc.SetParError(3,1e-7); 
    fitfunc.SetParameter(4,baseline_guess); 
    fitfunc.SetParError(4, 20); 
    status = hist->Fit(&fitfunc,"M"); 


    if (fitdir)
    {
      TCanvas c("c","c",800,600); 
      c.cd(); 
      hist->Draw(); 
      fitfunc.Draw("same"); 
      c.SaveAs(TString::Format("%s/%d.png",fitdir,i)); 
    }

    risetime = fitfunc.GetParameter(2); 
    height = fitfunc.GetParameter(1) * level_2_volt; 
    heightError = fitfunc.GetParError(1) * level_2_volt; 
    baseline = (fitfunc.GetParameter(4) - offset)* level_2_volt; 
    baselineError = fitfunc.GetParError(4) * level_2_volt; 
    start = fitfunc.GetParameter(0); 
    startError = fitfunc.GetParError(0); 
    rc = fitfunc.GetParameter(3); 
    chisq = fitfunc.GetChisquare(); 
    int ndof = fitfunc.GetNDF(); 
    rails = hist->GetMaximum() == 4095; 
    if (rails) n_since_last_rail = 0; 
    else n_since_last_rail++; 

    out.cd(); 
    outtree->Fill(); 

  }
  outtree->Write(); 
  out.Close(); 


  return 0; 
}

