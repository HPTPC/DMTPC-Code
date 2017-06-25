#include "ENDF_AngularDistribution.hh"
#include "TString.h"
#include "TMath.h"
#include "TRandom.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::istringstream;


dmtpc::physics::endf::AngularDistribution::AngularDistribution(int nPh)
{
  setNPhi(nPh);
  fEndfDir = "./";
  fEndfName = "";
 
  fNE = 0; 
  fVerbose = false;
  fMaxCS = 0; 
}

dmtpc::physics::endf::AngularDistribution::~AngularDistribution()
{}


void
dmtpc::physics::endf::AngularDistribution::setNPhi(int nPh)
{

  fNCosPhi = nPh;
  if (fCosPhi.size() >0)
  {
    fCosPhi.clear();
    fDCS.clear();
    fDCSInt.clear();
  }

  for (int i = 0; i< nPh; i++)
  {
    double cP = -1. + i * 2./(nPh-1.);
    fCosPhi.push_back(cP);
  }
}

int 
dmtpc::physics::endf::AngularDistribution::readFile(const char* name,const char* dir)
{
  setENDFDir(dir);
  setENDFName(name);
  return readFile();
}

int
dmtpc::physics::endf::AngularDistribution::readFile()
{
  static int MAX_N_TERMS=25;
  TString filename(fEndfDir);
  filename+=fEndfName;
  ifstream fin(filename);

  if (!fin) {
    cout <<"Could not open file for reading!"<<endl;
    return -1;
  }

  string line;
  double lastE = -1;
  double E;
  int NW;
  int exponent;

  for (int i = 0; i<5; i++)
  {
    getline(fin,line);
  }

  while(!fin.eof()) 
  { 
    getline(fin,line);
    istringstream sline(line);
    sline >> E >> exponent >> E >> exponent;
    sline >> NW >> NW >> NW;
    if (!NW) break;
    E *= pow(10,exponent-3); // keV
    if (E < lastE) break;
    
    lastE = E;
    fEnergy.push_back(E);

    istringstream *pline = NULL;
    double P[MAX_N_TERMS];//Maximum
    P[0] = 1;
    if (NW > MAX_N_TERMS)
    {
      cout <<"Error: Number of terms greater than maximum value!"<<endl;
      return -2;
    }
    for (int i = 0; i < NW; i++)
    {
      if (i%6==0) {
        if (pline) delete pline;
        getline(fin,line);
        pline = new istringstream(line);
      }
      double coeff;
      (*pline) >> coeff >> exponent;
      P[i+1] = coeff*pow(10,exponent);


    }//loop over coefficients
    if (pline) delete pline;

    if (fVerbose){
      cout <<"Energy: "<<E<<" keV"<<endl;
      cout <<"\tLegendre Coeffs: ";
      for (int i = 0 ; i <= NW; i++){
        cout <<P[i]<<"\t";
      }
      cout <<endl;
    }

    vector<double> tempVec(fNCosPhi);
    for (int i = 0; i < fNCosPhi; i++)
    {
      double cTh = -1 + 2.*i / (fNCosPhi-1.);
      tempVec[i] = legendre(cTh,P,NW+1);
      if (tempVec[i] > fMaxCS) fMaxCS = tempVec[i];
    }
    fDCS.push_back(tempVec);
    fNE++;

  }//while loop

  //Now fill the integrated CDF at each energy point.
  for (unsigned i = 0; i < fDCS.size(); i++)
  {
    vector<double> tempVec(fNCosPhi);
    tempVec[0] = 0;
    double dCosPhi = 2./(fNCosPhi-1.);
    for (int j = 0; j<fNCosPhi-1; j++)
      tempVec[j+1] = tempVec[j] + dCosPhi * 0.5 * (fDCS[i][j]+ fDCS[i][j+1]);
    
    for (int j = 0; j < fNCosPhi; j++)
      tempVec[j] /= tempVec[fNCosPhi-1];

    fDCSInt.push_back(tempVec);
    
  }

  return 0;
}

double
dmtpc::physics::endf::AngularDistribution::crossSection(double E, double cosPh) const
{

  if (cosPh > 1 || cosPh < -1 )
  {
    cout <<"Invalid cosine value!"<<endl;
    return -1;
  } else if (E < fEnergy[0] || E > fEnergy[fEnergy.size()-1]){
    cout <<"Invalid energy value!"<<endl;
    return -1;
  }
  
  int Eb1 = lower_bound(fEnergy.begin(),fEnergy.end(),E)-1-fEnergy.begin();
  int Cb1 = lower_bound(fCosPhi.begin(),fCosPhi.end(),cosPh)-1-fEnergy.begin();
  if (Eb1<0) Eb1 = 0;
  if (Cb1<0) Cb1 = 0;
  int Eb2 = Eb1 == fNE-1? Eb1:Eb1+1;
  int Cb2 = Cb1 == fNCosPhi-1?Cb1:Cb1+1;
  double E1 = fEnergy[Eb1];
  double E2 = fEnergy[Eb2];
  double c1 = fCosPhi[Cb1];
  double c2 = fCosPhi[Cb2];

  double CS_11 = fDCS[Eb1][Cb1];
  double CS_12 = fDCS[Eb1][Cb2];
  double CS_21 = fDCS[Eb2][Cb1];
  double CS_22 = fDCS[Eb2][Cb2];

  double cross_sec;
  if (Eb1 != Eb2 && Cb1 != Cb2)
  {//Bilinear interpolation
    //E direction first:
    double cs_c1 = (E-E1)/(E2-E1) * (CS_21-CS_11) + CS_11;
    double cs_c2 = (E-E1)/(E2-E1) * (CS_22-CS_12) + CS_12;
    //Interpolate these two points in cos(phi) direction;
    cross_sec = (cosPh - c1) / (c2-c1) * (cs_c2-cs_c1) + cs_c1;
  }else if (Eb1==Eb2 && Cb1 != Cb2)
    return CS_11 + (cosPh-c1)/(c2-c1)*(CS_12-CS_11);
  else if (Eb1!=Eb2)
    return CS_11 + (E-E1)/(E2-E1)*(CS_21-CS_11);
  else
    return CS_11;
  
    
  return cross_sec;
} 

double
dmtpc::physics::endf::AngularDistribution::cdf(double E, double cosPh) const
{

  if (cosPh > 1 || cosPh < -1 )
  {
    cout <<"Invalid cosine value!"<<endl;
    return -1;
  } else if (E < fEnergy[0] || E > fEnergy[fEnergy.size()-1]){
    cout <<"Invalid energy value!"<<endl;
    return -1;
  }
  
  int Eb1 = lower_bound(fEnergy.begin(),fEnergy.end(),E)-1-fEnergy.begin();
  int Cb1 = lower_bound(fCosPhi.begin(),fCosPhi.end(),cosPh)-1-fEnergy.begin();
  if (Eb1<0) Eb1 = 0;
  if (Cb1<0) Cb1 = 0;
  int Eb2 = Eb1 == fNE-1? Eb1:Eb1+1;
  int Cb2 = Cb1 == fNCosPhi-1?Cb1:Cb1+1;
  double E1 = fEnergy[Eb1];
  double E2 = fEnergy[Eb2];
  double c1 = fCosPhi[Cb1];
  double c2 = fCosPhi[Cb2];

  double CS_11 = fDCSInt[Eb1][Cb1];
  double CS_12 = fDCSInt[Eb1][Cb2];
  double CS_21 = fDCSInt[Eb2][Cb1];
  double CS_22 = fDCSInt[Eb2][Cb2];

  double cross_sec;
  if (Eb1 != Eb2 && Cb1 != Cb2)
  {//Bilinear interpolation
    //E direction first:
    double cs_c1 = (E-E1)/(E2-E1) * (CS_21-CS_11) + CS_11;
    double cs_c2 = (E-E1)/(E2-E1) * (CS_22-CS_12) + CS_12;
    //Interpolate these two points in cos(phi) direction;
    cross_sec = (cosPh - c1) / (c2-c1) * (cs_c2-cs_c1) + cs_c1;
  }else if (Eb1==Eb2 && Cb1 != Cb2)
    return CS_11 + (cosPh-c1)/(c2-c1)*(CS_12-CS_11);
  else if (Eb1!=Eb2)
    return CS_11 + (E-E1)/(E2-E1)*(CS_21-CS_11);
  else
    return CS_11;
  
    
  return cross_sec;
} 

double
dmtpc::physics::endf::AngularDistribution::inverseCDF(double E, double P) const
{

  if (E < fEnergy[0] || E > fEnergy[fEnergy.size()-1]){
    cout <<"Invalid energy value!"<<endl;
    return -1;
  }
  
  int Eb1 = lower_bound(fEnergy.begin(),fEnergy.end(),E)-1-fEnergy.begin();
  if (Eb1<0) Eb1 = 0;
  int Eb2 = Eb1 == fNE-1? Eb1:Eb1+1;
  double E1 = fEnergy[Eb1];
  double E2 = fEnergy[Eb2];
  double Efrac = (E-E1)/(E2-E1);


  vector<double> Pvec(fNCosPhi);
  for (int i = 0; i < fNCosPhi; i++)
    Pvec[i] = (1-Efrac)*fDCSInt[Eb1][i]+Efrac * fDCSInt[Eb2][i];
  
  
  int Pb1 = lower_bound(Pvec.begin(),Pvec.end(),P)-1-Pvec.begin();
  int Pb2 = Pb1 == fNCosPhi-1? Pb1:Pb1+1;

  if (Pb1 == Pb2) return fCosPhi[Pb1];

  double P1 = Pvec[Pb1];
  double P2 = Pvec[Pb2];
  double Pfrac = (P-P1)/(P2-P1);
  return fCosPhi[Pb1] + Pfrac * (fCosPhi[Pb2]-fCosPhi[Pb1]);;

} 


double
dmtpc::physics::endf::AngularDistribution::legendre(double x, double* par, int n) const
{
  if (n<1) return 1;
  double p[n];
  p[0] = 1;
  p[1] = x;
  for (int i = 1; i<n-1; i++)
  {
    p[i+1] = ((2*i+1)*x*p[i] - i*p[i-1])/(i+1);
  }
  double sum = 0;
  for (int i = 0; i<n; i++) sum += 0.5 * (2*i+1) * p[i] * par[i];
  return sum;

}

double
dmtpc::physics::endf::AngularDistribution::randomCosine(double E,TRandom* rand) const
{
  if (E < fEnergy[0]|| E> fEnergy[fEnergy.size()-1] )
  {
    cout <<"endf::AngularDistribution: Invalid energy!"<<endl;
    return -1;
  }

  TRandom* random = rand?rand:gRandom;

  return inverseCDF(E,random->Rndm());

}
