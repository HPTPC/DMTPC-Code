// if eveId<0 then read only this one event, counting from 0
// if eveID=0 read all events
// if eveID >0  read first N events
// 'loc' changes with location: loc=0:my VM, loc=1:m3daq

void rdRaw2CommonPed( int eveId=10,TString coreName="m3_Michael_R1028100" ,TString outPath="./out1/", int loc=0){  

  char *pc=strstr(coreName.Data(),"_R");
  int runId=atoi(pc+2);

  int runDay= (runId/1000)%1000;
  int runMonth=1; if(runDay>31) runMonth=2;
  
  TString calPath="doneCalib/";
  TString outTreePath=outPath;
  int userReb=1;  //degrade CCD resolution in software
  enum { mxCam=1};

  gStyle->SetOptStat(10);
  can=new TCanvas("aa","aa",820,800);
  gPad->SetRightMargin(0.15);

  assert(gSystem->Load("$DMTPC_HOME/DmtpcCore/lib/libDmtpcCore.so")==0); 
  assert(gSystem->Load("../DmtpcJanEve/lib/libM3Event.so")==0);
  assert(gSystem->Load("./lib/libJanAna2016.so")==0);

#if 1  // 2016:
  //TString inpPath="~/data/2016/gain/";
  TString inpPath="~/data/2016/AmBe/";
  if(loc==1) inpPath=Form("/data/2016/%02d/raw/",runMonth); // @ m3daq
  TString inpFile=inpPath+coreName+".raw.root";
#endif
  
  dmtpc::core::Dataset *ds = new dmtpc::core::Dataset; 
  ds->open(inpFile); 
  int nExpo=ds->nevents();  
  printf(" Opened=%s  nBias=%d  nExpo=%d  \n", inpFile.Data(), ds->nbias(),nExpo);
  assert( nExpo >0);  
  ds->getEvent(0); // Load first event to get  CCD data format  

  dmtpc::jana::CcdCalibMaker calMk[mxCam];
  dmtpc::jana::M3FrameQA  frameQA[mxCam];

  TString pedCoreName=coreName;
  for(int iCam=0;iCam < mxCam; iCam++) {
    int quadId=iCam; 
    int camDaq=iCam;
    if(mxCam==1) { quadId=3, camDaq=1;} // only cam3 when 2 cameras read out
    if(mxCam==2) { quadId=iCam+2, camDaq=iCam;}
    if(ds->event()->nccd()==4) camDaq+=2; // there were 4 cameras 

    //.... previously reject frames
    frameQA[iCam].loadFrameQA(runId,quadId,ds->nevents(),calPath+pedCoreName+".frameQA.root");
    frameQA[iCam].printCSV();
 
    //..... calibrator
    calMk[iCam].setQuadId(quadId,camDaq);
    calMk[iCam].load_pedStat(calPath+pedCoreName+".m3ped.root");
    calMk[iCam].setUserRebin( userReb);// on the top of data binning
    calMk[iCam].initDims( ds);

  } 

  // return;

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
  /*******  events loop start *******/
  for ( ; ieve <  nExpo; ieve++) {
    ds->getEvent(ieve);      
    dmtpc::core::Event * eve=ds->event();
    
    for(int iCam=0;iCam < mxCam; iCam++) {
      if ( frameQA[iCam].isBad(runId,ieve)) continue;
      calMk[iCam].calibrateImage(eve);
    }    
  }
  

  int delT=time(0)-time0;
  float rate=-1;
  if(delT>0)rate= ieve/delT;
  printf("tot1 number frames: inp=%d  readRate=%.1fHz elapsed time=%.1f minutes\n",ieve,rate,delT/60.);


  printf("\n exec:    root -l plComPed.C\'(4,\"%s\",\"quad3\",\"%s\")\'\n\n",coreName.Data(),outTreePath.Data());

  TFile *outHfile=new TFile(outPath+"/"+coreName+".comPed.root","recreate");
  assert(outHfile->IsOpen());
  for(int iCam=0;iCam < mxCam; iCam++) {
      calMk[iCam].saveHisto(outHfile); 
    }

}



