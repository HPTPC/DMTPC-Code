/** \file LinkDef.h
 CINT LinkDef file for this package
*/
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedtypedefs;
#pragma link C++ nestedclasses;

#pragma link C++ namespace dmtpc::mc::retrim;
#pragma link C++ namespace dmtpc::mc::retrim::PeriodicTable;
#pragma link C++ namespace dmtpc::mc::retrim::HistogramReader;
#pragma link C++ class dmtpc::mc::retrim::TableReader;
#pragma link C++ class dmtpc::mc::retrim::CollisionReader;
#pragma link C++ class dmtpc::mc::retrim::TrackMaker;
#pragma link C++ class dmtpc::mc::retrim::EnergyLossModel;
#pragma link C++ class dmtpc::mc::retrim::ConstantEnergyLossModel;
#pragma link C++ class dmtpc::mc::retrim::IonizationModel;
#pragma link C++ class dmtpc::mc::retrim::SimpleIonizationModel;
#pragma link C++ class dmtpc::mc::retrim::CollisionReader::collision;
#pragma link C++ class dmtpc::mc::retrim::CollisionReader::secondary_collision;

#endif

