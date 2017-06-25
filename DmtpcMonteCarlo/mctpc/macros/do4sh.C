int do4sh(int track = 0, int energy = 150) 
{
  gSystem->Load("lib/libmctpc.so"); 
  gSystem->Load("../retrim/lib/libmctpc.so"); 
  dmtpc::mc::retrim::CollisionReader c(TString::Format("../retrim/data/coll/COLLISON_%dkeV.txt",energy)); 
  dmtpc::mc::retrim::TableReader * tables[2]; 
  tables[0] = new dmtpc::mc::retrim::TableReader("../retrim/data/srim/f_in_cf4_30torr.txt"); 
  tables[1] = new dmtpc::mc::retrim::TableReader("../retrim/data/srim/c_in_cf4_30torr.txt"); 

  dmtpc::mc::retrim::TrackMaker t(&c,2,(const dmtpc::mc::retrim::TableReader**) tables,0.98); 
  dmtpc::mc::retrim::SimpleIonizationModel m; 

  dmtpc::mc::mctpc::TRIMGenerator g(&t,&m); 


  dmtpc::mc::mctpc::SimpleDrift d (0,300,0.0056, 0.0056, 1); 
  dmtpc::mc::mctpc::SimpleAvalanche a(6e4,0.3,0.1,0.254, 0.003); 
  dmtpc::mc::mctpc::SimpleCamera cam(1024,1024,4); 

  cam.setAcceptance(0.0008); 

  //make track
  g.setPosition(TVector3(0,0,50)); 
  g.setCurrentTrack(track); 

  dmtpc::mc::mctpc::Driver driver; 
  dmtpc::mc::mctpc::FileReadout r("out.root"); 

  driver.plug(&g,&d); 
  driver.plug(&d,&a); 
  driver.plug(&a,&cam); 
  driver.plug(&cam,&r); 

  driver.run(10); 

}
