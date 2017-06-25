#include "Waveform.hh"
#include <iostream>
#include <cstdlib>


ClassImp(dmtpc::core::Waveform); 

dmtpc::core::Waveform::Waveform()
{
  physical_data = 0; 
  data = 0; 
}


static void unsupported(const dmtpc::core::ScopeChannelInfo * info = 0)
{
  std::cerr << " UNSUPPORTED OPERATION IN WAVEFORM!!!! " << std::endl; 
  if (info)
  {
    std::cerr << " info->nbits: " << info->nbits << std::endl; 
    std::cerr << " info->nbytes_raw: " << info->nbytes_raw << std::endl; 
  }
  exit(1);
}



dmtpc::core::Waveform::Waveform(const char * name, const char * title, const void * raw_data, const ScopeChannelInfo * info, uint32_t secs, uint32_t nsecs) 
{


  nbits = info->nbits; 
  vmin = info->vmin;
  vmax = info->vmax;

  double tmin = -double(info->nsamples_pretrigger)/info->sample_rate*1e6; //ms->ns 
  double tmax = double(info->nsamples - info->nsamples_pretrigger - 1)/info->sample_rate*1e6; 

  if (info->nbits <= 8) 
  {
    type = 'C'; 
    data = new TH1C(name,title, info->nsamples, tmin, tmax); 

    if (info->nbytes_raw == 1)
    {
      memcpy(&(((TH1C*)data)->fArray[1]), raw_data, info->nsamples); 
    }

    else if (info->nbytes_raw == 2)
    {
      const uint16_t * short_data = (const uint16_t *) raw_data; 
      for(size_t i = 0; i< info->nsamples;i++) { ((TH1C*)data)->fArray[i+1] = (uint8_t) short_data[i]; } 
    }
    else if (info->nbytes_raw == 4)
    {
        const uint32_t * int_data = (const uint32_t *) raw_data; 
        for(size_t i = 0; i< info->nsamples;i++) { ((TH1C*)data)->fArray[i+1] = (uint8_t) int_data[i]; } 
    }
    else unsupported(info); 
  }
  else if (info->nbits <= 16) 
  {
    type = 'S'; 
    data = new TH1S(name,title, info->nsamples, tmin,tmax); 

    if (info->nbytes_raw == 2) 
    {
      memcpy(&((TH1S*)data)->fArray[1],raw_data, info->nsamples*2); 
    }
    else if (info->nbytes_raw == 4) 
    {
      const uint32_t * int_data = (const uint32_t *) raw_data; 
      for(size_t i = 0; i< info->nsamples;i++) { ((TH1S*)data)->fArray[i+1] = (uint16_t) int_data[i]; } 
    }
    else unsupported(info); 
  }

  else  if (info->nbits<= 32)
  {
    type = 'I'; 
    data = new TH1I(name,title, info->nsamples, tmin,tmax);

    if (info->nbytes_raw == 4) 
    {
      memcpy(&((TH1I*)data)->fArray[1],raw_data, info->nsamples*4); 
    }
    else unsupported(info); 
  }
  else unsupported(info); 

  physical_data = 0; 
  trace_info.secs = secs; 
  trace_info.nsecs = nsecs; 
  trace_info.channel = info->channel; 
  trace_info.scope = info->scope; 
}


uint32_t dmtpc::core::Waveform::GetBinContent(int i) const
{

  switch(type)
  {
    case 'C':
      return (uint8_t) ((TH1C*) data)->fArray[i]; 
    case 'S':
      return (uint16_t) ((TH1S*) data)->fArray[i]; 
    case 'I':
      return (uint32_t) ((TH1I*) data)->fArray[i]; 
    default:
      unsupported(); 
      return 0; 
  }
}



TH1D* dmtpc::core::Waveform::physical() const
{

  if (physical_data) return physical_data; 

  physical_data = new TH1D(TString::Format("%s_physical", data->GetName()), TString::Format("%s (Physical)",data->GetTitle()), 
                           data->GetNbinsX(), data->GetXaxis()->GetXmin(), data->GetXaxis()->GetXmax());
  physical_data->GetXaxis()->SetTitle("ns"); //data->GetXaxis()->GetTitle());
  physical_data->GetYaxis()->SetTitle("mV");
  for (int i = 1; i<= physical_data->GetNbinsX(); i++)
  {
    physical_data->SetBinContent(i,GetPhysicalBinContent(i)); 
  }

  return physical_data; 
}


void dmtpc::core::Waveform::destroyPhysical() const
{
  if (!physical_data) return; 
  delete physical_data; 
  physical_data = 0; 
}

dmtpc::core::Waveform::~Waveform()
{
  if(data) delete data; 
  destroyPhysical(); 
}




double dmtpc::core::Waveform::GetPhysicalBinContent(int i) const
{
  size_t nlevels = 1 << (nbits); 
  uint32_t raw = GetBinContent(i); 
  double millivolts_per_level = 1000*(vmax- vmin) / nlevels; 
  return vmin*1000 + raw * millivolts_per_level; 
}
