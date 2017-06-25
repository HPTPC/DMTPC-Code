// if eveId<0 then read only this one event, counting from 0
// if eveID=0 read all events
// if eveID >0  read first N events
// 'loc' changes with location: loc=0:my VM, loc=1:m3daq

void rdRaw2ClustSeed( int eveId=0, float nSigThr2=3.,TString coreName="m3_neutron_R1064082" ,TString outPath="./out1/", int loc=1){  

  //coreName="m3_Michael_R1046021"
  TString inpPath="~/data/2016/AmBe/";
  int userReb=4;   //degrade CCD resolution in software

  int runMonth=1;   
  char *pc=strstr(coreName.Data(),"_R"); 
  if (pc) { // 2016+ data
    int runIdx=atoi(pc+2);
    int runDay= (runIdx/1000)%1000;
    if(runDay>31) runMonth=2;
    if(runDay>60) runMonth=3;
  } 

  // goal=28:  7*4, 8*3, 14*2
  // goal=20:  5*4, 7*3, 10*2
  // goal=12:  3*4, 4*3,  6*2


  char *pc=strstr(coreName.Data(),"alpha_"); 
  if (pc) { // 2015 old alpha data
    int runIdx=atoi(pc+8);
    userReb=5;  // * 4x4
    if(runIdx>300) userReb=7; // * 3x3
    if(runIdx>=732) userReb=10; // * 2x2
    printf("run=%d userReb=%d\n", runIdx,userReb);    
    if(loc==1) loc=2;
    if(loc==0) {
      inpPath="/home/balewski/data/2015/"; // @ VM
      inpFile=inpPath+coreName+".root";
    }
  } 

  TString calPath="doneCalib/";
  TString outTreePath=outPath;

  enum { mxCam=1,mxQuad=4};

  gStyle->SetOptStat(10);
  can=new TCanvas("aa","aa",820,800);
  gPad->SetRightMargin(0.15);

  assert(gSystem->Load("$DMTPC_HOME/DmtpcCore/lib/libDmtpcCore.so")==0); 
  assert(gSystem->Load("../DmtpcJanEve/lib/libM3Event.so")==0);
  assert(gSystem->Load("./lib/libJanAna2016.so")==0);

  if(loc==1) inpPath=Form("/data/2016/%02d/raw/",runMonth); // @ m3daq
  TString inpFile=inpPath+coreName+".raw.root";

  if(loc==2) {
    inpPath="/data/AlphaData/"; // @ m3daq
    inpFile=inpPath+coreName+".root";
  }

  M3Event m3Eve;
  // prepare output event container
  TString treeName=outTreePath+"/"+coreName+".m3tree.root";
  TFile*  mHfile=new TFile(treeName,"RECREATE"," histograms & trees M3 events");
  assert(mHfile->IsOpen());

  // create tree file and event-branch
  mTree = new TTree("M3jan2016","M3 Dmtpc events");
  mTree->Branch("M3Eve",&m3Eve);
  
  dmtpc::core::Dataset *ds = new dmtpc::core::Dataset; 
  ds->open(inpFile); 
  int nExpo=ds->nevents();  
  printf("M:Opened=%s  nBias=%d  nExpo=%d  \n", inpFile.Data(), ds->nbias(),nExpo);
  assert( nExpo >0);  
  ds->getEvent(0); // Load first event to get  CCD data format  
  int runId=ds->event()->run();
  int nCamDaq=ds->event()->nccd();
  printf("M:runId=%d, nCamDaq=%d\n",runId, nCamDaq);

  dmtpc::jana::CcdCalibMaker calMk[mxCam];
  dmtpc::jana::CcdClusterMaker cluMk[mxCam];
  dmtpc::jana::M3FrameQA  frameQA[mxQuad];

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

    //..... cluster finder
    cluMk[iCam].setJanEve(&m3Eve,&calMk[iCam].ccdData);
    cluMk[iCam].setNsigThres12(nSigThr2/2., nSigThr2); //defines pix thres = ped+nSig*rms
    // min cell count; min light (adu), min diameter (pix), pcaRatio L,H
    cluMk[iCam].setCuts(8,500,32,0.02,0.12);  // , short tracks
    //cluMk[iCam].setCuts(10,100,160,0.0,0.2);  // , long alpha trcks


    // define fiducal volume limits in pixels, only known for cam2,cam3
    // (pix) band,  >0 in, <0 out, =0 off
    cluMk[iCam].setFiducial(60);  // numPix inward TPC
    //unixtime: 03/03/2016 @ 1:00am UTC =1456966800
    cluMk[iCam].setCapmaignTo(1456966800+16*3600);
    cluMk[iCam].setDebug(0);
    cluMk[iCam].initHisto();
    cluMk[iCam].setDrawings(can,outPath);
    cluMk[iCam].setCSVreport(outPath+"/"+coreName+Form("_quad%d.clust_ana.csv",quadId)); // output report file
  } 

  // bad frame masking is done always for all 4 quads
  
  for(int iCam=0;iCam < mxQuad; iCam++) {
    int quadId=iCam; 
    //.... previously reject frames
    int numFrame=frameQA[iCam].loadFrameQA(runId,quadId,calPath+pedCoreName+".frameQA.root");
    assert(ds->nevents()==numFrame);
    frameQA[quadId].printCSV();
  } 
 
  int ieve=0; // funny math to decide how many events to read-in
  if(eveId>0) {
      if(nExpo>eveId) nExpo=eveId;
  } else if ( eveId<0 ){
    eveId=-eveId;
    assert(eveId <nExpo);
    ieve=eveId;
    nExpo=eveId+1;
  }

  int time0=time(0);
  int nSkipFr=0;
  /*******  events loop start *******/
  for ( ; ieve <  nExpo; ieve++) {
    m3Eve.clearFrame();

    int isGood=true; // kill frame if spark is in any quad
    for(int iCam=0;iCam < mxQuad; iCam++) {
      if ( frameQA[iCam].isBad(runId,ieve)) {  isGood=false; break;}
    }	  
    if(!isGood) { nSkipFr++; continue;}

    ds->getEvent(ieve);      
    dmtpc::core::Event * eve=ds->event();

    for(int iCam=0;iCam < mxCam; iCam++) {
      calMk[iCam].calibrateImage(eve);
      if( cluMk[iCam].findSeedClusters() <=0) continue;
      if(cluMk[iCam].hasPiSpark()) {isGood=false; break;}
    }
    if(!isGood) { nSkipFr++; continue;}    

    for(int iCam=0;iCam < mxCam; iCam++) {
      if( cluMk[iCam].measureClusters() <=0) continue;
      cluMk[iCam].display();  
      cluMk[iCam].purgeClusters(); 
    }
    mTree->Fill();
    
  }// end-of-frame-loop
  

  int delT=time(0)-time0;
  float rate=-1;
  if(delT>0)rate= ieve/delT;
  printf("tot1 number frames: inp=%d nSkipFr=%d readRate=%.1fHz elapsed time=%.1f minutes\n",ieve,nSkipFr,rate,delT/60.);

  // mHfile->Print();
  mHfile->Write();
  mHfile->Close();
  printf("\n Tree saved -->%s<\n",treeName.Data());
  // return;

  printf("\n exec:    root -l rdWtree.C\'(\"%s\",\"%s\")\'\n\n",coreName.Data(),outTreePath.Data());
  printf("\n exec:    root -l plClustQA.C\'(1,\"%s\",\"%s\")\'\n\n",coreName.Data(),outTreePath.Data());

  TFile *outHfile=new TFile(outPath+"/"+coreName+".m3ana.root","recreate");
  assert(outHfile->IsOpen());
  for(int iCam=0;iCam < mxCam; iCam++) {
    calMk[iCam].saveHisto(outHfile); 
    cluMk[iCam].saveHisto(outHfile); 
    
    frameQA[iCam].producePatcher(outPath+"/"+coreName+Form("Q%d.patch.sh",iCam+3),cluMk[iCam].listPiSpark);
    assert(mxCam==1);
  }
  

}



