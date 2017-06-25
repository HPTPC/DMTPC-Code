//increment when breaking data file compatibility
#define CLEAN_SKIM_VERSION 6


#include "ScopeInfo.hh"
#include "ImageCalculations.hh"
#include "ImageProcessing.hh"
#include "Stitcher.hh"
#include "CameraInfo.hh"
#include "DatasetHeader.hh" 
#include "Compression.h"
#include "CleanSkimFunctions.hh"
#include <algorithm>
#include <numeric>
#include "Event.hh"
#include "Dataset.hh"
#include "SkimDataset.hh"
#include "SkimEvent.hh"
#include "TTree.h"
#include "TFile.h"
#include "TCutG.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include "CleanSkimConfig.hh"

//#include "CspWfVector.hh"

class CspWfVector;

using namespace std;
using namespace dmtpc::analysis::cleanskim; 
using namespace dmtpc::analysis;


int dmtpc::analysis::cleanskim::Functions::cleanSkim(dmtpc::core::Dataset & d, int runnum, SkimDataset & sd, bool delete_intermediate, 
						     const Config * conf, unsigned algo_index, dmtpc::Quality::RunQuality &runQ)
{


  /************************/
  /**** Initialization  ***/ 
  /************************/

  //load spark tagged events from quality database
  runQ.loadSparkTaggedEvents();

  /** Temporary tree without temporal information **/ 
  TString tmpoutfilename = TString(sd.getFileName()).ReplaceAll(".root","_tmp.root");
  TFile* tmpoutfile = new TFile(tmpoutfilename,"RECREATE");

  int  compression_settings = ROOT::CompressionSettings( (ROOT::ECompressionAlgorithm) conf->getEnum("compression_algo"), conf->getInt("compression_level"));  
 

  tmpoutfile->SetCompressionLevel(compression_settings); 
  sd.file()->SetCompressionLevel(compression_settings); 
  TTree * tmpskimtree; 
  tmpskimtree = new TTree(algo_index == 0 ? "skim" : conf->getStringList("cluster_find_algo",algo_index),"Temp Skim Tree"); 

  SkimEvent * tmp_event = new SkimEvent; 

  /* Load first event to get information */ 
  d.getEvent(0); 
  const int ncam_in = d.getHeader()->ncameras; 
  const int nbinsx =  d.event()->ccdData(0)->GetNbinsX();
  const int nbinsy =  d.event()->ccdData(0)->GetNbinsY();
  const double xmin =  d.event()->ccdData(0)->GetXaxis()->GetXmin();
  const double xmax =  d.event()->ccdData(0)->GetXaxis()->GetXmax();
  const double ymin =  d.event()->ccdData(0)->GetYaxis()->GetXmin();
  const double ymax =  d.event()->ccdData(0)->GetYaxis()->GetXmax();

  TH2F * blank = new TH2F("blank","blank", nbinsx, xmin, xmax, nbinsy, ymin,ymax); 
  
//  const int eventVersion = d.event()->IsA()->GetClassVersion();

  double last_spark_mean[ncam_in] ;
  ClusterImage * clusti = 0; 

  /* Support processing on datasets with no actual images */ 
  bool fake_image = false; 
  if (ncam_in ==0 || d.event()->ccdData(0)==NULL || d.event()->ccdData(0)->Integral() ==0)
  {
    fake_image = true; 
  }

 //Get camera id's 
 string camera_ids[ncam_in];  
 for (int u = 0; u < ncam_in; u++)
 {
    camera_ids[u] = std::string(d.event()->ccdConfig(u)->serialNumber.Data()); 
 }

 int ncam_out = conf->getBool("stitch") ? 1 : ncam_in; 
 tmp_event->initCamVectors(ncam_out); 
// tmp_event->initTrackVectors(ncam_out, 15); //backwards compatibility
 tmp_event->_clusters.insert(tmp_event->_clusters.begin(),ncam_out,NULL); 
 tmp_event->_ncamera = ncam_out; 
 // tmpskimtree->Branch("tmp_event","dmtpc::analysis::SkimEvent",&tmp_event,32000,1); 
 tmpskimtree->Branch("tmp_event","dmtpc::analysis::SkimEvent",&tmp_event,32000,1); 
 gROOT->cd();


 /* Open up file for bias frame saving */
 TFile * biasoutfile = 0;
 vector<TH2*> biases(ncam_in); 
 vector<TH2*> os_biases(ncam_in); 
 vector<double> bias_mean(ncam_in); 
 vector<double> os_bias_mean(ncam_in); 
 TTree * biastree = 0; 

 bias_clean_method_t bcm = (bias_clean_method_t) conf->getEnum("bias_clean_method"); 

 if (algo_index == 0) // We only handle bias frame once
 {
    TString biasoutfilename = TString(sd.getFileName()).ReplaceAll("skim.root","bias.root");
    biasoutfile = new TFile(biasoutfilename,"RECREATE");

    TH2F * biasframe = 0;
    TH2F * osframe;
    biastree = new TTree("bias","Bias Information");
    biastree->Branch("biasframe","TH2F",&biasframe,128000,0);
    gROOT->cd();

    for (int u = 0; u < ncam_in; u++)
    {
      //Get cleaned bias frame

      if (fake_image)
      {
        biasframe = new TH2F;   
      }
      else
      {
        TH2F * rawframe = bcm == BIAS_CLEAN_MEDIAN_STACK  
                          ? (TH2F*) Functions::medianBiasFrameStack((TTree*) d.getBiasTree(),u,conf,false)
                          : (TH2F*) d.biasAvg(u);                              

        biasframe =  (TH2F*)Functions::cleanBiasFrame(rawframe, conf); 

        if (bcm == BIAS_CLEAN_MEDIAN_STACK)
        {
          delete rawframe; 
        }
      }
      
      bias_mean[u] = dmtpc::image::calculations::getMean(biasframe);     
      last_spark_mean[u] = bias_mean[u]; 

      if (conf->getBool("overscan"))
      {

         TH2F * rawframe = bcm == BIAS_CLEAN_MEDIAN_STACK  
                          ? (TH2F*) Functions::medianBiasFrameStack((TTree*) d.getBiasTree(),u,conf,true)
                          : (TH2F*) d.biasOSAvg(u);                              
         osframe = (TH2F*) Functions::cleanBiasFrame(rawframe, conf); 

         if (bcm == BIAS_CLEAN_MEDIAN_STACK)
         {
            delete rawframe;   
         }

         os_bias_mean[u] = dmtpc::image::calculations::getMean(osframe); 
         os_biases[u] = osframe; 
      }
       
      biasoutfile->cd(); 
      biastree->Fill(); 
      gROOT->cd(); 
      biases[u] = biasframe; 
    }
  }

   cout << "All preclean activities done" << endl;


  /* Running list of positions so we don't have to read back through skim file
     all the time. It is a List of a Vector[2] of a double[2].*/
  list<vector<vector<pair<double,double> > > > positions; 
  

  /* List of spark ref excluded pixels */
  vector<pair<int,int> > * sparkref_running = new vector<pair<int,int> >[ncam_in]; 
  list<vector<pair<int,int> >*>  sparkref; 

  list<vector<vector<vector<BurninEncoded_t> > >*> burnin_temp; //Temporary list that is updated a lot
  list<vector<vector<vector<BurninEncoded_t> > >*> burnin; //permanent list 

  const int nev = d.nevents(); 

  Stitcher * stitch = conf->getBool("stitch") ? Functions::loadStitch(runnum, conf) : 0 ; 
  if (stitch) sd.writeStitch(stitch); 

  //Load Gain Maps; 
  vector<GainMap *> gainmaps (ncam_out);
  Functions::loadGainMaps(&(gainmaps[0]), &sd,  ncam_in, conf, stitch, camera_ids); 
  
  vector<int> last_spark(ncam_in,0); 

  //initialize waveform stuff
 
  cout << "Initializing waveform_vectors" << endl;

  cout << "d.event()->nscopes() = " << d.event()->nscopes() << endl;
  for(unsigned iscope = 0; iscope < d.event()->nscopes(); iscope++)
  { 
    cout << "iscope = " << iscope << endl;
    //cout << "nchannels = " << d.event()->getScopeConfig(iscope)->channel_info.size() << endl;
    for(unsigned ich=0; ich<d.event()->getScopeConfig(iscope)->channel_info.size(); ++ich)
    {
      cout << "ich = " << ich << endl;
      TString chName(d.event()->getScopeConfig(iscope)->channel_info[ich].name);
      cout << "chName = " << chName << endl;
      char readChannel(chName[11]); // assumes name is in form scope_X_ch_X_XXX
      int readout_channel = readChannel - '0';
      cout << "readout_channel = " << readout_channel << endl;

      // initialize waveform vectors according to enabled channels
      if (chName.Contains("csp",TString::kIgnoreCase)) 
	tmp_event->_cspwf_vectors.push_back(new dmtpc::waveform::CspWfVector(0,iscope,readout_channel,chName.Data())); 
      else if (chName.Contains("fas",TString::kIgnoreCase)) 
	tmp_event->_fastwf_vectors.push_back(new dmtpc::waveform::FastWfVector(0,iscope,readout_channel,chName.Data()));
      else if (chName.CompareTo("pmt",TString::kIgnoreCase)) 
	tmp_event->_pmtwf_vectors.push_back(new dmtpc::waveform::PMTWfVector(0,iscope,readout_channel,chName.Data()));
      else tmp_event->_waveform_vectors.push_back(new dmtpc::waveform::WaveformVector(0,iscope,readout_channel,chName.Data()));
    }
  }


  tmp_event->_runNumber = runnum; 

  /************************/
  /**** Main Event Loop ***/ 
  /************************/
  for (int i = 0; i < nev; i++)
  {
     cout << "Processing event:  " << i << endl;
     d.getEvent(i); 

     for (unsigned c = 0; c < tmp_event->_clusters.size(); c++)
     {
       if (tmp_event->_clusters[c]) 
       {
         delete tmp_event->_clusters[c]; 
         tmp_event->_clusters[c] = 0; 
       }
     }
//     tmp_event->_clusters->Clear(); 

     vector<vector<vector<BurninEncoded_t> > > * this_event_burnin= new vector<vector<vector<BurninEncoded_t> > >;

     /* TODO add waveform vectors */
     // Assume every channel is a CspWfVector for now
     /* 
    int nch = tmp_event->_waveform_vectors.size();
     cout << "nch = " << nch << endl;
     for (int ich=0; i<nch; ich++)
       {
	 dmtpc::waveform::CspWfVector *wv= static_cast<dmtpc::waveform::CspWfVector*>((*wvlist)[ich]);
	  wv->clear();

	  fillCspWfVector(d, wv, ich, nch);
       }
     */

     // clear waveform vectors first
     for (unsigned int wfvIndex=0; wfvIndex<tmp_event->_cspwf_vectors.size(); wfvIndex++) 
       tmp_event->_cspwf_vectors[wfvIndex]->clear();
     for (unsigned int wfvIndex=0; wfvIndex<tmp_event->_fastwf_vectors.size(); wfvIndex++) 
       tmp_event->_fastwf_vectors[wfvIndex]->clear();
     for (unsigned int wfvIndex=0; wfvIndex<tmp_event->_pmtwf_vectors.size(); wfvIndex++) 
       tmp_event->_pmtwf_vectors[wfvIndex]->clear();
     for (unsigned int wfvIndex=0; wfvIndex<tmp_event->_waveform_vectors.size(); wfvIndex++) 
       tmp_event->_waveform_vectors[wfvIndex]->clear();

     int total_nch=0;
     for(unsigned iscope = 0; iscope < d.event()->nscopes(); iscope++) 
       total_nch+=d.event()->getScopeConfig(iscope)->channel_info.size();
     cout << "total_nch = " << total_nch << endl;

     int cspIndex=0;
     int fastIndex=0;
     int pmtIndex=0;
     for(unsigned iscope = 0; iscope < d.event()->nscopes(); iscope++)
       {
	 cout << "filling waveform vectors for scope " << iscope << endl;
	 for(unsigned ich=0; ich<d.event()->getScopeConfig(iscope)->channel_info.size(); ++ich)
	   {
	     TString chName(d.event()->getScopeConfig(iscope)->channel_info[ich].name);
	     if (chName.Contains("csp",TString::kIgnoreCase)) {
	       fillCspWfVector(d,tmp_event->_cspwf_vectors[cspIndex],ich,total_nch);
	       cspIndex++;
	     }
	     else if (chName.Contains("fas",TString::kIgnoreCase)) {
	       fillFastWfVector(d,tmp_event->_fastwf_vectors[fastIndex],ich,total_nch);
	       fastIndex++;
	     }
	     else if (chName.CompareTo("pmt",TString::kIgnoreCase)) {
	       fillPMTWfVector(d,tmp_event->_pmtwf_vectors[pmtIndex],ich,total_nch);
	       pmtIndex++;
	     }
	     else cout << "WARNING: No analysis for generic waveform_vectors is currently implemented!" << endl;
	   }
       }


     //cout << "cspwf_vectors->GetEntries()=" << tmp_event->_cspwf_vectors.GetEntries() << endl;
     //cout << "cspwf_vectors->GetSize()=" << tmp_event->_cspwf_vectors.GetSize() << endl;

    TCutG os_cut("os_cut",5);
    if (conf->getBool("overscan"))
    {
      os_cut.SetPoint(0,0,1028);
      os_cut.SetPoint(1,1024,1028);
      os_cut.SetPoint(2,1024,1032);
      os_cut.SetPoint(3,0,1032);
      os_cut.SetPoint(4,0,1028);
    }

     int nspark = 0; 
     vector<const TH2*> cleaned_images(ncam_in); 
     vector<bool> spark(ncam_in,false); 
     vector<double> image_rms(ncam_in,0); 
     vector<double> cleaned_image_rms(ncam_in,0); 
     vector<double> cleaned_blurred_image_rms(ncam_in,0); 
     vector<double> cleaned_blurred_image_mean(ncam_in,0); 
     vector<double> cleaned_image_mean(ncam_in,0); 
     vector<double> raw_rms(ncam_in,0); 
     vector<double> image_mean(ncam_in,0); 
     vector<double> os_mean(ncam_in,0); 
     vector<double> os_rms(ncam_in,0); 
     vector<int> nkilled(ncam_in,0); 

     /**************************************/
     /**** Cleaning and Spark Detection  ***/ 
     /**************************************/
     for(int u=0; u<ncam_in; u++)
     {
        const TH2 * raw_img = d.event()->ccdData(u); 
        const TH2 * raw_overscan =  conf->getBool("overscan") ? d.event()->overscan(u) : 0 ; 

        cout << "\t CLEANING "<< u << "" << endl;
        raw_rms[u] = dmtpc::image::calculations::getRMS(raw_img); 
        // keep track of the last frame for the spark cut


        //Check for spark
        
        double spark_mean = fake_image ? 0 : dmtpc::image::calculations::getMean(raw_img); 
        double os_spark_mean = conf->getBool("overscan") ? dmtpc::image::calculations::getMean(raw_overscan, &os_cut) : 0; 

        if (!fake_image && !conf->getBool("mc")) 
        {
	  spark[u] = Functions::checkSpark(i,camera_ids[u].c_str(), spark_mean, last_spark_mean[u], os_spark_mean, conf, runQ); 
          //spark[u] = Functions::checkSpark(i,camera_ids[u].c_str(), spark_mean, last_spark_mean[u], os_spark_mean, conf); 
          if(!spark[u]) last_spark_mean[u] = spark_mean; 


         //Populate Sparkref if necessary 
          if (spark[u]) Functions::populateSparkRef(raw_img, biases[u], conf, &sparkref_running[u]); 
          if (spark[u]) nspark++; 
        }
         
        //Clean Images
        int clean_idx = stitch ? stitch->getIndex(camera_ids[u].c_str()) : u; 
        cleaned_images[clean_idx] = fake_image ? (TH2F*) blank->Clone() : 
                                         spark[u] ? (TH2F*) raw_img->Clone("sparkimg") : 
                                                    ((TH2F*) Functions::cleanImage(raw_img, biases[u], nkilled[u],
                                                    raw_rms[u], spark_mean, raw_overscan, os_spark_mean, os_bias_mean[u], conf)); 



        //Get image, overscan, mean and rms
        dmtpc::image::calculations::meanRMSNoOutliers(raw_img, image_mean[u], image_rms[u]); 
        dmtpc::image::calculations::meanRMSNoOutliers((TH2F*)cleaned_images[clean_idx], cleaned_image_mean[clean_idx], cleaned_image_rms[clean_idx]); 

        // we need blurred rms and mean for stitched seedClusterFind
        if (stitch && !strcmp(conf->getStringList("cluster_algo",algo_index),"seed"))
        {
          TH2 * blurred = dmtpc::image::processing::gaussianBlur(cleaned_images[clean_idx], conf->getInt("cluster_find_seed_blur_radius"), conf->getDouble("gaussian_blur_amount")); 
          dmtpc::image::calculations::meanRMSNoOutliers(blurred,cleaned_blurred_image_mean[clean_idx],cleaned_blurred_image_rms[clean_idx]); 
          delete blurred; 
        }

        if (conf->getBool("overscan")) dmtpc::image::calculations::meanRMSNoOutliers(raw_overscan, os_mean[u], os_rms[u]); 


        
        //Check for partial spark
        if (!fake_image && !conf->getBool("mc") && !spark[u])
        {
//          cout << "Checking partial " << endl; 
          spark[u] = Functions::checkPartialSpark(cleaned_images[clean_idx], conf); 
        }

        if (spark[u]) last_spark[u] = 0; 
        else last_spark[u]++; 


     }


     

     cout << "\t DONE CLEANING" << endl;

     TH2F * stitched = stitch ? (TH2F*) stitch->stitch(&cleaned_images, conf->getString("interpolation_method")) : 0; 
    
     tmp_event->_stitched = stitch ? true : false; 

     tmp_event->clearTrackVectors(); 
     for (int u = 0; u < ncam_out; u++)
     {

       TH2F * img = stitch ? stitched : (TH2F*)  cleaned_images[u]; 
       vector< vector<BurninEncoded_t> >  this_camera_burnin ;

       //Average camera for stitched images for output tree for now
       tmp_event->_integral[u] = stitch ? std::accumulate(raw_rms.begin(), raw_rms.end(),0.) / ncam_in : raw_rms[u]; 
       tmp_event->_image_rms[u] = stitch ? std::accumulate(image_rms.begin(), image_rms.end(),0.) / ncam_in : image_rms[u]; 
       tmp_event->_image_mean[u] = stitch ? std::accumulate(image_mean.begin(), image_mean.end(),0.) / ncam_in : image_mean[u]; 
       //tmp_event->_os_mean[u] = stitch ? std::accumulate(os_mean.begin(), os_mean.end(),0) / ncam_in : os_mean[u]; 
       //tmp_event->_os_rms[u] = stitch ? std::accumulate(os_rms.begin(), os_rms.end(),0) / ncam_in : os_rms[u]; 
       tmp_event->_pixels_killed[u] = stitch ? std::accumulate(nkilled.begin(), nkilled.end(),0)  : nkilled[u]; 
       tmp_event->_lastspark[u] = stitch ? *(std::min_element(last_spark.begin(), last_spark.end())) : last_spark[u]; 
       if (!stitch)
       {
         tmp_event->_cameraSerialNumber[u] = camera_ids[u]; 
       }
       else
       {
         tmp_event->_cameraSerialNumber[u] = "Stitched"; 
//         for (int c = 0; c < ncam_in; c++)
//         {
//           if (c > 0) 
//           {
//             tmp_event->_cameraSerialNumber[u] += "&"; 
//           }
//           tmp_event->_cameraSerialNumber[u] += camera_ids[c]; 
//         }
       }


       if (spark[u] || (stitched && nspark > 0))
       {
          cout << "Spark!" << endl; 
          //Just write out the spark 
          tmp_event->_ntracks[u] = 0; 
          tmp_event->_spark[u] = 1; 
          TH2F * sparkimg = (TH2F*) img->Clone("sparkimg"); 
          clusti = new ClusterImage(sparkimg, d.event()->getTimestamp()); 
          tmp_event->addTrackVector(15); //backwards compatibility
       }
       else if (fake_image)
       {
          tmp_event->_spark[u] = 0; 
          tmp_event->_ntracks[u] = 0;
          tmp_event->addTrackVector( 15); //backwards compatibility
       }
       else
       {
  
         tmp_event->_spark[u] = 0; 
         const dmtpc::image::StitchInfo * sinfo = 0; 
         if (stitch) sinfo = stitch->getStitchInfo(cleaned_images[0]->GetNbinsX());
         TH2F * clustimg = (TH2F*) img->Clone("clustimg"); 
         clusti = new ClusterImage(clustimg, d.event()->getTimestamp()); 
         tmp_event->_ntracks[u] = Functions::clusterFind(clusti, gainmaps[u], conf, sinfo,  &(cleaned_image_rms[0]),  &(cleaned_blurred_image_rms[0]) ,  &(cleaned_image_mean[0]), &(cleaned_blurred_image_mean[0]), algo_index, u);

         tmp_event->addTrackVector( tmp_event->_ntracks[u] > 15 ? tmp_event->_ntracks[u] : 15 ); //backwards compatibility
//         cout << tmp_event->_range.size() << endl; 
//         cout << tmp_event->_range[0].size() << endl; 
         /************************/
         /*** Loop over tracks ***/ 
         /************************/

         for (int v = 0; v < tmp_event->_ntracks[u]; v++)
         {
 
            Functions::fillTrackInfo(tmp_event, u,v,clusti->getCluster(v),conf,gainmaps[u], stitch, camera_ids[u].c_str(),d.event());  
      
            this_camera_burnin.push_back(Functions::computeTrackBurnin(&positions, &burnin_temp, 
                                                                               i, tmp_event->x(u,v), 
                                                                               tmp_event->y(u,v), u, v,conf)); 
         }

     }


     if (algo_index > 0) 
     {
//       clusti->forgetImage(); 
     }
     else if( (hist_save_type_t) conf->getEnum("hist_save_type") != HIST_SAVE_TYPE_FLOAT) 
     {
       switch ((hist_save_type_t) conf->getEnum("hist_save_type"))
       {
         case HIST_SAVE_TYPE_SHORT: 
            clusti->changeHistType('S');  
            break; 
         case HIST_SAVE_TYPE_INT: 
            clusti->changeHistType('I');  
            break; 
        default: 
            cerr << "Unrecognized histogram type "<< endl; 
       }

     }

     else if (conf->getDouble("round_amount") != 0)
     {
      clusti->roundValues(conf->getBool("round_in_clusters"), conf->getDouble("round_amount")); 
     }   


     tmp_event->_clusters[u] = clusti; 
     this_event_burnin->push_back(this_camera_burnin);


   } /*End loop over cameras */


   if (!fake_image) Functions::updateGlobalSparkRef(&sparkref_running, &sparkref, ncam_out); 


   //Fill Temp Tree 
   tmp_event->_eventNumber = i; 
   tmpoutfile->cd(); 
   tmpskimtree->Fill(); 
   gROOT->cd(); 

   if (stitched) delete stitched; 
   for (unsigned u = 0; u < cleaned_images.size(); u++) delete cleaned_images[u]; 

   //Update global burnin list 
   if (!fake_image) Functions::updateGlobalBurnin(&positions,&burnin_temp, &burnin, tmp_event, conf, ncam_out, this_event_burnin); 

  } //End loop over events


  while (burnin_temp.size() > 0)
  {
     burnin.push_back(burnin_temp.front());
     burnin_temp.pop_front(); 
  }

  //Write out and clean up

  tmpoutfile->cd();
  tmpskimtree->Write();

  if (algo_index == 0)
  {
    biasoutfile->cd();
    biastree->Write();
  }
 

  //Save configuration
  if (algo_index == 0)
  {
    std::stringstream str; 
    conf->print(str); 
    sd.setConfig(str.str().c_str()); 
    sd.writeConfig(); 
  }

  sd.mergeTempTrees(tmpskimtree,&burnin, &sparkref, runnum);    

  if (conf->getBool("gain_map"))
  {
    for (int i = 0; i < ncam_out; i++) 
    {
      if (gainmaps[i]) gainmaps[i]->Delete(); 
    }
  }
  
  
  
  while(burnin.size() > 0)
  {
    delete burnin.front(); 
    burnin.pop_front(); 
  }

  while(sparkref.size() > 0)
  {
    delete sparkref.front(); 
    sparkref.pop_front(); 
  }
  
  delete tmpskimtree;
  tmpoutfile->Close();
 
  delete biastree;
  if (algo_index == 0) 
  {
    biasoutfile->Close();
  }
  
  if (delete_intermediate)
  {
     cout << "Deleting " << tmpoutfilename << endl;
    unlink(tmpoutfilename.Data());
  }

  return 0; 
}


