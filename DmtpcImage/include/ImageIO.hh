#ifndef DMTPC_IMAGE_IO_HH
#define DMTPC_IMAGE_IO_HH

class TH2; 
#include "TString.h" 

namespace dmtpc
{
  namespace image
  {
    namespace io
    {
 
      /**
         Set the minimum and maximum of a TH2F for plotting purposes
      */
      void setFriendlyRange(TH2* img, float nsigma=3.0);


      /**TODO: fix memory leaks! */ 

      TH2* convertFitsIntoROOT(TString fitsFile, TString histName="imageHisto", int imageNumber=0, char type='F');

      int getNumberOfImagesInFitsFile(TString fitsFile);

      const char* getTimeStamp(TString fitsFile);

      time_t getTimeStampUnix(TString fitsFile);

      char* getFitsHeaderValue(TString fitsFile, char* key);

      // Returns NULL if keyword not found in FITS file 
      // Does NOT quit FITS functions fail (status)
      char* getFitsHeaderValueIfExists(TString fitsFile, char* key);
      
      int convertIntoFits(const TH2* image, TString fileName);

    }
  }
}

#endif
