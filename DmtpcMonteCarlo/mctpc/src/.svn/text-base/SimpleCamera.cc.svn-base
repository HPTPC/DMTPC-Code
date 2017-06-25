#include "SimpleCamera.hh"
#include "MCTruth.hh"


static int cam_counter = 0; 
#include <iostream> 

dmtpc::mc::mctpc::SimpleCamera::SimpleCamera(int nx, int ny, int nbin, const char * name, const TH2 * gainmap)
            : hist(name ? name : TString::Format("ccd_%d",cam_counter).Data(),TString::Format("%s Image",name ? name : "CCD"), nx/nbin, 0, nx, ny/nbin, 0, ny),
              biasframe(TString::Format("%s_bias", name ? name : TString::Format("ccd_%d",cam_counter++).Data()),"Biasframe" ,nx/nbin, 0, nx, ny/nbin, 0, ny),
              gainmap(0),nx(nx), ny(ny), nbin(nbin)
{

  scale = 0.16; 
  acc = 7e-4 * 0.9 * 0.9;
  qe = 0.43; 
  gain = 1.4;
  noise = 10;
  bias = 1000; 
  blur = 0; 
  rot = 0; 
  x0 = 0; 
  y0 = 0; 

}

const unsigned nrandpool = 10000; 
static __thread unsigned irandpool= 0; 
static __thread double randpool[nrandpool]; 


static double inline uniform(TRandom3 * rng)
{
  if (!irandpool)
  {
    rng->RndmArray(nrandpool, randpool); 
//    std::cout << "filling pool "<<randpool[0] << std::endl; 
  } 
  double ans = randpool[irandpool++]; 
  if (irandpool == nrandpool) irandpool = 0; 
  return ans; 
}


int dmtpc::mc::mctpc::SimpleCamera::doProcess(unsigned size,  const Particle * in)
{


  for (unsigned i = 0; i <size ; i++)
  {
    Particle  p = in[i];

    if (p.type != Particle::photon) continue; 


    //geometric acceptance and qe 
    if (uniform(&rng) > acc*qe) continue; 


    double xx = (p.x - x0) / scale;
    double yy = (p.y - y0) / scale; 

//    std::cout << xx << " " << yy << std::endl; 

    double xcam = xx * cos(rot) + yy * sin(rot);  //rotate about camera center? 
    double ycam = -xx * sin(rot) + yy * cos(rot); 

    xcam += nx/2; 
    ycam += ny/2; 

    // fov 
    if (xcam < 0 || xcam > nx) continue;
    if (ycam < 0 || ycam > ny) continue;



    if (blur) 
    {
      hist.Fill(rng.Gaus(xcam,blur), rng.Gaus(ycam,blur)); 
    }
    else
    {
      hist.Fill(xcam,ycam); 
    }

  }

  return 0; 
}

void dmtpc::mc::mctpc::SimpleCamera::clearSignals()
{
  Process::clearSignals(); 
  hist.Reset(); 
  biasframe.Reset();

}


void dmtpc::mc::mctpc::SimpleCamera::prepareSignals(dmtpc::analysis::MCTruth * truth)
{

  truth->xscale.push_back(1./scale); 
  truth->yscale.push_back(1./scale); 
  truth->xoffsets.push_back(x0); 
  truth->yoffsets.push_back(y0); 
  truth->noise.push_back(noise); 
  truth->rotation.push_back(rot); 
  truth->conversion_gain.push_back(gain); 
  truth->truth_images.push_back((TH2I*) hist.Clone(TString::Format("%s_truth", hist.GetName()))); 
  truth->nphoton.push_back((int)hist.Integral()); 


  //conversion gain and gain map 
  int prenoiseintegral = 0; 



  int raw_integral = 0; 
  truth->edge = false; 

  //noise 

  for (int i = 1; i <= hist.GetNbinsX(); i++)
  {
    for (int j = 1; j <= hist.GetNbinsY(); j++)
    {

      double content = hist.GetBinContent(i,j); 
      if (content && ( i == 1 || j ==1 || i == hist.GetNbinsX() || j == hist.GetNbinsY())) truth->edge = true; 
      double adjusted_content = content / gain;
      raw_integral += (int) adjusted_content; 
      if (gainmap) adjusted_content *= gainmap->GetBinContent(i,j); 
      prenoiseintegral += (int) adjusted_content; 
      hist.SetBinContent(i,j, rng.Gaus(bias + adjusted_content,noise));  
      biasframe.SetBinContent(i,j,bias); 
    }
  }

  printf("Integral before noise / bias: %d\n" , prenoiseintegral); 
  truth->nadu_raw.push_back(raw_integral); 
  truth->nadu.push_back(prenoiseintegral); 

  signals.push_back(Signal(&hist,2,hist.GetName() )); 
  signals.push_back(Signal(&biasframe,2,biasframe.GetName() )); 

}
