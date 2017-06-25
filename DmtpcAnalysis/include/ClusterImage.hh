#ifndef DMTPC_ANALYSIS_CLUSTER_IMAGE_HH
#define DMTPC_ANALYSIS_CLUSTER_IMAGE_HH

#include <vector>
#include "TObject.h" 
class TH2; 
class TTimeStamp; 

namespace dmtpc
{
  namespace analysis
  {

    class Cluster : public TObject
    {
      public: 

      std::vector<int> pixels; 
      std::vector<int> reduced_pixels; 
      mutable const TH2 * img; //!
      int i; 

      ClassDef(Cluster,2); 
    }; 

    class ClusterImage : public TObject
    {

      public: 

        ClusterImage() {time=0;img=0;image_rms=-1; }
        ClusterImage(TH2 * img, const TTimeStamp * time); 
        virtual ~ClusterImage(); 
        void addCluster(const Cluster *c);
        void replaceCluster(int i, const Cluster *c);
        void roundValues(bool onlyOutsideClusters = true, double roundTo=1.); 

        void changeHistType(char type) { (void) type; }  //TODO: STUB 
        double getRMS() const; 
        int getNCluster() const {return clusters.size();}
        const Cluster* getCluster(int i) const {clusters[i].img = img; return &clusters[i];}
        const Cluster* operator[](int i) const { return getCluster(i); } 
        const TH2* getImage() const {return img;}

      private:
        TH2 * img; 
        std::vector<Cluster> clusters; 

        double time; //in julian days past the J2000 epoch
        mutable double image_rms; 

        ClassDef(ClusterImage,1); 

    }; 
  }
}

#endif
