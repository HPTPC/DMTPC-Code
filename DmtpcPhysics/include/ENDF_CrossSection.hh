/** \file ENDF_CrossSection.hh
\brief Header file for class to read ENDF cross section tables
\author Jeremy Lopez
*/
#ifndef ENDF_CROSSSECTION_HH
#define ENDF_CROSSSECTION_HH

#include <vector>
#include <string>
class TRandom;

namespace dmtpc{
namespace physics{
namespace endf{

/**\class CrossSection
Class to read out ENDF elastic scattering tables.
All energies are in keV unless otherwise specified.
*/
class
CrossSection{

public:
  /**Constructor
    \param endfName the filename
    \param endfDir the file directory
  */
  CrossSection(const char* endfName = "",const char* endfDir = "./");
  /**Destructor*/
  virtual ~CrossSection();

  /**Get vector of table energies*/
  const std::vector<double>& energy() const {return fEnergy;}
  /**Get vector of table cross sectionr*/
  const std::vector<double>& crossSection() const {return fCS;}
  /**Set verbosity level.  If true, outputs more text when reading.
    \param v the verbosity level
  */
  void setVerbose(bool v=true){fVerbose =v;}
  /**Get the number of table entries*/
  int size() const{return fEnergy.size();}
  /**Get the peak cross section in the table*/
  double getMax()const{return fMaxCS;}
  /**Get the lowest energy included in the table*/
  double getMinE()const{return fEnergy[0];}
  /**Get the highest energy included in the table*/
  double getMaxE()const{return fEnergy[fEnergy.size()-1];}

  /**Set the file directory
    \param dir the new directory
  */
  void setENDFDir(const char* dir){fEndfDir = dir;}
  /**Set the file name
    \param name the file name
  */
  void setFileName(const char* name){fEndfName = name;}

  /**Reset the table for reading out a new file.  Automatically called by readFile() and readTextFile()*/
  void clear();
  /**Reset the file name and directory and read a file
    \param endfName the file name
    \param endfDir the file direcory
  */
  int readFile(const char* endfName,const char* endfDir="./");
  /**Read an ENDF table using the current file information*/
  int readFile();
  /**Read a 3-column table of the form 

  ENERGY UNIT CROSS_SECTION

    \param endfName the file name
    \param endfDir the file directory
  */
  int readTextFile(const char* endfName,const char* endfDir="./");
  /**Read a 3-column table using the current file information*/
  int readTextFile();
  /** Get the energy at a given index
      \param the index
  */
  double energy(int i) const {return fEnergy[i];}
  /** Get the energy at a given index
      \param the index
  */
  double getEnergy(int i) const {return fEnergy[i];}
  /** Get the cross section at a given index
      \param the index
  */
  double getCrossSection(int i) const {return fCS[i];}


  /**Interpolate the cross section at the given energy
    \param E the energy
  */
  double crossSection(double E)const;  
  /**Set the local maximum for the cross section for the given limits.
    \param Emin the minimum energy
    \param Emax the maximum energy
  */
  void setEnergyLimits(double Emin, double Emax);

  /**Accept the given energy, normalized to the table maximum
     \param E the energy to accept or reject
  */
  bool acceptEnergy(double E, TRandom* rnd=0);
  /**Accept the given energy, normalized to a local maximum
     \param E the energy to accept or reject
     \param Emin the minimum energy to consider
     \param Emax the maximum energy to consider
     \param rnd a random number generator. Use 0 for gRandom.
  */
  bool acceptEnergy(double E, double Emin, double Emax, TRandom* rnd=0);

  /**Interpolate the value of the cross section at the given energy
    \param E the energy
  */
  double operator()(double E) const
         {return crossSection(E);}

private:

  std::string fEndfDir;///< File directory
  std::string fEndfName;///< File name
  std::vector<double> fEnergy;///< Energy points
  std::vector<double> fCS;///< Cross section
  double fMaxCS;///< Maximum cross section
  bool fVerbose;///< Verbosity level
  double fLocMax;///< Local max cross section within some limits
  double fMinE;///< Min energy used for local max
  double fMaxE;///< Max energy used for local max
};
}//endf
}//physics
}//dmtpc
#endif

