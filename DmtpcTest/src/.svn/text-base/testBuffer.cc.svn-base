#include "Buffer.hh"
#include "TThread.h"
#include <iostream>
#include "TSystem.h"
#include <cmath>



void * writer2(void * args)
{

  dmtpc::daq::Buffer * buf = (dmtpc::daq::Buffer*) args; 
   
  for (int i = 0; i < 100000; i++)
  {
    
    int * mem = (int*) buf->getMem(); 
    for (int j = 0; j < 1000; j++)
    {
      mem[j] = i*i -i *j + j*j; 
    }
    buf->commit(); 
  }
  return 0;
}

void * reader2(void * args)
{

  int * ret = new int; 
  dmtpc::daq::Buffer * buf = (dmtpc::daq::Buffer*) args; 
   
  for (int i = 0; i < 100000; i++)
  {
    int mem[1000];  
    buf->pop(&mem); 

    for (int j = 0; j < 1000; j++)
    {
      if (mem[j] !=i*i-i*j+j*j)
      {
        std::cerr << "REASON: buffered threaded did not read correct value: " << mem[j] <<" != "<< i*j << std::endl; 
         (*ret)++; 
      }
    }
  }
  return ret; 
}



void * writer(void * args)
{

  dmtpc::daq::Buffer * buf = (dmtpc::daq::Buffer*) args; 
   
  for (int i = 0; i < 1000000; i++)
  {
    
    double * mem = (double*) buf->getMem(); 
    *mem = sqrt(i); 
    buf->commit(); 
  }
  return 0;
}

void * reader(void * args)
{

  int * ret = new int; 
  dmtpc::daq::Buffer * buf = (dmtpc::daq::Buffer*) args; 
   
  for (int i = 0; i < 1000000; i++)
  {
    double mem; 
    buf->pop(&mem); 
    if (mem != sqrt(i))
    {
      std::cerr << "REASON: threaded did not read correct value: " << mem <<" != "<< sqrt(i) << std::endl; 
       (*ret)++; 
    }
  }
  return ret; 
}


int main(int nargs, char ** args) 
{

  dmtpc::daq::Buffer buffer(1000,sizeof(double)); 

  int ret = 0; 
  for (int i = 0; i < 2000; i++)
  {
    double * memwrite  = (double*) buffer.getMem(); 
    *memwrite = sqrt(i); 
    buffer.commit(); 
    double memread; 
    buffer.pop(&memread);
    if (*memwrite != memread)
    {
      std::cerr << "REASON: did not read correct value: " << memread <<" != "<< memwrite << std::endl; 
      ret++;
    }
  }

  dmtpc::daq::Buffer buffer3(1000,sizeof(double)); 

  for (int i = 0; i < 100; i++)
  {
    for (int j = 0; j < 700; j++)
    {
      double * memwrite  = (double*) buffer3.getMem(); 
      *memwrite = sqrt(i+j); 
      buffer3.commit();
    }

    for (int j = 0; j < 700; j++)
    {
      double memread; 
      buffer3.pop(&memread);
      if (sqrt(i+j) != memread)
      {
        std::cerr << "REASON: interleaved did not read correct value: " << memread <<" != "<< sqrt(i+j) << std::endl; 
        ret++;
      }
    }
  }


  dmtpc::daq::Buffer buffer2(2500,sizeof(double)); 

  TThread writer_t(&writer,&buffer2); 
  TThread reader_t(&reader,&buffer2); 

  reader_t.Run();
  writer_t.Run();

  void * thread_ret = 0;
  writer_t.Join(); 
  reader_t.Join((void**) &thread_ret); 
  ret += *((int*)thread_ret); 

  dmtpc::daq::Buffer buffer4(2500,sizeof(int)*1000); 

  TThread writer2_t(&writer2,&buffer4); 
  TThread reader2_t(&reader2,&buffer4); 

  writer2_t.Run(); 
  reader2_t.Run(); 

  void * thread_ret2 = 0; 

  writer2_t.Join(); 
  reader2_t.Join((void**) &thread_ret2);
  ret += *((int*) thread_ret); 



  return ret; 

}
