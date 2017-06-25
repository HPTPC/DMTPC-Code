#ifndef DMTPC_WAVEFORM_PMT_WF_VECTOR
#define DMTPC_WAVEFORM_PMT_WF_VECTOR

#include <vector>
#include <string>
#include "TObject.h"

#include "PMTWaveform.hh"
#include "PMTPulse.hh"

namespace dmtpc
{

namespace waveform
{

  // use #include instead
  //class PMTWaveform;
  //class PMTPulse;


class PMTWfVector : public TObject
{

  public:
    PMTWfVector(int n=0,int bid=0, int cid=0, const char* nm="PMT");
    PMTWfVector(const PMTWfVector& v);
    virtual ~PMTWfVector(){;}
 
    const char* GetName() const {return name.c_str();}
    const char* getName() const {return GetName();}

    void setBoard(int id){boardID = id;}
    void setChan(int id){chanID = id;}

    int getBoard(){return boardID;}
    int getChan(){return chanID;}

    int size() const {return N;}
    void add(const PMTWaveform& w);
    void insert(int i, const PMTWaveform& w);
    void swap(int i, const PMTWaveform& w);
    void rm(int i);
    void clearWf();
    void clear(){ clearWf();}
    void resize(int i);

    PMTWfVector& operator=(const PMTWfVector& v);

    const PMTWaveform& at(int i) const;
    const PMTWaveform& operator()(int i) const;
    const PMTWaveform& operator[](int i) const;
    PMTWaveform& at(int i) ;
    PMTWaveform& operator()(int i) ;
    PMTWaveform& operator[](int i) ;

    const PMTPulse& at(int i, int j) const;
    const PMTPulse& operator()(int i, int j) const;
    PMTPulse& at(int i, int j);
    PMTPulse& operator()(int i, int j);
  

  protected:
    std::string name;
    int boardID;
    int chanID;
    int N;
    std::vector<PMTWaveform> wf;
    ClassDef(PMTWfVector,1)

};

}
}
#endif
