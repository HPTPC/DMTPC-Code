
void wrWtree(int nEve=10) { // write TTree  with precompiled class
  assert(gSystem->Load("../DmtpcJanEve/lib/libM3Event.so")==0);
  assert(gSystem->Load("lib/libJanAna2016.so")==0);

  M3Event myEve;
  myEve.clear();
  myEve.print();

  CcdCell cell;
  cell.print();
 
  TString mTreePath="./";
  TString treeName=mTreePath+"toy1.m3tree.root";
  TFile*  mHfile=new TFile(treeName,"RECREATE"," histograms & trees M3 events");
  assert(mHfile->IsOpen());
  // create tree file and event-branch
  mTree = new TTree("M3jan2016","M3 Dmtpc events");
  mTree->Branch("M3Eve",&myEve);

  WeventIO eveWrap; // util class to manipulate m3 event
  int runId=112233;
  for(int k=0;k<nEve;k++) {
    // fill local event container with random data depending on k
    eveWrap.fillEve(&myEve,k,runId);
    myEve.print();
    
    //..... save event in to tree
    mTree->Fill();
  }

  printf("\n\n::::::::::::::::JbToy6DGen::finish() :::::::::::\n");

  //mTree->Show(1);// event id startting from 0
  // mTree->Print(); 
  

 mHfile->Write();
 mHfile->Close();
 printf("\n Tree saved -->%s<\n",treeName.Data());
}
