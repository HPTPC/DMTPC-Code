#include "Image.hh" 
#include <iostream>
#include "CameraInfo.hh" 
#include "TH2.h" 
#include <cstdlib>

ClassImp(dmtpc::core::Image); 


dmtpc::core::Image * dmtpc::core::Image::makeImage(const TH2 * hist, uint8_t nbytes, bool dark) 
{

  
  Image *i = new Image;  
  CameraInfo * info = new CameraInfo; 

  info->nx = hist->GetNbinsX(); 
  info->ny = hist->GetNbinsY(); 
  info->camIndex = 0; 

  info->xbin = (uint16_t) hist->GetXaxis()->GetBinWidth(1); 
  info->ybin = (uint16_t) hist->GetYaxis()->GetBinWidth(1); 

  info->ul_x = (uint16_t) hist->GetXaxis()->GetXmin(); 
  info->lr_x = (uint16_t) hist->GetXaxis()->GetXmax(); 
  info->ul_y = (uint16_t) hist->GetYaxis()->GetXmin(); 
  info->lr_y = (uint16_t) hist->GetYaxis()->GetXmax(); 

  info->nflushes = 0; 
  info->ccdTemp =0; 
  info->ccdTempSet = 0; 
  info->exposureTime = 0; 
  info->readoutSpeedVertical = 0; 
  info->readoutSpeedHorizontal = 0; 
  info->dark = dark;
  info->shutterUsed = false; 
  info->serialNumber = hist->GetName(); 
  info->model = "fromhist"; 
  info->nbytes = nbytes; 
  info->overscanColumns = 0; 
  info->overscanRows = 0; 
  info->digitizeOverscan = false;

  i->info = info; 

  i->SetName(hist->GetName()); 
  i->SetTitle(hist->GetTitle()); 


  i->size = info->nx *info->ny *nbytes;
  i->buf = (uint8_t*) malloc(i->size); 

  int ihist = 0; 
  for (int x = 1; x <= hist->GetNbinsX(); x++)
  {
    for (int y = 1; y <= hist->GetNbinsY(); y++)
    {
      switch (info->nbytes) 
      {
        case 1: 
          (i->buf)[ihist++] = (uint8_t) hist->GetBinContent(x,y); 
          break; 
        case 2: 
          ((uint16_t*)i->buf)[ihist++] = (uint16_t) hist->GetBinContent(x,y); 
          break; 
        case 4: 
          ((uint32_t*)i->buf)[ihist++] = (uint32_t) hist->GetBinContent(x,y); 
          break; 
       default: 
          std::cerr << " invalid number of bytes!!!" << std::endl; 
      }
    }
  }



  return i; 

}

dmtpc::core::Image::Image(const char * name, const char * title, const void * data, const CameraInfo * info)
{
  SetName(name); 
  SetTitle(title); 
  size = info->nbytes * abs(-info->ul_x+info->lr_x) * abs(-info->ul_y + info->lr_y) / info->xbin / info->ybin; 
  buf = (uint8_t*) malloc(size); 
  memcpy(buf,data,size); 
  this->info = new CameraInfo(*info); 
  hist = 0; 
  visible = 0;
  overscan = 0;
}

TH2* dmtpc::core::Image::th2() const 
{
  if (hist) return hist;
  else hist=extractTH2(0);
  return hist;
}


TH2* dmtpc::core::Image::th2_visible() const
{
  if (visible) return visible;
  else visible=extractTH2(1);
  return visible;
}


TH2* dmtpc::core::Image::th2_overscan() const
{
  if (overscan) return overscan;
  else overscan=extractTH2(2);
  return overscan;
}


// 0 for total image (visible + overscan)
// 1 for only visible
// 2 for only overscan
TH2* dmtpc::core::Image::extractTH2(int mode=0) const
{
  int nbins_x = (info->lr_x-info->ul_x)/info->xbin;
  int nbins_y = (info->lr_y-info->ul_y)/info->ybin;

  int first_visible_bin_x = 1 + info->overscanColumns / info->xbin;
  int first_visible_bin_y = 1 + info->overscanColumns / info->ybin;
  int last_visible_bin_x  = (info->lr_x-info->ul_x-info->overscanRows+info->xbin-1) / info->xbin;
  int last_visible_bin_y  = (info->lr_y-info->ul_y-info->overscanRows+info->ybin-1) / info->ybin;

  //printf("Image::extractTH2: first visible bin = %d, last visible bin = %d\n", first_visible_bin_x, last_visible_bin_x);

  TH2* result = new TH2I(TString::Format("%s_th2i",fName.Data()),TString::Format("TH2I of %s", fTitle.Data()), 
			 nbins_x, info->ul_x, nbins_x*info->xbin, nbins_y, info->ul_y, nbins_y*info->ybin); 

  int idata = 0; 
  for (int i = 1; i <= result->GetNbinsX(); i++)
  {
    for (int j = 1; j <= result->GetNbinsY(); j++)
    {
      if      (mode==1 && (i<first_visible_bin_x || i>last_visible_bin_x ||
			   j<first_visible_bin_y || j>last_visible_bin_y)) { idata++; continue; } // skip if within OS region
      else if (mode==2 && (i>=first_visible_bin_x && i<=last_visible_bin_x &&
			   j>=first_visible_bin_y && j<=last_visible_bin_y)) { idata++; continue; } // skip if within visible region

      // hopefully we don't have 32bit unsigned data...
      int val =  info->nbytes == 1 ? (int) buf[idata++]: 
                     info->nbytes == 2 ? (int) (((uint16_t*)buf)[idata++]): 
                     info->nbytes == 4 ? (int) (((uint32_t*)buf)[idata++]): 
                     0; 
      

      result->SetBinContent(i,j,val); 
    }
  }

  return result; 
}


TH2D* dmtpc::core::Image::makeEmptyTH2D() const
{
  int nbins_x = (info->lr_x-info->ul_x)/info->xbin;
  int nbins_y = (info->lr_y-info->ul_y)/info->ybin;

  TH2D* result = new TH2D(TString::Format("%s_th2f",fName.Data()),TString::Format("TH2F of %s", fTitle.Data()),
                         nbins_x, info->ul_x, nbins_x*info->xbin, nbins_y, info->ul_y, nbins_y*info->ybin);

  return result;
}

dmtpc::core::Image::~Image()
{

  if (hist) delete hist; 
  if (visible) delete visible;
  if (overscan) delete overscan;
  if (info) delete info; 
  if (buf) 
    free(buf); 

}

