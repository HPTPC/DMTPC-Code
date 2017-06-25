// if eveId<0 then read only this one event, counting from 0
// if eveID=0 read all events
// if eveID >0  read first N events
// 'loc' changes with location: loc=0:my VM, loc=1:m3daq

// event-tree is driving analysis, no rejections of spark-frames

void rdRaw2ClustFit( int eveId=0,TString coreName="m3_neutron_R1064082" ,TString inpTreePath="./out1/",TString outPath="./out1/", int loc=1){  

  TString inpPath="~/data/2016/AmBe/";
  int userReb=2;   //degrade CCD resolution in software
  enum { mxCam=1};
  TString calPath="doneCalib-v1/";

  // refine path to input raw data
  char *pc=strstr(coreName.Data(),"_R"); 
  int runMonth=1;   
  if (pc) { // 2016+ data
    int runIdx=atoi(pc+2);
    int runDay= (runIdx/1000)%1000;
    if(runDay>31) runMonth=2;
    if(runDay>60) runMonth=3;
  } 
  if(loc==1) inpPath=Form("/data/2016/%02d/raw/",runMonth); // @ m3daq
  TString inpFile=inpPath+coreName+".raw.root";

  // cluster-display - if desired
  gStyle->SetOptStat(1000010);
  can=new TCanvas("aa","aa",800,850);
  gPad->SetRightMargin(0.15);

  assert(gSystem->Load("$DMTPC_HOME/DmtpcCore/lib/libDmtpcCore.so")==0); 
  assert(gSystem->Load("../DmtpcJanEve/lib/libM3Event.so")==0);
  assert(gSystem->Load("./lib/libJanAna2016.so")==0);
  
  // .........   input  event-tree  file   .........
  TChain *chain = new TChain("M3jan2016");

  TString text1=inpTreePath+"/"+coreName+".m3tree.root";
  chain->Add(text1);
  printf("add tree =%s=\n",text1.Data());
  
  int ntreeEve=(int)chain->GetEntries();
  printf("tot ntreeEve=%d expected in the chain\n",ntreeEve);
  if(ntreeEve<=0) { printf(" Warn: empty tree, quit\n"); return;}
  M3Event  *m3Eve=new M3Event;
  assert(chain->SetBranchAddress("M3Eve",&m3Eve)==0);
  //chain->Print();
  chain->GetEntry(0); // read one tree-event
  
  // .........   input  event-raw  file   .........
  dmtpc::core::Dataset *ds = new dmtpc::core::Dataset; 
  ds->open(inpFile); 
  int nExpo=ds->nevents();  
  printf(" Opened=%s  nBias=%d  nExpo=%d  \n", inpFile.Data(), ds->nbias(),nExpo);
  assert( nExpo >0);  
  ds->getEvent(0); // Load first event to get  CCD data format  
  int runId=ds->event()->run();
  int nCamDaq=ds->event()->nccd();
  printf("raw-eve runId=%d, nCamDaq=%d\n",runId, nCamDaq);
  assert(runId==m3Eve->runId);

  dmtpc::jana::CcdCalibMaker calMk[mxCam];
  dmtpc::jana::CcdTrackMaker traMk[mxCam];
  dmtpc::jana::M3FrameQA  frameQA[mxCam];

  TString pedCoreName=coreName;
  for(int iCam=0;iCam < mxCam; iCam++) {
    int quadId=iCam; 
    int camDaq=iCam;
    if(mxCam==1) { quadId=3, camDaq=1;} // only cam3 when 2 cameras read out
    if(mxCam==2) { quadId=iCam+2, camDaq=iCam;}
    if(nCamDaq==4) camDaq+=2; // there were 4 cameras 
    if(nCamDaq==3) camDaq+=1; // there were 3 cameras 

    //..... calibrator
    calMk[iCam].setQuadId(quadId,camDaq);
    calMk[iCam].load_pedStat(calPath+pedCoreName+".m3ped.root");
    calMk[iCam].setUserRebin( userReb);// on the top of data binning
    calMk[iCam].initDims( ds);

    //..... Track fitter
    traMk[iCam].setJanEve(m3Eve,&calMk[iCam].ccdData);
    // params bandHalf: WidthPix, LengthPix
    traMk[iCam].setCuts(100.,400.);
    traMk[iCam].setDebug(1);
    traMk[iCam].setDrawings(can,outPath);
    traMk[iCam].initHisto();
  } 


  if(eveId>0) {
      if(ntreeEve>eveId) ntreeEve=eveId;
  }

  int iTeve=0, iReve=0; // tree, raw
  int time0=time(0);

  /*******  events loop start *******/
  for ( ; iTeve < ntreeEve; iTeve++) {
    chain->GetEntry(iTeve);
    int frameId_T=m3Eve->frameId;    
    ds->getEvent(frameId_T);      
    dmtpc::core::Event * eve=ds->event();
    int frameId_R=eve->ev();
    assert( frameId_R ==frameId_T );
    
    m3Eve->print(0);
    for(int iCam=0;iCam < mxCam; iCam++) {
      int quadId=calMk[iCam].getQuadId();
      printf(" Fit (some of) %d clusters in  frame %d  quadId=%d\n",m3Eve->getNumClust(quadId),frameId_T,quadId);
      
      calMk[iCam].calibrateImage(eve);
      traMk[iCam].make();
      traMk[iCam].display();
    }// end-of-cam-loop
    
  }// end-of-frame-loop
  

  int delT=time(0)-time0;
  float rate=-1;
  if(delT>0)rate= iTeve/delT;
  printf("tot1 number frames: inp=%d  readRate=%.1fHz elapsed time=%.1f minutes\n",iTeve,rate,delT/60.);


  //  printf("\n exec: XXX   root -l plClustQA.C\'(1,\"%s\",\"%s\")\'\n\n",coreName.Data(),outTreePath.Data());

  TFile *outHfile=new TFile(outPath+"/"+coreName+".m3fit.root","recreate");
  assert(outHfile->IsOpen());
    for(int iCam=0;iCam < mxCam; iCam++) {
      calMk[iCam].saveHisto(outHfile); 
    }

}



