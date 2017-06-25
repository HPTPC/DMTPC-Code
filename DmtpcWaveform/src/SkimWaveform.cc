#include "SkimWaveform.hh"
#include "Pulse.hh"


dmtpc::waveform::SkimWaveform::SkimWaveform(int n) : TObject(),
N(n), pulse(n),
base(0), rms(0), 
wfMin(0), wfMinTime(0),wfMinBin(0),
wfMax(0), wfMaxTime(0),wfMaxBin(0)

{;}

dmtpc::waveform::SkimWaveform::SkimWaveform(const SkimWaveform& w) : TObject(w)
{
  N=w.size();
  pulse=w.pulse;
  base=w.base;
  rms=w.rms;

  wfMin=w.wfMin;
  wfMinTime=w.wfMinTime;
  wfMinBin=w.wfMinBin;

  wfMax=w.wfMax;
  wfMaxTime=w.wfMaxTime;
  wfMaxBin=w.wfMaxBin;
}

dmtpc::waveform::SkimWaveform&
dmtpc::waveform::SkimWaveform::operator=(const SkimWaveform& w) 
{
  if (&w==this) return *this;
  N=w.size();
  pulse=w.pulse;
  base=w.base;
  rms=w.rms;

  wfMin=w.wfMin;
  wfMinTime=w.wfMinTime;
  wfMinBin=w.wfMinBin;

  wfMax=w.wfMax;
  wfMaxTime=w.wfMaxTime;
  wfMaxBin=w.wfMaxBin;

  return *this;
}

const dmtpc::waveform::Pulse& 
dmtpc::waveform::SkimWaveform::at(int i) const{return pulse[i];}

const dmtpc::waveform::Pulse& 
dmtpc::waveform::SkimWaveform::operator()(int i)const{return pulse[i];}

const dmtpc::waveform::Pulse& 
dmtpc::waveform::SkimWaveform::operator[](int i) const
{return pulse[i];}

dmtpc::waveform::Pulse& 
dmtpc::waveform::SkimWaveform::at(int i){return pulse[i];}

dmtpc::waveform::Pulse& 
dmtpc::waveform::SkimWaveform::operator()(int i){return pulse[i];}

dmtpc::waveform::Pulse& 
dmtpc::waveform::SkimWaveform::operator[](int i){return pulse[i];}

void
dmtpc::waveform::SkimWaveform::add(const Pulse& p)
{
  N++;
  pulse.push_back(p);
}

void
dmtpc::waveform::SkimWaveform::insert(int i, const Pulse& p)
{
  N++;
  pulse.insert(pulse.begin()+i,p);
}

void
dmtpc::waveform::SkimWaveform::swap(int i, const Pulse& p)
{
  pulse[i] = p;
}

void
dmtpc::waveform::SkimWaveform::rm(int i)
{
  N--;
  pulse.erase(pulse.begin()+i);
}

void
dmtpc::waveform::SkimWaveform::clear()
{
  clearPulse();
  base=0;
  rms=0;

  wfMin=0;
  wfMinTime=0;
  wfMinBin=0;

  wfMax = 0;
  wfMaxTime=0;
  wfMaxBin=0;
}

void
dmtpc::waveform::SkimWaveform::clearPulse()
{
  N=0;
  pulse.clear();
}

void
dmtpc::waveform::SkimWaveform::resize(int i)
{
  N=i;
  pulse.resize(i);
}


