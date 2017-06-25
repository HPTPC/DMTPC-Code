#ifndef DMTPC_DAQ_BUFFER_HH
#define DMTPC_DAQ_BUFFER_HH

/** Circular Buffer Class that blocks on full or empty
 *
 *   Compile with -DBUFFER_USE_SEMAPHORES to use semaphores rather than busy waiting on block (probably hurts performance,
 *   but perhaps better use of resources) 
 *
 **/ 
#include <cstddef>

//#define BUFFER_USE_SEMAPHORES  //if you want to use semaphores instead of busy waiting 

#ifdef BUFFER_USE_SEMAPHORES
#include <semaphore.h> 
#endif 

namespace dmtpc
{
  namespace daq
  {
    class Buffer
    {
      public: 
        Buffer(size_t buffer_size, size_t data_length); 
        virtual ~Buffer(); 

        /** Returns a pointer to a block of memory of size data_length * data_nbytes that may be written to **/ 
        void* getMem(); 
        //commits the write
        void commit(); 

        /* returns the last portion of data (making a copy)*/ 
        void *pop(void * buf); 

        bool empty() const; 
        size_t occupancy() const { return produced_count - consumed_count; } 

      private:
        void * data; 
        volatile size_t produced_count;
        size_t entry_length;
        size_t capacity; 
        volatile size_t consumed_count; 
        size_t index; 

#ifdef BUFFER_USE_SEMAPHORES

        sem_t filledCount; 
        sem_t emptyCount; 
#endif
    }; 
  }
}

#endif
