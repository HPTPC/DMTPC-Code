#include "WritableDataset.hh" 
#include "TFile.h"
#include "DatasetHeader.hh" 
#include "Event.hh" 

#include <cstdio>


dmtpc::core::WritableDataset::WritableDataset(const char * fname, int ncam, 
    TTimeStamp * start, TTimeStamp * end, const char * config, bool overwrite) 
{

  committed = false; 
  file = new TFile(fname,overwrite ? "RECREATE" : "CREATE" ); 
  if (!file) 
  {
    fprintf(stderr,"Couldn't open file %s, overwrite mode = %d\n", fname,overwrite); 
    return; 
  }

  biasTree = new TTree(getBiasTreeName(),"Bias Tree"); 
  eventTree = new TTree(getDataTreeName(), "Event Tree"); 

  header = new DatasetHeader; 
  header->ncameras = ncam; 
  header->starttime = start; 
  header->endtime =end; 
  header->config.SetString(config); 
  biasPtr = new std::vector<Image*>(ncam); 
  biasOsPtr = new std::vector<Image*>(ncam); 
  biasTree->Branch("biasFrames",&biasPtr); 
  biasTree->Branch("biasOverscans",&biasOsPtr); 

  eventPtr = 0; 
  eventTree->Branch("event",&eventPtr); 

}

int dmtpc::core::WritableDataset::addBias(const Image ** biasframes, const Image ** os) 
{
  for (int i = 0; i < header->ncameras; i++)
  {
    biasPtr->at(i) = (Image*) biasframes[i]; 
    biasOsPtr->at(i) = os ? (Image*) os[i] : NULL; 
  }
  return biasTree->Fill(); 
}

int dmtpc::core::WritableDataset::addEvent(const Event * ev) 
{
  file->cd(); 
  eventPtr = (Event*) ev; 
  return eventTree->Fill(); 
}

void dmtpc::core::WritableDataset::flush()
{
  biasTree->FlushBaskets(); 
  eventTree->FlushBaskets(); 
}
void dmtpc::core::WritableDataset::commit()
{

  file->cd(); 
  biasTree->Write(); 
  eventTree->Write(); 
  header->Write("header"); 
  committed = true; 


}



dmtpc::core::WritableDataset::~WritableDataset()
{
  if (!committed) commit(); 
}





