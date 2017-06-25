#include "CspPulse.hh"

ClassImp(dmtpc::waveform::CspPulse)

void
dmtpc::waveform::CspPulse::setRise(double* v)
{
  R0 = v[0];
  R10 = v[1];
  R25 = v[2];
  R50 = v[3];
  R75 = v[4];
  R90 = v[5];

}

void
dmtpc::waveform::CspPulse::setFall(double* v)
{
  F0 = v[5];
  F10 = v[4];
  F25 = v[3];
  F50 = v[2];
  F75 = v[1];
  F90 = v[0];

}
