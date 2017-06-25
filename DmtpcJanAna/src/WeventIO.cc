#include <cstdio>
#include <iostream>
using namespace std;

#include <TH1.h>
#include "WeventIO.h"


ClassImp(WeventIO)


//========================
//========================
void WeventIO::fillEve(M3Event *eve, int k, int runId) {  

  eve->clear(); // do not forget it or you get stalle events piled up
  eve->frameId=10+k;
  eve->runId=runId; 
  if(eve->frameId%2==0) return;
  // add one cluster to this frame
  int camId=3;
  eve->assert_quadId(camId); // call it from time to time
  CcdCluster cluster; // working container
  cluster.light=200+rnd.Uniform(50);
  eve->clusterV[camId].push_back(cluster);  
}



//========================
//========================
void WeventIO::unpackEve(M3Event *m3Eve, TH1F *h, int camId){
  assert(h);
  m3Eve->assert_quadId(camId); // call it from time to timealways verify sanity of input
  int nFullClust=0;
 for(int k=0;k<m3Eve->clusterV[camId].size(); k++){
    CcdCluster &cluster= m3Eve->clusterV[camId][k];
    if( cluster.type==0 )  nFullClust++;
 }

 printf("WeventIO::unpackEve() runID=%d  frameId=%d  nAnyClust=%d  nFullClust=%d\n",m3Eve->runId,m3Eve->frameId,m3Eve->clusterV[camId].size(), nFullClust);
  //eve->print(2);
  h->Fill(m3Eve->frameId);
}


#include "../DmtpcQuality/include/RunQuality.hh"
//========================
//========================
std::set<int> WeventIO:: testRicSparkTool(int runId, TString dbName){
 // Ric's  spark-DB
  //---setup RunQuality  
  dmtpc::Quality::RunQuality runQ = dmtpc::Quality::RunQuality(runId,dbName);
  int noInfo= runQ.noInfo();
  printf("extract spark list for run=%d from %s, noInfo=%d, wiat ...\n",runQ.runid(),dbName.Data(),noInfo);

  std::set<int> sparkSet;
  if(noInfo)return sparkSet;
  printf("Ric's sparks are loaded and ready\n");
 //---get only spark events
  std::vector< std::vector< int > > sparkEvents = runQ.getSparkTaggedEvents();

 
  //---print out results
  for(unsigned int ch = 0; ch < sparkEvents.size(); ch++){
    printf("run=%d quad=%d has %d sparks: ",runId, ch,sparkEvents.at(ch).size());
    for(unsigned int ev = 0; ev < sparkEvents.at(ch).size(); ev++) {
      int frameId=sparkEvents.at(ch).at(ev);
      printf("%d, ",frameId);
      sparkSet.insert(frameId);
    }
    printf("\n");
  }

  printf("spark setSize=%d\n", sparkSet.size());

#if 0
  std::cout << "Set contains:";
  int nv=0;
  while (!sparkSet.empty()) {
    std::cout << ' ' << *sparkSet.begin();
    nv++;
    sparkSet.erase(sparkSet.begin());
  }
  std::cout << " END nv="<<nv<<std::endl;

  printf("spark setSize=%d\n", sparkSet.size());
#endif
  
  return sparkSet;
}
