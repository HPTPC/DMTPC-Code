/**\file RecoilKinematics.cc
\brief Implementation of dmtpc::physics::recoil namespace
\author Jeremy Lopez
*/

#include "RecoilKinematics.hh"
#include <cmath>

#ifndef M_PI
 #define M_PI 3.1415926535897932384626433832795029L
#endif

double
dmtpc::physics::recoil::maxRecoilEnergy(double Ei,double Mi,double Mr)
{
  return 4*Mi*Mr*Ei/( (Mi+Mr)*(Mi+Mr) );
}

double 
dmtpc::physics::recoil::recoilEnergyFromCosScatterCMS(double costh, double Ei, double Mi, double Mr)
{
  return 2 * Ei* Mr*Mi/pow(Mi+Mr,2) * (1 - costh); 
}


double
dmtpc::physics::recoil::minIncomingEnergy(double Ei,double Mi,double Mr)
{
  return 4*Mi*Mr*Ei/( (Mi+Mr)*(Mi+Mr) );
}

double
dmtpc::physics::recoil::minScatteredEnergy(double Ei,double Mi,double Mr)
{
  return Ei - maxRecoilEnergy(Ei,Mi,Mr);
}

double
dmtpc::physics::recoil::recoilEnergy(double cosThR,double Ei,double Mi,double Mr)
{
  double Emax = maxRecoilEnergy(Ei,Mi,Mr);
  return Emax * cosThR*cosThR;
}

double
dmtpc::physics::recoil::cosRecoilAngle(double Er,double Ei,double Mi,double Mr)
{
  double Emax = maxRecoilEnergy(Ei,Mi,Mr);
  return sqrt(Er/Emax);
}

double
dmtpc::physics::recoil::scatteredEnergy(double cosThS,double Ei,double Mi,double Mr)
{
  double A = (Mi+Mr)*(Mi+Mr);
  double B = 2*Ei*( Mi*Mi*(1-0.5*cosThS*cosThS)-Mr*Mr );
  double C = (Mi-Mr)*(Mi-Mr)*Ei*Ei;
  return (sqrt(B*B-4*A*C)-B)/(2*A);
}

double
dmtpc::physics::recoil::cosScatteringAngle(double Es,double Ei,double Mi,double Mr)
{

 return (1-Mr/Mi)*sqrt(Ei/Es) + (1+Mr/Mi)*sqrt(Es/Ei);
}

double
dmtpc::physics::recoil::cmVelocity(double Ei,double Mi,double Mr)
{
  return sqrt(2*Mi*Ei)/(Mi+Mr);
} 

double
dmtpc::physics::recoil::cmRecoilEnergy(double Ei, double Mi,double Mr)
{
  double u = cmVelocity(Ei,Mi,Mr);
  return 0.5 * Mr * u*u;
}

double
dmtpc::physics::recoil::cmIncomingEnergy(double Ei, double Mi, double Mr)
{
  double v = sqrt(2*Ei/Mi) - cmVelocity(Ei,Mi,Mr);
  return 0.5 * Mi * v * v;
}

double 
dmtpc::physics::recoil::recoilEnergyFromCosCmAngle(double cosThCm,double Ei, double Mi, double Mr)
{
  double ucm = cmVelocity(Ei,Mi,Mr);//frame velocity
  double Ecm = cmRecoilEnergy(Ei,Mi,Mr);//
  double vcm = sqrt(2*Ecm/Mr);//recoil velocity in CM frame
  double sinThCm = sqrt(1-cosThCm*cosThCm);
  double v1 = ucm + vcm*cosThCm;
  double v2 = vcm * sinThCm;
  return 0.5 * Mr * (v1*v1 + v2*v2);
}

double 
dmtpc::physics::recoil::cosRecoilAngleFromCosCmAngle(double cosThCm, double Ei, double Mi, double Mr)
{
  double ucm = cmVelocity(Ei,Mi,Mr);//frame velocity
  double Ecm = cmRecoilEnergy(Ei,Mi,Mr);//
  double vcm = sqrt(2*Ecm/Mr);//recoil velocity in CM frame
  double sinThCm = sqrt(1-cosThCm*cosThCm);
  double v1 = ucm + vcm*cosThCm;
  double v2 = vcm * sinThCm;
  return v1/sqrt(v1*v1+v2*v2);
}

double
dmtpc::physics::recoil::cosCmAngleFromRecoilEnergy(double Er,double Ei, double Mi, double Mr)
{
  double ucm = cmVelocity(Ei,Mi,Mr);//frame velocity
  double Ecm = cmRecoilEnergy(Ei,Mi,Mr);//
  double vcm = sqrt(2*Ecm/Mr);//recoil velocity in CM frame
  return (2*Er/Mr - vcm*vcm-ucm*ucm)/(2*vcm*ucm);
}

double
dmtpc::physics::recoil::cosCmAngleFromRecoilAngle(double cosTh,double Ei, double Mi, double Mr)
{
  double ucm = cmVelocity(Ei,Mi,Mr);//frame velocity
  double Er = recoilEnergy(cosTh,Ei,Mi,Mr);
  double v = sqrt(2*Er/Mr);
  double sinTh = sqrt(1-cosTh*cosTh);
  double v1 = v*cosTh-ucm;
  double v2 = v*sinTh;
  return v1/sqrt(v1*v1+v2*v2);
}

double
dmtpc::physics::recoil::quenchingLindhard(double E,double Z,double A)
{
  double eps = 11.5*E*pow(Z,-7./3);
  double k = 0.133*pow(Z,2./3)*sqrt(A);
  double g = 3 * pow(eps,0.15)+0.7*pow(eps,0.6)+eps;
  return k*g / (1+k*g);
}

double
dmtpc::physics::recoil::dQdELindhard(double E,double Z,double A)
{
  double eps = 11.5*E*pow(Z,-7./3);
  double k = 0.133*pow(Z,2./3)*sqrt(A);
  double g = 3 * pow(eps,0.15)+0.7*pow(eps,0.6)+eps;
  double dgdE = 3*0.15*pow(eps,-0.85) + 0.7*0.6*pow(eps,-0.4) + 1.;
  double den = 1+k*g;
  return k*g/den + E* dgdE*( k/den - k*k*g/(den*den));
}

double 
dmtpc::physics::recoil::recoilQSquared(double E, double Mr)
{
  return 2 * E * Mr;
}

double 
dmtpc::physics::recoil::recoilQvalue(double E,double Mr)
{return sqrt(2*E*Mr);}

const double qrConst = 6.92e-3;//
const double rnConst = 7./3 * M_PI*M_PI*0.52*0.52;

double 
dmtpc::physics::recoil::formFactorSI(double E, double A)
{
  if (E<=0) return 1;
  double cbrtA = pow(A,1./3);
//  double rrms = 0.89 * cbrtA + 0.3;//rms radius in fm
  double s = 1;//fm
  double c = 1.23 * cbrtA - 0.6;//fm
  double rn = sqrt(c*c+rnConst-5*s*s);
  double qr0 = qrConst * sqrt(A*E)*rn;
  double qs = qrConst * sqrt(A*E)*s;
  return 3*(sin(qr0)-qr0*cos(qr0) ) /(qr0*qr0*qr0) * exp(-0.5*qs*qs );
}

double
dmtpc::physics::recoil::formFactorSqSI(double E, double A)
{
  double ff = formFactorSI(E,A);
  return ff*ff;
}

double ffSDconst = sqrt(0.047);

double
dmtpc::physics::recoil::formFactorSD(double E, double A)
{
  if (E<=0) return 1;
  double qrn = qrConst * sqrt(E) * pow(A,5./6);
  if (qrn<2.55 || qrn>4.5)
    return sin(qrn)/qrn;
  return ffSDconst;
}

double
dmtpc::physics::recoil::formFactorSqSD(double E, double A)
{
  double ff = formFactorSD(E,A);
  return ff*ff;
}
