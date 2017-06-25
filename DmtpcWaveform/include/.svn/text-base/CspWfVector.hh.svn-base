#ifndef CSP_WF_VECTOR
#define CSP_WF_VECTOR

#include <vector>
#include <string>
#include "TObject.h"
#include "CspWaveform.hh"
#include "CspPulse.hh"

namespace dmtpc
{
namespace waveform
{
  // for some reason, DmtpcAnalysis does not like this forward declaration so use #include instead
  //class CspWaveform;
  //class CspPulse;

class CspWfVector : public TObject
{

  public:
    CspWfVector(int n=0,int bid=0, int cid=0, const char* nm="CSP");
    CspWfVector(const CspWfVector& v);
    virtual ~CspWfVector(){;}
 
    const char* GetName() const {return name.c_str();}
    const char* getName() const {return GetName();}

    void setBoard(int id){boardID = id;}
    void setChan(int id){chanID = id;}

    int getBoard(){return boardID;}
    int getChan(){return chanID;}

    int size() const {return N;}
    void add(const CspWaveform& w);
    void insert(int i, const CspWaveform& w);
    void swap(int i, const CspWaveform& w);
    void rm(int i);
    void clearWf();
    void clear(){ clearWf();}
    void resize(int i);

    CspWfVector& operator=(const CspWfVector& v);

    const CspWaveform& at(int i) const;
    const CspWaveform& operator()(int i) const;
    const CspWaveform& operator[](int i) const;
    CspWaveform& at(int i) ;
    CspWaveform& operator()(int i) ;
    CspWaveform& operator[](int i) ;

    const CspPulse& at(int i, int j) const;
    const CspPulse& operator()(int i, int j) const;
    CspPulse& at(int i, int j);
    CspPulse& operator()(int i, int j);
  

  protected:
    std::string name;
    int boardID;
    int chanID;
    int N;
    std::vector<CspWaveform> wf;
    ClassDef(CspWfVector,1)

};
}
}

#endif
