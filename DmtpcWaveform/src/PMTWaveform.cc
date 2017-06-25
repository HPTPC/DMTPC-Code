#include "PMTWaveform.hh"
#include "PMTPulse.hh"

dmtpc::waveform::PMTWaveform::PMTWaveform(int n) : TObject(),
N(n), pulse(n),
secs(0), nsecs(0),
base(0), rms(0), 
wfMax(0), wfMaxTime(0),wfMaxBin(0),
wfMin(0), wfMinTime(0),wfMinBin(0)
{;}


dmtpc::waveform::PMTWaveform::PMTWaveform(const dmtpc::waveform::PMTWaveform& w) : TObject(w)
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

dmtpc::waveform::PMTWaveform&
dmtpc::waveform::PMTWaveform::operator=(const dmtpc::waveform::PMTWaveform& w) 
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


const dmtpc::waveform::PMTPulse& 
dmtpc::waveform::PMTWaveform::at(int i) const{return pulse[i];}

const dmtpc::waveform::PMTPulse& 
dmtpc::waveform::PMTWaveform::operator()(int i)const{return pulse[i];}

const dmtpc::waveform::PMTPulse& 
dmtpc::waveform::PMTWaveform::operator[](int i) const
{return pulse[i];}

dmtpc::waveform::PMTPulse& 
dmtpc::waveform::PMTWaveform::at(int i){return pulse[i];}

dmtpc::waveform::PMTPulse& 
dmtpc::waveform::PMTWaveform::operator()(int i){return pulse[i];}

dmtpc::waveform::PMTPulse& 
dmtpc::waveform::PMTWaveform::operator[](int i){return pulse[i];}

void
dmtpc::waveform::PMTWaveform::add(const dmtpc::waveform::PMTPulse& p)
{
  N++;
  pulse.push_back(p);
}

void
dmtpc::waveform::PMTWaveform::insert(int i, const dmtpc::waveform::PMTPulse& p)
{
  N++;
  pulse.insert(pulse.begin()+i,p);
}

void
dmtpc::waveform::PMTWaveform::swap(int i, const dmtpc::waveform::PMTPulse& p)
{
  pulse[i] = p;
}

void
dmtpc::waveform::PMTWaveform::rm(int i)
{
  N--;
  pulse.erase(pulse.begin()+i);
}

void
dmtpc::waveform::PMTWaveform::clear()
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
dmtpc::waveform::PMTWaveform::clearPulse()
{
  N=0;
  pulse.clear();
}

void
dmtpc::waveform::PMTWaveform::resize(int i)
{
  N=i;
  pulse.resize(i);

}
