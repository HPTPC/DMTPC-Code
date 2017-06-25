#include "ParticleDistribution.hh"
#include "ScalarDistribution.hh"
#include "ToyGenerator.hh"
#include "TChain.h" 
#include "TRandom3.h"


#include "TFile.h"
#include "TTree.h" 


const char * charge_tree_file = "/net/zwicky/dmtpc/cozzyd/projects/DarkMatter/AnalysisFramework/v5/macros/chargefriend.root"; 
const char * fit_file = "/net/zwicky/dmtpc/cozzyd/projects/DarkMatter/AnalysisFramework/v5/2d_ambe_fits.root"; 
const char * ambe_dist_file = "AmBe_energy_spectrum_from_Igal.root"; 

double threshold_x0 = 37; 
double threshold_sigma = 9.7; 



/* kludgey for now */ 
static double quenchingC(double Er, TRandom * rng) 
{
  if (Er > 250) return Er*(0.85 + 0.05*(Er-250)/250.); 
  if (Er > 1000) return Er; 
  double  p0   =     0.475162;
  double  p1   =   0.00552874;
  double  p2   = -2.84367e-05;
  double  p3   =    5.334e-08;

  double width_at_50 = 0.047; 
  double width_at_200 = 0.027; 

  double width = width_at_50 + (width_at_200-width_at_50)/(150.) * (Er - 50);
  double center = p0 + p1 * Er + p2 * Er*Er + p3 * Er*Er*Er; 

  double Q = rng->Gaus(center,width); 
  return Q * Er; 
}

static double quenchingF(double Er, TRandom * rng) 
{
  if (Er > 250) return Er*(0.8 + 0.05*(Er-250)/250.); 
  if (Er > 1250) return Er; 

  double p0 =  0.364493;
  double p1 =  0.00481911 ;
  double p2 = -2.24917e-05 ;
  double p3 =  4.08042e-08 ;

  double width_at_50 = 0.05; 
  double width_at_200 = 0.03; 

  double width = width_at_50 + (width_at_200-width_at_50)/(150.) * (Er - 50);
  double center = p0 + p1 * Er + p2 * Er*Er + p3 * Er*Er*Er; 

  double Q = rng->Gaus(center,width); 

  return Q * Er; 
}

/*
class  UnfoldFn : public ROOT::Math::IParametricFunctionOneDim
{
  public: 


    int ndata; 
    double * data; 

    int nbins; 
    double * Ebins; 
    TTree * tree; 


    double DoEvalPar(const double *xx, const double *pp) const 
    {


     
    }


}

*/

int main(int nargs, char** args) 
{

  const char * out = nargs > 1 ? args[1] : "unfold.root";
  int ntracks = nargs > 2 ? atoi(args[2]) : 100000; 
  int nbins = nargs > 3 ?  atoi(args[3]) : 12; 


  TChain chain("fit"); 
  chain.Add(fit_file); 

  TChain chargetree("charge"); 
  chargetree.Add(charge_tree_file); 
  
  chain.AddFriend(&chargetree); 

  int ninchain = chain.Draw("meshE","meshE>0","goff"); 

  double * recoEis = chain.GetV1();
 
  dmtpc::mc::mctpc::UniformCylinder posdist(0,0,150,0,270);  
  TVector3 source_position(25.4 * 40, 0, 8); 
  TVector3 source_dir(-1,0,0); 


  dmtpc::physics::endf::CrossSection cs("ENDF_CS_n_on_19F","../../DmtpcPhysics/tables/"); 
  cs.readFile(); 
  dmtpc::physics::endf::CrossSection cs2("ENDF_CS_n_on_12C","../../DmtpcPhysics/tables/"); 
  cs2.readFile(); 

  dmtpc::physics::endf::AngularDistribution dcs(200);
  dcs.readFile("ENDF_DCS_n_on_19F","../../DmtpcPhysics/tables/"); 
  dmtpc::physics::endf::AngularDistribution dcs2(200);
  dcs2.readFile("ENDF_DCS_n_on_19F","../../DmtpcPhysics/tables/"); 



  const double Emax = 12000; 
  const double Emin = 0; 
  double Estep = (Emax - Emin) / nbins; 
  
  std::vector<double> Ebins(nbins); 

  TFile fout(out,"RECREATE");

  TTree * measured = new TTree("measured","measured"); 
  double Ei_measured; 
  measured->Branch("Ei",&Ei_measured); 
  for (int i = 0; i < ninchain; i++) { Ei_measured = recoEis[i]; measured->Fill(); }
  measured->Write(); 

  TTree * tree = new TTree("unfold","unfold"); 
  dmtpc::analysis::MCTruth *truth = new dmtpc::analysis::MCTruth; 
  double Ebin = 0; 
  tree->Branch("truth",&truth);
  tree->Branch("Ebin",&Ebin);

  std::vector<std::vector<double> > mcEis(nbins); 
  for (int b =0; b < nbins; b++)
  {
    Ebin = Emin + (b+0.5) * Estep; 
    printf("Generating MC centered at %f\n",Ebin); 
    Ebins[b] = Ebin; 

    dmtpc::mc::mctpc::UniformScalarDistribution edist(Emin+ b*Estep, Emin + (b+1) *Estep); 

    dmtpc::mc::mctpc::CollimatedNeutronSourceDistribution pdist(&source_position, &source_dir, &posdist, 
                                                                &edist, &cs, &dcs, 180,100); 

    dmtpc::mc::mctpc::CollimatedNeutronSourceDistribution pdist2(&source_position, &source_dir, &posdist, 
                                                                &edist, &cs2, &dcs2, 180,100); 
    pdist2.setRecoilMass(dmtpc::mc::mctpc::MASS_12C); 

    pdist.setRecoilELimits(50,600); 
    dmtpc::mc::mctpc::ToyGenerator g(&pdist,9); 

    pdist2.setRecoilELimits(50,600); 
    dmtpc::mc::mctpc::ToyGenerator g2(&pdist2,6); 



    double weight = 4*pdist.getXSIntegral(); 
    double weight2 = pdist2.getXSIntegral(); 
    
    double frac = weight / (weight + weight2); 

    TRandom3 rand(0); 


    mcEis[b].reserve(ntracks); 
    for (int i = 0; i < ntracks; i++)
    {
      
     // printf("%d using ",i); 
      if (rand.Uniform() < frac)
      {
        g.generate(truth); 
        truth->ionization = quenchingF(truth->recoilEnergy,&rand); 
      }
      else
      {
        g2.generate(truth); 
        truth->ionization = quenchingC(truth->recoilEnergy,&rand); 
      }

      truth->ionization = rand.Gaus(truth->ionization, threshold_sigma * sqrt(threshold_x0 / truth->ionization)); 

      if (truth->ionization <  threshold_x0)
      {
        continue; 
      }


      mcEis[b].push_back(truth->ionization); 

      tree->Fill() ; 
    }


  }
  tree->Write(); 

  //Now try to do the minimization




  fout.Close(); 

}
