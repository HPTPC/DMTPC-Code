#include "ScopeHandler.hh" 
#include "RunConfig.hh"
#include "ScopeConfig.hh"
#include "ScopeLoader.hh" 
#include "Buffer.hh"
#include "Scope.hh" 
#include "TThread.h" 
#include <iostream>

struct scopeThreadArgs
{
  const dmtpc::core::RunConfig * runCfg;
  const std::vector<dmtpc::core::ScopeConfig> * scopeCfgs;
  volatile bool * stop; 
  volatile bool * done; 
  std::vector<dmtpc::daq::Buffer*> * buffers; 
  std::vector<dmtpc::daq::Scope*> * scopes; 
};


struct scopeReadoutThreadArgs
{
  const dmtpc::core::RunConfig * runCfg;
  const dmtpc::core::ScopeConfig * scopeCfg;
  dmtpc::daq::Buffer * buf; 
  dmtpc::daq::Scope * scope; 
  volatile  bool * stop; 
};

void * scopeReadoutThread(void * args)
{

  scopeReadoutThreadArgs * a = (scopeReadoutThreadArgs*) args; 

  unsigned timeout = a->runCfg->scope_wait_timeout;
  a->scope->start();
  //std::cout << "Allocating scope buffer..." << std::endl;
  void * mem = a->buf->getMem();
  bool stopped_already = false; 
  while(!(*a->stop))
  { 
    //std::cout << "Waiting for data..." << std::endl;
    a->scope->waitForData(timeout);
    //std::cout << "While scope->readOut" << std::endl;
    while (a->scope->readOut(mem)) //WAS if, but we want to consume ALL readouts 
    {
      a->buf->commit(); 
      mem = a->buf->getMem(); 
      if (*a->stop && !stopped_already) 
      {
        a->scope->stop();
        stopped_already = true; 
      }
    }
    //while(a->scope->readOut(a->buf->getMem())) //readout all available
  }
  if (!stopped_already)
    a->scope->stop();
}

void * scopeThread(void * args)
{
  scopeThreadArgs *a = (scopeThreadArgs*) args; 
  std::vector<TThread*> threads(a->scopes->size()); 
  std::vector<scopeReadoutThreadArgs> rargs(a->scopes->size()); 

  for (unsigned i = 0; i < a->scopes->size(); i++)
  {
    rargs[i].runCfg = a->runCfg;
    rargs[i].scopeCfg = &(a->scopeCfgs->at(i));
    rargs[i].buf = a->buffers->at(i); 
    rargs[i].scope = a->scopes->at(i); 
    rargs[i].stop = a->stop; 
    threads[i] = new TThread(scopeReadoutThread, &rargs[i]); 

  }

  for (unsigned i = 0; i < a->scopes->size(); i++)
  {
    threads[i]->Run(); 
  }

  for (unsigned i = 0; i < a->scopes->size(); i++)
  {
    threads[i]->Join(); 
  }

  *(a->done) = true; 
  delete a; 
}


dmtpc::daq::ScopeHandler::ScopeHandler(const dmtpc::core::RunConfig * runCfg,
				       const std::vector<dmtpc::core::ScopeConfig> * scopeCfgs)
{
  std::cout << "ScopeHandler::ScopeHandler()" << std::endl;
  TThread::SetCancelOn(); 
  scopes = ScopeLoader::loadScopes(scopeCfgs); 

  for (unsigned i = 0; i < getNScopes(); i++) 
  {
    Scope * scope = scopes[i]; 
    scope->init(&(scopeCfgs->at(i))); 
    buffers.push_back(new Buffer(runCfg->scope_buffer_size, scope->getBufferSize())); 
  }

  this->runCfg = runCfg;
  this->scopeCfgs = scopeCfgs;

  scope_thread = new TThread(scopeThread); 
}


int dmtpc::daq::ScopeHandler::stop()
{
  stopme = true; 
  return 1;
}

bool dmtpc::daq::ScopeHandler::allConsumed()
{
  for (int i = 0; i < getNScopes(); i++)
  {
    if(!buffers[i]->empty())
    {
      return false;
    }
  }


  return true; 

}

int dmtpc::daq::ScopeHandler::start()
{

  stopme = false; 
  isDone = false; 
  scopeThreadArgs * args = new scopeThreadArgs; 
  args->runCfg = runCfg;
  args->scopeCfgs = scopeCfgs;
  args->done = &isDone; 
  args->stop = &stopme; 
  args->buffers = &buffers; 
  args->scopes = &scopes; 

  scope_thread->Run(args); 
  return 1;
}

bool dmtpc::daq::ScopeHandler::done()
{

  if (!getNScopes()) return true; 
  return isDone; 
}
