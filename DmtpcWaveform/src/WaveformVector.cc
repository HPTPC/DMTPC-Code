#include "WaveformVector.hh"
#include "SkimWaveform.hh"
#include "Pulse.hh"
#include <vector>
#include <string>
using std::vector;

ClassImp(dmtpc::waveform::WaveformVector); 

dmtpc::waveform::WaveformVector::WaveformVector(int n,int bid, int cid, const char* nm) :
name(nm), boardID(bid), chanID(cid), N(n), wf(n){;}

dmtpc::waveform::WaveformVector::WaveformVector(const WaveformVector& w) : TObject(w)
{
  name = w.name;
  boardID = w.boardID;
  chanID = w.chanID;
  N = w.N;
  wf = w.wf;
}


void
dmtpc::waveform::WaveformVector::add(const SkimWaveform& w)
{
  N++;
  wf.push_back(w);

}

void
dmtpc::waveform::WaveformVector::insert(int i, const SkimWaveform& w)
{
  N++;
  wf.insert(wf.begin()+i,w);
}

void
dmtpc::waveform::WaveformVector::swap(int i, const SkimWaveform& w)
{
  wf[i] = w;
}

void
dmtpc::waveform::WaveformVector::rm(int i)
{
  N--;
  wf.erase(wf.begin()+i);
}

void
dmtpc::waveform::WaveformVector::clearWf()
{
  N=0;
  wf.clear();
}

void
dmtpc::waveform::WaveformVector::resize(int i)
{
  N=i;
  wf.resize(i);
}

dmtpc::waveform::WaveformVector&
dmtpc::waveform::WaveformVector::operator=(const WaveformVector& v)
{
  if (&v==this) return *this;
  N = v.N;
  name=v.name;
  wf=v.wf;
  return *this;
}


const dmtpc::waveform::SkimWaveform& 
dmtpc::waveform::WaveformVector::at(int i) const{return wf[i];}

const dmtpc::waveform::SkimWaveform& 
dmtpc::waveform::WaveformVector::operator()(int i) const{return wf[i];}

const dmtpc::waveform::SkimWaveform& 
dmtpc::waveform::WaveformVector::operator[](int i) const{return wf[i];}

dmtpc::waveform::SkimWaveform& 
dmtpc::waveform::WaveformVector::at(int i) {return wf[i];}

dmtpc::waveform::SkimWaveform& 
dmtpc::waveform::WaveformVector::operator()(int i) {return wf[i];}

dmtpc::waveform::SkimWaveform& 
dmtpc::waveform::WaveformVector::operator[](int i) {return wf[i];}

const dmtpc::waveform::Pulse& 
dmtpc::waveform::WaveformVector::at(int i, int j) const{return wf[i][j];}

const dmtpc::waveform::Pulse& 
dmtpc::waveform::WaveformVector::operator()(int i, int j) const{return wf[i][j];}

dmtpc::waveform::Pulse& 
dmtpc::waveform::WaveformVector::at(int i, int j){return wf[i][j];}

dmtpc::waveform::Pulse& 
dmtpc::waveform::WaveformVector::operator()(int i, int j){return wf[i][j];}

