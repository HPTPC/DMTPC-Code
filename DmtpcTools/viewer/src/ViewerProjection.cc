#include "ViewerProjection.hh" 
#include "TGLabel.h"
#include "ImageTransforms.hh"
#include "TCanvas.h" 


static const char * interpolation_names[] = 
{
  "bilinear",
  "bicubic"
}; 



ClassImp(dmtpc::viewer::ViewerLine); 
ClassImp(dmtpc::viewer::ViewerProjection); 


dmtpc::viewer::ViewerProjection::ViewerProjection(const TGWindow *p, UInt_t w, UInt_t h, ViewerLine * line,  std::list<ViewerProjection*> * store, std::list<ViewerLine*> * line_store)
  : TGMainFrame(p,w,h)
{

  _store = store; 
  _line_store = line_store; 
  color = line->GetLineColor(); 
  line->Connect("Modified()","ViewerProjection",this,"draw()"); 
  TGHorizontalFrame * hframe = new TGHorizontalFrame(this,w,h);  
  TGVerticalFrame * leftframe = new TGVerticalFrame(hframe,w-200,h);  
  canvas = new TRootEmbeddedCanvas("ProjCanvas",leftframe,w-200,h); 
  leftframe->AddFrame(canvas, new TGLayoutHints(kLHintsExpandX , 3,3,3,3)); 

  TGVerticalFrame *buttonFrame = new TGVerticalFrame(hframe,200,h); 

  buttonFrame->AddFrame(new TGLabel(buttonFrame,"Interpolation Method: "), new TGLayoutHints(kLHintsCenterX | kLHintsCenterY,5,0,3,4)); 
  interpolation_select = new TGComboBox(buttonFrame); 
  for (int i = 0; i < 2; i++)
  {
    interpolation_select->AddEntry(interpolation_names[i],i); 
  }
  interpolation_select->Layout(); 
  interpolation_select->Select(0); 
  interpolation_select->Connect("Selected(Int_t)","ViewerProjection",this,"InterpolationChanged(Int_t)");
  interpolation_select->Resize(80,20); 
  buttonFrame->AddFrame(interpolation_select, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 6,5,6,4)); 


  buttonFrame->AddFrame(new TGLabel(buttonFrame,"Projection Width (px): "), new TGLayoutHints(kLHintsCenterX | kLHintsCenterY,5,0,3,4)); 

  width_select= new TGNumberEntry(buttonFrame,40,3,-1, TGNumberFormat::kNESReal, TGNumberEntry::kNEAPositive); 
  buttonFrame->AddFrame(width_select, new TGLayoutHints(kLHintsExpandX | kLHintsCenterY , 0,5,3,4));
  width_select->Connect("ValueChanged(Long_t)","ViewerProjection",this,"WidthChanged(Long_t)");
  width_select->Connect("ValueSet(Long_t)","ViewerProjection",this,"WidthChanged(Long_t)");


  hframe->AddFrame(leftframe, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2)); 
  hframe->AddFrame(buttonFrame, new TGLayoutHints(kLHintsExpandY,2,2,2,2)); 
  AddFrame(hframe, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2)); 

  selected_int = 0; 
  projline = line; 


  mutex = new TMutex(); 

  SetWindowName("Projection Window"); 
  MapSubwindows(); 
  Resize(GetDefaultSize()); 
  MapWindow(); 

}

void dmtpc::viewer::ViewerProjection::draw()
{

 mutex->Lock(); 
 double width = width_select->GetNumber(); 
 const char * interpolation = interpolation_names[selected_int]; 

 double x0 = projline->GetX1();
 double x1 = projline->GetX2();
 double y0 = projline->GetY1();
 double y1 = projline->GetY2();

 TH1 *longi = 0;
 TH1 *tranv = 0; 

 dmtpc::image::transform::projectAlongLine(current_img, &longi, &tranv,x0,y0,x1,y1,width,interpolation); 

 if (!longi) return; 
 canvas->GetCanvas()->Clear(); 
 canvas->GetCanvas()->Divide(1,2); 
 canvas->GetCanvas()->cd(1); 
 longi->SetLineColor(color); 
 longi->DrawCopy(); 
 canvas->GetCanvas()->Update(); 
 canvas->GetCanvas()->cd(2); 
 tranv->SetLineColor(color); 
 tranv->DrawCopy(); 
 canvas->GetCanvas()->Update(); 

 delete longi; 
 delete tranv; 


 mutex->UnLock(); 
}

dmtpc::viewer::ViewerProjection::~ViewerProjection()
{
  _store->remove(this); 
  _line_store->remove(projline); 
  projline->Delete(); 
}
