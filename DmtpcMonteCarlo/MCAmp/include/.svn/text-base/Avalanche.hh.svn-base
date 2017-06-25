#ifndef DMTPC_MC_AMP_AVALANCHE_HH
#define DMTPC_MC_AMP_AVALANCHE_HH

#include <vector>
#include "TRandom3.h" 
#include "AvalancheMicroscopic.hh"
#include "AvalancheMC.hh"
#include "TrackHeed.hh" 
#include "Model.hh" 
#include "Solver.hh" 
#include "TrackMaker.hh" 
#include "IonizationModel.hh" 


namespace dmtpc
{
  namespace mc
  {
    namespace amp
    {
      class Avalanche 
      {

        public: 
          enum AvalancheMode
          {
            SINGLE_ELECTRON, 
            RETRIM, 
            PHOTON, 
            TRACK
          }; 



          Avalanche(Solver * solver, const Model * m, const char * unit="mm"); 
          virtual ~Avalanche(); 
          void setBounds(double xlow,double xhigh, double ylow,double yhigh,double zlow,double zhigh, bool reflect_z = false ); 
          void setTrackDirection(double dx, double dy, double dz) { part_dir[0] = dx; part_dir[1] = dy; part_dir[2] = dz; } 
          void setDebug(bool dbg) { dbg ? aval->EnableDebugging() : aval->DisableDebugging(); } 
          void setSeed(unsigned seed) { rand.SetSeed(seed); } 
          void setMode(AvalancheMode aval_mode) { mode = aval_mode;}
          void setTrackMaker(dmtpc::mc::retrim::TrackMaker * maker) { track_maker = maker; } 
          void setIonizationModel(dmtpc::mc::retrim::IonizationModel * model) { ionization_model = model; } 
          void setRecordExcitations(bool record) { record_excitations = record; } 
          void setMaxAvalancheSize(int max) { max_avalanche_size = max; } 
          void setEnergy(double e) { part_e = e; }
          void setTimeWindow(double tstart, double tstep, int nsteps) { sensor->SetTimeWindow(tstart, tstep, nsteps); ion_aval->SetTimeWindow(tstart, tstep*nsteps); }
          void setIonZRange( double zmin, double zmax) { ion_zrange_set = true; ion_zrange_min = zmin; ion_zrange_max = zmax; } 
          bool inZRange(double z) const { return !ion_zrange_set || (z*factor < ion_zrange_max && z*factor > ion_zrange_min); } 
          void enablePlotting(bool plot) { do_plot = plot; } 
          void enablePhotonTransport(bool b) { enable_photons = b; b ? aval->EnablePhotonTransport() : aval->DisablePhotonTransport(); }
          void enableFakeVUVTransport(bool b) { enable_fakevuv = b; }
          void setVUVParameters(double p, double mu, double sigma) { vuv_p = p; vuv_u = mu; vuv_sigma = sigma; } 
          int run(unsigned n, unsigned id = 0); 

        private: 
          const Model * model; 
          bool debug; 
          Solver * solver; 
          Garfield::AvalancheMicroscopic * aval; 
          Garfield::AvalancheMC * ion_aval; 
          Garfield::Sensor * sensor; 
          Garfield::TrackHeed * track;

          dmtpc::mc::retrim::TrackMaker * track_maker; 
          dmtpc::mc::retrim::IonizationModel * ionization_model; 
          double part_e; 
          double part_dir[3]; 
          TRandom3 rand; 
          AvalancheMode mode; 
          double zl,xl,yl,xu,yu,zu; 
          double vuv_p, vuv_u, vuv_sigma;
          bool bounds_set; 
          double factor; 
          double ion_zrange_min, ion_zrange_max; 
          bool do_plot; 
          bool enable_photons;
          bool enable_fakevuv;
//          Garfield::ViewDrift * vd; 
          bool ion_zrange_set; 
          bool record_excitations; 
          int max_avalanche_size; 
          bool reflect_z; 
      }; 
    }
  }
}


#endif
