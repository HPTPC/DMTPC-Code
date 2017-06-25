#ifndef DMTPC_SKIM_VIEWER_FRAME
#define DMTPC_SKIM_VIEWER_FRAME

#define DEFAULT_SEARCH_DIR "/net/zwicky/dmtpc/data/:/net/zwicky/esata01/dmtpc/data/4sh/:/data/4sh/"

#include "TGFrame.h"
#include "SkimDataset.hh"
#include "SkimEvent.hh"
#include "TRootEmbeddedCanvas.h"
#include "TGNumberEntry.h"
#include "TGLabel.h"
#include <vector>
#include "TGraph.h"
//#include "ViewerScopeFrame.hh"
#include "ViewerImageTransform.hh"
#include "ViewerProjection.hh"
#include "TGSlider.h"
#include "TMarker.h"
#include "TArrow.h"
#include "TLine.h"
#include <fstream>
#include <list>
using std::ifstream;

namespace dmtpc
{

  namespace analysis
  {
    class Stitcher; 
    class Playlist;
    class MCTPCDataset; 
  }

namespace viewer
{
class ViewerFrame : public TGMainFrame
{
  public:

    ViewerFrame(const TGWindow *p, UInt_t w, UInt_t h); 
    virtual ~ViewerFrame(){;}
    void loadSkimFile(const char * file); 
    void loadRawFile(const char * file); 
    void loadPlaylist(const char * file); 
    void setOrigFile(const char * file); 

    void notSupportedForRaw(); 
    void Go(); 
    void CamChanged(Long_t ignore); 
    void TrackChanged(Long_t ignore); 
    void ZoomChanged(const char * ignore); 
    void HandleMenu(Int_t menu); 
    void Next();
    void Previous();
    void Traces();
    void CloseWindow(); 
    void TreeChanged(Int_t id); 

    void DoStitch(); 
    void ShowStitch(); 
    void ShowProjection(); 
    void CloseStitch(){ show_stitch = false; }

    Bool_t HandleKey(Event_t * event); 

    void ShowFFT(); 
    void UpdateFFT(); 
    void CloseFFT(){show_fft=false;}
    void CloseProjection(){show_projection=false;}

    void ShowLineProjection(); 

    void ShowTransform(const char * name); 
    void ShowBias(); 
    void UpdateBias(); 
    void UpdateProjection(); 
    void CloseBias(){show_bias=false;}

    void ShowOrig(); 
    void LoadOrig(); 
    void UpdateOrig(); 
    void CloseOrig(){show_orig=false;} 

    void ShowPixHist(); 
    void UpdatePixHist(); 
    void ClosePixHist() { show_pixhist = false; } 

    void ShowMosaic(); 
    void UpdateMosaic(); 
    void CloseMosaic(){show_mosaic=false;} 
    
    void ShowMCTracks();
    void ShowAlone(); 
    void UpdateAlone(); 
    void CloseAlone(){show_alone=false;} 

    void ShowLeftProj(); 
    void UpdateLeftProj(); 
    void CloseLeftProj(){ show_leftproj = false; } 

    void CanvasEvent(Int_t event,Int_t x,Int_t y,TObject* obj);

  private:
 
    void doGUI(); 
    void doKeys(); 

    void drawEvent(); 
    void updateEventLabels();
    void updateTrackLabels();
    void drawTracks(); 
    void showSaveDialog(); 
    void showFitsDialog(); 
    void loadOverlay(TString overlayFile); 
    void drawOverlay(); 
    void clearOverlays(); 

    TH2 * getImage();
    //Negative track for mc
    void drawTrack(int track, int color, int linewidth, bool red = false); 
   

    void updateZoomed(double x,double y); 

    bool loaded; 
    bool show_traces; 
    bool show_projection;
    bool show_leftproj; 
    bool show_bias; 
    bool show_fft; 
    bool show_orig; 
    bool show_mosaic; 
    bool show_alone; 
    bool show_outlines; 
    bool show_overlays; 
    bool show_zoom; 
    bool show_vector; 
    bool show_centroid; 
    bool show_mc_tracks; 
    bool show_stitch; 

//    ViewerScopeFrame * scopewindow; 
    dmtpc::analysis::SkimDataset * dataset;
    dmtpc::core::Dataset * raw_dataset;
    dmtpc::analysis::Playlist * playlist; 

    dmtpc::analysis::MCTPCDataset * mcd; 
    dmtpc::analysis::Stitcher * stitch; 

    
    TH2 * bias_subtracted; 
    int current_event;  
    int last_event; 
    int last_cam; 
    int current_cam; 
    int current_track; 
    std::string current_tree; 
    std::string last_tree; 
    std::vector<std::string> tree_names; 
    TRootEmbeddedCanvas * canvas; 
    TRootEmbeddedCanvas * zoom; 
    TGHSlider * zoom_level; 
    TH2 * zoomed; 
    TCanvas * bias_c; 
    TCanvas * fft_c; 
    TCanvas * orig_c; 
    TCanvas * mosaic_c; 
    TCanvas * alone_c;
    TCanvas * stitch_c; 
    TCanvas * projection_c; 
    TCanvas * leftproj_c; 
    TCanvas * pixhist_c; 

    TMarker * marker, * mc_marker;
    TArrow * arrow, * mc_arrow, * zoom_arrow, * zoom_mc_arrow;

    TH2D * fft_mag; 
    TH2D * fft_ph; 
    bool show_reduced; 
    bool show_pixhist; 

    TGNumberEntryField * ev_select; 
    TGNumberEntryField * minz; 
    TGNumberEntryField * maxz; 
    TGNumberEntry * cam_select; 
    TGNumberEntry * track_select; 
    TGComboBox * tree_select; 
    TGLabel * ev_properties;  
    TGLabel * track_properties; 
    std::list<TGraph> edges;
    std::vector<TGraph*> overlays;
    std::vector<int> overlay_cams; 

    double last_x; 
    double last_y; 

    std::list< ::dmtpc::viewer::ViewerImageTransform *> transforms; 

    std::list< ::dmtpc::viewer::ViewerProjection * > projections; 
    std::list< ::dmtpc::viewer::ViewerLine * > lines; 

    char * orig_data_file; //!
    bool using_playlist; 
    bool using_raw; 

    void unload(); 
    void find_orig_file_name(); 
    void setTreeNames(); 
    
    TH1 * projectionX;
    TH1 * projectionY;
    TH1 * projectionT;
    TH1 * projectionL;

    ClassDef(ViewerFrame,0); 
};

}
}


#endif 
