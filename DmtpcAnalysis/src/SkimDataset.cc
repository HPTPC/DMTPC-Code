#include "SkimDataset.hh"
#include "TDatime.h"
#include "TTimeStamp.h"
#include "TChain.h"
#include "SkimEvent.hh"
#include "TMath.h"
#include "TH2F.h"
#include "Stitcher.hh"
#include "TH1F.h"
#include "TSystem.h"
#include "TObjString.h"
#include "TKey.h"
#include <iostream>
#include <assert.h>
using std::cout;
using std::endl;
using std::cerr;

using namespace std;

ClassImp(dmtpc::analysis::SkimDataset); 

const char * get_basename(const char * f) 
{
  unsigned n = strlen(f); 

  for (int i = n-1; i >= 0; i--)
  {
    if (f[i] == '/')
      return f+i+1; 
  }

  return f; 

}

dmtpc::analysis::SkimDataset::SkimDataset() 
{ 
  _biasFrames = 0; 
  _file = 0; 

  _gainMaps =0; 
  bias_loaded = false; 
  orig_loaded = false; 

  d = 0; 
  _config = 0; 


}

dmtpc::analysis::SkimDataset::SkimDataset(const SkimDataset &other)  {
  cout << "SkimDataset: Copy Constructor not done" << endl;
  _file = other._file;

}

dmtpc::analysis::SkimDataset::~SkimDataset() 
{

  //std::cout << "Inside Destructor!" << std::endl; 


  if (_file){
    closeRootFile(); 
  }

  if (d) 
  {
    delete d; 
  }
}

/** TODO: Rewrite this piece of shit */ 
void dmtpc::analysis::SkimDataset::loadEvent(bool load , const char * orig)
{
 
  if (!_file || !_file->IsOpen())
  {
    cerr << "Trying to load Dataset before SkimDataset. Nothing done." <<endl;
    return; 
  }

  //If told to unload but nothing's open, just return
  if (!load and !orig_loaded) return; 

  //Unload if told to unload and already loaded
  else if (!load && orig_loaded)
  {
    d->close(); 
    orig_loaded = false; 
    orig = 0; 
    return; 
  }

  if (orig == NULL && !orig_loaded)
  {
    //Figure out the file name 
    TString n; 
    n+=TString(DEFAULT_DATA_DIR); 
    TString f = TString(
          get_basename(getFileName())
        ).ReplaceAll("skim","");

    //Figure out if the detector has a det tag 
    //if it does
    //the file name will be of the form
    //dmtpc_tag_xxxxx.root
    if (f.Data()[5] == '_')
    {
      for (int i = 6; i < 9; i++)
      {
        n+=f.Data()[i]; 
      }
      n+= "/"; 
    }

    n+=f;

    d->open(n.Data()); 
  }

  else if (!orig_loaded) d->open(orig); 

  orig_loaded = true; 
  d->getEvent(current_index); 

  return; 
}

void
dmtpc::analysis::SkimDataset::getEvent(int i) {

  for (unsigned j = 0; j < _trees.size(); j++)
  {
    TChain * t =  _trees[j]; 
    if (i<0 || i>=t->GetEntries()) {
        cerr <<  "SkimDataset: event number incorrect! " << i << endl;
        return; 
    }
     
    t->GetEntry(i);
  }

  if (orig_loaded) d->getEvent(i); 

  current_index = i; 
}
  


int 
dmtpc::analysis::SkimDataset::getEventNumber(int n, int startingGuess)
{
  int i;
  if (startingGuess > 0) i = startingGuess;
  else i = n;

  if (i < 0) i = 0; 
  if (i >= nevents()) i = nevents() - 1; 
  getEvent(i);
  while (event()->eventNumber()!=n)
  {
    int last = event()->eventNumber();
    if (last < n)
    {
      getEvent(++i); 
    }
    else
    {
      getEvent(--i); 
    }
      
    int now = event()->eventNumber(); 
    //cout << "last, now = " << last << ", " << now << endl;
    if ( (last < n && now > n) || (last > n && now < n) || (now==last)) return -1; 
  }
  return i; 
}



void dmtpc::analysis::SkimDataset::mergeTempTrees(TTree * tmpskim, list<vector<vector<vector<BurninEncoded_t> > > * > * burnin, list<vector<pair<int,int> >*> * sparkref, int run_n)
{
  if(!_file->IsOpen())
  {
    cerr << "No out file is open." << endl;  
    return; 
  }
  if (tmpskim->GetEntries() != (int) burnin->size())
  {
    cerr << "Two trees have non-matching dimensions. The merging is not complete" << endl;
    return; 
  } 

  int n = tmpskim->GetEntries(); 
  cout << "n=" << n << endl;
 
  SkimEvent * _event = new SkimEvent;

  tmpskim->SetBranchAddress("tmp_event",&_event); 

  _file->cd();

  TTree * imageTree = new TTree(tmpskim->GetName(), "Skimmed DMTPC events");
  TObjArray* waveform_vectors = NULL;

  imageTree->Branch("event", "dmtpc::analysis::SkimEvent", &_event, 128000, 1);  

  for (int i = 0; i < n; i++)
  {
    cout << "Merging Event: " << i << endl;

    tmpskim->GetEvent(i);

    /* TODO fix this shit 
     *
     *
    //copypasta from Shawn 
    // waveform_vectors appears to get saved twice... I guess that's fine. 
    if(!waveform_vectors)
    {
      cout << "_event->_waveform_vectors=" << &_event->_waveform_vectors << endl;
      cout << "_event->_waveform_vectors->GetEntries()=" << _event->_waveform_vectors.size() << endl; 
      cout << "generating the waveform_vectors vector in the skim tree" << endl;
      _waveform_vectors(_event->_waveform_vectors.size()); 
      for(Int_t ich=0; ich<_event->_waveform_vectors->size(); ++ich)
      {
        
        waveform::tools::addWaveformVectorToTObjArray( waveform_vectors ,
                                                       ich , 
                                                       (*_event->_waveform_vectors)[ich]->GetName() ,
                                                       ((TObject*)(*_event->_waveform_vectors)[ich])->IsA()->GetName() );
        
      }
      //      waveform_vectors->AddAt( new WaveformVector(0,0,0,(*_event->_waveform_vectors)[ii]->GetName()), ii);
      imageTree->Branch(waveform_vectors,32000,1);
    }
    
    for(Int_t ii=0; ii<_event->_waveform_vectors->GetEntries(); ++ii) (*waveform_vectors)[ii]=(*_event->_waveform_vectors)[ii];


    //end copypasta

    */ 
    _event->copyBurnin(burnin->front()); 
    delete burnin->front();
    burnin->pop_front(); 

    _event->copySparkRef(sparkref->front()); 
    delete[] sparkref->front();
    sparkref->pop_front(); 

    imageTree->Fill();
  }
 

  imageTree->Write(); 

  delete imageTree; 

}



void dmtpc::analysis::SkimDataset::writeStitch(const Stitcher * stitch)
{

  //only write once 

  if (_file->Get("stitch"))
  {
     cerr << "Dataset already contains stitch object!!! Not Writing! "<< endl;  
     return; 
  }

  _file->cd(); 
  stitch->Write("stitch",1); 
  gROOT->cd(); 
}

void dmtpc::analysis::SkimDataset::writeGainMaps()
{
  //Only write once
  if (_file->Get("gainMaps"))
    return; 
  _file->cd();
  _gainMaps->Write("gainMaps",1);
  gROOT->cd();
}

void 
dmtpc::analysis::SkimDataset::createRootFile( const char *fname, TString foption)
{
    foption.ToLower();    

    if (_file){
      //if (_file->IsOpen()) _file->Close();
      //delete _file;

      closeRootFile(); 

    }
    //
    // write
    //
    if (fname && foption=="create") {
      _file = TFile::Open(fname, foption);
      if(!_file->IsOpen())
      {
        cerr << "Error opening file! Files cannot be overwritten.\n";
      }
      _gainMaps = new TObjArray;  

    }
    //
    // read
    //
    else if (fname && foption=="") {
       
       _file = TFile::Open(fname);
       _file->cd(); 
       TIter next(_file->GetListOfKeys());
       while(TKey * key = (TKey*) next.Next())
       {
          TObject * obj = _file->Get(key->GetName());   
          if (!strcmp(obj->IsA()->GetName(),"TTree"))
          {
             if (_indices.count(key->GetName())) continue; 

         //    std::cout << " event address of " << key->GetName() << _events[ntrees] << std::endl; 
         //    std::cout << " chain address of " << key->GetName() << _trees[ntrees] << std::endl; 
             gROOT->cd(); 
             TChain * ch = new  TChain(key->GetName(),"Reconstructed Tree"); 
             ch->SetDirectory(0); 
             _trees.push_back(ch); 
         //    std::cout << " chain address of " << key->GetName() << ch << std::endl; 
         //    std::cout << " event address of " << key->GetName() << _events[ntrees] << std::endl; 
             _events.push_back(new dmtpc::analysis::SkimEvent); 
             ch->SetBranchAddress("event",&(_events[_trees.size()-1])); 
         //    std::cout << " event address of " << key->GetName() << _events[ntrees] << std::endl; 
             ch->Add(fname); 
             _nevents = ch->GetEntries(); 
             _indices[std::string(key->GetName())] =_trees.size()-1; 
          }

          obj->Delete(); 
       }
        
       _gainMaps = (TObjArray*)_file->Get("gainMaps");
       if (_file->Get("config"))
       {
         _config = (TObjString*) _file->Get("config"); 
       }
    }
    else if (fname) assert(0);
}

dmtpc::analysis::GainMap* dmtpc::analysis::SkimDataset::getGainMap(const char * serialNumber)
{
   for(int i=0; i<_gainMaps->GetEntries(); i++)
   {
      if(((GainMap*)_gainMaps->At(i))->GetName()==serialNumber)
      {
         return (GainMap*)_gainMaps->At(i);
      }
   }
   cout << "No gain map found with serial number " << serialNumber
        << ". \n";
   return 0;

}

const char* dmtpc::analysis::SkimDataset::getFileName() { return _file->GetName(); }

void 
dmtpc::analysis::SkimDataset::openRootFile( const char *fname) {
  createRootFile(fname,"");
}

void 
dmtpc::analysis::SkimDataset::loadBiasFrames(bool load, const char *fname)
{
  
  /* If bias not loaded and asked to not load, do nothing */
  if (!bias_loaded && !load) return; 
  
  /* Otherwise, we either want to delete because we no longer 
   * need the bias frames or because we want a different file */
  if (bias_loaded)
  {
    bias_loaded = false;
    for (int i = 0; i < nbias; i++)
    {
      _biasFrames[i]->Delete(); 
    } 
    
    delete _biasFrames; 
  }
  
  
  TFile * biasfile; 
  if (strcmp(fname,"")==0)
  {
    //figure out bias file from file name
    TString fn = TString(getFileName()).ReplaceAll("skim.root","bias.root");
    biasfile = new TFile(fn.Data()); 
  }
  else
  {
      biasfile = new TFile(fname);  
  }
  
  if (biasfile == 0)
  {
    cerr << fname << "could not be loaded" << endl; 
    return;
  }
  
  gROOT->cd(); 
  
  TTree * bias = (TTree *) biasfile->Get("bias"); 
  TH2F * frame = 0; 
  bias->SetBranchAddress("biasframe",&frame);
  
  nbias = bias->GetEntries(); 
  _biasFrames = new TH2*[nbias]; 
  
  for (int i = 0; i < nbias; i++)
  {
    bias->GetEvent(i);
    _biasFrames[i] = frame;
    //cout << frame->GetName() << " " << frame << endl; 
    frame = 0; 
  }
   
   bias->Delete();
   biasfile->Close(); 
   biasfile->Delete(); 
   bias_loaded = true; 
}


void
dmtpc::analysis::SkimDataset::closeRootFile()
{
  if (!_file) return;
  if (!_file->IsOpen()) return;
  if (orig_loaded) d->close(); 

  for (unsigned i = 0; i <_trees.size();i++)
  {
    //std::cout << "Deleting " << _trees[i] << std::endl; 
    _trees[i]->Delete();
    _trees[i] = 0; 
    delete _events[i]; 
    _events[i] = 0; 
  }
  _file->Close();
  delete _file;
  _indices.clear(); 

 if (_gainMaps)
  {
    _gainMaps->Delete(); 
    delete _gainMaps; 
    _gainMaps = 0; 
  }
 _file = 0; 

 if (_config)
 {
    _config->Delete(); 
 } 

}


void
dmtpc::analysis::SkimDataset::newRootFile(const char *fname)
{
   createRootFile(fname,"create");
}

