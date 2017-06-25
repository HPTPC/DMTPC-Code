#ifndef DMTPC_ANALYSIS_GAINMAP_HH
#define DMTPC_ANALYSIS_GAINMAP_HH

#include "TObjArray.h"
#include "TObject.h"
#include "TH2F.h"
#include "TF1.h" 
#include "TVector3.h"

#include <iostream>
#include <vector>


namespace dmtpc
{
namespace analysis
{

class GainMap : public TNamed {

   public:
      /**
	 Default Constructor
      */
      GainMap();
      /**
	 Constructor
	 \param name name of the gain map
      */
      GainMap(TString name);
      /**
	 Default Destructor
      */
      ~GainMap();

      /**
	 Draws the gain map with spacers overlaid
      */

      void drawWithSpacers();
      
      /**
	 Sets the map to a particular TH2F
	 \param gainMap pointer to the TH2F to become the gain map
      */
      void setGainMap(TH2F* gainMap) {_gainMap = gainMap;}
      /**
	 \return pointer to the gainMap
      */
      const TH2F* getGainMap() const {return _gainMap;}
      TH2F* getGainMap() {return _gainMap;}
      
      /** 
	  Adds a spacer to the list of spacers. All parameters in camera units
	  \param slope slope of the spacer
	  \param intercept intercept of the spacer
	  \param width width of the spacer
      */
      void addSpacer(double slope, double intercept, double width);
      /**
	 Adds a spacer to the list of spacers. All parameters in camera units
	 \param params TVector3 containing (slope,intercept,width)
      */
      void addSpacer(TVector3* params);
      
      /** 
	  \param i index of desired spacer
	  \return TVector3 of (slope,intercept,width)
      */
      const TVector3* getSpacer(int i) const {return (TVector3*)_spacers->At(i);}
      /**
	 \param i index of desired spacer
	 \return slope of spacer
       */
      double getSpacerSlope(int i) const {return ((TVector3*)_spacers->At(i))->X();}
      /**
	 \param i index of desired spacer
	 \return intercept of spacer
       */
      double getSpacerIntercept(int i) const {return ((TVector3*)_spacers->At(i))->Y();}
      /**
	 \param i index of desired spacer
	 \return width of spacer
       */
      double getSpacerWidth(int i) const {return ((TVector3*)_spacers->At(i))->Z();}
		  
      /**
	 \return number of spacers in list
      */
      int getNSpacers() const {return _nSpacers;}

      int nCrossingSpacers(double x1,double y1, double x2, double y2, std::vector<int>* which = NULL) const; 

      /**
	 Calculates whether the line between (x1,y1) and (x2,y2) crosses a spacer
	 \param x1 x-coordinate of first point
	 \param y1 y-coordinate of first point
	 \param x2 x-coordinate of second point
	 \param y2 y-coordinate of second point
	 \return true if it crosses any spacer, false if it does not
      */
      bool crossesSpacer(double x1, double y1, double x2, double y2) const;

      /**
	 Calculates the distance from point (x,y) to spacer i
	 \param i index of spacer of interest
	 \param x x-coordinate
	 \param y y-coordinate
	 \return the distance of nearest approach between the point and the line
      */
      double distanceToSpacer (int i, double x, double y) const ;
      /**
	 Calculates the distance from point (x,y) to the nearest spacer
	 \param x x-coordinate
	 \param y y-coordinate
	 \param imin the index of the nearest spacer, as calculated
	 \return the distance of nearest approach between the point and the nearest spacer
      */
      double distanceToNearestSpacer (double x, double y, int& imin) const;
      /**
	 Calculates the distance from point (x,y) to all spacers
	 \param x x-coordinate
	 \param y y-coordinate
	 \return a vector of the distances of nearest approach between the point and the line
      */
      std::vector < double > distanceToSpacers (double x, double y) const;
   
   /** 
    * Writes out an overlay based on the gain map 
    * \param outfile The file to write to 
    */
   void writeOverlay(const char * outfile) const; 

   TH2* getErrorEstimate() const; 
   void randomize(); //randomize gainmap by jittering values according to error estimate 



   private:
      TH2F* _gainMap; ///< the actual gain map
      TObjArray* _spacers; ///< the array of spacers
      int _nSpacers; ///< the number of spacers
    
//      vector<TF1*> *_spacer_fns; 
 //     bool _use_fns; 
      mutable TH2F* _error_estimate; //! 
      ClassDef(GainMap,2)
        
};

}
}

#endif
