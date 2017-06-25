#include "CameraHandler.hh" 
#include "CameraLoader.hh" 
#include "M3GlobalData.hh"
#include <time.h>
#include "TTimeStamp.h" 
#include "Channel.hh"
#include <cmath>

// this is for database progress bar
unsigned image_count;

struct WaitExposeArgs
{
  dmtpc::daq::Camera * cam;
  dmtpc::daq::Buffer * buf; 
};


void * waitExposeThread(void * a) 
{
  WaitExposeArgs * arg = (WaitExposeArgs*) a; 
  arg->cam->waitAcquire(); 
  arg->cam->readOut(arg->buf->getMem()); 
  arg->buf->commit(); 
  return 0; 
}

bool dmtpc::daq::CameraHandler::done() const
{
  critical.Lock(); 
  critical.UnLock(); 
  return done_yet; 
}




bool dmtpc::daq::CameraHandler::empty() const
{
  //check buffers
  for (int i = 0; i < getNCameras(); i++)
  {
    if (!buffers[i]->empty()) return false; 
  }

  return true; 
}

int dmtpc::daq::CameraHandler::setShutterMode(SHUTTER_MODE mode)
{
  for (unsigned i = 0; i < getNCameras(); i++) 
  {
    if (mode == AUTO)
    {
      cameras[i]->setAutoShutter(true); 
    }
    else
    {
      cameras[i]->setAutoShutter(false); 
      mode == ALWAYS_OPEN ? cameras[i]->openShutter() : cameras[i]->closeShutter(); 
    }
  }
}


int dmtpc::daq::CameraHandler::coolerOn()
{
  for (unsigned i = 0; i < getNCameras(); i++) 
  {
    cameras[i]->setCoolerState(true); 
  }
}

int dmtpc::daq::CameraHandler::waitForCool(double eps)
{
  for (unsigned i = 0; i < getNCameras(); i++) 
  {
    while( cameras[i]->readTemperature() > cameras[i]->getTargetTemperature() + eps)
    {
      printf("Camera %s temp is %g, target is %g\n", cameras[i]->getInfo()->serialNumber.Data(),cameras[i]->readTemperature(), cameras[i]->getTargetTemperature()); 
      // to monitor temp of other cameras in the meantime
      for (unsigned j=i+1; j< getNCameras(); j++) cameras[j]->readTemperature(); 
      sleep(2); 
    }
  }
}

struct exposeThreadArgs
{
  int n; 
  std::vector<dmtpc::daq::Camera*> * cameras; 
  std::vector<dmtpc::daq::Buffer*> *buffers;
  std::vector<TThread*> *wait_threads; 
  std::vector<TTimeStamp> *timestamps; 
  bool external_trigger; 
  bool threaded_readout;
  dmtpc::control::DOChannel * trigger_channel; 
  volatile bool *done; 
  volatile bool *cancel;
  TMutex * critical; 
}; 


void * exposeThread (void * arg)
{
    exposeThreadArgs * eargs = (exposeThreadArgs*) arg; 
    int n = eargs->n; 
    std::vector<dmtpc::daq::Camera*> * cameras = eargs->cameras; 
    std::vector<dmtpc::daq::Buffer*> *buffers = eargs->buffers;;
    std::vector<TThread*> *wait_threads = eargs->wait_threads; 
    dmtpc::control::DOChannel * trigger_channel = eargs->trigger_channel; 
    bool external_trigger = eargs->external_trigger; 
    bool threaded_readout = eargs->threaded_readout;
    struct timespec tp; 
    int ncameras = cameras->size(); 


    eargs->timestamps->reserve(eargs->timestamps->size() + n); 
    

    for (unsigned ev = 0; ev < n; ev++)
    {

      if (*eargs->cancel) break; 

      for (unsigned i = 0; i < cameras->size(); i++)
      {
        external_trigger ? cameras->at(i)->arm() : cameras->at(i)->expose(); 
      }

      if (external_trigger) trigger_channel->fire(); 

      clock_gettime(CLOCK_REALTIME,&tp); 
      eargs->timestamps->push_back(TTimeStamp((timespec_t) tp)); 

      if (ev == n-1)
      {
        eargs->critical->Lock();  /* avoid deadlock condition where buffer is filled and consumed before done is marked */ 
      }

      if (threaded_readout)
      {
        for (unsigned i = 0; i < ncameras; i++)
        {
          wait_threads->at(i)->Run(); 
          cameras->at(i)->readTemperature(); 
        }

        for (unsigned i = 0; i < ncameras; i++)
        {
          wait_threads->at(i)->Join(); 
        }
      }
      else
      {
         for (unsigned i = 0; i < ncameras; i++)
         {
           cameras->at(i)->readTemperature(); 
           cameras->at(i)->waitAcquire(); 
           cameras->at(i)->readOut(buffers->at(i)->getMem()); 
           buffers->at(i)->commit(); 
         }

      }
      // save image count into database
     image_count++;
     gM3Data.mySqlOnlM3->command(Form("insert into frame_image_count (count, timeStamp) VALUES (%d,NOW()) ", image_count));
    }

    *(eargs->done) = true; 
    eargs->critical->UnLock();

    delete eargs; 

    return 0; 
}


int dmtpc::daq::CameraHandler::exposeAll(unsigned n) 
{
  done_yet = false; 
  cancel = false; 

  exposeThreadArgs * args = new exposeThreadArgs; 
  args->n = n; 
  args->cameras = &cameras; 
  args->buffers = &buffers; 
  args->wait_threads = &wait_threads; 
  args->external_trigger = external_trigger; 
  args->threaded_readout = threaded_readout; 
  args->trigger_channel = trigger_channel; 
  args->timestamps = &timestamps; 
  args->done = &done_yet; 
  args->cancel =&cancel; 
  args->critical = &critical; 

  if (expose_thread->GetState() == TThread::kRunningState)
  {
    std::cout << "expose thread already running... returning" << std::endl; 
    return 1; 
  }


  return expose_thread->Run(args); 
}

dmtpc::daq::CameraHandler::CameraHandler(const dmtpc::core::RunConfig * runCfg, 
					 const std::vector<dmtpc::core::CameraConfig> * camCfgs)
{
  done_yet = true; 
//  TThread::SetCancelOn(); 
  cameras = CameraLoader::loadCameras(camCfgs); 
  ncam = cameras.size(); 
  image_count=0;

  threaded_readout = runCfg->ccd_threaded_readout; 
  printf("threaded_readout: %d\n", threaded_readout); 
  for (unsigned i = 0; i < getNCameras(); i++) 
  {
    Camera * cam = cameras[i]; 
    cam->setConfig(); 
    buffers.push_back(new Buffer(runCfg->ccd_buffer_size, cam->getBufferSize())); 
    
    if (threaded_readout)
    {
      WaitExposeArgs * arg = new WaitExposeArgs; 
      arg->cam = cam; 
      arg->buf= buffers[i]; 
      wait_threads.push_back(new TThread(waitExposeThread,arg)); 
    }
  }
  //this->cfg = cfg; // MAL do I need to keep a copy of the config somewhere? 
  external_trigger = runCfg->ccd_external_trigger; 
  trigger_channel = 0;//external_trigger ? TODO: dmtpc::control::ChannelFactory::makedio(cfg->getString("ccd_external_trigger_channel_conf")) : 0; 
  expose_thread = new TThread(exposeThread,0,TThread::kHighPriority); 
  lastTimeStamp = 0; 

}


