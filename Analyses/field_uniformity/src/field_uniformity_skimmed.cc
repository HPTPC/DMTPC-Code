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
  
  //std::cout<<"bias value: "<<biasvalue<<std::endl;

  Playlist sparks;

  ifstream infile;
  infile.open(argv[1]);
  TString tmp;
  vector<TString> datafiles;
  while(infile >> tmp){
    datafiles.push_back(tmp);
  }

  TFile savefile(argv[2],"recreate");
  const int ncam = 4;
  TH2F * summed_data[ncam];
  TH2F * summed_avg_data[ncam];
  TH2F * manual_summed_data[ncam];
  //  TH2D * noclust_sum[ncam];
  TH2D * add_counter[ncam];
  double threshold = 5000;

  //for spark cutting via playlist

  for(int i = 0; i < ncam; i++){
    summed_data[i] = new TH2F(Form("summed_image_%d",i),Form("Summed Source Data Cam %d",i),764,0, 3056,764,0,3056);
    manual_summed_data[i] = new TH2F(Form("manual_summed_image_%d",i),Form("Summed Source Data Cam %d",i),764,0, 3056,764,0,3056);
    summed_avg_data[i] = new TH2F(Form("summed__avg_image_%d",i),Form("Summed Averaged Source Data Cam %d",i),764,0, 3056,764,0,3056);
    summed_avg_data[i]->SetBit(TH1::kIsAverage);
    //    noclust_sum[i] = new TH2D(Form("noclust_sum_%d",i),Form("Summed Data Excluding Clusters Cam %d",i),764,0, 3056, 764, 0, 3056);
    add_counter[i] = new TH2D(Form("add_counter_%d",i), Form("Count of Bins Summed_%d",i), 764,0, 3056, 764, 0, 3056);
    
  }
  //  summed_data->SetBit(TH1::kIsAverage);
  SkimDataset d[datafiles.size()];
  Dataset * rd[datafiles.size()];
  //  Dataset 

  std::cout<<"datafiles.size(): "<<datafiles.size()<<std::endl;

  for(int i = 0; i < (int)datafiles.size(); i++){
    std::cout<<"file: "<<i<<std::endl;
    std::cout<<"filename: "<<datafiles.at(i)<<std::endl;

    TString rawname = datafiles.at(i);
    rawname.ReplaceAll("/production/skimout/skim/m3/skim/GainMap/skim/","/data/m3/GainMap/");
    rawname.ReplaceAll("skim.root",".root");

    d[i].openRootFile(datafiles.at(i));
    rd[i] = new Dataset;
    rd[i]->open(rawname);

    //for each dataset store the bins that are above threshold and exclude those bins from subsequent summation
    std::vector<int> thresh_bins;

    for(int ev = 0; ev < d[i].nevents(); ev++){
      d[i].getEvent(ev);
      rd[i]->getEvent(ev);


      
      if(ev%100==0) std::cout<<"event: "<<ev<<std::endl;
      for(int c = 3; c < 4; c++){
	bool trackcut = false;
	if(d[i].event()->ntracks(c) > 0){
	  for(int t = 0; t < d[i].event()->ntracks(c); t++){
	    if(d[i].event()->E(c,t) > 5000 && d[i].event()->range(c,t) > 20){
	      //loop over cluster bins and check for the high threshold values to exclude from subsequent summation
	      for(int bin = 0; bin < (int)d[i].event()->cluster(c)->getCluster(t)->pixels.size(); bin++){
		if(d[i].event()->image(c)->GetBinContent(d[i].event()->cluster(c)->getCluster(t)->pixels.at(bin)) > threshold){
		  thresh_bins.push_back(d[i].event()->cluster(c)->getCluster(t)->pixels.at(bin));
		}
	      }
	      trackcut = true;
	    }
	  }
	}
	  if(trackcut)
	    continue;
	//	const TH2F * biasframe = (TH2F*)rd[i].biasAvg(c)->Clone();
	//
	//double bias_mean = getMean(biasframe);
	double bias_mean = getMean(rd[i]->biasAvg(c));
	TH2F * tmpimg = const_cast<TH2F*>((TH2F*)d[i].event()->image(c)->Clone("tmpimg"));
	//subtractPedestal(tmpimg,bias_mean);
      //      if(bias)
	//MaxCamImageTools::subtractPedestal(tmpimg,biasvalue);
      //tmpimg->SetBit(TH1::kIsAverage);
	//do on the fly spark cut
	double img_mean = getMean(rd[i]->event()->ccdData(c));
	double mean_rat = img_mean/bias_mean;
	if(c == 3 && mean_rat > 1.0043){
	  sparks.add("m3",d[i].event()->runNumber(),d[i].event()->eventNumber(),c,0);
	  continue;
	}
	//loop over all image bin entries, check if the bin is in the threshold-excluded bins, if it not, then add to sum. store whether added or not for averaging - this is meant for rbi removal
	
	for(int xbin = 0; xbin < d[i].event()->image(c)->GetNbinsX(); xbin++){
	  for(int ybin = 0; ybin < d[i].event()->image(c)->GetNbinsY(); ybin++){
	    if(fill_sum(tmpimg->FindBin(xbin,ybin),thresh_bins)){
	      manual_summed_data[c]->SetBinContent(xbin,ybin,manual_summed_data[c]->GetBinContent(xbin,ybin)+d[i].event()->image(c)->GetBinContent(xbin,ybin));
	      add_counter[c]->SetBinContent(xbin,ybin,add_counter[c]->GetBinContent(xbin,ybin)+1);
	    }
	  }
	}
	//simple addition of event images, no 
	summed_data[c]->Add(d[i].event()->image(c));
	summed_avg_data[c]->Add(d[i].event()->image(c));
	//noclust_sum[i] = addNoClusters(noclust_sum[i],clust_counter[i],d[i].event(),c);
      }
    }
    d[i].closeRootFile();
    delete rd[i];
  }

  //----check for zero-valued bin entries in the add_counter histogram (since dividing by this histogram and dividing by zero will cause infs
  savefile.cd();
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

  //  d[i].close();
  savefile.Write();
  savefile.Close();
  
}
