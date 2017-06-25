{
  dmtpc::mc::amp::GasLoader g; 


  Garfield::MediumMagboltz *cf4 = g.getCF4(30); 



  Garfield::ComponentConstant c; 
  c.SetElectricField(0,0,125); 


  Garfield::GeometrySimple geom; 

  Garfield::SolidBox box(0,0,15,10,10,15); 
  geom.AddSolid(&box,cf4);

  c.SetGeometry(&geom); 

  Garfield::Sensor  s; 
  s.AddComponent(&c); 



  Garfield::AvalancheMicroscopic a; 
  a.SetSensor(&s); 


  TFile fout("initElectronz1.root","RECREATE"); 

  TTree * tree = new TTree("electron","electron"); 

  double x0,y0,z0,t0; 
  double xf,yf,zf,tf; 
  double dx0,dy0,dz0; 
  double E0, Ef; 
  int status;

  tree->Branch("x0",&x0); 
  tree->Branch("y0",&y0); 
  tree->Branch("z0",&z0); 
  tree->Branch("dx0",&dx0); 
  tree->Branch("dy0",&dy0); 
  tree->Branch("dz0",&dz0); 
  tree->Branch("t0",&t0); 
  tree->Branch("E0",&E0); 

  tree->Branch("xf",&xf); 
  tree->Branch("yf",&y0); 
  tree->Branch("zf",&zf); 
  tree->Branch("tf",&tf); 
  tree->Branch("Ef",&Ef); 
  tree->Branch("status",&status); 


  TRandom3 rand; 

  for (int i = 0; i < 100000; i++)
  {
    x0 = 0;
    y0 = 0;
    z0 = 1; 
    E0 = TMath::Pow(100, rand.Rndm()); 
    rand.Sphere(dx0,dy0,dz0,1);

    a.AvalancheElectron(x0,y0,z0,t0,E0,dx0,dy0,dz0); 
    a.GetElectronEndpoint(0,x0,y0,z0,t0,E0,xf,yf,zf,tf,Ef,status);

    x0 *=10; 
    y0 *=10; 
    z0 *=10; 
    xf *=10; 
    yf *=10; 
    zf *=10; 

    tree->Fill();
  }

  tree->Write();

  fout.Close(); 

}
