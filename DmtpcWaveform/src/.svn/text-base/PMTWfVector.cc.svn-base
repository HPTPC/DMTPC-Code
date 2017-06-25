#include "PMTWfVector.hh"
//#include "PMTPulse.hh"
//#include "PMTWaveform.hh"
#include <vector>
#include <string>
#include "TObject.h"
using std::vector;


ClassImp(dmtpc::waveform::PMTWfVector)

dmtpc::waveform::PMTWfVector::PMTWfVector(int n, int bid, int cid,const char* nm) :
name(nm), boardID(bid), chanID(cid), N(n), wf(n){;}

dmtpc::waveform::PMTWfVector::PMTWfVector(const PMTWfVector& w) : TObject(w)
{
  name = w.name;
  boardID = w.boardID;
  chanID = w.chanID;
  N = w.N;
  wf = w.wf;
}


void
dmtpc::waveform::PMTWfVector::add(const dmtpc::waveform::PMTWaveform& w)
{
  N++;
  wf.push_back(w);

}

void
dmtpc::waveform::PMTWfVector::insert(int i, const dmtpc::waveform::PMTWaveform& w)
{
  N++;
  wf.insert(wf.begin()+i,w);
}

void
dmtpc::waveform::PMTWfVector::swap(int i, const dmtpc::waveform::PMTWaveform& w)
{
  wf[i] = w;
}

void
dmtpc::waveform::PMTWfVector::rm(int i)
{
  N--;
  wf.erase(wf.begin()+i);
}

void
dmtpc::waveform::PMTWfVector::clearWf()
{
  N=0;
  wf.clear();
}

void
dmtpc::waveform::PMTWfVector::resize(int i)
{
  N=i;
  wf.resize(i);
}

dmtpc::waveform::PMTWfVector&
dmtpc::waveform::PMTWfVector::operator=(const PMTWfVector& v)
{
  if (&v==this) return *this;
  N = v.N;
  name=v.name;
  wf=v.wf;
  return *this;
}


const dmtpc::waveform::PMTWaveform& 
dmtpc::waveform::PMTWfVector::at(int i) const{return wf[i];}

const dmtpc::waveform::PMTWaveform& 
dmtpc::waveform::PMTWfVector::operator()(int i) const{return wf[i];}

const dmtpc::waveform::PMTWaveform& 
dmtpc::waveform::PMTWfVector::operator[](int i) const{return wf[i];}

dmtpc::waveform::PMTWaveform& 
dmtpc::waveform::PMTWfVector::at(int i) {return wf[i];}

dmtpc::waveform::PMTWaveform& 
dmtpc::waveform::PMTWfVector::operator()(int i) {return wf[i];}

dmtpc::waveform::PMTWaveform& 
dmtpc::waveform::PMTWfVector::operator[](int i) {return wf[i];}

const dmtpc::waveform::PMTPulse& 
dmtpc::waveform::PMTWfVector::at(int i, int j) const{return wf[i][j];}

const dmtpc::waveform::PMTPulse& 
dmtpc::waveform::PMTWfVector::operator()(int i, int j) const{return wf[i][j];}

dmtpc::waveform::PMTPulse& 
dmtpc::waveform::PMTWfVector::at(int i, int j){return wf[i][j];}

dmtpc::waveform::PMTPulse& 
dmtpc::waveform::PMTWfVector::operator()(int i, int j){return wf[i][j];}

