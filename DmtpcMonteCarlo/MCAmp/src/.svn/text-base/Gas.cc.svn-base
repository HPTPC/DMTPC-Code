#include "Gas.hh" 

#include <cstdio>
#include <cstdlib>
#include "TString.h"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

dmtpc::mc::amp::GasLoader::~GasLoader() 
{
  loaded.clear(); 
}

dmtpc::mc::amp::GasLoader::GasLoader(const char * dirname) 
{
  DIR * dir; 
  if (dir= opendir(dirname))
  {
    printf("Opened directory %s\n", dirname); 
    gasdir = dirname; 
    closedir(dir); 

  }
  else
  {
    if (errno == ENOENT)
    {
      printf("Attempting to create directory %s  ...",dirname); 
      if (mkdir(dirname,0755)) 
      {
        dir == 0; 
        printf("failed!\n"); 
      }
      else
      {
        printf("success!!\n"); 
        gasdir = dirname; 
      }
    }
    else
    {
        printf("Couldn't open %s\n",dirname); 
        gasdir = 0; 
    }
  }
}


Garfield::MediumMagboltz * dmtpc::mc::amp::GasLoader::getCF4(double pressure, double temp)
{
  char * gas_string = strdup(TString::Format("cf4_%g_%g.gas",pressure,temp)); 
  if (gasdir)
  {
    //first check if it's been loaded
    if (loaded.count(gas_string))
    {

      Garfield::MediumMagboltz * gas = loaded[gas_string]; 
      free(gas_string); 
      return gas; 
    }


    //check directory to see if it exists


    FILE * check = fopen(TString::Format("%s/%s", gasdir, gas_string),"r"); 
    

    if (check)
    {
      fclose(check); 
      Garfield::MediumMagboltz * gas = new Garfield::MediumMagboltz; 
      gas->LoadGasFile(std::string(TString::Format("%s/%s", gasdir, gas_string))); 
      loaded[gas_string] = gas; 
      gas->SetW(34);
      gas->SetFanoFactor(0.18);
      gas->LoadIonMobility(std::string(TString::Format("%s/cf3+_in_cf4_mobility.txt",gasdir))); 
      gas->EnableDeexcitation(); 
      gas->SetMaxElectronEnergy(150);
      gas->SetMaxPhotonEnergy(30);
      gas->EnableCrossSectionOutput(); 
      gas->Initialise(true); 
      return gas; 
    }
  }


  Garfield::MediumMagboltz * gas = new Garfield::MediumMagboltz; 
  gas->SetComposition("cf4",100.); 
  gas->SetTemperature(temp); 
  gas->SetPressure(pressure); 
  gas->EnableDeexcitation(); 
  gas->SetMaxElectronEnergy(150);
  gas->SetMaxPhotonEnergy(30);
  gas->SetFieldGrid(100.,20e3,5,true); 
  gas->SetW(34);
  gas->SetFanoFactor(0.18);

  const int ncoll = 5; 
  const bool verbose = true; 
  gas->EnableCrossSectionOutput(); 
  gas->Initialise(verbose); 
  gas->GenerateGasTable(ncoll,verbose); 

  if (gasdir)
  {
    gas->WriteGasFile(std::string(TString::Format("%s/%s", gasdir, gas_string))); 
  }

  loaded[gas_string] = gas; 
  return gas; 
}









