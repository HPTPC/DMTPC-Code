#include "InteractiveReadout.hh" 

#include "TH2.h" 
#include "TString.h"
#include "TCanvas.h" 
#include "TThread.h"
#include "TMutex.h"

static unsigned canv_num = 0; 

dmtpc::mc::mctpc::InteractiveReadout::InteractiveReadout(const char * filter, int w, int h) 
 : filter(filter), w(w), h(h)
{
  id = canv_num++; 
}

static void  * waitfn(void * args)
{
  ((TMutex*) args)->Lock(); 
  return 0; 
}



int dmtpc::mc::mctpc::InteractiveReadout::read(const std::vector<dmtpc::mc::mctpc::Signal>* in) 
{

  //get size 
  int npad = 0; 

  if (filter) 
  {
    for (unsigned i = 0; i < in->size(); i++) 
    {
      if (strstr(in->at(i).getName(), filter))
      {
        npad++; 
      }
    }
  }

  else
  {
    npad = in->size(); 
  }

  if (!npad) return 0; 

  TCanvas c(TString::Format("readout_canv_%d",id),"Readout",w,h);

  c.Divide(npad > 2 ? (npad+1)/2 : npad, npad > 2 ? 2 : 1); 

  int ipad = 1;

  for (unsigned i = 0; i < in->size(); i++)
  {
      if (!filter || strstr(in->at(i).getName(), filter))
      {
        c.cd(ipad++); 

        in->at(i).getNDim() == 2 ? in->at(i).getHist2D()->DrawCopy("colz") 
                                  : in->at(i).getHist()->DrawCopy(); 

      }
  }

  c.Show(); 
  c.Update(); 

  TMutex* m = new TMutex;
  m->Lock(); 


  TThread  wait_thread(waitfn);
  wait_thread.Run(m); 
  c.Connect("Closed()","TMutex",m,"UnLock()"); 
  wait_thread.Join();
  m->UnLock(); 

  return npad; 
}

