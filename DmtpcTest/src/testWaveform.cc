#include "Config.hh" 
#include "TMath.h" 
#include "TRandom3.h" 
#include "TCanvas.h" 
#include "Waveform.hh"
#include "ScopeInfo.hh" 



int main (int nargs, char ** args) 
{
  TRandom3 rand; 


  int ret = 0; 
  //test char 

  uint8_t * char_data = new uint8_t[8000]; 
  uint16_t * short_data = new uint16_t[8000]; 
  uint32_t * int_data = new uint32_t[8000]; 


  TH1D physical("physical","physical",8000,-0.001,0.007);

  for (int i = 0; i < 8000; i++)
  {
    double v = 0; 

    if (i > 1000 && i <= 1200)
    {
      v = (i-1000)*0.5/200; 
    }
    else if (i > 1200)
    {
      v = 0.5 * TMath::Exp((1200.-i)/2000.); 
    }

    v+=0.01*rand.Gaus();
    physical.SetBinContent(i+1,v); 


    char_data[i] = (v+1)/2. * 255;  
    short_data[i] = (v+1)/2 * 2047; 
    int_data[i] = (v+1)/2 * ((1<<23)-1); 


  }


  dmtpc::core::ScopeChannelInfo char_info; 

  char_info.vmin = -1; 
  char_info.vmax = 1; 
  char_info.nbits = 8; 
  char_info.nbytes_raw = 1; 
  char_info.sample_rate = 1e6; 
  char_info.nsamples = 8000; 
  char_info.nsamples_pretrigger = 1000; 
  char_info.channel= 0; 
  char_info.scope= 0; 

  double timestamp = 0.5; 

 
  dmtpc::core::ScopeChannelInfo short_info = char_info; 
  short_info.nbytes_raw = 2; 
  short_info.nbits = 12; 

  dmtpc::core::ScopeChannelInfo int_info = char_info; 
  int_info.nbytes_raw = 4; 
  int_info.nbits = 24; 

  dmtpc::core::Waveform char_wav("char_wav","Char wave", char_data, &char_info, timestamp); 
  dmtpc::core::Waveform short_wav("short_wav","Short wave", short_data, &short_info, timestamp); 
  dmtpc::core::Waveform int_wav("int_wav","Int wave", int_data, &int_info, timestamp); 

  for (int i = 0; i < 8000; i++)
  {

    if(char_data[i] != ((uint8_t) char_wav.GetBinContent(i+1)))
    {
      ret++; 
      std::cerr << "REASON: char data doesn't match at location " << i << std::endl; 
    }


    if(short_data[i] != ((uint16_t) short_wav.GetBinContent(i+1)))
    {
      ret++; 
      std::cerr << "REASON: short data doesn't match at location " << i << std::endl; 
    }

    if(int_data[i] != ((uint32_t) int_wav.GetBinContent(i+1)))
    {
      ret++; 
      std::cerr << "REASON: int data doesn't match at location " << i << std::endl; 
    }
  }
 


  TCanvas c1("test","test",600,1000); 
  c1.Divide(2,2) ; 

  c1.cd(1); 
  physical.Draw(); 
  c1.cd(2); 
  char_wav.Draw(""); 

  c1.cd(3); 
  short_wav.Draw(""); 
  c1.cd(4); 
  int_wav.Draw(""); 

  c1.SaveAs("output/char_wav.png"); 

}


