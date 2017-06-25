// mode=0 do ALL: read raw & accumulate ped, compute ped & stat--> R*
// mode=1 read raw & accumulate ped --> big_R*
// mode=2 read 1 raw + big_R histo , compued ped & stat --> R*
// Note, you need to manulay mv R* to 'archive' directory

// 'loc' changes with location: loc=0:my VM, loc=1:m3daq

void rdRaw2Ped(TString coreName="m3_Michael_R1028100", int mode=0, TString outPath="./", int loc=-1){  

  enum { mxCam=4};
  
  TString calPath="doneCalib/";
  int doPixelPlots=0;

  int runMonth=1;   
  char *pc=strstr(coreName.Data(),"_R"); 
  if (pc) { // 2016+ data
    int runIdx=atoi(pc+2);
    int runDay= (runIdx/1000)%1000;
    if(runDay>31) runMonth=2;
    if(runDay>60) runMonth=3;
  } 

  char *pc=strstr(coreName.Data(),"alpha_"); 
  if (pc) { // 2015 old alpha data
    if(loc==1) loc=2;
  } 
 
  assert(gSystem->Load("$DMTPC_HOME/DmtpcCore/lib/libDmtpcCore.so")==0); 
  assert(gSystem->Load("../DmtpcJanEve/lib/libM3Event.so")==0);
  assert(gSystem->Load("lib/libJanAna2016.so")==0);


  dmtpc::jana::CcdPedMaker pedMk[mxCam];
  dmtpc::jana::M3FrameQA  frameQA[mxCam];
   
  dmtpc::core::Dataset *ds = new dmtpc::core::Dataset; 

  //TString inpPath="~/data/2016/gain/";
  TString inpPath="~/data/2016/AmBe/";
  if(loc==1) inpPath=Form("/data/2016/%02d/raw/",runMonth); // @ m3daq
  TString inpFile=inpPath+coreName+".raw.root";
  
  if(loc==-1) inpFile="~/data/2015/"+coreName+".root"; // Am-241 data from 2015

  if(loc==2) {
    inpPath="/data/AlphaData/"; // @ m3daq
    inpFile=inpPath+coreName+".root";
  }

  
  ds->open(inpFile); 
  int nExpo=ds->nevents();  
  printf(" Opened=%s  nBias=%d  nExpo=%d  mode=%d\n", inpFile.Data(), ds->nbias(),nExpo,mode);
  assert( nExpo >0);  

  /* Load first event to get information about CCD data format*/
  ds->getEvent(0);
  int runId=ds->event()->run();
  int nCamDaq=ds->event()->nccd();
  printf("runId=%d, nCamDaq=%d\n",runId, nCamDaq);

  TCanvas *can=0;
  if(mode!=1 && doPixelPlots) {
    can=new TCanvas("aa","bb");
  }

  for(int iCam=0;iCam < mxCam; iCam++) {
    int quadId=iCam;
    int camDaq=iCam;
    if(mxCam==1) { quadId=3, camDaq=1;};
    if(mxCam==2) { quadId=iCam+2, camDaq=iCam;};
    // if(nCamDaq==4) camDaq+=2; // there were 4 cameras 
    //if(nCamDaq==3) camDaq+=1; // there were 3 cameras 
    //.... previously reject frames
    frameQA[iCam].loadFrameQA(runId,quadId,calPath+coreName+".frameQA.root");
    frameQA[iCam].printCSV();
    
    pedMk[iCam].can=can;// tmp
    pedMk[iCam].initDims( ds,quadId,camDaq); 
  }


  int time0=time(0);
  if (mode<2) {
    int nSkipFr=0;
    printf(" pedestal accumulation ...\n");
    /*******  events loop start *******/
    for (int ieve=0; ieve<  nExpo; ieve++) {

      int isGood=true; // kill frame if spark is in any quad
      for(int iCam=0;iCam < mxCam; iCam++) {
	if ( frameQA[iCam].isBad(runId,ieve)) {  isGood=false; break;}
      }	  
      if(!isGood) { nSkipFr++; continue;}
      
      ds->getEvent(ieve);  
      dmtpc::core::Event * eve=ds->event(); 
      
      for(int iCam=0;iCam < mxCam; iCam++) {
	pedMk[iCam].accumulatePed(eve);
      }
    }// end-of-frame-loop
    
    int delT=time(0)-time0;
    float rate=-1;
    if(delT>0)rate= ieve/delT;
    printf("tot number frames: inp=%d  nSkipFr=%d readRate=%.1fHz elapsed time=%.1f minutes\n",ieve,nSkipFr, rate,delT/60.);
  } else {
    for(int iCam=0;iCam < mxCam; iCam++) 
      pedMk[iCam].ingest_pedSpecBig(outPath+"/big_"+coreName+".m3ped.root");
  }
  
  if (mode!=1) {// compute pedestals & QA
    // measure time used for computation of pedestals
    time0=time(0);
    int nPix=0;
    for(int iCam=0;iCam < mxCam; iCam++) 
      nPix+=pedMk[iCam].computePed();
    int delT=time(0)-time0;
    float rate=-1;
    if(delT>0)rate= nPix/delT;
    printf("tot num pix%.1f(k)  rate=%.1fkHz, elapsed time=%.0f seconds\n",nPix/1000.,rate/1000.,delT);
  }

  TString txt1=""; if (mode==1) txt1="big_"; 
  TFile *outHfile=new TFile(outPath+"/"+txt1+coreName+".m3ped.root","recreate");
  assert(outHfile->IsOpen());
  for(int iCam=0;iCam < mxCam; iCam++) {       
     pedMk[iCam].saveHisto(outHfile,mode);
  }

  return;
}

