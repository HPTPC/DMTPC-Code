#ifndef DECAY_CHAIN_HH
#define DECAY_CHAIN_HH

#include <map>
#include <vector>
#include <string>
#include "TObject.h"
#include <TH1D.h>
#include "TGraph.h"

namespace dmtpc
{
  namespace physics
  {
    class DecayChain : public TObject
    {
      public: 
        DecayChain(){maxtime = 84600; dt = 0.01; binsize = 1;}; 
        virtual ~DecayChain(); 

        /** Read particle from file. Format is:
         *
         *    #comment
         *    PARTICLE_NAME   HALFLIFE    INITIAL_WEIGHT 
         *    {
         *        PRODUCT_NAME    PROBABILITY   ENERGY  PARTICLE_TYPE
         *        PRODUCT_NAME    PROBABILITY   ENERGY  PARTICLE_TYPE 
         *        ... 
         *    }
         *
         */

        DecayChain(const char * file, double maxtime, double binsize = 1, double dt = 0.001); 
     
        enum DecayType
        {
          ALPHA, BETA_MINUS, BETA_PLUS
        };

        /** Add an isotope to the decay chain 
         *
         * @param name the identifier for the 
         * @param initial_weight the weight of this isotope initially. 
         * @param halflife the halflife of this isotope in seconds
         * @param nproducts the number of products of this isotope
         * @param product_probabilities array of decay probabilities of each product
         * @param product_names array of the names of each product 
         * @param decay_Q energy (in keV) released by decay
         * @param decay_type the type of decay
         */
        void addIsotope(const char * name, double initial_weight, double halflife, int nproducts, const double * product_probabilities, const char * const * product_names, const double * decay_Q,  const DecayType * decay_type); 
        void setMaxTime(double t) { if (!init) maxtime = t; } 
        void setBinSize(double t) { if (!init) binsize = t; } 
        void setDt(double t) { if (!init) dt = t; } 
        
        /** Performs the numerical solution of the abundance pde's **/ 
        void compute(); 

        /** Get an alpha energy at time t after the initial conditions **/ 
        double getRandomEnergy(double t, DecayType type = ALPHA) const; 
        vector<pair<double,double> > getEnergyProbabilities(double t, DecayType type = ALPHA) const; 
        TGraph getEnergyProbabilitiesTGraph(double t, DecayType type = ALPHA) const; 
        const TH1D * getAbundance(const char * isotope) const; 
        double getAbundance(const char * isotope, double t) const; 

      private:

        map<string,int> isotope_ids; 
        vector<string> isotopes; 
        vector<double> halflives; 
        vector<double> initial; 
        vector<vector<string> > product_names; 
        vector<vector<int> > product_ids; 
        vector<vector<double> > product_probabilities; 
        vector<vector<DecayType> > product_types; 
        vector<vector<double> > product_Q; 
        double total_abundance; 

        bool init; 
        double dt; 
        double binsize; 
        double maxtime; 
        vector<TH1D *> abundances; 
        TH1D * total_alpha; 
        TH1D * total_beta_minus; 
        TH1D * total_beta_plus; 
        ClassDef(DecayChain,1); 
    };

  }
}

#endif
