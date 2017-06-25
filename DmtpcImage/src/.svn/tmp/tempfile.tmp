#include "ImageIO.hh"
#include "/scratch1/darkmatter/dmtpc/projects/DarkMatter/cfitsio/fitsio.h" 
#include <time.h>
#include <iostream>
#include "TH2.h"
#include "ImageCalculations.hh"
#include "ImageTools.hh"


void 
dmtpc::image::io::setFriendlyRange(TH2* img, float nsigma) {
  double mean, rms;
  calculations::meanRMSNoOutliers(img, mean, rms);
  img->SetMinimum(mean-nsigma*rms);
  img->SetMaximum(mean+nsigma*rms);
  return;
}


#ifndef DMTPC_NO_FITS
//TH2F*
//dmtpc::image::io::convertFitsIntoROOT(TString fitsFile, TString histName) {
//  // Convert an image in file fitsFile from FITS format to TH2F*.
//  
//  int status = 0;
//  fitsfile *fp = 0 ;
//  
//  TH2F *imageHisto=0;
//  
//  fits_open_diskfile( &fp, (const char*)fitsFile, READONLY_FILE, &status);
//  if (status) { fits_report_error(stderr, status); return imageHisto; }
//  
//  int simple, bitpix, naxis, extend;
//  long axes[2], pcount, gcount;
//  fits_read_imghdr(fp, 10, &simple, &bitpix, &naxis, axes, &pcount, &gcount, &extend, &status);
//  if (status) { fits_report_error(stderr, status); return imageHisto; }
//  
//  cout << simple << "  " << bitpix << "  "<< naxis << "  " << axes[0] <<","<<axes[1]<< "  "
//       << pcount << "  " << gcount << "  "<< extend << "  " << status << endl;
//  
//  //    u_int16_t *img = new u_int16_t[axes[0]*axes[1]];
//  float* img = new float[axes[0]*axes[1]];
//  
//  //imageHisto=new TH2F("imageHisto","Copy of a fits file", axes[0],0,axes[0],  axes[1],0,axes[1]);
//  imageHisto=new TH2F(histName.Data(),"Copy of a fits file", axes[0],0,axes[0],  axes[1],0,axes[1]);
//  
//  int nulval, anynul;
//  fits_read_img(fp, TFLOAT, 1, axes[0]*axes[1], &nulval, img, &anynul, &status);
//  if (status) { fits_report_error(stderr, status); return imageHisto; }
//  
//  for (int i=0; i<axes[1]; i++) {
//    for (int j=0; j<axes[0]; j++) {
//      //cout << j<<","<<img[ axes[0] * i + j ] << "  ";
//      //imageHisto->SetBinContent(j*(axes[0]+2)+i+1, img[ axes[1] * i + j ]);
//      imageHisto->SetBinContent(j+1,i+1, img[ axes[0] * i + j ]);
//    }
//    //cout<<endl;
//  }
//  
//  fits_close_file(fp,&status);
//  
//  return imageHisto;
//}


int dmtpc::image::io::getNumberOfImagesInFitsFile(TString fitsFile) {
  int status = 0;
  fitsfile *fp = 0 ;

  fits_open_diskfile( &fp, (const char*)fitsFile, READONLY_FILE, &status);
  if (status) { fits_report_error(stderr, status); return -1; }
  
  int simple, bitpix, naxis, extend;
  long axes[3], pcount, gcount;
  fits_read_imghdr(fp, 10, &simple, &bitpix, &naxis, axes, &pcount, &gcount, &extend, &status);
  if (status) { fits_report_error(stderr, status); return -1; }

  int nimg;
  if (naxis == 2) {
    nimg = 1;
  } else {
    nimg = axes[2];
  }

  fits_close_file(fp,&status);
  if (status) { fits_report_error(stderr, status); return 0; }

  return nimg;
}


time_t dmtpc::image::io::getTimeStampUnix(TString fitsFile)
{
  int status = 0;
  fitsfile *fp = 0 ;

  fits_open_diskfile( &fp, (const char*)fitsFile, READONLY_FILE, &status);
  if (status) { fits_report_error(stderr, status); return 0; }

  char value[32], comm[32], key[]="DATE";
  fits_read_key_str( fp, key, value, comm, &status);
  fits_close_file(fp,&status);
  if (status) { fits_report_error(stderr, status); return 0; }

  struct tm t; 
  strptime(value,"%Y-%m-%dT%H:%M:%S",&t); 
 
  return mktime(&t); 


}

const char*
dmtpc::image::io::getTimeStamp(TString fitsFile) {
  int status = 0;
  fitsfile *fp = 0 ;

  fits_open_diskfile( &fp, (const char*)fitsFile, READONLY_FILE, &status);
  if (status) { fits_report_error(stderr, status); return 0; }

  char value[32], comm[32], key[]="DATE";
  fits_read_key_str( fp, key, value, comm, &status);
  std::cout << value << std::endl;
  TString *sdate=new TString(value);
  
  fits_close_file(fp,&status);
  if (status) { fits_report_error(stderr, status); return 0; }

  return sdate->ReplaceAll("T"," ").Data();  
}

char* 
dmtpc::image::io::getFitsHeaderValueIfExists(TString fitsFile, char* key) {
  int status = 0;
  fitsfile *fp = 0 ;

  fits_open_diskfile( &fp, (const char*)fitsFile, READONLY_FILE, &status);
  //if (status) { fits_report_error(stderr, status); /*return 0;*/ }
  
  char value[32], comm[32];
  fits_read_key_str( fp, key, value, comm, &status);  
  fits_close_file(fp,&status);

  TString sdate(value);
  
  if (status) { status=0; return NULL; }
  //else return strdup(sdate.Data());  
  else return strdup(value);

}


char*
dmtpc::image::io::getFitsHeaderValue(TString fitsFile, char* key){
  int status = 0;
  fitsfile *fp = 0 ;

  fits_open_diskfile( &fp, (const char*)fitsFile, READONLY_FILE, &status);
  if (status) { fits_report_error(stderr, status); /*return 0;*/ }
  
  char value[32], comm[32];
  fits_read_key_str( fp, key, value, comm, &status);
//  cout << value << endl;
  TString sdate(value);
  
  fits_close_file(fp,&status);
  if (status) { printf("key=%s\n", key); fits_report_error(stderr, status); return 0; }
  
  return strdup(sdate.Data());  
}


TH2*
dmtpc::image::io::convertFitsIntoROOT(TString fitsFile, TString histName, int imageNumber, char type) {
  // Convert an image in file fitsFile from FITS format to TH2F*.
  // Extended to accommodate multiple images per fits file 
  // Assuming data is stored in a data cube in the FITS file (As per the Andor camera)
  // The data cube has:
  //   NAXIS  = 3
  //   NAXIS1 = npix on AXIS1
  //   NAXIS2 = npix on AXIS2
  //   NAXIS3 = Number of images.
  // So, for a 512x512 chip, and a fits file containing 100 images:
  //   NAXIS1 = NAXIS2 = 512
  //   NAXIS3 = 100
  // (jbattat May 2009)
  
  int status = 0;
  fitsfile *fp = 0 ;

  TH2 *imageHisto=0;
  
  fits_open_diskfile( &fp, (const char*)fitsFile, READONLY_FILE, &status);
  if (status) { fits_report_error(stderr, status); return imageHisto; }
  
  int simple, bitpix, naxis, extend;
  long axes[3], pcount, gcount;
  fits_read_imghdr(fp, 10, &simple, &bitpix, &naxis, axes, &pcount, &gcount, &extend, &status);
  if (status) { fits_report_error(stderr, status); return imageHisto; }
  std::cout << "axes[0] = " << axes[0] << std::endl;
  std::cout << "axes[1] = " << axes[1] << std::endl;
//  cout << simple << "  " << bitpix << "  "<< naxis << "  ";
  for (int ii=0; ii<naxis; ii++) {
 //   cout << axes[ii];
    if (ii < (naxis-1)) {
  //    cout << ",";
    }
  }
 // cout << "  " << pcount << "  " << gcount << "  "<< extend << "  " << status << endl;
  
  //    u_int16_t *img = new u_int16_t[axes[0]*axes[1]];
  float* img = new float[axes[0]*axes[1]];
  
  // Figure out the horizontal and vertical binning
  // to size the TH2F
  char value[32]; 
  char comm[32]; 
  // can figure out camera with this keyword
  //fits_read_key_str(fp, "INSTRUME", value, comm, &status); 

  // It looks like if "status" is nonzero prior to calling
  // fits_read_keyword(), then the function returns non-zero value...
  // i.e. claims that keyword is not present...
  // so reset "status" to zero prior to each call.
  int xbin, ybin;
  bool xbinKnown = false;
  bool ybinKnown = false;
  std::cout << "status a = " << status << std::endl;
  if (!fits_read_keyword(fp, "XBINNING", value, comm, &status)) {
    std::cout << "XBINNING is in header" << std::endl;
    xbin = atoi(value);
    xbinKnown = true;
  }
  status = 0;
  if (!fits_read_keyword(fp, "HBIN", value, comm, &status)) {
    std::cout << "HBIN is in header" << std::endl;
    xbin = atoi(value);
    xbinKnown = true;
  }
  if (!xbinKnown) {
    std::cout << "could not determine xbinning" << std::endl;
    std::cout << "WARNING: Defaulting to binning of 1" << std::endl;
    xbin = 1;
  }

  status = 0;
  if (!fits_read_keyword(fp, "YBINNING", value, comm, &status)) {
    std::cout << "YBINNING is in header" << std::endl;
    ybin = atoi(value);
    ybinKnown = true;
  }
  status = 0;
  if (!fits_read_keyword(fp, "VBIN", value, comm, &status)) {
    std::cout << "VBIN is in header" << std::endl;
    ybin = atoi(value);
    ybinKnown = true;
  }
  if (!ybinKnown) {
    std::cout << "could not determine ybinning" << std::endl;
    std::cout << "WARNING: Defaulting to binning of 1" << std::endl;
    ybin = 1;
  }
  std::cout << "xbin, ybin = " << xbin << ", " << ybin << std::endl;

  /////// Legacy code...
  ////Get binning
  ////fits_read_key_str(fp, "HBIN", value, comm, &status); 
  //fits_read_key_str(fp, "XBINNING", value, comm, &status); 
  //int xbin = atoi(value); 
  ////fits_read_key_str(fp, "VBIN", value, comm, &status); 
  //fits_read_key_str(fp, "YBINNING", value, comm, &status); 
  //int ybin = atoi(value); 

  imageHisto= tools::newTH2(type, histName.Data(),"Copy of a fits file", axes[0],0,axes[0]*xbin,  axes[1],0,axes[1]*ybin);

  int nulval, anynul;
  long fpixel[3], lpixel[3], inc[3];
  fpixel[0] = 1L;
  fpixel[1] = 1L;
  lpixel[0] = axes[0];
  lpixel[1] = axes[1];
  inc[0] = inc[1] = inc[2] = 1L;

  std::cout << "imageNumber = " << imageNumber << std::endl;

  if (naxis==2 || imageNumber == 0) {
    status = 0;
    fits_read_img(fp, TFLOAT, 1, axes[0]*axes[1], &nulval, img, &anynul, &status);
  } else {
    fpixel[2] = imageNumber+1;
    lpixel[2] = imageNumber+1;
    std::cout << "calling fits_read_subset" << std::endl;
    std::cout << "fpixel = " << fpixel[0] << ", " << fpixel[1] << ", " << fpixel[2] << std::endl;
    fits_read_subset(fp, TFLOAT, fpixel, lpixel, inc, &nulval, img, &anynul, &status);
  }
  if (status) { fits_report_error(stderr, status); return imageHisto; }

  for (int i=0; i<axes[1]; i++) {
    for (int j=0; j<axes[0]; j++) {
      //cout << j<<","<<img[ axes[0] * i + j ] << "  ";
      //imageHisto->SetBinContent(j*(axes[0]+2)+i+1, img[ axes[1] * i + j ]);
      imageHisto->SetBinContent(j+1,i+1, img[ axes[0] * i + j ]);
    }
    //cout<<endl;
  }
  
  fits_close_file(fp,&status);

  if (status) { fits_report_error(stderr, status); return imageHisto; }

  // dont you need to delete img?
  delete [] img;

  return imageHisto;
}


int
dmtpc::image::io::convertIntoFits(const TH2* image, TString fileName) {
        // Convert a ROOT image in TH2F format to fits format
        // and save it to a file.
        //    


    long naxes[2] = { image->GetNbinsX(), image->GetNbinsY() };
    fitsfile *fp;

    int status = 0;
    fits_create_file(&fp, (const char*)fileName, &status);
    if (status) { fits_report_error(stderr, status); return -1; }

    fits_create_img(fp, SHORT_IMG, 2, naxes, &status);
    if (status) { fits_report_error(stderr, status); return -1; }

    Double_t *img = new Double_t[ naxes[0] * naxes[1] ];
    for (int i=0; i<naxes[1]; i++) {
      for (int j=0; j<naxes[0]; j++) {
        img[naxes[0] * i + j]=(Double_t)image->GetBinContent(j+1,i+1);
      }
    }

    fits_write_img(fp, TDOUBLE, 1, naxes[0] * naxes[1], img, &status);
    if (status) { fits_report_error(stderr, status); return -1; }

    fits_close_file(fp, &status);
    if (status) { fits_report_error(stderr, status); return -1; }

    return status;
}

#endif


