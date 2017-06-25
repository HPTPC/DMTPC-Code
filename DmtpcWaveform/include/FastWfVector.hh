#ifndef FAST_WF_VECTOR
#define FAST_WF_VECTOR

#include "TObject.h"
#include "FastWaveform.hh"
#include "FastPulse.hh"

#include <vector>
namespace dmtpc
{
namespace waveform
{

  // use #include since DmtpcAnalysis is picky
  //class FastWaveform;
  //class FastPulse;

class FastWfVector : public TObject
{

  public:
    FastWfVector(int n=0,int bid=0, int cid=0, const char* nm="Pulse");
    FastWfVector(const FastWfVector& w);
    virtual ~FastWfVector(){;}
 
    const char* GetName() const {return name.c_str();}
    const char* getName() const {return GetName();}

    void setBoard(int id){boardID = id;}
    void setChan(int id){chanID = id;}
  
    int getBoard(){return boardID;}
    int getChan(){return chanID;}

    int size() const {return N;}
    void add(const FastWaveform& w);
    void insert(int i, const FastWaveform& w);
    void swap(int i, const FastWaveform& w);
    void rm(int i);
    void clearWf();
    void clear(){ clearWf();}
    void resize(int i);

    FastWfVector& operator=(const FastWfVector& w);

    const FastWaveform& at(int i) const;
    const FastWaveform& operator()(int i) const;
    const FastWaveform& operator[](int i) const;
    FastWaveform& at(int i) ;
    FastWaveform& operator()(int i) ;
    FastWaveform& operator[](int i) ;

    const FastPulse& at(int i, int j) const;
    const FastPulse& operator()(int i, int j) const;
    FastPulse& at(int i, int j);
    FastPulse& operator()(int i, int j);
  

  protected:
    std::string name;
    int boardID;
    int chanID;
    int N;
    std::vector<FastWaveform> wf;
    ClassDef(FastWfVector,1)

};

}
}



#endif
