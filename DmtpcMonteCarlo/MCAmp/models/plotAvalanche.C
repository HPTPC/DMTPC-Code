void plotAvalanche(const char * file, int i = 0) 
{

  gSystem->Load("$MAXCAM/libMaxCam.so"); 
  DmtpcRootTools::setColorStandard1(); 
  gStyle->SetOptFit(1); 

  TFile f(file); 

  TTree * tree = f.Get("avalanche"); 

  TCanvas * c = new TCanvas("c","c",1000,600); 

  c->Divide(3,2); 

  TCut cut(TString::Format("Entry$==%d",i)); 

  c->cd(1); 

  TH2I *hf = new TH2I("hf","Final e- Positions",100,-1,1,100,-1,1); 
  tree->Draw("xf:yf >> hf",cut,"colz");
  hf->DrawCopy("colz"); 

  c->cd(4); 
  TH2I *hi = new TH2I("hi","Initial  e- Positions",100,-1,1,100,-1,1); 
  tree->Draw("xi:yi >> hi",cut,"colz");
  hi->DrawCopy("colz");

  c->cd(2); 
  TH2I *yz = new TH2I("yz","Y-Z initial  e- Positions",100,-1,1,150,-1,2); 
  tree->Draw("zi:yi >> yz",cut,"colz");
//  c->GetPad(2)->SetLogy(); 
  c->GetPad(2)->SetLogz(); 
  yz->DrawCopy("colz");

  c->cd(5); 
//  c->GetPad(5)->SetLogy(); 
  c->GetPad(5)->SetLogz(); 
  TH2I *xz = new TH2I("xz","X-Z initial  e- Positions",100,-1,1,150,-1,2); 
  tree->Draw("zi:xi >> xz",cut,"colz");
  xz->DrawCopy("colz"); 


  c->cd(3); 
  TH1I * yi = new TH1I("yi","Y initial",100,-1,1); 
  tree->Draw("yi >> yi",cut); 
  yi->Fit("gaus"); 
  yi->DrawCopy();

  c->cd(6); 
  TH1I * xi = new TH1I("xi","X initial",100,-1,1); 
  tree->Draw("xi >> xi",cut); 
  xi->Fit("gaus"); 
  xi->DrawCopy();


 
  c->Show(); 






}
