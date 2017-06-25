#include "Scope.hh" 
#include <time.h>



size_t dmtpc::daq::Scope::waitForData(unsigned timeout)
{

  struct timespec tp0;  
  struct timespec tp1;  

  clock_gettime(CLOCK_REALTIME,&tp0); 
  while(true)
  {

    if (size_t a = dataAvailable())
    {
      return a; 
    }
    clock_gettime(CLOCK_REALTIME,&tp1); 

    double diff = (tp1.tv_sec-tp0.tv_sec) * 1e6 + (tp1.tv_nsec-tp0.tv_nsec)*1e-3 ; 
    if (diff > timeout)
      return 0; 

    sched_yield(); 
  }
}
