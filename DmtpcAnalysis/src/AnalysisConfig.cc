#include "AnalysisConfig.hh" 

dmtpc::analysis::AnalysisConfig::AnalysisConfig(const char * file)
{

  registerDoubleMap("gain"); 
  registerDoubleMap("lengthcal"); 
  registerDoubleMap("rotation"); 
  registerDoubleMap("noise"); 
  registerDoubleMap("conversion_gain"); 
  registerString("gainmap_file"); 
  registerDouble("pressure",60); 
  registerString("srim_table","SRIM_F_in_CF4_100Torr"); 
  registerBool("verbose",true); 
  registerBool("fit_do_likelihood",true); 
  registerBool("fit_do_integral",false); 
  registerBool("fit_use_gain_map_error",false); 
  registerBool("fit_enable_cauchy",false); 
  registerBool("fit_enable_second_gaus",false); 
  registerUInt("fit_minimization_max_calls",10000); 


  if (file)
    parseFile(file);
}

