#ifndef DMTPC_MC_MCTPC_SIGNAL_HH
#define DMTPC_MC_MCTPC_SIGNAL_HH


class TGraph2D; 
class TH1; 
class TH2; 

namespace dmtpc
{
  namespace mc
  {
    namespace mctpc
    {
      /* A signal from the detector ... can be 0 dimensional ( a value), 1-dimensional ( a th1), 2-dimensional (th2) or 3-dimensional (TGraph2D) */
      class Signal
      {

        public:
          Signal(const TH1 * hist, int ndim, const char * name) : sig(hist),  graph(0),value(0), ndim(ndim), name(name)  {;}
          Signal(const TGraph2D * graph, const char * name) : sig(0),graph(graph), value(0), ndim(3), name(name)  {;}
          Signal(double value, const char * name) : sig(0), graph(0), value(value) , ndim(0), name(name){;} 

          virtual ~Signal() {} 

          int getNDim() const { return ndim; } 
          const TH1 * getHist() const { return ndim == 1 ? sig : 0 ; } 
          const TH2 * getHist2D() const { return ndim == 2 ? (const TH2*) sig : 0; }
          const TGraph2D * getTGraph2D() const { return ndim == 3 ? graph : 0 ; } 
          double getValue() const { return value; } 
          const char * getName() const { return name; }
        
        private:
          const TH1 * sig;
          const TGraph2D * graph; 
          double value; 
          int ndim; 
          const char * name; 

      }; 
    }
  }
}
#endif

