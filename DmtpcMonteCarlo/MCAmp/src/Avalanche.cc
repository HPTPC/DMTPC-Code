#include "Avalanche.hh" 
#include "TTree.h"
#include <iostream>
#include "GarfieldConstants.hh"
#include "TFile.h"
#define MAX_ATTEMPTS 500 

/* must be declared here to use function pointer */
//todo modify garfield++ to take a void * to prevent this problem 
static std::vector<double> exc_x; 
static std::vector<double> exc_y; 
static std::vector<double> exc_z; 
static std::vector<double> exc_t; 



void clearExcitations() 
{
  exc_x.clear(); 
  exc_y.clear(); 
  exc_z.clear(); 
  exc_t.clear(); 
}


void recordExcitation(double x,double y, double z, double t, int type, int level, Garfield::Medium * med)
{
  if (type==Garfield::ElectronCollisionTypeExcitation)
  {
    exc_x.push_back(x); 
    exc_y.push_back(y); 
    exc_z.push_back(z); 
    exc_t.push_back(t); 
  }
}

bool ionRangeFn(double x, double y, double z, double t, const Garfield::Medium * medium, void * aux)
{
  dmtpc::mc::amp::Avalanche * av = (dmtpc::mc::amp::Avalanche*) aux; 
  return av->inZRange(z); 

}

dmtpc::mc::amp::Avalanche::Avalanche(Solver * s, const Model * m, const char * unit)
:  solver(s), model(m) 
{
  


  enable_photons = false; 
  enable_fakevuv = false; 
  vuv_p = 1e-3; 
  vuv_u = 20; 
  vuv_sigma=5; 
  do_plot = false; 
  ionization_model = 0; 
//  vd = 0; 
  bounds_set = false; 
  aval = new Garfield::AvalancheMicroscopic; 
  if (m->ionDriftEnabled()) ion_aval = new Garfield::AvalancheMC; 
  sensor = new Garfield::Sensor; 
  sensor->AddComponent(solver->fieldMap()); 

  record_excitations = false; 
  max_avalanche_size = getenv("MCAMP_MAX_AVALANCHE_SIZE") ? atoi(getenv("MCAMP_MAX_AVALANCHE_SIZE")) : 3e6; 
  for (unsigned i = 0; i < model->nElements(); i++)
  {
    if (model->getSurfaceProperties()->at(i) && model->getSurfaceProperties()->at(i)->isElectrode)
    {
      sensor->AddElectrode(solver->fieldMap(), model->getNames()->at(i));
    }
  }

  sensor->SetTimeWindow(0, 0.5, 1000); 
  aval->SetSensor(sensor); 
  if (m->ionDriftEnabled()) ion_aval->SetSensor(sensor); 
  if (m->electrodesEnabled())
  {
    aval->EnableSignalCalculation(); 
    if (m->ionDriftEnabled()) ion_aval->EnableSignalCalculation(); 
  }
  if (m->ionDriftEnabled()) ion_aval->SetTimeSteps(); 
//  ion_aval->SetAreaBounds(&ionRangeFn, (void*)this); 
  if (m->ionDriftEnabled()) ion_aval->SetTimeWindow(0,1000); 
  mode = SINGLE_ELECTRON; 
  part_dir[0] = 0; 
  part_dir[1] = 0; 
  part_dir[2] = 0; 

  track = 0; 
  factor = !strcasecmp(unit,"cm") ? 1. : 
           !strcasecmp(unit,"mm") ? 0.1 : 
           !strcasecmp(unit,"m") ? 100. : 
           !strcasecmp(unit,"um") ? 1e-4 : 
           0; 

  if (!factor)
  {
    std::cerr << " Unsupported unit: " << unit << std::endl; 
  }


}

dmtpc::mc::amp::Avalanche::~Avalanche()
{
  delete aval; 
  if (model->ionDriftEnabled())
    delete ion_aval; 
  delete sensor; 
}

void dmtpc::mc::amp::Avalanche::setBounds(double xlow,double xhigh, double ylow,double yhigh,double zlow,double zhigh, bool reflect) 
{
  bounds_set = true; 
  xl = xlow; 
  xu = xhigh; 
  yl = ylow;
  yu = yhigh; 
  zl = zlow;
  zu = zhigh; 
  reflect_z = reflect; 
}



int dmtpc::mc::amp::Avalanche::run(unsigned n, unsigned id)
{


  if (record_excitations || enable_fakevuv)
  {
    exc_x.reserve(5000); 
    exc_y.reserve(5000); 
    exc_z.reserve(5000); 
    exc_t.reserve(5000); 
    aval->SetUserHandleInelastic(&recordExcitation); 
  }
  else 
  {
    aval->UnsetUserHandleInelastic(); 
  }



  aval->EnableAvalancheSizeLimit(max_avalanche_size); 
  if (enable_photons)
  {
    aval->EnablePhotonTransport();
  }
  else
  {
    aval->DisablePhotonTransport();
  }


  if (!bounds_set) 
  {

    std::cerr << "Bounds not set! " << std::endl; 
    return 1; 
  }
  TString param_string; 
  TFile fout(TString::Format("%s/aval/%s.%d.root",model->getOutputDir(), model->getFullName(),id), "RECREATE"); 

  TTree * tree = new TTree("avalanche","Avalanche"); 

  double signal_start,signal_bin_width; 
  int signal_n; 
  sensor->GetTimeWindow(signal_start,signal_bin_width,signal_n); 
  TTree * signal_tree = new TTree("signal","Signals"); 

  std::vector<TH1*> signals; 
  std::vector<TH1*> signals_i; 
  std::vector<TH1*> signals_e; 
  std::vector<TH1*> amp_signals; 
  std::vector<TH1*> amp_signals_i; 
  std::vector<TH1*> amp_signals_e; 
  std::vector<const Amplifier * > amps; 
  std::vector<const char *> signal_names; 

  TH1 * electron_energy_histo = new TH1I("electron_energies", "Electron Energies", 300,0,300); 
  aval->EnableElectronEnergyHistogramming(electron_energy_histo);

  if (model->nElectrodes()) 
  {
    for (unsigned i = 0; i < model->nElements(); i++)
    {
      if (model->getSurfaceProperties()->at(i) && model->getSurfaceProperties()->at(i)->isElectrode)
      {
        const char * name = model->getNames()->at(i);
        printf("making signals for %s\n", name); 

        TH1 * signal = new TH1F(TString::Format("%s_raw", name),
                                 TString::Format("Raw Signal for %s",name),
                                 signal_n, signal_start, signal_start + signal_bin_width * signal_n); 

        TH1 * signal_e = new TH1F(TString::Format("%s_raw_e", name),
                                 TString::Format("Raw Electron Signal for %s",name),
                                 signal_n, signal_start, signal_start + signal_bin_width * signal_n); 

        TH1 * signal_i = new TH1F(TString::Format("%s_raw_i", name),
                                 TString::Format("Raw Ion Signal for %s",name),
                                 signal_n, signal_start, signal_start + signal_bin_width * signal_n); 

        signals.push_back(signal); 
        signals_i.push_back(signal_i); 
        signals_e.push_back(signal_e); 
        signal_names.push_back(name); 

        int idx = signals.size()-1; 

        signal_tree->Branch(signal->GetName(),(signals[idx])); 
        signal_tree->Branch(signal_e->GetName(),(signals_e[idx])); 
        signal_tree->Branch(signal_i->GetName(), (signals_i[idx])); 


        if (model->getSurfaceProperties()->at(i)->amp) 
        {
          TH1 * amp_signal = new TH1F(TString::Format("%s_signal", name),
                                   TString::Format("Signal for %s",name),
                                   signal_n, signal_start, signal_start + signal_bin_width * signal_n); 

          TH1 * amp_signal_e = new TH1F(TString::Format("%s_signal_e", name),
                                   TString::Format("Electron Signal for %s",name),
                                   signal_n, signal_start, signal_start + signal_bin_width * signal_n); 

          TH1 * amp_signal_i = new TH1F(TString::Format("%s_signal_i", name),
                                   TString::Format("Ion Signal for %s",name),
                                   signal_n, signal_start, signal_start + signal_bin_width * signal_n); 

          amp_signals.push_back( amp_signal ) ; 
          amp_signals_i.push_back( amp_signal_i ) ; 
          amp_signals_e.push_back( amp_signal_e ) ; 

          signal_tree->Branch(amp_signal->GetName(),(amp_signals[idx])); 
          signal_tree->Branch(amp_signal_e->GetName(),(amp_signals_e[idx])); 
          signal_tree->Branch(amp_signal_i->GetName(),(amp_signals_i[idx])); 

          amps.push_back( model->getSurfaceProperties()->at(i)->amp ) ; 
        }
        else 
        {
          amp_signals.push_back(0) ; 
          amp_signals_i.push_back(0) ; 
          amp_signals_e.push_back(0) ; 
          amps.push_back(0) ; 
        }
      }
    }
  }


  int ne,ni; 
  double x0,y0,z0; 
  double track_x0,track_y0,track_z0, track_t,track_E,track_dx,track_dy,track_dz; 
  int nav;

  std::vector<double> xi; 
  std::vector<double> yi; 
  std::vector<double> zi; 
  std::vector<double> ti; 
  std::vector<int> visible_i; 
  std::vector<int> from_photon; 
  std::vector<double> xf; 
  std::vector<double> yf; 
  std::vector<double> zf; 
  std::vector<double> tf; 
  std::vector<int> status; 

  std::vector<double> ion_xi; 
  std::vector<double> ion_yi; 
  std::vector<double> ion_zi; 
  std::vector<double> ion_ti; 
  std::vector<double> ion_xf; 
  std::vector<double> ion_yf; 
  std::vector<double> ion_zf; 
  std::vector<double> ion_tf; 
  std::vector<int> ion_status; 


  int nexc; 
  tree->Branch("x0",&x0); 
  tree->Branch("y0",&y0); 
  tree->Branch("z0",&z0); 
  tree->Branch("xf",&xf); 
  tree->Branch("yf",&yf); 
  tree->Branch("zf",&zf); 
  tree->Branch("xi",&xi); 
  tree->Branch("yi",&yi); 
  tree->Branch("zi",&zi); 
  tree->Branch("visible_i",&visible_i); 
  tree->Branch("from_photon",&from_photon); 
  tree->Branch("ti",&ti); 
  tree->Branch("tf",&tf); 
  tree->Branch("energyHisto",electron_energy_histo); 

  if (mode!=SINGLE_ELECTRON)
  {
    tree->Branch("track_x0"); 
    tree->Branch("track_y0"); 
    tree->Branch("track_z0"); 
    tree->Branch("track_dx"); 
    tree->Branch("track_dy"); 
    tree->Branch("track_dz"); 
    tree->Branch("track_t"); 
    tree->Branch("track_E"); 
  }


  std::vector<double> photon_xi; 
  std::vector<double> photon_yi; 
  std::vector<double> photon_zi; 
  std::vector<double> photon_ti; 
  std::vector<double> photon_xf; 
  std::vector<double> photon_yf; 
  std::vector<double> photon_zf; 
  std::vector<double> photon_tf; 
  std::vector<double> photon_e; 
  std::vector<int> photon_status; 
  int np; 

  std::vector<double> photon_induced_xi; 
  std::vector<double> photon_induced_yi; 
  std::vector<double> photon_induced_zi; 
  std::vector<double> photon_induced_x0; 
  std::vector<double> photon_induced_y0; 
  std::vector<double> photon_induced_z0; 
  std::vector<double> photon_induced_ti; 
  std::vector<double> photon_induced_xf; 
  std::vector<double> photon_induced_yf; 
  std::vector<double> photon_induced_zf; 
  std::vector<double> photon_induced_tf; 
  std::vector<double> photon_induced_t0; 
  std::vector<int> photon_induced_status; 
  std::vector<int> photon_induced_visible_i; 
  int nphoton_induced = 0; 
  int nphoton_induced_0 = 0; 




  if (enable_photons || enable_fakevuv)
  {
    tree->Branch("photon_xf",&photon_xf); 
    tree->Branch("photon_yf",&photon_yf); 
    tree->Branch("photon_zf",&photon_zf); 
    tree->Branch("photon_xi",&photon_xi); 
    tree->Branch("photon_yi",&photon_yi); 
    tree->Branch("photon_zi",&photon_zi); 
    tree->Branch("photon_ti",&photon_ti); 
    tree->Branch("photon_tf",&photon_tf); 
    tree->Branch("photon_e",&photon_e); 
    tree->Branch("photon_status",&photon_status); 
    tree->Branch("np",&np); 
    if (enable_fakevuv)
    {
      tree->Branch("photon_induced_xf",&photon_induced_x0); 
      tree->Branch("photon_induced_yf",&photon_induced_y0); 
      tree->Branch("photon_induced_zf",&photon_induced_z0); 
      tree->Branch("photon_induced_xf",&photon_induced_xf); 
      tree->Branch("photon_induced_yf",&photon_induced_yf); 
      tree->Branch("photon_induced_zf",&photon_induced_zf); 
      tree->Branch("photon_induced_xi",&photon_induced_xi); 
      tree->Branch("photon_induced_yi",&photon_induced_yi); 
      tree->Branch("photon_induced_zi",&photon_induced_zi); 
      tree->Branch("photon_induced_ti",&photon_induced_ti); 
      tree->Branch("photon_induced_tf",&photon_induced_tf); 
      tree->Branch("photon_induced_t0",&photon_induced_t0); 
      tree->Branch("photon_induced_status",&photon_induced_status); 
      tree->Branch("photon_induced_visible_i",&photon_induced_visible_i); 
      tree->Branch("np_induced",&nphoton_induced); 
      tree->Branch("np_induced0",&nphoton_induced_0); 
    }

  }
 

  tree->Branch("ion_xf",&ion_xf); 
  tree->Branch("ion_yf",&ion_yf); 
  tree->Branch("ion_zf",&ion_zf); 
  tree->Branch("ion_xi",&ion_xi); 
  tree->Branch("ion_yi",&ion_yi); 
  tree->Branch("ion_zi",&ion_zi); 
  tree->Branch("ion_ti",&ion_ti); 
  tree->Branch("ion_tf",&ion_tf); 
  tree->Branch("ion_status",&ion_status); 
 
  int nexc_vis; 
  std::vector<int> exc_vis; 

  int nvisible_i; 
  tree->Branch("ne",&ne); 
  tree->Branch("ni",&ni); 
  tree->Branch("nexc",&nexc); 
  tree->Branch("exc_x",&exc_x); 
  tree->Branch("exc_y",&exc_y); 
  tree->Branch("exc_z",&exc_z); 
  tree->Branch("nexc_vis",&nexc_vis); 
  tree->Branch("nvisible_i",&nvisible_i); 
  tree->Branch("exc_vis",&exc_vis); 
  /*
  tree->Branch("np",&np); 
  */
  tree->Branch("nav",&nav); 
  for (unsigned i = 0; i < model->getUserParams()->size(); i++)
  {
    model->getUserParams()->at(i)->branchMe(tree);
  }

  /*  // This doesn't seem to work... 
  if (do_plot && !vd) 
  {
     vd = new Garfield::ViewDrift; 
     aval->EnablePlotting(vd); 
     ion_aval->EnablePlotting(vd); 
  }
  else if (!do_plot && vd)
  {
    delete vd; 
    vd = 0; 
    aval->DisablePlotting();
    ion_aval->DisablePlotting(); 
  }

  tree->Branch("view",&vd); 
  */ 

  if (mode == RETRIM) 
    part_e = track_maker->E(); 

  for (int i = 0; i < n; i++)
  {
     printf("Avalanche Number: %d\n",i); 


     if (xl == xu) 
       x0 = xl; 
     else
       x0 = rand.Uniform(xl,xu);
     if (yl==yu) 
       y0 = yl; 
     else
       y0 = rand.Uniform(yl,yu);
     if (zl==zu)
       z0 = zl; 
     else
       z0 = rand.Uniform(zl,zu);


     if (reflect_z && i % 2) z0=-z0; 

     nav = i; 
     track_x0 = x0; 
     track_y0 = y0; 
     track_z0 = z0; 
     track_t = nav;
     track_E = part_e; 

     track_dx = part_dir[0]; 
     track_dy = part_dir[1]; 
     track_dz = part_dir[2]; 
     
     int ei=0, ninc=0; 
     sensor->ClearSignal(); 

     if (mode == PHOTON) 
     {
       if (!track) 
       {

         track = new Garfield::TrackHeed; 
         track->SetSensor(sensor); 
       }


       int nattempts = 0; 
       while(!ninc) 
       {
         if (nattempts++ > MAX_ATTEMPTS) 
         {
           fprintf(stderr," WARNING, did not create a photon track with collisions after %d attempts!\n", MAX_ATTEMPTS); 
           break; 

         }
          track->TransportPhoton(x0*factor,y0*factor,z0*factor,track_t,part_e,part_dir[0],part_dir[1],part_dir[2],ninc); 
          if (!ninc) 
          {
           x0 = rand.Uniform(xl,xu);
           y0 = rand.Uniform(yl,yu);
           z0 = rand.Uniform(zl,zu);
          }

           track_x0 = x0; 
           track_y0 = y0; 
           track_z0 = z0; 
       }

       printf("Photon created %d electrons\n", ninc); 

     }
     
     if (mode == RETRIM)
     {
       if (!track_maker) 
       {
         fprintf(stderr,"ERROR: NO TRACK MAKER!!!\n"); 
         return -1; 
       }
       if (!ionization_model) 
       {
         ionization_model = new dmtpc::mc::retrim::SimpleIonizationModel; 
       }

       TVector3 origin(x0,y0,z0); 
       TVector3 dir(part_dir); 
       track_maker->makeTrack(nav % track_maker->ntracks(), &origin, &dir, track_t);  
       ninc = track_maker->makeElectrons(ionization_model); 
     }

     double xcls,ycls,zcls,tcls,ecls,unused; 


     while(true) 
     {
       double t0,e0; 
       clearExcitations(); 

       if (mode == PHOTON || mode == TRACK || mode == RETRIM )
       {
         if (ei == ninc) 
         {
           if (mode == TRACK && track->GetCluster(xcls,ycls,zcls,tcls,ninc, ecls,unused))  
           {
             ei = 0; 
             printf("%f,%f,%f\n", xcls,ycls,zcls); 
           }
           else
           {
             break;
           }
         }
         double xe,ye,ze,te,ee, edx,edy,edz; 

         if (mode == PHOTON)
         {
           ((Garfield::TrackHeed*)track)->GetElectron(ei,xe,ye,ze,te,ee,edx,edy,edz); 
         }
         if (mode == RETRIM) 
         {
             track_maker->getElectron(ei,xe,ye,ze,te,ee,edx,edy,edz); 
         }

         x0 = xe; 
         y0 = ye; 
         z0 = ze; 

         aval->AvalancheElectron(xe*factor,ye*factor,ze*factor,te,ee,edx,edy,edz);
         printf("Drifting electron %d \n",ei); 
         ei++; 
       }
       else
       {
         aval->AvalancheElectron(x0*factor,y0*factor,z0*factor,0,.1,0,0,0); 
       }

       aval->GetAvalancheSize(ne,ni);
       xi.clear(); xi.reserve(ne); 
       xf.clear(); xf.reserve(ne); 
       yi.clear(); yi.reserve(ne); 
       yf.clear(); yf.reserve(ne); 
       zi.clear(); zi.reserve(ne); 
       zf.clear(); zf.reserve(ne); 
       ti.clear(); ti.reserve(ne); 
       tf.clear(); tf.reserve(ne); 
       status.clear(); status.reserve(ne); 
       from_photon.clear(); from_photon.reserve(ne);
       visible_i.clear(); visible_i.reserve(ne);

       
       if (model->ionDriftEnabled())
       {
         ion_xi.clear(); ion_xi.reserve(ne); 
         ion_xf.clear(); ion_xf.reserve(ne); 
         ion_yi.clear(); ion_yi.reserve(ne); 
         ion_yf.clear(); ion_yf.reserve(ne); 
         ion_zi.clear(); ion_zi.reserve(ne); 
         ion_zf.clear(); ion_zf.reserve(ne); 
         ion_ti.clear(); ion_ti.reserve(ne); 
         ion_tf.clear(); ion_tf.reserve(ne); 
         ion_status.clear(); ion_status.reserve(ne); 
       }


       if (enable_photons)
       {
         np = aval->GetNumberOfPhotons(); 

         photon_xi.clear(); photon_xi.reserve(np); 
         photon_xf.clear(); photon_xf.reserve(np); 
         photon_yi.clear(); photon_yi.reserve(np); 
         photon_yf.clear(); photon_yf.reserve(np); 
         photon_zi.clear(); photon_zi.reserve(np); 
         photon_zf.clear(); photon_zf.reserve(np); 
         photon_tf.clear(); photon_tf.reserve(np); 
         photon_ti.clear(); photon_ti.reserve(np); 
         photon_status.clear(); photon_status.reserve(np); 

         for (int j = 0; j < np; j++)
         {
           int status0; 
           double xi0,yi0,zi0,xf0,yf0,zf0, t0,t1,e; 
           aval->GetPhoton(j,e,xi0,yi0,zi0,t0,xf0,yf0,zf0,t1,status0); 
           photon_xi.push_back(xi0/factor); 
           photon_xf.push_back(xf0/factor); 
           photon_yi.push_back(yi0/factor); 
           photon_yf.push_back(yf0/factor); 
           photon_zi.push_back(zi0/factor); 
           photon_zf.push_back(zf0/factor); 
           photon_ti.push_back(t0); 
           photon_tf.push_back(t1); 
           photon_status.push_back(status0); 
           photon_e.push_back(e); 
         }


       }

       for (int j = 0; j < aval->GetNumberOfElectronEndpoints(); j++)
       {

         int status0; 
         double xi0,yi0,zi0,xf0,yf0,zf0, t0,t1, e0,e1; 
         aval->GetElectronEndpoint(j,xi0,yi0,zi0,t0,e0, xf0,yf0,zf0,t1,e1,status0); 


         xi.push_back(xi0/factor); 
         xf.push_back(xf0/factor); 
         yi.push_back(yi0/factor); 
         yf.push_back(yf0/factor); 
         zi.push_back(zi0/factor); 
         zf.push_back(zf0/factor); 
         ti.push_back(t0); 
         tf.push_back(t1); 
         status.push_back(status0); 
         from_photon.push_back(int(aval->electronEndPointFromPhoton(j))); 

         double ion_x0=0, ion_y0=0, ion_z0=0, ion_t0=0, ion_x1=0, ion_y1=0, ion_z1=0, ion_t1=0;
         if (model->ionDriftEnabled())
         {
           ion_aval->DriftIon(xi0,yi0,zi0,t0); 
           int ion_status0=0; 
           ion_aval->GetIonEndpoint(0,ion_x0, ion_y0, ion_z0, ion_t0, ion_x1, ion_y1, ion_z1, ion_t1, ion_status0 ); 

           ion_xi.push_back(ion_x0/factor); 
           ion_xf.push_back(ion_x1/factor); 
           ion_yi.push_back(ion_y0/factor); 
           ion_yf.push_back(ion_y1/factor); 
           ion_zi.push_back(ion_z0/factor); 
           ion_zf.push_back(ion_z1/factor); 
           ion_ti.push_back(ion_t0); 
           ion_tf.push_back(ion_t1); 
           ion_status.push_back(ion_status0); 
   
         }
       }

       nexc = exc_x.size(); 

       //handle fake vuv
       if (enable_fakevuv)
       {

         printf("handling fake vuv...\n"); 
         if (!track) 
         {

           track = new Garfield::TrackHeed; 
           track->SetSensor(sensor); 
         }

         photon_xi.clear(); photon_xi.reserve(nexc*0.1); 
         photon_yi.clear(); photon_yi.reserve(nexc*0.1); 
         photon_zi.clear(); photon_zi.reserve(nexc*0.1); 
         photon_ti.clear(); photon_ti.reserve(nexc*0.1); 
         photon_status.clear(); photon_status.reserve(nexc*0.1); 

         photon_induced_x0.clear(); photon_induced_x0.reserve(nexc*0.1); 
         photon_induced_y0.clear(); photon_induced_y0.reserve(nexc*0.1); 
         photon_induced_z0.clear(); photon_induced_z0.reserve(nexc*0.1); 
         photon_induced_t0.clear(); photon_induced_t0.reserve(nexc*0.1); 
         photon_induced_xi.clear(); photon_induced_xi.reserve(nexc*0.1); 
         photon_induced_xf.clear(); photon_induced_xf.reserve(nexc*0.1); 
         photon_induced_yi.clear(); photon_induced_yi.reserve(nexc*0.1); 
         photon_induced_xi.clear(); photon_induced_xi.reserve(nexc*0.1); 
         photon_induced_xf.clear(); photon_induced_xf.reserve(nexc*0.1); 
         photon_induced_yi.clear(); photon_induced_yi.reserve(nexc*0.1); 
         photon_induced_yf.clear(); photon_induced_yf.reserve(nexc*0.1); 
         photon_induced_zi.clear(); photon_induced_zi.reserve(nexc*0.1); 
         photon_induced_zf.clear(); photon_induced_zf.reserve(nexc*0.1); 
         photon_induced_tf.clear(); photon_induced_tf.reserve(nexc*0.1); 
         photon_induced_ti.clear(); photon_induced_ti.reserve(nexc*0.1); 
         photon_induced_status.clear(); photon_induced_status.reserve(nexc*0.1); 
         photon_induced_visible_i.clear(); photon_induced_visible_i.reserve(nexc*0.1); 




         for (unsigned j = 0; j < exc_x.size(); j++)
         {

           if (rand.Rndm() < vuv_p) 
           {
             int nmade; 
             photon_xi.push_back(exc_x[j]/factor); 
             photon_yi.push_back(exc_y[j]/factor); 
             photon_zi.push_back(exc_z[j]/factor); 
             photon_ti.push_back(exc_t[j]/factor); 
             track->TransportPhoton(exc_x[j], exc_y[j], exc_z[j], exc_t[j],
               rand.Gaus(vuv_u,vuv_sigma), 
               0,0,0, nmade); 

             np++; 

             printf("exc %d/%d: Photon made %d electrons\n",j, exc_x.size(),  nmade); 
             nphoton_induced_0 += nmade; 
             double xe,ye,ze,te,ee,edx,edy,edz; 
             for (int jj = 0; jj < nmade; jj++)
             {
               track->GetElectron(jj, xe,ye,ze,te,ee,edx,edy,edz); 
               printf("Electron distance from photon origin: %f\n", sqrt(pow(xe-exc_x[j],2) + pow(ye-exc_y[j],2) + pow(ze-exc_z[j],2)));  
               printf("Electron distance from origin: %f\n", sqrt(pow(xe-x0,2) + pow(ye-y0,2)));  

               photon_induced_x0.push_back(xe/factor); 
               photon_induced_y0.push_back(ye/factor); 
               photon_induced_z0.push_back(ze/factor); 
               photon_induced_t0.push_back(te); 
               aval->AvalancheElectron(xe,ye,ze,te,ee,edx,edy,edz); 
               int num_e,num_i; 
               aval->GetAvalancheSize(num_e,num_i); 
               nphoton_induced += num_e; 

               printf("avalanche size: %d\n", num_e); 

               for (int jjj = 0; jjj < aval->GetNumberOfElectronEndpoints(); jjj++)
               {

                 int status0; 
                 double xi0,yi0,zi0,xf0,yf0,zf0, t0,t1, e0,e1; 
                 aval->GetElectronEndpoint(jjj,xi0,yi0,zi0,t0,e0, xf0,yf0,zf0,t1,e1,status0); 

                 photon_induced_xi.push_back(xi0/factor); 
                 photon_induced_xf.push_back(xf0/factor); 
                 photon_induced_yi.push_back(yi0/factor); 
                 photon_induced_yf.push_back(yf0/factor); 
                 photon_induced_zi.push_back(zi0/factor); 
                 photon_induced_zf.push_back(zf0/factor); 
                 photon_induced_ti.push_back(t0); 
                 photon_induced_tf.push_back(t1); 
                 photon_induced_status.push_back(status0); 

               }



             }

           }

         }

       }


       exc_vis.clear(); 
       exc_vis.reserve(nexc); 
       nexc_vis = 0;
       nvisible_i = 0; 
       visible_i.reserve(ne);


       //check visibility 
       printf("checking visibility...\n"); 

       for (int j = 0; j < ne; j++)
       {
         bool visible = !model->occluded(xi[j],yi[j],zi[i]); 
         if (visible) 
         {
           visible_i.push_back(1); 
           nvisible_i++;
         }
         else
         {
           visible_i.push_back(0);
         }
       }

       for (int j = 0; j < nphoton_induced; j++)
       {
         bool visible = !model->occluded(photon_induced_xi[j], photon_induced_yi[j], photon_induced_zi[j]); 
         photon_induced_visible_i.push_back(visible ? 1 : 0); 

       }

       for (int j = 0; j < nexc; j++)
       {
         exc_x[j]=exc_x[j]/factor; 
         exc_y[j]=exc_y[j]/factor; 
         exc_z[j]=exc_z[j]/factor; 
       //  printf("%d ",j); 
         bool visible = !model->occluded(exc_x[j],exc_y[j],exc_z[j]); 
         if (visible) 
         {
           nexc_vis++;
           exc_vis.push_back(1); 
         }
         else
           exc_vis.push_back(0); 

       }
       printf("\n"); 


       tree->Fill(); 

       if (mode == SINGLE_ELECTRON) 
         break; 
     }

     //fill signal hists 
     

     if (model->electrodesEnabled())
     {
       for (unsigned i = 0; i < signals.size(); i++)
       {
         const char * name = signal_names[i]; 
         TH1 * signal = signals[i]; 
         TH1 * signal_e = signals_e[i]; 
         TH1 * signal_i = signals_i[i]; 

         for (int bin = 0; bin < signal_n; bin++)
         {
           signal->SetBinContent(bin+1, sensor->GetSignal(name, bin)); 
           signal_e->SetBinContent(bin+1, sensor->GetElectronSignal(name, bin)); 
           signal_i->SetBinContent(bin+1, sensor->GetIonSignal(name, bin)); 
         }

         //apply signal processing, if applicable
         if (amps[i]) 
         {

           
           TH1 * amp_signal = amp_signals[i]; 
           TH1 * amp_signal_e = amp_signals_e[i]; 
           TH1 * amp_signal_i = amp_signals_i[i]; 

           amps[i]->result(signal,amp_signal); 
           amps[i]->result(signal_e,amp_signal_e); 
           amps[i]->result(signal_i,amp_signal_i); 
         }
       }

     
       signal_tree->Fill(); 
     }
  }

  tree->Write(); 

  if (model->electrodesEnabled())
  {
    signal_tree->Write(); 
  }
  fout.Close(); 



//delete signal hists 
/*  for (unsigned i = 0; i < signals.size(); i++)
  {
    delete signals[i]; 
    delete signals_e[i]; 
    delete signals_i[i]; 
    delete amp_signals[i]; 
    delete amp_signals_e[i]; 
    delete amp_signals_i[i]; 
  }
  */



}

