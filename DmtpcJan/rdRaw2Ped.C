// mode=0 do ALL: read raw & accumulate ped, compute ped & stat--> R*
// mode=1 read raw & accumulate ped --> big_R*
// mode=2 read 1 raw + big_R histo , compued ped & stat --> R*
// Note, you need to manulay mv R* to 'archive' directory

// 'loc' changes with location: loc=0:my VM, loc=1:m3daq

void rdRaw2Ped( int runId=1029005, int mode=0, TString outPath="./", int loc=0){  
  int doPixelPlots=0;
 
  assert(gSystem->Load("$DMTPC_HOME/DmtpcCore/lib/libDmtpcCore.so")==0); 
  assert(gSystem->Load("./lib/libJanAnalysis2016.so")==0);

  enum { mxCam=2};
  dmtpc::ped::M3CcdPedMaker pedMk[mxCam];
   
  dmtpc::core::Dataset *ds = new dmtpc::core::Dataset; 
  //TString inpPath="/net/zwicky/dmtpc/balewski/raw-alpha/";

  
#if 1  // 2016:
  TString coreName=Form("m3_Michael_R%d",runId);
  TString inpPath="~/data/2016/AmBe/";
  if(loc==1) inpPath="/data/2016/01/raw/"; // @ m3daq
  TString inpFile=inpPath+coreName+".raw.root";
#endif

#if 0  // 2015:
  TString inpPath="~/data/2015/";
  TString coreName=Form("dmtpc_m3_alpha_%05d",runId); inpPath="~/data/2015/";
  TString inpFile=inpPath+coreName+".root"; 
#endif 

  ds->open(inpFile); 
  int nExpo=ds->nevents();  
  printf(" Opened=%s  nBias=%d  nExpo=%d  mode=%d\n", inpFile.Data(), ds->nbias(),nExpo,mode);
  assert( nExpo >0);  

  /* Load first event to get information about CCD data format*/
  ds->getEvent(0);

  if(mode!=1 && doPixelPlots) {
    TCanvas *can=new TCanvas("aa","bb");
    // pedMk.can=can;// tmp
  }

  for(int iCam=0;iCam < mxCam; iCam++) {
    int camId=iCam;
    int camSrc=iCam;
    if(mxCam==1) { camId=3, camSrc=1;};
    if(mxCam==2) { camId=iCam+2, camSrc=iCam;};
    
    pedMk[iCam].initDims( ds,camId,camSrc); 
  }

  //return;
  int time0=time(0);
  if (mode<2) {
    printf(" pedestal accumulation ...\n");
    /*******  events loop start *******/
    for (int ieve=0; ieve <  nExpo; ieve++) {
      ds->getEvent(ieve);  
      dmtpc::core::Event * eve=ds->event(); 
      for(int iCam=0;iCam < mxCam; iCam++)        
	pedMk[iCam].accumulatePed(eve);
      // break;  // just one eve
    }
    int delT=time(0)-time0;
    float rate=-1;
    if(delT>0)rate= ieve/delT;
    printf("tot number frames: inp=%d  readRate=%.1fHz elapsed time=%.1f minutes\n",ieve,rate,delT/60.);
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
   for(int iCam=0;iCam < mxCam; iCam++)        
     pedMk[iCam].saveHisto(outHfile,mode);

  return;
  //cam3_fr0_cal->Draw("lego1 0");  
}

