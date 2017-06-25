/** \file RecoilKinematics.hh
\brief Header file for the dmtpc::physics::recoil namespace
\author Jeremy Lopez
*/
#ifndef DMTPC_RECOILKINEMATICS_HH
#define DMTPC_RECOILKINEMATICS_HH

namespace dmtpc{
  namespace physics{

/** \brief A namespace for elastic scattering calculations.
Contains various kinematics formulae and other useful things such as
the Lindhard quenching factor and spin-independent and spin-dependent
form factors found in the Lewin and Smith dark matter review.

Except where otherwise specified, masses and energies must have the same units.

*/
    namespace recoil{
      /** Get the maximum possible recoil energy
          \param Ei the kinetic energy of the incoming particle
          \param Mi the mass of the incoming particle
          \param Mr the mass of the recoiling particle
      */
      double maxRecoilEnergy(double Ei,double Mi,double Mr);


      /* recoil energy from cosine scattering angle */ 
      double recoilEnergyFromCosScatterCMS(double costheta, double Ei, double Mi, double Mr); 

      /** Get the minimum energy of an incoming particle required to get a recoil energy
          \param Er the recoil energy
          \param Mi the mass of the incoming particle
          \param Mr the mass of the recoiling particle
      */
      double minIncomingEnergy(double Er,double Mi,double Mr);


      /** Get the minimum energy of the scattered particle given its initial energy
          \param Ei the kinetic energy of the incoming particle
          \param Mi the mass of the incoming particle
          \param Mr the mass of the recoiling particle
      */
      double minScatteredEnergy(double Ei,double Mi,double Mr);


      /** Get the recoil energy given a recoil angle
          \param cosThR the cosine of the recoil angle with respect to the incoming particle's direction
          \param Ei the kinetic energy of the incoming particle
          \param Mi the mass of the incoming particle
          \param Mr the mass of the recoiling particle
      */
      double recoilEnergy(double cosThR,double Ei,double Mi,double Mr);


      /** Get the cosine of the recoil angle with respect to the incoming particle's direction
          \param Er the recoil energy
          \param Ei the kinetic energy of the incoming particle
          \param Mi the mass of the incoming particle
          \param Mr the mass of the recoiling particle
      */
      double cosRecoilAngle(double Er,double Ei,double Mi,double Mr);


      /** Get the energy of the scattered particle given the scattering angle
          \param cosThS the cosine of the scattering angle
          \param Ei the initial energy of the scattered particle
          \param Mi the mass of the incoming (scattered) particle
          \param Mr the mass of the recoiling particle
      */
      double scatteredEnergy(double cosThS,double Ei,double Mi,double Mr);


      /** Get the scattering angle given the final energy of the scattered particle
          \param ES the final energy of the scattered particle
          \param Ei the initial energy of the scattered particle
          \param Mi the mass of the incoming (scattered) particle
          \param Mr the mass of the recoiling particle
      */
      double cosScatteringAngle(double ES,double Ei,double Mi,double Mr);


      /** Get the velocity (as a fraction of the speed of light) of the center of mass
          frame with respect to the lab frame.
          \param Ei the kinetic energy of the incoming particle
          \param Mi the mass of the incoming particle
          \param Mr the mass of the recoiling particle
      */
      double cmVelocity(double Ei,double Mi,double Mr);


      /** Get the energy of the recoiling particle in the center of mass frame.
          \param Ei the kinetic energy of the incoming particle
          \param Mi the mass of the incoming particle
          \param Mr the mass of the recoiling particle
      */
      double cmRecoilEnergy(double Ei,double Mi,double Mr);


      /** Get the energy of the incoming particle in the center of mass frame.
          \param Ei the kinetic energy of the incoming particle
          \param Mi the mass of the incoming particle
          \param Mr the mass of the recoiling particle
      */
      double cmIncomingEnergy(double Ei,double Mi,double Mr);


      /** Get the recoil energy in the lab frame given the recoil angle in the
          center of mass frame
          \param cosThCm the cosine of the recoil with respect to the direction of the incoming particle in the center of mass frame
          \param Ei the kinetic energy of the incoming particle
          \param Mi the mass of the incoming particle
          \param Mr the mass of the recoiling particle
      */
      double recoilEnergyFromCosCmAngle(double cosThCm,double Ei, double Mi, double Mr);


      /** Get the recoil angle in the lab frame given the recoil angle in the
          center of mass frame
          \param cosThCm the cosine of the recoil with respect to the direction of the incoming particle in the center of mass frame
          \param Ei the kinetic energy of the incoming particle
          \param Mi the mass of the incoming particle
          \param Mr the mass of the recoiling particle
      */
      double cosRecoilAngleFromCosCmAngle(double cosThCm, double Ei, double Mi, double Mr);


      /** Get the recoil angle in the center of mass frame given the recoil energy in the
          lab frame
          \param Er the recoil energy in the lab frame
          \param Ei the kinetic energy of the incoming particle
          \param Mi the mass of the incoming particle
          \param Mr the mass of the recoiling particle
      */
      double cosCmAngleFromRecoilEnergy(double Er,double Ei, double Mi, double Mr);


      /** Get the recoil angle in the center of mass frame given the recoil angle in the
          lab frame
          \param cosTh the cosine of the angle between the recoil and the incoming particle's direction
          \param Ei the kinetic energy of the incoming particle
          \param Mi the mass of the incoming particle
          \param Mr the mass of the recoiling particle
      */
      double cosCmAngleFromRecoilAngle(double cosTh,double Ei, double Mi, double Mr);


      /** Get the quenching factor (fraction of total energy loss contributing to ionization)
        \param E the recoil energy in keV
        \param Z the nucleus atomic number
        \param A the nucleus atomic weight
      */
      double quenchingLindhard(double E,double Z,double A);


      /** Get the quenching factor 
        \param E the recoil energy in keV
        \param Z the nucleus atomic number
        \param A the nucleus atomic weight
      */
      double dQdELindhard(double E,double Z,double A);


      /** Get the recoil momentum transfer squared 
        \param E the recoil energy
        \param Mr the mass of the recoiling nucleus
      */
      double recoilQSquared(double E, double Mr);


      /** Get the recoil momentum transfer
        \param E the recoil energy
        \param Mr the mass of the recoiling nucleus
      */
      double recoilQvalue(double E, double Mr);


      /** Get the squared spin-independent form factor
        \param E the recoil energy in keV
        \param A the nucleus atomic weight
      */
      double formFactorSqSI(double E, double A);


      /** Get the squared spin-dependent form factor
        \param E the recoil energy in keV
        \param A the nucleus atomic weight
      */
      double formFactorSqSD(double E, double A);


      /** Get the spin-independent form factor
        \param E the recoil energy in keV
        \param A the nucleus atomic weight
      */
      double formFactorSI(double E, double A);


      /** Get the spin-dependent form factor
        \param E the recoil energy in keV
        \param A the nucleus atomic weight
      */
      double formFactorSD(double E, double A);


    }//recoil
  }//physics
}//dmtpc
#endif
