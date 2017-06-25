void rdRaw2Ana( int eveId=10, int userReb=1, float nSigThr1=1.5, int runId=1029005,   TString outPath="./out/",  int loc=0){  
  // if eveId<0 then read only this one event, counting from 0
  // if eveID=0 read all events
  // if eveID >0  read first N events
  // 'loc' changes with location: loc=0:my VM, loc=1:m3daq

  TString calPath="doneCalib/";

  enum { mxCam=2};

  gStyle->SetOptStat(10);
  can=new TCanvas("aa","aa",820,800);
  gPad->SetRightMargin(0.15);

  assert(gSystem->Load("$DMTPC_HOME/DmtpcCore/lib/libDmtpcCore.so")==0); 
  assert(gSystem->Load("./lib/libJanAnalysis2016.so")==0);

  dmtpc::core::Dataset *ds = new dmtpc::core::Dataset; 
  
#if 0  // 2015:
  TString inpPath="~/data/2015/";
  TString coreName=Form("dmtpc_m3_alpha_%05d",runId); 
  TString inpFile=inpPath+Form("dmtpc_m3_alpha_%05d.root",runId);
#endif

#if 1  // 2016:
  TString inpPath="~/data/2016/AmBe/";
  if(loc==1) {  inpPath="/data/2016/01/raw/"; calPath="doneCalib/";}
  TString coreName=Form("m3_Michael_R%d",runId);
  TString inpFile=inpPath+coreName+".raw.root";
#endif

  ds->open(inpFile); 
  int nExpo=ds->nevents();  
  printf(" Opened=%s  nBias=%d  nExpo=%d  \n", inpFile.Data(), ds->nbias(),nExpo);
  assert( nExpo >0);  
  ds->getEvent(0); // Load first event to get  CCD data format  

  M3JanEvent *jEve=new M3JanEvent;

  dmtpc::ped::M3CcdCalibMaker calMk[mxCam];
  dmtpc::ped::M3CcdClusterMaker cluMk[mxCam];

  TString pedCoreName=coreName;
  //  pedCoreName="m3_Michael_R1019020";
  for(int iCam=0;iCam < mxCam; iCam++) {
    int camId=iCam; 
    int camSrc=iCam;
    if(mxCam==1) { camId=3, camSrc=1;}; // only cam3
    //if(mxCam==1) { camId=2, camSrc=0;}; // only cam2
    if(mxCam==2) { camId=iCam+2, camSrc=iCam;};
    //..... calibrator
    calMk[iCam].setJanEve(jEve,camId,camSrc);
    calMk[iCam].load_pedStat(calPath+pedCoreName+".m3ped.root");
    calMk[iCam].setUserRebin( userReb);// on the top of data binning
    calMk[iCam].initDims( ds);

    //  calMk[iCam].addPixTrace(500,500,nExpo); // not on mesh
    //calMk[iCam].addPixTrace(1500,1500,nExpo); //  
    //calMk[iCam].addPixTrace(2500,2500,nExpo); //  on mesh    

    //  calMk[iCam].addPixCorrel(2187,949,2,0);
    // calMk[iCam].addPixCorrel(2187,949,-2,0);
  
    //..... cluster finder
    cluMk[iCam].setJanEve(jEve,camId);
    cluMk[iCam].setNsigThres12(nSigThr1, 2*nSigThr1); //defines pix thres = ped+nSig*rms
    //r198    
    //cluMk[iCam].setCuts(5,25.,0.25); // min cell count; min diameter (mm), InertiaRatio
    //r1022002 : AmBe
    // min cell count; min light (adu), min diameter (mm), InertiaRatio
    //Assuming 10 ADU/keV is correct, you are rejecting about 30 keV recoils
    //cluMk[iCam].setCuts(5,300,3.,0.25);  //setX
    // cluMk[iCam].setCuts(4,200,1.4,0.60);  //setA - short gam-cam
    cluMk[iCam].setCuts(5,900,3.0,0.33);  //setB1 - longer , NR
    //cluMk[iCam].setCuts(6,500,5.0,0.1);  //setB2 - very long , NR
    //cluMk[iCam].setCuts(6,7000,7.0,0.1);  //setB3 - very long & very bright, NR
    // min cell count; min light (adu), min diameter (mm), InertiaRatio
    cluMk[iCam].setDebug(0);
    // define fiducal volume limits in pixels, only known for cam2,cam3
    // (pix) band,  >0 in, <0 out, =0 off
    // if(camId==2) cluMk[iCam].setFiducial(40); 
    // if(camId==3) cluMk[iCam].setFiducial(40); 
    // cluMk[iCam].setHadrRecCut(600); // accept diag > light/factor, 300 is good
    
    cluMk[iCam].initHisto();
    cluMk[iCam].setDrawings(can,outPath);
    cluMk[iCam].setCSVreport(outPath+"/"+coreName+Form("_cam%d.clust_ana.csv",camId)); // output report file
  } 
 
  int ieve=0;
  if(eveId>0) {
      if(nExpo>eveId) nExpo=eveId;
  } else if ( eveId<0 ){
    eveId=-eveId;
    assert(eveId <nExpo);
    ieve=eveId;
    nExpo=eveId+1;
  }
  int time0=time(0);
  /*******  events loop start *******/
  for ( ; ieve <  nExpo; ieve++) {
    ds->getEvent(ieve);  
    dmtpc::core::Event * eve=ds->event();
    jEve->clearFrame();
    for(int iCam=0;iCam < mxCam; iCam++) {
      calMk[iCam].calibrateImage(eve);
      if( cluMk[iCam].findSeedClusters() <=0) continue;
      if(cluMk[iCam].hasPiSpark()) continue;
      
      if( cluMk[iCam].measureClusters() <=0) continue;
      cluMk[iCam].display();  
     
      //cluMk[iCam].bestTrack();   break;
    }
    
  }
  

  int delT=time(0)-time0;
  float rate=-1;
  if(delT>0)rate= ieve/delT;
  printf("tot1 number frames: inp=%d  readRate=%.1fHz elapsed time=%.1f minutes\n",ieve,rate,delT/60.);
 
  // return;
  TFile *outHfile=new TFile(outPath+"/"+coreName+".m3ana.root","recreate");
  assert(outHfile->IsOpen());
    for(int iCam=0;iCam < mxCam; iCam++) {
      calMk[iCam].saveHisto(outHfile); 
      cluMk[iCam].saveHisto(outHfile); 
    }

  // can->Print(coreName+Form("_cam%d_frame%03d_reb%d.pdf",iCam,eveId, userReb));
}
  //cam3_fr0_cal->Draw("lego1 0");



#if 0 // run198, 1x1 bin, fr127, all w/ adu>60k
calMk.addTrace(742,2082,nExpo);
calMk.addTrace(742,2086,nExpo);
calMk.addTrace(746,2078,nExpo);
calMk.addTrace(746,2082,nExpo);
calMk.addTrace(746,2086,nExpo);
calMk.addTrace(746,2090,nExpo);
calMk.addTrace(746,2094,nExpo);
calMk.addTrace(750,2078,nExpo);
calMk.addTrace(750,2082,nExpo);
calMk.addTrace(750,2086,nExpo);
calMk.addTrace(750,2090,nExpo);
calMk.addTrace(750,2094,nExpo);
calMk.addTrace(754,2078,nExpo);
calMk.addTrace(754,2082,nExpo);
calMk.addTrace(754,2086,nExpo);
calMk.addTrace(754,2090,nExpo);
calMk.addTrace(754,2094,nExpo);
calMk.addTrace(758,2078,nExpo);
calMk.addTrace(758,2082,nExpo);
calMk.addTrace(758,2086,nExpo);
calMk.addTrace(758,2090,nExpo);
calMk.addTrace(758,2094,nExpo);
calMk.addTrace(758,2098,nExpo);
calMk.addTrace(762,2082,nExpo);
calMk.addTrace(762,2086,nExpo);
calMk.addTrace(762,2090,nExpo);
calMk.addTrace(762,2094,nExpo);
#endif
