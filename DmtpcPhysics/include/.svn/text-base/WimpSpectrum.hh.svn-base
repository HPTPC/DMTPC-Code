/** \file WimpSpectrum.hh
\brief Header file for the dmtpc::physics::WimpSpectrum class.
\author Jeremy Lopez
*/
#ifndef DMTPC_WIMPSPECTRUM_HH
#define DMTPC_WIMPSPECTRUM_HH
class TRandom;

namespace dmtpc{
namespace physics{

/** Calculate WIMP spectrum properties assuming a truncated Maxwell-Boltzmann distribution.

*/
class WimpSpectrum
{
  public:
    /** Constructor */
    WimpSpectrum();
    /** Destructor */
    virtual ~WimpSpectrum();
 
    /** Set the random number generator seed
        \param seed the seed
    */
    void setSeed(long seed);
 
    /** Set the WIMP density
        \param wd the density in GeV/cm^3
    */
    void setWimpDensity(double wd){fWimpDensity = wd;}
    /** Get the WIMP density */
    float getWimpDensity() const{return fWimpDensity;}
    /** Set the total detector target mass
        \param tm the mass in kg
    */
    void setTotalTargetMass(double tm){fTotalMass = tm;}
    /** Get the total detector target mass */
    float getTotalTargetMass() const{return fTotalMass;}
    /** Set the interaction cross section
        \param cs the cross section in cm^2
    */
    void setCrossSection(double cs){fCS = cs;}
    /** Get the cross section */
    float getCrossSection() const{return fCS;}
    /** Set the WIMP mass
        \param wm the WIMP mass in GeV
    */
    void setWimpMass(double wm){fWimpMass = wm*1e6;}
    /** Get the WIMP mass in keV */
    float getWimpMass() const{return fWimpMass;}
    /** Set the target nucleus mass 
        \param tm the mass in amu
    */
    void setTargetMass(double tm){fTargetMass = tm*1e6*0.931494;}
    /** Get the target nucleus mass in keV */
    float getTargetMass() const{return fTargetMass;}
    /** Set the width of the velocity distribution.
        \param v0 the velocity spread in km/s
    */
    void setV0(double v0){fV0 = v0;}
    /** Get the width of the velocity distribution */
    float getV0() const{return fV0;}
    /** Set Earth's velocity with respect to the WIMP halo
        \param vE the velocity in km/s
    */
    void setVE(double vE){fVE = vE;}
    /** Get Earth's velocity with respect to the WIMP halo */
    float getVE() const{return fVE;}
    /** Set the WIMP escape velocity from the galaxy
        \param vesc the escape velocity in km/s
    */
    void setVesc(double vesc){fVesc = vesc;}
    /** Get the escape velocity from the galaxy */
    float getVesc() const{return fVesc;}
    /** Generate an event based on the current parameters*/
    void generate();
    /** Make a ROOT tree containing many events 
        \param N the number of events
        \param file the output file
    */
    void makeTree(long N,const char* file);
    /** Get the recoil energy in keV */
    float getE() const{return fE;}
    /** Get the polar angle */
    float getTheta() const{return fTheta;}
    /** Get the azimuthal angle */
    float getPhi() const{return fPhi;}
    /** Get the polar angle of the incoming WIMP */
    float getThetaW() const{return fThetaW;}
    /** Get the azimuthal angle of the incoming WIMP */
    float getPhiW() const{return fPhiW;}
    /** Get the velocity of the incoming WIMP */
    float getVw() const{return fVw;}
    /** Get the maximum allowed energy for this event */
    float getEmax() const{return fEmax;}
    /** Get the WIMP velocity in a nonrotating galactic frame*/
    float getVgal() const{return fVgal;}
    /** Get the event rate in events per day. Calculates by Monte Carlo integration.
        \param thresh the energy threshold in keV
        \param Ntrials the number of trials used to build the distribution
    */
    double getEventRate(double thresh=0,long Ntrials = 1000000) const;

  protected:
    //Properties
    float fCS;///< Cross section
    float fTotalMass;///< Mass of detector, kg
    float fWimpDensity;///< Density of WIMPs, GeV/cm^3
    float fWimpMass;///< WIMP mass, keV
    float fTargetMass;///< Target nucleus mass, keV
    float fV0;///< Width parameter of Maxwell-Boltzmann distribution, km/s
    float fVE;///< Velocity of Earth through WIMP wind, km/s
    float fVesc;///< Escape velocity from galaxy, km/s
    //Recoil properties
    float fE;///< Recoil energy, keV
    float fTheta;///< Recoil polar angle
    float fPhi;///< Recoil zimuthal angle
    float fThetaW;///< Incoming WIMP polar angle
    float fPhiW;///< Incoming WIMP azimuthal angle
    float fVw;///< Incoming WIMP velocity
    float fEmax;///< Max recoil energy for this velocity
    float fVgal;///< WIMP velocity in nonrotating galactic frame
    //Random gen
    TRandom* fRandom;///< Random number generator
};//Wimp
}//physics
}//dmtpc
#endif
