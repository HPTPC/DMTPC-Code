#include "CspWaveform.hh"
#include "CspPulse.hh"

dmtpc::waveform::CspWaveform::CspWaveform(int n) : TObject(),
N(n), pulse(n),
secs(0), nsecs(0),
base(0), rms(0), 
wfMax(0), wfMaxTime(0),wfMaxBin(0),
wfMin(0), wfMinTime(0),wfMinBin(0)
{;}


dmtpc::waveform::CspWaveform::CspWaveform(const CspWaveform& w) : TObject(w)
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

dmtpc::waveform::CspWaveform&
dmtpc::waveform::CspWaveform::operator=(const CspWaveform& w) 
{
  if (&w==this) return *this;
  N=w.size();
  secs=w.secs;
  nsecs=w.nsecs;
  pulse=w.pulse;
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


const dmtpc::waveform::CspPulse& 
dmtpc::waveform::CspWaveform::at(int i) const{return pulse[i];}

const dmtpc::waveform::CspPulse& 
dmtpc::waveform::CspWaveform::operator()(int i)const{return pulse[i];}

const dmtpc::waveform::CspPulse& 
dmtpc::waveform::CspWaveform::operator[](int i) const
{return pulse[i];}

dmtpc::waveform::CspPulse& 
dmtpc::waveform::CspWaveform::at(int i){return pulse[i];}

dmtpc::waveform::CspPulse& 
dmtpc::waveform::CspWaveform::operator()(int i){return pulse[i];}

dmtpc::waveform::CspPulse& 
dmtpc::waveform::CspWaveform::operator[](int i){return pulse[i];}

void
dmtpc::waveform::CspWaveform::add(const CspPulse& p)
{
  N++;
  pulse.push_back(p);
}

void
dmtpc::waveform::CspWaveform::insert(int i, const CspPulse& p)
{
  N++;
  pulse.insert(pulse.begin()+i,p);
}

void
dmtpc::waveform::CspWaveform::swap(int i, const CspPulse& p)
{
  pulse[i] = p;
}

void
dmtpc::waveform::CspWaveform::rm(int i)
{
  N--;
  pulse.erase(pulse.begin()+i);
}

void
dmtpc::waveform::CspWaveform::clear()
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
dmtpc::waveform::CspWaveform::clearPulse()
{
  N=0;
  pulse.clear();
}

void
dmtpc::waveform::CspWaveform::resize(int i)
{
  N=i;
  pulse.resize(i);

}
