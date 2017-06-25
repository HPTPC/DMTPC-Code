#include "ClusterImage.hh" 
#include "TTimeStamp.h" 
#include "ImageCalculations.hh"
#include "TH2.h" 
#include "ClusterAlgo.hh"


ClassImp(dmtpc::analysis::ClusterImage); 
ClassImp(dmtpc::analysis::Cluster); 

static double round_nearest(double in, double to)
{

  if (to == 1) 
  {
    int intpart = int(in);
    return in - intpart >= 0.5 ? intpart + 1 : intpart; 
  }

  return round_nearest(in/to,1.) * to; 
}


dmtpc::analysis::ClusterImage::ClusterImage(TH2* image, const TTimeStamp * t)
{

   img = image; 
   if (t) 
   {
     double Julian = (t->GetSec()+1E-9*t->GetNanoSec())/86400.0 + 2440587.5;
     time = Julian-2451544.0;
   }
   else 
   {
     time = 0; 

   }
   image_rms = DBL_MAX; 
}

dmtpc::analysis::ClusterImage::~ClusterImage()
{
  if (img)
    delete img; 
}

void dmtpc::analysis::ClusterImage::addCluster(const Cluster *c)
{
  Cluster newc; 
  newc.pixels = c->pixels; 
  newc.reduced_pixels = c->reduced_pixels; 
  newc.img = img; 
  newc.i = clusters.size(); 
  clusters.push_back(newc); 
}

void dmtpc::analysis::ClusterImage::replaceCluster(int i, const Cluster *c)
{
  Cluster newc; 
  newc.pixels = c->pixels; 
  newc.reduced_pixels = c->reduced_pixels; 
  newc.img = img; 
  clusters[i] = newc; 
}

void dmtpc::analysis::ClusterImage::roundValues(bool outside_clusters, double roundTo)
{

  if (roundTo <= 0) return; 

  for (int x = 1; x <= getImage()->GetNbinsX(); x++)
  {
    for (int y = 1; y <= getImage()->GetNbinsY(); y++)
    {
      int bin = getImage()->GetBin(x,y); 
      
      bool round = true; 
      if (outside_clusters)
      {
        for (int c = 0; c < getNCluster(); c++)
        {
          if (clusteralgo::inCluster(getCluster(c),bin))
          {
            round = false; 
            break; 
          }
        }
      }
      if (!round) continue; 

      img->SetBinContent(bin, round_nearest(getImage()->GetBinContent(bin),roundTo)); 

    }
  }

}

double dmtpc::analysis::ClusterImage::getRMS() const
{

  if (image_rms != DBL_MAX) return image_rms; 

  image_rms = dmtpc::image::calculations::getRMS(img); 
  return image_rms; 
}



