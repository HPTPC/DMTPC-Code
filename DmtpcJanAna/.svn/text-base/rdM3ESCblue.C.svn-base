void rdM3ESCblue(int runId=1022003, TString outPath="./out3/") {  // read TTree  with precompiled class

  assert(gSystem->Load("../DmtpcJanEve/lib/libM3Event.so")==0);
  assert(gSystem->Load("lib/libJanAna2016.so")==0);
  int camId=3;

  //TString fName="m3_Michael_R1022003"
  TString coreName=Form("m3_Michael_R%d",runId);

  // .........   input  CCD-TTree file   .........
  TChain *chain = new TChain("M3jan2016");
  TString mTreePath="./out/";
  TString text1=mTreePath+coreName+".m3tree.root";
  chain->Add(text1);
  printf("add tree =%s=\n",text1.Data());
  
  int nEve=(int)chain->GetEntries();
  printf("tot nEve=%d expected in the chain\n",nEve);
  M3Event  *m3Eve=new M3Event;
  assert(chain->SetBranchAddress("M3Eve",&m3Eve)==0);

  //--------  input scope pulses data -----------
  TString scopePath="./scopePulses/";
  TString scopeFname=scopePath+Form("ChargePulses_R%d.csv",runId);

  AnaM3EveBlue  blueAna;
  //clust: min light (adu), min diameter (pix), InertiaRatio
  //blueAna.setClusterCuts(1500,20,0.10); // tight
  blueAna.setClusterCuts(250,11,0.34); // wider then input
  //pulse: timeL(sec), timeH(sec), ampl (V)
  blueAna.setPulseCuts(0.1,2.9, 1.65);  // to cut a-peka use 1.45
  blueAna.setM3Eve(m3Eve);
  blueAna.initHisto(camId);
  blueAna.readScopeData(scopeFname, runId);  
  //return;

  // chain->Print();
  //  chain->Show(1); // event id startting from 0
  
  int t1=time(0);
  int ie, im=0;
  //nEve=12;
  for( ie=0;ie<nEve;ie++) { 
    chain->GetEntry(ie);
    blueAna.clear();
    if(blueAna.matchScopeFrame() ==0 ) continue;
    //printf("GOT MATCH frame=%d\n",m3Eve->frameId);
    im++;
    // blueAna.printEve();
    
    if(im%5==0)
      printf("--- done ccd eve=%d of %d   eve.frameId=%d  im=%d\n",ie,nEve,m3Eve->frameId,im);
    //blueAna.printEve();
    blueAna.analyze();

  }

  //myEve->print(); // directly just the last event

  int t2=time(0);
  if(t1==t2) t2++;
  float rate=1.*ie/(t2-t1);
  float nMnts=(t2-t1)/60.;
  printf("sorting done, elapsed rate=%.1f Hz, tot %.1f minutes\n",rate,nMnts);


 TFile *outHfile=new TFile(outPath+"/"+coreName+".m3blue.root","recreate");
 assert(outHfile->IsOpen());
 blueAna.saveHisto(outHfile); 


}

