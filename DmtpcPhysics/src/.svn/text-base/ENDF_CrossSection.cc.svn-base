#include "ENDF_CrossSection.hh"

#include "TMath.h"
#include "TRandom.h"
#include "TString.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>


using namespace std;

namespace{
double convertEnergy(double E, TString unit)
{
  if (unit=="meV")
    E *= 1e-6;
  else if (unit=="eV")
    E *= 1e-3;
  else if (unit=="keV")
    E *= 1;
  else if (unit=="MeV")
    E *= 1e3;
  else if (unit=="GeV")
    E *= 1e6;
  else if (unit=="TeV")
    E *= 1e9;

  return E;
}

double max(double x,double y)
{  return x>y ? x:y; }

double min(double x,double y)
{  return x>y ? y:x; }

}


dmtpc::physics::endf::CrossSection::CrossSection(const char* endfName,const char* endfDir)
{
  fVerbose = false;
  fMaxCS = 0;
  setENDFDir(endfDir);
  setFileName(endfName);

}

dmtpc::physics::endf::CrossSection::~CrossSection()
{}

void
dmtpc::physics::endf::CrossSection::clear()
{
  fEnergy.clear();
  fCS.clear();
  fMaxCS = 0;
  fLocMax = 0;
  fMinE=0;
  fMaxE=0;

}

int
dmtpc::physics::endf::CrossSection::readFile(const char* endfName,const char* endfDir)
{
  setENDFDir(endfDir);
  setFileName(endfName);
  return readFile();
}

int
dmtpc::physics::endf::CrossSection::readFile()
{
  clear();

  string filename(fEndfDir);
  filename+=fEndfName;
  ifstream fin(filename.c_str());
  
  if (!fin){
    cout << "File " <<filename<<"not found!" << endl;
    return -1;
  }

  string line;
  int exponent;
  for (int i = 0; i<= 2; i++)
  {
    getline(fin,line);
  }

  double tmp;
  double Elast = -1;
  while (!fin.eof())
  {
   
    getline(fin,line);    
   
    istringstream sline(line);
    
    bool breakLoop = false;
    for (int i = 0; i<5; i+=2) {
      sline >> tmp >> exponent;
      tmp*= pow(10.,exponent-3);
      if (tmp <= Elast){
        breakLoop = true;
        break;
      }

      if (fVerbose)cout <<"Energy: "<<tmp<<"\t";
      Elast = tmp;
      fEnergy.push_back(tmp);

      sline >> tmp >> exponent;
      tmp *= pow(10.,exponent);
      if (tmp > fMaxCS) fMaxCS = tmp;
      fCS.push_back(tmp);
      if (fVerbose) cout <<"Cross section: "<<tmp<<endl;
    }//loop over number of points
    if (breakLoop) break;

  }//while loop
  fLocMax = fMaxCS;
  fMaxE = getMaxE();
  fMinE = getMinE();
  return 0;
}

int
dmtpc::physics::endf::CrossSection::readTextFile(const char* endfName,const char* endfDir)
{
  setENDFDir(endfDir);
  setFileName(endfName);
  return readTextFile();
}

int
dmtpc::physics::endf::CrossSection::readTextFile()
{
  clear();

  string filename(fEndfDir);
  filename+=fEndfName;
  ifstream fin(filename.c_str());

  if (!fin){
    cout << "File " <<filename<<"not found!" << endl;
    return -1;
  }

  string line, unit;
  while (!fin.eof())
  {
    getline(fin,line);
    if (line.size()==0||line[0]=='#') continue;
    istringstream iss(line);
    double E, N;
    iss>>E>>unit>>N;
    E = ::convertEnergy(E,unit);
    fEnergy.push_back(E);
    fCS.push_back(N);
    if (fVerbose) cout <<"Energy: "<<E<<"keV\tCross Section: "<<N<<endl;
    if (N>fMaxCS) fMaxCS=N;
  }//while loop
  fLocMax = fMaxCS;
  fMaxE = getMaxE();
  fMinE = getMinE();
  return 0;
}


double
dmtpc::physics::endf::CrossSection::crossSection(double E) const
{
  typedef vector<double>::iterator iter;
  if (E< getMinE()||E>getMaxE()){
    cout<<"dmtpc::physics::endf::CrossSection: Invalid energy!"<<endl;
    return -1;
  }
  

  int p1 = lower_bound(fEnergy.begin(),fEnergy.end(),E) -fEnergy.begin()-1;
  int p2 = p1==(int) fEnergy.size()-1 ? p1 : p1+1;
  if (p1==p2) return fCS[p1];
  
  double P1 = fCS[p1];
  double P2 = fCS[p2];

  double E1 = fEnergy[p1];
  double E2 = fEnergy[p2];

  return (P2 - P1) * (E - E1) / (E2 -E1) + P1; 

}

void
dmtpc::physics::endf::CrossSection::setEnergyLimits(double Emin, double Emax)
{

  if (Emin<0 && Emax<0)
  {
    fMinE = getMinE();
    fMaxE = getMaxE();
    fLocMax = fMaxCS;
    return;
  }

  //Get energy limits
  Emax = Emax<0?getMaxE(): ::min(Emax,getMaxE());
  Emin = Emin<0?getMinE(): ::max(Emin,getMinE());

  //Check boundary values
  double tmp = ::max(crossSection(Emin),crossSection(Emax));
  int minPoint = lower_bound(fEnergy.begin(),fEnergy.end(),Emin)-1-fEnergy.begin();
  int maxPoint = lower_bound(fEnergy.begin(),fEnergy.end(),Emax)-1-fEnergy.begin();
  if (minPoint==maxPoint && maxPoint<(int) fEnergy.size() -1)
    maxPoint++;

  double tmp2 = *max_element(fCS.begin()+minPoint,fCS.begin()+maxPoint);
  fLocMax = ::max(tmp,tmp2);
  fMinE = Emin;
  fMaxE = Emax;


}


bool
dmtpc::physics::endf::CrossSection::acceptEnergy(double E, TRandom* rnd)
{
  TRandom* random = rnd ? rnd : gRandom;
  if (E> fMaxE||E<fMinE )
  {
    cout <<"dmtpc::physics::endf::CrossSection : Invalid energy. Reset energy limits!"<<endl;
    return 0;
  }

  return crossSection(E) > fLocMax*random->Rndm();

}

bool 
dmtpc::physics::endf::CrossSection::acceptEnergy(double E, double Emin,double Emax,TRandom* rnd)
{
  TRandom* random = rnd ? rnd : gRandom;
  //Get energy limits
  Emax = Emax<0?getMaxE(): ::min(Emax,getMaxE());
  Emin = Emin<0?getMinE(): ::max(Emin,getMinE());

  //Check if limits are valid
  if (Emax < Emin){
    cout << "dmtpc::physics::endf::CrossSection : Max energy must be greater than min energy!"<<endl;
    return 0;
  } else if (Emin > getMaxE()){
    cout <<"dmtpc::physics::endf::CrossSection : Min energy too high!"<<endl;
    return 0;
  } else if (Emax < getMinE()){
    cout <<"dmtpc::physics::endf::CrossSection : Max energy too low!"<<endl;
    return 0;
  }

  if (fMinE!=Emin||fMaxE!=Emax) setEnergyLimits(Emin,Emax);

  //throw random energy
  return crossSection(E)> fLocMax*random->Rndm();
}


