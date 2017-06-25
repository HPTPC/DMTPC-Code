

void sum(int run =1, int cam = 0, const char * dir = "out")
{
  double stops[5] = {0,0.34,0.61,0.84,1.0};
  gStyle->SetOptStat(0); 
  double red[5] = {0.0,0.0,0.87,1.0,0.51};
  double green[5] = {0.0,0.81,1.00,0.2,0.0};
  double blue[5] = {0.51,1.0,0.12,0.0,0.0};
  TColor::CreateGradientColorTable(5,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);


  d = new dmtpc::core::Dataset;
  d->open(TString::Format("%s/dmtpc_m3_%05d.root",dir,run)); 
  d->biasAvg(cam); 


  int n = d->nevents(); 
  TH2 * sum = new TH2D("sum","sum", d->event()->ccdData(cam)->GetNbinsX(), 
         d->event()->ccdData(cam)->GetXaxis()->GetXmin(), 
         d->event()->ccdData(cam)->GetXaxis()->GetXmax(), 
         d->event()->ccdData(cam)->GetNbinsY(), 
         d->event()->ccdData(cam)->GetYaxis()->GetXmin(), 
         d->event()->ccdData(cam)->GetYaxis()->GetXmax()); 



  for (int i = 0; i < n; i++)
  {
    d->getEvent(i); 
    sum->Add(d->event()->ccdData(cam)); 
    sum->Add(d->biasAvg(cam),-1); 

  }
  sum->Scale(1./n); 
  sum->SetMinimum(-100); 
  sum->SetMaximum(300); 
  sum->Draw("colz");


  TCanvas *c = new TCanvas("c","",0,0,900,300);
  c->Divide(2,1);
  c->cd(1);
  sum->ProjectionX("ProjX", 0, -1)->Draw();
  c->cd(2);
  sum->ProjectionY("ProjY", 0, -1)->Draw();
  c->Update();
  
}


