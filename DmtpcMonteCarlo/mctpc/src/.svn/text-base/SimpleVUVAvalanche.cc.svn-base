#include "SimpleVUVAvalanche.hh" 
#include <cmath> 
#include "MCTruth.hh"
#include <gsl/gsl_randist.h>



dmtpc::mc::mctpc::SimpleVUVAvalanche::SimpleVUVAvalanche(Params &p, double x0 , double y0, double rot, 
                                                   double xy_bin_size, double xy_size, double t_bin_size, double t0, double t1) 
            : sap(p), x0(x0), y0(y0), rot(rot),
            avalanche_hist_xy("avalanche_hist_xy","Avalanche Space Histogram", 
                              int(xy_size / xy_bin_size), - xy_size/2, xy_size/2, int(xy_size/ xy_bin_size), -xy_size/2, xy_size/2), 
            avalanche_hist_t("avalanche_hist_t","Avalanche Time Histogram", int ((t1-t0) / t_bin_size), t0,t1) , 
            xy_size(xy_size), xy_bin_size(xy_bin_size)
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

void dmtpc::mc::mctpc::SimpleVUVAvalanche::prepareSignals(dmtpc::analysis::MCTruth * truth)
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
//  printf("nseen: %d\n",nseen); 

}

void dmtpc::mc::mctpc::SimpleVUVAvalanche::clearSignals()
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
}


dmtpc::mc::mctpc::SimpleVUVAvalanche::~SimpleVUVAvalanche()
{
  gsl_rng_free(rand); 
}


static double frac(double x) 
{
  return fabs(x - round(x)); 
}

int dmtpc::mc::mctpc::SimpleVUVAvalanche::doProcess(unsigned size, const Particle * in) 
{

  particles.reserve(int(1.1*sap.frac_scint * sap.gain * size * sap.acceptance)); 
  std::vector<double> xs; 
  std::vector<double> ys; 
  std::vector<double> zs; 
  std::vector<double> ts; 

  double beta = sap.gain / (alpha * (1- sap.fraction_attached)); 

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


    double t0 = p.t; 


    xs.clear(); 
    ys.clear();
    zs.clear();
    ts.clear();


    xs.push_back(x); 
    ys.push_back(y); 
    zs.push_back(sap.gap); 
    ts.push_back(t0); 

    for (unsigned k = 0; k < xs.size(); k++)
    {
    //compute gain
      bool attached = gsl_rng_uniform(rand) < sap.fraction_attached; 
      int ne = attached ? 0 :  int (gsl_ran_gamma(rand,alpha,beta)); 
      double xoffset = 0; 
      double yoffset = 0; 

      if (ne && zs[k] < sap.gap) 
      {
        ne= (int) exp( log(0.+ne) * zs[k] / sap.gap); 
      }
      else
      {

        xoffset = gsl_ran_gaussian_ziggurat(rand, sap.mesh_center_width); 
        yoffset = gsl_ran_gaussian_ziggurat(rand, sap.mesh_center_width); 
      }
      
      ntotal += ne; 

  
      //generate visible photons and charges
      for (int j = 0; j < ne; j++)
      {

        bool pass_acceptance = gsl_rng_uniform(rand) < sap.frac_scint * sap.acceptance; 
        bool pass_photon = gsl_rng_uniform(rand) < sap.vuv_photon_p; 
        if (!(pass_acceptance || pass_photon)) continue; 


        double z = zs[k]; 
        while ( z >= zs[k])
          z = -log(gsl_rng_uniform(rand))* sap.expo_lambda; 

        double t = ts[k] + (zs[k] - z) * sap.electron_drift_speed; 

        double xx = xs[k] + xoffset + gsl_ran_gaussian_ziggurat(rand,sap.avalanche_width); 
        double yy = ys[k] + yoffset + gsl_ran_gaussian_ziggurat(rand,sap.avalanche_width); 
        if (pass_photon)
        {
          double phi = gsl_rng_uniform(rand) * 2 * M_PI; 
          double sinth = -1 + 2 * gsl_rng_uniform(rand); 
          double costh = sqrt(1 - sinth*sinth);
          double distance = -log(gsl_rng_uniform(rand)) * sap.vuv_photon_lambda; 
          double zz = z + sinth*distance; 

          if (zz > 0) 
          {
            double tt = t - gsl_rng_uniform(rand) * sap.vuv_photon_time; 
            double xxx = xx + cos(phi) * costh * distance; 
            double yyy = yy + sin(phi) * costh * distance; 

            if (zz > sap.gap)
            {
              tt += (zz - sap.gap) / sap.electron_drift_speed; 
              zz = sap.gap;
              xxx = round(xxx/sap.pitch) * sap.pitch;  
              yyy = round(yyy/sap.pitch) * sap.pitch;
            }

            printf("Adding photoelectron %lu at %f,%f,%f\n",xs.size(), xxx,yyy,zz); 

            if (xs.size() > 10000) 
            {
              printf("Maximum number of photo electrons reached!!!!\n"); 
            }
            else
            {
              xs.push_back(xxx); 
              ys.push_back(yyy);
              zs.push_back(zz); 
              ts.push_back(tt); 
            }
          }

        }

        if (!pass_acceptance) continue;

        Particle photon; 


        nphotons++; 

        if ( frac((xx-xs[k])/sap.pitch - 0.5)  <  sap.wire_width/sap.pitch/2)
          continue; //assumed to be blocked by mesh
        if ( frac((yy-ys[k])/sap.pitch - 0.5) <  sap.wire_width/sap.pitch/2)
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

 
  }


//  printf("particles.size(): %d\n",(int) particles.size()); 
  return particles.size(); 
}
