#include "ScalarDistribution.hh"


static const unsigned nbins = 53; 

static double iso_ambe_bin_centers[] = {
 4.14E-7 , 0.11 , 0.33 , 0.54 , 0.75 , 0.97 , 1.18
, 1.40 , 1.61 , 1.82 , 2.04 , 2.25 , 2.47 , 2.68 , 2.90
, 3.11 , 3.31 , 3.54 , 3.75 , 3.97 , 4.18 , 4.39 , 4.61
, 4.82 , 5.04 , 5.25 , 5.47 , 5.68 , 5.89 , 6.11 , 6.32
, 6.54 , 6.75 , 6.96 , 7.18 , 7.39 , 7.61 , 7.82 , 8.03
, 8.25 , 8.46 , 8.68 , 8.89 , 9.11 , 9.32 , 9.53 , 9.75
, 9.96 , 10.2 , 10.4 , 10.6 , 10.8 , 11.0 }; 


static double iso_ambe_probs[] = {
 1.44E-2 , 3.34E-2 , 3.13E-2 , 2.81E-2 , 2.50E-2 , 2.14E-2
, 1.98E-2 , 1.75E-2 , 1.92E-2 , 2.23E-2 , 2.15E-2 , 2.25E-2 , 2.28E-2
, 2.95E-2 , 3.56E-2 , 3.69E-2 , 3.46E-2 , 3.07E-2 , 3.00E-2 , 2.69E-2
, 2.86E-2 , 3.18E-2 , 3.07E-2 , 3.33E-2 , 3.04E-2 , 2.74E-2 , 2.33E-2
, 2.06E-2 , 1.82E-2 , 1.77E-2 , 2.04E-2 , 1.83E-2 , 1.63E-2 , 1.68E-2
, 1.68E-2 , 1.88E-2 , 1.84E-2 , 1.69E-2 , 1.44E-2 , 9.68E-3 , 6.52E-3
, 4.26E-3 , 3.67E-3 , 3.81E-3 , 5.06E-3 , 6.25E-3 , 5.52E-3 , 4.68E-3
, 3.70E-3 , 2.78E-3 , 1.51E-3 , 3.63E-4 , 0.0 }; 

dmtpc::mc::mctpc::AmBeEnergyDistribution::AmBeEnergyDistribution(ScalarDistribution * redfn, int par)
{
  reduction= redfn; 
  redpar = par; 

  double iso_ambe_bin_lowedges[nbins+1]; 
  iso_ambe_bin_lowedges[0] = 0; 
  iso_ambe_bin_lowedges[nbins] = 11.1;

  for (unsigned i = 0; i < nbins-1; i++)
  {
    iso_ambe_bin_lowedges[i+1] = 0.5 * (iso_ambe_bin_centers[i] + iso_ambe_bin_centers[i+1]); 
  }

  iso_ambe_spectrum = new TH1F("isoambe","Iso Ambe Spectrum", nbins, iso_ambe_bin_lowedges); 

  for (unsigned i = 1; i <= nbins; i++)
  {
    iso_ambe_spectrum->SetBinContent(i, iso_ambe_probs[i-1]); 
  }

}


double dmtpc::mc::mctpc::AmBeEnergyDistribution::value() 
{
  while (true) 
  {
    double  E = 1000 * iso_ambe_spectrum->GetRandom(); //MeV -> keV. 

    if (reduction)
    {
      if (redpar >= 0)
      {
        reduction->setParameter(redpar,E); 
      }

      E-= reduction->value(); 
    }

    if (E > 0) return E; 
  }
}
