#include "FastWfVector.hh"
//#include "FastPulse.hh"
//#include "FastWaveform.hh"
#include <vector>
#include <string>
#include "TObject.h"
using std::vector;


ClassImp(dmtpc::waveform::FastWfVector)

dmtpc::waveform::FastWfVector::FastWfVector(int n,int bid, int cid, const char* nm) :
name(nm),boardID(bid),chanID(cid), N(n), wf(n){;}

dmtpc::waveform::FastWfVector::FastWfVector(const dmtpc::waveform::FastWfVector& w) : TObject(w)
{
  name = w.name;
  boardID = w.boardID;
  chanID = w.chanID;
  N = w.N;
  wf = w.wf;
}


void
dmtpc::waveform::FastWfVector::add(const dmtpc::waveform::FastWaveform& w)
{
  N++;
  wf.push_back(w);

}

void
dmtpc::waveform::FastWfVector::insert(int i, const dmtpc::waveform::FastWaveform& w)
{
  N++;
  wf.insert(wf.begin()+i,w);
}

void
dmtpc::waveform::FastWfVector::swap(int i, const dmtpc::waveform::FastWaveform& w)
{
  wf[i] = w;
}

void
dmtpc::waveform::FastWfVector::rm(int i)
{
  N--;
  wf.erase(wf.begin()+i);
}

void
dmtpc::waveform::FastWfVector::clearWf()
{
  N=0;
  wf.clear();
}

void
dmtpc::waveform::FastWfVector::resize(int i)
{
  N=i;
  wf.resize(i);
}

dmtpc::waveform::FastWfVector&
dmtpc::waveform::FastWfVector::operator=(const dmtpc::waveform::FastWfVector& v)
{
  if (&v==this) return *this;
  N = v.N;
  name=v.name;
  wf=v.wf;
  return *this;
}

const dmtpc::waveform::FastWaveform& 
dmtpc::waveform::FastWfVector::at(int i) const{return wf[i];}

const dmtpc::waveform::FastWaveform& 
dmtpc::waveform::FastWfVector::operator()(int i) const{return wf[i];}

const dmtpc::waveform::FastWaveform& 
dmtpc::waveform::FastWfVector::operator[](int i) const{return wf[i];}

dmtpc::waveform::FastWaveform& 
dmtpc::waveform::FastWfVector::at(int i) {return wf[i];}

dmtpc::waveform::FastWaveform& 
dmtpc::waveform::FastWfVector::operator()(int i) {return wf[i];}

dmtpc::waveform::FastWaveform& 
dmtpc::waveform::FastWfVector::operator[](int i) {return wf[i];}

const dmtpc::waveform::FastPulse& 
dmtpc::waveform::FastWfVector::at(int i, int j) const{return wf[i][j];}

const dmtpc::waveform::FastPulse& 
dmtpc::waveform::FastWfVector::operator()(int i, int j) const{return wf[i][j];}

dmtpc::waveform::FastPulse& 
dmtpc::waveform::FastWfVector::at(int i, int j){return wf[i][j];}

dmtpc::waveform::FastPulse& 
dmtpc::waveform::FastWfVector::operator()(int i, int j){return wf[i][j];}

