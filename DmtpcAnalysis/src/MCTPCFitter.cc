#include "SkimDataset.hh"
#include "MCTPCDataset.hh"
#include "ImagePalettes.hh"
#include "PrincipalComponentFinder.hh"
#include "MCTruthAlgo.hh"
#include "TrackFitter.hh"
#include "SRIMTable.hh"
#include "TFile.h" 
#include <iostream>


int main (int nargs, char ** args) 
{
  dmtpc::image::palette::setStandard1(); 

  if (nargs < 4) 
  {
    std::cout << "usage: MCTPCFitter mcfile skimfile outfile out_dir srimfile=SRIM_F_in_CF4_100Torr P=60 enable_second_gaus=0" << std::endl;; 
    return 1; 
  }


  dmtpc::analysis::SkimDataset d; 
  d.openRootFile(args[2]); 

  dmtpc::analysis::MCTPCDataset mcd(args[1]); 

  const char * outdir = args[4]; 
  const char * srim_file = nargs > 5 ? args[5] : "SRIM_F_in_CF4_100Torr"; 
  double P = nargs > 6 ? atof(args[6]) : 60; 
  dmtpc::physics::SRIMTable sr(P, 298, 5); 
  sr.setIonizationFractions(1,0);
  bool second_gaus = nargs > 7 ? atoi(args[7]):  0; 

  sr.readFile(srim_file,"../DmtpcPhysics/tables/"); 
  mcd.getEvent(0); 
  sr.setPressure(P); 

  TFile out(args[3],"RECREATE"); 

  TTree * tree = new TTree("fit","fit"); 

  tree->SetAutoSave(5*1024); 


  std::vector<dmtpc::analysis::TrackFitter*> fitters;

  std::vector<double> gains; 
  d.getEvent(0); 
  
  int ncam = d.event()->ncamera(); 

  for (int i = 0; i < ncam; i++)
  {
     TH2 * gainmap = d.getGainMap(i) ? d.getGainMap(i)->getGainMap() : 0; 
 //    if (d.getGainMap(i))
 //    {
//       d.getGainMap(i)->randomize();
 //    }

     gains.push_back(dmtpc::analysis::truthalgo::getGain(&mcd,i,"",d.getGainMap(i))); 
     double noise = (mcd.truth()->noise.size() && mcd.truth()->noise[i]) ? mcd.truth()->noise[i] : 10; 
     double conversion_gain = (mcd.truth()->conversion_gain.size() && mcd.truth()->conversion_gain[i]) ? mcd.truth()->conversion_gain[i] : 1.4; 
     std::cout << "adding fitter for " << d.event()->cameraSerialNumber(i) << std::endl; 

     //printf("gainmap: %x\n",gainmap); 
     fitters.push_back(new dmtpc::analysis::TrackFitter(&sr, 1./mcd.truth()->xscale[i], gains[i], gainmap,0.2, noise, conversion_gain)); 
     fitters[i]->setDraw(true); 
     fitters[i]->setVerbose(true); 
//   fitters[i]->setDoIntegral(true); 
     fitters[i]->setDoLikelihood(true); 
     fitters[i]->setMinimizationMaxCalls(10000); 
     fitters[i]->setUseGainMapError(true); 
     fitters[i]->enableSecondGaus(second_gaus);
  }


  const dmtpc::analysis::TrackFitter::Result* result = 0; 
  

  int nfound; 
  double phi2; 
  double recoRange; 
  double recoE; 
  double gain; 
  double mcEr; 
  double mcEi; 
  double mcPhi; 
  double mcTheta; 
  double mcParticlePhi; 
  double mcParticleTheta; 
  double mcSourcePhi; 
  double mcSourceTheta; 
  double mcPCAPhi;
  double mcPCATheta;
  double mcX; 
  double mcY; 
  double mcZ; 
  double mcRange; 
  double rot; 
  double nearest_spacer_distance;

  int track, run, event, cam, mcnadu; 
  int edge; 

  tree->Branch("result",&result); 
  tree->Branch("nfound",&nfound); 
  tree->Branch("mcX",&mcX); 
  tree->Branch("mcY",&mcY); 
  tree->Branch("mcZ",&mcZ); 
  tree->Branch("mcTheta",&mcTheta); 
  tree->Branch("mcPhi",&mcPhi); 
  tree->Branch("mcParticleTheta",&mcParticleTheta); 
  tree->Branch("mcParticlePhi",&mcParticlePhi); 
  tree->Branch("mcSourceTheta",&mcSourceTheta); 
  tree->Branch("mcSourcePhi",&mcSourcePhi); 
  tree->Branch("mcPCATheta",&mcPCATheta); 
  tree->Branch("mcPCAPhi",&mcPCAPhi); 
  tree->Branch("mcEr", &mcEr); 
  tree->Branch("mcEi", &mcEi); 
  tree->Branch("mcRange", &mcRange); 
  tree->Branch("mcnadu", &mcnadu); 
  tree->Branch("phi2",&phi2); 
  tree->Branch("recoRange",&recoRange); 
  tree->Branch("recoE",&recoE); 
  tree->Branch("track",&track); 
  tree->Branch("run",&run); 
  tree->Branch("event",&event); 
  tree->Branch("cam",&cam); 
  tree->Branch("edge",&edge); 
  tree->Branch("gain",&gain); 
  tree->Branch("rot",&rot); 
  tree->Branch("truth",mcd.truth()); 
  tree->Branch("nearest_spacer_distance",&nearest_spacer_distance); 

  dmtpc::math::PrincipalComponentFinder pca(3); 

  printf("nevents: %d\n", d.nevents()); 

  for (int i = 0; i < d.nevents(); i++)
  {

    mcd.getEvent(i); 
    d.getEvent(i); 
    
    mcEi = mcd.truth()->ionization; 
    if (mcEi<=0) continue; 

    //choose the camera with the maximum deposition
    cam = dmtpc::analysis::truthalgo::maincam(mcd.truth()); 
    printf("cam %d\n",cam); 
    if (cam < 0) continue; 
    gain = gains[cam]; 

    mcEr = mcd.truth()->recoilEnergy; 
    mcPhi= mcd.truth()->phi(); 
    mcParticlePhi = atan2(mcd.truth()->pdy, mcd.truth()->pdx); 
    mcSourcePhi = atan2(mcd.truth()->sdy, mcd.truth()->sdx); 
    mcParticleTheta = atan2(sqrt(pow(mcd.truth()->pdx,2) + pow(mcd.truth()->pdy,2)), mcd.truth()->pdz); 
    mcSourceTheta = atan2(sqrt(pow(mcd.truth()->sdx,2) + pow(mcd.truth()->sdy,2)), mcd.truth()->sdz); 
    mcTheta= mcd.truth()->theta(); 
    mcnadu = mcd.truth()->nadu[cam]; 
    rot = mcd.truth()->rotation.size() > 0 ? mcd.truth()->rotation[cam] : 0; 
    edge = dmtpc::analysis::truthalgo::edge(mcd.truth(), cam); 

    TVector3 pca_dir = dmtpc::analysis::truthalgo::pca_electrons(mcd.truth(), &pca)->asVec(); 
    mcPCAPhi = pca_dir.Phi(); 
    mcPCATheta = pca_dir.Theta(); 

    mcRange = dmtpc::analysis::truthalgo::max_displacement(mcd.truth()); 



    mcZ = mcd.truth()->z0; 
    mcX = mcd.truth()->x0cam(cam); 
    mcY = mcd.truth()->y0cam(cam); 
    double x,y; 


    event = d.event()->eventNumber();
    run = d.event()->runNumber();

    nfound = d.event()->ntracks(cam); 

    for (int t = 0; t < d.event()->ntracks(cam); t++)
    {
         recoE = d.event()->E(cam,t);      
         x = d.event()->x(cam,t);      
         y = d.event()->y(cam,t);      
         recoRange = d.event()->range(cam,t);      
         std::cout << "recoE: " << recoE << std::endl; 
         std::cout << "c,t: " << cam << " , " << t << std::endl;
         if (recoE == 0 )
         {
           std::cout << "huh?!? skipping track " << recoE << std::endl; 
           continue;
         }

         if (fabs(x-mcX) > 50 && fabs(y-mcY) >50)
         {
           std::cout <<"x: " << x << " mcX: " << mcX << " y: " << y << " mcY" << mcY <<std::endl; 
           std::cout << "huh?!? bad match " << std::endl; 
           continue;
         }

         phi2 = d.event()->phi(cam,t); 
         track = t; 

         nearest_spacer_distance = d.event()->nearestSpacerDistance(cam,track); 
         fitters[cam]->fit(d.event(),cam,t); 
         result = fitters[cam]->getResult(); 
         fitters[cam]->getCanvas()->SaveAs(TString::Format("%s/%05d.%04d.%d.%d.png",outdir,run,event,cam,track)); 
         fitters[cam]->getCanvas()->SaveAs(TString::Format("%s/%05d.%04d.%d.%d.pdf",outdir,run,event,cam,track)); 

         out.cd(); 
         tree->Fill(); 
         if (t && t % 100 == 0) tree->AutoSave(); 
    }
  }

 tree->Write(); 
 out.Close(); 

 return 0; 
}



