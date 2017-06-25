#include "CspWfVector.hh"
//#include "CspPulse.hh"
//#include "CspWaveform.hh"
#include <vector>
#include <string>
#include "TObject.h"
using std::vector;


ClassImp(dmtpc::waveform::CspWfVector)

dmtpc::waveform::CspWfVector::CspWfVector(int n, int bid, int cid,const char* nm) :
name(nm), boardID(bid), chanID(cid), N(n), wf(n){;}

dmtpc::waveform::CspWfVector::CspWfVector(const CspWfVector& w) : TObject(w)
{
  name = w.name;
  boardID = w.boardID;
  chanID = w.chanID;
  N = w.N;
  wf = w.wf;
}


void
dmtpc::waveform::CspWfVector::add(const CspWaveform& w)
{
  N++;
  wf.push_back(w);

}

void
dmtpc::waveform::CspWfVector::insert(int i, const CspWaveform& w)
{
  N++;
  wf.insert(wf.begin()+i,w);
}

void
dmtpc::waveform::CspWfVector::swap(int i, const CspWaveform& w)
{
  wf[i] = w;
}

void
dmtpc::waveform::CspWfVector::rm(int i)
{
  N--;
  wf.erase(wf.begin()+i);
}

void
dmtpc::waveform::CspWfVector::clearWf()
{
  N=0;
  wf.clear();
}

void
dmtpc::waveform::CspWfVector::resize(int i)
{
  N=i;
  wf.resize(i);
}

dmtpc::waveform::CspWfVector&
dmtpc::waveform::CspWfVector::operator=(const CspWfVector& v)
{
  if (&v==this) return *this;
  N = v.N;
  name=v.name;
  wf=v.wf;
  return *this;
}


const dmtpc::waveform::CspWaveform& 
dmtpc::waveform::CspWfVector::at(int i) const{return wf[i];}

const dmtpc::waveform::CspWaveform& 
dmtpc::waveform::CspWfVector::operator()(int i) const{return wf[i];}

const dmtpc::waveform::CspWaveform& 
dmtpc::waveform::CspWfVector::operator[](int i) const{return wf[i];}

dmtpc::waveform::CspWaveform& 
dmtpc::waveform::CspWfVector::at(int i) {return wf[i];}

dmtpc::waveform::CspWaveform& 
dmtpc::waveform::CspWfVector::operator()(int i) {return wf[i];}

dmtpc::waveform::CspWaveform& 
dmtpc::waveform::CspWfVector::operator[](int i) {return wf[i];}

const dmtpc::waveform::CspPulse& 
dmtpc::waveform::CspWfVector::at(int i, int j) const{return wf[i][j];}

const dmtpc::waveform::CspPulse& 
dmtpc::waveform::CspWfVector::operator()(int i, int j) const{return wf[i][j];}

dmtpc::waveform::CspPulse& 
dmtpc::waveform::CspWfVector::at(int i, int j){return wf[i][j];}

dmtpc::waveform::CspPulse& 
dmtpc::waveform::CspWfVector::operator()(int i, int j){return wf[i][j];}

