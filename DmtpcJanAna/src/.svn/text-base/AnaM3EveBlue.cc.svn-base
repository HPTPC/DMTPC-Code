#include <TH1.h>

#include "AnaM3EveBlue.hh"

//========================
//========================
AnaM3EveBlue::AnaM3EveBlue(){
  m3Eve=0;
  scEve=0;
  lastSCEidx=0; // this is ugly
  coreName="fixMe8";

  // cuts
  par_pulseTimeL=0; // (sec)  
  par_pulseTimeH=2.5; // (sec)  
  cut_clustLightL=100; // adu
  cut_clustDiagPixL=6;
  cut_pcaRatioH=0.9;
}

//========================
//========================
int AnaM3EveBlue:: matchScopeFrame(){ // 0 - no match
  assert( m3Eve);
  assert( scEve);
  hA[1]->Fill("anyClust",1.);
  //printf("RR  i%d m=%d s=%d\n", lastSCEidx,m3Eve->runId ,  scEve->runId);scEve->print();
  //printf("AA  i%d m=%d s=%d\n", lastSCEidx,m3Eve->frameId ,  scEve->frameId);
  if( lastSCEidx >= scEveV.size()) return 0;
  assert(scEve->runId == m3Eve->runId );
  //  assert( m3Eve->frameId <=  scEve->frameId);// logic below is messed-up
  

  while( lastSCEidx < scEveV.size()  && 
	 m3Eve->frameId >  scEve->frameId  ) {
    //printf("AB  i%d m=%d s=%d\n", lastSCEidx,m3Eve->frameId ,  scEve->frameId);
    lastSCEidx++;
    hA[2]->Fill("anyPulse",1.);
    scEve=&(scEveV[lastSCEidx]);
  }
  if(m3Eve->frameId == scEve->frameId ) return 1;
  // printf("no scope match for CCD frame=%d, skip CCD image\n",m3Eve->frameId );
  return 0;
}




//========================
//========================
void AnaM3EveBlue::evalScopeFrameSolo(){
  int nTok=0, nAok=0;
  M3ScopePulse * myPulse=0;
  for( int ip=0;ip<scEve->pulseV[ quadId].size(); ip++) {
    M3ScopePulse *pulse=&(scEve->pulseV[quadId][ip]);
    hA[10]->Fill(pulse->timeInFrame);
    // printf("XX ip=%d T=%f  fr=%d\n",ip,pulse->timeInFrame, scEve->frameId);
    if(pulse->timeInFrame  < par_pulseTimeL) continue;
    if(pulse->timeInFrame  > par_pulseTimeH) continue;
    nTok++;
    float ampl_v=pulse->ampl/1000.;
    hA[11]->Fill(ampl_v); // now in (V)
    if(ampl_v >par_pulseAmplL) continue;
    //if(ampl_v <0.6) continue;
    nAok++;
    hA[12]->Fill(pulse->timeInFrame, pulse->raise50/1000.);
    myPulseV.push_back(pulse);
  }  

  if(nTok) hA[1]->Fill("Tok",1.);
  if(nAok && nTok) hA[1]->Fill("Aok",1.);
  if(nAok!=1)    return ;
  hA[1]->Fill("one",1.);

}

//========================
//========================
void  AnaM3EveBlue::evalCamFrameSolo(){
  int isMany=0;
  CcdCluster  *myCluster=0;
  for(int cl=0; cl< m3Eve->clusterV[quadId].size(); cl++) {
    CcdCluster  *cluster=&( m3Eve->clusterV[quadId][cl]);
    if( cluster->light < cut_clustLightL) continue;
    float diagPix=cluster->getDiagonal_bin() * m3Eve->agregRebin;
    if (diagPix<cut_clustDiagPixL) continue;
    double pcaR=cluster->pcaMin/cluster->pcaMax;
    if(pcaR>cut_pcaRatioH) continue;

#if 0 // fiducial cut on xy for cam3
    float cluCMpixRX=3000-cluster->kxBinCM*m3Eve-> agregRebin; 
    float cluCMpixRY=3000-cluster->kyBinCM*m3Eve-> agregRebin; 
    if((cluCMpixRX*cluCMpixRX + cluCMpixRY*cluCMpixRY) >2800*2800) continue;
    if(fabs(cluCMpixRX)<200) continue;
    if(fabs(cluCMpixRY)<200) continue;
#endif

    myClustV.push_back(cluster);
    //printf("evalCam ok:"); cluster->print();
    isMany++;
    hA[20]->Fill(diagPix);
    hA[21]->Fill(log10(cluster->light));
    hA[22]->Fill(pcaR);
  }
  if(isMany==0) return ;
  hA[2]->Fill("good",1.);
  if(isMany==1)  hA[2]->Fill("one",1.);

}

//========================
//========================
void AnaM3EveBlue::analyze() {
  assert(scEve->runId == m3Eve->runId );
  assert( m3Eve->frameId ==  scEve->frameId);
  hA[0]->Fill("anyPair",1.);
  hA[1]->Fill("andScope",1.);
  hA[2]->Fill("andCam",1.);
  
  evalScopeFrameSolo();
 
  evalCamFrameSolo();

  if ( myPulseV.size()<=0 || myClustV.size()<=0) return;
  hA[0]->Fill("goodPair",1.);

  hA[36]->Fill( myClustV.size(), myPulseV.size());
  int nRok=0;
  for(int pu=0; pu< myPulseV.size(); pu++)
  for(int cl=0; cl<myClustV.size(); cl++) {
    float pulAmpl=myPulseV[pu]->ampl;
    float pulTime=myPulseV[pu]->timeInFrame;
    float cluLight=myClustV[cl]->light;
    //    myClustV[cl]->print(); myPulseV[pu]->print();    assert(1==2);
    float cluCMpixX=myClustV[cl]->kxBinCM*m3Eve-> agregRebin; 
    float cluCMpixY=myClustV[cl]->kyBinCM*m3Eve-> agregRebin; 
    float diagPix=myClustV[cl]->getDiagonal_bin() * m3Eve->agregRebin;
#if 0 // Ph/L loose correlation
    if( 0.19*light >pulseAmpl) continue;
    if( 0.79*light <pulseAmpl) continue;
    nRok++;
#endif

    hA[30]->Fill(cluLight/1000.,pulAmpl/1000.);
    hA[31]->Fill(cluCMpixX,cluCMpixY);

    hA[32]->Fill(pulTime);
    hA[33]->Fill(pulAmpl/1000.);

    hA[34]->Fill(diagPix);
    hA[35]->Fill(log10(cluLight));
    
  }
  if(nRok>0) hA[0]->Fill("goodRatio",1.);

}

//========================
//========================
void AnaM3EveBlue::clear() {
  myPulseV.clear();
  myClustV.clear();
}
