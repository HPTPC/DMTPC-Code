#include "TTree.h" 
#include "TChain.h" 
#include <assert.h>
#include "TH2.h"
#include "TFile.h" 
#include "TFile.h"
#include "TRandom3.h" 
#include "TMath.h" 
#include "../../../DmtpcAnalysis/include/MCTruth.hh" 
#include "../../../DmtpcAnalysis/include/MCTruthAlgo.hh" 
#include "../../../DmtpcMath/include/PrincipalComponentFinder.hh"
#include "../../../DmtpcMath/include/Angular.hh"
#include "../../../DmtpcImage/include/ImageTransforms.hh"

int directionality_analysis(TTree * c, const char * out, int min_event = 0, int max_event = -1)
{
  dmtpc::analysis::MCTruth * truth = 0; 
  c->SetBranchAddress("truth",&truth); 

  if (max_event < 0) max_event = c->GetEntries(); 


  TFile of(out,"RECREATE"); 
  TTree * tree= new TTree("pca","PCA"); 

  double E; 
  double ionization; 
  double z0; 
  double dz; 
  double opening_source_wimp; 
  double opening_wimp_recoil; 
  double opening_source_recoil; 
  double opening_recoil_electrons; 
  double opening_source_electrons;
  double opening_recoil_diffused_electrons; 
  double opening_source_diffused_electrons;
  double phi_recoil_avalanche; 
  double phi_source_avalanche; 
  double phi_recoil_truth; 
  double phi_source_truth; 
  double P; 

  double phi_recoil_electrons_flat; 
  double phi_source_electrons_flat;
  double phi_recoil_diffused_electrons_flat; 
  double phi_source_diffused_electrons_flat;
 

  const int npercentile = 2; 
  double percentile[] = {0, 0.05} ; 
  

  int asymmetry_electrons[npercentile]; 
  int asymmetry_electrons_flat[npercentile]; 
  int asymmetry_diffused_electrons[npercentile]; 
  int asymmetry_diffused_electrons_flat[npercentile]; 
  int asymmetry_avalanche[npercentile]; 
  int asymmetry_truth[npercentile]; 
  int asymmetry_electrons_init[npercentile]; 
  int asymmetry_diffused_electrons_init[npercentile]; 
  int asymmetry_electrons_init_flat[npercentile]; 
  int asymmetry_diffused_electrons_init_flat[npercentile]; 
  int asymmetry_avalanche_init[npercentile]; 
  int asymmetry_truth_init[npercentile]; 

  double asymmetry_electrons_line; 
  double asymmetry_diffused_electrons_line; 
  double asymmetry_electrons_line_flat; 
  double asymmetry_diffused_electrons_line_flat; 
  double asymmetry_avalanche_line; 
  double asymmetry_truth_line; 

  double asymmetry_electrons_line_init; 
  double asymmetry_diffused_electrons_line_init; 
  double asymmetry_electrons_line_init_flat; 
  double asymmetry_diffused_electrons_line_init_flat; 
  double asymmetry_avalanche_line_init; 
  double asymmetry_truth_line_init; 

  int edge , cam; 

  tree->Branch("E" , &E); 
  tree->Branch("edge" , &edge); 
  tree->Branch("cam" , &cam); 
  tree->Branch("ionization", &ionization); 
  tree->Branch("z0", &z0); 
  tree->Branch("dz", &dz); 
  tree->Branch("opening_source_wimp" ,  &opening_source_wimp); 
  tree->Branch("opening_wimp_recoil" ,  &opening_wimp_recoil); 
  tree->Branch("opening_source_recoil" ,  &opening_source_recoil); 
  tree->Branch("opening_recoil_electrons"   ,  &opening_recoil_electrons); 
  tree->Branch("opening_source_electrons",  &opening_source_electrons);
  tree->Branch("opening_recoil_diffused_electrons",  &opening_recoil_diffused_electrons); 
  tree->Branch("opening_source_diffused_electrons",  &opening_source_diffused_electrons);
  tree->Branch("phi_recoil_electrons_flat"   ,  &phi_recoil_electrons_flat); 
  tree->Branch("phi_source_electrons_flat",  &phi_source_electrons_flat);
  tree->Branch("phi_recoil_diffused_electrons_flat",  &phi_recoil_diffused_electrons_flat); 
  tree->Branch("phi_source_diffused_electrons_flat",  &phi_source_diffused_electrons_flat);
  tree->Branch("phi_recoil_avalanche" ,  &phi_recoil_avalanche); 
  tree->Branch("phi_source_avalanche" ,  &phi_source_avalanche); 
  tree->Branch("phi_recoil_truth" ,  &phi_recoil_truth); 
  tree->Branch("phi_source_truth",  &phi_source_truth); 

  for (int i = 0; i< npercentile; i++)
  {
    int pct = int(percentile[i] * 100); 
    tree->Branch(TString::Format("asymmetry_electrons_%d",pct ),  &asymmetry_electrons[i]); 
//    tree->Branch(TString::Format("asymmetry_diffused_electrons_%d",pct) ,  &asymmetry_diffused_electrons[i]); 
    tree->Branch(TString::Format("asymmetry_electrons_%d_flat" ,pct),  &asymmetry_electrons_flat[i]); 
    tree->Branch(TString::Format("asymmetry_diffused_electrons%d_flat",pct ),  &asymmetry_diffused_electrons_flat[i]); 
    tree->Branch(TString::Format("asymmetry_avalanche_%d",pct  ) ,  &asymmetry_avalanche[i]); 
    tree->Branch(TString::Format("asymmetry_truth_%d",pct) ,  &asymmetry_truth[i]); 
    tree->Branch(TString::Format("asymmetry_electrons_%d_init",pct),  &asymmetry_electrons_init[i]); 
//    tree->Branch(TString::Format("asymmetry_diffused_electrons_%d_init",pct) ,  &asymmetry_diffused_electrons_init[i]); 
//    tree->Branch(TString::Format("asymmetry_electrons_%d_init_flat",pct),  &asymmetry_electrons_init_flat[i]); 
//    tree->Branch(TString::Format("asymmetry_diffused_electrons_%d_init_flat",pct) ,  &asymmetry_diffused_electrons_init_flat[i]); 
//    tree->Branch(TString::Format("asymmetry_avalanche_%d_init",pct) ,  &asymmetry_avalanche_init[i]); 
 //   tree->Branch(TString::Format("asymmetry_truth_%d_init",pct ) ,  &asymmetry_truth_init[i]); 
  }

  tree->Branch("asymmetry_electrons_line",&asymmetry_electrons_line); 
  tree->Branch("asymmetry_electrons_line_flat",&asymmetry_electrons_line_flat); 
 // tree->Branch("asymmetry_diffused_electrons_line",&asymmetry_diffused_electrons_line); 
  tree->Branch("asymmetry_diffused_electrons_line_flat",&asymmetry_diffused_electrons_line_flat); 
  tree->Branch("asymmetry_avalanche_line",&asymmetry_avalanche_line); 
  tree->Branch("asymmetry_truth_line",&asymmetry_truth_line); 
  tree->Branch("asymmetry_electrons_line_init",&asymmetry_electrons_line_init); 
//  tree->Branch("asymmetry_diffused_electrons_line_init",&asymmetry_diffused_electrons_line_init); 
//  tree->Branch("asymmetry_electrons_line_init_flat",&asymmetry_electrons_line_init_flat); 
//  tree->Branch("asymmetry_diffused_electrons_line_init_flat",&asymmetry_diffused_electrons_line_init_flat); 
//  tree->Branch("asymmetry_avalanche_line_init",&asymmetry_avalanche_line_init); 
//  tree->Branch("asymmetry_truth_line_init",&asymmetry_truth_line_init); 

  dmtpc::math::PrincipalComponentFinder  pca3(3,true); 
  dmtpc::math::PrincipalComponentFinder  pca2(2,true); 


  for (int i = min_event; i < max_event; i++)
  {

    printf("processing entry %d\n", i);
    c->GetEntry(i); 
    E = truth->recoilEnergy; 
    ionization = truth->ionization; 

    //angles of recoil, wimp and source
    TVector3 recoil_dir(truth->dx, truth->dy, truth->dz); 
    TVector3 recoil_dir_flat(truth->dx, truth->dy,0); 
    recoil_dir_flat = recoil_dir_flat.Unit(); 
    TVector3 wimp_dir(truth->pdx, truth->pdy, truth->pdz); 
    TVector3 source_dir(truth->sdx, truth->sdy, truth->sdz); 

    TVector3 startpos(truth->x0, truth->y0, truth->z0); 
    TVector3 startpos_flat(truth->x0, truth->y0, 0); 
    z0 = truth->z0; 
    dz = truth->dz; 
//    source_dir.Print(); 
//    recoil_dir.Print(); 
//    wimp_dir.Print();
    double source_phi = source_dir.Phi(); 
    double recoil_phi = recoil_dir.Phi(); 
    double wimp_phi = wimp_dir.Phi(); 
    P = truth->pressure; 

    if (truth->nprimary < 3) continue; //can't do pca 
    cam = dmtpc::analysis::truthalgo::maincam(truth); 
    if (cam < 0) continue; 
    edge = truth->edge || dmtpc::analysis::truthalgo::edge(truth,cam); 

    //initial electron angle
    TVector3 electrons_dir = dmtpc::analysis::truthalgo::pca_electrons(truth, &pca3)->asVec(); 
    TVector3 electrons_mean(pca3.getMean(0), pca3.getMean(1), pca3.getMean(2)); 
    electrons_mean -= startpos; 
    bool electrons_sign = cos(electrons_mean.Angle(electrons_dir)) > 0; 
    if (!electrons_sign) electrons_dir *= -1; 

    //diffused electron angle
    TVector3 diffused_electrons_dir = dmtpc::analysis::truthalgo::pca_electrons(truth, &pca3,true)->asVec(); 
    TVector3 diffused_electrons_mean(pca3.getMean(0), pca3.getMean(1), pca3.getMean(2)); 
    diffused_electrons_mean -= startpos; 
    bool diffused_electrons_sign = cos(diffused_electrons_mean.Angle(electrons_dir)) > 0; 
    if (!diffused_electrons_sign) diffused_electrons_dir *= -1; 

    //initial electron angle flat
    TVector3 electrons_dir_flat = dmtpc::analysis::truthalgo::pca_electrons_flat(truth, &pca2)->asVec(); 
    TVector3 electrons_mean_flat(pca2.getMean(0), pca2.getMean(1),0); 
    electrons_mean_flat -= startpos_flat; 
    bool electrons_sign_flat = cos(electrons_mean_flat.Angle(electrons_dir_flat)) > 0; 
    if (!electrons_sign_flat) electrons_dir_flat *= -1; 

    //diffused electron angle flat
    TVector3 diffused_electrons_dir_flat = dmtpc::analysis::truthalgo::pca_electrons_flat(truth, &pca2,true)->asVec(); 
    TVector3 diffused_electrons_mean_flat(pca2.getMean(0), pca2.getMean(1), 0); 
    diffused_electrons_mean_flat -= startpos_flat; 
    bool diffused_electrons_sign_flat = cos(diffused_electrons_mean_flat.Angle(diffused_electrons_dir_flat)) > 0; 
    if (!diffused_electrons_sign_flat) diffused_electrons_dir_flat *= -1; 



    //avalanche angle
    double avalanche_xmean, avalanche_ymean;
    double avalanche_phi = dmtpc::analysis::truthalgo::pca_hist((TH2*)truth->avalanche_visible_photon_xy, &avalanche_xmean, &avalanche_ymean); 
    double avalanche_mean_phi = atan2(avalanche_ymean-truth->y0, avalanche_xmean- truth->x0); 
    bool avalanche_sign = cos(avalanche_phi - avalanche_mean_phi) > 0; 
    if (!avalanche_sign)
    {
      avalanche_phi = dmtpc::math::angle::normalize(avalanche_phi + TMath::Pi()); 
    }


    //truth angle
    double truth_xmean, truth_ymean; 
    TH2 * truth_image= (TH2*) dmtpc::image::transform::cropZero(truth->getTruthImage(cam)); 
    double truth_phi = dmtpc::analysis::truthalgo::pca_hist(truth_image,&truth_xmean, &truth_ymean); 
    double truth_mean_phi = atan2(truth_ymean - truth->y0cam(cam), truth_ymean-truth->y0cam(cam)); 

    bool truth_sign = cos(truth_phi - truth_mean_phi) > 0; 
    if (!truth_sign)
    {
      truth_phi = dmtpc::math::angle::normalize(truth_phi + TMath::Pi()); 
    }


    //compute opening angles and such 
    opening_source_wimp = source_dir.Angle(wimp_dir); 
    opening_wimp_recoil = wimp_dir.Angle(recoil_dir); 
    opening_source_recoil = source_dir.Angle(recoil_dir); 
    opening_recoil_electrons = recoil_dir.Angle(electrons_dir); 
    opening_source_electrons = source_dir.Angle(electrons_dir); 
    opening_recoil_diffused_electrons = recoil_dir.Angle(diffused_electrons_dir); 
    opening_source_diffused_electrons = source_dir.Angle(diffused_electrons_dir); 
    phi_recoil_avalanche = dmtpc::math::angle::normalize(recoil_dir.Phi() - avalanche_phi); 
    phi_source_avalanche = dmtpc::math::angle::normalize(source_dir.Phi() - avalanche_phi); 
    phi_recoil_truth = dmtpc::math::angle::normalize(recoil_dir.Phi() - truth_phi); 
    phi_source_truth = dmtpc::math::angle::normalize(source_dir.Phi() - truth_phi); 

    phi_recoil_electrons_flat = dmtpc::math::angle::normalize(electrons_dir_flat.Phi() - recoil_dir_flat.Phi()); 
    phi_source_electrons_flat = dmtpc::math::angle::normalize(electrons_dir_flat.Phi() - source_dir.Phi()); 
    phi_recoil_diffused_electrons_flat = dmtpc::math::angle::normalize(diffused_electrons_dir_flat.Phi() - recoil_dir_flat.Phi()); 
    phi_source_diffused_electrons_flat = dmtpc::math::angle::normalize(diffused_electrons_dir_flat.Phi() - source_dir.Phi()); 
    
    //TRandom3 rand(0); 

    //compute asymmetries
    asymmetry_electrons_line = dmtpc::analysis::truthalgo::electron_asymmetry(truth, &electrons_dir, false, false,npercentile, percentile, asymmetry_electrons);
//    asymmetry_diffused_electrons_line = dmtpc::analysis::truthalgo::electron_asymmetry(truth, &electrons_dir, true, npercentile, percentile, asymmetry_diffused_electrons);
    asymmetry_electrons_line_init = dmtpc::analysis::truthalgo::electron_asymmetry(truth, &recoil_dir, false, false,npercentile, percentile, asymmetry_electrons_init);
 //   asymmetry_diffused_electrons_line_init = dmtpc::analysis::truthalgo::electron_asymmetry(truth, &recoil_dir, true, npercentile, percentile, asymmetry_diffused_electrons_init);
    asymmetry_electrons_line_flat = dmtpc::analysis::truthalgo::electron_asymmetry(truth, &electrons_dir_flat, false, true,npercentile, percentile, asymmetry_electrons_flat);
//    asymmetry_electrons_line_init_flat = dmtpc::analysis::truthalgo::electron_asymmetry(truth, &recoil_dir_flat, false, npercentile, percentile, asymmetry_electrons_init_flat);
    asymmetry_diffused_electrons_line_flat = dmtpc::analysis::truthalgo::electron_asymmetry(truth, &diffused_electrons_dir_flat, true,true, npercentile, percentile, asymmetry_diffused_electrons_flat);
 //   asymmetry_diffused_electrons_line_init_flat = dmtpc::analysis::truthalgo::electron_asymmetry(truth, &recoil_dir_flat, true, npercentile, percentile, asymmetry_diffused_electrons_init_flat);



    asymmetry_avalanche_line = dmtpc::analysis::truthalgo::hist_asymmetry((TH2*) truth->avalanche_visible_photon_xy, avalanche_phi, npercentile, percentile, asymmetry_avalanche,0, 1e-2); 
 //   asymmetry_avalanche_line_init = dmtpc::analysis::truthalgo::hist_asymmetry((TH2*) truth->avalanche_visible_photon_xy, recoil_dir.Phi(), npercentile, percentile, asymmetry_avalanche_init,&rand); 
 

    asymmetry_truth_line = dmtpc::analysis::truthalgo::hist_asymmetry(truth_image, truth_phi,  npercentile, percentile, asymmetry_truth,0); 
    delete truth_image; 
//    asymmetry_truth_line_init = dmtpc::analysis::truthalgo::hist_asymmetry((TH2*) truth->getTruthImage(cam), recoil_dir.Phi(), npercentile, percentile, asymmetry_truth_init,&rand); 

    tree->Fill(); 
    if ( (i-min_event) && ( ((i-min_event) % 100) == 0)) tree->AutoSave("SaveSelf"); 
  }

  tree->Write(); 
}

int directionality_analysis(const char * file, const char * out, int min_event = 0, int max_event = -1)
{
  TFile f(file); 
  return directionality_analysis((TTree*) f.Get("Simulation"), out, min_event, max_event);
}


