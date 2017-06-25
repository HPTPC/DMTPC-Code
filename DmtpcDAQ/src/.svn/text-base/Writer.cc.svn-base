#include "Writer.hh" 
#include <sstream>
#include "TFile.h"
#include "Event.hh"
#include "Stats.hh" 
#include "EventBuilder.hh" 
#include <cstdlib>
#include "WritableDataset.hh" 
#include "CameraHandler.hh" 
#include <cstdlib>


static char * makeName(const dmtpc::core::RunConfig * runCfg)
{
  return strdup(TString::Format("%s", runCfg->output_filename.Data()));
  // legacy: TString::Format("%s/dmtpc_%s_%07d.raw.root", runCfg->out_dir.Data(), runCfg->run_tag.Data(), runCfg->run_num)); 
}


struct writerArgs
{
  const dmtpc::core::RunConfig * runCfg;
  dmtpc::daq::CameraHandler * ccds; 
  dmtpc::daq::EventBuilder * builder; 
  dmtpc::core::WritableDataset * d; 
  volatile bool * flushing; 
}; 


struct darkArgs
{
  dmtpc::daq::CameraHandler * ccds; 
  const dmtpc::core::RunConfig * runCfg;
  dmtpc::core::WritableDataset * d; 
};

void * darkThread(void * arg) 
{

  darkArgs * d = (darkArgs*) arg; 
  int ncam = d->ccds->getNCameras(); 
  void ** raw_img = new void* [ncam]; 
  dmtpc::core::Image ** bias = new dmtpc::core::Image*[ncam]; 
  dmtpc::core::Image ** ov = new dmtpc::core::Image*[ncam]; 

  for (int i = 0; i < ncam; i++)
  {
    raw_img[i] = 0; 
  }

  while (!d->ccds->done() || !d->ccds->empty())
  {
  
    for (int cam = 0; cam < d->ccds->getNCameras();cam++)
    {

       printf("darkthread: waiting for buffer from cam %d\n",cam); fflush(stdout); 
       raw_img[cam] = d->ccds->getBuffer(cam)->pop(raw_img[cam]); 
       dmtpc::core::Image * img = d->ccds->getCamera(cam)->makeImage(raw_img[cam]); 
       //dmtpc::core::Image * os  = d->ccds->getCamera(cam)->getInfo()->digitizeOverscan ? 
       //                   d->ccds->getCamera(cam)->makeOverscan(raw_img[cam]) : 0; 

       bias[cam] = img; 
       ov[cam] = 0; // os; 
    }
    d->ccds->getNextTimeStamp();  //eat a time stamp 

    d->d->addBias((const dmtpc::core::Image**) bias,(const dmtpc::core::Image**) ov); 
  }

  for (int i = 0; i < ncam; i++)
  {
    free(raw_img[i]); 
  }

  delete raw_img;
  delete bias; 
  delete ov; 
  delete d; 

  return 0; 

}

void * writerThread(void * arg) 
{

  writerArgs * w = (writerArgs*) arg; 

  while (!(*(w->flushing)) || !w->builder->finished())
  {
    while (w->builder->available())
    {
      dmtpc::core::Event * ev = w->builder->pop(); 
      printf("adding event to dataset\n"); fflush(stdout); 
      w->d->addEvent(ev); 
      delete ev; 
    }
    sched_yield(); 
  }

  delete w; 

  return 0; 
} 

dmtpc::daq::Writer::Writer(const dmtpc::core::RunConfig * conf, CameraHandler * handler, EventBuilder * builder)
          :runCfg(conf), ccd_handler(handler), builder(builder), th(writerThread,0,TThread::kLowPriority), dark_th(darkThread,0,TThread::kLowPriority), flushing(false)
{
  name = makeName(runCfg); 
  std::ostringstream str; 
  //cfg->print(str); 
  runCfg->print();
  stamps[0] = new TTimeStamp; 
  stamps[1] = new TTimeStamp; 
  d = new dmtpc::core::WritableDataset(name , ccd_handler->getNCameras(), stamps[0], stamps[1], str.str().c_str(), false); 

  d->getFile()->SetCompressionAlgorithm(runCfg->compression_algo); 
  d->getFile()->SetCompressionLevel(runCfg->compression_level); 
}

dmtpc::daq::Writer::~Writer()
{
  printf("Writer::~Writer()\n");
  printf("#output_file %s\n", d->getFile()->GetName());
  d->commit(); 
  delete d; 
  free(name); 
}

int dmtpc::daq::Writer::flush()
{
  flushing = true; 
  long ret = th.Join(); 
  stamps[1]->Set(); 
  return ret; 
}

int dmtpc::daq::Writer::flushDark()
{
  return dark_th.Join(); 
}





int dmtpc::daq::Writer::dark() 
{

  darkArgs *args = new darkArgs; 
  args->runCfg = runCfg; 
  args->ccds = ccd_handler; 
  args->d = d; 

  return dark_th.Run(args); 
}

int dmtpc::daq::Writer::start() 
{

  writerArgs * args = new writerArgs; 
  args->runCfg = runCfg; 
  args->ccds = ccd_handler; 
  args->builder = builder; 
  args->d = d; 
  args->flushing = &flushing; 


  return th.Run(args); 
}

