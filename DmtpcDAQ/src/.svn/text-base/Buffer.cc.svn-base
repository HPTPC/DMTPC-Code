#include <iostream>
#include "Buffer.hh" 
#include <cstdlib>
#include "TThread.h"
#include <cstdlib>


/** 
 * Define custom memory fence since the implementation in gcc 4.2 seems to be broken
 *
 */ 
#define MEMORY_FENCE __asm__ volatile ("mfence":::"memory") ; 

static size_t buffer_count = 0; 

dmtpc::daq::Buffer::Buffer(size_t buffer_size, size_t data_size)
{

  data = malloc(data_size * buffer_size); 

#ifdef BUFFER_USE_SEMAPHORES
  sem_init(&filledCount,0,0); 
  sem_init(&emptyCount,0,buffer_size); 
#endif

  if (!data) 
  {
    std::cerr << " Out of RAM? That's not good... " << std::endl; 
  }

  capacity = buffer_size; 
  entry_length = data_size; 
  produced_count = 0; 
  consumed_count = 0; 
  index = buffer_count++; 
}

dmtpc::daq::Buffer::~Buffer()
{
  if (data) 
    free((void*)data); 


#ifdef BUFFER_USE_SEMAPHORES
  sem_destroy(&filledCount); 
  sem_destroy(&emptyCount); 
#endif
}


void * dmtpc::daq::Buffer::getMem()
{
  
#ifdef BUFFER_USE_SEMAPHORES
  sem_wait(&emptyCount); 
#else
  while (produced_count-consumed_count  == capacity) 
  {
   fprintf(stderr,"WARNING: buffer %d is full!\n", index); 

   sched_yield(); 
  }
#endif 

  void * ret = (void*) (size_t(data) + entry_length * (produced_count % capacity)) ; 
  return ret; 
}

void dmtpc::daq::Buffer::commit()
{ 
  MEMORY_FENCE
  produced_count ++; 

#ifdef BUFFER_USE_SEMAPHORES
  sem_post(&filledCount);
#endif
}


void * dmtpc::daq::Buffer::pop(void * buf)
{

  //allocate buffer if none is given 
  if (!buf) buf = malloc(entry_length); 

#ifdef BUFFER_USE_SEMAPHORES
  sem_wait(&filledCount); 
#else
  while(produced_count - consumed_count == 0) 
  {
    sched_yield(); 
  }

#endif
  memcpy(buf,(void*)( size_t(data) + entry_length*(consumed_count % capacity)),entry_length); 
  MEMORY_FENCE
  consumed_count++;

#ifdef BUFFER_USE_SEMAPHORES
  sem_post(&emptyCount); 
#endif

  return buf; 
}

bool dmtpc::daq::Buffer::empty() const
{
  return produced_count == consumed_count; 
}

