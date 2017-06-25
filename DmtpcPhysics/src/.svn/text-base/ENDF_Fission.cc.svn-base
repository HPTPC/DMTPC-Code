#include "ENDF_Fission.hh"

#include "TRandom.h"
#include "TString.h"
#include "TMath.h"

#include <algorithm>
#include <iostream>
#include <vector>
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
}

dmtpc::physics::endf::Fission::Fission(const char* endfName,const char* endfDir)
{
  fVerbose = false;
  fMaxProb = 0;
  fMaxProb = 0;
  fMinP = 0;
  fMaxP = 1;
  fMinE = 0;
  fMaxE = 0;
  setENDFDir(endfDir);
  setFileName(endfName);

}

dmtpc::physics::endf::Fission::~Fission()
{}

void
dmtpc::physics::endf::Fission::clear()
{
  fEnergy.clear();
  fPDF.clear();
  fCDF.clear();
}

int
dmtpc::physics::endf::Fission::readFile(const char* endfName, const char* endfDir)
{
  setENDFDir(endfDir);
  setFileName(endfName);
  return readFile();
}

int
dmtpc::physics::endf::Fission::readFile()
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
  int NW=0, exponent;
  for (int i = 0; i< 9; i++)
  {
    getline(fin,line);
  }

  istringstream sline(line);
  sline >> NW;
  if (!NW){
    cout <<"Couldn't retrieve nummber of points!"<<endl;
    return -2;
  } 
  istringstream *pline = 0;
  double tmp;
  double Elast = -1;
  while (!fin.eof())
  {

    bool breakLoop = false;
    for (int i = 0; i<NW; i+=2) {
      if (i%6==0) {
        if (pline) delete pline; 
        getline(fin,line);
        pline = new istringstream(line);
      }
    
      (*pline) >> tmp >> exponent;
      
      tmp*= pow(10.,exponent-3);
      if (tmp <= Elast){
        breakLoop = true;
        break;
      }


      if (fVerbose)cout <<"Energy: "<<tmp<<"\t";
      Elast = tmp;
      fEnergy.push_back(tmp);

      (*pline) >> tmp >> exponent;
      tmp *= pow(10.,exponent);
      if (tmp > fMaxProb) fMaxProb = tmp;
      fPDF.push_back(tmp);
      if (fCDF.size()==0) fCDF.push_back(0);
      else{
        int index = fPDF.size()-1;
        fCDF.push_back(fCDF[index-1]+ 0.5*(fPDF[index]+fPDF[index-1]) * (fEnergy[index]-fEnergy[index-1]) );
      }

      if (fVerbose) cout <<"Probability: "<<tmp<<endl;
    }//loop over number of points
    if (breakLoop) break;

  }//while loop
  if (*pline) delete pline;

  fMinE = fEnergy[0];
  fMaxE = fEnergy[fEnergy.size()-1];


  //Normalize CDF:
  double cdfMax = fCDF[fCDF.size()-1];
  if (fVerbose) cout <<"CDF Max Prob: "<<cdfMax;
  for (unsigned int i = 0; i < fCDF.size(); i++)
    fCDF[i]/=cdfMax;

  return 0;
}

int
dmtpc::physics::endf::Fission::readTextFile(const char* endfName, const char* endfDir)
{
  setENDFDir(endfDir);
  setFileName(endfName);
  return readTextFile();
}

int
dmtpc::physics::endf::Fission::readTextFile()
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
    fPDF.push_back(N);
    if (fCDF.size()==0) fCDF.push_back(0);
    else{
      int index = fPDF.size()-1;
      fCDF.push_back(fCDF[index-1] + 0.5*(fPDF[index]+fPDF[index-1])*(fEnergy[index]-fEnergy[index-1]) );
    }
    if (fVerbose) cout <<"Energy: "<<E<<" keV\tProbability: "<<N<<endl;
    if (N>fMaxProb) fMaxProb=N;
  }//while loop

  fMinE = fEnergy[0];
  fMaxE = fEnergy[fEnergy.size()-1];
  return 0;
}

double
dmtpc::physics::endf::Fission::pdf(double E) const
{

  if (E< fMinE||E>fMaxE){
    cout<<"Invalid energy: "<<E<<endl;
    return -1;
  }
  
  int p1 = lower_bound(fEnergy.begin(),fEnergy.end(),E) - fEnergy.begin()-1;
  int p2 = p1==(int)fEnergy.size()-1?p1:p1+1;

  if (p1==p2) return fPDF[p1];  

  double P1 = fPDF[p1];
  double P2 = fPDF[p2];

  double E1 = fEnergy[p1];
  double E2 = fEnergy[p2];

  return (P2 - P1) * (E - E1) / (E2 -E1) + P1; 

}

double
dmtpc::physics::endf::Fission::cdf(double E) const
{

  if (E< fMinE||E>fMaxE){
    cout<<"Invalid energy: "<<E<<endl;
    return -1;
  }
  
  int p1 = lower_bound(fEnergy.begin(),fEnergy.end(),E) - fEnergy.begin()-1;
  int p2 = p1==(int)fEnergy.size()-1?p1:p1+1;

  if (p1==p2) return fCDF[p1];
  
  double P1 = fCDF[p1];
  double P2 = fCDF[p2];

  double E1 = fEnergy[p1];
  double E2 = fEnergy[p2];

  return (P2 - P1) * (E - E1) / (E2 -E1) + P1; 

}

double
dmtpc::physics::endf::Fission::inverseCDF(double P) const
{

  int p1 = lower_bound(fCDF.begin(),fCDF.end(),P) - fCDF.begin()-1;
  int p2 = p1==(int)fCDF.size()-1?p1:p1+1;
 
  if (p1==p2) return fEnergy[p1];
 
  double P1 = fCDF[p1];
  double P2 = fCDF[p2];

  double E1 = fEnergy[p1];
  double E2 = fEnergy[p2];

  return (E2 - E1) * (P - P1) / (P2 -P1) + E1; 
}

void
dmtpc::physics::endf::Fission::setEnergyLimits(double emin, double emax)
{

  if ( emin<=getMinE() )
  {
    fMinE = getMinE();
    fMinP = 0;
  } else if (emin>getMaxE()){
    cout <<"dmtpc::physics::endf::Fission : min energy too high"<<endl;
    fMinE = getMinE();
    fMinP = 0;
  } else{
    int bin = lower_bound(fEnergy.begin(),fEnergy.end(),emin)-fEnergy.begin()-1;
    fMinE = emin;
    double frac = (emin-fEnergy[bin]) / (fEnergy[bin+1]-fEnergy[bin]);
    fMinP = fCDF[bin] + frac * (fCDF[bin+1] - fCDF[bin] ); 
  }

  if ( emax>=getMaxE() )
  {
    fMaxE = getMaxE();
    fMaxP = 1;
  } else if (emax<getMinE()){
    cout <<"dmtpc::physics::endf::Fission : max energy too low"<<endl;
    fMaxE = getMaxE();
    fMaxP = 1;
  } else{
    int bin = lower_bound(fEnergy.begin(),fEnergy.end(),emax)-fEnergy.begin()-1;
    fMaxE = emax;
    double frac = (emax-fEnergy[bin]) / (fEnergy[bin+1]-fEnergy[bin]);
    fMaxP = fCDF[bin] + frac * (fCDF[bin+1] - fCDF[bin] ); 
  }

}


double
dmtpc::physics::endf::Fission::randomEnergy(TRandom* rnd) const
{
  TRandom* random = rnd ? rnd : gRandom;
  double P = fMinP + random->Rndm() * (fMaxP-fMinP);
  return inverseCDF(P);
}
