#include "CleanSkimConfig.hh" 
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>

dmtpc::analysis::cleanskim::Config::Config()
{
  setDefaults(); 
}

dmtpc::analysis::cleanskim::Config::Config(const char * file)
{
  setDefaults();
  parseFile(file); 
}

void dmtpc::analysis::cleanskim::Config::setDefaults()
{

 const char * compression_keys[5]; 
 compression_keys[0]="global"; 
 compression_keys[1]="zlib"; 
 compression_keys[2]="lzma"; 
 compression_keys[3]="old"; 
 compression_keys[4]="future"; 
 registerEnum("compression_algo",5,compression_keys); 

 registerInt("compression_level",5); 
 registerBool("overscan", true); 
 registerBool("normalize_gainmap",true); 
 registerString("interpolation_method","bicubic"); 
 registerUInt("burnin_num_events",1000); 
 registerDouble("burnin_distance_thresh",12); 
 registerDouble("outlier_factor",1.25); 
 registerDouble("legacy_blur_amount",0.8); 
 registerDouble("gaussian_blur_amount",2); 
 registerDouble("cluster_min_sigma",3.7); 
 registerDouble("cluster_max_sigma", 300); 
 registerInt("cluster_min_size",5); 
 registerInt("cluster_min_size_unbinned", 16); 
 registerDouble("cluster_min_dist", 3600); 
 registerInt("range_algorithm", 1); 
 registerInt("phi_algorithm", 2); 
 registerDouble("north_angle", 240); 
 registerDouble("longitude",-71.110556); 
 registerDouble("latitude",42.373611); 
 registerDouble("partial_blur",2);
 registerDouble("partial_low_thresh",5.15);
 registerDouble("partial_high_thresh",18);
 registerDouble("partial_n_primary_thresh",85);
 registerDouble("partial_n_secondary_thresh",75);
 registerDouble("partial_dist_low",2);
 registerDouble("partial_dist_high",8);

 registerBool("mc",false); 
 registerBool("nokill",false); 

 registerDouble("cluster_red_threshold",3.7); 
 registerBool("stitch", false); 

 registerStringMap("channel_types"); 
 registerDouble("sat_thresh", 55000); 
 registerInt("n_sat_thresh", 4); 


 registerBool("gain_map",false); 
 registerString("gain_map_file","" ); 
 registerString("stitch_file", "auto"); 
 registerString("stitch_dir", ""); 

 registerInt("cluster_find_seed_blur_radius",4); 

 registerDouble("cluster_find_seed_thresh_ring_ratio", 0.75); 
 registerDouble("cluster_find_seed_min_threshold", 0.75); 
 registerDouble("cluster_find_seed_seed_threshold",5.2); 

 registerInt("cluster_neighbors_threshold_for_filling", 4);

 registerInt("cluster_min_neighbors_to_keep_pixel", 2);

 registerDouble("cluster_join_min_rxy_cluster",0.8); 

 registerDouble("cluster_join_min_rxy_global",0.65); 

 registerDouble("cluster_join_max_join_residual", 5); 

 registerDouble("cluster_join_least_squares_weight", 2); 
     
 registerInt("cluster_find_ad_niter",30); 

 registerBool("cluster_find_ad_zero_neg", true); 

 registerDouble("cluster_find_ad_gradient_blur",2.5); 

 registerDouble("cluster_find_ad_high_thresh",8.5); 

 registerDouble("cluster_find_ad_low_thresh",4); 


 registerDouble("cluster_join_spacer_width",2); 

 registerDouble("cluster_find_ad_k",25); 

 registerDouble("cluster_find_ad_lambda",1); 

 registerDouble("cluster_find_ring_space_sigma",4); 
 registerDouble("cluster_find_ring_rms_sigma",1); 
 registerDouble("cluster_find_ring_core_thresh_low",0.75); 
 registerDouble("cluster_find_ring_core_thresh_high",1.4); 
 registerDouble("cluster_find_ring_ring_thresh",0.5); 
 registerDouble("cluster_find_ring_ring_nsigma",3); 
 registerInt("cluster_find_ring_ncleanup",0); 

  
  std::vector<const char *> algos;
  algos.push_back("seed"); 
  registerStringList("cluster_find_algo",&algos); 


  registerDoubleMap("spark_cut"); 
  registerDoubleMap("overscan_spark_cut"); 

//  setDoubleMap("overscan_spark_cut","081264",-1); 
//  setDoubleMap("overscan_spark_cut","100439",-7.5); 

  registerBool("cluster_find_ci_use_gain_map_for_merging", false); 

  registerDouble("spacer_join_r",30); 
  registerDouble("spacer_join_theta",0.2); 



  const char * bias_clean_keys[2] = {"traditional","median_stack"}; 

  registerEnum("bias_clean_method",2, bias_clean_keys); 

  const char * image_clean_keys[3]; 
  image_clean_keys[0] = "traditional"; 
  image_clean_keys[1] = "bias_subtract_first"; 
  image_clean_keys[2] = "median_difference"; 
  registerEnum("image_clean_method",3, image_clean_keys); 

  const char * image_ratio_keys[2];
  image_ratio_keys[0] = "mean"; 
  image_ratio_keys[1] = "median"; 
  registerEnum("image_ratio_method",2, image_ratio_keys); 

  const char * save_type_keys[3]; 
  save_type_keys[0] = "float"; 
  save_type_keys[1] = "short"; 
  save_type_keys[2] = "int"; 
  registerEnum("hist_save_type",3,save_type_keys); 

  registerDouble("round_amount",0); 
  registerBool("round_in_clusters",false); 

  registerBool("seed_cluster_find_reproduce_v4_bug",true); 

}


