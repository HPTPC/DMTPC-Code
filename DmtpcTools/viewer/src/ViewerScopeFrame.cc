#include "ViewerScopeFrame.hh"
#include "TCanvas.h"
#include "Waveform.hh"
#include "TFrame.h"

ClassImp(dmtpc::viewer::ViewerScopeFrame); 

void dmtpc::viewer::ViewerScopeFrame::UpdateAlone()
{
  if (!show_alone) return; 
  alone_c->Clear(); 
  if (original_event != NULL && original_event->nScopeData() > 0)
  {
    alone_c->cd(); 
    original_event->getTrace(current_chan,current_group)->Draw();
  }
}

void dmtpc::viewer::ViewerScopeFrame::ShowAlone()
{
  if (show_alone)
  {
    alone_c->Close(); 
    show_alone=false; 
    return; 
  }

  alone_c = new TCanvas("alone_c","Standalone Trace",400,400); 
  alone_c->Connect("Closed()","DmtpcSkimViewerScopeFrame",this,"CloseAlone()"); 
  show_alone = true; 
  UpdateAlone(); 
  return;

}

dmtpc::viewer::ViewerScopeFrame::ViewerScopeFrame(const TGWindow *p,
                                           UInt_t w, UInt_t h, bool * handle) 
                                          : TGMainFrame(p,w,h)
{


  //base layout
  TGHorizontalFrame * hframe = new TGHorizontalFrame(this,600,500); 


  /* Left part*/
  TGVerticalFrame * leftframe = new TGVerticalFrame(hframe,350,400);  
  canvas = new TRootEmbeddedCanvas("ScopeCanvas",leftframe,350,350); 
  leftframe->AddFrame(canvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 3,3,3,3)); 

  TGHorizontalFrame *buttonFrame = new TGHorizontalFrame(leftframe,400,40); 
  buttonFrame->AddFrame(new TGLabel(buttonFrame,"Trigger Group: "), new TGLayoutHints(kLHintsCenterX | kLHintsCenterY,5,0,3,4)); 

  group_select = new TGNumberEntry(buttonFrame, 0,2,-1,TGNumberFormat::kNESInteger,
                                TGNumberFormat::kNEANonNegative,
                                TGNumberFormat::kNELLimitMinMax,
                                0,0); 
  group_select->Connect("ValueChanged(Long_t)","ViewerScopeFrame",this,"GroupChanged(Long_t)"); 
  group_select->Connect("ValueSet(Long_t)","ViewerScopeFrame",this,"GroupChanged(Long_t)"); 
  buttonFrame->AddFrame(group_select,new TGLayoutHints(kLHintsCenterX | kLHintsCenterY,0,5,3,4)); 

  buttonFrame->AddFrame(new TGLabel(buttonFrame,"Channel: "), new TGLayoutHints(kLHintsCenterX,20,0,3,4)); 

  channel_select = new TGComboBox(buttonFrame); 
  channel_select->AddEntry("CHANNEL",0); 
  channel_select->Layout(); 
  channel_select->Connect("Selected(Int_t)","ViewerScopeFrame",this,"ChannelChanged(Int_t)"); 

  buttonFrame->AddFrame(channel_select,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,0,5,3,4)); 

  leftframe->AddFrame(buttonFrame, new TGLayoutHints(kLHintsExpandX ,3,3,3,3)); 

  TGVerticalFrame * rightframe = new TGVerticalFrame(hframe,300,400);

  TGTextButton * showall = new TGTextButton(rightframe,"Show All"); 
  showall->Connect("Clicked()","ViewerScopeFrame",this,"DrawAll()"); 
  rightframe->AddFrame(showall,new TGLayoutHints(kLHintsCenterX,5,5,3,4)); 

  TGTextButton * showalone = new TGTextButton(rightframe,"Show in own TCanvas"); 
  showalone->Connect("Clicked()","ViewerScopeFrame",this,"ShowAlone()"); 
  rightframe->AddFrame(showalone,new TGLayoutHints(kLHintsCenterX,5,5,3,4)); 

  trace_text = new TGLabel(rightframe, "No Traces"); 
  trace_text->SetBackgroundColor(0xffffff); 
  trace_text->ChangeOptions(kSunkenFrame); 

  rightframe->AddFrame(trace_text, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,3,3,3,10)); 

  //spacer
  rightframe->AddFrame(new TGLabel(rightframe,""), new TGLayoutHints(kLHintsCenterX,120,120,0,0)); 

  hframe->AddFrame(leftframe, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2)); 
  hframe->AddFrame(rightframe, new TGLayoutHints(kLHintsExpandY,2,2,2,2)); 
  AddFrame(hframe, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2)); 

  current_group = 0;
  current_chan = 0;
  SetWindowName("Scope Traces"); 
  MapSubwindows();
  Resize(GetDefaultSize()); 
  SetIconPixmap("/net/zwicky/dmtpc/software/viewer/icon.png");
  MapWindow(); 
  show_traces = handle; 
  show_all = false; 
  show_alone = false; 
  waveform_vectors = NULL; 
  all = NULL; 

  algorithm_draw_objects = new TObjArray;
  algorithm_draw_objects->SetOwner(kTRUE);
}


void dmtpc::viewer::ViewerScopeFrame::DrawTrace()
{
 
  if (original_event != NULL && original_event->scopeData()->GetEntries() > 0)
  {
    canvas->GetCanvas()->cd(); 
    original_event->scopeData(current_chan,current_group)->Draw();
    
    double timestamp = original_event->scopeData(current_chan,current_group)->GetBinContent(0); 
    // update this
    TString text;
    /*
      text+=wf->getType(); 
      text+="\", Trigger Group "; 
      text += current_group; 
      text += "\n Rise Time: "; 
      text += wf->getRiseTime(); 
      text += "\n Fall Time: "; 
      text += wf->getFallTime(); 
      text += "\n Baseline: "; 
      text += wf->getBaseline(); 
      text += "\n Baseline RMS: "; 
      text += wf->getBaselineRMS(); 
      text += "\n Peak Height: "; 
      text += wf->getPeakHeight(); 
      text += "\n Peak Height Time: "; 
      text += wf->getPeakHeightTime(); 
      text += "\n Minimum: "; 
      text += wf->getMinimum(); 
      text += "\n Trough Depth: "; 
      text += wf->getTroughDepth(); 
    */
    
    if(waveform_vectors != NULL && waveform_vectors->GetEntries() > 0){
      text += GenerateWfVectorText( ((TObject*)(*waveform_vectors)[current_chan]) );
    }

    text += "\nTimestamp: "; 
    text += timestamp; 
    text += "\n";
    
    trace_text->SetText(text.Data());
    
    if (show_all) UpdateAll(); 
    if (show_alone) UpdateAlone(); 

    if (waveform_vectors != NULL) 
      DrawWfAnalysis( ((TObject*)(*waveform_vectors)[current_chan]) );
  }
  else
  {
    canvas->GetCanvas()->Clear();  
    trace_text->SetText("No trigger groups"); 
  }


  canvas->GetCanvas()->Update(); 
  trace_text->Layout(); 
}

void dmtpc::viewer::ViewerScopeFrame::DrawAll()
{

  if (show_all)
  {
    all->Close(); 
    show_all = false;
    return;
  }

  all = new TCanvas("alltraces","All Traces",800,400); 
  all->Connect("Closed()","ViewerScopeFrame",this,"CloseAll()");
  show_all = true; 
  UpdateAll();

}
void dmtpc::viewer::ViewerScopeFrame::UpdateAll()
 {

  all->Clear();
  if (original_event != NULL && original_event->scopeData()->GetEntries() > 0)
  {

    int ntrig = original_event->scopeDataInfo(0)->getNTriggers();
    int nchan = ( ( original_event->scopeData()->GetEntries() )/
                  ntrig );

    all->Divide(ntrig,nchan);

    int cid = 0; 
    for (int chan = 0; chan < nchan; chan++)
    {
      for (int g = 0; g < ntrig; g++)
      {
        all->cd(++cid);  
        original_event->scopeData(chan,g)->Draw();
      }
    }

  }

}


void dmtpc::viewer::ViewerScopeFrame::ChannelChanged(Int_t id)
{
  current_chan = id;
  DrawTrace(); 
}

void dmtpc::viewer::ViewerScopeFrame::GroupChanged(Long_t id __attribute__((unused)))
{
  current_group = group_select->GetIntNumber();
  DrawTrace(); 
}

void dmtpc::viewer::ViewerScopeFrame::Display(dmtpc::core::Event * evt, TObjArray * wfvecs)
{

  canvas->GetCanvas()->Clear(); 

  original_event = evt;
  waveform_vectors = wfvecs;  

  if (original_event != NULL && original_event->scopeData()->GetEntries() > 0)
  {
    group_select->SetLimitValues(0,original_event->scopeDataInfo(0)->getNTriggers()-1); 
    channel_select->RemoveAll();
    
    int nchan = ( ( original_event->scopeData()->GetEntries() )/
                  ( original_event->scopeDataInfo(0)->getNTriggers() ) );
    //    cout << "nchan=" << nchan << endl;


    ////////////////////////////////////////////////////////////////////////
    ///
    ///   Set the names for the channels in the Channel: menu 
    ///
    // if the waveform_vector analysis objects exist, take the channel names
    // (specified in the configuration file used to skim the dataset) from
    // them; otherwise, just use first trigger's raw histogram names 
    if(waveform_vectors != NULL && waveform_vectors->GetEntries() > 0){

      for (int i = 0; i < nchan; i++)
      {
        channel_select->AddEntry( ((TObject*)(*waveform_vectors)[i])->GetName(),i);
      }

    } else {
      
      for (int i = 0; i < nchan; i++)
      {
          // want this to be the names given to the channels in the config file, but for now, 
          // take the name from the first trigger on each channel...
          channel_select->AddEntry(original_event->scopeData(i,0)->GetName(),i);
      }
    }
      
    if (current_chan < nchan)
      {
        channel_select->Select(current_chan,false); 
      }
    else
      {
        channel_select->Select(0,false); 
        current_chan = 0; 
      }
    channel_select->Layout(); 
    group_select->SetIntNumber(0);
    current_group = 0; 
    DrawTrace();     
  }
  else
  {
    trace_text->SetText("Original data file\nmust be loaded\nto view traces!"); 
    group_select->SetLimitValues(0,0); 
  }



  canvas->GetCanvas()->Update();
}

TString dmtpc::viewer::ViewerScopeFrame::GenerateWfVectorText(TObject * wfvec)
{
  TString text;

  text += "Channel \""; 
  text += wfvec->GetName(); 
  text +="\", Trigger "; 
  text += current_group; 

  text += "\n WfVector Type: "; 
  TString WfVectorType=wfvec->IsA()->GetName();
  text += WfVectorType;
  
  // need to cast the class to print the analysis level quantities
  if(WfVectorType==TString("CspWfVector"))
  {
    CspWfVector *cspwfvec=((CspWfVector*)wfvec);
    
    text += "\n Baseline: "; 
    text += cspwfvec->at(current_group).getBase(); 
    text += "\n Baseline RMS: "; 
    text += cspwfvec->at(current_group).getRMS(); 
    text += "\n NPulses: "; 
    text += cspwfvec->at(current_group).size();
    for (int i = 0; i < cspwfvec->at(current_group).size(); i++)
    {
       text += "\n    Pulse "; 
       text += i; 
       text + ":"; 
       text += "\n         Height: "; 
       text += cspwfvec->at(current_group,i).getPeak();  
       text += "\n         Peak Time: "; 
       text += cspwfvec->at(current_group,i).getPeakTime();  
    }
  } 
  else if(WfVectorType==TString("PMTWfVector"))
  {
    PMTWfVector *pmtwfvec=((PMTWfVector*)wfvec);
    
    text += "\n Baseline: "; 
    text += pmtwfvec->at(current_group).getBase(); 
    text += "\n Baseline RMS: "; 
    text += pmtwfvec->at(current_group).getRMS(); 
    text += "\n NPulses: "; 
    text += pmtwfvec->at(current_group).size();

    for (int i = 0; i < pmtwfvec->at(current_group).size(); i++)
    {
       text += "\n    Pulse "; 
       text += i; 
       text + ":"; 
       text += "\n         Height: "; 
       text += pmtwfvec->at(current_group,i).getPeak();  
       text += "\n         Peak Time: "; 
       text += pmtwfvec->at(current_group,i).getPeakTime();  
    }
  }
  else if(WfVectorType==TString("FastWfVector"))
  {
    FastWfVector *fastwfvec=((FastWfVector*)wfvec);
    
    text += "\n Baseline: "; 
    text += fastwfvec->at(current_group).getBase(); 
    text += "\n Baseline RMS: "; 
    text += fastwfvec->at(current_group).getRMS(); 
    text += "\n NPulses: "; 
    text += fastwfvec->at(current_group).size();
    for (int i = 0; i < fastwfvec->at(current_group).size(); i++)
    {
       text += "\n    Pulse "; 
       text += i; 
       text + ":"; 
       text += "\n         Height: "; 
       text += fastwfvec->at(current_group,i).getPeak();  
       text += "\n         Peak Time: "; 
       text += fastwfvec->at(current_group,i).getPeakTime();  
    }
 }
  else
  {
    // text for this analysis class is not yet implemented
  }

  return text;
}

void dmtpc::viewer::ViewerScopeFrame::DrawWfAnalysis(TObject * wfvec)
{
  // clear the algorithm-related objects
  algorithm_draw_objects->Clear();

  TString WfVectorType=wfvec->IsA()->GetName();

  //  alone_c->cd();
  
  // need to cast the class to print the analysis level quantities
  if(WfVectorType==TString("CspWfVector"))
  {
    CspWfVector *cspwfvec=((CspWfVector*)wfvec);

    /*    
    TArrow *csppulse=new TArrow(cspwfvec->at(current_group).getWfMaxTime(),
                                cspwfvec->at(current_group).getWfMax()+20.*cspwfvec->at(current_group).getRMS(),
                                cspwfvec->at(current_group).getWfMaxTime(),
                                cspwfvec->at(current_group).getWfMax()+10.*cspwfvec->at(current_group).getRMS());
    */

    TArrow *cspPulsePosition=new TArrow(cspwfvec->at(current_group).at(0).getPeakTime(),
                                        original_event->scopeData(current_chan,current_group)->GetMaximum()+20.*cspwfvec->at(current_group).getRMS(),
                                        cspwfvec->at(current_group).at(0).getPeakTime(),
                                        original_event->scopeData(current_chan,current_group)->GetMaximum()+10.*cspwfvec->at(current_group).getRMS());

    
    cspPulsePosition->SetLineWidth(3);
    cspPulsePosition->SetLineColor(kRed);

    algorithm_draw_objects->Add(cspPulsePosition);
    
    // draw a vertical line at the start of the identified pulse
    TLine *cspPulseStartTime=new TLine(cspwfvec->at(current_group).at(0).getStartTime(),
                                       -1e6,
                                       cspwfvec->at(current_group).at(0).getStartTime(),
                                       1e6);
    cspPulseStartTime->SetLineWidth(3);
    cspPulseStartTime->SetLineColor(kRed);
    cspPulseStartTime->SetLineStyle(2);
    
    algorithm_draw_objects->Add(cspPulseStartTime);

    // draw a vertical line at the end of the identified pulse
    TLine *cspPulseEndTime=new TLine(cspwfvec->at(current_group).at(0).getEndTime(),
                                     -1e6,
                                     cspwfvec->at(current_group).at(0).getEndTime(),
                                     1e6);
    cspPulseEndTime->SetLineWidth(3);
    cspPulseEndTime->SetLineColor(kRed);
    cspPulseEndTime->SetLineStyle(2);
    
    algorithm_draw_objects->Add(cspPulseEndTime);
    
    // draw a horizontal line at the baseline level
    TLine *cspPulseBaseline=new TLine(-1e6,
                                      cspwfvec->at(current_group).getBase(),
                                      1e6,
                                      cspwfvec->at(current_group).getBase());
    cspPulseBaseline->SetLineWidth(3);
    cspPulseBaseline->SetLineColor(kBlue);
    cspPulseBaseline->SetLineStyle(2);

    algorithm_draw_objects->Add(cspPulseBaseline);
    
  } else if(WfVectorType==TString("PMTWfVector")) {
    PMTWfVector *pmtwfvec=((PMTWfVector*)wfvec);

    /*    
    TArrow *pmtpulse=new TArrow(pmtwfvec->at(current_group).getWfMaxTime(),
                                pmtwfvec->at(current_group).getWfMax()+20.*pmtwfvec->at(current_group).getRMS(),
                                pmtwfvec->at(current_group).getWfMaxTime(),
                                pmtwfvec->at(current_group).getWfMax()+10.*pmtwfvec->at(current_group).getRMS());
    */

    TArrow *pmtPulsePosition=new TArrow(pmtwfvec->at(current_group).at(0).getPeakTime(),
                                        original_event->scopeData(current_chan,current_group)->GetMaximum()+20.*pmtwfvec->at(current_group).getRMS(),
                                        pmtwfvec->at(current_group).at(0).getPeakTime(),
                                        original_event->scopeData(current_chan,current_group)->GetMaximum()+10.*pmtwfvec->at(current_group).getRMS());

    
    pmtPulsePosition->SetLineWidth(3);
    pmtPulsePosition->SetLineColor(kRed);

    algorithm_draw_objects->Add(pmtPulsePosition);
    
    // draw a vertical line at the start of the identified pulse
    TLine *pmtPulseStartTime=new TLine(pmtwfvec->at(current_group).at(0).getStartTime(),
                                       -1e6,
                                       pmtwfvec->at(current_group).at(0).getStartTime(),
                                       1e6);
    pmtPulseStartTime->SetLineWidth(3);
    pmtPulseStartTime->SetLineColor(kRed);
    pmtPulseStartTime->SetLineStyle(2);
    
    algorithm_draw_objects->Add(pmtPulseStartTime);

    // draw a vertical line at the end of the identified pulse
    TLine *pmtPulseEndTime=new TLine(pmtwfvec->at(current_group).at(0).getEndTime(),
                                     -1e6,
                                     pmtwfvec->at(current_group).at(0).getEndTime(),
                                     1e6);
    pmtPulseEndTime->SetLineWidth(3);
    pmtPulseEndTime->SetLineColor(kRed);
    pmtPulseEndTime->SetLineStyle(2);
    
    algorithm_draw_objects->Add(pmtPulseEndTime);
    
    // draw a horizontal line at the baseline level
    TLine *pmtPulseBaseline=new TLine(-1e6,
                                      pmtwfvec->at(current_group).getBase(),
                                      1e6,
                                      pmtwfvec->at(current_group).getBase());
    pmtPulseBaseline->SetLineWidth(3);
    pmtPulseBaseline->SetLineColor(kBlue);
    pmtPulseBaseline->SetLineStyle(2);

    algorithm_draw_objects->Add(pmtPulseBaseline);
    
  } 
  else if(WfVectorType==TString("FastWfVector"))
  {
    FastWfVector *fastwfvec=((FastWfVector*)wfvec);
    
    if(fastwfvec->at(current_group).size()>0){
      TArrow *fastFirstPulsePosition=new TArrow(fastwfvec->at(current_group).at(0).getPeakTime(),
                                                original_event->scopeData(current_chan,current_group)->GetMaximum()+20.*fastwfvec->at(current_group).getRMS(),
                                                fastwfvec->at(current_group).at(0).getPeakTime(),
                                                original_event->scopeData(current_chan,current_group)->GetMaximum()+10.*fastwfvec->at(current_group).getRMS());
      
      
      fastFirstPulsePosition->SetLineWidth(3);
      fastFirstPulsePosition->SetLineColor(kRed);
      
      algorithm_draw_objects->Add(fastFirstPulsePosition);

      // draw a vertical line at the start of the identified pulse
      TLine *fastPulseStartTime=new TLine(fastwfvec->at(current_group).at(0).getStartTime(),
                                          -1e6,
                                          fastwfvec->at(current_group).at(0).getStartTime(),
                                          1e6);
      fastPulseStartTime->SetLineWidth(3);
      fastPulseStartTime->SetLineColor(kRed);
      fastPulseStartTime->SetLineStyle(2);
      
      algorithm_draw_objects->Add(fastPulseStartTime);
    }
    
    if(fastwfvec->at(current_group).size()>1){
      
      TArrow *fastSecondPulsePosition=new TArrow(fastwfvec->at(current_group).at(1).getPeakTime(),
                                                 original_event->scopeData(current_chan,current_group)->GetMaximum()+20.*fastwfvec->at(current_group).getRMS(),
                                                 fastwfvec->at(current_group).at(1).getPeakTime(),
                                                 original_event->scopeData(current_chan,current_group)->GetMaximum()+10.*fastwfvec->at(current_group).getRMS());
      
      
      fastSecondPulsePosition->SetLineWidth(3);
      fastSecondPulsePosition->SetLineColor(kRed);
      
      algorithm_draw_objects->Add(fastSecondPulsePosition);

      // draw a vertical line at the end of the identified pulse
      TLine *fastPulseEndTime=new TLine(fastwfvec->at(current_group).at(1).getEndTime(),
                                        -1e6,
                                        fastwfvec->at(current_group).at(1).getEndTime(),
                                        1e6);
      fastPulseEndTime->SetLineWidth(3);
      fastPulseEndTime->SetLineColor(kRed);
      fastPulseEndTime->SetLineStyle(2);
    
      algorithm_draw_objects->Add(fastPulseEndTime);
    }
    
    // draw a horizontal line at the baseline level
    TLine *fastPulseBaseline=new TLine(-1e6,
                                       fastwfvec->at(current_group).getBase(),
                                       1e6,
                                       fastwfvec->at(current_group).getBase());
    fastPulseBaseline->SetLineWidth(3);
    fastPulseBaseline->SetLineColor(kBlue);
    fastPulseBaseline->SetLineStyle(2);
    
    algorithm_draw_objects->Add(fastPulseBaseline);
  }
  else
  {
    // draw for this analysis class is not yet implemented
  }

  algorithm_draw_objects->Draw();

  //  alone_c->Update();
}
