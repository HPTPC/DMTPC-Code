#ifndef DMTPC_CORE_IMAGE_HH
#define DMTPC_CORE_IMAGE_HH



#include "TH2.h"
#include "TNamed.h" 
#include <stdint.h>

namespace dmtpc
{
  namespace core
  {

    class CameraInfo; 
    class Image : public TNamed
    {
      public: 

        Image() { hist = 0; visible = 0; overscan = 0; info = 0; buf = 0; } 
        virtual ~Image(); 
        Image(const char * name, const char * title, const void * rawdata, const CameraInfo * info); 

        operator TH2*() { return  th2(); }  //non-const
        operator TH2()  { return *th2(); }  //non-const

        void Draw(const char * opt = 0) { th2()->Draw(opt); } 
        void DrawVisible(const char * opt = 0) { th2_visible()->Draw(opt); }
        void DrawOverscan(const char * opt = 0) { th2_overscan()->Draw(opt); }
        
        const TH2 * getHist() const { return th2(); } 
        const TH2 * getVisible() const { return th2_visible(); }
        const TH2 * getOverscan() const { return th2_overscan(); }

        TH2D* makeEmptyTH2D() const;

        const CameraInfo *getInfo() const { return info; } 

        static Image * makeImage(const TH2 * hist, uint8_t nbytes = 2, bool dark = false); 

      protected:
        TH2* extractTH2(int mode) const;
        TH2* th2() const;
        TH2* th2_visible() const;
        TH2* th2_overscan() const;
        mutable TH2* hist; //! 
        mutable TH2* visible; //!
        mutable TH2* overscan; //!
        const CameraInfo * info; 
        uint32_t size; 
        uint8_t * buf;//[size]


        ClassDef(Image,2); 
    }; 
  }
}

#endif

