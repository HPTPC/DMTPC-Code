#include "SkimEvent.hh"
#include "TDatime.h"
#include "TTimeStamp.h"
#include <stdlib.h>
#include <utility>
#include <assert.h>
#include <cmath>

//#include "ScopeWaveform.hh"

#include "TMath.h"
#include "TH2F.h"
#include "TH1F.h"

//#include "WaveformVector.hh"
//#include "CspWfVector.hh"
//#include "FastWfVector.hh"
//#include "PMTWfVector.hh"

#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

ClassImp(dmtpc::analysis::SkimEvent); 

dmtpc::analysis::SkimEvent::SkimEvent()
{
  _sparkref = NULL;    
  _burnin = NULL;    
}

bool dmtpc::analysis::SkimEvent::isRBI(int cam, int track, int nposthresh, int sparkrefbins, int binning) const 
{
  //Do nburnin check
  if (nposthresh != -1 && nburnin(cam,track) > nposthresh)
  {
    return true; 
  }
  
  if (sparkrefbins > -1)
  {
     double xb = x(cam,track)/binning; 
     double yb = y(cam,track)/binning; 
   
     for (int i = 0; i < nsparkref(cam); i++)
     {
       if ( fabs(xb - sparkrefX(cam,i)) <= sparkrefbins && 
            fabs(yb - sparkrefY(cam,i)) <= sparkrefbins   ) 
          {
            return true; 
          }
     } 
  }

  return false; 
}


void dmtpc::analysis::SkimEvent::copySparkRef(std::vector<std::pair<int,int> > * this_sparkref) 
{

  _s4 = 0; 
  for (int c = 0; c < _ncamera; c++)
  {
    _sparkref_base[c] = _s4; 
    _nsparkref[c] = this_sparkref[c].size(); 
    _s4+=_nsparkref[c];
  }

  _sparkref = (UInt_t*) malloc(_s4*sizeof(UInt_t)); 

  int j = 0; 
  for (int c = 0; c < _ncamera; c++)
  {
    for (int i = 0; i < _nsparkref[c]; i++)
    {
      _sparkref[j++] = encodeSparkRef(this_sparkref[c].at(i).first,this_sparkref[c].at(i).second);
    } 
  }
}

void dmtpc::analysis::SkimEvent::clearTrackVectors()
{
  _nburnin.clear();
  _burnin_base.clear();
  _burnin_this_index.clear();
  _theta.clear();
  _phi.clear();
  _range.clear();
  _diffusedRange.clear();
  _majoraxis.clear();
  _minoraxis.clear();
  _E.clear();
  _EGainMap.clear();
  _x.clear();
  _y.clear();
  _skewness.clear();
  _edge.clear();
  _cluster_mean.clear();
  _cluster_rms.clear();
  _neighbors.clear();
  _maxpixel.clear();
  _cygnus_angle.clear();
  _npixel.clear();
  _npixel_red.clear();
  _moments.clear();
  _transverse_moments.clear();
  _ra.clear();
  _dec.clear();
  _glon.clear();
  _glat.clear();
  _inactive.clear();
  _crossing.clear();
  _r.clear();
  _nearest_spacer_distance.clear(); 
  _nearest_spacer.clear(); 


}

void dmtpc::analysis::SkimEvent::addTrackVector(int ntracks)
{
  _nburnin.push_back( std::vector<int>(ntracks));
  _burnin_base.push_back( std::vector<int>(ntracks));
  _burnin_this_index.push_back( std::vector<int>(ntracks));
  _theta.push_back( std::vector<double>(ntracks));
  _phi.push_back( std::vector<double>(ntracks));
  _range.push_back( std::vector<double>(ntracks));
  _diffusedRange.push_back( std::vector<double>(ntracks));
  _majoraxis.push_back(std::vector<double>(ntracks));
  _minoraxis.push_back(std::vector<double>(ntracks));
  _E.push_back( std::vector<double>(ntracks));
  _EGainMap.push_back( std::vector<double>(ntracks));
  _x.push_back( std::vector<double>(ntracks));
  _y.push_back( std::vector<double>(ntracks));
  _skewness.push_back( std::vector<double>(ntracks));
  _edge.push_back( std::vector<bool>(ntracks));
  _cluster_mean.push_back( std::vector<double>(ntracks));
  _cluster_rms.push_back( std::vector<double>(ntracks));
  _neighbors.push_back( std::vector<int>(ntracks));
  _maxpixel.push_back( std::vector<double>(ntracks));
  _cygnus_angle.push_back( std::vector<double>(ntracks));
  _npixel.push_back( std::vector<int>(ntracks));
  _npixel_red.push_back( std::vector<int>(ntracks));
  _moments.push_back( std::vector<std::vector<double> >(4,std::vector<double>(ntracks)));
  _transverse_moments.push_back( std::vector<std::vector<double> >(4,std::vector<double>(ntracks)));
  _ra.push_back( std::vector<double>(ntracks));
  _dec.push_back( std::vector<double>(ntracks));
  _glon.push_back( std::vector<double>(ntracks));
  _glat.push_back( std::vector<double>(ntracks));
  _inactive.push_back( std::vector<double>(ntracks));
  _crossing.push_back( std::vector<double>(ntracks));
  _r.push_back( std::vector<double>(ntracks));
  _nearest_spacer_distance.push_back(std::vector<double>(ntracks)); 
  _nearest_spacer.push_back(std::vector<int>(ntracks)); 
}

void dmtpc::analysis::SkimEvent::initCamVectors(int ncam) 
{
  _ntracks.clear(); _ntracks.insert(_ntracks.begin(),ncam,0); 
  _spark.clear(); _spark.insert(_spark.begin(),ncam,0); 
  _lastspark.clear(); _lastspark.insert(_lastspark.begin(),ncam,0);
  _integral.clear(); _integral.insert(_integral.begin(),ncam,0); 
  _pixels_killed.clear(); _pixels_killed.insert(_pixels_killed.begin(),ncam,0); 
  _image_mean.clear(); _image_mean.insert(_image_mean.begin(),ncam,0); 
  _image_rms.clear(); _image_rms.insert(_image_rms.begin(),ncam,0); 
  _nsparkref.clear(); _nsparkref.insert(_nsparkref.begin(),ncam,0); 
  _sparkref_base.clear(); _sparkref_base.insert(_sparkref_base.begin(),ncam,0); 
  _cameraSerialNumber.clear(); _cameraSerialNumber.insert(_cameraSerialNumber.begin(),ncam,0);
}

void dmtpc::analysis::SkimEvent::initVectors(int ncam, int ntracks)
{
  initCamVectors(ncam); 
  for (int i = 0; i < ncam ;i++)
  addTrackVector(ntracks); 
}

void dmtpc::analysis::SkimEvent::copyBurnin(std::vector<std::vector<std::vector<BurninEncoded_t> > > * this_burnin)
{
  assert((int) this_burnin->size() == _ncamera);
 
  // Figure out how much space to allocate... ( and set up _burnin_base )
  int s = 0; 
  for (int c = 0; c < _ncamera; c++)
  {
    for (int t = 0; t < ntracks(c); t++)
    {
      if (t >=15) break; 
      int this_size = t >= ntracks(c)? 0 : this_burnin->at(c).at(t).size();
        
      _nburnin[c][t] = this_size;
      _burnin_base[c][t] = s; 
      s += this_size; 
    }  
  }
  
  
  _s3 = s ;
  _burnin =  (UInt_t *)  malloc(sizeof(UInt_t) * _s3); 

  int cnt = 0; 
  // Copy to flat array 
  for (int c = 0; c < _ncamera; c++)
  {
    for (int t = 0; t < ntracks(c); t++)
    {
      if (t>=15) break; 
      for (int i = 0; i < nburnin(c,t); i++)
      {
        BurninEncoded_t brn = this_burnin->at(c)[t][i]; 
        if ( decodeBurninEvent(brn) < _index ) 
        {
            _burnin_this_index[c][t]++ ; 
        }
        _burnin[cnt++] = brn; 
      } 
    }
  }
}


void dmtpc::analysis::SkimEvent::printOutCamera(int c, std::ostream & out)
{
    out << "EventNumber: "<< eventNumber() <<"   Camera " << c << ":" << std::endl;
    out << "\tntracks: " << ntracks(c) << " spark: " << spark(c) << " rms: " << image_rms(c) << std::endl;
    out << "\tpixels killed: "<< + pixels_killed(c)  << " image mean: " << image_mean(c) << std::endl; 
}


void dmtpc::analysis::SkimEvent::printOutTrack(int c, int t, std::ostream & out)
{

    out << "\t Track " << t <<":" << std::endl;
    out << "\t\tE: " << E(c,t) << " range: " << range(c,t) << " phi: " << phi(c,t) << std::endl;
    out << "\t\tx: " << x(c,t) << " y: " << y(c,t) << " theta: " << theta(c,t) << std::endl;
    out << "\t\tskewness: " << skewness(c,t) << " clustrms: " << cluster_rms(c,t) << " clustmean: " << cluster_mean(c,t) << std::endl;
    out << "\t\tmaxpixel: " << maxpixel(c,t) << " cygang: " << cygnus_angle(c,t) << " edge: " << edge(c,t) << std::endl;
    out << "\t\tneighbors: " << neighbors(c,t) << " npixel: " << npixel(c,t) << std::endl;
    out << "\t\tra: " << ra(c,t) << " dec: " << dec(c,t) << std::endl; 
    out << "\t\t moment_1: " << moment(c,1,t) << " moment_2: " << moment(c,2,t) << " moment_3: " << moment(c,3,t) << " moment_4: " << moment(c,4,t) << std::endl;  
    out << "\t\t transverse_moment_1: " << transverse_moment(c,1,t) << " transverse_moment_2: " << transverse_moment(c,2,t) << " transverse_moment_3: " << transverse_moment(c,3,t) << " transverse_moment_4: " << transverse_moment(c,4,t) << std::endl;  
    out << "\t nburnin: " << nburnin(c,t) <<" ra: " << ra(c,t) << " dec: " << dec(c,t) << " glon: " << glon(c,t) << " glat: " << glat(c,t) << endl; 
    
    int sp = 0; 
    for (int b = 0; b < nburnin(c,t); b++)
    {
      if (sp % 8 == 0) out << "\t\t"; 
      out << "(" << burnin_event(c,t,b) << "," << burnin_track(c,t,b) << ") ";
      if (++sp % 8 ==0) out << endl;
    }
    
    out << endl; 

}

void dmtpc::analysis::SkimEvent::printOut(std::ostream & out)
{
  
  for (int c = 0; c < _ncamera; c++)
  {
    
    printOutCamera(c,out);
    for (int t = 0; t < ntracks(c); t++)
    {
      printOutTrack(c,t,out); 
    }
  }
}

void dmtpc::analysis::SkimEvent::printOut(int c, int t, std::ostream & out)
{
  printOutCamera(c,out); 
  printOutTrack(c,t,out); 
}

int dmtpc::analysis::SkimEvent::findSerialNumber(TString serialNumber)
{
   for(int i=0; i<int(_cameraSerialNumber.size()); i++)
   {
      if(_cameraSerialNumber[i]==serialNumber)
      return i;
   }

//   cout << "Camera serial number " << serialNumber << " not found.\n";
   return -1;
      
}


dmtpc::analysis::SkimEvent::SkimEvent(const dmtpc::analysis::SkimEvent &other, bool diet) : TObject() {
  // Copy constructor

  _eventNumber=other._eventNumber;
  _runNumber=other._runNumber;
  _index = other._index; 
  if(!diet)
  {
    for (unsigned i = 0; i < other._clusters.size(); i++)
    {
    _clusters.push_back((ClusterImage*) other._clusters[i]->Clone()); 
    }
  }

  for (unsigned i = 0; i < other._waveform_vectors.size(); i++)
  {
    _waveform_vectors.push_back((dmtpc::waveform::WaveformVector*) other._waveform_vectors[i]->Clone()); 
  }
  for (unsigned i = 0; i < other._cspwf_vectors.size(); i++)
    {
      _cspwf_vectors.push_back((dmtpc::waveform::CspWfVector*) other._cspwf_vectors[i]->Clone());
    }
  for (unsigned i = 0; i < other._fastwf_vectors.size(); i++)
    {
      _fastwf_vectors.push_back((dmtpc::waveform::FastWfVector*) other._fastwf_vectors[i]->Clone());
    }
  for (unsigned i = 0; i < other._pmtwf_vectors.size(); i++)
    {
      _pmtwf_vectors.push_back((dmtpc::waveform::PMTWfVector*) other._pmtwf_vectors[i]->Clone());
    }


  _ncamera = other._ncamera; 
  _s3 = other._s3; 
  _s4 = other._s4; 

  _theta = other._theta;
  _phi = other._phi;
  _E = other._E;
  _range = other._range;
  _x = other._x;
  _y = other._y;
  _skewness = other._skewness;
  _edge = other._edge;
  _ntracks = other._ntracks;
  _spark = other._spark;
  _lastspark = other._lastspark;
  _integral = other._integral;
  _cluster_rms = other._cluster_rms;
  _cluster_mean = other._cluster_mean;
  _neighbors = other._neighbors;
  _npixel = other._npixel;
  _npixel_red = other._npixel_red;
  _maxpixel = other._maxpixel;
  _cygnus_angle = other._cygnus_angle;
  _moments = other._moments;
  _transverse_moments = other._transverse_moments;
  _dec = other._dec;
  _ra = other._ra;
  _glon = other._glon; 
  _glat = other._glat; 
  _image_mean = other._image_mean; 
  _image_rms = other._image_rms; 
  _pixels_killed = other._pixels_killed; 
 
  _nburnin = other._nburnin; 
  _burnin_this_index = other._burnin_this_index; 
  _burnin_base = other._burnin_base; 

  _sparkref_base = other._sparkref_base; 

  _inactive = other._inactive; 
  _crossing = other._crossing; 

  _burnin = (UInt_t* ) malloc(_s3 * sizeof(UInt_t)); 
  memcpy(_burnin,other._burnin,_s3*sizeof(UInt_t)); 

  _sparkref = (UInt_t*) malloc(_s4 * sizeof(UInt_t)); 
  memcpy (_sparkref, other._sparkref, _s4*sizeof(UInt_t)); 

  _nearest_spacer_distance = other._nearest_spacer_distance; 
  _nearest_spacer= other._nearest_spacer; 

}

dmtpc::analysis::SkimEvent::~SkimEvent() 
{
  if (_sparkref!=NULL)
    free(_sparkref); 
  if (_burnin!=NULL)
    free(_burnin); 

  for (unsigned i = 0; i < _clusters.size(); i++)
  {
    _clusters[i]->Delete(); 
  }
 
  for (unsigned i = 0; i < _waveform_vectors.size(); i++)
  {
    _waveform_vectors[i]->Delete(); 
  }
  for (unsigned i = 0; i < _cspwf_vectors.size(); i++)
  {
    _cspwf_vectors[i]->Delete();
  }
  for (unsigned i = 0; i < _fastwf_vectors.size(); i++)
  {
    _fastwf_vectors[i]->Delete();
  }
  for (unsigned i = 0; i < _pmtwf_vectors.size(); i++)
    {
      _pmtwf_vectors[i]->Delete();
    }
}


