#include "SimpleAvalanche.hh" 
#include <cmath> 
#include "MCTruth.hh"
#include <gsl/gsl_randist.h>



dmtpc::mc::mctpc::SimpleAvalanche::SimpleAvalanche(Params &p, double x0 , double y0, double rot, 
                                                   double xy_bin_size, double xy_size, double t_bin_size, double t0, double t1) 
            : sap(p), x0(x0), y0(y0), rot(rot),
            avalanche_hist_xy("avalanche_hist_xy","Avalanche Space Histogram", 
                              int(xy_size / xy_bin_size), - xy_size/2, xy_size/2, int(xy_size/ xy_bin_size), -xy_size/2, xy_size/2), 
            xy_size(xy_size), xy_bin_size(xy_bin_size),
            avalanche_hist_t("avalanche_hist_t","Avalanche Time Histogram", int ((t1-t0) / t_bin_size), t0,t1)  
{
            gsl_rng_env_setup(); 
            rand = gsl_rng_alloc(gsl_rng_mt19937); 
            alpha = 3.4;  
            ntotal = 0; 
            nphotons = 0; 
            nseen = 0; 

            if (sap.acceptance <0) sap.acceptance = 1; 
            max_xy_bin_x = -1; 
            max_xy_bin_y = -1; 
            min_xy_bin_x = -1; 
            min_xy_bin_y = -1; 
}

void dmtpc::mc::mctpc::SimpleAvalanche::prepareSignals(dmtpc::analysis::MCTruth * truth)
{


  int xlow = min_xy_bin_x > 0 ? min_xy_bin_x : 1; 
  int xup = max_xy_bin_x <= avalanche_hist_xy.GetNbinsX() ? max_xy_bin_x : avalanche_hist_xy.GetNbinsX(); 
  int ylow = min_xy_bin_y > 0 ? min_xy_bin_y : 1; 
  int yup = max_xy_bin_y <= avalanche_hist_xy.GetNbinsY() ? max_xy_bin_y : avalanche_hist_xy.GetNbinsY(); 

//  printf("xl,xu,yl,yu:%d,%d,%d,%d\n", xlow,xup,ylow,yup); 
  truth->avalanche_visible_photon_xy = new TH2I("avalanche_visible_photon_xy", "Visible Photons from Avalanche", 
                                              xup - xlow + 1, avalanche_hist_xy.GetXaxis()->GetBinLowEdge(xlow), avalanche_hist_xy.GetXaxis()->GetBinLowEdge(xup+1), 
                                              yup - ylow + 1, avalanche_hist_xy.GetYaxis()->GetBinLowEdge(ylow), avalanche_hist_xy.GetYaxis()->GetBinLowEdge(yup+1) 
                                              ); 

  for (int i = 1; i <= truth->avalanche_visible_photon_xy->GetNbinsX(); i++)
  {
    for (int j = 1; j <= truth->avalanche_visible_photon_xy->GetNbinsY(); j++)
    {
      truth->avalanche_visible_photon_xy->SetBinContent(i,j,  avalanche_hist_xy.GetBinContent(xlow + i -1, ylow + j -1)); 
    }
  }
  
  truth->avalanche_visible_photon_t = (TH1I*) avalanche_hist_t.Clone("avalanche_visible_photon_t");
  truth->ne_in_avalanche = ntotal; 
  truth->nphotons_from_avalanche = nphotons; 
  truth->nvisible_photons_from_avalanche = nseen; 
  truth->individual_avalanche_gain = nes; 

}

void dmtpc::mc::mctpc::SimpleAvalanche::clearSignals()
{
  avalanche_hist_xy.Reset(); 
  avalanche_hist_t.Reset(); 
  max_xy_bin_x = -1; 
  max_xy_bin_y = -1; 
  min_xy_bin_x = -1; 
  min_xy_bin_y = -1; 
  ntotal = 0; 
  nphotons = 0; 
  nseen = 0; 
  nes.clear(); 
}


dmtpc::mc::mctpc::SimpleAvalanche::~SimpleAvalanche()
{
  gsl_rng_free(rand); 
}


static double frac(double x) 
{
  return fabs(x - round(x)); 
}

int dmtpc::mc::mctpc::SimpleAvalanche::doProcess(unsigned size, const Particle * in) 
{

  particles.reserve(int(1.1*sap.frac_scint * sap.gain * size * sap.acceptance)); 

  for (unsigned i = 0; i < size; i++)
  {
    //check cut

    Particle p = in[i]; 
    if (sap.xycut && pow(p.x-sap.x0cut,2) + pow(p.y - sap.y0cut,2) > sap.r2cut) continue; 

    //transform to mesh frame
    if (! p.type == Particle::electron) continue; 

    double x= p.x * cos(rot) + sin(rot) * p.y - x0; 
    double y= -p.x * sin(rot) + cos(rot) * p.y - y0; ; 

    //move to nearest mesh center (with some width)

    x = round(x/sap.pitch) * sap.pitch;  
    y = round(y/sap.pitch) * sap.pitch;

    double xoffset = gsl_ran_gaussian_ziggurat(rand, sap.mesh_center_width); 
    double yoffset = gsl_ran_gaussian_ziggurat(rand, sap.mesh_center_width); 

    double t0 = p.t; 

    //compute gain

    bool attached = gsl_rng_uniform(rand) < sap.fraction_attached; 
    double beta = sap.gain / (alpha * (1- sap.fraction_attached)); 
    int ne = attached ? 0 :  int (gsl_ran_gamma(rand,alpha,beta)); 

    ntotal += ne; 
    nes.push_back(ne); 
    //generate visible photons and charges
    for (int j = 0; j < ne; j++)
    {

      if (gsl_rng_uniform(rand) > sap.frac_scint * sap.acceptance) continue; 

      nphotons++; 
      double z = -log(gsl_rng_uniform(rand))/ sap.expo_lambda; 
      double t = t0 + (sap.gap - z) * sap.electron_drift_speed; 

      Particle photon; 

      double xx = x + xoffset + gsl_ran_gaussian_ziggurat(rand,sap.avalanche_width); 
      double yy = y + yoffset + gsl_ran_gaussian_ziggurat(rand,sap.avalanche_width); 

      if ( frac((xx-x)/sap.pitch - 0.5)  <  sap.wire_width/sap.pitch/2)
        continue; //assumed to be blocked by mesh
      if ( frac((yy-y)/sap.pitch - 0.5) <  sap.wire_width/sap.pitch/2)
        continue; //assumed to be blocked by mesh

      nseen ++; 
      //transform back to lab frame
      xx+=x0; 
      yy+=y0; 

      photon.x =  xx * cos(rot) - yy*sin(rot); 
      photon.y =  yy * cos(rot) + xx*sin(rot); 

      photon.z = 0; 
      photon.vx=0; 
      photon.vy=0; 
      photon.vz=0; 

      // add scintillation time to photon 
      photon.t = t - log(gsl_rng_uniform(rand))/ sap.scintillation_time;  
      photon.type = Particle::photon; 

      particles.push_back(photon); 

      //fill histograms 
      
      
      if (avalanche_hist_xy.GetEntries() == 0)
      {
        // reset histogram center to this location, but keep the same global binning, I think this might work? We'll see. 
        double new_x_bin_center =round( photon.x / xy_bin_size) * xy_bin_size;    
        double new_y_bin_center =round( photon.y / xy_bin_size) * xy_bin_size;    
        avalanche_hist_xy.GetXaxis()->SetLimits(new_x_bin_center - xy_size/2, new_x_bin_center + xy_size/2); 
        avalanche_hist_xy.GetYaxis()->SetLimits(new_y_bin_center - xy_size/2, new_y_bin_center + xy_size/2); 
      }


      int binx = avalanche_hist_xy.GetXaxis()->FindFixBin(photon.x); 
      if (max_xy_bin_x < 0 || binx > max_xy_bin_x) max_xy_bin_x = binx; 
      if (min_xy_bin_x < 0 || binx < min_xy_bin_x) min_xy_bin_x = binx; 

      int biny = avalanche_hist_xy.GetYaxis()->FindFixBin(photon.y); 
      if (max_xy_bin_y  < 0 || biny > max_xy_bin_y) max_xy_bin_y = biny; 
      if (min_xy_bin_y  < 0 || biny < min_xy_bin_y) min_xy_bin_y = biny; 

      avalanche_hist_xy.Fill(photon.x, photon.y); 
      avalanche_hist_t.Fill(photon.t); 
    }
  }


  return particles.size(); 
}
