#include "Pulse.hh"
#include <strings.h>
#include <iostream>
using std::cout;
using std::endl;

ClassImp(dmtpc::waveform::Pulse)

void dmtpc::waveform::Pulse::print(ostream & out)
{
  out << "getPulseHeight(): " 
      << getPulseHeight()
      << " getPulseHeightTime(): "
      << getPulseHeightTime()
      << " getPulseStartTime(): "
      << getPulseStartTime()
      << " getPulseStartBin(): "
      << getPulseStartBin()
      << " getPulseEndTime(): "
      << getPulseEndTime()
      << " getPulseEndBin(): "
      << getPulseEndBin()
      << " getPulseIntegral(): "
      << getPulseIntegral()
      << endl;
}
