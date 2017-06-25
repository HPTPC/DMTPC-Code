#include "SRIMTable.hh"

#include "TString.h"

#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

namespace{
double kB = 1.3806504e-23;//Boltzmann constant in g-mm^2/s^2 /K

double convertEnergy(double E, TString unit)
{
  if (unit == "eV")
    E *= 1e-3;
  else if (unit =="keV")
    E *= 1;
  else if (unit =="MeV")
    E *= 1e3;
  else if (unit =="GeV")
    E *= 1e6;
  else if (unit =="TeV")
    E *= 1e9;

  return E;
}

double convertLength(double l, TString unit)
{
  if (unit == "A")
    l *= 1e-7;
  else if (unit == "nm")
    l *= 1e-6;
  else if (unit == "um")
    l *= 1e-3;
  else if (unit == "mm")
    l *= 1;
  else if (unit == "cm")
    l *= 10;
  else if (unit == "m")
    l *= 1000;

  return l;
}

double convertMass(double m, TString unit)
{
  if (unit =="ng")
    m *= 1e-9;
  else if (unit == "ug")
    m *= 1e-6;
  else if (unit == "mg")
    m *= 1e-3;
  else if (unit == "g")
    m *= 1;
  else if (unit == "kg")
    m *= 1e3;
  return m;
}
}
dmtpc::physics::SRIMTable::SRIMTable(double pres, double temp, double atomPerMolecule)
{
  fPressure = pres;
  fTemperature = temp;
  fAtomsPerMolecule = atomPerMolecule;
  
  fNumberDensity =  fPressure * 101325/760. / (::kB*fTemperature) * 1e-9;

  fNuclIon = 0.3;
  fElecIon = 1.0;

  fFile = "";
  fFileDir = "";
  fVerbose = false;
  fCurrentIndex = 0;
  fIndexFrac = 0;

}

void
dmtpc::physics::SRIMTable::clear()
{
  fE.clear();
  fEion.clear();
  fRange.clear();
  fIonStop.clear();
  fElecStop.clear();
  fNuclStop.clear();
  fLongStraggle.clear();
  fTranStraggle.clear();
}

void
dmtpc::physics::SRIMTable::setAtomsPerMolecule(double apm)
{
  double ratio = apm/fAtomsPerMolecule;
  fTableNumberDensity /= ratio;
  fGasMass *= ratio;
  fMassDensity *= ratio;
  fAtomsPerMolecule = apm;

}

void
dmtpc::physics::SRIMTable::setNumberDensity(double nd, bool constPres)
{
  double ratio = nd / fNumberDensity;
  fNumberDensity = nd;
  fMassDensity *= ratio;
  if (constPres)
    fTemperature /= ratio;
  else
    fPressure *= ratio;

}

void
dmtpc::physics::SRIMTable::setMassDensity(double md, bool constPres)
{
  double ratio = md / fMassDensity;
  fMassDensity = md;
  fNumberDensity *= ratio;
  if (constPres)
    fTemperature /= ratio;
  else
    fPressure *= ratio;

}

void
dmtpc::physics::SRIMTable::setPressure(double pr, bool constTemp)
{
  double ratio = pr/fPressure;
  fPressure = pr;

  if (constTemp){
    fNumberDensity *= ratio;
    fMassDensity *= ratio;
  } else
    fTemperature *= ratio;

}

void
dmtpc::physics::SRIMTable::setTemperature(double tmp,bool constPres)
{
  double ratio = tmp/fTemperature;
  fTemperature = tmp;

  if (constPres){
    fNumberDensity /= ratio;
    fMassDensity /= ratio;
  }else
    fPressure *= ratio;

}

void
dmtpc::physics::SRIMTable::setPressureAndTemp(double pr, double tmp)
{
  fPressure = pr;
  fTemperature = tmp;
  fNumberDensity = fPressure*101325./760. / (::kB*fTemperature) * 1e-9;
  fMassDensity = fNumberDensity * fGasMass;
}

void
dmtpc::physics::SRIMTable::setIonizationFractions(double elec,double nucl)
{
  fElecIon = elec;
  fNuclIon = nucl;
}


int
dmtpc::physics::SRIMTable::readFile(const char* fname, const char* fdir)
{

  clear();
  fE.push_back(0);
  fEion.push_back(0);
  fRange.push_back(0);
  fIonStop.push_back(0);
  fElecStop.push_back(0);
  fNuclStop.push_back(0);
  fLongStraggle.push_back(0);
  fTranStraggle.push_back(0); 

  fFile = fname;
  fFileDir = fdir;

  string fileN(fdir);
  fileN = fileN + fname;
  ifstream fin(fileN.c_str());
  if (!fin){
    cout <<"File "<<fileN<<" not found!"<<endl;
    return -1;
  }

  string line,s1,s2,s3;
  double stoppingConst = 1;
  bool startedTable = false;
  while(!fin.eof())
  {

    getline(fin,line);
    if (line=="") continue;
    istringstream iss(line);

    if (!startedTable)
    {//read file header for info

      iss >> s1 >> s2;

      if (s1=="Stopping"&&s2=="Units")
      {
        iss >> s1 >> s1 >> s2 >> s2;
        if (fVerbose) cout <<"Table dEdx energy unit: "<<s1<<endl;
        stoppingConst *= ::convertEnergy(1,s1.c_str());

        if (fVerbose) cout <<"Table radiation length units: "<<s2<<endl;
        size_t index = s2.find_first_of("()/1234567890");
        while(index!=string::npos){
          s2.replace(index,1,1,' ');
          index = s2.find_first_of("()/1234567890");
        }
        istringstream iss2(s2);
        iss2 >> s1 >> s3;
        if (fVerbose){
          cout <<"Table mass unit: "<<s1<<endl;
          cout <<"Table length unit: "<<s3<<endl;
        }

        stoppingConst /= ::convertMass( 1, s1.c_str() );
        stoppingConst *= pow( ::convertLength(1,s3.c_str() ),2);

      }else if (s1=="Target"&&s2=="Density"){

        double nd, md;
        //Target Density = ... = ...
        iss >> s1 >>md >> s1 >>s2 >> nd >> s2;


        size_t index = s1.find_first_of("()/1234567890");
        while(index!=string::npos){
          s1.replace(index,1,1,' ');
          index = s1.find_first_of("()/1234567890");
        }

        index = s2.find_first_of("()/1234567890");
        while(index!=string::npos){
          s2.replace(index,1,1,' ');
          index = s2.find_first_of("()/1234567890");
        }

        istringstream iss2(s1);
        string s4;
        iss2 >> s3 >> s4;
        fTableMassDensity = md*::convertMass(1,s3.c_str()) * pow(::convertLength(1,s4.c_str()),-3);
        istringstream iss3(s2);
        iss3 >> s3 >> s4;
        fTableNumberDensity = nd * pow(::convertLength(1,s4.c_str()),-3) /
                             fAtomsPerMolecule;

        if (fVerbose){
           cout <<"Mass density in table is "<< fTableMassDensity*1e6 <<"g/cm^3"<<endl;
           cout <<"Table number density is "<<fTableNumberDensity*1e3<<" molecules per cm^3"<<endl;
        }
        fGasMass = fTableMassDensity / fTableNumberDensity;
        if (fVerbose) cout <<"Gas particle mass is "<<fGasMass <<" kg"<<endl;
        stoppingConst *= fTableMassDensity;
        
      }else if (s1=="-----------"&&s2=="----------"){
        startedTable = true;
        if (fVerbose){
           cout <<"Starting reading SRIM table."<<endl;
           cout <<"Conversion factor to keV/mm: "<<stoppingConst<<endl;
        }
      }

    }else
    {//fill table values

      if (line.find("----------------")!=string::npos) break;
      double en, dEdxe, dEdxn,r, ls, ts;
      string eu, ru, lsu, tsu;
      iss >> en >> eu >> dEdxe>>dEdxn >> r>>ru>>ls>>lsu>>ts>>tsu;


      fE.push_back( ::convertEnergy(en,eu) );
      fRange.push_back( ::convertLength(r,ru) );
      fElecStop.push_back(dEdxe * stoppingConst );
      fNuclStop.push_back(dEdxn * stoppingConst );
      fLongStraggle.push_back( ::convertLength(ls,lsu) );
      fTranStraggle.push_back( ::convertLength(ts,tsu) );
      fIonStop.push_back( (dEdxe*fElecIon + dEdxn*fNuclIon ) * stoppingConst );
      int sz = size()-1;
      double ionFrac;
      if (sz==1)
        ionFrac = fIonStop[sz]/(fElecStop[sz]+fNuclStop[sz]);
      else
        ionFrac = 0.5 * ( fIonStop[sz-1]/(fElecStop[sz-1]+fNuclStop[sz-1]) + fIonStop[sz]/(fElecStop[sz]+fNuclStop[sz]) );
      fEion.push_back( fEion[sz-1] + ionFrac*(fE[sz]-fE[sz-1]) );

      if (fVerbose) cout <<fE[sz]<<" "<<fEion[sz]<<" "<<fRange[sz]<<" "<<endl;
    }

  }//while loop
  fMassDensity = fNumberDensity * fGasMass;
  return 0;
}

int
dmtpc::physics::SRIMTable::interpolate(double x, InterpolationMode mode)
{
  int sz = size();
  if (mode==ENERGY)
  {
    if (x>fE[sz-1]){
      cout <<"SRIMTable: Can't interpolate: energy too high"<<endl;
      return -1;
    }
    fCurrentIndex = lower_bound(fE.begin(),fE.end(),x)-1-fE.begin();
    if (fCurrentIndex==(int)fEion.size()-1)
    {
      fIndexFrac=1; 
      fCurrentIndex--;
    }
    else
      fIndexFrac = (x-fE[fCurrentIndex])/(fE[fCurrentIndex+1]-fE[fCurrentIndex]);
  }
  else if (mode==IONIZATION)
  {
    if (x>fEion[sz-1]){
      cout <<"SRIMTable: Can't interpolate: ionization too high"<<endl;
      return -2;
    }
    fCurrentIndex = lower_bound(fEion.begin(),fEion.end(),x)-1-fEion.begin();
    if (fCurrentIndex==(int)fEion.size()-1)
    {
      fIndexFrac = 1;
      fCurrentIndex--;
    }
    else
      fIndexFrac = (x-fEion[fCurrentIndex])/(fEion[fCurrentIndex+1]-fEion[fCurrentIndex]);
  }
  else //mode==RANGE
  {
    double ratio = fNumberDensity/fTableNumberDensity;
    if (x*ratio>fRange[sz-1]){
      cout <<"SRIMTable: Can't interpolate: range too high"<<endl;
      return -3;
    }
    fCurrentIndex = lower_bound(fRange.begin(),fRange.end(),x*ratio)-1-fRange.begin();
    if (fCurrentIndex==(int)fRange.size()-1)
    {
      fIndexFrac = 0;
      fCurrentIndex--;
    }
    else 
      fIndexFrac = (x*ratio-fRange[fCurrentIndex])/(fRange[fCurrentIndex+1]-fRange[fCurrentIndex]);
  }
  return 0;
}

double
dmtpc::physics::SRIMTable::energy() const
{return fE[fCurrentIndex] + fIndexFrac*(fE[fCurrentIndex+1]-fE[fCurrentIndex]);}

double
dmtpc::physics::SRIMTable::ionization() const
{return fEion[fCurrentIndex] + fIndexFrac*(fEion[fCurrentIndex+1]-fEion[fCurrentIndex]);}

double
dmtpc::physics::SRIMTable::range() const
{return fTableNumberDensity/fNumberDensity * (fRange[fCurrentIndex] + fIndexFrac*(fRange[fCurrentIndex+1]-fRange[fCurrentIndex]) );}

double
dmtpc::physics::SRIMTable::quenchingFactor() const
{return ionization()/energy();}

double
dmtpc::physics::SRIMTable::stoppingPower() const
{return elecStoppingPower()+nuclStoppingPower();}

double
dmtpc::physics::SRIMTable::elecStoppingPower() const
{return fNumberDensity/fTableNumberDensity*(fElecStop[fCurrentIndex] + fIndexFrac*(fElecStop[fCurrentIndex+1]-fElecStop[fCurrentIndex]) );}

double
dmtpc::physics::SRIMTable::nuclStoppingPower() const
{return fNumberDensity/fTableNumberDensity * (fNuclStop[fCurrentIndex] + fIndexFrac*(fNuclStop[fCurrentIndex+1]-fNuclStop[fCurrentIndex]) );}

double
dmtpc::physics::SRIMTable::ionizationDensity() const
{return fNumberDensity/fTableNumberDensity * ( fIonStop[fCurrentIndex] + fIndexFrac*(fIonStop[fCurrentIndex+1]-fIonStop[fCurrentIndex]) );}

double
dmtpc::physics::SRIMTable::ionizationFraction() const
{return ionizationDensity()/stoppingPower();}

double
dmtpc::physics::SRIMTable::longitudinalStraggling() const
{return fTableNumberDensity/fNumberDensity*(fLongStraggle[fCurrentIndex] + fIndexFrac*(fLongStraggle[fCurrentIndex+1]-fLongStraggle[fCurrentIndex]) );}

double
dmtpc::physics::SRIMTable::transverseStraggling() const
{return fTableNumberDensity/fNumberDensity*(fTranStraggle[fCurrentIndex] + fIndexFrac*(fTranStraggle[fCurrentIndex+1]-fTranStraggle[fCurrentIndex]) );}


