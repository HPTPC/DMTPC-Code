#ifndef DMTPC_WAVEFORM_VECTOR_HH
#define DMTPC_WAVEFORM_VECTOR_HH

#include "SkimWaveform.hh"
#include "Pulse.hh"
#include "TObject.h"
#include <vector>
#include <string>



namespace dmtpc
{
namespace waveform
{


class WaveformVector : public TObject
{

  public:
    WaveformVector(int n=0,int bid=0, int cid=0, const char* nm="Pulse");
    WaveformVector(const WaveformVector& w);
    virtual ~WaveformVector(){;}
 
    const char* GetName() const {return name.c_str();}
    const char* getName() const {return GetName();}

    void setBoard(int id){boardID = id;}
    void setChan(int id){chanID = id;}

    int getBoard(){return boardID;}
    int getChan(){return chanID;}

    int size() const {return N;}
    void add(const SkimWaveform& w);
    void insert(int i, const SkimWaveform& w);
    void swap(int i, const SkimWaveform& w);
    void rm(int i);
    void clearWf();
    void clear(){ clearWf();}
    void resize(int i);

    WaveformVector& operator=(const WaveformVector& w);

    const SkimWaveform& at(int i) const;
    const SkimWaveform& operator()(int i) const;
    const SkimWaveform& operator[](int i) const;
    SkimWaveform& at(int i) ;
    SkimWaveform& operator()(int i) ;
    SkimWaveform& operator[](int i) ;

    const Pulse& at(int i, int j) const;
    const Pulse& operator()(int i, int j) const;
    Pulse& at(int i, int j);
    Pulse& operator()(int i, int j);
  

  protected:
    std::string name;
    int boardID;
    int chanID;
    int N;
    std::vector<SkimWaveform> wf;
    ClassDef(WaveformVector,1)

};
}
}


#endif
