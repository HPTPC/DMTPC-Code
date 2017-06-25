#include "ClusterFinding.hh"
#include "ImageCalculations.hh"
#include "StitchInfo.hh"
#include <assert.h>
#include <set>
#include <list>
#include "Math/ProbFuncMathCore.h"
#include "TROOT.h" 
#include "TFile.h" 
#include "ImageTools.hh"
#include "GainMap.hh"
#include "Pixel.hh"
#include <algorithm>
#include <cmath>
#include <iostream>
#include "TStopwatch.h" 
#include <fstream>
#include "TH2.h"
#include "TMath.h"

/** Cluster finding algorithms and methods / functions that are ONLY 
 *  helpers for cluster finding algos go here */

static unsigned int numNeighbors(int nx, std::vector<bool> * used, int bin)
{
  unsigned int neighbors = 0; 
  for (int i = -1; i <=1; i++)
  {
    for (int j = -1; j <=1; j++)
    {
       if (!i && !j) continue; 
       int b = bin + i + j*nx; 
       if (used->at(b))  neighbors++; 
    }
  }  

  return neighbors; 
}



static bool should_collinear_merge(double min_global_rxy,  double min_each_rxy, 
                     double max_join_residual, double weight, 
                     const std::vector<int> c1, const std::vector<int> c2, const TH2 * image)
{

 //    std::cout << "Starting collinear merge!!!" << std::endl; 
     double uxy, ux, uxx,uy,uyy;        
     uxy=0;ux=0;uxx=0; uy=0;uyy=0; 
     double n=0;
     double rxy_each[2];  
     double m[2]; 
     double b[2]; 
     double ctr_x[2]; 
     double ctr_y[2]; 

     for (int clust = 0; clust < 2; clust++)
     {
       std::vector<int> which = clust == 0 ? c1 : c2; 

       double this_uxy, this_ux, this_uxx,this_uy,this_uyy;        
       this_uxy=0;this_ux=0;this_uxx=0; this_uy=0;this_uyy=0; 
       double this_n= 0; 
       for (unsigned int bin = 0; bin < which.size();bin++)
       {
          double content = image->GetBinContent(which[bin]); 
          content = pow(content,weight); 
          this_n+=content; 
          int x,y,z; 
          image->GetBinXYZ(which[bin],x,y,z); 
          this_ux+=content*x; 
          this_uy+=content*y; 
          this_uxx+=content*x*x; 
          this_uyy+=content*y*y;
          this_uxy+=content*x*y; 
       }

       //TODO: There are some simplications that can be made here
       m[clust] = (this_uxy - 1./this_n*this_ux*this_uy) / (this_uxx - 1./this_n*this_ux*this_ux); 
       rxy_each[clust] = (this_n*this_uxy - this_ux*this_uy) / TMath::Sqrt( (this_n*this_uxx - this_ux*this_ux) * (this_n*this_uyy - this_uy*this_uy) ); 
       b[clust] = this_uy/this_n - m[clust]*this_ux/this_n; 

//       std::cout << "m,b,rxy: " << m[clust] << " , " << b[clust] << " , " << rxy_each[clust] << std::endl; 
       ctr_x[clust] = this_ux/this_n; 
       ctr_y[clust] = this_uy/this_n; 

       n += this_n; 
       ux += this_ux/this_n; 
       uy += this_uy/this_n; 
       uyy += this_uyy/this_n; 
       uxx += this_uxx/this_n; 
       uxy += this_uxy/this_n; 

     }

     double global_rxy = (n*uxy - ux*uy) / TMath::Sqrt( (n*uxx - ux*ux) * (n*uyy - uy*uy) ); 


     //There are three cases here:
     //
     //  1) Both clusters have a well defined direction
     //     In this case, see if they match up well. 
     //  2) One cluster has a well defined direction. 
     //     In this case, see if the not so well defined cluster matches up well with the well defined cluster
     //  3) Neither cluster has a well defined direction. In this case, use the global line fit to determine if it's a good match. 
    
    if (fabs(rxy_each[0]) > min_each_rxy && fabs(rxy_each[1]) > min_each_rxy)
    {
        //Test residual for each each track extended to the other 
        //to see if it matches up    
       
        double resid1 = fabs (ctr_y[0] - m[1] * ctr_x[0] - b[1]) / TMath::Sqrt(1 + m[1]*m[1]); 
        double resid2 = fabs (ctr_y[1] - m[0] * ctr_x[1] - b[0]) / TMath::Sqrt(1 + m[0]*m[0]); 

        double resid_mean = TMath::Sqrt(resid1*resid2);  

//        std::cout << "BOTH HAVE DIRECTION, RESIDUALS: " << resid2 << " , " << resid1 << "MEAN: " << resid_mean <<  std::endl; 

        return resid_mean < max_join_residual; 
    }


    if (fabs(rxy_each[0]) > min_each_rxy) 
    {
       double resid = fabs (ctr_y[1] - m[0] * ctr_x[1] - b[0]) / TMath::Sqrt(1 + m[0]*m[0]); 
//       std::cout << "FIRST HAS DIRECTION, RESIDUAL: " << resid << std::endl; 
       return resid < max_join_residual; 
    } 

    if (fabs(rxy_each[1]) > min_each_rxy) 
    {
       double resid = fabs (ctr_y[0] - m[1] * ctr_x[0] - b[1]) / TMath::Sqrt(1 + m[1]*m[1]); 
//       std::cout << "SECOND HAS DIRECTION, RESIDUAL: " << resid << std::endl; 
       return resid < max_join_residual; 
    } 
    
//    std::cout << "NEITHER HAS DIRECTION, global_rxy: " << global_rxy << std::endl; 
    return fabs(global_rxy) > min_global_rxy; 
}

static void MergeClustersGM(const TH2* image, 
                           std::vector<std::vector<int> > & allcluster, 
                           const dmtpc::analysis::clusterfinding::GMMergeParams * p
                           )
{

  double minDistance = p->min_dist; 
  double min_rxy_global = p->min_rxy_global; 
  double min_rxy_cluster = p->min_rxy_cluster; 
  double max_join_residual = p->max_join_residual; 
  double ls_weight = p->least_squares_weight; 
  double spacer_width = p->spacer_width; 
  const dmtpc::analysis::GainMap* map = p->map; 

  if (allcluster.size() <=1) return; 

  int nbinsx = image->GetNbinsX()+2;
  int nbinsy = image->GetNbinsY()+2;

  std::vector<std::vector<double> > dists(allcluster.size(), std::vector<double>(map ? map->getNSpacers() : 1 ));  
  std::vector<std::vector<std::pair<double,double> > > positions(allcluster.size(), std::vector<std::pair<double,double> >(map ? map->getNSpacers() : 1));  
  std::vector<std::vector<int> > npositions(allcluster.size(), std::vector<int>(map ? map->getNSpacers() : 1)); 

  if (map && map->getNSpacers())
  {
    for (unsigned int i = 0; i < allcluster.size(); i++)
    {
      for (int s = 0; s < map->getNSpacers(); s++)
      {
        dists[i][s]=1e10; 
        positions[i][s].first=0; 
        positions[i][s].second=0; 
        npositions[i][s]=0; 
      }

      for (unsigned int b=0; b < allcluster[i].size(); b++)
      {
        int bin = allcluster[i][b]; 
       // std::cout << "LOOKING AT BIN " << b << std::endl; 
        int bx = bin % nbinsx; 
        int by = ((bin -bx) / nbinsx) % nbinsy; 
        double x = image->GetXaxis()->GetBinCenter(bx);
        double y = image->GetYaxis()->GetBinCenter(by);
       // std::cout << "x,y: " << x << "," << y << std::endl; 
        int s;
        double dist = map->distanceToNearestSpacer(x,y,s); 
         positions[i][s].first+=x;  
         positions[i][s].second+=y;  
         npositions[i][s]++; 
//         std::cout << "Nearest Spacer: " << s << " at a distance of " << dist << std::endl; 
        if (dist < dists[i][s])
        {
          dists[i][s] = dist;
        }
      }

      for (int s = 0; s < map->getNSpacers(); s++)
      {
        positions[i][s].first /= npositions[i][s];  
        positions[i][s].second /= npositions[i][s];  
      }

    }
  }


  for (unsigned int i = 0; i < allcluster.size(); i++)
  {
    for (unsigned int j = i+1; j < allcluster.size(); j++)
    {
      if (i == j) continue; 
//      std::cout << " CHECKING " << i << " AGAINST " << j << std::endl; 
      double diag = TMath::Sqrt(2)*image->GetXaxis()->GetBinWidth(1);
      //Check for the following condition
      // i is close to a spacer
      // j is close to the same spacer
      // i and j are on opposite sides of the spacer
      // i and j are close together

      double mind = dmtpc::image::pixel::minDist(allcluster[i],allcluster[j],const_cast<TH2*>(image)) ;


      assert (mind > 0); 

      bool merge = false;
     
      if (mind < minDistance) 
      {
//          std::cout << "DISTANCE BETWEEN " << i << " AND " << j << " is, " << mind << ", BELOW THRESH" << std::endl; 
          merge = true; 
      }
      else if (map) 
      {
        for (int s = 0; s < map->getNSpacers(); s++)
        {
//          std::cout << "MINIMUM DISTANCE FROM CLUSTER " << i << " TO SPACER " << s << " IS " << dists[i][s] << std::endl;
//          std::cout << "MINIMUM DISTANCE FROM CLUSTER " << j << " TO SPACER " << s << " IS " << dists[j][s] << std::endl;

          if (dists[i][s] < spacer_width*map->getSpacerWidth(s) + diag 
             && dists[j][s] < spacer_width*map->getSpacerWidth(s) + diag 
             && map->crossesSpacer(positions[i][s].first,
                                   positions[i][s].second,
                                   positions[j][s].first,
                                   positions[j][s].second)
            )
          {
             merge = true;  
             break; //We've already considered merging these two clusters, so no point in continuing 
          }
        }
      }
      
      if (merge && (mind <= diag*diag  || should_collinear_merge(min_rxy_global, min_rxy_cluster, max_join_residual, ls_weight, allcluster[i], allcluster[j],image)))
      {

        //Merge clusters
        std::cout << "MERGING " << i << " WITH " << j << std::endl; 

/*        for (vector<int>::const_iterator it = allcluster[i].begin(); it!=allcluster[i].end(); it++) std::cout << *it << " "  ; 
        std::cout << std::endl; 
        std::cout << std::endl; 
        for (vector<int>::const_iterator it = allcluster[j].begin(); it!=allcluster[j].end(); it++) std::cout << *it  << " " ; 
        std::cout << std::endl; 
        std::cout << std::endl; 
        */

        allcluster[i].insert(allcluster[i].end(), allcluster[j].begin(), allcluster[j].end()); 
        allcluster.erase(allcluster.begin()+j); 


        if (map)
        {
          // Merge distances 
          for (int s = 0; s < map->getNSpacers(); s++)
          {
             if (dists[j][s] < dists[i][s]) 
             {
               dists[i][s] = dists[j][s]; 
               positions[i][s].first = positions[j][s].first; 
               positions[i][s].second = positions[j][s].second; 
             }
          }

          dists.erase(dists.begin() + j); 
          positions.erase(positions.begin() + j); 
        }


        if (i) i = i-1;
        j = 0;  
        
        
      }
    }
  }
}


struct image_value_t
{
   int bin; 
   double blurred;
   double raw; 
   double blurred_scaled; 
   double raw_scaled; 
};


bool image_value_cmp(image_value_t i, image_value_t j)
{
     return i.blurred_scaled < j.blurred_scaled; 
}



static TH2F * rmsScale(const TH2 * image, const dmtpc::image::StitchInfo * sinfo, const double * image_rms, const double * image_mean)
{
  
  int nimages = sinfo ? sinfo->nimages : 1; 
  TH2F * scaled = (TH2F*) dmtpc::image::tools::newTH2StealSize(image, 'F',"scaledhist","scaledhist"); 

  TH2F * mean =(TH2F*)  dmtpc::image::tools::newTH2StealSize(scaled,'F',"mean","mean"); 
  TH2F * rms = (TH2F*) dmtpc::image::tools::newTH2StealSize(scaled,'F',"rms","rms"); 

  for (int x = 1; x <= image->GetNbinsX(); x++)
  {
    for (int y = 1; y <= image->GetNbinsY(); y++)
    {
      for (int img = 0; img < nimages; img++)
      {
        if (!sinfo)
        {
           scaled->SetBinContent(x,y, (image->GetBinContent(x,y) - *image_mean) / *image_rms); 
        }
        else
        {
          rms->SetBinContent(x,y, rms->GetBinContent(x,y) + pow(image_rms[img],2) * sinfo->frac[img]->GetBinContent(x,y) * sinfo->weight[img] * sinfo->weight[img]);
          mean->SetBinContent(x,y, mean->GetBinContent(x,y) +image_mean[img] * sinfo->frac[img]->GetBinContent(x,y) * sinfo->weight[img]);
        }
      }
      
      if (sinfo)
      {
        rms->SetBinContent(x,y,sqrt(rms->GetBinContent(x,y)) / sinfo->weight_sum->GetBinContent(x,y)); 
        mean->SetBinContent(x,y,mean->GetBinContent(x,y) / sinfo->weight_sum->GetBinContent(x,y)); 
      }
    }
  }

  if (sinfo)
  {
    for (int x=1; x <= scaled->GetNbinsX(); x++)
    {
      for (int y = 1; y<= scaled->GetNbinsY(); y++)
      {
        scaled->SetBinContent(x,y, sinfo->weight_sum->GetBinContent(x,y) > 0 ? (image->GetBinContent(x,y) - mean->GetBinContent(x,y)) / rms->GetBinContent(x,y) : 0); 
      }
    }
  }


   delete mean; 
   delete rms; 

   return scaled; 
}


int dmtpc::analysis::clusterfinding::findClustersGMRing( const TH2 * image, 
                                          std::vector<std::vector<int> > * clust, 
                                          std::vector<std::vector<int> > * reduced_clust, 
                                          const dmtpc::image::StitchInfo * sinfo, 
                                          const double * image_rms, const double * image_means, double space_sigma, 
                                          double rms_sigma, double core_thresh_high, double core_thresh_low,  
                                          double ring_thresh, double ring_nsigma, unsigned int min_size, dmtpc::image::processing::BILATERAL_VALUE_FN fn, unsigned int ncleanup, 
                                          const GMMergeParams * merge_params,
                                          const char * debug, int fast_bilateral_scale_exp)
{
  TStopwatch timer; 
  TH2 * scaled = rmsScale(image, sinfo, image_rms, image_means); 
  std::cout <<"Scaled: " << timer.CpuTime() <<  std::endl;
  timer.Start(); 
  TH2 * filtered = fastBilateralFilter(scaled, space_sigma, rms_sigma, 3, fn, fast_bilateral_scale_exp, true); 
  std::cout <<"Filtered: " << timer.CpuTime() <<  std::endl;
  timer.Start(); 

  TFile * dbg = 0; 
  if (debug)
  {
     dbg = new TFile(debug,"recreate"); 
     dbg->cd(); 
     scaled->Write("scaled"); 
     filtered->Write("filtered"); 
     image->Write("image"); 
     gROOT->cd(); 
  }

  const int nbinsx = image->GetNbinsX()+2; 
  const int nbinsy = image->GetNbinsY()+2; 
  const int nbins = nbinsx * nbinsy; 

  std::vector<std::set<int> > proto_clusters; 
  std::vector<bool> used(nbins); 
  std::vector<std::pair<double, int> > values(nbins); 

  int i = 0; 
  for (int x= 1; x <= nbinsx; x++)
  {
    for (int y= 1; y <= nbinsy; y++)
    {
       values[i++] = std::pair<double,int>(filtered->GetBinContent(x,y), filtered->GetBin(x,y)); 
    }
  }

  std::sort(values.rbegin(), values.rend()); 

  std::cout <<"Sorted: " << timer.CpuTime() <<  std::endl;
  timer.Start(); 
  //do first pass hysteresis thresholding
  for (i = 0; i < nbins; i++)
  {
    int bin = values[i].second; 
    double binval = values[i].first; 
    if (binval < core_thresh_high) break; 
    if (used[bin]) continue; 
    if (sinfo && sinfo->weight_sum->GetBinContent(bin) == 0) continue; 
    used[bin] = true; 

    std::vector<int> cluster; 
    cluster.push_back(bin); 
    std::cout <<"Starting new cluster" << std::endl; 
    for (unsigned int b = 0; b< cluster.size(); b++)
    {

      const static int xneighbors[4] = { -1,0,0,1}; 
      const static int yneighbors[4] = { 0,1,-1,0}; 
     
      for (int ni = 0; ni <4; ni++)
      {
          int xi = xneighbors[ni]; 
          int yi = yneighbors[ni]; 
          int new_bin = cluster[b] + xi + nbinsx * yi; 
          if (used[new_bin]) continue; 
          if (filtered->GetBinContent(new_bin) >= core_thresh_low)
          {
            used[new_bin]=true;  
            cluster.push_back(new_bin); 
          }
      }
    }

    std::set<int> cluster_set(cluster.begin(), cluster.end()); 
    std::set<int> cluster_set_orig(cluster_set.begin(), cluster_set.end()); 

    int max_size = 0; 
    for (unsigned cleanup = 0; cleanup < ncleanup; cleanup++)
    {
      max_size += dmtpc::image::pixel::erode(filtered, &cluster_set); 
    }

    for (unsigned cleanup = 0; cleanup < ncleanup; cleanup++)
    {
      max_size += dmtpc::image::pixel::dilate(filtered, &cluster_set); 
    }


    std::vector<int> to_use(max_size,-1); 

    std::set_difference(cluster_set.begin(),cluster_set.end(), cluster_set_orig.begin(), cluster_set_orig.end(), to_use.begin()); 

    for (std::vector<int>::iterator bit = to_use.begin(); bit != to_use.end(); bit++)
    {
        if (*bit == -1) break;
        used[*bit] = true;  
    }

    std::vector<int> to_unuse(max_size,-1); 
    std::set_difference(cluster_set_orig.begin(), cluster_set_orig.end(), cluster_set.begin(),cluster_set.end(), to_unuse.begin()); 

    for (std::vector<int>::iterator bit = to_unuse.begin(); bit != to_unuse.end(); bit++)
    {
        if (*bit == -1) break;
        used[*bit] = false;  
    }


    if (cluster_set.size() < min_size)
    {
      //unuse 
      //
      for (std::set<int>::iterator bit = cluster_set.begin(); bit != cluster_set.end(); bit++)
      {
        used[*bit] = false;  
      }
    }
 
    else
    {
      proto_clusters.push_back(cluster_set); 
    }
  }


  std::cout << "Protoclustered: " << timer.CpuTime() << std::endl; 
  timer.Start(); 
  //Calculate rings
  
  std::vector<std::vector<int> > clusters; 

  for (i = 0; i < int(proto_clusters.size()); i++)
  {

    std::cout <<"Adding rings" << std::endl; 
    std::list<double> layers; 
    std::set<int> clust_copy = proto_clusters[i]; 
    std::set<int> boundary;
    int ninner = 0; 
    //calculate inner rings
    while (true )  
    {
       boundary.clear(); 
       dmtpc::image::pixel::innerBorder(scaled, &clust_copy, &boundary); 
       double sum = 0; 
       for(std::set<int>::iterator bit = boundary.begin(); bit!= boundary.end(); bit++)
       {
         sum += scaled->GetBinContent(*bit);  
         clust_copy.erase(*bit); 
       }
       layers.push_front(sum); 
       ninner++; 
       std::cout <<"ninner: "<< ninner << "  " <<  timer.CpuTime() << std::endl; 
       timer.Start(); 
       if (clust_copy.size() == 0) break;
    }


    //calculate outer rings
    clust_copy = proto_clusters[i];  
    while(true)
    {
      boundary.clear(); 
      dmtpc::image::pixel::outerBorder(scaled, &clust_copy, &boundary); 
      double sum = 0; 
      int bsize  = 0; 
      for(std::set<int>::iterator bit = boundary.begin(); bit!= boundary.end(); bit++)
      {
         //check if other clusters own this. If they do, don't consider
         if (used[*bit]) continue; 
         if (sinfo && sinfo->weight_sum->GetBinContent(*bit) == 0) continue; 
         sum += scaled->GetBinContent(*bit);  
         clust_copy.insert(*bit); 
         used[*bit] = true; 
         bsize++; 
      }

      layers.push_back(sum/bsize); 
      std::cout << "sum: " << sum << "  " <<  timer.CpuTime() << std::endl; 
      timer.Start(); 
      if (!bsize || sum < ring_thresh * sqrt(bsize))  break; 
    }



    if (debug)
    {
      dbg->cd(); 
      TH1F * track_layers = new TH1F(TString::Format("track%d_layers",i),"Track Layers", layers.size(), 0, layers.size()); 
      int j = 1; 
      for (std::list<double>::iterator it = layers.begin(); it!=layers.end(); it++) track_layers->SetBinContent(j++, *it); 
      track_layers->Write(); 
      gROOT->cd(); 
    }

   
    //compute rms of layers
    int d = 0; 
    double rms = 0; 
    double sum = 0; 
    for (std::list<double>::iterator it = layers.begin(); it!=layers.end(); it++)
    {
      rms += (*it) *d*d; 
      sum += *it; 
      d++; 
    }
    rms = 2*sqrt(fabs(rms/(sum))); 
    std::cout << "rms: " << rms << std::endl; 
    
    int n_needed = int(ring_nsigma * rms) - layers.size(); 
    if (n_needed >10) n_needed = 10; 

    std::cout <<" n needed: " << n_needed<< std::endl; 

    for (int l = 0; l < n_needed; l++)
    {
      boundary.clear(); 
      dmtpc::image::pixel::outerBorder(scaled, &clust_copy, &boundary); 
      std::cout << l << std::endl; 
      for(std::set<int>::iterator bit = boundary.begin(); bit!= boundary.end(); bit++)
      {
        if (used[*bit]) continue; 
         if (sinfo && sinfo->weight_sum->GetBinContent(*bit) == 0) continue; 
        used[*bit]= true;
        clust_copy.insert(*bit); 
      }
    }


    std::vector<int> cluster_v; 
    for (std::set<int>::iterator bit = clust_copy.begin(); bit != clust_copy.end(); bit++)
    {
     cluster_v.push_back(*bit);  
    }

    clusters.push_back(cluster_v); 
  }

  std::cout << "premerge: " << timer.CpuTime() << std::endl; 
  timer.Start(); 
  //merge clusters here 
  MergeClustersGM(scaled, clusters, merge_params); 

  std::cout << "postmerge: " << timer.CpuTime() << std::endl; 
  timer.Start(); 
  

  for (i = 0; i < int(clusters.size()); i++)
  {
    std::vector<int> red_cluster; 

    for (int k = 0; k < (int) clusters[i].size(); k++)
    {
       if (scaled->GetBinContent(clusters[i][k]) > core_thresh_low)
       {
          red_cluster.push_back(clusters[i][k]); 
       }
    }

    clust->push_back(clusters[i]); 
    reduced_clust->push_back(clusters[i]); 
  }

  delete scaled; 
  delete filtered; 

  std::cout << "final: " << timer.CpuTime() << std::endl; 
  timer.Start(); 

  if (debug)
  {
    dbg->Close(); 
    dbg->Delete(); 
  }
  return clusters.size(); 

}




int dmtpc::analysis::clusterfinding::findClustersGMSeedStitch (const TH2 * image, 
                                                   std::vector<std::vector<int> > * clust,
                                                   std::vector<std::vector<int> > * reduced_clust,
                                                   const dmtpc::image::StitchInfo * sinfo,
                                                   const double * image_mean, const double * blurred_means,
                                                   const double * image_rms, const double * blurred_rmses, 
                                                   double seed_thresh, double red_thresh, 
                                                   double thresh_pct,
                                                   double max_wrong_p, double min_thresh,
                                                   int blur_n, double blur_sigma, 
                                                   unsigned int neighbors_thresh, int min_neighbors, 
                                                   unsigned int minsize, 
                                                   const GMMergeParams * merge_params, 
                                                   const char * debug, 
                                                   bool reproduce_bug
                                                   )  
{

  TH2 * blurred  = (TH2*) dmtpc::image::processing::gaussianBlur(image, blur_n, blur_sigma); 

  TH2F * rms = (TH2F*) dmtpc::image::tools::newTH2StealSize(sinfo->weight_sum,'F',"rmshist","rmshist"); 
  TH2F * blurred_rms = (TH2F*) dmtpc::image::tools::newTH2StealSize(sinfo->weight_sum,'F',"blurredrmshist","blurredrmshist"); 
  TH2F * mean = (TH2F*) dmtpc::image::tools::newTH2StealSize(sinfo->weight_sum,'F',"meanhist","meanhist"); 
  TH2F * blurred_mean = (TH2F*) dmtpc::image::tools::newTH2StealSize(sinfo->weight_sum,'F',"blurredmeanhist","blurredmeanhist"); 
   
//  cout << rms->GetNbinsX() << endl; 

  for (int x = 1; x <=rms->GetNbinsX(); x++)
  {
    for (int y = 1; y <=rms->GetNbinsY(); y++)
    {
        for (unsigned i = 0; i < sinfo->frac.size(); i++)                   
        {
          rms->SetBinContent(x,y, rms->GetBinContent(x,y) + pow(image_rms[i],2) * sinfo->frac[i]->GetBinContent(x,y)*sinfo->weight[i] ); 
          blurred_rms->SetBinContent(x,y, blurred_rms->GetBinContent(x,y) + pow(blurred_rmses[i],2) * sinfo->frac[i]->GetBinContent(x,y)*sinfo->weight[i] ); 
          mean->SetBinContent(x,y, mean->GetBinContent(x,y) + image_mean[i] * sinfo->frac[i]->GetBinContent(x,y) * sinfo->weight[i]);
          blurred_mean->SetBinContent(x,y, blurred_mean->GetBinContent(x,y) + blurred_means[i] * sinfo->frac[i]->GetBinContent(x,y) * sinfo->weight[i]);
        }

        rms->SetBinContent(x,y, sqrt(rms->GetBinContent(x,y)) / sinfo->weight_sum->GetBinContent(x,y)); 
        blurred_rms->SetBinContent(x,y, sqrt(blurred_rms->GetBinContent(x,y)) / sinfo->weight_sum->GetBinContent(x,y)); 
        mean->SetBinContent(x,y, mean->GetBinContent(x,y) / sinfo->weight_sum->GetBinContent(x,y)); 

        if (mean->GetBinContent(x,y) + min_thresh * rms->GetBinContent(x,y) < 0)
        {
           mean->SetBinContent(x,y,0);
        }

        blurred_mean->SetBinContent(x,y, blurred_mean->GetBinContent(x,y) / sinfo->weight_sum->GetBinContent(x,y)); 
    }
  }

//  double blur_factor = 1./(2*sqrt(M_PI) * blur_sigma); 


  std::vector<std::vector<int> > allcluster; 
  std::vector<int> cluster; 

  const int nbinsx = image->GetNbinsX()+2; 
  const int nbinsy = image->GetNbinsY()+2; 

//  const int nbins = (nbinsx-2) * (nbinsy-2); 
  std::vector< bool> used(nbinsx*nbinsy); 

  std::vector<image_value_t > values; 
  TH2 * blurred_scaled_img = 0; 
  TH2 * scaled_img = 0; 

  if (debug)
  {
     blurred_scaled_img = dmtpc::image::tools::newTH2StealSize(rms,'F',"blur_scaled","blur_scaled"); 
     scaled_img = dmtpc::image::tools::newTH2StealSize(rms,'F',"scaled","scaled"); 
  }

  for (int u = 1; u <= image->GetNbinsX(); u++)
  {
     for (int v = 1; v <= image->GetNbinsY(); v++)  
     {
        if (sinfo->weight_sum->GetBinContent(u,v) == 0) continue; 

        image_value_t val;
        val.bin = image->GetBin(u,v); 
        val.blurred = blurred->GetBinContent(val.bin); 
        val.raw = image->GetBinContent(val.bin); 
        double raw_rms = rms->GetBinContent(val.bin); 
        double blur_rms = blurred_rms->GetBinContent(val.bin); 
        double m= mean->GetBinContent(val.bin);
        double bm= blurred_mean->GetBinContent(val.bin);
        val.raw_scaled = (val.raw-m) / raw_rms;  //ASSUMPTION mean is same
//        val.blurred_scaled = (val.blurred-m) / (raw_rms * blur_factor);  //ASSUMPTION mean is same 
        val.blurred_scaled = (val.blurred-bm) / (blur_rms);  //ASSUMPTION mean is same 

        if (isnan(val.blurred_scaled) || isinf(val.blurred_scaled))
        {
          std::cout << u << " " << v << " " << val.bin<< " "  << val.blurred<< " "  << val.raw<< " "  << raw_rms << " " << m << " " << sinfo->weight_sum->GetBinContent(u,v) << std::endl; 

        }
            
        else
        {
          values.push_back(val); 
          if (debug)
          {
            blurred_scaled_img->SetBinContent(u,v, val.blurred_scaled); 
            scaled_img->SetBinContent(u,v, val.raw_scaled); 
          }
        }
     }
  }

  std::sort(values.rbegin(),values.rend(), image_value_cmp); 

  int niter = 0; 

  while(true)
  {
    cluster.clear(); 
    double blurred_binval = values[niter].blurred_scaled; 
    //cout <<"blurred_binval: " <<  blurred_binval <<endl; 
    if (blurred_binval < seed_thresh) break; 
    int bin = values[niter].bin;      

    double binval = values[niter].raw_scaled; 
    niter++; 
    //cout <<"binval: " <<  binval <<endl; 
    //cout <<"bin: " <<  bin<<endl; 
    if (used[bin]) continue; 
    used[bin] = true; 
    //double seedval = blurred_binval;
    double current_min_increment = (binval - min_thresh) * (1 - thresh_pct); 

    //To reproduce the behavior of findClusterGMSeed. However, this is probably a bug in the original... 
    double adj_current_min_increment = (binval*rms->GetBinContent(bin)/blurred_rms->GetBinContent(bin) - min_thresh) * (1 - thresh_pct); 
    double current_min = binval - current_min_increment; 
    double blurred_current_min_increment = (blurred_binval - min_thresh) * (1 - thresh_pct); 
    double blurred_current_min = blurred_binval - blurred_current_min_increment; 
    double fake_prob = TMath::Erfc(blurred_binval); 
    double chisq = - 2 * TMath::Log(fake_prob); 

    //Start a new cluster 
    cluster.push_back(bin); 
    while(current_min > min_thresh && blurred_current_min > min_thresh)
    {
      //cout <<"current_min: " <<  current_min <<endl; 
      int nadded = 0; 
      for (unsigned int b = 0; b < cluster.size(); b++)
      {
        int xc,yc;           
        xc = cluster[b] % nbinsx; 
        yc = (cluster[b]/nbinsx)%nbinsy; 

        for (int i = xc -1; i<=xc+1;i++)
        {
          for (int j = yc-1; j<=yc+1; j++)
          {
            if (i==xc && j==yc) continue; 
            bin = image->GetBin(i,j);   
            if (used[bin]) continue; 

            binval = (image->GetBinContent(bin)-mean->GetBinContent(bin)) / rms->GetBinContent(bin); 
//            blurred_binval = (blurred->GetBinContent(bin)-mean->GetBinContent(bin)) / rms->GetBinContent(bin) / blur_factor;  
            blurred_binval = (blurred->GetBinContent(bin)-blurred_mean->GetBinContent(bin)) / blurred_rms->GetBinContent(bin); 
            //cout << " new bin, binval, blurred_binval: " << bin << " , "  <<  binval << " , "<< blurred_binval << endl; 
            if (binval >current_min && blurred_binval > blurred_current_min) 
            {
              fake_prob = TMath::Erfc(blurred_binval); 
              chisq -= 2 * TMath::Log(fake_prob);  
              used[bin] = true; 
              cluster.push_back(bin); 
              //cout << " clustersize: " << cluster.size() << std::endl; 
              nadded++; 
            }
          }
        }
      }
      //cout <<"nadded: " <<  nadded <<endl; 
      if (nadded == 0) break; 
      current_min -= current_min_increment;
//      blurred_current_min -= blurred_current_min_increment; 
      blurred_current_min -= reproduce_bug ?  adj_current_min_increment : blurred_current_min_increment ; 
    }

    if (cluster.size() < minsize) 
    {
      for (unsigned int i = 0; i < cluster.size(); i++) { used[cluster[i]] = false; } 
      continue; 
    }
//    std::cout << "niter: " << niter << std::endl; 
//    std::cout << "seedval: " << blurred_binval << std::endl; 
    std::cout << "CLUSTERSIZE BEFORE FILLING IN HOLES: " << cluster.size() << std::endl; 

    //Fill in holes
    for (unsigned int b = 0; b < cluster.size(); b++)
    {
      for (int i = -1; i <=1; i++)
      {
        for (int j = -1; j <=1; j++)
        {
          if (!i && !j) continue; 
          int nb = cluster[b] + i + j*nbinsx; 
          if (nb < nbinsx || nb % nbinsx == 0 || nb % nbinsx == nbinsx-1 || nb >= nbinsx*(nbinsy-1)) continue; 
          if( !used[nb] && numNeighbors(nbinsx, &used, nb) > neighbors_thresh)
          {
            used[nb] = true; 
            cluster.push_back(nb); 
            blurred_binval = (blurred->GetBinContent(nb)  - mean->GetBinContent(nb))  / blurred_rms->GetBinContent(nb); 
            fake_prob = TMath::Erfc(blurred_binval); 
            chisq -= 2 * TMath::Log(fake_prob);  
          } 
        }  
      }
    }


    std::cout << "CLUSTERSIZE AFTER FILLING IN HOLES: " << cluster.size() << std::endl; 

    //remove peninsulas
    
    while(true)
    {
      int nremoved = 0; 
      
      for (int b = cluster.size()-1; b >=0; b--)
      {
        bin = cluster[b]; 

        if (bin < nbinsx || bin % nbinsx == 0 || bin % nbinsx == nbinsx-1 || bin >= nbinsx*(nbinsy-1)) continue; 
        if ((int) numNeighbors(nbinsx, &used, bin) < min_neighbors)  
        {
          used[bin] = false; 
          nremoved++; 
          cluster.erase(cluster.begin()+b); 
          blurred_binval = (blurred->GetBinContent(bin) - mean->GetBinContent(bin) )  / blurred_rms->GetBinContent(bin); 
          fake_prob = TMath::Erfc(blurred_binval); 
          chisq += 2 * TMath::Log(fake_prob);  
        }
      }
      if (!nremoved) break; 
    }

    std::cout << "CLUSTERSIZE AFTER REMOVING PENINSULAS: " << cluster.size() << std::endl; 

    if (cluster.size() < minsize)
    {
      for (unsigned int i = 0; i < cluster.size(); i++) { used[cluster[i]] = false; } 
      continue; 
    }
    double wrong_p = ROOT::Math::chisquared_cdf_c(chisq, cluster.size() -1);  
//    std:: cout << "WRONG_P: " << wrong_p << std::endl; 
    if (wrong_p > max_wrong_p)
    {
      for (unsigned int i = 0; i < cluster.size(); i++) { used[cluster[i]] = false; } 
      continue; 
    }
    allcluster.push_back(cluster); 
  }


  if (debug)
  {
    TFile dfile(debug,"RECREATE");
    dfile.cd(); 
    mean->Write(); 
    rms->Write(); 
    blurred_mean->Write(); 
    blurred_rms->Write(); 
    blurred->Write(); 
    blurred_scaled_img->Write(); 
    scaled_img->Write(); 
    dfile.Flush(); 
    gROOT->cd(); 

    delete blurred_scaled_img; 
    delete scaled_img; 
  }


  std:: cout << "niter: " << niter << std::endl; 
  std::cout << "About to merge. Premerge we have: " << allcluster.size() << std::endl; 

  //Now time to merge! 
  MergeClustersGM(image, allcluster, merge_params);

  for(int j = 0; j < (int) allcluster.size(); j++)
  {
     //We also need to add reduced clusters here 
    std::vector<int> red_cluster; 

     for (int k = 0; k < (int) allcluster[j].size(); k++)
     {
       if (image->GetBinContent(allcluster[j][k]) > red_thresh * rms->GetBinContent(allcluster[j][k]))
       {
          red_cluster.push_back(allcluster[j][k]); 
       }
     }

     clust->push_back(allcluster[j]);
     reduced_clust->push_back(red_cluster); 

  }


  mean->Delete(); 
  rms->Delete(); 
  blurred_mean->Delete(); 
  blurred_rms->Delete(); 
  blurred->Delete(); 
  std::cout << allcluster.size() << " cluster(s) found. \n";


  return allcluster.size();      
}



int dmtpc::analysis::clusterfinding::findClustersGMSeed(const TH2 * image, std::vector<std::vector<int> > * clust,
                                                   double seed_thresh, double thresh_pct,
                                                   double max_wrong_p, double min_thresh,
                                                   int blur_n, double blur_sigma, 
                                                   unsigned int neighbors_thresh, int min_neighbors, 
                                                   unsigned int minsize, 
                                                   const GMMergeParams * merge_params, 
                                                   bool reproduce_bug 
                                                   )  
{


  TH2 * blurred = (TH2*) dmtpc::image::processing::gaussianBlur(image, blur_n, blur_sigma); 

  double mean, rms; 
  dmtpc::image::calculations::meanRMSNoOutliers(image,mean,rms); 

  double blurred_mean, blurred_rms; 
  dmtpc::image::calculations::meanRMSNoOutliers(blurred,blurred_mean,blurred_rms); 

  double min_seed = seed_thresh*blurred_rms + blurred_mean; 
//  std::cout << "min_seed: " << min_seed << std::endl; 
  double min = min_thresh * rms + mean; 
  double blurred_min = min_thresh * blurred_rms + blurred_mean; 
  if (min < 0) min = min_thresh * rms; 


  std::vector<std::vector<int> > allcluster; 
  std::vector<int> cluster; 

  const int nbinsx = image->GetNbinsX()+2; 
  const int nbinsy = image->GetNbinsY()+2; 

  const int nbins = nbinsx * nbinsy; 
  std::vector< bool> used(nbins); 
  std::vector<std::pair<double,int> > values(nbins); 

  for (int u = 1; u <= nbinsx; u++)
  {
     for (int v = 1; v <= nbinsy; v++)  
     {
        int bin = image->GetBin(u,v); 
        values.push_back(std::pair<double,int>(blurred->GetBinContent(bin),bin)); 
     }
  }

  std::sort(values.rbegin(),values.rend()); 

  int niter = 0; 

  while(true)
  {
    cluster.clear(); 
    double blurred_binval = values[niter].first; 
    if (blurred_binval < min_seed) break; 
    int bin = values[niter++].second;      
    if (used[bin]) continue; 
    used[bin] = true; 

    double binval = image->GetBinContent(bin); 
    double seedval = blurred_binval;
    double current_min_increment = (binval - min) * (1 - thresh_pct); 
    double current_min = binval - current_min_increment; 
    double blurred_current_min_increment = (blurred_binval - blurred_min) * (1 - thresh_pct); 
    double blurred_current_min = blurred_binval - blurred_current_min_increment; 
    double fake_prob = TMath::Erfc(blurred_binval/blurred_rms); 
    double chisq = - 2 * TMath::Log(fake_prob); 

    //Start a new cluster 
    cluster.push_back(bin); 
    while(current_min > min && blurred_current_min > blurred_min)
    {
//      std::cout << "current_min: " << current_min << std::endl; 
//      std::cout << "blurred_current_min: " << blurred_current_min << std::endl; 
      int nadded = 0; 
      for (unsigned int b = 0; b < cluster.size(); b++)
      {
        int xc,yc;           
        xc = cluster[b] % nbinsx; 
        yc = ((cluster[b]-xc)/nbinsx)%nbinsy; 

        for (int i = xc -1; i<=xc+1;i++)
        {
          for (int j = yc-1; j<=yc+1; j++)
          {
            if (i==xc && j==yc) continue; 
            bin = image->GetBin(i,j);   
            if (used[bin]) continue; 

            binval = image->GetBinContent(bin); 
            blurred_binval = blurred->GetBinContent(bin);  
            if (binval >current_min && blurred_binval > blurred_current_min) 
            {
              fake_prob = TMath::Erfc(binval/rms); 
              chisq -= 2 * TMath::Log(fake_prob);  
              used[bin] = true; 
              cluster.push_back(bin); 
              nadded++; 

            }
          }
        }
      }
      if (nadded == 0) break; 
      current_min -= current_min_increment;
      blurred_current_min -= reproduce_bug ? current_min_increment : blurred_current_min_increment;
 //     blurred_current_min -= blurred_current_min_increment;  // Is this a bug?
    }

    if (cluster.size() < minsize) 
    {
      for (unsigned int i = 0; i < cluster.size(); i++) { used[cluster[i]] = false; } 
      continue; 
    }
    std::cout << "niter: " << niter << std::endl; 
    std::cout << "seedval: " << seedval << std::endl; 
    std::cout << "CLUSTERSIZE BEFORE FILLING IN HOLES: " << cluster.size() << std::endl; 

    //Fill in holes
    for (unsigned int b = 0; b < cluster.size(); b++)
    {
      for (int i = -1; i <=1; i++)
      {
        for (int j = -1; j <=1; j++)
        {
          if (!i && !j) continue; 
          int nb = cluster[b] + i + j*nbinsx; 
          if (nb < nbinsx || nb % nbinsx == 0 || nb % nbinsx == nbinsx-1 || nb >= nbinsx*(nbinsy-1)) continue; 
          if( !used[nb] && numNeighbors(nbinsx, &used, nb) > neighbors_thresh)
          {
            used[nb] = true; 
            cluster.push_back(nb); 
//            blurred_binval = blurred->GetArray()[nb]; 
            fake_prob = TMath::Erfc(binval/rms); 
            chisq -= 2 * TMath::Log(fake_prob);  
          } 
        }  
      }
    }


    std::cout << "CLUSTERSIZE AFTER FILLING IN HOLES: " << cluster.size() << std::endl; 

    //remove peninsulas
    
    while(true)
    {
      int nremoved = 0; 
      
      for (int b = cluster.size()-1; b >=0; b--)
      {
        bin = cluster[b]; 

        if (bin < nbinsx || bin % nbinsx == 0 || bin % nbinsx == nbinsx-1 || bin >= nbinsx*(nbinsy-1)) continue; 
        if ((int) numNeighbors(nbinsx, &used, bin) < min_neighbors)  
        {
          used[bin] = false; 
          nremoved++; 
          cluster.erase(cluster.begin()+b); 
          blurred_binval = blurred->GetBinContent(bin); 
          fake_prob = TMath::Erfc(binval/rms); 
          chisq += 2 * TMath::Log(fake_prob);  
        }
      }
      if (!nremoved) break; 
    }

    std::cout << "CLUSTERSIZE AFTER REMOVING PENINSULAS: " << cluster.size() << std::endl; 

    if (cluster.size() < minsize)
    {
      for (unsigned int i = 0; i < cluster.size(); i++) { used[cluster[i]] = false; } 
      continue; 
    }
    double wrong_p = ROOT::Math::chisquared_cdf_c(chisq, cluster.size() -1);  
//    std:: cout << "WRONG_P: " << wrong_p << std::endl; 
    if (wrong_p > max_wrong_p)
    {
      for (unsigned int i = 0; i < cluster.size(); i++) { used[cluster[i]] = false; } 
      continue; 
    }
    allcluster.push_back(cluster); 
  }

  blurred->Delete(); 
  //std:: cout << "niter: " << niter << std::endl; 
  std::cout << "About to merge. Premerge we have: " << allcluster.size() << std::endl; 

  //Now time to merge! 
  MergeClustersGM(image, allcluster, merge_params);

  for(int j = 0; j < (int) allcluster.size(); j++)
  {
     clust->push_back(allcluster[j]);
  }

  std::cout << allcluster.size() << " cluster(s) found. \n";

  return allcluster.size();      
}

int dmtpc::analysis::clusterfinding::findClustersADHysteresisGM(const TH2 *image, std::vector<std::vector<int> > * clust, 
                             double K, double Lambda, dmtpc::image::processing::ANISOTROPIC_DIFFUSION_FN f,
                             int ntimes, double gradient_sigma, dmtpc::image::processing::GRADIENT_OPERATOR g, 
                             double high_thresh, double low_thresh, unsigned int min_neighbors, 
                             unsigned int minsize, 
                             const GMMergeParams * merge_params) 
{
  TH2 * diffused = (TH2*) image->Clone("copy"); 
        

  //0 out negative bins...
  for (int i = 1; i <= diffused->GetNbinsX(); i++)
  {
    for (int j = 1; j <= diffused->GetNbinsX(); j++)
    {
      if (diffused->GetBinContent(i,j) < 0) diffused->SetBinContent(i,j,0);  
    }
  }
  
  for (int i = 0; i < ntimes -1; i++)
  {
    TH2 *next = anisotropicDiffusion(diffused, Lambda, K, f, gradient_sigma, g); 
    diffused->Delete(); 
    diffused = next; 
  }

  int n =  findClustersGMHysteresis(diffused,clust, high_thresh, low_thresh, min_neighbors, minsize,merge_params, false, false); 

  diffused->Delete(); 

  return n; 
}

int dmtpc::analysis::clusterfinding::findClustersGMHysteresis
                            (const TH2 *image, std::vector<std::vector<int> > * clust, 
                             double high_thresh, double low_thresh, 
                             unsigned int min_neighbors, unsigned int minsize, 
                             const GMMergeParams * merge_params, bool allow_edge_seed, bool allowOutliers) 
{
  double mean, rms; 

  if (allowOutliers) 
  {
    mean = dmtpc::image::calculations::getMean(image); 
    rms = dmtpc::image::calculations::getRMS(image); 
  }
  else
  {
    dmtpc::image::calculations::meanRMSNoOutliers(image,mean,rms); 
  }
  double low = low_thresh*rms+mean;
  double high = high_thresh*rms+mean; 
  if (low < 0) low = rms; 

  std::cout << "High Thresh: " << high << "  ,  Low Thresh: " << low << std::endl; 
  std::vector<std::vector<int> > allcluster; 
  std::vector<int> cluster; 

  const int nbinsx = image->GetNbinsX()+2; 
  const int nbinsy = image->GetNbinsY()+2; 
  const int nbins = nbinsx * nbinsy; 

  std::vector< bool> used(nbins); 
  std::vector<std::pair<double,int> > values(nbins); 

  for (int u = 1; u <= nbinsx; u++)
  {
     for (int v = 1; v <= nbinsy; v++)  
     {
        int bin = image->GetBin(u,v); 
        values.push_back(std::pair<double,int>(image->GetBinContent(bin),bin)); 
     }
  }

  std::sort(values.rbegin(),values.rend()); 

  int niter = 0; 
  while (true)
  {
    cluster.clear(); 
    double binval = values[niter].first; 
    std::cout << "binval: " << binval << std::endl; 
    if (binval < high) break; 
    int bin = values[niter++].second; 
    if (used[bin]) continue; 


    if (!allow_edge_seed &&  (bin / nbinsx <= 1 || bin / nbinsx >= nbinsy - 2|| bin % nbinsx <= 1 || bin % nbinsx >= nbinsx -2)) 
      continue; 
    used[bin] = true; 

    cluster.push_back(bin); 

    for (unsigned int b = 0; b < cluster.size(); b++)
    {
        int xc,yc; 
        xc = cluster[b] % nbinsx; 
        yc = ((cluster[b]-xc)/nbinsx)%nbinsy; 

        for (int i = xc -1; i<=xc+1;i++)
        {
          for (int j = yc-1; j<=yc+1; j++)
          {
            if (i==xc && j==yc) continue; 
            bin = image->GetBin(i,j);  
            if (used[bin]) continue; 
            binval = image->GetBinContent(bin); 

            if (binval > low)
            {
              used[bin] = true; 
              cluster.push_back(bin); 
            }
          }
        }
    }

    if (cluster.size() < minsize) 
    {
      for (unsigned int i = 0; i < cluster.size(); i++) { used[cluster[i]] = false; } 
      continue; 
    }

   //Fill holes 
    for (unsigned int b = 0; b < cluster.size(); b++)
    {
      for (int i = -1; i <=1; i++)
      {
        for (int j = -1; j <=1; j++)
        {
          if (!i && !j) continue; 
          int nb = cluster[b] + i + j*nbinsx; 
          if (nb < nbinsx || nb % nbinsx == 0 || nb % nbinsx == nbinsx-1 || nb >= nbinsx*(nbinsy-1)) continue; 
          if( !used[nb] && numNeighbors(nbinsx, &used, nb) > min_neighbors)
          {
            used[nb] = true; 
            cluster.push_back(nb); 
          } 
        }  
      }
    }


    allcluster.push_back(cluster); 

  } 

 
  MergeClustersGM(image, allcluster, merge_params);

  for(int j = 0; j < (int) allcluster.size(); j++)
  {
     clust->push_back(allcluster[j]);
  }

  std::cout << allcluster.size() << " cluster(s) found. \n";

  return allcluster.size();      
}

int dmtpc::analysis::clusterfinding::findClustersGMLegacy(const TH2 * image, std::vector<std::vector<int> > *clust,
                                     double minsig, double maxsig, 
                                     unsigned int minsize,
                                     const GMMergeParams * merge_params)  
{
  double mean, rms; 
  dmtpc::image::calculations::meanRMSNoOutliers(image,mean,rms); 
  double min = minsig*rms+mean;
  double max = maxsig*rms+mean; 
  if (min < 0) min = rms; 

  std::vector< std::vector <int> > allcluster; 
  std::vector<int> cluster; 
  unsigned int isearch = 0; 

  TH2 * copy = (TH2*) image->Clone("copy");  

  int nbinsx = copy->GetNbinsX()+2; 
  int nbinsy = copy->GetNbinsY()+2; 

  bool done = false; 
  int ntries =0; 

  while (!done)
  {
    cluster.clear(); 
    isearch = 0; 
    int bin = copy->GetMaximumBin(); 

    if (copy->GetMaximum() > min && copy->GetMaximum() < max)
    {
      cluster.push_back(bin); 
    }
    copy->SetBinContent(bin,0.0); 
    if (++ntries > 32) done=true; 

    while (isearch < cluster.size())
    {
      int xc,yc;           
      xc = cluster[isearch] % nbinsx; 
      yc = ((cluster[isearch]-xc)/nbinsx)%nbinsy; 
      for (int i = xc -1; i<=xc+1;i++)
      {
        for (int j = yc-1; j<=yc+1; j++)
        {
          if (copy->GetBinContent(i,j) > max) 
            copy->SetBinContent(i,j,0); 
          if (copy->GetBinContent(i,j) > min)
          {
            bin = copy->GetBin(i,j);   
            cluster.push_back(bin); 
            copy->SetBinContent(i,j,0.0);
          }
        }
      }

      isearch++; 
    }

    std:: cout << "CLUSTERSIZE: " << cluster.size() << std::endl; 

    //add skirt 
    if (cluster.size() > minsize)  
    {
      for (int n = 0; n < 0; n++)
      {
        unsigned int currentsize = cluster.size();              
        for (unsigned int q; q < currentsize; q++)
        {
          int xc = cluster[q]%nbinsx;
          int yc = ((cluster[q]-xc)/nbinsx)%nbinsy; 
          for (int i = xc-1; i<=xc+1; i++)
          {
            for(int j=yc-1; j<=yc+1;j++)
            {
              if (copy->GetBinContent(i,j) > 0 && copy->GetBinContent(i,j) <=min)
              {
                bin = copy->GetBin(i,j);
                cluster.push_back(bin); 
                copy->SetBinContent(i,j,0.);
              }
            }
          }
        }
      }

      allcluster.push_back(cluster);
    }


    if (copy->GetMaximum() < min)  done=true;
  } //while !done


  std::cout << "About to merge. Premerge we have: " << allcluster.size() << std::endl; 

  //Now time to merge! 
  MergeClustersGM(image, allcluster, merge_params);

  for(int j = 0; j < (int) allcluster.size(); j++)
  {
     clust->push_back(allcluster[j]);
  }

  std::cout << allcluster.size() << " cluster(s) found. \n";


  delete copy;

  return allcluster.size();      
}


int dmtpc::analysis::clusterfinding::findClustersLegacy(TH2* image, std::vector<std::vector<int> >* clust, double minsig, double maxsig, int minsize, double mindist)
{
   double mean, rms;
   
   //find threshhold
   dmtpc::image::calculations::meanRMSNoOutliers(image, mean, rms);
   double min = minsig*rms+mean;
   double max = maxsig*rms+mean;
   //double max = 1000;
   if (min < 0) 
   {
      min=rms;
   }

   //cout << mean << "," << rms << ":" << min << "," << max << endl;
   
   std::cout << "Maximum: " << max << std::endl;

//    TCanvas* d = new TCanvas("d","d",0,0,800,800);
//    d->cd();

   std::vector< std::vector <int> > allcluster;

   std::vector<int> cluster;
   int isearch=0;
   TH2* copy = (TH2*)image->Clone("copy");
   //TH2F* show = (TH2F*)image->Clone("show");

   int nbinsx = copy->GetNbinsX()+2;
   int nbinsy = copy->GetNbinsY()+2;
   
   bool done = false;
   
   int ntries=0;
   while(!done){
      
      cluster.clear();
      isearch=0;
      int bin;
      bin = copy->GetMaximumBin();

      std::cout << "Max Bin Val: " << copy->GetBinContent(bin) << std::endl;
         
      if(copy->GetMaximum() > min && copy->GetMaximum() < max)
      {
         cluster.push_back(bin);
      }
      copy->SetBinContent(bin,0.0);

      ntries++;
      if(ntries > 32) done=true;

      while(isearch < int(cluster.size()))
      {
//         cout << "inside loop" << endl;

         int xc,yc;
         xc = cluster[isearch]%nbinsx;
         yc = ((cluster[isearch]-xc)/nbinsx)%nbinsy;
         
         for(int i=xc-1; i<=xc+1;i++)
         {
            for(int j=yc-1; j<=yc+1;j++)
            {
               if(copy->GetBinContent(i,j) > max) copy->SetBinContent(i,j,0);
               if(copy->GetBinContent(i,j) > min)
               {
                  bin = copy->GetBin(i,j);
                  cluster.push_back(bin);
                  copy->SetBinContent(i,j,0.0);
               }
            }
         }
         isearch++;
      }

      
      if(int(cluster.size()) > minsize) 
      {
         for(int n=0; n<0; n++)
         {
            int currentsize=cluster.size();
            for(int q=0;q<currentsize;q++) //add edges to cluster
            {
               int xc,yc;
               xc = cluster[q]%nbinsx;
               yc = ((cluster[q]-xc)/nbinsx)%nbinsy;
               for(int i=xc-1; i<=xc+1;i++)
               {
                  for(int j=yc-1; j<=yc+1;j++)
                  {
                     if(copy->GetBinContent(i,j) > 0 &&     //leave out bins
                        copy->GetBinContent(i,j) <= min)    //already found
                     {
                        bin = copy->GetBin(i,j);
                        cluster.push_back(bin);
                        copy->SetBinContent(i,j,0.0);
                     }
                  }
               }
            }
         }
         
         allcluster.push_back(cluster);
//          pxcounter-=5;
//          show->Draw("colz");
//          d->Update();
      }
      
      if(copy->GetMaximum() < min) {done=true;}

   }
   
//   cout << "N Clusters: " << allcluster.size() << endl;
   
   for(int i=0; i<int(allcluster.size()); i++)
   {
      for(int j=0; j<int(allcluster.size()); j++)
      {
         double dist = dmtpc::image::pixel::minDist(allcluster[i],allcluster[j],image);

         if(i > int(allcluster.size()-1) || j > int(allcluster.size()-1)) 
            continue;
         
         if(dist<=mindist && (i!=j))
         {
            allcluster[i].insert(allcluster[i].end(),allcluster[j].begin(),
                                 allcluster[j].end());
            allcluster.erase(allcluster.begin()+j);
            j--;
         }

      }

   }


   for(int j=0; j<int(allcluster.size()); j++)
   {
      clust->push_back(allcluster[j]);
   }

   std::cout << allcluster.size() << " cluster(s) found. \n";

   delete copy;
   
   return allcluster.size();      

}




