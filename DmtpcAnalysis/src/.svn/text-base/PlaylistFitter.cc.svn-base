#include "Playlist.hh"
#include "SkimDataset.hh" 
#include "AnalysisConfig.hh"
#include "SRIMTable.hh"
#include "ImagePalettes.hh"
#include "TFile.h" 
#include "TrackFitter.hh"

int main (int nargs, char ** args) 
{
  dmtpc::image::palette::setStandard1(); 

  if (nargs < 4) 
  {
    std::cout << "usage: PlaylistFitter playlist outfile cfg out_dir=0 start=0 end=-1" << std::endl;; 
    return 1; 
  }



  dmtpc::analysis::Playlist p(args[1]); 


  const char * outdir = nargs > 4 ? args[4] : 0 ; 

  dmtpc::analysis::AnalysisConfig cfg(args[3]); 

  double P = cfg.getDouble("pressure"); 

  dmtpc::physics::SRIMTable sr(P, 298, 5); 

  sr.readFile(cfg.getString("srim_table"),"../DmtpcPhysics/tables/"); 
  sr.setPressure(P); 

  TFile out(args[2],"RECREATE"); 

  out.cd(); 
  TTree * tree = new TTree("fit","fit"); 
  tree->SetAutoSave(5*1024); 


  std::tr1::unordered_map<std::string, dmtpc::analysis::TrackFitter*> fitters;

  TFile * gmfile = 0; 

  bool use_gain_map = strlen(cfg.getString("gainmap_file")); 
  if (use_gain_map)
  {
    gmfile = new TFile(cfg.getString("gainmap_file")); 
  }




  
  double phi2; 
  double recoRange; 
  double recoE; 
  double gain; 
  double rot; 
  int track, run, event, cam; 
  double lengthcal; 
  const dmtpc::analysis::TrackFitter::Result* result = 0; 
  const dmtpc::analysis::SkimEvent* ev = 0; 

  tree->Branch("result",&result); 
  tree->Branch("phi2",&phi2); 
  tree->Branch("recoRange",&recoRange); 
  tree->Branch("recoE",&recoE); 
  tree->Branch("track",&track); 
  tree->Branch("run",&run); 
  tree->Branch("event",&event); 
  tree->Branch("skimEvent",&ev); 
  tree->Branch("cam",&cam); 
  tree->Branch("gain",&gain); 
  tree->Branch("lengthcal",&lengthcal); 
  tree->Branch("rot",&rot); 

  int start = nargs > 5 ? atoi(args[5]) : 0; 
  int end = nargs > 6 ? atoi(args[6]) : -1; 

  if (end < 0) end =p.n(); 
  for (int i = start; i < end; i++)
  {
    p.go(i); 

    cam = p.getCam(); 
    track = p.getTrack(); 
    ev=  p.getEvent(); 
    run = ev->runNumber(); 
    event = ev->eventNumber();
    std::string ser(ev->cameraSerialNumber(cam)); 

    gain = cfg.getDoubleMap("gain",ser.c_str()); 

    lengthcal = cfg.getDoubleMap("lengthcal", ser.c_str());
    rot = cfg.getDoubleMap("rotation", ser.c_str()); 
    recoRange = ev->range(cam,track); 
    recoE = ev->E(cam,track); 
    phi2 = ev->phi(cam,track); 

    if (!fitters.count(ser))
    {
      TH2 * gm_hist = 0;
      if (use_gain_map)
      {
        dmtpc::analysis::GainMap * gm = (dmtpc::analysis::GainMap*) gmfile->Get(ser.c_str()); 
        gm->randomize(); 
        gm_hist = gm->getGainMap(); 
      }

      printf("adding fitter with gain=%f\n, noise=%f, conversion_gain = %f",gain, cfg.getDoubleMap("noise",ser.c_str()), cfg.getDoubleMap("conversion_gain",ser.c_str())); 
      dmtpc::analysis::TrackFitter * fitter = new dmtpc::analysis::TrackFitter(&sr, lengthcal, gain, 
                                                                               gm_hist,0.2, cfg.getDoubleMap("noise",ser.c_str()),
                                                                               cfg.getDoubleMap("conversion_gain", ser.c_str())); 

      fitter->setDraw(outdir !=0); 
      fitter->setVerbose(cfg.getBool("verbose")); 
      fitter->setDoIntegral(cfg.getBool("fit_do_integral")); 
      fitter->setDoLikelihood(cfg.getBool("fit_do_likelihood")); 
      fitter->setMinimizationMaxCalls(cfg.getUInt("fit_minimization_max_calls")); 
      fitter->setUseGainMapError(cfg.getBool("fit_use_gain_map_error")); 
      fitter->enableCauchy(cfg.getBool("fit_enable_cauchy")); 
      fitter->enableSecondGaus(cfg.getBool("fit_enable_second_gaus")); 

      fitters[ser] = fitter; 
    }


    fitters[ser]->fit(ev,cam,track); 
    result = fitters[ser]->getResult(); 
    if (outdir)
    {
      fitters[ser]->getCanvas()->SaveAs(TString::Format("%s/%05d.%04d.%d.%d.png",outdir,run,event,cam,track)); 
    }

    
    out.cd(); 

    tree->Fill(); 
  }

  out.cd(); 
  tree->Write();

  out.Close(); 
  return 0; 







}


