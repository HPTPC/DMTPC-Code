void rdM3Eblue(TString fName="m3_Michael_R1022003") {  // read TTree  with precompiled class

  assert(gSystem->Load("../DmtpcJanEve/lib/libM3Event.so")==0);
  assert(gSystem->Load("lib/libJanAna2016.so")==0);
  

  // .........   input  event file   .........
  TChain *chain = new TChain("M3jan2016");

  TString mTreePath="./out/";
  TString text1=mTreePath+fName+".m3tree.root";
  chain->Add(text1);
  printf("add tree =%s=\n",text1.Data());
  
  int nEve=(int)chain->GetEntries();
  printf("tot nEve=%d expected in the chain\n",nEve);
  M3Event  *m3Eve=new M3Event;
  assert(chain->SetBranchAddress("M3Eve",&m3Eve)==0);


  // chain->Print();
  //  chain->Show(1); // event id startting from 0

  AnaM3EveBlue  blueAna;

  TH1F *h=new TH1F("hr", "frameID",100,0,0);
  
  int t1=time(0);
  int ie;

  for( ie=0;ie<nEve;ie++) { 
    chain->GetEntry(ie);
    if(ie%5==0)
      printf("--- done eve=%d of %d   eve.frameId=%d\n",ie,nEve,m3Eve->frameId);
    blueAna.unpackEve(m3Eve,h);
    //m3Eve->print(1);
  }

  //myEve->print(); // directly just the last event

  int t2=time(0);
  if(t1==t2) t2++;
  float rate=1.*ie/(t2-t1);
  float nMnts=(t2-t1)/60.;
  printf("sorting done, elapsed rate=%.1f Hz, tot %.1f minutes\n",rate,nMnts);

  h->Draw();
  return ;

}
