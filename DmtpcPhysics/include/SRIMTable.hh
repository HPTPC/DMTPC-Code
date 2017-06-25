/** \file DmtpcSRIM.hh
\brief Header file for SRIM table class
\author Jeremy Lopez
*/

#ifndef DMTPC_SRIMTABLE_HH
#define DMTPC_SRIMTABLE_HH

#include <string>
#include <vector>

using std::string;
using std::vector;
namespace dmtpc{
namespace physics{
 


/** \class SRIMTable
\brief Class to read SRIM output files and interpolate values.

This class outputs masses in g, lengths in mm, and energies in keV.


*/
  class SRIMTable  {

    public:

      /**Constructor

         \param pres the pressure in torr
         \param temp the temperature in K
         \param atomPerMolecule the number of atoms per molecule in the gas.
      */
      SRIMTable(double pres=100,double temp=293.15,double atomPerMolecule=1);

      /**Destructor
      */
      virtual ~SRIMTable(){;}
      //Set basic event info
      /**Set the number of atoms per molecule
         \warning recalculates number densities but does not change pressure or temperature
         \param apm the new number of atoms per molecule
      */
      void setAtomsPerMolecule(double apm);
      /**Directly set the number density of the target material and recalculate table values.
         \param nd the new number density in mm^-3
         \param constPres if false, change pressure if true, change temperature to match ideal gas law.
      */
      void setNumberDensity(double nd, bool constPres=false);
      /**Directly set the mass density of the target material and recalculate table values.
         \param md the new number density in g/mm^-3
         \param constPres if false, change pressure if true, change temperature to match ideal gas law.
      */     
      void setMassDensity(double md, bool constPres=false);
      /**Set the pressure of the target material and recalculate table values.
         \param pr the new pressure in torr
         \param constTemp if false, hold density and change temperature. If true, hold temperature and change the density.
      */
      void setPressure(double pr, bool constTemp = true);
      /**Set the temperature of the target material and recalculate table values.
         \param tmp the new temperature in K
         \param constPres if false, hold density and change pressure. If true, hold pressure and change the density.
      */
      void setTemperature(double tmp, bool constPres = false);
      /**Set the temperature and pressure of the target material, assuming an ideal gas, and recalculate table values.
         This is the recommended way to set the table values.
         \param pr the new pressure in torr
         \param tmp the new temperature in K
      */
      void setPressureAndTemp(double pr, double tmp);
      /**Set the fractions of nuclear and electronic energy loss that contribute to ionization.
         \param elec the fraction of electronic energy loss
         \param nucl the fraction of nuclear energy loss
      */
      void setIonizationFractions(double elec=1.0, double nucl=0.3);

      /** Get the number of atoms per molecule
      */
      double getAtomsPerMolecule() const{return fAtomsPerMolecule;}
      /** Get the mass of a gas molecule */
      double getGasMass() const 
             {return fGasMass;}
      /** Get the number density of the material */
      double getNumberDensity() const 
             {return fNumberDensity;}
      /** Get the mass density of the material */
      double getMassDensity() const 
             {return fMassDensity;}
      /** Get the pressure of the material */
      double getPressure() const 
             {return fPressure;}
      /** Get the temperature of the material */
      double getTemperature() const
             {return fTemperature;}
      /** Get the number density of the material in the original file */
      double getTableNumberDensity() const 
             {return fTableNumberDensity;}
      /** Get the mass density of the material in the original file */
      double getTableMassDensity() const
             {return fTableMassDensity;}
      /** Get the fraction of electronic energy loss contributing to ionization */
      double getElectronicIonizationFraction() const {return fElecIon;}
      /** Get the fraction of nuclear energy loss contributing to ionization */
      double getNuclearIonizationFraction() const {return fNuclIon;}   

 
      /** Get the number of entries in the table
      */
      int size() const{return fE.size();}
      /** Clear all table entries */
      void clear();
      /** Read a SRIM file to set raw table values and then use current 
          density information to set current table values.
          \param fname the file name
          \param fdir the directory.
          \warning the directory must include the final slash
          \return 0 if successful
      */
      int readFile(const char* fname = "SRIM_F_in_CF4_100Torr",const char* fdir="./");

      /**Set verbosity
         \param v if true output more information during reading
      */
      void setVerbose(bool v=true){fVerbose = v;}

      /**Get the current SRIM filename*/
      string getFile() const {return fFile;}
      /**Get the directory of the current SRIM file*/
      string getFileDirectory() const {return fFileDir;}

 
      /** Different ways to interpolate SRIM table results */
      enum InterpolationMode {ENERGY,IONIZATION,RANGE};
      /** Find the index at which the given table reaches value x. Saves both this index
          and the fraction (using linear interpolation) between this index and the next that
          the table reaches the exact value.
          \param x the value at which the interpolation is wanted
          \param mode the table to interpolate
          \return 0 if successful
      */
      int interpolate(double x, InterpolationMode mode);
      /** Get the current index used in interpolation */
      int getCurrentIndex() const {return fCurrentIndex;} 
      /** Get the fractional index used in the current interpolation */
      double getIndexFraction() const {return fIndexFrac;}
      /** Get the energy */
      double energy() const;
      /** Get the quenched energy (amount of ionization) */
      double ionization() const;
      /** Get the range */
      double range() const;
      /** Get the (integrated) quenching factor */
      double quenchingFactor() const;
      /** Get the total stopping power */
      double stoppingPower() const;
      /** Get the electronic stopping power */
      double elecStoppingPower() const;
      /** Get the nuclear stopping power */
      double nuclStoppingPower() const;
      /** Get the fraction of the stopping power contributing to ionization */
      double ionizationFraction() const;
      /** Get the stopping power contributing to ionization */
      double ionizationDensity() const;
      /** Get the longitudinal straggling */
      double longitudinalStraggling() const; 
      /** Get the transverse straggling */
      double transverseStraggling() const;
      /** Get the energy at a given place in the SRIM table
          \param i the index 
      */
      double energy(int i) const{return fE[i];}
      /** Get the quenched energy (ionization) at a given place in the SRIM table
          \param i the index 
      */
      double ionization(int i) const{return fEion[i];}
      /** Get the range (not corrected for the current density) at a given place in the SRIM table
          \param i the index 
      */
      double range(int i) const{return fRange[i];}
      /** Get the integrated quenching factor at a given place in the SRIM table
          \param i the index 
      */
      double quenchingFactor(int i) const{return i==0?0: fEion[i]/fE[i];}
      /** Get the total stopping power (not corrected for the current density) at a given place in the SRIM table
          \param i the index 
      */
      double stoppingPower(int i) const{return fNuclStop[i]+fElecStop[i];}
      /** Get the electronic stopping power (not corrected for the current density) at a given place in the SRIM table
          \param i the index 
      */
      double elecStoppingPower(int i) const{return fElecStop[i];}
      /** Get the nuclear stopping power (not corrected for the current density) at a given place in the SRIM table
          \param i the index 
      */
      double nuclStoppingPower(int i) const{return fNuclStop[i];}
      /** Get the stopping power contributing to ionization (not corrected for the current density) at a given place in the SRIM table
          \param i the index 
      */
      double ionizationDensity(int i) const{return fIonStop[i];}
      /** Get the fraction of the stopping power contributing to ionization at a given place in the SRIM table
          \param i the index 
      */
      double ionizationFraction(int i) const{return i==0?0:ionizationDensity(i)/stoppingPower(i);}
      /** Get the longitudinal straggling (not corrected for the current density) at a given place in the SRIM table
          \param i the index 
      */
      double longitudinalStraggling(int i) const{return fLongStraggle[i];}
      /** Get the transverse straggling (not corrected for the current density) at a given place in the SRIM table
          \param i the index 
      */
      double transverseStraggling(int i) const{return fTranStraggle[i];}
 
    private:
    
      std::vector<double> fE;///< Energy
      std::vector<double> fEion;///< Quenched energy
      std::vector<double> fRange;///< Range
      vector<double> fIonStop;///< Ionization stopping power
      vector<double> fElecStop;///< Electronic stopping power
      vector<double> fNuclStop;///< Nuclear stopping power
      vector<double> fLongStraggle;///< Longitudinal straggling
      vector<double> fTranStraggle;///< Transverse straggling

      double fGasMass;///< Average mass of a gas molecule
      double fTableMassDensity;///< SRIM file mass density
      double fTableNumberDensity;///< SRIM file number density
      double fAtomsPerMolecule;///< Number of atoms per molecule
      double fMassDensity;///< Scaled mass density
      double fNumberDensity;///< Scaled number density
      double fPressure;///< Pressure
      double fTemperature;///< Temperature

      double fNuclIon;///< Nuclear ionization fraction
      double fElecIon;///< Electronic ionization fraction

      std::string fFile;///< Current SRIM file
      std::string fFileDir;///< Current SRIM file directory
      bool fVerbose;///< Verbosity level

      int fCurrentIndex;///< The index for the current interpolation
      double fIndexFrac;///< The fraction between the current index and the next in the current interpolation


  };


  } 
}
#endif
