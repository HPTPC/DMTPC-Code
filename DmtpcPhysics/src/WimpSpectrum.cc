/**\file WimpSpectrum.cc
\brief Source file for WimpSpectrum class
\author Jeremy Lopez
*/
#include "TRandom3.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TF1.h"
#include "TVector3.h"
#include "TRandom.h"
#include "WimpSpectrum.hh"
#include <cmath>
#include <vector>
#include <iostream>

using namespace std;

double c_light = 299792.458;//in km/s

dmtpc::physics::WimpSpectrum::WimpSpectrum()
{
  fWimpDensity = 0.3;//GeV/cm^3
  fCS = 1e-36;//1 pb
  fTotalMass = 1;//1 kg
  fV0 = 230;//km/s
  fVesc = 2300;//km/s
  fVE = 257.9114;//km/s
  setTargetMass(19);//amu
  setWimpMass(100);//GeV
  fRandom = new TRandom3(0);
}

dmtpc::physics::WimpSpectrum::~WimpSpectrum()
{
  delete fRandom;
}

void
dmtpc::physics::WimpSpectrum::setSeed(long seed)
{
  fRandom->SetSeed(seed);
}

void
dmtpc::physics::WimpSpectrum::generate()
{
  float& Mw = fWimpMass;
  float& Mt = fTargetMass;
  TVector3 vwind(0,fVE,0);
  TVector3 vwimp;
  //Velocity maximizing the probability
  double vM = 0.5 * (fVE + sqrt(fVE*fVE+6*fV0*fV0));
  double maxF = vM*vM*vM*exp(-(vM*vM-2*vM*fVE +fVE*fVE)/(fV0*fV0) );

  TVector3 unit(0,1,0);
  TVector3 orth1(1,0,0);
  TVector3 orth2(0,0,1);
  //Azimuthal angle
  double phi = 2*M_PI*fRandom->Rndm();  
  fEmax = 0;
  fVw =0;
  //Get a WIMP velocity: 
  while(1)
  {
    double cosTheta = 2*fRandom->Rndm()-1;
    double sinTheta = sqrt(1-cosTheta*cosTheta);
    //Try a velocity
    fVw = (fVesc+fVE) * fRandom->Rndm();
    vwimp = fVw*cosTheta*unit +
            fVw*sinTheta*cos(phi)*orth1 +
            fVw*sinTheta*sin(phi)*orth2;
    //Test velocity
    if ((vwind+vwimp).Mag() > fVesc) continue;//escape velocity reached
    double F = fVw*fVw*fVw * exp(-(fVw*fVw+2*fVw*fVE*cosTheta+fVE*fVE)/(fV0*fV0));
    if (F>maxF) cout <<"error!"<<endl;
    if (fRandom->Rndm()*maxF < fVw*fVw*fVw*exp(-(fVw*fVw+2*fVw*fVE*cosTheta+fVE*fVE)/(fV0*fV0) )) break;

  }//Get a wimp velocity
  fVgal = (vwind+vwimp).Mag();
  unit = vwimp.Unit();
  orth1 = vwimp.Orthogonal().Unit();
  orth2 = vwimp.Cross(orth1).Unit();
 
  fVw /= c_light;
  //Throw random energy (flat for isotropic cross section)
  fEmax = 2*Mt*Mw*Mw * fVw*fVw/( (Mt+Mw)*(Mt+Mw)  );
  fE = fRandom->Rndm()*fEmax;
  //Lab frame scattering angle
  double cosTh = sqrt(fE/fEmax);
  double sinTh = sqrt(1-cosTh*cosTh);
  phi = 2*M_PI*fRandom->Rndm();
  TVector3 dir = cosTh * unit + sinTh*cos(phi)*orth1+sinTh*sin(phi)*orth2;
  //Copy angles
  fPhi = dir.Phi();
  fTheta = dir.Theta();
  fPhiW = vwimp.Phi();
  fThetaW = vwimp.Theta();
  fVw*=c_light;
}

void 
dmtpc::physics::WimpSpectrum::makeTree(long N, const char* filename)
{
  TFile f(filename,"RECREATE");
  TTree tr("Wimps","Wimps");
  tr.Branch("Mw",&fWimpMass,"Mw/D");
  tr.Branch("Mt",&fTargetMass,"Mt/D");
  tr.Branch("v0",&fV0,"v0/D");
  tr.Branch("vE",&fVE,"vE/D");
  tr.Branch("vesc",&fVesc,"vesc/D");
  tr.Branch("E",&fE,"E/F");
  tr.Branch("theta",&fTheta,"theta/F");
  tr.Branch("phi",&fPhi,"phi/F");
  tr.Branch("theta_wimp",&fThetaW,"theta_wimp/F");
  tr.Branch("phi_wimp",&fPhiW,"phi_wimp/F");
  tr.Branch("vw",&fVw,"vw/F");
  tr.Branch("Emax",&fEmax,"Emax/F");
  tr.Branch("vgal",&fVgal,"vgal/F");
  for (long i = 0; i < N; i++)
  {
    if (i%100000==0) cout <<(100.*i)/N <<"\% done!"<<endl;
    generate();
    tr.Fill();
  }
  f.cd();
  tr.Write();
  f.Close();

}

double
dmtpc::physics::WimpSpectrum::getEventRate(double thresh,long NTrials) const
{
  //Convert to keV
  double totalMass = fTotalMass*c_light*c_light*1e6 /1.602e-16;
  double prefactor = fCS*totalMass*(fWimpDensity*1e6/*keV*/) / fTargetMass/fWimpMass;
  //Average over 
  double v=0;
  double norm = 0; 
  for (long i = 0; i < NTrials; i++)
  {

    while(1)
    {
      double vx = fRandom->Gaus(fVE,fV0/sqrt(2));
      double vy = fRandom->Gaus(0,fV0/sqrt(2));
      double vz = fRandom->Gaus(0,fV0/sqrt(2));
      if ( sqrt((vx-fVE)*(vx-fVE)+vy*vy+vz*vz)>fVesc) continue;
      double beta2 = (vx*vx+vy*vy+vz*vz)/(c_light*c_light);
      double Emax = 2*fTargetMass*fWimpMass*fWimpMass*beta2/((fTargetMass+fWimpMass)*(fTargetMass+fWimpMass)  );
      norm++;
      if (thresh>Emax) continue;
      double weight =1- thresh/Emax;

      v += weight*sqrt(vx*vx+vy*vy+vz*vz);

      break;
    }

  }

  v= v*1e5/norm;

  double rate = prefactor * v;//events per second
  return rate *24*3600;//events per day

}
