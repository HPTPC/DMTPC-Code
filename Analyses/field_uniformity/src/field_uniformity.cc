#include "Dataset.hh"
#include "Event.hh"
#include "ImageCalculations.hh"
#include "ImageProcessing.hh"
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

int main(int argc, char** argv){

  //  TFile biasfile("output/darco_bias_10.root");

  //  double biasvalue = MaxCamImageTools::getMean(biasframe);

  bool bias = false;
  
  //std::cout<<"bias value: "<<biasvalue<<std::endl;

  ifstream infile;
  infile.open(argv[1]);
  TString tmp;
  vector<TString> datafiles;
  while(infile >> tmp){
    datafiles.push_back(tmp);
  }

  TFile savefile(argv[2],"recreate");
  const int ncam = 1;
  TH2F * summed_data[ncam];

  for(int i = 0; i < ncam; i++){
    summed_data[i] = new TH2F(Form("summed_image_%d",i),Form("Summed Source Data Cam %d",i),1019,0, 3056,1019,0,3056);
  }
  //  summed_data->SetBit(TH1::kIsAverage);
  Dataset d;

  std::cout<<"datafiles.size(): "<<datafiles.size()<<std::endl;

  for(int i = 0; i < datafiles.size(); i++){
    d.open(datafiles.at(i));
    std::cout<<"file: "<<i<<std::endl;
    for(int ev = 0; ev < d.getEventTree()->GetEntries(); ev++){
      d.getEvent(ev);
      if(ev%100==0) std::cout<<"event: "<<ev<<std::endl;
      for(int c = 0; c < ncam; c++){
	TH2F * biasframe = (TH2F*)d.biasAvg(c)->Clone("tmpbias");
	double bias_mean = getMean(biasframe);
	TH2F * tmpimg = (TH2F*)d.event()->ccdData(c)->Clone("tmpimg");
	subtractPedestal(tmpimg,bias_mean);
      //      if(bias)
	//MaxCamImageTools::subtractPedestal(tmpimg,biasvalue);
      //tmpimg->SetBit(TH1::kIsAverage);
	summed_data[c]->Add(tmpimg);
      }
    }
  }
  d.close();
  savefile.Write();
  savefile.Close();
  
}
