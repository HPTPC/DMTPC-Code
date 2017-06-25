#ifndef DMTPC_CORE_ITERATORS_HH
#define DMTPC_CORE_ITERATORS_HH

#include <iterator>
#include "Dataset.hh" 
#include "Event.hh"
#include "TSystemDirectory.h"
#include "TH2.h" 
#include "TTree.h"


namespace dmtpc
{

namespace core
{

class DmtpcDatasetImageIterator : public iterator<std::input_iterator_tag, const TH2*> 
{
  private: 
    Dataset * d; 
    int c; 
    int iev; 

  public: 
    DmtpcDatasetImageIterator(Dataset * dataset, int camera = 0, int init = 0) 
      { d = dataset; c = camera; iev = init; } 
    DmtpcDatasetImageIterator(const DmtpcDatasetImageIterator & oth) 
      { d = oth.d; c = oth.c; iev = oth.iev; } 
    DmtpcDatasetImageIterator & operator++() 
      { iev++; return *this;} 
    DmtpcDatasetImageIterator operator++(int) 
      { DmtpcDatasetImageIterator tmp(*this); operator++(); return tmp;} 
    bool operator==(const DmtpcDatasetImageIterator & oth)
      { return d == oth.d && c == oth.c && iev == oth.iev;} 
    bool operator!=(const DmtpcDatasetImageIterator & oth)
      { return d != oth.d || c != oth.c || iev != oth.iev;} 
    const TH2 * operator*() 
      { d->getEvent(iev); return d->event()->ccdData(c); } 
}; 


template <class T = double> 
class TreeIterator : public iterator<std::input_iterator_tag, T> 
{
  private:  
    TTree *t; 
    T * ptr; 
    int ev; 
  public: 
    TreeIterator(TTree * tree, T *branch_pointer, int i = 0) { t = tree; ptr = branch_pointer; ev = i; } 
    TreeIterator(const TreeIterator & oth) { t = oth.t; ptr = oth.ptr; ev = oth.ev; } 
    TreeIterator & operator++() { ev++; return *this;}
    TreeIterator operator++(int) { TreeIterator tmp(*this); ev++; return tmp;}
    bool operator==(const TreeIterator & oth) { return t == oth.t && ptr == oth.ptr && ev == oth.ev; } 
    bool operator!=(const TreeIterator & oth) { return !(t == oth.t && ptr == oth.ptr && ev == oth.ev); } 
    T operator *() { t->GetEntry(ev); return *ptr; } 
};


}
}

#endif

