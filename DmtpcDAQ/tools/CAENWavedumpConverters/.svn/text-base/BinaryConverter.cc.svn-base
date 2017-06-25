#include "TFile.h" 
#include "TH1.h" 
#include <inttypes.h>
#include "TTree.h"
#include "stdio.h" 

void printUsage()
{
  printf("Usage: ASCIIConverter in.dat out.root [treename=traces]\n"); 
}


struct BinHeader
{
  uint32_t record_length; 
  uint32_t board_id; 
  uint32_t pattern; 
  uint32_t channel; 
  uint32_t counter; 
  uint32_t timestamp; 
}; 


int main(int nargs, char **args)
{
  if (nargs <3)
  {
    printUsage(); 
    return 0; 
  }

  //open file to read

  FILE * in = fopen(args[1],"r"); 
  TFile out(args[2],"CREATE"); 

  TTree * tree = new TTree(nargs > 3 ? args[3] : "traces", "Converted Waveforms"); 

  TH1S * histo = 0; 
  BinHeader header; 
  tree->Branch("timestamp",&(header.timestamp)); 
  tree->Branch("channel",&(header.channel)); 
  tree->Branch("board_id",&(header.board_id)); 
  tree->Branch("pattern",&(header.pattern)); 
  tree->Branch("record_length",&(header.record_length)); 

  int counter = 0; 


  while (!feof(in))
  {
    printf("record %d\n", counter++); 

    fread(&header, sizeof(header), 1, in); 
    header.record_length -= sizeof(header); 
    header.record_length /= 2; 
//    printf("nsamples=%u\n",header.record_length); 
    if (!histo)
    {
      histo = new TH1S("waveform","waveform",header.record_length, 0, header.record_length* ( nargs > 4 ?  1. / (1e6*atof(args[4])) : 4.e-7)); 
      tree->Branch("waveform",&histo); 
    }

    fread(&histo->GetArray()[1], 2, header.record_length,in); 

    tree->Fill(); 
  }

  fclose(in); 
  tree->Write(); 
  out.Close();  

  return 0; 


}


