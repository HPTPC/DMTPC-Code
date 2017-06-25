#include "DatasetReadout.hh" 
#include <sys/stat.h>
#include "MCTruth.hh"
#include "TROOT.h"
#include "TFile.h"
#include <iostream>

#include "Image.hh"

dmtpc::mc::mctpc::DatasetReadout::DatasetReadout(const char * outdir, TTimeStamp * start, TTimeStamp * end) 
{

  this->start = start; 
  this->end = end; 
  event = 0; 
  d = 0; 
  dir = outdir; 
}

void dmtpc::mc::mctpc::DatasetReadout::defineCamera(const char * name, const char * biasname)
{
  cameras.push_back(name); 
  camera_biases.push_back(biasname); 
}


void dmtpc::mc::mctpc::DatasetReadout::read(const std::vector<dmtpc::mc::mctpc::Signal>* in, dmtpc::analysis::MCTruth * truth) 
{

  dmtpc::core::WritableEvent ev; 
  truthptr = truth; 
  TString fname;
  if (d == 0)
  {
    file_index = 1; 
    struct stat st; 
    while(true) 
    {
       fname = TString::Format("%s/dmtpc_mc_%05d.root",dir,file_index); 
       std::cout << fname << std::endl; 
       int result = stat(fname.Data(),&st); 
       if (result < 0) 
         break; 
       file_index++; 
    } 


    d = new dmtpc::core::WritableDataset(fname.Data(), cameras.size(), start, end); 

    //add the bias frames 
    
    std::vector<dmtpc::core::Image*>  biasframes;

    //loop over input signals
    for (unsigned i = 0; i < in->size(); i++) 
    {
      const char * signame = in->at(i).getName(); 

      for (unsigned j = 0; j < cameras.size(); j++) 
      {
        if (!strcmp(camera_biases[j],signame))
        {
          std::cout <<" adding bias frame " << signame << std::endl; 
          biasframes.push_back(dmtpc::core::Image::makeImage(in->at(i).getHist2D(),2)); 
          break;
        }
      }
    }

    d->addBias((const dmtpc::core::Image**) (&biasframes[0]),0); 
    d->flush(); 


    d->getFile()->cd(); 

    truth_tree = new TTree("Simulation","Simulation"); 
    truth_tree->Branch("truth",&truthptr); 
    int sec = (int) truth->time; 
    int nsec = (int ) (1e9 * (truth->time - sec)); 
    ev.setTimeStamp(new TTimeStamp(sec, nsec)); 
    gROOT->cd(); 

  }


  d->getFile()->cd(); 
  truth_tree->Fill(); 
  gROOT->cd(); 

  ev.setRunNumber(file_index); 
  ev.setEventNumber(event++); 
  

  //loop over input signals
  for (unsigned i = 0; i < in->size(); i++) 
  {
    const char * signame = in->at(i).getName(); 

    for (unsigned j = 0; j < cameras.size(); j++) 
    {
      if (!strcmp(cameras[j],signame))
      {
        std::cout <<" adding image frame " << signame << std::endl; 
        ev.ccdDataPtr()->push_back(dmtpc::core::Image::makeImage(in->at(i).getHist2D(), 2)); 
        ev.overscanPtr()->push_back(0); 
        break;
      }
    }
  }

  d->addEvent(&ev); 
//  d->flush(); 


}

dmtpc::mc::mctpc::DatasetReadout::~DatasetReadout()
{

  d->getFile()->cd(); 
  truth_tree->Write(); 
  gROOT->cd(); 
  d->commit(); 
  delete d; 
}


