#include "TChain.h" 
#include "CommonTools.hh" 
#include <fstream>



int dmtpc::core::common::addFilesToChain(TChain * ch, const char * file_list)
{

  std::ifstream sigstream(file_list); 
  std::string tmp; 

  int ret = 0;
  while (sigstream >> tmp) 
  {
    ret+= ch->AddFile(tmp.c_str()); 
  }
 
  return ret; 
}



