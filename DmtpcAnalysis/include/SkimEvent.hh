#ifndef DMTPC_ANALYSIS_SKIM_EVENT_HH
#define DMTPC_ANALYSIS_SKIM_EVENT_HH

#include "TClonesArray.h"
#include "TObject.h"
#include "ClusterImage.hh"
#include "WaveformVector.hh"
#include "CspWfVector.hh"
#include "FastWfVector.hh"
#include "PMTWfVector.hh"
#include "TDatime.h"
#include "TTimeStamp.h"
#include <ostream>
#include <iostream>
#include <vector>
#include <utility>




class TTree; 
class TDatime;
class TTimeStamp;
class TH1F;
class TH2F;

namespace dmtpc
{
  namespace core
  {
    class Event; 
    class Dataset; 
  }
  namespace waveform
   {
     //    class WaveformVector;
     //class CspWfVector;
     //class FastWfVector;
     //class PMTWfVector;
  }
  namespace analysis
  {
    class SkimDataset; 
    class Cluster; 
    class GainMap; 
    class Stitcher; 

    namespace cleanskim
    {
      class Config; 
      class Functions; 
    }


  /** The first four bits of BurninEncoded_t are used to store
   * the track (which has a value between 0 and 14).
   * 
   * The last 28 bits are used to store the event number (overkill, but yeah) 
   * 
   * You can use decodeBurninTrack, decodeBurninEvent and encodeBurnin to 
   * deal. 
   * 
   **/
  typedef UInt_t BurninEncoded_t; 

  /** Decode the track from a BurninEncoded_t 
   * @param be the encoded burnin information to decode
   * @returns the track
   */
  inline int decodeBurninTrack(BurninEncoded_t be)          
    {return (int) (be >> 28) ; }
    
  /** Decode the event number from a BurninEncoded_t 
   * @param be the encoded burnin information to decode
   * @returns the event number
   */
  inline int decodeBurninEvent(BurninEncoded_t be)          
    {return (int) (be & 0x0FFFFFFF) ;} 
    
  /** Encodes a track number and an event number into a BurninEncoded_t
   * @param track the track to encode
   * @param event the event to encode
   * @returns the encoded value
   */
  inline BurninEncoded_t encodeBurnin(int track, int event) 
    {return (BurninEncoded_t)  ((track << 28) | (event & 0x0FFFFFFF));}


  /** The first 16 bits of SparkRefEncoded_t are used to store
   * the x position, the second 16 bits are used to store the y position 
   * This will support binnings up to 65536x65536
   */
  typedef UInt_t SparkRefEncoded_t;


  /** Decode the x bin from a SparkRefEncoded_t value
   * @param sre The encoded value
   * @returns the x bin
   */
  inline int decodeSparkRefX(SparkRefEncoded_t sre)
    {return (int) (sre >> 16) ; } 

  /** Decode the y bin from a SparkRefEncoded_t value
   * @param sre The encoded value
   * @returns the y bin
   */
  inline int decodeSparkRefY(SparkRefEncoded_t sre)
    {return (int) (sre & 0xFFFF) ; } 

  /** Encodes an x bin and a y bin to a SparkRefEncoded_t value 
   * @param x the x bin to encode
   * @param y the y bin to encode
   * @returns the encoded value
   */
  inline SparkRefEncoded_t encodeSparkRef(int x, int y)
    { return (SparkRefEncoded_t) (( x << 16) | (y & 0xFFFF)); }





/**
Class to hold reconstructed/processed data
\author Jeremy Lopez (documentation)
\author Cosmin (rewrite)
*/
class SkimEvent : public TObject {

  /** SkimDataset has the merge function */ 
  friend class SkimDataset; 
  friend class cleanskim::Functions; 

#ifndef __CINT__
  friend int cleanSkim(dmtpc::core::Dataset &, int, dmtpc::analysis::SkimDataset &, bool, const dmtpc::analysis::cleanskim::Config*,unsigned int); 
#endif 
  public:
    /**
    Constructor
    */
     SkimEvent();
  
    /**
    Copy constructor
    \param diet if false, copy cluster and image information else, just copy reconstructed variables
    */   
     SkimEvent(const SkimEvent &other, bool diet = false);

    /**
    Destructor
    */
     virtual ~SkimEvent();


    /**
    \return an array of ClusterImage objects
    */
     const std::vector<ClusterImage*>* clusters() const {return &_clusters;}

    /**
    \param i the camera number
    \return the cluster information for the given camera
    */
     const ClusterImage* cluster(int i) const {return _clusters[i];}

    /**
    \return the event number
    */
     int eventNumber() const{return _eventNumber; }

    /**
    \return the run number
    */
     int runNumber() const {return _runNumber; }

    /**
    \return the number of cameras used
    */
     int ncamera () const {return _ncamera; }
    /**
    \return the serial number of the camera
    */
      TString cameraSerialNumber(int cam) const {return _cameraSerialNumber[cam];}
      
    /**
       \param test serial number
       \retun the index of the camera given a serial number
    */
      int findSerialNumber(TString serialNumber);

    /**
    \param cam the camera  
    \param track the index of the track  
    \return the polar angle in rad (track angle measured from normal to amplification plane). 90 degrees is parallel to the amplification plane, 0 is perpendicular
    */
     double theta(int cam, int track) const {return _theta[cam][track];}

    /**
    \param cam the camera  
    \param track the index of the track  
    \return the azimuthal angle in rad.  0 is along the x axis and 90 degrees is along the y axis  
    */
     double phi(int cam, int track) const            {return _phi[cam][track];}
 
    /**
    \param cam the camera  
    \param track the index of the track  
    \return the track energy in ADU
    */
     double E(int cam, int track) const {return _E[cam][track];}

    /**
    \param cam the camera  
    \param track the index of the track  
    \return the track energy in ADU, corrected by gainmap. zero if no gainmap
    */
     double EGainMap(int cam, int track) const {return _EGainMap[cam][track];}

   /**
    \param cam the camera  
    \param track the index of the track  
    \return the track 2D range in pixels
    */
     double range(int cam, int track)  const {return _range[cam][track];}
   /**
    \param cam the camera  
    \param track the index of the track  
    \return the track 2D diffused range in pixels
    */
     double diffusedRange(int cam, int track)  const {return _diffusedRange[cam][track];}
      
      /**
	 \param cam the camera
	 \param track the index of the track
	 \return the track major axis in pixels
      */
     double majorAxis(int cam, int track) const {return _majoraxis[cam][track];}

      /**
	 \param cam the camera
	 \param track the index of the track
	 \return the track minor axis in pixels
      */
     double minorAxis(int cam, int track) const {return _minoraxis[cam][track];}

      
    /**
    \param cam the camera  
    \param track the index of the track  
    \return the track position along the x axis in pixels
    */
     double x(int cam, int track) const{return _x[cam][track];}

    /**
    \param cam the camera  
    \param track the index of the track  
    \return the track position along the y axis in pixels
    */
     double y(int cam, int track) const {return _y[cam][track];}

     double r (int cam, int track) const { return _r[cam][track];} 
     double inactive (int cam, int track) const { return _inactive[cam][track];} 
     double crossing (int cam, int track) const { return _crossing[cam][track];} 

    /**
    \param cam the camera  
    \param track the index of the track  
    \return the track skewness (head-tail).  Already implicitly included with phi.
    */
     double skewness(int cam, int track) const{return _skewness[cam][track];}

    /**
    \param cam the camera  
    \param track the index of the track  
    \return whether or not the track crosses a camera edge
    */
     bool edge(int cam, int track) const{return _edge[cam][track];}

    /**
    \param cam the camera  
    \return the number of tracks in a camera
    */
     int ntracks(int cam) const {return _ntracks[cam];}

    /**
    \param cam the camera  
    \return whether or not the image shows a spark event
    */
     bool spark(int cam=0) const {return _spark[cam];}

      /**
     \param cam the camera
     \return number of images since last spark in that camera
      */
      int lastspark(int cam=0) const {return _lastspark[cam];}

    /**
    \param cam the camera  
    \deprecated
    \return the image rms (not the image integral)
    */
     double integral(int cam=0) const{return _integral[cam];}

    /**
    \param cam the camera  
    \param track the index of the track
    \return the rms of the number of counts in each pixel of the cluster
    */
     double cluster_rms(int cam, int track) const   {return _cluster_rms[cam][track];}

    /**
    \param cam the camera  
    \param track the index of the track
    \return the mean of the number of counts in each cluster pixel (\f$=E/npixel\f$)
    */
     double cluster_mean(int cam, int track) const   {return _cluster_mean[cam][track];}

    /**
    \param cam the camera  
    \param track the index of the track
    \return the number of neighboring pixels around the cluster maximum that are also above a trackfinding threshold
    */
     int neighbors(int cam, int track)  const {return _neighbors[cam][track];}

    /**
    \param cam the camera  
    \param track the index of the track
    \return the total number of ccd pixels in the cluster
    */
     int npixel(int cam, int track) const {return _npixel[cam][track];}

    /**
    \param cam the camera  
    \param track the index of the track
    \return the maximum number of counts of any pixel in a cluster
    */
     double maxpixel(int cam, int track) const       {return _maxpixel[cam][track];}

    /**
    \param cam the camera  
    \param track the index of the track
    \return the angle the track makes with respect to the expected angle of the constellation Cygnus (the expected average WIMP velocity direction).
    */
     double cygnus_angle(int cam, int track) const   {return _cygnus_angle[cam][track];}


    /**
    \param cam the array index of the camera
    \param track the array index of the track
    \param m the moment to find (1-4)
    \return the 1st - 4th moments of the track along the measured axis
    */
     double moment(int cam, int m, int track) const   {return _moments[cam][m-1][track];}

    /**
    \param cam the array index of the camera
    \param track the array index of the track
    \param m the moment to find (1-4)
    \return the 1st - 4th moments of the track orthogonal to the measured axis
    */
     double transverse_moment(int cam, int m, int track) const   {return _transverse_moments[cam][m-1][track];}

    /**
    \param cam the camera  
    \param track the index of the track
    \return the track declination
    */
     double dec(int cam, int track) const {return _dec[cam][track];}

    /**
    \param cam the camera  
    \param track the index of the track
    \return the track right ascension
    */
     double ra(int cam, int track) const {return _ra[cam][track];}

    /**
    \param cam the camera  
    \param track the index of the track
    \return the track galactic longitude
    */
     double glon(int cam, int track) const {return _glon[cam][track];}

    /**
    \param cam the camera  
    \param track the index of the track
    \return the track right ascension
    */
     double glat(int cam, int track) const {return _glat[cam][track];}

    /**
    \param cam the camera  
    \param track the index of the track
    \return the mean value of a pixel in the image
    */
     double image_mean(int cam)	const		{return _image_mean[cam];}

    /**
    \param cam the camera  
    \param track the index of the track
    \return the RMS value of a pixel in the image (after processing)
    */
     double image_rms(int cam)	const		{return _image_rms[cam];}

    /**
    \return the number of pixels killed in image processing
    */
     int pixels_killed(int cam)	const		{return _pixels_killed[cam];}

    /**
    \return the number of pixels in the reduced cluster with a higher threshold
    */
    int npixel_red(int cam, int track) const		{return _npixel_red[cam][track];}
  

    /** 
     * Shortcut method to access image 
     * */ 
    const TH2 * image(int cam) const { return cluster(cam)->getImage();} 

    /** Returns true if the track is determined to be an RBI according to the following criteria.
     *  A track is considered an RBI if:
     * <ul>
     *   <li> number of tracks near the same position within the same run > nposthresh .
     *       <br> <i>Note that the distance threshold and the maximum number of bins checked are
     *               specified in the CleanSkim configuration</i>
     *   <li> the track is at least sparkrefbins away from a previous over threshold spark. The thresholds
     *        are also specified in the CleanSkim configuration.
     *</ul>

     @param cam the camera of the track to test
     @param track the track number of the track to test
     @param nposthresh the minimum number of closely spaced in position other tracks that must exist 
            to consider this track an RBI. Use -1 to not use this criteria.
     @param nsparkrefbins the inclusive distance threshold in bins to a previously saturated pixel (-1 to not use this criteria)
     @param the binning used (npixels/bin)

     @returns true if the track is considered an RBI by the criteria specified
     */
    bool isRBI(int cam, int track, int nposthresh = 2, int sparkrefbins = 3, int binning = 4) const;

    
    /**
    \param cam the camera 
    \param track the index of the track  
    \return the number of tracks which are nearby in both space and time.  A large value indicates that the event is likely an RBI. What is considered nearby 
    is set by the CleanSkimConfig
    */
     int nburnin(int cam,int track)  const {return _nburnin[cam][track];}

    /**
    \param cam the camera 
    \param track the index of the track  
    \return the event of a cluster which is closer than the RBI cuts to a track in the current event.
    */
     
     int burnin_event(int cam,int track,int i)  const {return decodeBurninEvent(_burnin[burninIndex(cam,track,i)]);}
    /**
    \param cam the camera 
    \param track the index of the track  
    \return the track number of a cluster which is closer than the RBI cuts to a track in the current event
    */
     
     int burnin_track(int cam,int track,int i)  const {return decodeBurninTrack(_burnin[burninIndex(cam,track,i)]);}

    /**
    Tells where in the burnin (RBI) array that this event starts
    \param cam the camera 
    \param track the index of the track  
    \return the index of this track in the burnin array
    */
     int burnin_this_index (int cam,int track)  const {return _burnin_this_index[cam][track];}

     /**Get the encoded burnin vector 
      * \param c camera  
      * \param t track  
      * \returns the BurninEncoded_t value of the track
      */

     const BurninEncoded_t * burnin_v(int c,int t)  const   {return &_burnin[burninIndex(c,t,0)];}


     /* methods related to spark saturation cut*/

     /** The number of pixels that have been saturated before
      *  \param c the camera 
      */
     int nsparkref(int c) const {return _nsparkref[c];}

     /** X bin number of ith saturated pixel in the camera */  
     int sparkrefX(int c,int i) const{return decodeSparkRefX(_sparkref[_sparkref_base[c]+i]);}

     /** Y bin number of ith saturated pixel in the camera */  
     int sparkrefY(int c,int i) const{return decodeSparkRefY(_sparkref[_sparkref_base[c]+i]);}


     double nearestSpacerDistance(int c, int t) const { return _nearest_spacer_distance[c][t];} 
     double nearestSpacer(int c, int t) const { return _nearest_spacer[c][t];} 

     /** Get an array of waveform vectors */
     const std::vector<dmtpc::waveform::WaveformVector*> * waveform_vectors(){return &_waveform_vectors;}

     /** Get a vector of CspWfVectors */ 
     const std::vector<dmtpc::waveform::CspWfVector*> * cspwf_vectors() {return &_cspwf_vectors;}

     /** Get a vector of FastWfVectors */
     const std::vector<dmtpc::waveform::FastWfVector*> * fastwf_vectors() {return &_fastwf_vectors;}

     /** Get a vector of FastWfVectors */
     const std::vector<dmtpc::waveform::PMTWfVector*> * pmtwf_vectors() {return &_pmtwf_vectors;}

     /** Get the nth trigger group */
     // had to remove const in front to get my code to work
//     MaxCamTriggerGroup * trigger_group(int i) const  { return (i < ntriggers() && i >= 0) ? (MaxCamTriggerGroup * ) _trigger_groups->At(i) :  NULL ; } 

     /** Print out event information to the given stream
      * @param stream output stream (default: cout) */ 
     void printOut(std::ostream & stream = std::cout);

     /** Print out track information to the given stream
      * @param cam the camera to print out
      * @param track the track to print out
      * @param stream output stream (default: cout) */ 
     void printOut(int cam, int track, std::ostream & stream = std::cout); 

     bool isStitched() const { return _stitched; } 
     

  private:    

     void copyBurnin(std::vector<std::vector<std::vector<BurninEncoded_t> > > *);
     void copySparkRef(std::vector<std::pair<int,int> > *);
     void initVectors(int ncam, int ntracks = 15);
     void initCamVectors(int ncam);
     void addTrackVector(int ntracks = 15);
     void clearTrackVectors(); 
     inline int burninIndex(int c, int t, int i) const {return _burnin_base[c][t] + i;}
     
     void printOutTrack(int c, int t, std::ostream & stream); 
     void printOutCamera(int c, std::ostream & stream); 
     int _eventNumber;
     int _index; 
     int _runNumber;
     int _ncamera;
     bool _stitched; 
   
     /* Sizes of the array... are stored for I/O purposes */
     Int_t _s3;
     Int_t _s4;

     /* This keeps track of the base index of each track inside the burnin array */
     std::vector<std::vector <int> >   _burnin_base;
     std::vector<std::vector <int> >   _burnin_this_index;


     /* Number of correlated tracks in other events for each track */
     std::vector<std::vector<int> >  _nburnin; 

     /* Analysis variables */ 
     std::vector<std::vector <double> >  _theta; 
     std::vector<std::vector <double> >  _range; 
     std::vector<std::vector <double> >  _diffusedRange; 
     std::vector<std::vector <double> >  _phi; 
     std::vector<std::vector <double> >  _majoraxis;
     std::vector<std::vector <double> >  _minoraxis;
     std::vector<std::vector <double> >  _E; 
     std::vector<std::vector <double> >  _EGainMap; 
     std::vector<std::vector <double> >  _r; 
     std::vector<std::vector <double> >  _x; 
     std::vector<std::vector <double> >  _y; 
     std::vector<std::vector <double> >  _skewness; 
     std::vector<std::vector <bool> >   _edge; 
     std::vector<std::vector <double> >   _cluster_mean; 
     std::vector<std::vector <double> >   _cluster_rms; 
     std::vector<std::vector <int> >   _neighbors; 
     std::vector<std::vector <double> >  _maxpixel; 
     std::vector<std::vector <double> >  _cygnus_angle; 
     std::vector<std::vector <int> >   _npixel; 
     std::vector<std::vector <int> >   _npixel_red; 
     std::vector<std::vector <std::vector <double> > >  _moments; 
     std::vector<std::vector <std::vector <double> > >  _transverse_moments; 
     std::vector<std::vector <double> >  _ra; 
     std::vector<std::vector <double> >  _dec; 
     std::vector<std::vector <double> >  _glat; 
     std::vector<std::vector <double> >  _glon; 
     std::vector<std::vector <double> > _inactive; 
     std::vector<std::vector <double> > _crossing; 
     
     std::vector<int>    _ntracks;             
     std::vector<bool>   _spark;             
     std::vector<int>    _lastspark;
     std::vector<double>  _integral;          
     std::vector<double>  _image_mean;          
     std::vector<int>  _pixels_killed;          
     std::vector<double>  _image_rms;          
     std::vector<int>    _nsparkref;        
     std::vector<int>    _sparkref_base;   
     std::vector<TString> _cameraSerialNumber;
      

     /* Flat array. Use utility methods to decode */
     UInt_t     *_burnin;               //[_s3] 
     UInt_t     *_sparkref;             //[_s4]

     std::vector<dmtpc::waveform::WaveformVector*> _waveform_vectors;
     std::vector<dmtpc::waveform::CspWfVector*> _cspwf_vectors;
     std::vector<dmtpc::waveform::FastWfVector*> _fastwf_vectors;
     std::vector<dmtpc::waveform::PMTWfVector*> _pmtwf_vectors;
     std::vector<ClusterImage*> _clusters; 
     std::vector<std::vector<double> > _nearest_spacer_distance; 
     std::vector<std::vector<int> > _nearest_spacer; 

     ClassDef(SkimEvent,2)
     
};
}
}

#endif
