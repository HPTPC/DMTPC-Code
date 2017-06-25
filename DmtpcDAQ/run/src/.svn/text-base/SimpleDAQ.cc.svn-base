#include "DAQ.hh" 
#include <cstdlib>
#include <assert.h>
#include "tinyxml2.h"
#include <cstdlib>
#include <string>     
#include <ctime>  

#include <TSystem.h>
#include "M3GlobalData.hh" 


int main(int nargs, char ** args) { 
  // check if old outname exist and crash.
  gM3Data.mySqlOnlM3=new TinyMySqlInterface("m3slow.lns.mit.edu", "onlM3",      "dmatter2","seedark");
  gM3Data.mySqlTest =new TinyMySqlInterface("m3slow.lns.mit.edu", "DMTPC_TEST", "dmatter2","seedark");
  
  const char *xmlName=args[1]; 
  const char *outName=args[2];
  int runNo = nargs > 3 ? atoi(args[3]) : 1000000; 
  dmtpc::daq::DAQ daq; 
  int ret=daq.init(xmlName,outName,runNo);
  assert(!ret);

  int ndark = nargs > 4 ? atoi(args[4]) : 1; 
  int nlight = nargs > 5 ? atoi(args[5]) : 5; 

  int tStart=time(0);
  
  // record no frames were taken yet
  gM3Data.mySqlOnlM3->command("insert into frame_image_count  (timeStamp,count) VALUES  (NOW(),0) ");

  //get exposure times for each camera
  int expoTime4sh=0;
  int expoTime1sh=0;
  for (int c=0; c<daq.nCams(); c++)
    {
      if (daq.getCameraConfig(c)->cameraType=="FLIProline") expoTime4sh=daq.getCameraConfig(c)->exposureTime;
      if (daq.getCameraConfig(c)->cameraType=="SICam")      expoTime1sh=daq.getCameraConfig(c)->exposureTime;
    }


  // valid run config was presented, official run start
  gM3Data.mySqlOnlM3->command(Form("insert into run_start  (runId,setNumBias,setNumExpo,expoTime4sh,expoTime1sh,timeStamp,outName) VALUES  (%d,%d,%d,%d,%d,FROM_UNIXTIME(%d),'%s') ",runNo,ndark,nlight,expoTime4sh,expoTime1sh,tStart,outName));

  daq.run(ndark,nlight);
  daq.finish();

  printf(" SimpleDAQ run finished, record DB again\n");
  const char *quality="questionable";
  int numExposures=ndark+nlight;
  const char *remark="produced by SimpleDAQ";

  gM3Data.mySqlOnlM3->command(Form("insert into run_end  (runId, timeStart,  timeStop, quality, numExposures, timeStamp, remark) VALUES  (%d,FROM_UNIXTIME(%d),NOW(),'%s',%d,NOW(),'%s') ",runNo,tStart,quality,numExposures, remark   ));

} 
