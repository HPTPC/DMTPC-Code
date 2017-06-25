// 'loc' changes with location: loc=0:my VM, loc=1:m3daq

void rdRaw2StarSpark(TString coreName="m3_Michael_R1028100", TString outPath="./doneCalib/", int loc=1){  
  int mxEve=99999;
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

  enum { mxCam=4};
  dmtpc::jana::M3StarSparkDet  sparkDet[mxCam];
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
  printf(" Opened=%s  nBias=%d  nExpo=%d \n", inpFile.Data(), ds->nbias(),nExpo);
  assert( nExpo >0);  

  /* Load first event to get information about CCD data format*/
  ds->getEvent(0);
  int runId=ds->event()->run();
  int nCamDaq=ds->event()->nccd();
  printf("runId=%d, nCamDaq=%d\n",runId, nCamDaq);

  for(int iCam=0;iCam < mxCam; iCam++) {
    int quadId=iCam;
    int camDaq=iCam;
    if(mxCam==1) { quadId=3, camDaq=1;};
    if(mxCam==2) { quadId=iCam+2, camDaq=iCam;};
    //if(nCamDaq==4) camDaq+=2; // there were 4 cameras 
    //if(nCamDaq==3) camDaq+=1; // there were 3 cameras 
    sparkDet[iCam].initDims(ds,quadId,camDaq); 
    frameQA[iCam].createStatus(sparkDet+iCam); // init from scratch
  }

  int time0=time(0);
  printf(" star-spark  accumulation ...\n");
  /*******  events loop start *******/
  for (int ieve=0; ieve<  nExpo; ieve++) {
    if(ieve>mxEve) break;
    ds->getEvent(ieve);  
    dmtpc::core::Event * eve=ds->event(); 
    for(int iCam=0;iCam < mxCam; iCam++) {
      sparkDet[iCam].evalQuad(eve); 
      if(sparkDet[iCam].isFrameBad() ) {
	printf(" ieve=%d iCam=%d quadId=%d  is BAD\n",ieve,iCam,sparkDet[iCam].getQuadId());
	  frameQA[iCam].maskFrame(ieve);
	}
    }
    //if(ieve>20) return;
    //break;  // just one eve
  }
  
  int delT=time(0)-time0;
  float rate=-1;
  if(delT>0)rate= ieve/delT;
  printf("tot number frames: inp=%d  readRate=%.1fHz elapsed time=%.1f minutes\n",ieve,rate,delT/60.);

  TFile *outSfile=new TFile(outPath+"/"+coreName+".frameQA.root","recreate");
  assert(outSfile->IsOpen());

  for(int iCam=0;iCam < mxCam; iCam++) {       
     frameQA[iCam].printCSV();
     frameQA[iCam].saveHisto(outSfile);
     sparkDet[iCam].hHotPix->Write();
  }

  return;

}

