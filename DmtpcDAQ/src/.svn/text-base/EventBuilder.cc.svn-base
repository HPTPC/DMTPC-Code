#include "EventBuilder.hh" 
#include "Buffer.hh"
#include "CameraHandler.hh" 
#include "Stats.hh"
#include "Waveform.hh"
#include "TriggerInfo.hh"
#include "TriggerHandler.hh"
#include "Location.hh"
#include "RunConfig.hh"
#include "DetectorConfig.hh"
#include "ScopeHandler.hh"
#include "WritableEvent.hh" 
#include <iostream>
#include "Scope.hh" 
#include <cstdlib>
#include "TThread.h"
#include <cstdlib>

#define DEBUG 1

/** This thread builds the events, so that event building may be done 
 * independently of raw data acquisition and writing to disk. 
 *
 * The triggering operation and statistics gathering also happens in this thread, 
 * although those are implemented elsewhere. 
 *
 */

struct builderArgs
{
  const dmtpc::core::RunConfig * runCfg;
  dmtpc::daq::TriggerHandler * trigger; 
  std::list<dmtpc::core::Event*>* built; 
  TMutex* build_lock;
  volatile bool * almost_dead_populated; 
  unsigned * almost_dead_idx; 
  unsigned * dead; 
  std::vector<dmtpc::core::Event*> * almost_dead; 

  dmtpc::daq::CameraHandler * ccds; 
  dmtpc::daq::ScopeHandler * scopes; 
  dmtpc::daq::Stats * stats; 
  int run; 
  int event; 
  volatile bool * done; 
};


static size_t count_all( const std::vector<std::deque<dmtpc::core::Waveform *> > * wavequeue) 
{
  size_t size = 0; 
  for (unsigned i = 0; i < wavequeue->size(); i++) 
  {
    size+= wavequeue->at(i).size(); 
  }


  return size; 

}

void * builderThread(void * arg) 
{
  builderArgs * a = (builderArgs*) arg; 

  *(a->done) = false; 
  unsigned ncam = a->ccds->getNCameras(); 

#ifdef DEBUG
  std::cout << "builderThread: ncam = " << ncam << std::endl;
#endif

  unsigned nscopes = a->scopes->getNScopes(); 
#ifdef DEBUG
  std::cout << "builderThread: nscopes = " << nscopes << std::endl;
#endif

  if (! ncam && !nscopes)
  {
    std::cerr << "builderThread: no cameras or scopes... nothing to do! " << std::endl; 
    return 0; 
  }

  std::vector<void*> raw_img(ncam);
  std::vector<void*> raw_scope (nscopes); 
  std::vector<std::deque<dmtpc::core::Waveform *> > wavequeue(nscopes); 

  for (int i = 0; i < ncam; i++)
  {
    raw_img[i] = 0; 
  }

  std::cout << "nscopes = " << nscopes << std::endl;

  for (int i = 0; i < nscopes; i++)
  {
    raw_scope[i] = 0; 
  }

  int i = 0;

  while (!a->ccds->done() || !a->ccds->empty())
  {
    //Here is where we build each event 
    dmtpc::core::WritableEvent * ev = new dmtpc::core::WritableEvent; 

    /* grab the raw camera data and make it into images. */
    for (unsigned cam =  0 ; cam < ncam; cam++)
    {
       printf("grabbing buffer from cam %d\n",cam); fflush(stdout);
       raw_img[cam] = a->ccds->getBuffer(cam)->pop(raw_img[cam]); 

       dmtpc::core::Image * img = a->ccds->getCamera(cam)->makeImage(raw_img[cam]); 

       //dmtpc::core::Image * os  = a->ccds->getCamera(cam)->getInfo()->digitizeOverscan ? 
       //                   a->ccds->getCamera(cam)->makeOverscan(raw_img[cam]) : 0; 

       ev->ccdDataPtr()->push_back(img);

       //ev->overscanPtr()->push_back(os); 
    }

    //we need to match up the scope traces within the timestamps here 
    const TTimeStamp * timestamp = a->ccds->getNextTimeStamp(); 
    //printf("1: EventBuilder timestamp\n");
    //printf("timestamp: %x, %d,%d\n",timestamp, timestamp->GetSec(), timestamp->GetNanoSec()); 
    ev->setTimeStamp(new TTimeStamp(timestamp->GetSec(), timestamp->GetNanoSec())); 

    printf("event: %d\n",i); 
    while (!a->ccds->hasNextTimeStamp() && !a->ccds->done())
    {
        sched_yield(); 
    }

    const TTimeStamp * nexttimestamp = (!a->ccds->hasNextTimeStamp() && a->ccds->done()) ? 0 : a->ccds->peekNextTimeStamp(); 
    /* grab the raw scope data and make it into waveforms.
     *
     *
     * Because a single buffer can contain an arbitrary number of waveforms, we keep a queue of 
     * of the converted waveforms and pop them off when we associate them to an event.
     *
     **/
    //std::cout << "pre loop: a->scopes->done() " << a->scopes->done() << std::endl;

    while (!a->scopes->done() || !a->scopes->allConsumed() || count_all(&wavequeue))  // grab scope traces until there is no more data possibly associable with this event
    {
      int nwithdata = 0; 

      for (unsigned scope = 0; scope < a->scopes->getNScopes(); scope++)
      {
        //printf("grabbing buffer from scope %d\n",scope); 
        
	// Fix by MAL: advances if ccd buffer is at half capacity
	while(!a->scopes->done() && a->scopes->getBuffer(scope)->empty() && (((double)a->ccds->getBuffer(0)->occupancy()/a->runCfg->ccd_buffer_size)<0.5))
	{
           sched_yield(); 
        }

        // Fix by MAL: was
	// if (!(a->scopes->getBuffer(scope)->empty() && a->scopes->done()))
	// but we do not want pop to block if buffer is empty and scopes are not yet done
	if (!a->scopes->getBuffer(scope)->empty())
        {
          raw_scope[scope] = a->scopes->getBuffer(scope)->pop(raw_scope[scope]); 

          a->scopes->getScope(scope)->makeWaveforms(&wavequeue[scope],  
                                                    raw_scope[scope]); 
           
        }
 
        while (wavequeue[scope].size())
        {
//          printf("\nevent: %d. scope: %d.  nexttimestamp available yet? %s\n", i, scope, nexttimestamp ? "yes" :"no"); 
          if (nexttimestamp)
          {
//            printf("nexttimestamp: %u.%09u\n",nexttimestamp->GetSec(), nexttimestamp->GetNanoSec()); 
//            printf("first queue t: %u.%09u\n",wavequeue[scope][0]->getInfo()->secs, wavequeue[scope][0]->getInfo()->nsecs); 
          }

//          fflush(stdout); 
          if (!nexttimestamp || 
              wavequeue[scope][0]->getInfo()->secs < nexttimestamp->GetSec() || 
              (wavequeue[scope][0]->getInfo()->secs == nexttimestamp->GetSec() && wavequeue[scope][0]->getInfo()->nsecs < nexttimestamp->GetNanoSec())
             ) 
          {
            ev->scopeDataPtr()->push_back(wavequeue[scope][0]); 
            wavequeue[scope].pop_front(); 
            nwithdata++; 
          }
          else break;  // we'll process this later 
        }
    
	// Fix by MAL: move outside of while loop, otherwise too many ScopeInfos!
	// ev->scopeConfigsPtr()->push_back(new dmtpc::core::ScopeInfo(*a->scopes->getScope(scope)->getInfo())); 
      }
      if (!nwithdata) 
        break; 
    }

    dmtpc::core::DetectorConfig *dc = new dmtpc::core::DetectorConfig; 
    a->stats->fillDetectorConfig(dc); 
    ev->setDetectorConfig(dc); 
    ev->setLocation(new dmtpc::core::Location(*a->runCfg->getLocation())); 
    ev->setRunNumber(a->runCfg->run_num); 
    ev->setEventNumber(a->event + i);
    for (unsigned scope = 0; scope < a->scopes->getNScopes(); scope++) 
      ev->scopeConfigsPtr()->push_back(new dmtpc::core::ScopeInfo(*a->scopes->getScope(scope)->getInfo()));

    /** Here we check if the event passes the trigger or not. 
     *  If it does not, it is added into the almost dead circular buffer, 
     *  where it may be resurrected if an event close enough in the future
     *  does pass a trigger and the trigger prebuffer is greater than 0. 
     *  Otherwise, it'll be permanently killed when the buffer fills up. 
     *
     *  If it does pass, then any events in the almost dead buffer will be 
     *  added before the passing event. 
     */ 

    printf("evaluating trigger...\n"); fflush(stdout); 
    if (a->trigger->eval(ev))
    {
      printf("...passed\n"); fflush(stdout); 
      a->build_lock->Lock(); 
      if (*a->almost_dead_populated)
      {
        for (unsigned j = 0; j < a->almost_dead->size(); j++)
        {
          int idx =  (*a->almost_dead_idx + 1 + j) % a->almost_dead->size(); 
          dmtpc::core::Event * resurrected = a->almost_dead->at(idx); 
          a->almost_dead->at(idx) = 0; 
          if (resurrected)
          {
            a->built->push_back(resurrected); 
          }

        }
        *a->almost_dead_populated = false; 
      }
      a->built->push_back(ev); 
      a->build_lock->UnLock(); 
    }
    else
    {
      printf("...failed\n"); fflush(stdout); 
      if (a->almost_dead->at(*a->almost_dead_idx))
      {
        //rip 
        delete a->almost_dead->at(*a->almost_dead_idx); 
        (*a->dead)++; 
      }
      *a->almost_dead_populated = true; 
      (*a->almost_dead)[*a->almost_dead_idx] = ev; 
      *a->almost_dead_idx = (*a->almost_dead_idx + 1) % a->almost_dead->size(); 
    }
    i++;

  }

  for (int i = 0; i < ncam; i++)
  {
    free(raw_img[i]); 

  }

  for (int i = 0; i < nscopes; i++)
  {
    free(raw_scope[i]); 

  }




  *(a->done) = true; 
  //std::cout << "EventBuilder line 231 a->done" << a->done << std::endl;
  delete a; 
  return 0; 
}


dmtpc::daq::EventBuilder::EventBuilder(const dmtpc::core::RunConfig * runCfg, 
				       CameraHandler * ccd_handler, ScopeHandler * scope_handler, 
				       TriggerHandler * trigger, Stats * stats) 
{
  this->runCfg = runCfg; 
  ccds =ccd_handler; 
  scopes = scope_handler; 
  this->stats = stats; 
  this->trigger = trigger; 


#ifdef DEBUG
  std::cout << "EventBuilder ctor: prebuffersize: "<< trigger->getPreBufferSize() << std::endl;
  std::cout << "EventBuilder ctor: ncameras: " <<ccds->getNCameras()<< std::endl;
  std::cout << "EventBuilder ctor: nscopes: " <<scopes->getNScopes()<< std::endl;
#endif

  if (trigger->getPreBufferSize())
  {
    almost_dead.reserve(trigger->getPreBufferSize()); 
    for (unsigned i = 0; i < trigger->getPreBufferSize(); i++)
    {
      almost_dead.push_back(0); 
    }
  }

  th = new TThread(builderThread, 0, TThread::kLowPriority); 
}


dmtpc::core::Event* dmtpc::daq::EventBuilder::pop()
{ 
//  printf("pop!\n"); fflush(stdout); 
  build_lock.Lock();
  dmtpc::core::Event * ev =built.front(); 
  built.pop_front();
  build_lock.UnLock();
  return ev;
} 





int dmtpc::daq::EventBuilder::start()
{
  if (th->GetState() == TThread::kRunningState)
  {
    std::cout << "builder thread already running... returning" << std::endl; 
    return -1; 
  }


  builderArgs * a = new builderArgs; 
  a->runCfg = runCfg;
  a->ccds = ccds; 
  a->event = 0; 
  a->scopes = scopes; 
  a->trigger = trigger; 
  a->built = &built; 
  a->build_lock = &build_lock; 
  a->stats = stats; 
  a->almost_dead_populated = &almost_dead_populated; 
  a->dead = &dead; 
  a->almost_dead_idx = &almost_dead_idx; 
  a->almost_dead = &almost_dead; 
  a->done= &done; 

  th->Run(a); 

}


dmtpc::daq::EventBuilder::~EventBuilder() 
{
  if (built.size()) 
  {
    std::cerr << "WARNING: EventBuilder buffer is not empty on destruction!" << std::endl; 
  }
}

