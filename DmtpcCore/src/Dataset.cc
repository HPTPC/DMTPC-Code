#include "Dataset.hh" 
#include "DatasetHeader.hh" 
#include "TH2.h" 
#include "Event.hh" 
#include "TFile.h" 
#include "TTree.h" 
#include <cstdio>
#include <assert.h>
#include "Image.hh" 


const char * dmtpc::core::Dataset::getBiasTreeName()
{
  return "bias"; 
}

const char * dmtpc::core::Dataset::getDataTreeName()
{
  return "data"; 
}

dmtpc::core::Dataset::Dataset(const char * f)
{

  file = 0; 
  eventPtr= 0; 
  if (f) 
  { 
    open(f); 
  }
}

void dmtpc::core::Dataset::clearFile()
{

  if (file)
  {
    for (unsigned i = 0; i < bias_avg.size(); i++)
    {
      if (bias_avg[i]) 
      {
        delete bias_avg[i]; 
        bias_avg[i] = 0; 
      }
    }

    file->Close(); 
    delete file; 
  }

}

int dmtpc::core::Dataset::close() 
{

  clearFile(); 
  file = 0; 
  return 0; 
}


dmtpc::core::Dataset::~Dataset()
{
  clearFile(); 
}

int dmtpc::core::Dataset::open(const char * f) 
{

  clearFile(); 
  file = TFile::Open(f); 

  assert(file); // to prevent long-winded segfaults

  if (!file) return -1; 

  biasTree = (TTree*) file->Get(getBiasTreeName()); 

  if (!biasTree)
  {
    fprintf(stderr,"No bias tree in file!!! This will probably not go well\n"); 
  }
  else
  {
    biasPtr = 0; 
    biasOsPtr = 0; 
    biasTree->SetBranchAddress("biasFrames",&biasPtr); 
    biasTree->SetBranchAddress("biasOverscans",&biasOsPtr); 
    biasTree->GetEntry(0); 
  }

  eventTree = (TTree*) file->Get(getDataTreeName()); 

  if (!eventTree)
  {
    fprintf(stderr,"No data tree in file!!! This will probably not go well.\n"); 
  }
  else
  {
    eventPtr = new Event; 
    eventTree->SetBranchAddress("event",&eventPtr); 
    eventTree->GetEntry(0); 
  }
      
  bias_avg.clear(); 

  header = (DatasetHeader*) file->Get("header"); 
  if (!header)
  {
    header = 0; 
    fprintf(stderr,"No header in file... this might cause problems!\n"); 
  }

  else
  {
    bias_avg.reserve(header->ncameras); 
    for (int i = 0; i < header->ncameras; i++)
    {
      bias_avg.push_back(0); 
    }
  }
}

const TH2* dmtpc::core::Dataset::computeAvg(int icam, bool os)
{
  int currentEntry = biasTree->GetReadEntry(); 

  TString name = TString::Format("biasosavg%d", icam); 

  std::vector<Image*> * ptr = os ? biasOsPtr : biasPtr; 

  const TH2 * h= ptr->at(icam)->getHist(); 
 
  os ? bias_os_avg[icam] : bias_avg[icam] = new TH2D(name,name, h->GetNbinsX(), h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax(), 
                                                     h->GetNbinsY(), h->GetYaxis()->GetXmin(), h->GetYaxis()->GetXmax());

  TH2 * avg = os ? bias_os_avg[icam] : bias_avg[icam]; 

  for (int i = 0; i < biasTree->GetEntries(); i++)
  {
     biasTree->GetEntry(i); 
     avg->Add(ptr->at(icam)->getHist()); 
  }

  avg->Scale(1./biasTree->GetEntries()); 

  biasTree->GetEntry(currentEntry); // appear like we did nothing 

  return avg; 
}

const TH2 * dmtpc::core::Dataset::biasOSAvg(int icam) 
{
 return bias_os_avg[icam] ? bias_os_avg[icam] : computeAvg(icam,true); 
}

const TH2 * dmtpc::core::Dataset::biasAvg(int icam) 
{
 return bias_avg[icam] ? bias_avg[icam] : computeAvg(icam); 
}


