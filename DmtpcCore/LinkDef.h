#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedtypedefs;
#pragma link C++ nestedclasses;

#pragma link C++ namespace dmtpc; 
#pragma link C++ namespace dmtpc::core; 
#pragma link C++ namespace dmtpc::core::common; 

#pragma link C++ class dmtpc::core::Config; 
#pragma link C++ class dmtpc::core::Waveform+; 
#pragma link C++ class dmtpc::core::Dataset-; 
#pragma link C++ class dmtpc::core::WritableDataset-; 
#pragma link C++ class dmtpc::core::DatasetHeader+; 
#pragma link C++ class dmtpc::core::Location+; 
#pragma link C++ class dmtpc::core::DetectorConfig; 

#pragma link C++ class dmtpc::core::TriggerInfo;+ 
#pragma link C++ class dmtpc::core::ScopeInfo+; 
#pragma link C++ class dmtpc::core::ScopeChannelInfo+; 
#pragma link C++ class dmtpc::core::ScopeTraceInfo+; 
#pragma link C++ class dmtpc::core::CameraInfo; 

#pragma link C++ class dmtpc::core::CameraConfig+;
#pragma link C++ class dmtpc::core::ScopeConfig+;
#pragma link C++ class dmtpc::core::ScopeChannelConfig+;
#pragma link C++ class dmtpc::core::RunConfig+;

#pragma link C++ class dmtpc::core::Image+; 
#pragma link C++ class dmtpc::core::Event+; 
#pragma link C++ class dmtpc::core::WritableEvent+; 



#endif
