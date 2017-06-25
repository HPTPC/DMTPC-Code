#ifndef DMTPC_SKIM_VIEWER_SCOPE_FRAME
#define DMTPC_SKIM_VIEWER_SCOPE_FRAME

#include "SkimDataset.hh"
#include "TGFrame.h"
#include "SkimEvent.hh"
#include "Event.hh"
/*#include "../ScopeDataInfo.hh"*/

// waveformtools includes
//  for analysis of generic waveforms
#include "WaveformVector.hh"
#include "SkimWaveform.hh"
//#include "DmtpcPulse.hh"
//  for analysis of voltage sensitive amplitifier waveforms
#include "FastWfVector.hh"
#include "FastWaveform.hh"
#include "FastPulse.hh"
//  for analysis of charge sensitive pre-amp waveforms
#include "CspWfVector.hh"
#include "CspWaveform.hh"
#include "CspPulse.hh"
//  for analysis of pmt waveforms
#include "PMTWfVector.hh"
#include "PMTWaveform.hh"
#include "PMTPulse.hh"
//  general functions for waveform analysis
#include "WaveformAnalysis.hh"

#include "TArrow.h"
#include "TLine.h"
#include "TRootEmbeddedCanvas.h"
#include "TGComboBox.h"
#include "TGNumberEntry.h"
#include "TGLabel.h"
#include "TObjArray.h"
#include "TCanvas.h"

namespace dmtpc
{
namespace viewer
{
class ViewerScopeFrame : public TGMainFrame
{

  public:
    ViewerScopeFrame(const TGWindow *p, UInt_t w, UInt_t h, bool * show); 
    virtual ~ViewerScopeFrame(){canvas->Delete(); *show_traces = false; }
    void Display(dmtpc::core::Event * evt, TObjArray * wfvecs); 
    void GroupChanged(Long_t id); 
    void ChannelChanged(Int_t id); 
    void DrawTrace(); 
    void DrawAll(); 
    void UpdateAll(); 
    void CloseAll(){show_all=false;} 

    void ShowAlone(); 
    void UpdateAlone(); 
    void CloseAlone(){show_alone=false;} 
  
    TString GenerateWfVectorText(TObject * wfvec);
    void DrawWfAnalysis(TObject * wfvec);

  private: 
    TRootEmbeddedCanvas * canvas; 
    bool * show_traces; //!
    TGNumberEntry * group_select; 
    TGComboBox * channel_select; 
    TGLabel * trace_text; 
    TObjArray * waveform_vectors; 
    dmtpc::core::Event * original_event;
    int current_group;
    int current_chan;
    TCanvas * all; 
    TCanvas * alone_c; 
    bool show_all;
    bool show_alone; 

    TObjArray * algorithm_draw_objects;

  ClassDef(ViewerScopeFrame,0);
};

}
}

#endif

