/**\file ENDFFission.hh
  Header for ENDF fission file class.
*/
#ifndef DMTPC_ENDFFISSION_HH
#define DMTPC_ENDFFISSION_HH

#include <vector>
#include <string>
class TRandom; 

namespace dmtpc{
namespace physics{
namespace endf{
/**\class Fission
Reads ENDF fission neutron spectra and generates random energies.
All energies are in keV unless otherwise specified.
*/
class
Fission{

public:

  /**Constructor
    \param endfName the filename
    \param endfDir the file directory
  */
  Fission(const char* endfName = "",const char* endfDir = "./");

  /**Destructor*/
  virtual ~Fission();

  /** Get the energy vector */
  const std::vector<double>& getE() const {return fEnergy;}
  /** Get the pdf vector */
  const std::vector<double>& getPDF() const {return fPDF;}
  /** Get the cdf vector */
  const std::vector<double>& getCDF() const {return fCDF;}

  /**Set verbosity.
     \param v the verbosity. If true outputs more text during reading.
  */
  void setVerbose(bool v=true){fVerbose =v;}
  
  /**Get the number of elements.*/
  int size() const {return fEnergy.size();}
  /**Get the maximum value of any bin.*/
  double getMax()const {return fMaxProb;}
  /**Get the minimum energy of the table.*/
  double getMinE()const{return fEnergy[0];}
  /**Get the maximum energy of the table.*/
  double getMaxE()const{return fEnergy[fEnergy.size()-1];}
  /** Set the directory holding the ENDF file
      \param dir the directory
  */
  void setENDFDir(const char* dir){fEndfDir = dir;}
  /** Set the file name
      \param name the file name
  */
  void setFileName(const char* name){fEndfName = name;}

  /**Clear the table*/
  void clear();
  /** Read an ENDF fission table
     \param endfName the file name
     \param endfDir the file directory
  */
  int readFile(const char* endfName,const char* endfDir="./");
  /**Read an ENDF fission table using the current file and directory names.*/
  int readFile();
  /**Read a 3-column text file of the form:
     
     Energy EnergyUnit Probability
     
     The energies must be in ascending order and the probabilities need 
     not be normalized.

     \param endName the file name
     \param endfDir the file directory

  */
  int readTextFile(const char* endfName,const char* endfDir="./");
  /**Read a text file using the current file and directory names.*/
  int readTextFile();

  /**Interpolate the probability density at the given energy
    \param E the energy
    \return the probability density at E using linear interpolation
  */
  double pdf(double E) const;  
  /**Interpolate the cumulative distribution function at the given energy
     \param E the energy
  */ 
  double cdf(double E) const;
  /**Get the inverse CDF (energy as function of integrated probability)
     \param P the probability
  */
  double inverseCDF(double P) const;

  /** Set the energy range used for random number generation.
      \param emin the minimum energy
      \param emax the maximum energy
  */
  void setEnergyLimits(double emin, double emax);
  /** Get a random energy according to the distribution
      \param rnd the random number generator. Set to 0 to use gRandom
  */
  double randomEnergy(TRandom* rnd = 0) const;

private:
  std::string fEndfDir;///< file directory
  std::string fEndfName;///< file name
  std::vector<double> fEnergy;///< energy
  std::vector<double> fPDF;///< pdf
  std::vector<double> fCDF;///< integrated prob./CDF

  double fMaxProb;///< maximum probability density
  double fMinP;///< Minimum probability for random energy generation 
  double fMaxP;///< Maximum probability for random energy generation
  double fMinE;///< minimum energy for random energy generation
  double fMaxE;///< maximum energy for random energy generation
  bool fVerbose;///< verbosity level
  
};

}//endf
}//physics
}//dmtpc
#endif
