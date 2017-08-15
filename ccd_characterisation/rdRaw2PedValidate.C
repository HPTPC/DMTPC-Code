// mode=0 do ALL: read raw & accumulate ped, compute ped & stat--> R*
// mode=1 read raw & accumulate ped --> big_R*
// mode=2 read 1 raw + big_R histo , compued ped & stat --> R*
// 'loc' changes with location: loc=0:my VM, loc=1:m3daq

void rdRaw2PedValidate(int runId=1000000, int mode=0, TString outPath="./Alpha_March_4x4", int loc=0){  

  gSystem->Load("$DMTPC_HOME/DmtpcSkim/lib/libDmtpcSkim.so");
  gSystem->Exec(Form("mkdir %s",outPath));

  dmtpc::skim::CcdPedMaker pedMk; //

  //can is a flag. 0: only output a .m3ped.root file
  //               not 0: draw pixels sampled randomly in pdf
    //TCanvas *can=new TCanvas("aa","bb");
    TCanvas *can=0;
    pedMk.can=can;// tmp

        const Int_t dim = 3084;
    Int_t nReb=4;
    const Int_t tempnbins = (Int_t)dim/nReb;
    const Int_t nbins = tempnbins;

    pedMk.initDimsValidate(nbins); //
    Int_t nExpo=100;

  int time0=time(0);
  if (mode<2) {
    printf(" pedestal accumulation ...\n");
    /*******  events loop start *******/
    for (int ieve=0; ieve <  nExpo; ieve++) {
      std::cout<<"Event"<<ieve+1<<"   Total event"<<nExpo<<endl;
	     pedMk.accumulatePedValidate(nbins);
    }

    //Draw the hBigPed which contains all the data.
    /*TH2S *big = pedMk.getBig();
    TCanvas *c1=new TCanvas("c1","c1",10,10,1000,800);
    big->Draw("colz");
    return;*/
    int delT=time(0)-time0;
    float rate=-1;
    if(delT>0)rate= (float)ieve/(float)delT;
    printf("tot number frames: inp=%d  readRate=%.1fHz elapsed time=%.1f minutes\n",ieve,rate,delT/60.);
  }
  
  if (mode!=1) {// compute pedestals & QA
    // measure time used for computation of pedestals
    time0=time(0);
    int nPix=0;
    nPix+=pedMk.computePed();
    int delT=time(0)-time0;
    float rate=-1;
    if(delT>0)rate= (float)nPix/(float)delT;
    printf("tot num pix%.1f(k)  rate=%.1fkHz, elapsed time=%.0f seconds\n",nPix/1000.,rate/1000.,delT);
  }
  
  TString txt1="validation_R1000000"; if (mode==1) txt1="big"; 
  TFile *outHfile=new TFile(outPath+"/"+txt1+".m3ped.root","recreate");
  assert(outHfile->IsOpen());
     pedMk.saveHisto(outHfile,mode);

   outHfile->Close();
  return;
}

