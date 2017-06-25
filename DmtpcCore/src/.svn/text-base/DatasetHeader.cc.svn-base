#include "DatasetHeader.hh"

ClassImp(dmtpc::core::DatasetHeader); 

dmtpc::core::DatasetHeader::DatasetHeader()
{
  starttime = 0; 
  endtime = 0; 
}
dmtpc::core::DatasetHeader::~DatasetHeader()
{
  if (starttime) 
    delete starttime; 
  if (endtime)
    delete endtime; 
}
