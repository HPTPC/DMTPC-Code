dmtpc::core::Dataset * d; 

int up= 250; 
int down = -50; 

int show(int i, int cam = 0)
{
  d->getEvent(i); 
  static TH2 * copy = 0;
  if (copy) delete copy; 
  copy =   (TH2*) d->event()->ccdData(cam)->Clone("subtracted"); 
  copy->Add(d->biasAvg(cam),-1); 
  copy->SetMinimum(down); 
  copy->SetMaximum(up); 
  copy->DrawCopy("colz"); 
}

void viewer(int run =1, const char * dir = "out")
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
  d->biasAvg(0); 
  show(0); 
}

void setLimit(int ll, int lu)
{
  up = lu; 
  down = ll; 
}

