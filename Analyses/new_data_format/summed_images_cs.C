int cam=1; // BL quadrant                                                                                                               
unsigned int ev_counter;
unsigned int bias_counter;

void sum(TH2D* sum, int run=5002, unsigned int& counter)
{
  dmtpc::core::Dataset *d = new dmtpc::core::Dataset;
  TString filename = TString::Format("/scratch3/darkmatter/dmtpc/data/m3/2016/04/raw/m3_gain_R%d.raw.root",run);
  //if (run>=1032000) file="../../02/raw/"+filename;
  d->open(filename);

  for (int i=0; i<d->nevents();i++){
    d->getEvent(i);
    TH2I* myImage = d->event()->getImage(cam)->getHist();
    sum->Add(myImage);
    counter++;
  }
}


void summed_images_cs()
{
  gSystem->Load("$DMTPC_HOME/DmtpcCore/lib/libDmtpcCore.so");
  ev_counter=0;
  bias_counter=0;

  // open a data file just to get histo clones                                                                                          
  dmtpc::core::Dataset *d = new dmtpc::core::Dataset;
  d->open("/scratch3/darkmatter/dmtpc/data/m3/2016/04/raw/m3_gain_R1092010.raw.root");
  d->getEvent(0);
  TH2I* myImage = d->event()->getImage(cam)->getHist();
  TH2D* sumImage = (TH2D*)myImage->Clone("sumImage");
  sumImage->Reset();
  TH2D* biasImage = (TH2D*)myImage->Clone("biasImage");
  biasImage->Reset();

  // exposure                                                                                                                           
  // for (int r=1029052; r<=1029057; r++) sum(sumImage,r,ev_counter);
  //for (int r=1030009; r<=1030023; r++) sum(sumImage,r,ev_counter);
  //for (int r=1092001; r<=1092001; r++) sum(sumImage,r,ev_counter);
  for (int r=1092010; r<=1092016; r++) sum(sumImage,r,ev_counter);
  for (int r=1092018; r<=1092025; r++) sum(sumImage,r,ev_counter);
  for (int r=1092026; r<=1092030; r++) sum(sumImage,r,ev_counter);
  for (int r=1093001; r<=1093023; r++) sum(sumImage,r,ev_counter);
  //for (int r=1093004; r<=1093023; r++) sum(sumImage,r,ev_counter);
  for (int r=1094001; r<=1094004; r++) sum(sumImage,r,ev_counter);
  for (int r=1094006; r<=1094024; r++) sum(sumImage,r,ev_counter);
  for (int r=1095001; r<=1095021; r++) sum(sumImage,r,ev_counter);
  for (int r=1095022; r<=1095024; r++) sum(sumImage,r,ev_counter);
  for (int r=1096001; r<=1096023; r++) sum(sumImage,r,ev_counter);
  for (int r=1097001; r<=1097017; r++) sum(sumImage,r,ev_counter);
  for (int r=1097019; r<=1097023; r++) sum(sumImage,r,ev_counter);
  for (int r=1098001; r<=1098016; r++) sum(sumImage,r,ev_counter);
  for (int r=1099001; r<=1099014; r++) sum(sumImage,r,ev_counter);
  for (int r=1099017; r<=1099021; r++) sum(sumImage,r,ev_counter);
  for (int r=1100001; r<=1100013; r++) sum(sumImage,r,ev_counter);
  for (int r=1100014; r<=1100022; r++) sum(sumImage,r,ev_counter);
 
  // voltage-off, shutter closed                                                                                                        
  /*for (int b=1110001; b<=1110010; b++) sum(biasImage,b,bias_counter);
  for (int b=1111015; b<=1111019; b++) sum(biasImage,b,bias_counter);
  for (int b=1112001; b<=1112013; b++) sum(biasImage,b,bias_counter);
  for (int b=1112023; b<=1112025; b++) sum(biasImage,b,bias_counter);
  for (int b=1113001; b<=1113010; b++) sum(biasImage,b,bias_counter);
  for (int b=1114001; b<=1114024; b++) sum(biasImage,b,bias_counter);
  for (int b=1115001; b<=1115004; b++) sum(biasImage,b,bias_counter);
  for (int b=1116001; b<=1116007; b++) sum(biasImage,b,bias_counter);
  for (int b=1117001; b<=1117009; b++) sum(biasImage,b,bias_counter);
  for (int b=1117011; b<=1117016; b++) sum(biasImage,b,bias_counter);
  for (int b=1118001; b<=1118024; b++) sum(biasImage,b,bias_counter);
  for (int b=1119001; b<=1119023; b++) sum(biasImage,b,bias_counter);
  for (int b=1120001; b<=1120017; b++) sum(biasImage,b,bias_counter);
  for (int b=1121007; b<=1121029; b++) sum(biasImage,b,bias_counter);*/
   // average                                                                                                                            
  //sumImage->Scale(1./(double)ev_counter);
  //biasImage->Scale(1./(double)bias_counter);

  int bin = sumImage->FindBin(1500,1500);
  cout << "sumImage bin = " << sumImage->GetBinContent(bin) << endl;
  cout << "total summed events = " << ev_counter << endl;
  //cout << "biasImage bin = " << biasImage->GetBinContent(bin) << endl;
  //cout << "total summed bias = " << bias_counter << endl;

  TFile *file=new TFile("summed_von.root","recreate");
  sumImage->Write();
  //biasImage->Write();
  file->Close();
}

