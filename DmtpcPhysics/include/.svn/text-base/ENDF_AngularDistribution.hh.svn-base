#ifndef ENDF_DiffCS_HH
#define ENDF_DiffCS_HH

#include <vector>
#include <string>
class TRandom;


namespace dmtpc{
namespace physics{
namespace endf{
/**\brief Class to read ENDF differential cross section files.

This type of ENDF file typically reports the angular distributions via a list of Legendre 
coefficients at different energies. The angle is also usually the neutron scattering angle
in the lab frame rather than the recoil angle. 
Make sure you know what the table is actually reporting before using it.

*/
class AngularDistribution
{

public:

  /**Constructor
    \param nPh the number of angles to calculate
  */
  AngularDistribution(int nPh=40);

  /**Destructor*/
  virtual ~AngularDistribution();

  /**Read a file from the given file information
    \param name the file name
    \param the file directory
    \warning the directory must include the final slash
    \return 0 if successful
  */
  int readFile(const char* name,const char* dir="./");
  /**Read an ENDF file from the current file information
     \return 0 if successful
  */
  int readFile();

  /**Get the file directory*/
  const char* getFileDir() const {return fEndfDir.c_str();}
  /**Get the file name*/
  const char* getFileName() const {return fEndfName.c_str();}

  /**Get the number of energy points*/
  int getNE()const{return fNE;}
  /**Get the number of angle points*/
  int getNPhi()const{return fNCosPhi;}
  /**Set the number of angle points. 
     \warning Only use prior to reading a file. Otherwise you will likely get errors. 
  */
  void setNPhi(int nPh);
  /**Set the verbosity level.
    \param v the verbosity level.  True for more output.
  */
  void setVerbose(bool v = true){fVerbose = v;}
  /**Get the maximum value of the differential cross section*/
  double getMaxCS()const{return fMaxCS;}
  /**Get the minimum value of the differential cross section*/
  double getMinCS()const{return fMinCS;}
  /**Set the file directory
    \param dir the directory
  */
  void setENDFDir(const char* dir){fEndfDir = dir;}
  /**Set the file name
    \param name the file name
  */
  void setENDFName(const char* name){fEndfName = name;}

  /**Get the vector of energies*/
  const std::vector<double>& getE() const {return fEnergy;}
  /**Get the vector of angles*/
  const std::vector<double>& getCosPhi() const {return fCosPhi;}
  /**Get the vector of cross sections*/
  const std::vector< std::vector<double> >& getDifferentialCS() const {return fDCS;}
  /**Interpolate the cross section at the given values
     \param E the energy
     \param cosPh the cosine of the neutron scattering angle
  */
  double crossSection(double E, double cosPh) const;
  /**Interpolate the angular distribution CDF at the given values. Note that the angle is the neutron scattering 
     angle not the recoil angle
     \param E the energy
     \param cosPh the cosine of the angle
  */
  double cdf(double E, double cosPh) const;
  /**Get the cosine of the scattering angle given an energy and a probability (0 is backwards, 1 is forward)
     angle not the recoil angle
     \param E the energy
     \param P the probability
  */
  double inverseCDF(double E, double P) const;
  /**Generate a random cosine at the given energy
     \param E the energy
     \param rnd the random number generator. Set to 0 to use gRandom.
  */
  double randomCosine(double E, TRandom* rnd = 0) const;

private:
  /**Legendre polynomial calculator
    \param x the argument of the polynomial
    \param par the coefficient of each term
    \param n the number of terms
  */
  double legendre(double x, double* par, int n) const;

  bool fVerbose;///< The verbosity
  std::string fEndfDir;///< The file directory
  std::string fEndfName;///< The file name
  int fNE;///< The number of energy points
  int fNCosPhi;///< the number of angle points
  double fMaxCS;///< The maximum cross section
  double fMinCS;///< The minimum cross section
  std::vector<double> fEnergy;///< The energies
  std::vector<double> fCosPhi;///< The angles at which the cross sections are calculated
  std::vector< std::vector<double> > fDCS;///< The differential cross section, built from Legendre coefficients
  std::vector< std::vector<double> > fDCSInt;///< The integrated differential cross section
};

}
}
}

#endif
