#include "SkimDataset.hh"
#include "SkimEvent.hh"
#include "ImageCalculations.hh"
#include "ImageProcessing.hh"
#include "ClusterAlgo.hh"
#include "Playlist.hh"
#include "Dataset.hh"
#include "Event.hh"
//#include "MaxCamImageTools.hh"


#include <TFile.h>
#include <TH1.h>
#include <TH2F.h>
#include <TString.h>

#include <fstream>
#include <vector>
#include <iostream>

using namespace std;
using namespace dmtpc::core;
using namespace dmtpc::image::calculations;
using namespace dmtpc::image::processing;
using namespace dmtpc::analysis;

TH2D * addNoClusters(TH2D * sum_img, TH2D * clust_hist, SkimEvent * ev, int c){

  TH2D * temp_img = sum_img;

  int xbins = temp_img->GetNbinsX();
  int ybins = temp_img->GetNbinsY();
  int nbins = xbins*ybins;
  
  //  for(int x = 0; x < xmax; x++){		
  //    for(int y = 0; y < ymax; y++){
  for(int bin = 0; bin < nbins; bin++){
    if(ev->ntracks(c) > 0){
    //don't include reconstructed track pixels
      for(int t = 0; t < ev->ntracks(c); t++){
      	if(dmtpc::analysis::clusteralgo::inCluster(ev->cluster(c)->getCluster(t), bin)){
	  continue;
      	}
	else{
	  temp_img->SetBinContent(bin,sum_img->GetBinContent(bin)+ev->image(c)->GetBinContent(bin));
	  clust_hist->SetBinContent(bin, clust_hist->GetBinContent(bin)+1);
	}
      }
    }
    else{
      temp_img->SetBinContent(bin,sum_img->GetBinContent(bin)+ev->image(c)->GetBinContent(bin));
      clust_hist->SetBinContent(bin,clust_hist->GetBinContent(bin)+1);
    }
  }    
  return temp_img;
}

bool fill_sum(int fillbin, std::vector<int> * ex_bins){

  for(int i = 0; i < ex_bins->size(); i++){
    if (fillbin == ex_bins->at(i))
      return false;
  }
  return true;
}

bool fill_sum(int fillbin, std::vector<int> ex_bins){

  for(int i = 0; i < ex_bins.size(); i++){
    if (fillbin == ex_bins.at(i))
      return false;
  }
  return true;
}
  

int main(int argc, char** argv){

  //  TFile biasfile("output/darco_bias_10.root");

  //  double biasvalue = MaxCamImageTools::getMean(biasframe);

  bool bias = false;

  double maxsparkE = 20000;
  double maxsparkR = 300;
  
  //std::cout<<"bias value: "<<biasvalue<<std::endl;

  Playlist sparks;

  ifstream infile;
  infile.open(argv[1]);
  TString tmp;
  vector<TString> datafiles;
  while(infile >> tmp){
    datafiles.push_back(tmp);
  }

  TFile * savefile = new TFile(argv[2],"recreate");
  const int ncam = 4;
  TH2F * summed_data[ncam];
  TH2F * summed_avg_data[ncam];
  TH2F * manual_summed_data[ncam];
  TH2D * noclust_sum[ncam];
  TH2D * add_counter[ncam];
  TH2D * noclust_add_counter[ncam];
  double threshold = 5000;

  //for spark cutting via playlist

  for(int i = 0; i < ncam; i++){
    summed_data[i] = new TH2F(Form("summed_image_%d",i),Form("Summed Source Data Cam %d",i),764,0, 3056,764,0,3056);
    //manual histograms with no spark clusters or images
    manual_summed_data[i] = new TH2F(Form("manual_summed_image_%d",i),Form("Summed Source Data Cam %d",i),764,0, 3056,764,0,3056);
    add_counter[i] = new TH2D(Form("add_counter_%d",i), Form("Count of Bins Summed_%d",i), 764,0, 3056, 764, 0, 3056);

    summed_avg_data[i] = new TH2F(Form("summed__avg_image_%d",i),Form("Summed Averaged Source Data Cam %d",i),764,0, 3056,764,0,3056);
    summed_avg_data[i]->SetBit(TH1::kIsAverage);
    
    //manual histograms with no tracks at all
    noclust_sum[i] = new TH2D(Form("noclust_sum_%d",i),Form("Summed Data Excluding Clusters Cam %d",i),764,0, 3056, 764, 0, 3056);
    noclust_add_counter[i] = new TH2D(Form("noclust_add_counter_%d",i), Form("Count of Bins Summed_%d (No Clusters)",i), 764,0, 3056, 764, 0, 3056);
    
  }
  //  summed_data->SetBit(TH1::kIsAverage);
  //  SkimDataset * d[datafiles.size()];
  //  Dataset * rd[datafiles.size()];


  std::cout<<"datafiles.size(): "<<datafiles.size()<<std::endl;

  for(int i = 0; i < (int)datafiles.size(); i++){
    std::cout<<"file: "<<i<<std::endl;
    std::cout<<"filename: "<<datafiles.at(i)<<std::endl;

    TString rawname = datafiles.at(i);
    rawname.ReplaceAll("/production/skimout/skim/m3/skim/GainMap/skim/","/data/m3/GainMap/");
    rawname.ReplaceAll("skim.root",".root");

    SkimDataset * d = new SkimDataset;
    Dataset * rd = new Dataset;
    
 
    d->openRootFile(datafiles.at(i));

    rd->open(rawname);

    //for each dataset store the bins that are above threshold and exclude those bins from subsequent summation
    std::vector<int> thresh_bins;

    for(int ev = 0; ev < d->nevents(); ev++){
      d->getEvent(ev);
      rd->getEvent(ev);


      
      if(ev%10==0) std::cout<<"event: "<<ev<<std::endl;
      for(int c = 3; c < 4; c++){
	bool trackcut = false;
	bool alltrackcut = false;
	if(d->event()->ntracks(c) > 0){
	  alltrackcut = true;
	  for(int t = 0; t < d->event()->ntracks(c); t++){
	    //produce summed hist with no clusters added, (not just sparks removed like below)
	    //noclust_sum and counter must also be added to if there are no tracks in event (lower down)
	    int nbins = noclust_sum[c]->GetNbinsX()*noclust_sum[c]->GetNbinsY();
	    std::vector<int> pixels = d->event()->cluster(c)->getCluster(t)->pixels;
	    for(int bin = 0; bin < nbins; bin++){
	      if(fill_sum(bin,pixels)){//check if bin is in the cluster
		   noclust_sum[c]->SetBinContent(bin,noclust_sum[c]->GetBinContent(bin)+d->event()->image(c)->GetBinContent(bin));
		   noclust_add_counter[c]->SetBinContent(bin,noclust_add_counter[c]->GetBinContent(bin)+1);
	      }
	    }
		 
	    //account for spark clusters
	    if(d->event()->E(c,t) > maxsparkE && d->event()->range(c,t) > maxsparkR){
	      //loop over cluster bins and check for the high threshold values to exclude from subsequent summation
	      //and store the bins to remove from next events. 
	      for(int bin = 0; bin < (int)d->event()->cluster(c)->getCluster(t)->pixels.size(); bin++){
		if(d->event()->image(c)->GetBinContent(d->event()->cluster(c)->getCluster(t)->pixels.at(bin)) > threshold){
		  thresh_bins.push_back(d->event()->cluster(c)->getCluster(t)->pixels.at(bin));
		}
	      }
	      trackcut = true;
	    }
	  }
	}
	if(!alltrackcut){//the event contains no tracks, so add all bins to the noclust sum
	  noclust_sum[c]->Add(d->event()->image(c));
	}
	
	if(trackcut){
	  if(ev != 0 && ev%120 == 0){
	    d->closeRootFile();
	    //delete rd;
	  }
	  continue;//images with tracks of energy and range exceeding above are excluded 
	}

	//	Const TH2F * biasframe = (TH2F*)rd->biasAvg(c)->Clone();
	//
	//double bias_mean = getMean(biasframe);
	double bias_mean = getMean(rd->biasAvg(c));
	TH2F * tmpimg = const_cast<TH2F*>((TH2F*)d->event()->image(c)->Clone("tmpimg"));
	//subtractPedestal(tmpimg,bias_mean);
      //      if(bias)
	//MaxCamImageTools::subtractPedestal(tmpimg,biasvalue);
      //tmpimg->SetBit(TH1::kIsAverage);
	//do on the fly spark cut
	double img_mean = getMean(rd->event()->ccdData(c));
	double mean_rat = img_mean/bias_mean;
	if(c == 3 && mean_rat > 1.0043){
	  sparks.add("m3",d->event()->runNumber(),d->event()->eventNumber(),c,0);
	  continue;
	}
	//loop over all image bin entries, check if the bin is in the threshold-excluded bins, if it not, then add to sum. store whether added or not for averaging - this is meant for rbi removal
	
	for(int xbin = 0; xbin < d->event()->image(c)->GetNbinsX(); xbin++){
	  for(int ybin = 0; ybin < d->event()->image(c)->GetNbinsY(); ybin++){
	    if(fill_sum(tmpimg->FindBin(xbin,ybin),thresh_bins)){
	      manual_summed_data[c]->SetBinContent(xbin,ybin,manual_summed_data[c]->GetBinContent(xbin,ybin)+d->event()->image(c)->GetBinContent(xbin,ybin));
	      add_counter[c]->SetBinContent(xbin,ybin,add_counter[c]->GetBinContent(xbin,ybin)+1);
	    }
	  }
	}
	//simple addition of event images, no 
	summed_data[c]->Add(d->event()->image(c));
	summed_avg_data[c]->Add(d->event()->image(c));
	//noclust_sum[i] = addNoClusters(noclust_sum[i],clust_counter[i],d->event(),c);
      }
    }
    d->closeRootFile();
    //    delete d;
    //    delete rd;
  }

  //----check for zero-valued bin entries in the add_counter histogram (since dividing by this histogram and dividing by zero will cause infs
  savefile->cd();
  int c = 3;
  TH2F * manual_summed_avgd[ncam];
  for(int cam = 0; cam < ncam; cam++){
    manual_summed_avgd[cam] = (TH2F*)manual_summed_data[cam]->Clone(Form("manual_summed_avgd_%d",cam));
  }
  //save zero-valued bins for filling with average of neighbours
  std::vector<int> zero_xbins;
  std::vector<int> zero_ybins;
  for(int xbin = 0; xbin < add_counter[c]->GetNbinsX(); xbin++){
    for(int ybin = 0; ybin < add_counter[c]->GetNbinsY(); ybin++){
      if(add_counter[c]->GetBinContent(xbin,ybin) == 0){
	add_counter[c]->SetBinContent(xbin,ybin,-1);
	zero_xbins.push_back(xbin);//store zero valued bin
	zero_ybins.push_back(ybin);
	continue;//skip averaging this bins as will set to average of neighbours
      }
      manual_summed_avgd[c]->SetBinContent(xbin,ybin,manual_summed_avgd[c]->GetBinContent(xbin,ybin)/add_counter[c]->GetBinContent(xbin,ybin));//average bin-by-bin
    }
  }

  //average zero-valued bin enties
  for(int i = 0; i < zero_xbins.size(); i++){//loop over x, they will be the same size
    double avg_value = 0;
    double count = 0;
    for(int x = (zero_xbins.at(i)-1); x == (zero_xbins.at(i)+1); x++){
      if(x == zero_xbins.at(i))
	continue;//don't want to include the zero bin
      for(int y = (zero_ybins.at(i)-1); y == (zero_ybins.at(i)+1); y++){
	if(y == zero_ybins.at(i))
	  continue;
	avg_value += manual_summed_avgd[c]->GetBinContent(x,y);
	count++;
      }
    }
    avg_value /= count;
    manual_summed_avgd[c]->SetBinContent(zero_xbins.at(i),zero_ybins.at(i),avg_value);
  }  



  /*
  for(int i = 0; i < ncam; i++){
    noclust_sum[i]->Divide(clust_counter[i]);
  }
  */
  sparks.save("cam3_spark_tagged.play");

  //  d->close();
  savefile->Write();
  savefile->Close();
  
}
