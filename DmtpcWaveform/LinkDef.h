/** \file DmtpcWaveformLinkDef.h
\brief CINT LinkDef file for this package
 \author Jeremy Lopez
*/

/**\namespace dmtpc::waveform
\brief Top-level namespace for DMTPC waveform processing and reconstruction
*/


#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedtypedefs;
#pragma link C++ nestedclasses;

#pragma link C++ namespace dmtpc;
#pragma link C++ namespace dmtpc::waveform;

#pragma link C++ class dmtpc::waveform::FirFilter<float>;
#pragma link C++ class dmtpc::waveform::FirFilter<double>;
#pragma link C++ class dmtpc::waveform::IirFilter<float>;
#pragma link C++ class dmtpc::waveform::IirFilter<double>;
#pragma link C++ class dmtpc::waveform::RangeDomainFilter<double>;
#pragma link C++ class dmtpc::waveform::RangeDomainFilter<float>;
#pragma link C++ class dmtpc::waveform::FourierTransform;
#pragma link C++ class dmtpc::waveform::CspPulse;
#pragma link C++ class dmtpc::waveform::CspWaveform;
#pragma link C++ class dmtpc::waveform::CspWfVector;
#pragma link C++ class dmtpc::waveform::FastPulse;
#pragma link C++ class dmtpc::waveform::FastWaveform;
#pragma link C++ class dmtpc::waveform::FastWfVector;
#pragma link C++ class dmtpc::waveform::PMTPulse;
#pragma link C++ class dmtpc::waveform::PMTWaveform;
#pragma link C++ class dmtpc::waveform::PMTWfVector;
#pragma link C++ class dmtpc::waveform::Pulse;
#pragma link C++ class dmtpc::waveform::SkimWaveform;
#pragma link C++ class dmtpc::waveform::WaveformVector;
#pragma link C++ namespace dmtpc::waveform::fir;
#pragma link C++ namespace dmtpc::waveform::analysis;
#endif

