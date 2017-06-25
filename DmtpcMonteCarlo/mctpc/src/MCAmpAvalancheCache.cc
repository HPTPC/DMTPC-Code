#include "TChain.h" 
#include "TMath.h"
#include <vector>
#include "TChainElement.h" 
#include "TH2.h"
#include "TFile.h"
#include "TVectorD.h" 
#include "TDirectory.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TRandom.h" 
#include "MCAmpAvalancheCache.hh"
#include "TMD5.h"
#include <stdint.h>

dmtpc::mc::mctpc::MCAmpAvalancheCache::MCAmpAvalancheCache(TChain *chain, const char * cut, Params &p, const char * cachedir)
{


  //Create cache identifier from cut + concatenated file list
  //Also build up md5sum, which will be used  as filename 
  
  TObjArray *fileElements=chain->GetListOfFiles();
  TIter next(fileElements);
  TChainElement *chEl=0;

  TString checkstr = TString::Format("%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d:", p.size,p.bin_size,p.max_t,p.tbin,p.top_mesh_pitch,p.top_mesh_offset_x, p.top_mesh_offset_y,p.ztop,p.zbot,p.nzbins,p.from_photon);

  checkstr+= cut; 

  while (( chEl=(TChainElement*)next() ))
  {
    checkstr+=":"; 
    checkstr+=chEl->GetTitle();
  }



  TMD5 md5maker; 
  md5maker.Update((uint8_t*)checkstr.Data(), checkstr.Length()); 
  md5maker.Final(); 
  TString md5 = md5maker.AsString(); 

  bool found = false; 
  int i =0; 
  printf("%d\n",i); 


  mesh_pitch = p.top_mesh_pitch; 
  f = 0; 
  h = 0; 
  t = 0; 
  hvis = 0; 


  gSystem->mkdir(cachedir,true); 


  while ((f = TFile::Open(TString::Format("%s/%s.%d.root", cachedir, md5.Data(), i))) )
  {


    if (f->Get("checkstr") && (((TObjString*) f->Get("checkstr"))->GetString() == checkstr)) 
    {
      found = true; 
      break; 

    }
    else
    {
      delete f; 
      i++; 
    }
  }


  if (!found)
  {
    std::vector<double> *x = 0; 
    std::vector<double> *y = 0; 
    std::vector<double> *zs = 0; 
    std::vector<double> *ts = 0; 
    std::vector<int>* visible = 0; 
    std::vector<int>* from_photon = 0; 




    int n = chain->Draw(p.from_photon ? "Entry$: ne" : "Entry$: ne - Sum$(from_photon)", cut,"goff"); 
    int ne; 

    std::vector<int> pass_cut; 
    pass_cut.reserve(n); 

    avg_gain = 0; 
    for (int j = 0; j <n; j++)
    {
       pass_cut.push_back(int(chain->GetV1()[j])); 
       avg_gain += chain->GetV2()[j] / n; 
    }



    chain->SetBranchStatus("*",0); 
    chain->SetBranchStatus("xi",1); 
    chain->SetBranchAddress("xi",&x); 
    chain->SetBranchStatus("yi",1); 
    chain->SetBranchAddress("yi",&y); 
    chain->SetBranchStatus("zi",1); 
    chain->SetBranchAddress("zi",&zs); 
    chain->SetBranchStatus("ti",1); 
    chain->SetBranchAddress("ti",&ts); 
    chain->SetBranchStatus("visible_i",1); 
    chain->SetBranchAddress("visible_i",&visible); 
    chain->SetBranchStatus("from_photon",1); 
    chain->SetBranchAddress("from_photon",&from_photon); 
    chain->SetBranchStatus("ne",1); 
    chain->SetBranchAddress("ne",&ne); 



    TString fstr = TString::Format("%s/%s.%d.root", cachedir,md5.Data(), i); 
    f = new TFile (fstr,"RECREATE");

    TObjString chk(checkstr); 
    chk.Write("checkstr"); 

    int nbins = int(2*p.size / p.bin_size); 
    cache_tree = new TTree("cache","Cache Tree"); 
    h = new TH2I("avalanche_hist_xy","Avalanche Histogram", nbins, -p.size,p.size, nbins, -p.size,p.size); 
    hvis = new TH2I("avalanche_hist_xy_vis","Visible Avalanche Histogram", nbins, -p.size,p.size, nbins, -p.size,p.size); 
    t = new TH1I("avalanche_hist_t","Avalanche Times", int(p.max_t / p.tbin), 0, p.max_t); 
    z = new TH1I("avalanche_hist_z","Avalanche Z", p.nzbins, p.zbot, p.ztop); 

    h_int = new std::vector<double>(nbins*nbins +2); 
    t_int = new std::vector<double>(int(p.max_t / p.tbin) +2); 
    z_int = new std::vector<double>(p.nzbins +2); 


    cache_tree->Branch("hvis",&hvis); 
    cache_tree->Branch("h",&h); 
    cache_tree->Branch("t",&t); 
    cache_tree->Branch("z",&z); 
    cache_tree->Branch("h_int",&h_int); 
    cache_tree->Branch("t_int",&t_int); 
    cache_tree->Branch("z_int",&z_int); 

    TVectorD avg(1); 
    avg[0] = avg_gain; 
    avg.Write("avg"); 


    double t0 = 0; 
    for (int j = 0; j < n; j++)
    {
      h->Reset(); 
      hvis->Reset(); 
      t->Reset(); 

      printf("%d\n",chain->GetEntry(pass_cut[j]));
      printf("processing %d:%lu %lu %d\n",j, x->size(), visible->size(),ne); 

      double tmin = DBL_MAX; 
      int min_index = -1; 

      for (unsigned jj = 0; jj < ts->size(); jj++)
      {
        if (ts->at(jj) > t0 && ts->at(jj) < tmin) 
        {
          tmin = ts->at(jj); 
          min_index = jj; 
        }
      }
      for (unsigned jj = 0; jj < ts->size(); jj++)
      {

        if (from_photon->at(jj) & !p.from_photon) continue; 
        if (ts->at(jj))
        {
          t->Fill(ts->at(jj) - tmin); 
        }

        z->Fill(zs->at(jj)); 
     
      }


      if (min_index >=0)
      {

        double x0 = x->at(min_index); 
        double y0 = y->at(min_index); 
        x0 -= p.top_mesh_offset_x; 
        y0 -= p.top_mesh_offset_y; 

        x0  -= int (x0 / p.top_mesh_pitch + 0.5) * p.top_mesh_pitch; 
        y0  -= int (y0 / p.top_mesh_pitch + 0.5) * p.top_mesh_pitch; 

        for (unsigned jj = 0; jj < x->size(); jj++)
        {
          if (ts->at(jj))
          {
            if (from_photon->at(jj) & !p.from_photon) continue; 
            h->Fill(x->at(jj) - x0,y->at(jj) - y0);
            if (jj < visible->size() && visible->at(jj))
            {
              hvis->Fill(x->at(jj) - x0,y->at(jj) - y0);
            }
          }
        }
      }

      if (h->GetEntries())
      {
        memcpy(&h_int->at(0), h->GetIntegral(), h_int->size() * sizeof(double)); 
        memcpy(&t_int->at(0), t->GetIntegral(), t_int->size() * sizeof(double)); 
        memcpy(&z_int->at(0), z->GetIntegral(), z_int->size() * sizeof(double)); 
      }
      else
      {
        memset(&h_int->at(0), 0, h_int->size() * sizeof(double)); 
        memset(&t_int->at(0), 0, t_int->size() * sizeof(double)); 
        memset(&z_int->at(0), 0, z_int->size() * sizeof(double)); 
      }

      cache_tree->Fill(); 


    }

    cache_tree->Write(); 

    f->Flush(); 
    f->Close(); 

    delete visible; 
    delete x; 
    delete y; 
    delete zs; 
    delete ts; 
    delete h_int; 
    delete t_int; 
    delete z_int; 

    f = TFile::Open(fstr); 
  }
  cache_tree = (TTree*)f->Get("cache"); 
  TVectorD * avg = (TVectorD*) f->Get("avg"); 
  avg_gain = (*avg)[0]; 

  cache_tree->SetBranchAddress("h",&h); 
  cache_tree->SetBranchAddress("hvis",&hvis); 
  cache_tree->SetBranchAddress("t",&t); 
  cache_tree->SetBranchAddress("z",&z); 

  h_int = 0; 
  t_int = 0; 
  z_int = 0; 
  cache_tree->SetBranchAddress("h_int",&h_int); 
  cache_tree->SetBranchAddress("t_int",&t_int); 
  cache_tree->SetBranchAddress("z_int",&z_int); 

  cache_tree->LoadBaskets(); 

}


static double xrand(const double *fIntegral, const TH1 * h)
{

  int nbinsx = h->GetNbinsX(); 

  Double_t integral = fIntegral[nbinsx];
  if (integral == 0) return 0;
  // return a NaN in case some bins have negative content
  if (integral == TMath::QuietNaN() ) return TMath::QuietNaN(); 

  Double_t r1 = gRandom->Rndm();
  Int_t ibin = TMath::BinarySearch(nbinsx,fIntegral,r1);
  Double_t x = h->GetBinLowEdge(ibin+1);
  if (r1 > fIntegral[ibin]) x +=
      h->GetBinWidth(ibin+1)*(r1-fIntegral[ibin])/(fIntegral[ibin+1] - fIntegral[ibin]);
   return x;
}

double dmtpc::mc::mctpc::MCAmpAvalancheCache::thist_rand() const
{
  return xrand(&t_int->at(0), t); 
}


double dmtpc::mc::mctpc::MCAmpAvalancheCache::zhist_rand() const
{
  return xrand(&z_int->at(0), z); 
}

void dmtpc::mc::mctpc::MCAmpAvalancheCache::xyhist_rand(double *xx, double *yy) const
{

  //copypaste from ROOT
  const double * fIntegral = &h_int->at(0); 

  // return 2 random numbers along axis x and y distributed according
  // the cellcontents of a 2-dim histogram
  // return a NaN if the histogram has a bin with negative content

   Int_t nbinsx = h->GetNbinsX();
   Int_t nbinsy = h->GetNbinsY();
   Int_t nbins  = nbinsx*nbinsy;
   Double_t integral = fIntegral[nbins];

   if (integral == 0 ) { if (xx) *xx = 0; if (yy) *yy = 0; return;}
   // case histogram has negative bins
   if (integral == TMath::QuietNaN() ) {if (xx) *xx = TMath::QuietNaN(); if (yy)  *yy = TMath::QuietNaN(); return;}

   Double_t r1 = gRandom->Rndm();
   Int_t ibin = TMath::BinarySearch(nbins,fIntegral,(Double_t) r1);
   Int_t biny = ibin/nbinsx;
   Int_t binx = ibin - nbinsx*biny;
   if (xx)
   {
     *xx = h->GetXaxis()->GetBinLowEdge(binx+1);
     if (r1 > fIntegral[ibin]) 
      {
      *xx += h->GetXaxis()->GetBinWidth(binx+1)*(r1-fIntegral[ibin])/(fIntegral[ibin+1] - fIntegral[ibin]);
      }
   }  
   if (yy)
   {
     *yy = h->GetYaxis()->GetBinLowEdge(biny+1) + h->GetYaxis()->GetBinWidth(biny+1)*gRandom->Rndm();
   }
}


int  dmtpc::mc::mctpc::MCAmpAvalancheCache::drawAvalanche(int i)
{
  int entry = i < 0 ? gRandom->Integer(n()) : i % n(); 
  cache_tree->GetEntry(entry); 
  return entry;

}
