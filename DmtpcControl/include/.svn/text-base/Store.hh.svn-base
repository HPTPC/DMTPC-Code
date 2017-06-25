#ifndef DMTPC_CONTROL_STORE_HH
#define DMTPC_CONTROL_STORE_HH

class TTimeStamp; 

namespace dmtpc
{
  namespace control
  {
    class Store
    {
      // find other solution:templates may not be ‘virtual’
      public: 
      //template <typename T> 
      //  virtual int save(const char * key, T value) = 0; 
       virtual int save(const char * key, double value) = 0; 

      //template <typename T> 
      //  virtual T getValue(const char * key, TTimeStamp * when = 0) = 0; 
      virtual double getValue(const char * key, TTimeStamp * when = 0) = 0; 
    }; 
  }
}




#endif
