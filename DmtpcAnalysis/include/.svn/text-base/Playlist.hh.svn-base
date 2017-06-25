#ifndef DMTPC_ANALYSIS_PLAYLIST_HH
#define DMTPC_ANALYSIS_PLAYLIST_HH

#include <cstdlib>
#include "SkimDataset.hh"
#include "Dataset.hh"
#include "SkimEvent.hh" 

namespace dmtpc
{
  namespace analysis
  {
    class Playlist 
    {

      public: 

      enum TYPE
      {
        SKIM, RAW
      }; 

      Playlist(); 
      Playlist(const char * file); 
      virtual ~Playlist(); 


      SkimDataset * getDataset() { return cur_dataset; }

      dmtpc::core::Dataset * getRawDataset() { return cur_raw_dataset; }

      const SkimEvent * getEvent() { return cur_dataset == NULL ? NULL : cur_dataset->event();} 
      const dmtpc::core::Event * getRawEvent() { return cur_raw_dataset == NULL ? NULL : cur_raw_dataset->event();} 
      int getCam() const {return cams[i];} 
      int getTrack() const {return tracks[i];} 
      TYPE getType() const {return type; }
      const char * getDataDir() const { return data_dir; } 

      void next(){go(i+1);} 
      void previous(){go(i-1);}
      int index(){ return i;}
      void go(int index); 
      int n(){ return run_nums.size();}


      void setDataDir(const char * datadir) { free(data_dir); strcpy(data_dir,datadir); }
      void setType(TYPE t) { type = t; } 
      void add(const char * det_tag, int run, int ev, int cam, int track); 
      void save(const char * file); 
      void open(const char * file); 


    private:

    int i; 
    string current_det;
    int current_run; 
    SkimDataset * cur_dataset;
    dmtpc::core::Dataset * cur_raw_dataset; 
    char * data_dir; 
    TYPE type; 

    TTree * _tree;
    vector<string> det_tags;
    vector<int> run_nums;  
    vector<int> events;   
    vector<int> cams;    
    vector<int> tracks; 


    ClassDef(Playlist,0); 

    };

  }
}


#endif 
