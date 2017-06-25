#ifndef M3GlobalData_hh
#define  M3GlobalData_hh
#include <TSystem.h>
#include "TinyMySqlInterface.hh"
class TinyMySqlInterface;
class M3GlobalData{ // variables accessible everywhere, very ugly solution
public:
  M3GlobalData(){ mySqlTest=0; mySqlOnlM3=0; verbose=0;}
  int verbose; // 0=silent, larger value --> more printout
  TinyMySqlInterface *mySqlTest;
  TinyMySqlInterface *mySqlOnlM3;
  // add more if needed, not advised.
};
extern  M3GlobalData gM3Data;
#endif
