#ifndef DMTPC_SKIM_VIEWER_PROJECTION_HH
#define DMTPC_SKIM_VIEWER_PROJECTION_HH

#include "TGFrame.h"
#include "TLine.h"
#include "TMutex.h"
#include "TRootEmbeddedCanvas.h"
#include "TGNumberEntry.h"
#include "TGComboBox.h"
#include "TVirtualX.h" 
#include "TPad.h" 
#include "TH2.h" 
#include <list>

namespace dmtpc
{

namespace viewer
{

class ViewerLine : public TLine, public TQObject 
{
 
  public: 
  ViewerLine(Double_t x0, Double_t x1, Double_t y0, Double_t y1) : TLine(x0,x1,y0,y1) {};   

  void Modified() { Emit("Modified()"); } 
  virtual void  ExecuteEvent(Int_t event, Int_t px, Int_t py) { TLine::ExecuteEvent(event, px, py); if (event == kButton1Down || event == kMouseMotion || event  == kButton1Motion || event == kButton1Up) {Modified();} } 
  virtual void  SetX1(Double_t x) { TLine::SetX1(x); Modified();}
  virtual void  SetX2(Double_t x) { TLine::SetX2(x); Modified();}
  virtual void  SetY1(Double_t y) { TLine::SetY1(y); Modified();}
  virtual void  SetY2(Double_t y) { TLine::SetY2(y); Modified();}
  ClassDef(ViewerLine,0); 
};


class ViewerProjection : public TGMainFrame
{

  public:
    ViewerProjection(const TGWindow *p, UInt_t w, UInt_t h, ViewerLine * line, std::list<dmtpc::viewer::ViewerProjection *> * store, std::list<dmtpc::viewer::ViewerLine*> * linestore ); 
    virtual ~ViewerProjection(); 
    void setImage(TH2 * img) { current_img = img; draw(); }
    void draw(); 
    void InterpolationChanged(Int_t id) { selected_int = id; draw();} 
    int color; 
    void WidthChanged(Long_t id __attribute__((unused))) { draw();} 

  private:
    TRootEmbeddedCanvas * canvas; 
    ViewerLine * projline; 
    TH2 *current_img; 
    TGNumberEntry * width_select; 
    TGComboBox * interpolation_select; 
    int selected_int;
    std::list<ViewerProjection*> * _store; 
    std::list<ViewerLine*> * _line_store; 
    TMutex * mutex; 


  ClassDef(ViewerProjection,0); 
}; 

}
}

#endif
