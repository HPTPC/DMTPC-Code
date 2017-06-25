#include "Playlist.hh"
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>

#include "TSystem.h"

#define DATADIR ".:/net/zwicky/esata01/dmtpc/production/skimout:/data/4sh/skimout"
//#define DATADIR "/net/zwicky/esata01/dmtpc/production/skimout"

ClassImp(dmtpc::analysis::Playlist);

dmtpc::analysis::Playlist::~Playlist()
{
  if (cur_dataset!=NULL)
    delete cur_dataset; 

  if (_tree!=NULL)
    _tree->Delete(); 
}

dmtpc::analysis::Playlist::Playlist()
{
  cur_dataset = NULL; 
  _tree = NULL;
  current_det = "";
  current_run = -1; 
}


dmtpc::analysis::Playlist::Playlist(const char * file) 
{
  cur_dataset= NULL;
  _tree = NULL; 
  open(file); 
}

void dmtpc::analysis::Playlist::go(int index)
{
  if (index <0) 
  {
    return; 
  }

  if (index >= n())
  {
    return;
  }

  i = index; 

  
  //Load new dataset
  if (cur_dataset == NULL || current_run != run_nums[i] || current_det != det_tags[i])
  {

    current_det = det_tags[i]; 
    current_run = run_nums[i]; 

    // make skimout dir suffix
    TString suffix;
    if (current_det=="4sh")
    {
      suffix+="_4sh"; 
    }
    if (current_det=="Raytheon")
    {
      suffix+="_Raytheon"; 
    }


    suffix += TString::Format("/skim/dmtpc_%s_%05dskim.root", current_det.c_str(), current_run);

    TString default_directories;
    default_directories = TString(DATADIR);
    FileStat_t fstat; 
    Ssiz_t from = 0;
    TString testdir, goodfn;
    while(default_directories.Tokenize(testdir,from,":")) {
      testdir += suffix;
      if (!gSystem->GetPathInfo(testdir.Data(),fstat)) { // found it
        goodfn = testdir;
        break;
      }
    }
    

    if (cur_dataset!=NULL)
    {
      delete cur_dataset; 
    } 

    cur_dataset = new SkimDataset; 
    cur_dataset->openRootFile(goodfn); 
    
    cout << "Opened " << goodfn << endl; 
  }

  cur_dataset->getEvent(events[i]); 
}

void dmtpc::analysis::Playlist::add(const char * det_tag, int run, int ev, int cam, int track)
{
  det_tags.push_back(string(det_tag));
  run_nums.push_back(run);
  events.push_back(ev); 
  cams.push_back(cam); 
  tracks.push_back(track); 
}

void dmtpc::analysis::Playlist::save(const char * file)
{
  
  ofstream of(file); 

  if(!of.is_open())
  {
    cerr << "Could not open " << file << " for saving playlist." << endl; 
    return; 
  }

  of << "#Format: \n#dettag run event cam track \n\n"; 
  for (int line = 0; line < n(); line++)
  {
     of << det_tags[line] << " " << run_nums[line] << " " << events[line] << " " << cams[line] << " " << tracks[line] << "\n";
  }

  of.close();
}

static string trim(string s)
{
  int first = -1; 
  int last = -1; 

  for (size_t i = 0; i < s.length(); i++)
  {
    if (s[i]!=' ' && s[i] !='\t' && s[i]!='\n' && s[i]!='\r') 
    {
      first = i; 
      break;
    }
  }

  for (int i = s.length()-1; i >=0; i--)
  {
    if (s[i]!=' ' && s[i] !='\t' && s[i]!='\n' && s[i]!='\r') 
    {
      last = i+1; 
      break;
    }
  }

  if (first == -1 || last==-1)
  {
    return string(""); 
  }
  else
  {
    return s.substr(first,last-first); 
  }
}

static size_t next_whitespace(string s)
{
  for (size_t i = 0; i < s.length(); i++)
  {
    if (s[i]==' ' || s[i] =='\t' || s[i]=='\n' || s[i]=='\r') 
      return i;
  }

  return 0; 
}

void dmtpc::analysis::Playlist::open(const char * file)
{
  ifstream ifs(file);   
  if(!ifs.is_open())
  {
    cerr << "Could not open " << file << " for loading playlist." << endl; 
    return; 
  }

  string line; 
  while (getline(ifs,line))
  {
    line = trim(line); 
    if (line.length()==0) continue; 
    if (line[0]=='#') continue; 

    //// trim off the comment and beyond
    //size_t cpos = line.find('#');
    //if (cpos != string::npos)  
    //  line = line.substr(0,cpos);

    bool valid = true; 
   
    string tag; 
    int run,ev,c,t;

    for (int token = 0; token < 5; token++)
    {
      
      size_t nw = next_whitespace(line); 
      if( (token < 4 && !nw) || line=="" ) 
      { 
        cerr << "Insufficient tokens on line: " << line << endl; 
        valid = false; 
        break;
      }

      string tokstr = nw > 0 ? line.substr(0,nw) : trim(line);

      switch (token)
      {
        case 0:
          tag = tokstr; break; 
        case 1: 
          run = atoi(tokstr.c_str()); 
          break;
        case 2: 
          ev = atoi(tokstr.c_str()); 
          break;
        case 3: 
          c = atoi(tokstr.c_str()); 
          break;
        case 4: 
          t = atoi(tokstr.c_str()); 
          break;
        default: break; 
      }

      line = trim(line.substr(nw)); 

    }


    if (valid)
    {
      add(tag.c_str(),run,ev,c,t); 
    }
  }
}


