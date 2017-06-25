#include "ViewerFrame.hh"
#include "CameraInfo.hh"
#include "Event.hh"
#include "MCTPCDataset.hh" 
#include "ImageIO.hh"
#include "Playlist.hh"
#include "ImagePalettes.hh"
#include "TCanvas.h"
#include "Stitcher.hh" 
#include "ClusterAlgo.hh"
#include "TApplication.h"
#include "TSystem.h"
#include "KeySymbols.h"
#include "TGWindow.h"
#include "TGMenu.h"
#include "TGFileDialog.h"
#include "TGMsgBox.h"
#include "TGraph.h"
#include "TString.h"
#include "TF1.h"
#include "TEllipse.h"
#include "TStyle.h"
#include <sstream>
#include <map>


#define NOTRACESYET

static const char * filetypes_open_orig[] = { "Data Files","*.root",0,0};
static const char * filetypes_stitch[] = { "Stitcher Files","*stitch.root",0,0};
static const char * filetypes_open[] = { "Skim Files","*skim.root",0,0};
static const char * filetypes_raw[] = { "Raw Data Files","*.root",0,0};
static const char * filetypes_play[] = { "Playlist Files","*.play",0,0};
static const char * filetypes_fits[] = { "FITS Files","*.fit",0,0};
static const char * filetypes_save[] = { "PNG Image","*.png",
                                 "PDF File","*.pdf",
                                 "EPS File","*.eps",
                                 "PS File","*.ps",
                                 "GIF Image","*.gif",
                                 "JPEG Image","*.jpg",
                                 0,0};
static const char * filetypes_overlay[] = { "Overlay Files","*.overlay",0,0};


const char * get_basename(const char * f) 
{
  unsigned n = strlen(f); 

  for (int i = n-1; i >= 0; i--)
  {
    if (f[i] == '/')
      return f+i+1; 
  }

  return f; 

}
bool checkIfFileContainsTree(const char * file, const char * tree)
{

  TFile * f = TFile::Open(file); 
  if (!f) return false; 

  TObject * o = f->Get(tree); 
  if (!o)
  {
    f->Close(); 
    f->Delete(); 
    return false; 
  }

  bool istree  = o->InheritsFrom("TTree"); 


  f->Close(); 
  f->Delete(); 

  return istree; 

}



/** Most of DmtpcViewer is implemented here. Documentation is scant or non-existent.  There be dragons etc. **/

ClassImp(dmtpc::viewer::ViewerFrame); 


static TString sigfig(double d)
{
  return TString::Format("%.4g",d); 
}

static const int BGCOLOR = 0x969696; 
//static const int CANVAS_COLOR = 18; //grey
static const int CANVAS_COLOR = kWhite;

void dmtpc::viewer::ViewerFrame::setOrigFile(const char * f)
{
 if (orig_data_file) free(orig_data_file); 
 orig_data_file = strdup(f); 
}


void dmtpc::viewer::ViewerFrame::notSupportedForRaw()
{
    new TGMsgBox(gClient->GetRoot(),this,"Not Supported","The operation is not supported for raw files.");
}


void dmtpc::viewer::ViewerFrame::ShowLineProjection()
{
  double x0 = 0; 
  double x1 = 400; 
  double y0 = 100; 
  double y1 = 200; 

  if (!using_raw && dataset->event()->ntracks(current_cam) > 0)
  {
    dmtpc::analysis::clusteralgo::clusterBounds(dataset->event()->cluster(current_cam)->getCluster(current_track),&x0,&x1,&y0,&y1,4);
  }

  ViewerLine * line = new ViewerLine(x0,x1,y0,y1); 
  lines.push_back(line); 
  //pick color!
  
  int test_color = 2; 
  for (std::list<ViewerProjection*>::iterator it = projections.begin(); it != projections.end(); it++)
  {
    if ((*it)->color == test_color)
    {
      test_color++; 
      it = projections.begin(); 
    }
  }

  line->SetLineColor(test_color); 
  canvas->GetCanvas()->cd(); 
  line->Draw(); 
  ViewerProjection * p = new ViewerProjection(gClient->GetRoot(),600,600,line,&projections, &lines); 
  p->setImage(getImage()); 
  projections.push_back(p); 
  canvas->GetCanvas()->Update(); 
}


void dmtpc::viewer::ViewerFrame::DoStitch()
{
  if (!stitch)
  {
    new TGMsgBox(gClient->GetRoot(),this,"No Stitch For You","Load a stitch file pls");
    stitch_c->Close(); 
    show_stitch = false; 
    return; 
  }

  std::vector<const TH2*> imgs ( using_raw ? raw_dataset->event()->nccd() : dataset->event()->ncamera() ); 

  if (using_raw)
  {
    for (unsigned i = 0; i < raw_dataset->event()->nccd(); i++)
    {
      int idx = stitch->getIndex(raw_dataset->event()->ccdConfig(i)->serialNumber.Data()); 
      if (idx < 0)
      {
        new TGMsgBox(gClient->GetRoot(),this,"No Stitch For You","Camera index wrong!");
        return; 
      }

      char name[20]; 
      sprintf(name,"subtract_%d",i); 
      TH2 * subtracted = (TH2*) raw_dataset->event()->ccdData(i)->Clone(name); 
      subtracted->Add(raw_dataset->biasAvg(i),-1); 
      imgs[idx] = subtracted; 
    }
  } 
  else 
  {
    for (int i = 0; i < dataset->event()->ncamera(); i++)
    {
      int idx = stitch->getIndex(dataset->event()->cameraSerialNumber(i).Data()); 
      if (idx < 0)
      {
        new TGMsgBox(gClient->GetRoot(),this,"No Stitch For You","Camera index wrong!");
        return; 
      }
      imgs[idx] = dataset->event()->cluster(i)->getImage(); 
    }
  }

  TH2 * stitched = stitch->stitch(&imgs); 
  stitch_c->Clear(); 
  stitch_c->cd(); 
  stitched->SetMinimum(minz->GetNumber()); 
  stitched->SetMaximum(maxz->GetNumber()); 
  stitched->DrawCopy("colz"); 

  double inner_radius = 0; 
  double outer_radius = 0; 
  for (unsigned i = 0; i < imgs.size(); i++)
  {
    inner_radius += stitch->innerRadius(i); 
    outer_radius += stitch->outerRadius(i); 
    double c = stitch->getScale(i); 
    double th = stitch->getRotation(i); 
    double x0 = stitch->xCenter(i); 
    double y0 = stitch->yCenter(i); 
    
   
    for (unsigned s = 0; s < stitch->getNSpacers(i); s++ )
    {
//      cout << stitch << " " << i <<" " <<  s <<  endl; 
      double m = stitch->getSpacerSlope(i,s); 
      double b = stitch->getSpacerIntercept(i,s); 
      TF1 f("f","1/(cos([1]) + [0]*sin([1])) * (x * ([0] * cos([1]) - sin([1])) - [2] * [3] + [0]*[3]*[4] + [3]*[5])",-c * (x0*cos(th) + y0*sin(th)), c * ((1024-x0)*cos(th) + (1024-y0)*sin(th)) ); 
      f.SetParameters(m,th,y0,c,x0,b); 
      f.SetLineColor(20+i); 
      f.SetLineWidth(2); 
      f.SetLineStyle(7); 
      f.DrawCopy("same"); 
    }
  }

  inner_radius /= imgs.size(); 
  outer_radius /= imgs.size();

  TEllipse inner(0,0, inner_radius, inner_radius); 
  inner.SetFillStyle(0); 
  inner.SetLineColor(0); 
  inner.SetLineWidth(2); 
  TEllipse outer(0,0, outer_radius, outer_radius); 
  outer.SetFillStyle(0); 
  outer.SetLineColor(0); 
  outer.SetLineWidth(3); 
  inner.Draw("same"); 
  outer.Draw("same"); 

  stitch_c->Update(); 
  stitched->Delete(); 

  if (using_raw)
  {
    for (unsigned i = 0; i < imgs.size(); i++)
    {
      ((TH2*) imgs[i])->Delete(); 
    }
  }
 
}

void dmtpc::viewer::ViewerFrame::setTreeNames()
{
  tree_select->RemoveAll(); 
  tree_names.clear(); 

  if (!loaded || using_raw) 
  {
    return; 
  }

  int skim_id = 0; 
  const std::map<std::string,unsigned> * ind = dataset->getTreeIndices(); 
  for (unsigned i = 0; i < ind->size(); i++)
  {
    tree_names.push_back(std::string("")); 
  }
  for (std::map<std::string,unsigned>::const_iterator it = ind->begin(); it != ind->end(); it++)
  {
    unsigned this_id = (*it).second; 
    std::string this_key = (*it).first; 
    if (this_key == "skim") skim_id = this_id; 
    tree_names[this_id] = this_key; 
    tree_select->AddEntry(this_key.c_str(), this_id); 
  }

  tree_select->Layout(); 
  tree_select->Select(skim_id); 
}

//Keyboard down handler
//TODO:  Make control configurable? 
Bool_t dmtpc::viewer::ViewerFrame::HandleKey(Event_t * event) 
{
  if (event->fType == kGKeyPress)
  {

    UInt_t keysym; 
    char tmpstr[2]; 
    gVirtualX->LookupString(event,tmpstr,sizeof(tmpstr),keysym); 
    keysym &= ~0x20; 

    if (event->fState & kKeyControlMask && keysym == kKey_S )
    {
      showSaveDialog();
    }

    else if (keysym == kKey_A)
    {
      Previous(); 
    }

    else if (keysym == kKey_H)
    {
      ShowPixHist(); 
    }

    else if ( keysym == kKey_D)
    {
      Next(); 
    }

    else if ( keysym == kKey_W)
    {
      cam_select->SetIntNumber(++current_cam); 
      CamChanged(0); 
    }

    else if ( keysym == kKey_S)
    {
      cam_select->SetIntNumber(--current_cam); 
      CamChanged(0); 
    }

    else if ( keysym == kKey_Q)
    {
      if (!using_raw)
      {
        track_select->SetIntNumber(current_track-1 < 0 ? current_track : --current_track ); 
        TrackChanged(0); 
      }
    }

    else if ( keysym == kKey_E)
    {
      if (!using_raw)
      {
        track_select->SetIntNumber(current_track+1 >= dataset->event(current_tree.c_str())->ntracks(current_cam) ? current_track : ++current_track); 
        TrackChanged(0); 
      }
    }

    else if ( keysym == kKey_M)
    {
      ShowMCTracks(); 
    }

    else if ( keysym == kKey_C)
    {
      if (!using_raw)
      {
        show_centroid =! show_centroid; 
        drawEvent();
      }
    }

    else if ( keysym == kKey_V)
    {
      if (!using_raw)
      {
        show_vector =! show_vector; 
        drawEvent();
      }
    }
  }

  //Make sure children get their bindings run... 

  if (!fBindList) return kFALSE;

  TIter next(fBindList);
  TGMapKey *m;
  TGFrame  *w = 0;

  while ((m = (TGMapKey *) next())) {
    if (m->fKeyCode == event->fCode) {
      w = (TGFrame *) m->fWindow;
      if (w->HandleKey(event)) return kTRUE;
    }
  }

  return kFALSE;
}

// Canvas event handler
void dmtpc::viewer::ViewerFrame::CanvasEvent(Int_t event,Int_t px,Int_t py,TObject* ob __attribute__((unused)))
{

  TCanvas *c = (TCanvas*) gTQSender; 
  TPad *pad = (TPad*) c->GetSelectedPad(); 
  //Select track j
  if (loaded && event == kButton1Down && !using_raw)
  {
    double x = pad->PadtoX(pad->AbsPixeltoX(px));
    double y = pad->PadtoY(pad->AbsPixeltoY(py));

    const dmtpc::analysis::ClusterImage * ci = dataset->event()->cluster(current_cam); 

    int bin = ((TH2*)ci->getImage())->FindBin(x,y); 

    for (int i = 0; i < dataset->event()->ntracks(current_cam); i++)
    {
      if (dmtpc::analysis::clusteralgo::inCluster(ci->getCluster(i), bin))
      {
        current_track = i; 
        track_select->SetIntNumber(i); 
        drawTracks(); 
        if (show_projection)
          UpdateProjection(); 
        if (show_leftproj)
        {
          UpdateLeftProj(); 
        }
        updateTrackLabels(); 
        break; 
      }
    }
  }

  else if (loaded && show_zoom && zoomed!=NULL && event == kMouseMotion)
  {
    double x = pad->PadtoX(pad->AbsPixeltoX(px));
    double y = pad->PadtoY(pad->AbsPixeltoY(py));

    last_x = x;
    last_y = y;

    updateZoomed(x,y); 
  }

}

void dmtpc::viewer::ViewerFrame::updateZoomed(double x, double y)
{
    TH2 * img = getImage(); 

    int bin = img->FindBin(x,y); 

    int binx,biny,binz; 

    img->GetBinXYZ(bin,binx,biny,binz); 


    //Clear zoom if you go off the image
    if (binx <1 || binx > img->GetNbinsX()  
        || biny <1 || biny > img->GetNbinsY())
    {
      zoom->GetCanvas()->Clear(); 
      zoom->GetCanvas()->Update(); 
      return; 
    }

    int zlevel = zoom_level->GetPosition();

    int shiftx = 0; 
    int shifty = 0; 

    if (binx - zlevel < 1 ) shiftx = zlevel - binx + 1; 
    if (biny - zlevel < 1 ) shifty = zlevel - biny + 1; 
    if (binx + zlevel > img->GetNbinsX()) shiftx = img->GetNbinsX() - zlevel - binx -1 ; 
    if (biny + zlevel > img->GetNbinsY()) shifty = img->GetNbinsY() - zlevel - biny -1 ; 

    zoomed->GetXaxis()->SetRange(binx-zlevel+shiftx,binx +zlevel+shiftx); 
    zoomed->GetYaxis()->SetRange(biny-zlevel+shifty,biny+zlevel+shifty); 

    zoom->GetCanvas()->cd(); 
    zoomed->Draw("colz"); 

    if (show_outlines)
    {
      for (std::list<TGraph>::iterator i = edges.begin(); i!=edges.end() ;i++) (*i).Draw("lsame"); 
    }
    if (show_vector && zoom_arrow) zoom_arrow->Draw(); 
    if (show_vector && zoom_mc_arrow) zoom_mc_arrow->Draw(); 
    if (show_centroid && marker) marker->Draw(); 
    if (show_centroid && mc_marker) mc_marker->Draw(); 

    for (int i = 0; i < (int) overlays.size(); i++)
    {
       if (overlay_cams[i] == current_cam)
       {
         overlays[i]->Draw("l same");
       }
    }

    zoom->GetCanvas()->Update(); 
}


void dmtpc::viewer::ViewerFrame::ShowLeftProj()
{
  /*
  if (!loaded) return; 
  if (show_leftproj)
  {
    leftproj_c->Close(); 
    show_leftproj = false; 
    return; 
  }

  leftproj_c = new TCanvas("leftproj_c","Left Projection", 600, 400);
  leftproj_c->Connect("Closed()","dmtpc::viewer::ViewerFrame",this,"CloseLeftProj()"); 
  show_leftproj = true; 
  UpdateLeftProj(); 
  */
}

void dmtpc::viewer::ViewerFrame::UpdateLeftProj()
{

  /*
  if (!loaded) return; 
  if (!show_leftproj) return; 
  leftproj_c->Clear(); 

  if (using_raw)
  {
    ShowLeftProj(); 
    notSupportedForRaw(); 
    return; 
  }

//  std::cout << dataset->event()->ntracks(current_cam) << std::endl;
  if (dataset->event()->ntracks(current_cam) > 0) 
  {
     leftproj_c->cd(); 

     TH1 * leftproj = dataset->event()->cluster(current_cam)->getLeftProj(current_track, 0, true,0.01); 

     double baseline_estimate = MaxCamImageTools::median(getImage()); 
//     std::cout << baseline_estimate << std::endl; 
     double raw_integral = leftproj->Integral(); 
     double integral = raw_integral - baseline_estimate * leftproj->GetNbinsX() * dataset->event()->x(current_cam,current_track);   
     int color = integral > dataset->event()->E(current_cam,current_track) * 0.3 ? 2 : 1; 
     leftproj->SetLineColor(color); 
     leftproj->DrawCopy(); 
     leftproj->Delete(); 
  }
  leftproj_c->Update(); 
  */
}


void dmtpc::viewer::ViewerFrame::UpdateProjection()
{
  if (!loaded) return; 
  if (!show_projection) return; 
  projection_c->Clear(); 
  projection_c->Divide(1,4); 

  if (using_raw)
  {
    ShowProjection(); 
    notSupportedForRaw(); 
    return; 
  }

  if (projectionX) { projectionX->Delete(); projectionX = 0; }
  if (projectionY) { projectionY->Delete(); projectionY = 0; }
  if (projectionT) { projectionT->Delete(); projectionT = 0; } 
  if (projectionL) { projectionL->Delete(); projectionL = 0; } 

  gStyle->SetOptTitle(1); 
  if (dataset->event()->ntracks(current_cam) > 0)
  {
    projection_c->cd(1); 
    projectionL = dmtpc::analysis::clusteralgo::project(dataset->event()->cluster(current_cam)->getCluster(current_track), dataset->event()->phi(current_cam,current_track), "bicubic"); 
    projectionL->SetTitle("Longitudinal Projection"); 
    projectionL->UseCurrentStyle();
    projectionL->Draw(); 

    projection_c->cd(2); 
    projectionT = dmtpc::analysis::clusteralgo::project(dataset->event()->cluster(current_cam)->getCluster(current_track), dataset->event()->phi(current_cam,current_track) + M_PI/2.,"bicubic"); 
    projectionT->SetTitle("Transverse Projection"); 
    projectionT->UseCurrentStyle();
    projectionT->Draw(); 

    projection_c->cd(3); 
    TH2 * clusterhist = dmtpc::analysis::clusteralgo::clusterHist(dataset->event()->cluster(current_cam)->getCluster(current_track),1); 
    projectionX = clusterhist->ProjectionX(); 
    projectionX->SetTitle("X Projection"); 
    projectionX->UseCurrentStyle();
    projectionX->Draw(); 

    projection_c->cd(4); 
    projectionY = clusterhist->ProjectionY(); 
    projectionY->SetTitle("Y Projection"); 
    projectionY->UseCurrentStyle();
    projectionY->Draw(); 

    clusterhist->Delete(); 
  }
  projection_c->Update(); 

  gStyle->SetOptTitle(0); 

  return; 
}

void dmtpc::viewer::ViewerFrame::UpdateFFT()
{
  if (!loaded) return; 
#ifdef HAVE_FFT
  fft_c->Clear(); 
  fft_c->Divide(2,1); 
  
  fft_mag =(TH2D*) MaxCamImageTools::fftshift((TH2*) getImage()->FFT((TH1*)fft_mag,"MAG")); 
  fft_ph = (TH2D*) MaxCamImageTools::fftshift((TH2*) getImage()->FFT((TH1*)fft_ph,"PH")); 

  if (fft_mag == NULL || fft_ph == NULL )
  {
    cout << "FFT Hist is NULL" << endl; 
    new TGMsgBox(gClient->GetRoot(),this,"Could not compute FFT.","FFT histogram is null. Perhaps this machine doesn't have FFTW3 support yet? See Readme.");
    ShowFFT(); 
    return; 
  }
  fft_c->cd(1);
  fft_mag->GetXaxis()->SetTitle("Magnitude"); 
  fft_mag->Draw("colz"); 
  fft_c->cd(2);
  fft_ph->GetXaxis()->SetTitle("Phase"); 
  fft_ph->Draw("colz"); 
  fft_c->Update(); 

#endif
}


void dmtpc::viewer::ViewerFrame::UpdatePixHist()
{
  if (!loaded) return; 
  if (!show_pixhist) return; 

  int xmin = getImage()->GetXaxis()->GetFirst(); 
  int ymin = getImage()->GetYaxis()->GetFirst(); 
  int xmax = getImage()->GetXaxis()->GetLast(); 
  int ymax = getImage()->GetYaxis()->GetLast(); 

  vector<double> vals; 
  double min = 1e99; 
  double max = -1e99; 
  for (int x = xmin; x <= xmax; x++)
  {
    for (int y = ymin; y <= ymax; y++)
    {
       double val = getImage()->GetBinContent(x,y);  

       if (val < min) min = val; 
       if (val > max) max = val; 
       vals.push_back(val); 
    }
  }

  int nbins = int(max - min + 0.5); 

  TH1F ph("ph","ph",nbins,min,max); 

  for (unsigned i = 0; i < vals.size(); i++)
  {
    ph.Fill(vals[i]); 
  }

  pixhist_c->cd(); 
  ph.SetLineColor(1); 
  ph.SetFillColor(1); 
  ph.DrawCopy(); 
  pixhist_c->Update(); 
}


void dmtpc::viewer::ViewerFrame::ShowPixHist()
{

  if(!loaded) return; 

  if (show_pixhist)
  {
    pixhist_c->Close(); 
    show_pixhist = false; 
    return; 
  }

  pixhist_c = new TCanvas("pixhist_c", "Pixel Histogram",600,600); 
  pixhist_c->Connect("Closed()","dmtpc::viewer::ViewerFrame", this, "ClosePixHist()"); 
  show_pixhist = true; 
  UpdatePixHist(); 
}

void dmtpc::viewer::ViewerFrame::ShowProjection()
{
  if(!loaded) return; 

  if (show_projection)
  {
    projection_c->Close();
    show_projection= false; 
    return; 
  }

  projection_c = new TCanvas("projection_c","Cluster Projection",600,800); 
  projection_c->Connect("Closed()","dmtpc::viewer::ViewerFrame",this,"CloseProjection()"); 
  show_projection = true; 

  UpdateProjection(); 
}


void dmtpc::viewer::ViewerFrame::ShowFFT()
{
  if(!loaded) return; 
#ifdef HAVE_FFT
  if (show_fft)
  {
    fft_c->Close(); 
    show_fft = false; 
    fft_mag->Delete(); 
    fft_ph->Delete(); 
    return; 
  }

  fft_c = new TCanvas("fft_c","FFT Window",800,400); 
  fft_c->Connect("Closed()","dmtpc::viewer::ViewerFrame",this,"CloseFFT()"); 
  show_fft = true;

  int nx = getImage()->GetNbinsX(); 
  int ny = getImage()->GetNbinsY(); 
  fft_mag = new TH2D ("fft_mag","fft_mag",nx,0,nx,ny,0,ny); 
  fft_ph= new TH2D("fft_ph","fft_phase",nx,0,nx,ny,0,ny); 

  UpdateFFT(); 
#endif
}

void dmtpc::viewer::ViewerFrame::UpdateBias()
{
  if (!show_bias) return; 
  bias_c->cd(); 

  using_raw ? ((TH2*)raw_dataset->biasAvg(current_cam))->Draw("colz") : ((TH2*)dataset->getBiasFrame(current_cam))->Draw("colz"); 
  bias_c->Update(); 
}

void dmtpc::viewer::ViewerFrame::ShowBias()
{
  if (!loaded) return; 
  if (show_bias)
  {
    bias_c->Close(); 
    show_bias = false; 
    if (!using_raw) dataset->loadBiasFrames(false); 
    return; 
  }

  if (!using_raw) dataset->loadBiasFrames(true); 
  bias_c = new TCanvas("bias_c","Bias Frame",400,400); 
  bias_c->Connect("Closed()","dmtpc::viewer::ViewerFrame",this,"CloseBias()"); 
  show_bias = true;
  UpdateBias(); 
}

void dmtpc::viewer::ViewerFrame::UpdateOrig()
{
  if (!show_orig) return; 

  if (using_raw)
  {
    orig_c->cd();
    ((TH2*)raw_dataset->event()->ccdData(current_cam))->Draw("colz"); 
    orig_c->Update(); 
    return; 
  }

  if (dataset->orig_event()==NULL) 
  {
    new TGMsgBox(gClient->GetRoot(),this,"File Load Error",
         "Could not load original image. ");
    return;
  }

  orig_c->cd();
  ((TH2*)dataset->orig_event()->ccdData(current_cam))->Draw("colz"); 
  orig_c->Update(); 

}

void dmtpc::viewer::ViewerFrame::UpdateMosaic()
{
  if (!show_mosaic) return; 

  std::vector<const TH2*> imgs ( using_raw ? raw_dataset->event()->nccd() : dataset->event()->ncamera() ); 

  if (using_raw)
    {
    for (unsigned i = 0; i < raw_dataset->event()->nccd(); i++)
    {
      char name[20]; 
      sprintf(name,"subtract_%d",i); 
      TH2 * subtracted = (TH2*) raw_dataset->event()->ccdData(i)->Clone(name); 
      subtracted->Add(raw_dataset->biasAvg(i),-1); 
      imgs[i] = subtracted; 
    }
  } 
  else 
  {
    for (int i = 0; i < dataset->event()->ncamera(); i++)
    {
      imgs[i] = dataset->event()->cluster(i)->getImage(); 
    }
  }

  gStyle->SetOptTitle(1);
  for (unsigned i = 0; i < imgs.size(); i++)
    {
      mosaic_c->cd(i+1);
      ((TH2*)imgs[i])->SetMinimum(minz->GetNumber()); 
      ((TH2*)imgs[i])->SetMaximum(maxz->GetNumber()); 

      // display serial # with image
      ((TH2*)imgs[i])->SetTitle((using_raw ? 
				  raw_dataset->event()->ccdConfig(i) ? raw_dataset->event()->ccdConfig(i)->serialNumber : ""
				  : dataset->event(current_tree.c_str())->cameraSerialNumber(i) ));
      if (i==0) {  // append the run and event numbers to first image (rX eX)
	TString title = ((TH2*)imgs[0])->GetTitle();
	title+="  r";
	title +=(using_raw ? raw_dataset->event()->run() : dataset->event(current_tree.c_str())->runNumber());
	title+="  e";
	title +=(using_raw ? raw_dataset->event()->ev() : dataset->event(current_tree.c_str())->eventNumber());
	((TH2*)imgs[0])->SetTitle(title);      
      }
      imgs[i]->DrawCopy("COLZ");
    }

  
  if (using_raw)
    {
      for (unsigned i = 0; i < imgs.size(); i++)
	{
	  delete imgs[i]; 
	}
    }

  mosaic_c->Update(); 
  gStyle->SetOptTitle(0);
}

void dmtpc::viewer::ViewerFrame::ShowMCTracks()
{
  if (!loaded) return; 
  if (show_mc_tracks)
  {
    show_mc_tracks = false; 
    delete mcd; 
    drawEvent(); 
    return; 
  }

  if (!orig_data_file)
  {
    find_orig_file_name();
  }

  if (!mcd) mcd = new dmtpc::analysis::MCTPCDataset(); 

  if (mcd->open(orig_data_file) ) 
  {
    show_mc_tracks=true; 
    drawEvent(); 
  }
  else
  {
    new TGMsgBox(gClient->GetRoot(),this,"File Load Error",
         "Not an MC file");

  }
  
}


void dmtpc::viewer::ViewerFrame::LoadOrig()
{
  if (orig_data_file!=NULL) free(orig_data_file);
//  if (orig_data_file==NULL)
//  {
    find_orig_file_name();
//  }
  if (using_raw) return; 
  
  dataset->loadEvent(true,orig_data_file); 
}

void dmtpc::viewer::ViewerFrame::ShowOrig()
{
  if(!loaded) return; 
  if (show_orig)
  {
    orig_c->Close(); 
    show_orig = false; 
    return; 
  }

  LoadOrig(); 

  orig_c = new TCanvas("orig_c","Original Image",400,400); 
  orig_c->Connect("Closed()","dmtpc::viewer::ViewerFrame",this,"CloseOrig()"); 
  show_orig = true;
  UpdateOrig(); 
}

void dmtpc::viewer::ViewerFrame::ShowMosaic()
{
  if (show_mosaic)
  {
    mosaic_c->Close(); 
    show_mosaic = false; 
    return; 
  }

  // size here assumes 4 cameras, but this gets changed in UpdateMosaic 
  // if it needs to be
  mosaic_c = new TCanvas("mosaic_c","Mosaic Image",800,800); 
  mosaic_c->Connect("Closed()","dmtpc::viewer::ViewerFrame",this,"CloseMosaic()"); 
  show_mosaic = true;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // configure the # of mosaic pads
  // size window so that each image is canvas_size_for_one_camera x canvas_size_for_one_camera; if two images, stack them on top of each other 
  // vertically
  Double_t ww=mosaic_c->GetWindowWidth(); // Double_t wh=mosaic_c->GetWindowHeight();
  
  // if the mosaic canvas is the wrong size for the # of cameras that are in this event, 
  // resize it.  If there are two cameras, put the images on top of each other.  Otherwise,
  // make a block.
  Int_t canvas_size_for_one_camera=400;
  Int_t ncameras=( using_raw ? raw_dataset->event()->nccd() : dataset->event()->ncamera() );
  if(ncameras ==1){
    mosaic_c->SetWindowSize(canvas_size_for_one_camera /*width*/ , canvas_size_for_one_camera /*height*/ );
  } else {
    Int_t xdiv=( ( ncameras + ncameras %2 )/2 );
    if( ww!=xdiv*canvas_size_for_one_camera ) {
      mosaic_c->SetWindowSize( xdiv*canvas_size_for_one_camera /*width*/ , 2*canvas_size_for_one_camera /*height*/ );
    }
    mosaic_c->Divide( xdiv , 2 );
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  UpdateMosaic(); 
}

void dmtpc::viewer::ViewerFrame::find_orig_file_name()
{

    if(using_raw)
    {
      orig_data_file = strdup(raw_dataset->getFile()->GetName());
      return; 
    }

    //Must figure out filename of original file or prompt if can't figure out
    //
    //    TString n; 
    //    n+=TString(DEFAULT_SEARCH_DIR); 

    TString default_directories; 
    default_directories+=TString(DEFAULT_SEARCH_DIR); 
    
    Ssiz_t from = 0;
    TString n;

    TString f = TString(
           get_basename(dataset->getFileName())
         ).ReplaceAll("skim","");
    FileStat_t fstat; 

    while(default_directories.Tokenize(n,from,":")){
      
      //Figure out if the detector has a det tag 
      //if it does
      //the file name will be of the form
      //dmtpc_tag_xxxxx.root
      if (f.Data()[5] == '_')
	{
	  for (int i = 6; i < 9; i++)
	    {
	      n+=f.Data()[i]; 
	    }
	  n+= "/"; 
	}
      
      n+=f;
      
      cout <<"Trying original file: " << n << endl; 
      if (!gSystem->GetPathInfo(n.Data(),fstat))
	{
	  orig_data_file = strdup(n.Data()); 
	  return; 
	}
    }

    //If file not found, maybe it's monte carlo
    f = TString(dataset->getFileName()).ReplaceAll("skim","");  
    f = f.ReplaceAll("AnalysisFramework/v3/","MaxCam/Simulations/v1/output/data"); 
    f = f.ReplaceAll("AnalysisFramework/v4/","MaxCam/Simulations/v1/output/data"); 
    cout << "Trying original file: " << f << endl;  

    if (!gSystem->GetPathInfo(f.Data(),fstat))
    {
      orig_data_file = strdup(f.Data()); 
      return; 
    }

    //Otherwise, prompt

   TGFileInfo fi; 
   fi.fFileTypes=filetypes_open_orig;
   fi.fIniDir = strdup(DEFAULT_SEARCH_DIR); 
   new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fi); 
   if (strcmp(fi.fFilename,"")==0) return; 
   orig_data_file = strdup(fi.fFilename); 
}

void dmtpc::viewer::ViewerFrame::UpdateAlone()
{
  if (!show_alone) return; 
  alone_c->cd(); 
  getImage()->Draw("colz"); 
  alone_c->Update(); 

}

void dmtpc::viewer::ViewerFrame::ShowStitch()
{
  if(!loaded) return; 
  if (show_stitch)
  {
    stitch_c->Close(); 
    show_stitch=false; 
    return; 
  }

  stitch_c = new TCanvas("stitch_c","Stitched (Be Patient!)",800,800); 
  stitch_c->Connect("Closed()","dmtpc::viewer::ViewerFrame",this,"CloseStitch()"); 
  show_stitch = true; 
  DoStitch(); 
  return;
}

void dmtpc::viewer::ViewerFrame::ShowAlone()
{
  if(!loaded) return; 
  if (show_alone)
  {
    alone_c->Close(); 
    show_alone=false; 
    return; 
  }

  alone_c = new TCanvas("alone_c","Standalone Image",400,400); 
  alone_c->Connect("Closed()","dmtpc::viewer::ViewerFrame",this,"CloseAlone()"); 
  show_alone = true; 
  UpdateAlone(); 
  return;
}

void dmtpc::viewer::ViewerFrame::CloseWindow()
{
  gApplication->Terminate(0); 
}

void dmtpc::viewer::ViewerFrame::Traces()
{

  new TGMsgBox(gClient->GetRoot(), this,"Not implemented yet","Traces not implemented yet in viewer 2.0"); 

#ifndef NOTRACESYET

  if (show_traces)
  {
    show_traces = false; 
    scopewindow->CloseWindow(); 
  }
  else
  {
    if (!loaded) return; 
    show_traces = true;  
    LoadOrig();
    scopewindow = new DmtpcViewerScopeFrame(gClient->GetRoot(),800,400,&show_traces); 
    if (loaded)
      scopewindow->Display(using_raw ? raw_dataset->event() : dataset->orig_event(),
                           using_raw ? NULL :dataset->event()->waveform_vectors()); 
  }
#endif

}

void dmtpc::viewer::ViewerFrame::updateTrackLabels()
{

  if (using_raw || current_track >= dataset->event()->ntracks(current_cam) || current_track <0 )
  {
    track_properties->SetText("\n\n\n\n\n\nNo current track\n\n\n\n\n\n"); 
    return;
  }

  TString text = "Track "; 
  text += current_track;

  text += "\nE: "; 
  text += sigfig(dataset->event(current_tree.c_str())->E(current_cam,current_track)); 
  text += "\nRange: "; 
  text += sigfig(dataset->event(current_tree.c_str())->range(current_cam,current_track)); 
  text += "\nPos: "; 
  text += sigfig(dataset->event(current_tree.c_str())->x(current_cam,current_track)); 
  text += ","; 
  text += sigfig(dataset->event(current_tree.c_str())->y(current_cam,current_track)); 
  text += "\nPhi: ";
  text += sigfig(dataset->event(current_tree.c_str())->phi(current_cam,current_track)); 
  text += "\nSkewness: "; 
  text += sigfig(dataset->event(current_tree.c_str())->skewness(current_cam,current_track)); 
  text += "\nEdge: "; 
  text += dataset->event(current_tree.c_str())->edge(current_cam,current_track); 
  text += "\nCluster RMS: "; 
  text += sigfig(dataset->event(current_tree.c_str())->cluster_rms(current_cam,current_track)); 
  text += "\nCluster Mean: "; 
  text += sigfig(dataset->event(current_tree.c_str())->cluster_mean(current_cam,current_track)); 
  text += "\nNeighbors: "; 
  text += dataset->event(current_tree.c_str())->neighbors(current_cam,current_track); 
  text += "\nNPixel: "; 
  text += dataset->event(current_tree.c_str())->npixel(current_cam,current_track); 
  text += "\nMaxPixel: "; 
  text += sigfig(dataset->event(current_tree.c_str())->maxpixel(current_cam,current_track)); 
  text += "\nNBurnin: "; 
  text += dataset->event(current_tree.c_str())->nburnin(current_cam,current_track); 

  track_properties->SetText(text.Data()); 

  track_properties->Layout(); 

}

void dmtpc::viewer::ViewerFrame::updateEventLabels()
{
  TString text = "Run "; 
  text+= using_raw ? raw_dataset->event()->run() : dataset->event(current_tree.c_str())->runNumber(); 
  text += ", Event "; 
  text += using_raw ? raw_dataset->event()->ev() : dataset->event(current_tree.c_str())->eventNumber(); 
  text += "\nCam: "; 
  text += current_cam; 
  text += " (";
  text += using_raw ? 
        raw_dataset->event()->ccdConfig(current_cam) ? raw_dataset->event()->ccdConfig(current_cam)->serialNumber : ""
        : dataset->event(current_tree.c_str())->cameraSerialNumber(current_cam);   
  text += ")"; 
  text +="\nNTracks: "; 
  if (using_raw)
    text +=  "N/A";  
  else 
    text += dataset->event(current_tree.c_str())->ntracks(current_cam); 

  // MaxCamTriggerGroups no longer stored in skim data files
  //  text +="\nNTriggerGroups: "; 
  //  text += dataset->event()->trigger_groups()->GetEntries(); ; 

  text +=", NTriggers: "; 

  if (!using_raw)
  {
    if(dataset->orig_event()==NULL){
      text += "NULL"; 
    } else {
#ifndef NOTRACESYET
      if(dataset->orig_event()->scopeData()->GetEntries()!=0){
        text += dataset->orig_event()->scopeDataInfo(0)->getNTriggers();
      } else {
        text += 0;
      }
#endif
    }
    text +="\nImage Mean: "; 
    text += sigfig(dataset->event(current_tree.c_str())->image_mean(current_cam)); 
    text +="\nImage RMS: "; 
    text += sigfig(dataset->event(current_tree.c_str())->image_rms(current_cam)); 
    text +="\nPixels Killed: "; 
    text += dataset->event(current_tree.c_str())->pixels_killed(current_cam); 
    text +="\nSpark: "; 
    text += dataset->event(current_tree.c_str())->spark(current_cam); 
  }
  else 
  {

#ifndef NOTRACESYET
    if (raw_dataset->event()->scopeData()->GetEntries()!=0)
    {
        text += raw_dataset->event()->scopeDataInfo(0)->getNTriggers();
    }
    else
    {
      text += 0; 
    }

#endif
    text += "\n\n\n\n"; 
  }

  if (show_mc_tracks)
  {
    text +="\nMC Er: "; 
    text += sigfig(mcd->truth()->recoilEnergy); 
    text +="\nMC Ei: "; 
    text += sigfig(mcd->truth()->ionization); 
    text +="\nMC nadu: "; 
    text += sigfig(mcd->truth()->nadu[current_cam]); 
    text +="\nMC ne: "; 
    text += sigfig(mcd->truth()->nprimary); 
    text +="\nMC Phi: "; 
    text += sigfig(mcd->truth()->phi()); 
    text +="\nMC Theta: "; 
    text += sigfig(mcd->truth()->theta()); 
    text +="\nMC Opening: "; 
    text += sigfig(mcd->truth()->opening()); 
    text +="\nMC X,Y,Z (mm): "; 
    text += sigfig(mcd->truth()->x0);
    text += ",\n";
    text += sigfig(mcd->truth()->y0);
    text += ",\n";
    text += sigfig(mcd->truth()->z0); 
  } 
  else 
  {
      text+="\n\n\n\n\n\n\n\n"; 
  }

  ev_properties->SetText(text.Data()); 
  ev_properties->Layout(); 
  updateTrackLabels(); 
}


// Draws images. Complicated! 
void dmtpc::viewer::ViewerFrame::drawEvent()
{

  if (!loaded) return; 

  ev_select->SetIntNumber(current_event); 

  if (using_playlist)
  {
    dataset = playlist->getDataset(); 
    dataset->loadBiasFrames(show_bias); 
    current_cam = playlist->getCam(); 
    current_track = playlist->getTrack(); 
    cam_select->SetLimitValues(current_cam,current_cam); 

    if (show_orig || show_mc_tracks || show_traces)
    {

      free(orig_data_file);
      orig_data_file = NULL;
      find_orig_file_name(); 
      if (show_orig||show_traces){
        dataset->loadEvent(true,orig_data_file);              
      }
      else{
        mcd->open(orig_data_file); 
      }
    }
    
    gROOT->cd(); 
  }

  cam_select->SetIntNumber(current_cam); 
  if (last_event != current_event)
  {
    if (!using_playlist && !using_raw)
    {
      dataset->getEvent(current_event); 
    }
    if (using_raw)
    {
      raw_dataset->getEvent(current_event); 
    }

#ifndef NOTRACESYET
    if(show_traces)
    {


      scopewindow->Display(using_raw ? raw_dataset->event() :  dataset->orig_event(),
                           using_raw ? NULL : dataset->event()->waveform_vectors()); 
    }
#endif
  }

  if (show_mc_tracks)
  {
    mcd->getEvent(current_event); 
  }

  if (!using_raw && !using_playlist && (last_event != current_event || last_cam != current_cam))
    current_track = 0; 

  track_select->SetIntNumber(current_track); 
  
  if (!using_raw)
    track_select->SetLimitValues(0,dataset->event(current_tree.c_str())->ntracks(current_cam)-1); 
  else
    track_select->SetLimitValues(-1,-1); 
  updateEventLabels();
  canvas->GetCanvas()->cd(); 

  if (last_event != current_event || last_cam !=current_cam)
  {
    if(bias_subtracted)
    {
      bias_subtracted->Delete(); 
      bias_subtracted = 0; 
    }
  }
  TH2 * img = getImage(); 


  img->SetMinimum(minz->GetNumber());
  img->SetMaximum(maxz->GetNumber());
  img->Draw("colz");

  drawTracks(); 

  drawOverlay();
  canvas->GetCanvas()->Update(); 

  if (show_zoom)
  {
    zoom->GetCanvas()->Clear(); 
    zoom->GetCanvas()->Update();
    if (zoomed!=NULL) zoomed->Delete();  
    zoomed = (TH2*) img->Clone("zoomed"); 
    zoomed->GetZaxis()->SetRange(0,0);
    zoomed->SetMinimum();
    zoomed->SetMaximum();
    zoomed->ResetBit(TH1::kIsZoomed); 
  }


  if (show_bias && (last_cam != current_cam || using_playlist))
  {
    UpdateBias(); 
  }

  if (show_orig)
  {
    UpdateOrig(); 
  }

  if (show_mosaic)
  {
    UpdateMosaic();
  }

#ifdef HAVE_FFT
  if (show_fft)
  {
    UpdateFFT(); 
  }
#endif

  if (show_pixhist) 
  {
    UpdatePixHist(); 
  }

  if (last_event != current_event || last_cam != current_cam || last_tree != current_tree)
  {
    std::list<ViewerImageTransform*>::iterator it;
    for (it = transforms.begin(); it != transforms.end(); it++)
    {
      (*it)->SetImage(img); 
    }

    std::list<ViewerProjection*>::iterator it2; 
    for (it2 = projections.begin(); it2 != projections.end(); it2++)
    {
      (*it2)->setImage(img); 
    }

 


  }

  canvas->GetCanvas()->cd(); 
  std::list<ViewerLine*>::iterator it3; 
  for (it3 = lines.begin(); it3 != lines.end(); it3++)
  {
      (*it3)->Draw(); 
  }
  canvas->GetCanvas()->Update(); 

  if (show_alone)
  {
    UpdateAlone(); 
  }

  if (show_stitch && last_event != current_event) 
  {
    DoStitch(); 
  }

  if (show_projection)
  {
    UpdateProjection(); 
  }

  if (show_leftproj)
  {
    UpdateLeftProj(); 
  }

  last_event = current_event;
  last_cam = current_cam;
  last_tree = current_tree; 
  
  if (last_x > 0 && last_y > 0) 
    updateZoomed(last_x,last_y);

}


void dmtpc::viewer::ViewerFrame::loadPlaylist(const char * file)
{
  (void) file; 
  cout << "loadPlaylist" << endl;
  unload(); 
  using_playlist = true; 
  using_raw = false; 
  cout << "file = " << file << endl;
  playlist = new dmtpc::analysis::Playlist(file); 
  loaded = true; 
  playlist->go(0); 
  dataset = playlist->getDataset(); 
  setTreeNames(); 
  ev_select->SetLimits(TGNumberFormat::kNELLimitMinMax,0,playlist->n()-1); 
  SetWindowName(TString("DmtpcViewer: ") + TString(file)); 
  if (playlist->n()==0)
  {
    cout << "Empty Playlist!" << endl; 
    unload(); 
  }
  drawEvent(); 
}

void dmtpc::viewer::ViewerFrame::unload()
{
  if (loaded && !using_playlist && !using_raw)
  {
    loaded = false;
    dataset->closeRootFile();
    delete dataset; 
  }

  else if (loaded && using_playlist)
  {
    loaded=false; 
    delete playlist; 
  }

  else if (loaded && using_raw)
  {
    loaded = false; 
    raw_dataset->close(); 
    delete raw_dataset; 
  }
}


void dmtpc::viewer::ViewerFrame::loadRawFile(const char * file)
{

  if (! checkIfFileContainsTree(file,"data"))
  {
    new TGMsgBox(gClient->GetRoot(),this,"Invalid Raw File.","Invalid file, does not contain 'dmtpc' tree");
    return;
  }

  unload();
  canvas->GetCanvas()->Clear(); 
  zoom->GetCanvas()->Clear(); 
  using_playlist = false; 
  using_raw = true; 

  raw_dataset = new dmtpc::core::Dataset; 
  raw_dataset->open(file); 
  gROOT->cd(); 
  
  if (orig_data_file!=NULL)
    free(orig_data_file); 
  orig_data_file =strdup(file);

  setTreeNames(); 

  current_cam = 0; 
  current_event = 0; 
  current_track = -1;
  last_cam = -1; 
  last_event = -1; 
  loaded = true; 

   if (show_orig)
    ShowOrig(); 
  if (show_bias)
    ShowBias(); 
  if (show_traces)
    Traces(); 

  drawEvent(); 
  cam_select->SetLimitValues(0,using_raw ? raw_dataset->event()->nccd()-1 : dataset->event()->ncamera()-1); 
  ev_select->SetLimits(TGNumberFormat::kNELLimitMinMax,0,using_raw ? raw_dataset->nevents()-1 : dataset->nevents()-1); 
  SetWindowName(TString("DmtpcViewer: ") + TString(file)); 

 
}

void dmtpc::viewer::ViewerFrame::loadSkimFile(const char * file)
{
  if (! checkIfFileContainsTree(file,"skim"))
  {
    new TGMsgBox(gClient->GetRoot(),this,"Invalid Skim File.","Invalid file, does not contain 'skim' tree");
    return;
  }
  unload();
  canvas->GetCanvas()->Clear(); 
  zoom->GetCanvas()->Clear(); 
  using_playlist = false; 
  using_raw = false; 

  dataset = new dmtpc::analysis::SkimDataset; 
  dataset->openRootFile(file);
  gROOT->cd(); 
  current_cam = 0; 
  current_event = 0; 
  current_track = 0; 
  last_cam = -1; 
  last_event = -1; 
  loaded = true; 
  setTreeNames(); 

   //Close these windows if they are there 
  if (show_orig)
    ShowOrig(); 
  if (show_bias)
    ShowBias(); 

  drawEvent(); 
  cam_select->SetLimitValues(0,dataset->event()->ncamera()-1); 
  ev_select->SetLimits(TGNumberFormat::kNELLimitMinMax,0,dataset->nevents()-1); 
  SetWindowName(TString("DmtpcViewer: ") + TString(file)); 

  if (orig_data_file!=NULL)
    free(orig_data_file); 
  orig_data_file =NULL;
}


void dmtpc::viewer::ViewerFrame::drawTracks()
{
  if (!loaded) return; 
  if (!show_outlines) return; 

//  for (std::list<TGraph*>::iterator i = edges.begin(); i!=edges.end(); i++) (*i)->Delete(); 
  edges.clear(); 

  canvas->GetCanvas()->cd(); 
  if (show_centroid)
  {
    if (marker!=NULL) marker->Delete(); 
    if (mc_marker!=NULL) mc_marker->Delete(); 
    marker = NULL; 
    mc_marker = NULL; 
  }

  if (show_vector)
  {
      if (arrow!=NULL) arrow->Delete(); 
      if (zoom_arrow!=NULL) zoom_arrow->Delete(); 
      if (mc_arrow!=NULL) mc_arrow->Delete(); 
      if (zoom_mc_arrow!=NULL) zoom_mc_arrow->Delete(); 
      arrow = NULL;
      zoom_arrow=NULL; 
      mc_arrow = NULL; 
      zoom_mc_arrow = NULL; 
  }

  if (!using_raw)
  {
    for (int i = 0; i < dataset->event(current_tree.c_str())->ntracks(current_cam); i++)
    {
      if (current_track == i) 
      {
        drawTrack(i,18,1);  
        if (show_reduced)
          drawTrack(i,5,1,true);  
        
        if (show_centroid)
        {
          marker = new TMarker(dataset->event(current_tree.c_str())->x(current_cam,current_track), dataset->event()->y(current_cam,current_track),5); 
          marker->Draw(); 
        }

        if (show_vector)
        {

          arrow = new TArrow( dataset->event(current_tree.c_str())->x(current_cam,current_track),
                              dataset->event(current_tree.c_str())->y(current_cam,current_track),
                              dataset->event(current_tree.c_str())->x(current_cam,current_track)+100*TMath::Cos(dataset->event()->phi(current_cam,current_track)),
                              dataset->event(current_tree.c_str())->y(current_cam,current_track)+100*TMath::Sin(dataset->event()->phi(current_cam,current_track))
                            ); 

          zoom_arrow = new TArrow( dataset->event(current_tree.c_str())->x(current_cam,current_track),
                              dataset->event(current_tree.c_str())->y(current_cam,current_track),
                              dataset->event(current_tree.c_str())->x(current_cam,current_track)+20*TMath::Cos(dataset->event()->phi(current_cam,current_track)),
                              dataset->event(current_tree.c_str())->y(current_cam,current_track)+20*TMath::Sin(dataset->event()->phi(current_cam,current_track))
                            ); 
          arrow->Draw(); 
        }
      }
      else
      {
        drawTrack(i,17,1); 
      }
    }
  }

  if (show_mc_tracks && mcd->truth()->nadu[current_cam] > 0)
  {

    TGraph electrons(mcd->truth()->nprimary); 
    for (int e = 0; e < mcd->truth()->nprimary; e++)
    {
      electrons.SetPoint(e,
            mcd->truth()->xcam(mcd->truth()->primary_electron_x[e], current_cam),
            mcd->truth()->ycam(mcd->truth()->primary_electron_y[e], current_cam)
          ); 
    }


    electrons.Draw("psame"); 

    edges.push_back(electrons); 

    if (show_centroid || show_vector)
    {
      double x,y; 
      x = mcd->truth()->x0cam();     
      y = mcd->truth()->y0cam();     

      if (show_centroid)
      {
        mc_marker = new TMarker(x,y,6); 
        mc_marker->Draw(); 
      }

      if (show_vector)
      { 
        mc_arrow = new TArrow( x, y,
                            x+100*TMath::Cos(mcd->truth()->phi()),
                            y+100*TMath::Sin(mcd->truth()->phi())
                          ); 
        zoom_mc_arrow = new TArrow( x, y,
                            x+10*TMath::Cos(mcd->truth()->theta()),
                            y+10*TMath::Sin(mcd->truth()->theta())
                          ); 
       
        mc_arrow->SetLineColor(46);  
        zoom_mc_arrow->SetLineColor(46);  
        mc_arrow->Draw(); 
      } 
    }

  }

  canvas->GetCanvas()->Update(); 
}

void dmtpc::viewer::ViewerFrame::loadOverlay(TString overlayFile){
  ifstream inputFileStream(overlayFile.Data(),ios::in);
  
  // if the file asked for can't be found, return an error
  if (inputFileStream.is_open()) {
    // we successfully opened the input data file
    string tmp;
    while( !inputFileStream.eof()){
      // read each line
      getline(inputFileStream,tmp,'\n');
      // NEED TO ADD CODE TO MAKE SURE THAT THERE
      // ARE AN EVEN NUMBER OF NUMBERS
      if(tmp.find_first_of('#')!=0 && tmp.size()>0){
          std::istringstream iss(tmp);
          float xbin,ybin;
          //cout << "tmp=" << tmp << endl;
    
          // this line corresponds to an overlay TGraph
          TGraph * overlay = new TGraph();
          overlay->SetEditable(0); 
          overlay->SetBit(kCannotPick); 
          overlay->SetLineColor(18); 
          overlay->SetLineWidth(2); 
          overlay->SetLineStyle(2); 

        while ( iss >> xbin >> ybin ){
            //cout << "xbin=" << xbin << " ybin=" << ybin << endl;
            overlay->SetPoint(overlay->GetN(),xbin,ybin);
        }
        overlay->Draw("l same");
        // done putting the points into the TGraph
        overlays.push_back(overlay); 
        overlay_cams.push_back(current_cam); 
      }
    } 
  }
}

void dmtpc::viewer::ViewerFrame::drawOverlay()
{
  if (!loaded) return; 
  if (!show_overlays) return; 

  canvas->GetCanvas()->cd(); 

  for (int i = 0; i < (int) overlays.size(); i++)
  {
     if (overlay_cams[i] == current_cam)
     {
       overlays[i]->Draw("l same");
     }
  }
  
  canvas->GetCanvas()->Update(); 
}

void dmtpc::viewer::ViewerFrame::drawTrack(int track, int color, int linewidth, bool red)
{

 const dmtpc::analysis::ClusterImage * ci =  dataset->event(current_tree.c_str())->cluster(current_cam); 

 if (!ci) return; 
 std::list<TGraph> these_edges; 
 dmtpc::analysis::clusteralgo::makeBoundary(ci->getCluster(track), &these_edges, red, true, "lsame", color,linewidth); 
 edges.splice(edges.end(), these_edges); 

 return; 
} 

void dmtpc::viewer::ViewerFrame::Go()
{
  if (!loaded) return; 

  if (!using_playlist)
  {
    if (current_event == ev_select->GetIntNumber()) return; 
    current_event = ev_select->GetIntNumber(); 
  }
  else
  {
    playlist->go(ev_select->GetIntNumber()); 
    current_event = playlist->index(); 
  }
  drawEvent(); 
}

void dmtpc::viewer::ViewerFrame::Previous()
{
  if (!loaded) return;
  if (!using_playlist)
  {
    if (--current_event < 0)
    {
      current_event = 0; 
      return; 
    }
  }
  else
  {
    playlist->previous();
    current_event = playlist->index(); 
  }

  drawEvent(); 
}

void dmtpc::viewer::ViewerFrame::Next()
{
  if (!loaded) return;
  if (!using_playlist && !using_raw)
  {
    if (++current_event >= dataset->nevents())
    {
      current_event = dataset->nevents()-1; 
      return;
    }
  }
  else if (using_playlist) 
  {
    playlist->next(); 
    current_event = playlist->index(); 
  }
  else if (using_raw)
  {
    if (++current_event >= raw_dataset->nevents())
    {
      current_event = raw_dataset->nevents()-1; 
      return;
    }
  }
  drawEvent(); 

}

void dmtpc::viewer::ViewerFrame::TrackChanged(Long_t ignored __attribute__((unused)) )
{
  if (!loaded) return; 
  if (using_raw) return; 
  current_track = track_select->GetIntNumber(); 
  if (current_track < 0) current_track = 0; 
  if (current_track >= dataset->event(current_tree.c_str())->ntracks(current_cam)) current_track = dataset->event()->ntracks(current_cam)-1; 
  track_select->SetIntNumber(current_track); 
  updateTrackLabels(); 
  if (show_projection)
    UpdateProjection(); 
  if (show_leftproj) 
  {
    UpdateLeftProj(); 
  }

  drawTracks(); 
}


void dmtpc::viewer::ViewerFrame::ZoomChanged(const char * ignored __attribute__((unused)))
{
  drawEvent(); 
}

void dmtpc::viewer::ViewerFrame::CamChanged(Long_t ignored  __attribute__((unused)))
{
  if (!loaded||using_playlist) return; 
  current_cam = cam_select->GetIntNumber(); 
  if (current_cam < 0) current_cam = 0; 

  if ( (!using_raw && current_cam >= dataset->event()->ncamera()) || 
       (using_raw && current_cam >= (int) raw_dataset->event()->nccd()) ) 
  {
        current_cam = using_raw ? raw_dataset->event()->nccd()-1 : dataset->event()->ncamera()-1;
  }
  cam_select->SetIntNumber(current_cam); 
  drawEvent(); 
}

/* Menu constants */ 
#include "ViewerMenuConstants.hh" 

void dmtpc::viewer::ViewerFrame::showSaveDialog()
{
   TGFileInfo fis; 
   fis.fFileTypes=filetypes_save; 
   fis.fOverwrite=1; 
   new TGFileDialog(gClient->GetRoot(), this, kFDSave, &fis);
   TString saveName(fis.fFilename); 
   if (saveName=="") return; 

   if (!saveName.EndsWith(fis.fFileTypes[2*fis.fFileTypeIdx+1]+1)) 
   {
     saveName += TString(fis.fFileTypes[2*fis.fFileTypeIdx+1]+1);   
   }

   canvas->GetCanvas()->SaveAs(saveName);
}


void dmtpc::viewer::ViewerFrame::showFitsDialog()
{
   TGFileInfo fis; 
   fis.fFileTypes=filetypes_fits; 
   fis.fOverwrite=1; 
   new TGFileDialog(gClient->GetRoot(), this, kFDSave, &fis);
   TString saveName(fis.fFilename); 
   if (saveName=="") return; 

   if (!saveName.EndsWith(fis.fFileTypes[2*fis.fFileTypeIdx+1]+1)) 
   {
     saveName += TString(fis.fFileTypes[2*fis.fFileTypeIdx+1]+1);   
   }

   if(dmtpc::image::io::convertIntoFits(getImage(),saveName))
   {
      new TGMsgBox(gClient->GetRoot(),this,"Error","Operation returned non-zero return code");
   }
   else
   {
      std::cout << "Saved as " << saveName << std::endl; 
   }
}

void dmtpc::viewer::ViewerFrame::HandleMenu(Int_t id)
{
  
  switch(id)
  {

    case M_EXIT:
     gApplication->Terminate(0); 
     break; 

    case M_TRACES:
      Traces(); 
      break; 

    case M_PIXHIST:
      ShowPixHist(); 
      break; 

    case M_BIAS:
      ShowBias(); 
      break; 

    case M_FFT:
      ShowFFT(); 
      break; 

    case M_SHOW_REDUCED:
      show_reduced = !show_reduced; 
      drawTracks(); 
      break; 

    case M_SHOW_MC:
      ShowMCTracks(); 
      break; 

    case M_STITCH:
      ShowStitch(); 
      break; 

    case M_PROJECTION:
      ShowProjection(); 
      break; 

    case M_LEFTPROJ:
      ShowLeftProj(); 
      break; 

    case M_ORIG:
      ShowOrig(); 
      break; 

     case M_MOSAIC:
      ShowMosaic(); 
      break; 

    case M_ZOOM:
      zoom->GetCanvas()->Clear(); 
      zoom->GetCanvas()->Update(); 
      show_zoom = !show_zoom; 
      break; 

    case M_ALONE:
      ShowAlone(); 
      break; 

    case M_OUTLINES:
      show_outlines = !show_outlines; 
      drawEvent(); 
      break; 

    case M_LINEPROJ:
      ShowLineProjection(); 
      break; 

    case M_SHOW_CENTROID:
      show_centroid = !show_centroid; 
      drawEvent(); 
      break; 

    case M_SHOW_VECTOR:
      show_vector = !show_vector; 
      drawEvent(); 
      break; 

    case M_OVERLAYS:
      show_overlays = !show_overlays; 
      drawEvent(); 
      break; 

    case M_CLEAR_OVERLAY:
      clearOverlays();  
      break; 

    case M_LOAD_STITCH:
      {
        TGFileInfo fio; 
        fio.fFileTypes=filetypes_stitch; 
        fio.fIniDir = strdup("./"); 
        new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fio); 
        if (fio.fFilename == NULL || strcmp(fio.fFilename,"")==0) return; 
        cout << "Opening " << fio.fFilename << endl; 
        TFile f(fio.fFilename); 
        if (stitch) stitch->Delete(); 
        gROOT->cd(); 
        stitch = (dmtpc::analysis::Stitcher *) f.Get("stitch"); 
        if (!stitch) 
        {
          new TGMsgBox(gClient->GetRoot(),this,"No stitch for you","Could not find key 'stitch' in your file");
        }
        f.Close(); 
      }
      break; 
    case M_OPEN_OVERLAY:
      {
        TGFileInfo fio; 
        fio.fFileTypes=filetypes_overlay; 
        fio.fIniDir = strdup("./"); 
        new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fio); 
        if (fio.fFilename == NULL || strcmp(fio.fFilename,"")==0) return; 
        cout << "Opening " << fio.fFilename << endl; 

        loadOverlay(strdup(fio.fFilename));
        show_overlays=true;
        drawEvent();
      }
      break; 

    case M_OPEN:
     {
       TGFileInfo fio; 
       fio.fFileTypes=filetypes_open; 
       fio.fIniDir = strdup("/net/zwicky/dmtpc/production/skimout/skim/"); 
       new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fio); 
       if (fio.fFilename == NULL || strcmp(fio.fFilename,"")==0) return; 
       cout << "Opening " << fio.fFilename << endl; 
       loadSkimFile(strdup(fio.fFilename));
     }
     break; 

    case M_OPEN_PLAYLIST:
     {
       TGFileInfo fio; 
       fio.fFileTypes=filetypes_play; 
       fio.fIniDir = strdup("/net/zwicky/dmtpc/"); 
       new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fio); 
       if (fio.fFilename == NULL || strcmp(fio.fFilename,"")==0) return; 
       cout << "Opening " << fio.fFilename << endl; 
       loadPlaylist(strdup(fio.fFilename));
     }
     break; 

    case M_OPEN_RAW:
     {
       TGFileInfo fio; 
       fio.fFileTypes=filetypes_raw; 
       fio.fIniDir= strdup("/net/zwicky/dmtpc/data/"); 
       new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fio); 
       if (fio.fFilename == NULL || strcmp(fio.fFilename,"")==0) return; 
       cout << "Opening " << fio.fFilename << endl; 
       loadRawFile(strdup(fio.fFilename));
     }
     break; 

    case M_SAVE_IMAGE:
     showSaveDialog(); 
     break; 

    case M_FITS: 
     showFitsDialog(); 
     break; 

    case M_GAUSSIAN_BLUR: 
     ShowTransform("gaussianBlur"); 
     break;
    case M_LAPLACE: 
     ShowTransform("Laplacian"); 
     break;
    case M_NEIGHBOR_RATIO: 
     ShowTransform("neighborRatio"); 
     break;
    case M_LOG: 
     ShowTransform("LoG"); 
     break;
    case M_BILATERAL: 
     ShowTransform("bilateralFilter"); 
     break; 
    case M_FAST_BILATERAL: 
     ShowTransform("fastBilateralFilter"); 
     break; 

    case M_DETECT_EDGES: 
     ShowTransform("edgeDetect"); 
     break; 
    case M_SEED_CLUSTER:
     ShowTransform("seedClusterFind"); 
     break; 
    case M_RING_CLUSTER:
     ShowTransform("ringClusterFind"); 
     break; 
    case M_BLUR:
     ShowTransform("blur"); 
     break; 
    case M_CLUSTER_FIND_CI:
     ShowTransform("findClustersCI"); 
     break; 
    case M_GRADIENT:
     ShowTransform("gradient"); 
     break; 
    case M_ANISO:
     ShowTransform("anisotropicDiffusion"); 
     break; 
    case M_CLUSTER_FIND_AD:
     ShowTransform("findClustersAD"); 
     break;

    case M_HOUGH_LINE:
     ShowTransform("houghTransformLine"); 
     break;

    case M_LENS:
     ShowTransform("lens"); 
     break;
    case M_MEDIAN:
     ShowTransform("median"); 
     break;

    case M_RADON:
     ShowTransform("radonTransform"); 
     break;

    case M_ROTATE:
     ShowTransform("rotate"); 
     break;
  
    case M_RAINBOW:
     dmtpc::image::palette::setStandard1(); drawEvent(); break; 
    case M_GRAYSCALE:
     dmtpc::image::palette::setGrayscale(); drawEvent(); break; 
    case M_GREEN:
     dmtpc::image::palette::setGreen(); drawEvent(); break; 
    case M_BLUE:
     dmtpc::image::palette::setBlue(); drawEvent(); break; 
    case M_RED:
     dmtpc::image::palette::setRed(); drawEvent(); break; 
    case M_CYAN:
     dmtpc::image::palette::setCyan(); drawEvent(); break; 
    case M_MAGENTA:
     dmtpc::image::palette::setMagenta(); drawEvent(); break; 
    case M_YELLOW:
     dmtpc::image::palette::setYellow(); drawEvent(); break; 
    case M_BLUISH:
     dmtpc::image::palette::setPalette1(); drawEvent(); break; 
    case M_PINK_GREEN:
     dmtpc::image::palette::setPalette2(); drawEvent(); break; 
    case M_GREEN_PINK:
     dmtpc::image::palette::setPalette3(); drawEvent(); break; 
    case M_MATLAB_HOT:
     dmtpc::image::palette::setHot(); drawEvent(); break; 
    case M_MATLAB_COOL:
     dmtpc::image::palette::setCool(); drawEvent(); break; 
    case M_MATLAB_JET:
     dmtpc::image::palette::setJet(); drawEvent(); break; 
    case M_MATLAB_COPPER:
     dmtpc::image::palette::setCopper(); drawEvent(); break; 
    case M_MATLAB_BONE:
     dmtpc::image::palette::setBone(); drawEvent(); break; 
    case M_MATLAB_WINTER:
     dmtpc::image::palette::setWinter(); drawEvent(); break; 
    case M_MATLAB_SPRING:
     dmtpc::image::palette::setSpring(); drawEvent(); break; 
    case M_MATLAB_SUMMER:
     dmtpc::image::palette::setSummer(); drawEvent(); break; 
    case M_MATLAB_AUTUMN:
     dmtpc::image::palette::setAutumn(); drawEvent(); break; 
    case M_MIT:
     dmtpc::image::palette::setMIT(); drawEvent(); break; 
    case M_BU:
     dmtpc::image::palette::setBU(); drawEvent(); break; 
    case M_RANDOM_3:
     dmtpc::image::palette::setRandom(3); drawEvent(); break; 
    case M_RANDOM_5:
     dmtpc::image::palette::setRandom(5); drawEvent(); break; 
    case M_RANDOM_10:
     dmtpc::image::palette::setRandom(10); drawEvent(); break; 

    default:
      cout << "Unhandled Menu Entry" << endl;    
  }
}


//Constructor
dmtpc::viewer::ViewerFrame::ViewerFrame(const TGWindow *p,
                                           UInt_t w, UInt_t h) 
                                          : TGMainFrame(p,w,h)
{

  doGUI(); 

  current_tree = "skim"; 
  current_event = 0; 
  last_event = -1; 
  last_cam = -1; 
  current_cam = 0; 
  current_track = 0; 
  mcd = 0; 
  loaded = false; 
  show_traces = false; 
  show_bias = false; 
  show_fft = false; 
  show_projection = false; 
  show_leftproj = false; 
  show_pixhist = false;
  show_orig = false; 
  show_mosaic = false;
  show_alone = false; 
  show_outlines = true; 
  show_overlays = false; 
  show_zoom = true;
  show_centroid = false; 
  show_mc_tracks = false; 
  show_vector = false; 
  zoomed=NULL; 
  using_playlist =false; 
  bias_subtracted = 0; 
  last_x = -1; 
  last_y = -1; 
  marker = NULL;
  arrow = NULL; 
  zoom_arrow = NULL; 
  mc_marker = NULL;
  zoom_mc_arrow = NULL; 
  mc_arrow = NULL; 
  orig_data_file = NULL;
  using_raw = false; 
  stitch = 0; 
  stitch_c = 0; 
  show_stitch = false;
  show_reduced = false; 

  projectionX = 0; 
  projectionY = 0; 
  projectionT = 0; 
  projectionL = 0; 

  doKeys(); 


  SetBackgroundColor(BGCOLOR); 
  SetWindowName("DmtpcViewer"); 
  MapSubwindows();
  Resize(GetDefaultSize()); 
  SetIconPixmap("/net/zwicky/dmtpc/software/viewer/icon.png"); 
  MapWindow(); 
}  


TH2 * dmtpc::viewer::ViewerFrame::getImage()
{
  if (!using_raw) return (TH2*) dataset->event()->cluster(current_cam)->getImage(); 

  if (!bias_subtracted)
  {
    bias_subtracted = (TH2*) raw_dataset->event()->ccdData(current_cam)->Clone("bias_subtract"); 
    bias_subtracted->Add(raw_dataset->biasAvg(current_cam),-1); 
    bias_subtracted->SetEntries(1); 
  }

  return bias_subtracted; 
}
void dmtpc::viewer::ViewerFrame::TreeChanged(Int_t id)
{
   if (using_raw) return; 
   current_tree = tree_names[id]; 
   drawEvent();
}

void dmtpc::viewer::ViewerFrame::ShowTransform(const char * name)
{
  ViewerImageTransform * t = ViewerImageTransform::Factory((char*)name, &transforms);
  if (t)
  {
    transforms.push_back(t); 
    t->SetImage(getImage()); 
  }
}

void dmtpc::viewer::ViewerFrame::clearOverlays()
{
  overlay_cams.clear(); 
  for (unsigned int i = 0; i < overlays.size(); i++) overlays[i]->Delete(); 
  overlays.clear(); 
  drawEvent(); 
}

void dmtpc::viewer::ViewerFrame::doKeys()
{
  //Bind keys
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_s), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_S), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_a), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_A), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_w), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_W), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_d), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_D), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_q), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_Q), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_e), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_E), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_m), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_M), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_c), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_C), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_v), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_V), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_H), kAnyModifier, kTRUE);
  gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_h), kAnyModifier, kTRUE);
}




void dmtpc::viewer::ViewerFrame::doGUI()
{
  /*Menu Bar */
  TGPopupMenu * filemenu = new TGPopupMenu(gClient->GetDefaultRoot()); 
  filemenu->AddEntry("&Open Skim File...",M_OPEN); 
  filemenu->AddEntry("Open &Playlist File...",M_OPEN_PLAYLIST); 
  filemenu->AddEntry("Open &Raw File...",M_OPEN_RAW); 
  filemenu->AddEntry("Open &Overlay File...",M_OPEN_OVERLAY); 
  filemenu->AddEntry("Open Stitc&h File...",M_LOAD_STITCH); 
  filemenu->AddEntry("&Clear All Overlays...",M_CLEAR_OVERLAY); 
  filemenu->AddEntry("&Save Image",M_SAVE_IMAGE); 
  filemenu->AddEntry("&Export to FITS",M_FITS); 
  filemenu->AddSeparator(); 
  filemenu->AddEntry("E&xit",M_EXIT); 

  TGPopupMenu * viewmenu = new TGPopupMenu(gClient->GetDefaultRoot()); viewmenu->AddEntry("Show &Traces",M_TRACES); 
  viewmenu->AddEntry("Show &Bias Frame",M_BIAS); 
  viewmenu->AddEntry("Show &Original Image",M_ORIG); 
  viewmenu->AddEntry("Show Image Mosaic",M_MOSAIC); 
#ifdef HAVE_FFT
  viewmenu->AddEntry("Compute &FFT",M_FFT); 
#endif
  viewmenu->AddEntry("Show In Standalone T&Canvas",M_ALONE); 
  viewmenu->AddEntry("Toggle Out&lines",M_OUTLINES); 
  viewmenu->AddEntry("Toggle Show &Reduced",M_SHOW_REDUCED); 
  viewmenu->AddEntry("Toggle &Overlay",M_OVERLAYS); 
  viewmenu->AddEntry("Toggle &Zoom",M_ZOOM); 
  viewmenu->AddEntry("Toggle &Centroid",M_SHOW_CENTROID); 
  viewmenu->AddEntry("Toggle &Arrow",M_SHOW_VECTOR); 
  viewmenu->AddEntry("Show &MC",M_SHOW_MC); 
  viewmenu->AddEntry("Show &Stitch",M_STITCH); 
  viewmenu->AddEntry("Show &Projection",M_PROJECTION); 
  viewmenu->AddEntry("Show &Left Projection",M_LEFTPROJ); 
  viewmenu->AddEntry("Show Pi&xel Histogram",M_PIXHIST); 
  viewmenu->AddEntry("&Line Projection",M_LINEPROJ); 

  TGPopupMenu * opmenu = new TGPopupMenu(gClient->GetDefaultRoot()); 
  opmenu->AddEntry("B&lur (legacy)", M_BLUR); 
  opmenu->AddEntry("Gaussian &Blur", M_GAUSSIAN_BLUR); 
  opmenu->AddEntry("Anisotropic &Diffusion", M_ANISO); 
  opmenu->AddEntry("Bilateral &Filter", M_BILATERAL); 
  opmenu->AddEntry("Fast Bilateral Filter", M_FAST_BILATERAL); 
  opmenu->AddEntry("Canny &Edge Detector", M_DETECT_EDGES); 
  opmenu->AddEntry("&Gradient", M_GRADIENT); 
  opmenu->AddEntry("&Seed Cluster Finder", M_SEED_CLUSTER); 
  opmenu->AddEntry("Ring Cluster Finder", M_RING_CLUSTER); 
  opmenu->AddEntry("findClusters&CI", M_CLUSTER_FIND_CI); 
  opmenu->AddEntry("findClusters&AD", M_CLUSTER_FIND_AD); 
  opmenu->AddEntry("Linear Hough &Transform", M_HOUGH_LINE); 
  opmenu->AddEntry("&Radon Transform", M_RADON); 
  opmenu->AddEntry("Rotate", M_ROTATE); 
  opmenu->AddEntry("Laplacian", M_LAPLACE); 
  opmenu->AddEntry("Laplacian of Gaussian", M_LOG); 
  opmenu->AddEntry("Neighbor Ratio", M_NEIGHBOR_RATIO); 
  opmenu->AddEntry("Lens Distortion", M_LENS); 
  opmenu->AddEntry("Median Filter", M_MEDIAN); 

  TGPopupMenu * palmenu = new TGPopupMenu(gClient->GetDefaultRoot()); 

  palmenu->AddEntry("Rainbow",M_RAINBOW);
  palmenu->AddEntry("Grayscale",M_GRAYSCALE);
  palmenu->AddEntry("Green",M_GREEN);
  palmenu->AddEntry("Blue",M_BLUE);
  palmenu->AddEntry("Red",M_RED);
  palmenu->AddEntry("Cyan",M_CYAN);
  palmenu->AddEntry("Magenta",M_MAGENTA);
  palmenu->AddEntry("Yellow",M_YELLOW);
  palmenu->AddEntry("Bluish",M_BLUISH);
  palmenu->AddEntry("Pink/Green",M_PINK_GREEN);
  palmenu->AddEntry("Green/Pink",M_GREEN_PINK);
  palmenu->AddEntry("Matlab Hot",M_MATLAB_HOT);
  palmenu->AddEntry("Matlab Cool",M_MATLAB_COOL);
  palmenu->AddEntry("Matlab Jet",M_MATLAB_JET);
  palmenu->AddEntry("Matlab Copper",M_MATLAB_COPPER);
  palmenu->AddEntry("Matlab Bone",M_MATLAB_BONE);
  palmenu->AddEntry("Matlab Winter",M_MATLAB_WINTER);
  palmenu->AddEntry("Matlab Spring",M_MATLAB_SPRING);
  palmenu->AddEntry("Matlab Summer",M_MATLAB_SUMMER);
  palmenu->AddEntry("Matlab Autumn",M_MATLAB_AUTUMN);
  palmenu->AddEntry("MIT",M_MIT);
  palmenu->AddEntry("BU",M_BU);
  palmenu->AddEntry("Random (3)",M_RANDOM_3); 
  palmenu->AddEntry("Random (5)",M_RANDOM_5); 
  palmenu->AddEntry("Random (10)",M_RANDOM_10); 

  TGMenuBar * menubar = new TGMenuBar(this,100,20,kHorizontalFrame); 
  menubar->SetBackgroundColor(BGCOLOR); 

  menubar->AddPopup("&Palettes",palmenu, new TGLayoutHints(kLHintsTop | kLHintsRight,0,4,0)); 
  menubar->AddPopup("&Operations",opmenu, new TGLayoutHints(kLHintsTop | kLHintsRight,0,4,0)); 
  menubar->AddPopup("&View",viewmenu, new TGLayoutHints(kLHintsTop | kLHintsRight,0,4,0)); 
  menubar->AddPopup("&File",filemenu, new TGLayoutHints(kLHintsTop | kLHintsRight,0,4,0)); 

  filemenu->Connect("Activated(Int_t)","dmtpc::viewer::ViewerFrame",this,"HandleMenu(Int_t)"); 
  viewmenu->Connect("Activated(Int_t)","dmtpc::viewer::ViewerFrame",this,"HandleMenu(Int_t)"); 
  opmenu->Connect("Activated(Int_t)","dmtpc::viewer::ViewerFrame",this,"HandleMenu(Int_t)"); 
  palmenu->Connect("Activated(Int_t)","dmtpc::viewer::ViewerFrame",this,"HandleMenu(Int_t)"); 

  AddFrame(menubar); 

  /**Base layout*/
  TGHorizontalFrame * hframe = new TGHorizontalFrame(this,1000,600); 
  hframe->SetBackgroundColor(BGCOLOR); 

  /* Left part*/
  TGVerticalFrame * leftframe = new TGVerticalFrame(hframe,550,600);  
  leftframe->SetBackgroundColor(BGCOLOR); 

  /* image part */
  canvas = new TRootEmbeddedCanvas("Ecanvas",leftframe,757,553); 
  canvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)","dmtpc::viewer::ViewerFrame",
                             this,"CanvasEvent(Int_t,Int_t,Int_t,TObject*)"); 

// This breaks the main display? ROOT bug? 
//  canvas->GetCanvas()->Connect("RangeChanged()","dmtpc::viewer::ViewerFrame",
//                             this,"UpdatePixHist()"); 

  canvas->GetCanvas()->SetFillColor(CANVAS_COLOR); 
  canvas->SetBackgroundColor(CANVAS_COLOR);
  leftframe->AddFrame(canvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,5,5,5,5));
  
  TGHorizontalFrame *buttonFrame = new TGHorizontalFrame(leftframe,700,40); 

  buttonFrame->SetBackgroundColor(BGCOLOR); 
  /* button bar */
  TGTextButton * prev = new TGTextButton(buttonFrame,"<---&,");
  prev->Connect("Clicked()","dmtpc::viewer::ViewerFrame",this,"Previous()"); 

  ev_select = new TGNumberEntryField(buttonFrame, -1,0,TGNumberFormat::kNESInteger,
                               TGNumberFormat::kNEANonNegative,
                                TGNumberFormat::kNELLimitMinMax,
                                0,1000); 
  ev_select->Resize(50,20); 
  TGTextButton * go = new TGTextButton(buttonFrame,"Go"); 
  go->Connect("Clicked()","dmtpc::viewer::ViewerFrame",this,"Go()"); 
  cam_select = new TGNumberEntry(buttonFrame, 0,2,-1,TGNumberFormat::kNESInteger,
                                TGNumberFormat::kNEANonNegative,
                                TGNumberFormat::kNELLimitMinMax,
                                0,1); 
  cam_select->Connect("ValueChanged(Long_t)","dmtpc::viewer::ViewerFrame",this,"CamChanged(Long_t)"); 
  cam_select->Connect("ValueSet(Long_t)","dmtpc::viewer::ViewerFrame",this,"CamChanged(Long_t)"); 
  TGTextButton * next = new TGTextButton(buttonFrame,"&.--->");
  next->Connect("Clicked()","dmtpc::viewer::ViewerFrame",this,"Next()"); 

  buttonFrame->AddFrame(prev,new TGLayoutHints(kLHintsCenterX,5,5,3,4)); 
  buttonFrame->AddFrame(next,new TGLayoutHints(kLHintsCenterX,5,20,3,4)); 

  TGLabel * eventlabel = new TGLabel(buttonFrame,"Event: ");
  eventlabel->SetBackgroundColor(BGCOLOR); 
  buttonFrame->AddFrame(eventlabel, new TGLayoutHints(kLHintsCenterY|kLHintsCenterX,10,5,3,4)); 
  buttonFrame->AddFrame(ev_select,new TGLayoutHints(kLHintsCenterY|kLHintsCenterX,0,0,3,4)); 
  buttonFrame->AddFrame(go,new TGLayoutHints(kLHintsCenterY|kLHintsCenterX,0,5,3,4)); 
  
  TGLabel * camlabel = new TGLabel(buttonFrame,"Cam: ");
  camlabel->SetBackgroundColor(BGCOLOR); 
  buttonFrame->AddFrame(camlabel, new TGLayoutHints(kLHintsCenterY|kLHintsCenterX,10,5,3,4)); 
  buttonFrame->AddFrame(cam_select,new TGLayoutHints(kLHintsCenterY|kLHintsCenterX,0,5,3,4)); 

  TGLabel * minZlabel = new TGLabel(buttonFrame, "min Z:");  
  minZlabel->SetBackgroundColor(BGCOLOR); 
  buttonFrame->AddFrame(minZlabel, new TGLayoutHints(kLHintsCenterY|kLHintsCenterX,10,5,3,4)); 

  minz = new TGNumberEntryField(buttonFrame,-1,-50,TGNumberFormat::kNESReal); 
  minz->Resize(50,20);
  minz->Connect("TextChanged(const char *)", "dmtpc::viewer::ViewerFrame", this, "ZoomChanged(const char *)"); 
  buttonFrame->AddFrame(minz,new TGLayoutHints(kLHintsCenterY|kLHintsCenterX,0,0,3,4)); 

  TGLabel * maxZlabel = new TGLabel(buttonFrame, "max Z:");  
  maxZlabel->SetBackgroundColor(BGCOLOR); 
  buttonFrame->AddFrame(maxZlabel, new TGLayoutHints(kLHintsCenterY|kLHintsCenterX,10,5,3,4)); 

  maxz = new TGNumberEntryField(buttonFrame,-1,250,TGNumberFormat::kNESReal); 
  maxz->Connect("TextChanged(const char *)", "dmtpc::viewer::ViewerFrame", this, "ZoomChanged(const char *)"); 
  maxz->Resize(50,20);
  buttonFrame->AddFrame(maxz,new TGLayoutHints(kLHintsCenterY|kLHintsCenterX,0,0,3,4)); 

  TGTextButton * showtraces = new TGTextButton(buttonFrame,"Show &Traces"); 
  showtraces->Connect("Clicked()","dmtpc::viewer::ViewerFrame",this,"Traces()"); 
  buttonFrame->AddFrame(showtraces,new TGLayoutHints(kLHintsCenterX,50,5,3,4)); 

  leftframe->AddFrame(buttonFrame); 

  /*Right part */

  TGVerticalFrame * rightframe = new TGVerticalFrame(hframe,400,600);

  rightframe->SetBackgroundColor(BGCOLOR); 

  ev_properties = new TGLabel(rightframe, "\n\n\n\n\n\n\n\n\n\n"); 
  ev_properties->SetBackgroundColor(0xffffff); 
  ev_properties->ChangeOptions(kSunkenFrame);

  rightframe->AddFrame(ev_properties, new TGLayoutHints(kLHintsExpandX  ,3,3,3,3)); 

  TGLabel * spacerlabel = new TGLabel(rightframe,"");
  spacerlabel->SetBackgroundColor(BGCOLOR); 
  rightframe->AddFrame(spacerlabel,  new TGLayoutHints(kLHintsCenterX,100,100,0,0)); 
  

  TGHorizontalFrame * trackframe = new TGHorizontalFrame(rightframe,100,20);  
  trackframe->SetBackgroundColor(BGCOLOR); 

  TGLabel * tracklabel = new TGLabel(trackframe,"Track:");
  tracklabel->SetBackgroundColor(BGCOLOR); 
  trackframe->AddFrame(tracklabel, new TGLayoutHints(kLHintsCenterX,3,3,3,1)); 

  track_select = new TGNumberEntry(trackframe, 0,2,-1,TGNumberFormat::kNESInteger,
                                TGNumberFormat::kNEANonNegative,
                                TGNumberFormat::kNELLimitMinMax,
                                0,1); 
  track_select->Connect("ValueChanged(Long_t)","dmtpc::viewer::ViewerFrame",this,"TrackChanged(Long_t)"); 
  track_select->Connect("ValueSet(Long_t)","dmtpc::viewer::ViewerFrame",this,"TrackChanged(Long_t)"); 

  trackframe->AddFrame(track_select, new TGLayoutHints(kLHintsCenterX,3,3,1,4)); 
  rightframe->AddFrame(trackframe, new TGLayoutHints(kLHintsCenterX,3,3,1,4)); 

  track_properties = new TGLabel(rightframe, "\n\n\n\n\n\n\n\n\n\n\n\n\n"); 
  track_properties->SetBackgroundColor(0xffffff); 
  track_properties->ChangeOptions(kSunkenFrame);
  track_properties->Layout();
  rightframe->AddFrame(track_properties, new TGLayoutHints( kLHintsExpandX,3,3,10,3)); 

  TGHorizontalFrame * treeframe = new TGHorizontalFrame(rightframe,100,20); 
  treeframe->SetBackgroundColor(BGCOLOR); 
  tree_select = new TGComboBox(treeframe); 
  tree_select->Connect("Selected(Int_t)","dmtpc::viewer::ViewerFrame",this,"TreeChanged(Int_t)"); 
  tree_select->AddEntry("skim",1); 
  tree_select->Resize(80,20); 
  tree_select->Layout(); 
  TGLabel * treelabel = new TGLabel(treeframe,"Tree:");
  treelabel->SetBackgroundColor(BGCOLOR); 
  treeframe->AddFrame(treelabel, new TGLayoutHints(kLHintsCenterX,3,3,3,1)); 
  treeframe->AddFrame(tree_select, new TGLayoutHints(kLHintsCenterX,3,3,1,4)); 
  rightframe->AddFrame(treeframe, new TGLayoutHints(kLHintsCenterX,3,3,1,4)); 
  zoom = new TRootEmbeddedCanvas("zoomC",rightframe,170,150); 

  rightframe->AddFrame(zoom, new TGLayoutHints(kLHintsCenterX |kLHintsBottom,5,5,5,5));
  
  zoom_level = new TGHSlider(rightframe,100, kSlider1 | kScaleDownRight, 1); 
  zoom_level->SetBackgroundColor(BGCOLOR); 
  zoom_level->SetRange(3,20); 
  zoom_level->SetPosition(5); 

  rightframe->AddFrame(zoom_level, new TGLayoutHints(kLHintsCenterX  | kLHintsBottom,5,5,5,5));

  hframe->AddFrame(leftframe, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2)); 
  hframe->AddFrame(rightframe, new TGLayoutHints(kLHintsExpandY,2,2,2,2)); 

  AddFrame(hframe, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,2,2)); 

}


