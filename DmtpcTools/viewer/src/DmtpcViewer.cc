#include "ViewerFrame.hh"

#include "TApplication.h"
#include "TStyle.h"
#include "ViewerStyle.hh"

int main(int nargs, char ** args)
{
  TApplication app("app",0,0); 
  dmtpc::viewer::style::setStyle(); 
  dmtpc::viewer::ViewerFrame  fr(gClient->GetRoot(),1024,600); 
  if (nargs>1)
  {
    if (TString(args[1]).EndsWith("skim.root"))
      fr.loadSkimFile(args[1]);
    else if (TString(args[1]).EndsWith(".play"))
      fr.loadPlaylist(args[1]);
    else if (TString(args[1]).EndsWith(".root"))
      fr.loadRawFile(args[1]);
  }

  if (nargs > 2)
  {
    fr.setOrigFile(args[2]); 
  }

  app.Run(); 
  return 0; 
}
