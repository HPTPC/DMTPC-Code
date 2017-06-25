/** CAEN Ascii to ROOT tree converter **/ 

#include "TFile.h" 
#include "TH1.h" 
#include <inttypes.h>
#include "TTree.h"
#include "stdio.h" 

void printUsage()
{
  printf("Usage: ASCIIConverter in.dat out.root [treename=traces]\n"); 
}

struct ASCIIHeader
{
  size_t record_length; 
  size_t board_id; 
  size_t channel; 
  size_t event_number; 
  size_t pattern; 
  size_t timestamp; 
  size_t offset; 
}; 

int main(int nargs, char ** args)
{

  if (nargs <3)
  {
    printUsage(); 
    return 0; 
  }

  //open file to read

  FILE * in = fopen(args[1],"r"); 

  TFile out(args[2],"recreate"); 
  TTree * tree = new TTree(nargs > 3 ? args[3] : "traces", "Converted Waveforms"); 

  TH1S * histo = 0; 
  ASCIIHeader header; 
  tree->Branch("waveform",&histo); 
  tree->Branch("timestamp",&(header.timestamp)); 
  tree->Branch("offset",&(header.offset)); 
  tree->Branch("channel",&(header.channel)); 
  tree->Branch("board_id",&(header.board_id)); 
  tree->Branch("pattern",&(header.pattern)); 
  tree->Branch("record_length",&(header.record_length)); 

  int counter = 0; 


  while (!feof(in))
  {

    printf("record %d\n",counter++); 
    if(!fscanf(in,"Record Length: %d\n", &(header.record_length))) 
    {
      printf("couldn't find record length\n"); 
      break; 
    }


    if(!fscanf(in,"BoardID: %2d\n", &header.board_id)) 
    {
      printf("COULDN'T FIND BOARD ID\n"); 
      break; 
    }

    if(!fscanf(in,"Channel: %d\n", &header.channel)) 
    {
      printf("COULDN'T FIND CHANNEL\n"); 
      break; 
    }

    if(!fscanf(in,"Event Number: %d\n", &header.event_number)) 
    {
      printf("COULDN'T FIND EVENT NUMBER\n"); 
      break; 
    }

    if(!fscanf(in,"Pattern: 0x%04X\n", &header.pattern)) 
    {
      printf("COULDN'T FIND PATTERN\n"); 
      break; 
    }

    if(!fscanf(in,"Trigger Time Stamp: %u\n", &header.timestamp)) 
    {
      printf("COULDN'T FIND TIMESTAMP\n"); 
      break; 
    }

    if(!fscanf(in,"DC offset (DAC): 0x%04X\n", &header.offset)) 
    {
      printf("COULDN'T FIND OFFSET\n"); 
      break; 
    }

    if (histo) delete histo; 
    histo = new TH1S("waveform","waveform",header.record_length, 0, header.record_length*4e-7); 


    for (int i = 0; i < header.record_length; i++)
    {
      uint32_t val; 
      if (!fscanf(in,"%d\n",&val))
      {
        printf("INCOMPLETE RECORD!!!!\n"); 
        break; 
      }

      histo->SetBinContent(i+1, val); 
    }

    tree->Fill(); 
  }

  fclose(in); 
  tree->Write(); 
  out.Close(); 


  return 0; 

}
