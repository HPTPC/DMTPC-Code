#include "SampledMCAmpAvalanche.hh"
#include <assert.h>
#include "MCTruth.hh"
#include "TChain.h" 



dmtpc::mc::mctpc::SampledMCAmpAvalanche::SampledMCAmpAvalanche(TChain *c, 
    const char * cut,Params &p, double x0, double y0, double rot, double xy_bin_size, double xy_size, 
    double t_bin_size, double t0, double t1) 
: chain(c),par(p),
 avalanche_hist_xy("avalanche_hist_xy","Avalanche Space Histogram",
     int(xy_size / xy_bin_size), - xy_size/2, xy_size/2, int(xy_size/ xy_bin_size), -xy_size/2, xy_size/2), 
 avalanche_hist_t("avalanche_hist_t","Avalanche Time Histogram", int ((t1-t0) / t_bin_size), t0,t1), 
 x0(x0), y0(y0), orig_x0(x0), orig_y0(y0),rot(rot),
 xy_size(xy_size), xy_bin_size(xy_bin_size)

{
 gsl_rng_env_setup(); 
 rand = gsl_rng_alloc(gsl_rng_mt19937); 


 int n =  c->Draw("ne:Entry$",cut); 
 
// printf("n entries: %d\n",n); 
 double avg = 0; 
 double max_gain = 0; 

 pass_cut.reserve(n);
 for (int i = 0; i <n; i++)
 {
   avg += c->GetV1()[i]/n; 
   if (c->GetV1()[i] > max_gain) max_gain = c->GetV1()[i]; 
   pass_cut.push_back(int(c->GetV2()[i])); 
 }

 printf("avg gain: %f, max_gain: %f\n", avg, max_gain);

 if (!p.gain)
 {
   avg_gain = avg; 
   gain_scale_factor = 1; 
 }
 else
 {
   avg_gain = par.gain; 
   gain_scale_factor = par.gain/avg; 
   printf("gain_scale_factor: %f\n", gain_scale_factor); 
 }


 chain->SetBranchStatus("*",0); 
 chain->SetBranchStatus("ne",1); 
 chain->SetBranchAddress("ne",&aval_ne); 
 chain->SetBranchStatus("x0",1); 
 chain->SetBranchAddress("x0",&aval_x0); 
 chain->SetBranchStatus("y0",1); 
 chain->SetBranchAddress("y0",&aval_y0); 
 chain->SetBranchStatus("xi",1); 
 chain->SetBranchAddress("xi",&aval_xi); 
 chain->SetBranchStatus("yi",1); 
 chain->SetBranchAddress("yi",&aval_yi); 
 chain->SetBranchStatus("zi",1); 
 chain->SetBranchAddress("zi",&aval_zi); 
 chain->SetBranchStatus("ti",1); 
 chain->SetBranchAddress("ti",&aval_ti); 
 chain->SetBranchStatus("visible_i",1); 
 chain->SetBranchAddress("visible_i",&aval_visible); 


 max_xy_bin_x = -1; 
 max_xy_bin_y = -1; 
 min_xy_bin_x = -1; 
 min_xy_bin_y = -1; 

 if (par.randomize_angle)
   randomizeAngle(); 
 if (par.randomize_offset)
   randomizeOffset(); 

}


int dmtpc::mc::mctpc::SampledMCAmpAvalanche::doProcess(unsigned size, const Particle * in) 
{

  particles.reserve(int(1.1*par.frac_scint * avg_gain * size * par.acceptance)); 
  std::vector<double> xs; 
  std::vector<double> ys; 
  std::vector<double> zs; 
  std::vector<double> ts; 

  double prob_keep = par.acceptance * par.frac_scint; 


  for (unsigned i = 0; i < size; i++)
  {

    Particle p = in[i]; 
    if (par.xycut && pow(p.x-par.x0cut,2) + pow(p.y - par.y0cut,2) > par.r2cut) continue; 
    if (! p.type == Particle::electron) continue; 


    xs.clear(); 
    ys.clear();
    zs.clear();
    ts.clear();


    xs.push_back(p.x); 
    ys.push_back(p.y); 
    zs.push_back(par.gap_top); 
    ts.push_back(p.t); 


 
    for (unsigned k = 0; k < xs.size(); k++)
    {

      int entry = pass_cut[gsl_rng_uniform_int(rand, pass_cut.size())]; 
      assert(chain->GetEntry(entry)); 

    
      double aval_t0 = aval_ti[0]; 


      int ne = (int) (aval_ne * gain_scale_factor); 

      if (ne && zs[k] < par.gap_top) 
      {
        ne = (int) exp( log(0.+ne) * zs[k] / fabs(par.gap_top - par.gap_bottom)); 
      }


      ntotal+=ne; 

      nes.push_back(ne); 

      for (int ii =0; ii <ne; ii++)
      {



         bool pass_acceptance = gsl_rng_uniform(rand) < prob_keep;
         double lambda,photon_t,photon_E; 
         bool made_photon = par.photon_table.draw(&photon_E,&lambda,&photon_t); 

         if (!(made_photon || pass_acceptance)) continue; 

         unsigned j = gsl_rng_uniform_int(rand,aval_ne); 

         double xx = xs[k]+ aval_xi[j] - aval_x0 + x0; 
         double yy = ys[k] + aval_yi[j] - aval_y0 + y0; 
         assert(j < aval_zi.size()); 
         double z = aval_zi[j];
         double t = aval_ti[j] - aval_t0 + ts[k]; 

         if (made_photon)
         {

            double phi = gsl_rng_uniform(rand) * 2 * M_PI; 
            double sinth = -1 + 2 * gsl_rng_uniform(rand); 
            double costh = sqrt(1 - sinth*sinth);
            double distance = -log(gsl_rng_uniform(rand)) * lambda; 
            double zz = z + sinth*distance; 

            if (zz > par.gap_bottom)
            {

              double tt = t - gsl_rng_uniform(rand) * photon_t; 
              double xxx = xx + cos(phi) * costh * distance; 
              double yyy = yy + sin(phi) * costh * distance; 

              if (zz > par.gap_top)
              {
                tt += (zz - par.gap_top) / par.electron_drift_speed; 
                zz = par.gap_top;
           //     xxx = round(xxx/par.pitch) * par.pitch;  
           //     yyy = round(yyy/par.pitch) * par.pitch;
              }

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
         nphotons++; 

         if (! aval_visible[j]) continue; 
         nseen++; 

         /* Something here is probably wrong? */

         Particle photon; 
         photon.x = xx * cos(rot) - yy*sin(rot); 
         photon.y = yy * cos(rot) + xx*sin(rot); 
         photon.z = z;
         photon.vx=0; 
         photon.vy=0; 
         photon.vz=0; 

         photon.t = t  - log(gsl_rng_uniform(rand)) * par.scintillation_time;  
         photon.type = Particle::photon; 

         particles.push_back(photon); 
          
         /* copypasta from SimpleAvalanche... */ 
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
  return particles.size(); 

}


void dmtpc::mc::mctpc::SampledMCAmpAvalanche::prepareSignals(dmtpc::analysis::MCTruth * truth)
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

}

void dmtpc::mc::mctpc::SampledMCAmpAvalanche::clearSignals()
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

 if (par.randomize_angle)
   randomizeAngle(); 
 if (par.randomize_offset)
   randomizeOffset(); 

}


void dmtpc::mc::mctpc::SampledMCAmpAvalanche::randomizeOffset()
{
  x0 = orig_x0 + 2 * gsl_rng_uniform(rand) * par.max_offset - par.max_offset;  
  y0 = orig_y0 + 2 * gsl_rng_uniform(rand) * par.max_offset - par.max_offset;  
}

void dmtpc::mc::mctpc::SampledMCAmpAvalanche::randomizeAngle()
{
  rot += gsl_rng_uniform(rand) * M_PI * 2; 
}


dmtpc::mc::mctpc::SampledMCAmpAvalanche::~SampledMCAmpAvalanche()
{
  gsl_rng_free(rand); 
}

