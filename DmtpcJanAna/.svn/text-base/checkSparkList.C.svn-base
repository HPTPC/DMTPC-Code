// 'loc' changes with location: loc=0:my VM, loc=1:m3daq

void checkSparkList( TString coreName="m3_neutron_R1064082" , int loc=0){  

  assert(gSystem->Load("$DMTPC_HOME/DmtpcCore/lib/libDmtpcCore.so")==0); 
  assert(gSystem->Load("../DmtpcQuality/lib/libDmtpcQuality.so")==0);
  assert(gSystem->Load("../DmtpcJanEve/lib/libM3Event.so")==0);
  assert(gSystem->Load("./lib/libJanAna2016.so")==0);

  TString calPath="doneCalib/";
  enum {mxQuad=4};


  // extract run number form run-name
  char *pc=strstr(coreName.Data(),"_R"); 
  int runMonth=1;   
  int runId=0;
  if (pc) { // 2016+ data
    runId=atoi(pc+2);
    int runDay= (runId/1000)%1000;
    if(runDay>31) runMonth=2;
    if(runDay>60) runMonth=3;
  } 
  // if(loc==1) inpPath=Form("/data/2016/%02d/raw/",runMonth); // @ m3daq
  // TString inpFile=inpPath+coreName+".raw.root";

  // Ric's  spark-DB
  TString db_dir = Form("/Calibration/2016/03/Quality/final/");
  TString dbName=db_dir+"spark-neutX123-v1.db";

  WeventIO  wUtil;
  std::set<int> sparkRic=wUtil.testRicSparkTool(runId,dbName);
  printf("M:sparkRic setSize=%d\n", sparkRic.size());

  std::set<int> sparkJan;
  printf("M:sparkJan setSize=%d\n", sparkJan.size());
  dmtpc::jana::M3FrameQA  frameQA[mxQuad];
  // is done always for all 4 quads
  int numFrame=0;
  for(int iCam=0;iCam < mxQuad; iCam++) {
    int quadId=iCam;
    //.... previously reject frames   
    numFrame=frameQA[quadId].loadFrameQA(runId,quadId,calPath+coreName+".frameQA.root");
    frameQA[quadId].printCSV();
    frameQA[quadId].addTo(&sparkJan);
  }
  printf("M2:sparkJan setSize=%d\n", sparkJan.size());
  return;
 
  // cross-check frames
  int nJ=0, nR=0, nJR=0;
  for(int fr=0; fr<numFrame; fr++) {
    auto search = sparkRic.find(fr);
    int ricSpark=0; 
    if(search != sparkRic.end()) {
      ricSpark=1;
    }
    
    int janSpark=0;
    for(int iCam=0; iCam<mxQuad; iCam++)
      janSpark+=frameQA[iCam].isBad(runId,fr);
    
    if (janSpark) nJ++;
    if (ricSpark) nR++;    
    char *star=' ';
    if (ricSpark && janSpark) { nJR++;   star='*';} 

    if(!janSpark && !ricSpark) continue;

    printf("fr=%d  janSpark=%d ricSpark=%d  %c\n",
	   fr,janSpark,ricSpark,star);
 }
  
  printf("Spark summary %s  numFrame=%d  sparks: jan=%d ric=%d jan*ric=%d\n",coreName.Data(),numFrame,nJ,nR,nJR);


  return;

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



