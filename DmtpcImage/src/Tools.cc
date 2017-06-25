#include "ImageTools.hh" 
#include "TClass.h"
#include <iostream>
#include "TH2.h"

static char getType(const TH1 * in)
{
  if (in->IsA()->InheritsFrom("TArrayF")) 
    return 'F';
  if (in->IsA()->InheritsFrom("TArrayC")) 
    return 'C';
  if (in->IsA()->InheritsFrom("TArrayS")) 
    return 'S';
  if (in->IsA()->InheritsFrom("TArrayI")) 
    return 'I';
  if (in->IsA()->InheritsFrom("TArrayD")) 
    return 'D';
  if (in->IsA()->InheritsFrom("TArrayL")) 
    return 'L';



}

TH2 * dmtpc::image::tools::newTH2StealType(const TH2* type_giver, const char * name, const char * title, int nbinsx, double xmin, double xmax, int nbinsy, double ymin, double ymax)
{
  return newTH2(getType(type_giver),name,title,nbinsx,xmin,xmax,nbinsy,ymin,ymax); 
}

TH2 * dmtpc::image::tools::newTH2StealSize(const TH2* giver, char type, const char * name, const char * title)
{
  return newTH2(type,name,title,giver->GetNbinsX(),giver->GetXaxis()->GetXmin(),giver->GetXaxis()->GetXmax()
                               ,giver->GetNbinsY(),giver->GetYaxis()->GetXmin(),giver->GetYaxis()->GetXmax()); 
}

TH1 * dmtpc::image::tools::newTH1StealSize(const TH1* giver, char type, const char * name, const char * title)
{
  return newTH1(type,name,title,giver->GetNbinsX(),giver->GetXaxis()->GetXmin(),giver->GetXaxis()->GetXmax()); 
}


TH2 * dmtpc::image::tools::newTH2StealTypeAndSize(const TH2* giver, const char * name, const char * title)
{
  return newTH2(getType(giver),name,title,giver->GetNbinsX(),giver->GetXaxis()->GetXmin(),giver->GetXaxis()->GetXmax()
                                                          ,giver->GetNbinsY(),giver->GetYaxis()->GetXmin(),giver->GetYaxis()->GetXmax()); 
}

UInt_t dmtpc::image::tools::TH1GetNbins(const TH1* hist)
{
  return (hist->GetNbinsX() + 2 ) * (hist->GetNbinsY() > 0 ? hist->GetNbinsY() + 2 : 1 ) * (hist->GetNbinsZ() > 0 ? hist->GetNbinsZ() + 2 : 1); 
}

TH2 * dmtpc::image::tools::newTH2(char type, const char * name, const char * title, int nbinsx, double xmin, double xmax, int nbinsy, double ymin, double ymax)
{
  switch(type)
  {
    case 'C': 
      return new TH2C(name,title,nbinsx,xmin,xmax,nbinsy,ymin,ymax); 
    case 'S':
      return new TH2S(name,title,nbinsx,xmin,xmax,nbinsy,ymin,ymax); 
    case 'I':
      return new TH2I(name,title,nbinsx,xmin,xmax,nbinsy,ymin,ymax); 
    case 'F':
      return new TH2F(name,title,nbinsx,xmin,xmax,nbinsy,ymin,ymax); 
    case 'D':
      return new TH2D(name,title,nbinsx,xmin,xmax,nbinsy,ymin,ymax); 
    default:
      std::cerr << "Bad type TH2" << type << std::endl; 
      return 0; 
  }
}


TH1 * dmtpc::image::tools::newTH1StealType(const TH1* type_giver, const char * name, const char * title, int nbinsx, double xmin, double xmax, TH1 * ptr)
{
  return newTH1(getType(type_giver),name,title,nbinsx,xmin,xmax,ptr); 
}


TH1 * dmtpc::image::tools::newTH1(char type, const char * name, const char * title, int nbinsx, double xmin, double xmax, TH1* ptr)
{
  switch(type)
  {
    case 'C': 
      return ptr ? new(ptr) TH1C(name,title,nbinsx,xmin,xmax) :  new TH1C(name,title,nbinsx,xmin,xmax); 
    case 'S':
      return ptr ? new(ptr) TH1S(name,title,nbinsx,xmin,xmax) :  new TH1S(name,title,nbinsx,xmin,xmax); 
    case 'I':
      return ptr ? new(ptr) TH1I(name,title,nbinsx,xmin,xmax) :  new TH1I(name,title,nbinsx,xmin,xmax); 
    case 'F':
      return ptr ? new(ptr) TH1F(name,title,nbinsx,xmin,xmax) :  new TH1F(name,title,nbinsx,xmin,xmax); 
    case 'D':
      return ptr ? new(ptr) TH1D(name,title,nbinsx,xmin,xmax) :  new TH1D(name,title,nbinsx,xmin,xmax); 
    default:
      std::cerr << "Bad type TH1" << type << std::endl; 
      return 0; 
  }
}


