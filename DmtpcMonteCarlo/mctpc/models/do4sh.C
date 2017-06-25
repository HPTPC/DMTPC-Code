void do4sh(int track=0) 
{

  gStyle->SetOptStat(0); 
  gStyle->SetNumberContours(255); 
  gSystem->Load("../retrim/lib/libretrim.so"); 
  gSystem->Load("lib/libmctpc.so"); 

  dmtpc::mc::retrim::CollisionReader c("../retrim/data/coll/COLLISON_150keV.txt"); 
  dmtpc::mc::retrim::TableReader * tables[2]; 
  tables[0] = new dmtpc::mc::retrim::TableReader("../retrim/data/srim/f_in_cf4_30torr.txt"); 
  tables[1] = new dmtpc::mc::retrim::TableReader("../retrim/data/srim/c_in_cf4_30torr.txt"); 

  dmtpc::mc::retrim::TrackMaker t(&c,2,tables,0.98); 
  dmtpc::mc::retrim::SimpleIonizationModel m; 

  dmtpc::mc::mctpc::TRIMGenerator g(&t,&m); 


  dmtpc::mc::mctpc::SimpleDrift d (0,300,0.0056, 0.0056, 0.1); 
  dmtpc::mc::mctpc::SimpleAvalanche a(6e4,0.3,0.1,0.254, 0.003); 
  dmtpc::mc::mctpc::SimpleCamera cam(1024,1024,4); 

  cam.setAcceptance(0.0008); 

  //make track
  g.setPosition(TVector3(0,0,250)); 
  g.setCurrentTrack(track); 
  g.generate(); 
  cout << d.process(g.getParticles()) << endl; 
  cout <<a.process(d.getParticles()) << endl; ; 
  cam.process(a.getParticles()); 
  dmtpc::mc::mctpc::Signal img = cam.getSignal(0); 
  img.getHist2D()->DrawCopy("colz"); 

}
