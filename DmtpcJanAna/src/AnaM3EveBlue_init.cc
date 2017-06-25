#include <TH1.h>
#include <TFile.h>
#include <TLine.h>

#include "AnaM3EveBlue.hh"

//========================
//========================
void AnaM3EveBlue::readScopeData(TString csvFname, int runId) {
  printf("Opening scope pulses data =%s=\n", csvFname.Data());
  FILE *fd=fopen(csvFname.Data(),"r"); //<== open CSV ascii table to read
  assert(fd);
  const int mx=30000;
  char buf[mx];

  float par_pulseAmplL=400; // (mV);
  
  M3ScopeEvent scEve1;
  scEve1.runId=runId;
  M3ScopePulse pulse;

  int lastFrameId=-1;
  int nTotPulse=0;

  while(true) {    
    char * ret=fgets(buf,mx,fd); //<== reading one row of CSV table as a string
    if(ret==0) break ; //<== EOF
    if(buf[0]=='#') continue; //<== skip comment line
    char *item=strtok(buf,","); // init 'strtok'
    int i=1;
    int frameId,quadId,pulseId;
    float pulseAmpl, pulseR50, timeInFrame;

    do {
      //printf("i=%d, item=%s=\n",i,item);
      switch (i) {
      case 1:  frameId=atoi(item); break;
      case 2:  pulseId=atoi(item); break;
      case 3:  quadId=atoi(item); break;
      case 4:  sscanf(item,"%f",&pulseAmpl); break;
      case 5:  sscanf(item,"%f",&pulseR50); break;
      case 7:  sscanf(item,"%f",&timeInFrame); break;
      }
      i++;
    } while (item=strtok(0,","));  // advance by one name
    nTotPulse++;
    // if(nTotPulse>3) break;//tmp
    // pack it to new container
    if(lastFrameId!=frameId) {
      if(scEve1.frameId>=0) { // add previous frame, since new few one is observed
	scEveV.push_back(scEve1);
	//	scEve1.print(1);
      }
      scEve1.clearFrame();
      lastFrameId=frameId;
      scEve1.frameId=frameId;
      scEve1.timeStamp= TTimeStamp(frameId,0); // fake filing
    }
    
    pulse.clear();
    pulse.id=pulseId;
    pulse.ampl=pulseAmpl;
    pulse.raise50=pulseR50;
    pulse.timeInFrame=timeInFrame;
    scEve1.pulseV[quadId].push_back(pulse);
    //    printf(" %d %f\n",frameId,pulseAmpl);
    //    if(scEveV.size()>3 ) break;//tmp
  }// end-of-while
  scEveV.push_back(scEve1);// must add the last read-in frame

  printf("  readScopeData end, nScopeEve=%d  nTotPulse=%d\n  last frame:", scEveV.size(),nTotPulse);
  scEveV[scEveV.size()-1].print();
  fclose(fd);

  assert(scEveV.size()>0);
  lastSCEidx=0; // set default as 1'st reported scope event
  scEve=&(scEveV[lastSCEidx]);
  hA[2]->Fill("anyPulse",1.);

  
}

//========================
//========================
void AnaM3EveBlue::saveHisto(TFile *fd) {

  printf(" %s AnaM3EveBlue-histos  are written  to '%s' " ,coreName.Data(),fd->GetName());
  for(int i=0;i<mxH;i++) {
    if(hA[i]) hA[i]->Write();
  }

  printf(" , save Ok\n");
}



//========================
//========================
void AnaM3EveBlue::initHisto( int camid){
  assert(m3Eve);
  m3Eve->assert_quadId(camid);
  quadId=camid;
  coreName=Form("quad%dbl",quadId);
  printf(" AnaM3EveBlue::initHisto  quadId=%d   \n", quadId);
    
  memset(hA,0,sizeof(hA)); // input spectra, unweighted
  TH1* h;
  int mxFrames=500; // no impact on performance

  enum {mxCase=6};

  hA[0]=h=new TH1D(coreName+"_eveCase"," scope+cam frames; type; FRAMES",mxCase,0,mxCase);  
  h->SetTitleSize(0.22);
  h->GetXaxis()->SetTitleOffset(0.4);  h->GetXaxis()->SetLabelSize(0.12);  h->GetXaxis()->SetTitleSize(0.08);
  h->GetYaxis()->SetTitleOffset(0.3); h->GetYaxis()->SetTitleSize(0.08);
  h->SetMinimum(0.8);  h->SetFillColor(29); h->SetLineWidth(2);  h->SetMarkerSize(3);//<-- large text 
    
  hA[1]=h=(TH1D*) hA[0]->Clone();
  h->SetNameTitle(coreName+"_camFrCase"," CCD frames selection");  h->SetFillColor(36);

  hA[2]=h=(TH1D*) hA[0]->Clone();
  h->SetNameTitle(coreName+"_scoFrCase"," Scope pulses selection");  h->SetFillColor(42);

  TString nameA[mxCase]={"anyPair","goodPair","goodRatio","d","e","-"};
  for(int i=0;i<mxCase;i++) {
    hA[0]->Fill(nameA[i],0.);
  }

  TString nameB[mxCase]={"anyClust","andScope","Tok","Aok","one","-"};
  for(int i=0;i<mxCase;i++) {
    hA[1]->Fill(nameB[i],0.);
  }

  TString nameC[mxCase]={"anyPulse","andCam","good","one","d","-"};
  for(int i=0;i<mxCase;i++) {
    hA[2]->Fill(nameC[i],0.);
  }

  // hA: 10 -19  for scope
  hA[10]=h=new TH1D(coreName+"_scoT","Scope time-in-frame , all; time (sec)",140,0,7);  h->SetFillColor(kBlue);
  { TList *tl=h->GetListOfFunctions(); assert(tl);
    float val=par_pulseTimeH;
    TLine *ln=new TLine( val, 0, val, 1e6);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
    val=par_pulseTimeL;
    ln=new TLine( val, 0, val, 1e6);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
  }
  
  hA[11]=h=new TH1D(coreName+"_scoA","Scope amplitude , all; pulse (V)",50,0,1.6);  h->SetFillColor(kBlue);
  { TList *tl=h->GetListOfFunctions(); assert(tl);
    float val=par_pulseAmplL; //  in (V)
    TLine *ln=new TLine( val, 0, val, 1e6);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
  }

  hA[12]=h=new TH2D(coreName+"_scoTR50","Scope raise50-time , all; time (sec); R50(msec)",110,0,5.5,100,0,25);  h->SetFillColor(kBlue);

  //h20-29 : ccd cuts
  float diag2=80;// pixels
  hA[20]=h=new TH1D(coreName+"_cluDiag","cluster diameter , so far; diagonal (ccd pix); clust count", 100,1,diag2);  h->SetFillColor(kBlue);
  { TList *tl=h->GetListOfFunctions(); assert(tl);
    double cutVal=cut_clustDiagPixL;
    TLine *ln=new TLine(cutVal, 0, cutVal, 1e6);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
  }

  hA[21]=h=new TH1D(coreName+"_cluLight","cluster light, so far ; Log10(light/ADU); clust count", 100,1,7);  h->SetFillColor(kBlue);
  { TList *tl=h->GetListOfFunctions(); assert(tl);
    double cutVal=TMath::Log10(cut_clustLightL);
    TLine *ln=new TLine(cutVal, 0, cutVal, 1e6);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
  }

  hA[22]=h=new TH1D(coreName+"_cluPcaR","cluster PCA ratio; I_small/I_large ",100,0,1.); h->SetFillColor(kBlue);
  { TList *tl=h->GetListOfFunctions(); assert(tl);
    double cutVal=cut_pcaRatioH;
    TLine *ln=new TLine(cutVal, 0, cutVal, 1e6);    tl->Add(ln);
    ln->SetLineColor(kMagenta); ln->SetLineStyle(2);
  }
   
  //hA 30+  full event
  hA[30]=h=new TH2D(coreName+"_eveL_P","light-pulseAmpl, final ; light (kADU); pulse ampl (V)", 100,0,20, 100,0,1.6);
  hA[31]=h=new TH2D(coreName+"_eveCMxy","Cluster CM  XY, final, W=1; X pos (pix); Y pos (pix)",93,0,3100,93,0,3100);

  hA[32]=h=new TH1D(coreName+"_scoTok","Scope time-in-frame , final; pulse time (sec)",140,0,7);  h->SetFillColor(kGreen);
  hA[33]=h=new TH1D(coreName+"_scoAok","Scope amplitude ,final; pulse ampl(V)",50,0,1.6);  h->SetFillColor(kGreen);

  hA[34]=h=new TH1D(coreName+"_cluDiagOk","cluster diameter , final; diagonal (ccd pix); clust count", 100,1,diag2);  h->SetFillColor(kGreen);
  
  hA[35]=h=new TH1D(coreName+"_cluLgOk","cluster light, final ; Log10(light/ADU); clust count", 100,1,7);  h->SetFillColor(kGreen);

  hA[36]=h=new TH2D(coreName+"_MuOk","multiplicity, final ; Cam clust; Scope pulse",10,-0.5,9.5,10,-0.5,9.5); h->SetFillColor(kGreen);

 }
