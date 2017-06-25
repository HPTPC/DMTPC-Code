#ifndef DMTPC_IMAGE_TOOLS_HH
#define DMTPC_IMAGE_TOOLS_HH
class TH1; 
class TH2; 


namespace dmtpc 
{
  namespace image
  {
    namespace tools
    {
      TH2 * newTH2(char type, const char * name, const char * title, int nbinsx, double xmin, double xmax, int nbinsy, double ymin, double ymax);
      TH1 * newTH1(char type, const char * name, const char * title, int nbinsx, double xmin, double xmax, TH1 * ptr = 0); 
      TH2 * newTH2StealType(const TH2* type_giver, const char * name, const char * title, int nbinsx, double xmin, double xmax, int nbinsy, double ymin, double ymax);
      TH2 * newTH2StealTypeAndSize(const TH2* type_giver, const char * name, const char * title); 
      TH2 * newTH2StealSize(const TH2* size_giver, char type, const char * name, const char * title); 
      TH1 * newTH1StealSize(const TH1* size_giver, char type, const char * name, const char * title); 
      TH1 * newTH1StealType(const TH1* type_giver, const char * name, const char * title, int nbinsx, double xmin, double xmax, TH1 * placement_ptr = 0); 
      unsigned int TH1GetNbins(const TH1* hist); 
    }
  }
}


#endif
