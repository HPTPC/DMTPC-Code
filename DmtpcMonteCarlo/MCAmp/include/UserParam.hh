#ifndef DMTPC_MC_AMP_USER_PARAM_HH
#define DMTPC_MC_AMP_USER_PARAM_HH

#include "TString.h" 
#include "TVector3.h" 
#include "TTree.h"
#include <cstdlib>

namespace dmtpc
{
  namespace mc
  {
    namespace amp
    {
      class UserParam
      {
        public: 
          UserParam(const char * name, double val  = 0, bool needs_remesh = true, bool needs_resolve = true) : vec(0), dbl(val), name(name), needs_remesh(needs_remesh), needs_resolve(needs_resolve) 
               { TString str = TString::Format("%g",val); repr = strdup(str.Data()) ; }
          UserParam(const char * name, TVector3 *val, bool needs_remesh = true, bool needs_resolve = true) : vec(val),name(name) , needs_remesh(needs_remesh), needs_resolve(needs_resolve)
              { TString str = TString::Format("[%g,%g,%g]",val->x(), val->y(), val->z()); repr = strdup(str.Data()) ; }  
          virtual ~UserParam() { free(repr); } 
          bool isScalar() const { return vec; } 
          const char * getName() const { return name;}
          const char * toString() const { return repr; } 
          operator double() const { return vec ? vec->Mag() : dbl; }
          operator int() const { return (int) double(*this); } 
          operator const TVector3*() const{ return vec; } 
          void branchMe(TTree * tree) { vec ? tree->Branch(name, vec) : tree->Branch(name,&dbl); }
          bool needsRemesh() const { return needs_remesh;} 
          bool needsReSolve() const { return needs_resolve;} 

        private: 
            double dbl; 
            int n; 
            TVector3* vec; 
            const char * name; 
            char * repr; 
            bool needs_remesh; 
            bool needs_resolve; 
      };
    }
  }
}

#endif
