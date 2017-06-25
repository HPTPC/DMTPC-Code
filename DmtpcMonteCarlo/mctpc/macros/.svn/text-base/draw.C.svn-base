void draw(const char * file, int n = 20)
{

  double stops[5] = {0,0.34,0.61,0.84,1.0};
  double red[5] = {0.0,0.0,0.87,1.0,0.51};
  double green[5] = {0.0,0.81,1.00,0.2,0.0};
  double blue[5] = {0.51,1.0,0.12,0.0,0.0};
  TColor::CreateGradientColorTable(5,stops,red,green,blue,255);
  gStyle->SetNumberContours(255);



  TFile f(file); 

  TCanvas * c = new TCanvas; 
  c->Divide(5,n/5); 

  for (int i = 0; i < n; i++)
  {

    c->cd(i+1); 
    TH2 * ccd = (TH2*) f.Get(TString::Format("ccd_%d",i)); 
    TH2 * bias = f.Get(TString::Format("bias_%d",i)); 
    ccd->Add(bias,-1); 
    ccd->GetXaxis()->SetRangeUser(512-16,512+48); 
    ccd->GetYaxis()->SetRangeUser(512-24,512+24); 
    ccd->DrawCopy("colz"); 
  }




}
