#include "ViewerStyle.hh"
#include "TStyle.h"
#include "TColor.h"

void dmtpc::viewer::style::setStyle()
{


  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetTitleColor(0);
  gStyle->SetTitleFont(42);
  gStyle->SetStatColor(0);
  //gStyle->SetFillColor(0);
  gStyle->SetTitleColor(1);

  // set the paper & margin sizes
  //gStyle->SetPaperSize(20,26);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadRightMargin(0.15); // 0.2
  gStyle->SetPadBottomMargin(0.05);
  gStyle->SetPadLeftMargin(0.1);

  gStyle->SetNdivisions(505,"x");
  gStyle->SetNdivisions(505,"y");

  // use large Times-Roman fonts
  gStyle->SetTextFont(42);
  gStyle->SetTextSize(0.08);

  gStyle->SetLabelFont(42,"x");
  gStyle->SetLabelFont(42,"y");
  gStyle->SetLabelFont(42,"z");
  gStyle->SetTitleFont(42,"x");
  gStyle->SetTitleFont(42,"y");
  gStyle->SetTitleFont(42,"z");

  gStyle->SetTitleOffset(1.25, "x");
  gStyle->SetTitleOffset(1.25, "y");
  gStyle->SetTitleOffset(1.25, "z");

  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetTitleSize(0.06,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.06,"y");
  gStyle->SetLabelSize(0.05,"z");
  gStyle->SetTitleSize(0.06,"z");

  // stat box
  gStyle->SetStatBorderSize(1);
  gStyle->SetStatX(0.95);
  gStyle->SetStatY(0.95);
  gStyle->SetStatW(.200);
  gStyle->SetStatH(.125);
  gStyle->SetStatColor(0);
  gStyle->SetStatStyle(0);
  gStyle->SetStatFont(42);

  // title
  gStyle->SetTitleX(0.3);
  gStyle->SetTitleW(0.5);


  // use bold lines and markers
  gStyle->SetMarkerStyle(20);
  gStyle->SetHistLineWidth((Width_t)1.85);
  gStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // do not display any of the standard histogram decorations
 gStyle->SetOptTitle(0);
  
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);

  // put tick marks on top and RHS of plots
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);


  const int nrgbx = 5; 
  const int ncont = 255; 
  double stops[nrgbx] = { 0, 0.34, 0.61, 0.84, 1.00};
  double red[nrgbx] = { 0, 0, 0.87, 1.00, 0.51};
  double green[nrgbx] = { 0, 0.81, 1.00, 0.2, 0};
  double blue[nrgbx] = { 0.51, 1.00, 0.12, 0.0, 0};
  TColor::CreateGradientColorTable(nrgbx, stops, red, green, blue, ncont);
  gStyle->SetNumberContours(ncont);

}

