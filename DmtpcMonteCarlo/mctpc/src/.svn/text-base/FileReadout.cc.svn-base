#include "FileReadout.hh" 
#include "TH2.h" 
#include "MCTruth.hh"
#include "TROOT.h"

dmtpc::mc::mctpc::FileReadout::FileReadout(const char * file, const char * filter) 
 : filter(filter), f(file,"RECREATE"), counter(0)
{

}


void dmtpc::mc::mctpc::FileReadout::read(const std::vector<dmtpc::mc::mctpc::Signal>* in, dmtpc::analysis::MCTruth * truth) 
{
  for (unsigned i = 0; i < in->size(); i++)
  {
      if (!filter || strstr(in->at(i).getName(), filter))
      {
        f.cd();
        in->at(i).getHist()->Write(TString::Format("%s_%d",in->at(i).getName(), counter)); 
        gROOT->cd();
      }
  }

  for (unsigned i = 0; i < truth->truth_images.size(); i++)
  {
      TH2 * hist = truth->truth_images[i]; 
      if (!filter || strstr(hist->GetName(), filter))
      {
        f.cd();
        hist->Write(TString::Format("%s_%d",hist->GetName(), counter)); 
        gROOT->cd();
      }
  }

  counter++; 
}

