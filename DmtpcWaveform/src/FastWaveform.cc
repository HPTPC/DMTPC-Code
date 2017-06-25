#include "FastWaveform.hh"
#include "FastPulse.hh"

dmtpc::waveform::FastWaveform::FastWaveform(int n) : TObject(),
N(n), pulse(n),
secs(0), nsecs(0),
base(0), rms(0), 
wfMax(0), wfMaxTime(0),wfMaxBin(0),
wfMin(0), wfMinTime(0),wfMinBin(0)
{;}

dmtpc::waveform::FastWaveform::FastWaveform(const FastWaveform& w) : TObject(w)
{
  N=w.size();
  pulse=w.pulse;
  secs=w.secs;
  nsecs=w.nsecs;
  base=w.base;
  rms=w.rms;
  wfMax=w.wfMax;
  wfMaxTime=w.wfMaxTime;
  wfMaxBin=w.wfMaxBin;
  wfMin=w.wfMin;
  wfMinTime=w.wfMinTime;
  wfMinBin=w.wfMinBin;
}

dmtpc::waveform::FastWaveform&
dmtpc::waveform::FastWaveform::operator=(const FastWaveform& w) 
{
  if (&w==this) return *this;
  N=w.size();
  pulse=w.pulse;
  secs=w.secs;
  nsecs=w.nsecs;
  base=w.base;
  rms=w.rms;
  wfMax=w.wfMax;
  wfMaxTime=w.wfMaxTime;
  wfMaxBin=w.wfMaxBin;
  wfMin=w.wfMin;
  wfMinTime=w.wfMinTime;
  wfMinBin=w.wfMinBin;
  return *this;
}

const dmtpc::waveform::FastPulse& 
dmtpc::waveform::FastWaveform::at(int i) const{return pulse[i];}

const dmtpc::waveform::FastPulse& 
dmtpc::waveform::FastWaveform::operator()(int i)const{return pulse[i];}

const dmtpc::waveform::FastPulse& 
dmtpc::waveform::FastWaveform::operator[](int i) const
{return pulse[i];}

dmtpc::waveform::FastPulse& 
dmtpc::waveform::FastWaveform::at(int i){return pulse[i];}

dmtpc::waveform::FastPulse& 
dmtpc::waveform::FastWaveform::operator()(int i){return pulse[i];}

dmtpc::waveform::FastPulse& 
dmtpc::waveform::FastWaveform::operator[](int i){return pulse[i];}

void
dmtpc::waveform::FastWaveform::add(const FastPulse& p)
{
  N++;
  pulse.push_back(p);
}

void
dmtpc::waveform::FastWaveform::insert(int i, const FastPulse& p)
{
  N++;
  pulse.insert(pulse.begin()+i,p);
}

void
dmtpc::waveform::FastWaveform::swap(int i, const FastPulse& p)
{
  pulse[i] = p;
}

void
dmtpc::waveform::FastWaveform::rm(int i)
{
  N--;
  pulse.erase(pulse.begin()+i);
}

void
dmtpc::waveform::FastWaveform::clear()
{
  clearPulse();
  secs=0;
  nsecs=0;
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
dmtpc::waveform::FastWaveform::clearPulse()
{
  N=0;
  pulse.clear();
}

void
dmtpc::waveform::FastWaveform::resize(int i)
{
  N=i;
  pulse.resize(i);
}
