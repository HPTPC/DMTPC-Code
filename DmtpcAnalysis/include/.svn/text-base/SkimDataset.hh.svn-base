#ifndef DMTPC_ANALYSIS_SKIM_DATASET_HH
#define DMTPC_ANALYSIS_SKIM_DATASET_HH



#include <vector>
#include <list>
#include <map>
#include "TChain.h" 
#include "TObjArray.h" 
#include "TROOT.h" 
#include "Dataset.hh" 
#include "GainMap.hh"
#include "TFile.h"
class TH2; 
class TTree; 

#define DEFAULT_DATA_DIR  "/net/zwicky/dmtpc/data/" 

using namespace std; 

/**
Class to hold reconstructed data trees
\author Jeremy Lopez (documentation)
*/

namespace dmtpc
{
  namespace core
  {
    class Event; 
  }

namespace analysis
{

class SkimEvent; 
class Stitcher; 
typedef UInt_t BurninEncoded_t;

class SkimDataset{


public:
/**
Constructor
*/
    SkimDataset();
    
/**
Copy constructor
\param other SkimDataset to copy
*/
    SkimDataset(const SkimDataset &other);
    
/**
Destructor
*/
    virtual ~SkimDataset();
    

/**
\return tree holding reconstructed event info
*/
    TTree* tree(const char * key = "skim")  { return (TTree*)chain(key); }
    
/**
\return tree holding reconstructed event info as a TChain object
*/
    TChain* chain(const char * key = "skim") { return _indices.count(key) ? _trees[_indices[key]] : 0 ; }
    
/**
\return pointer to the file containing the trees
*/
    TFile* file() { return _file; }
    
/**
Create or open a root file for writing/reading
\param fname the name of the file
\param foption a file opening option.  If "create" will create a new file and open for writing. If "" will open for reading only.  Others will report an error and quit.  This will not overwrite an existing file.
*/
    void createRootFile( const char *fname, TString foption);
    
/**
Opens a new file for reading.  Just call createRootFile(fname,"")
\param fname the name of the file
*/
    void openRootFile( const char *fname); 
    
/**
If a file is open, deletes the event tree if it exists and closes the file.
*/
    void closeRootFile();
 
/**
Creates a new root file for writing.  Just calls createRootFile(fname,"create")
\param fname the name of the file
*/
    void newRootFile(const char *fname);
    
    const char* getFileName(); 
   
    
/**
\return the reconstructed event
*/  
    SkimEvent* event(const char * key = "skim"){ return _indices.count(key) ? _events[_indices[key]] : 0 ;} 

/** 
\return a pointer to the indices 
*/
    const std::map<std::string,  unsigned> * getTreeIndices() const { return &_indices; } 

/**
\return the original event or NULL if the DmtpcEvents are not loaded
*/
    const dmtpc::core::Event* orig_event() { return orig_loaded ? d->event() : NULL;}

/**
Get an entry from the data tree
\param i the number of the entry to get
*/
    void getEvent(int i);
  
/**
 *\deprecated
  Find the entry corresponding to a particular event in the original data tree. This should 
  no longer be needed. 
  \param n the event to find (number from the original dataset)
  \param startingGuess an initial value to check in order to speed up the search
  \return the index of this event or the nearest one in the skim dataset
*/
    int getEventNumber(int n, int startingGuess=-1); 
    
/**
  \return the total number of events/entries in the tree
*/
    int nevents() { return _nevents; }

/**
  Load the bias frames from this run into memory
  \param load true to load false to not load
  \param fname the name of the file where the bias frames are held
*/
    void loadBiasFrames(bool load, const char * fname=""); 
    
    const TH2 * const * getBiasFrames() const { return _biasFrames; };
    const TH2* getBiasFrame(int i){ return _biasFrames[i]; }
    GainMap* getGainMap(int i){return (_gainMaps && _gainMaps->GetEntries() > i ) ? (GainMap*) _gainMaps->At(i) : 0;}
    GainMap* getGainMap(const char * serialNumber); 
    TObjArray * getGainMaps() {return _gainMaps;}


    //TODO move these somewhere else
    void addGainMap(GainMap* gainMap) {_gainMaps->Add(gainMap);}
    void writeGainMaps(); 

  /**
  Merges the given trees into an a skim tree using DmtpcSkimEvent 
  \param tmpskim a tree holding track information (except RBI info)
  \param burnin a structure holding information about potential RBI events
  \param sparkref a structure holding information about saturated pixels from sparks
  \param run_n the run number
  */

    //TODO move this somewhere else

  void mergeTempTrees(TTree * tmpskim,  list<vector<vector<vector<dmtpc::analysis::BurninEncoded_t> > > * > * burnin, 
                                         list<vector<pair<int,int> > *> * sparkref, int run_n); 
            
  /**
  Loads the original events. Note that now instead of being stored in the skim file, this method will
  transparently load the original file. . 
  /param load whether or not to load the events. if they are loaded and load is false, the file will be closed. 
  /param orig_dataset the filename of the original run file or NULL to try to figure it out
  */
   void loadEvent(bool load = true, const char * orig_dataset = NULL);


   //TODO move these somewhere else
  void setConfig(const char * config) { if (_config){_config->Delete(); } _config = new TObjString(config); } 
  void writeConfig() { _file->cd(); _config->Write("config"); gROOT->cd(); }

  const char * getConfig(){ return _config->String().Data(); } 


  //todo move this elsewhere 
  void writeStitch(const Stitcher * stitch); 

  const Stitcher * getStitch() { return (Stitcher *) _file->Get("stitch"); } 


/**
Loads MaxCamClusterImage objects containing lists of clusters and processed images.  Default is true, but set to false if not needed to speed up readout.
\param l true to load, false to not load
*/
   void loadClusters(bool l) { tree()->SetBranchStatus("_clusters",l); }

  private:
   std::vector<TChain *> _trees; 
   std::vector<SkimEvent *> _events; 

   std::map<std::string, unsigned> _indices;  

   TFile* _file; ///< event file
   
   TH2 ** _biasFrames;  
   int _nevents; ///<number of events

   TObjArray * _gainMaps;///<TObjArray of gainMaps   

   TObjString * _config;

   bool bias_loaded;    
   int nbias; 

   int current_index; 

   bool orig_loaded; 
   dmtpc::core::Dataset * d; 
};

}
}
#endif

