//program to sum up images from source free dataset and produce pedestal for subtraction

#include "DmtpcDataset.hh"

#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

#include "TString.h"
#include "TFile.h"
#include "TH2.h"

using namespace std;

int main(int arc, char ** argv){


  ifstream infile;
  infile.open(argv[1]);
  vector<TString> datafiles;
  TString file;
  while(infile >> file){
    std::cout<<"file: "<<file<<std::endl;
    datafiles.push_back(file);
  }

  DmtpcDataset d;
  //setup
  d.openRootFile(datafiles.at(0));
  d.getEvent(0);

  int ncam = d.getNcameras();
  /*
  int nbinsx = d.event()->ccdData()->GetNbinsX();
  int nbinsy = d.event()->ccdData()->GetNbinsY();
  int xmin = 0;
  int ymin = 0;
  int xmax = d.event()->ccdData()->GetXaxis()->GetXmax();
  int ymax = d.event()->ccdData()->GetYaxis()->GetYmax();
  */

  TFile savefile(argv[2],"recreate");
  TH2F * img[ncam];
  TH1D * pixint[ncam];
  for(int i = 0; i < ncam; i++){
    //    img[i] = new TH2F(Form("bias_image_%d",i),Form("Image for Pedestal Value",i),nbinsx,xmin,xmax,nbinsy,ymin,ymax);
    img[i] = (TH2F*)d.event()->ccdData(i)->Clone(Form("bias_%d",i));
    //img[i]->SetBit(TH1::kIsAverage);//set to average histograms when added
    pixint[i] = new TH1D(Form("pixint_%d",i),Form("Pixel Intensity Distribution Cam %d",i),2000,0,2000);
  }

  int nimg = 1;

  for(int i = 0; i < datafiles.size(); i++){
    d.openRootFile(datafiles.at(i));
    for(int ev = 0; ev < d.tree()->GetEntries(); ev++){
      if(i == 0 && ev == 0)
	continue;//already got first image from cloning above
      d.getEvent(ev);
      for(int c = 0; c < ncam; c++){
	TH2F * temp = (TH2F*)d.event()->ccdData(c)->Clone("temp");
	//	temp->SetBit(TH1::kIsAverage);
	img[c]->Add(temp);
	nimg++;
	for(int xbin = 1; xbin <= temp->GetNbinsX(); xbin++){
	  for(int ybin = 1; ybin <= temp->GetNbinsY(); ybin++){
	    pixint[c]->Fill(temp->GetBinContent(xbin,ybin));
	  }
	}
      }
    }
    d.closeRootFile();
  }
  for(int i = 0; i < ncam; i++){
    for(int xbin = 1; xbin <= img[i]->GetNbinsX(); xbin++){
      for(int ybin = 1; ybin <= img[i]->GetNbinsY(); ybin++){
	img[i]->SetBinContent(xbin,ybin,img[i]->GetBinContent(xbin,ybin)/nimg);

      }
    }
  }
  savefile.Write();
  savefile.Close();
}

