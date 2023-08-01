#ifndef BINREADER_H
#define BINREADER_H
#include <stdio.h> /// for FILE
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>

#include "TString.h"
#include "TBenchmark.h"
#include "TMath.h"

#define MaxNSample 10000  //trace length

struct Data{

  unsigned short Header;  /// only the last 4 bits
  unsigned short BoardID;
  unsigned short Channel;
  
  unsigned long long TimeStamp;
  unsigned short Energy;
  unsigned int Flags;
  
  char WaveformCode;
  unsigned int NSample;
  unsigned short Trace[MaxNSample];
  
  void Clear(){
    
    Header = 0;
    BoardID = 0;
    Channel = 0;
    TimeStamp = 0;
    Energy = 0;
    Flags = 0;
    WaveformCode = 0;
    NSample = 0;
    for( int i = 0; i < MaxNSample; i++) Trace[i] = 0;
  };
  
  void Print(){
    
    printf("header : 0x%X \n", Header);
    printf(" Board : %u , Channel : %u\n", BoardID, Channel);
    printf("Energy : %u , TimeStamp : %llu\n", Energy, TimeStamp);
    printf("  Flag : 0x%X\n", Flags);
    if( (Header & 0x8 ) == 1 ){ /// is waevform exist
      printf(" Wave form code : %d , nSample : %d\n", WaveformCode, NSample);
      for( int i = 0 ; i < NSample ; i++){
        printf("%4d | %d \n", i, Trace[i]);
      }
    }
    
  }
};


class BinReader{
  public:
    Data data;
    
  private:
    FILE * inFile;
    
    long int  inFileSize;
    long int  inFilePos;
    bool      endOfFile;  
    bool      isOpened;
    Long64_t  blockID;
    long int  nBlock;
    
    TBenchmark gClock;
    
    long int inFilePosPrecent[10];
    Long64_t blockIDPrecent[10];
    
    bool isHeaderOK;
    
    
    template <typename T> int FillData(T * dataItem); // if successful, return 1, else 0; cannot fill Trace
  
  ///============================================ Methods
  public:
    BinReader();
    BinReader(TString inFileName);
    ~BinReader();
    
    void OpenFile(TString inFileName);
    void SetHeader(unsigned short header); /// when no header, this has to be set, 4bit
    
    void CloseFile();    
    void UpdateFileSize();
    bool IsEndOfFile();
    
    bool IsOpen()               {return isOpened;}         
    long int GetFilePos()       {return inFilePos;}
    long int GetFileSize()      {return inFileSize;}
    Long64_t GetBlockID()       {return blockID;}
    Long64_t GetNumberOfBlock() {return nBlock;}
    
    int ReadBlock(int opt = 0);  /// 0 = default, fill waveform if any. slow
                                 /// 1 = no fill waveform, fast
    void ScanNumberOfBlock();
    void JumptoPrecent(int precent); ///this is offset by 1 block
    void PrintStatus(int mod);
    
};
//========================== implementation

BinReader::BinReader(){
  inFile     = 0;
  data.Clear();
  
  inFileSize = 0;
  inFilePos  = 0;
  
  nBlock     = 0;    
  blockID    = -1;
  endOfFile  = false;
  isOpened   = false;
  
  isHeaderOK = false;
  
}

BinReader::~BinReader(){
  fclose(inFile); /// fclose already delete inFile;
}


BinReader::BinReader(TString inFileName){ 
  inFile     = 0;
  data.Clear();
  
  inFileSize = 0;
  inFilePos  = 0;
  nBlock     = 0;    
  blockID    = -1;
  endOfFile  = false;
  isOpened   = false;
  
  OpenFile(inFileName);
}

void BinReader::OpenFile(TString inFileName){
  inFile = fopen(inFileName, "r");
  if( inFile == NULL ){
    printf("Cannot read file : %s \n", inFileName.Data());
  }else{
    fseek(inFile, 0L, SEEK_END);
    inFileSize = ftell(inFile);
    rewind(inFile); ///back to the File begining
    data.Clear();
    
    gClock.Reset();
    gClock.Start("timer");
    
    isOpened = true;
    
    ///============= Read the Header,
    /// the header only at the beginning of a file
    isHeaderOK = FillData(&data.Header);
    if( isHeaderOK == false ){
      printf(" header cannot read. \n");
      return;
    }
    ///printf("HEADER: 0x%X , 0x%X\n", data.Header, (data.Header >> 4));
    if( (data.Header >> 4 ) != 0xCAE ) {
      printf(" Header format not right. \n");
      isHeaderOK = false;
      rewind(inFile); ///back to the File begining
      return ;
    }
  
    data.Header = (data.Header & 0xF);
    isHeaderOK = true;
  }
};

void BinReader::SetHeader(unsigned short header){
  
  data.Header = header;
  
}

void BinReader::CloseFile(){
  fclose(inFile);
  isOpened = false;
  data.Clear();
  inFileSize = 0;
  inFilePos  = 0;
  nBlock     = 0;    
  blockID    = -1;
  endOfFile  = false;
};

void BinReader::UpdateFileSize(){
  if( inFile == NULL ) return;
  fseek(inFile, 0L, SEEK_END);
  inFileSize = ftell(inFile);
  fseek(inFile, inFilePos, SEEK_SET);
}

bool BinReader::IsEndOfFile() {
  int haha = feof(inFile);
  return haha > 0 ? true: false;
}

template <typename T> int BinReader::FillData(T * dataItem){
  if ( fread(dataItem, sizeof(*dataItem), 1, inFile) != 1 ) {
      endOfFile = IsEndOfFile();
      return -1;
  }
  ///printf("----- 0x%16llX \n", *dataItem);
  return 1;
}

int BinReader::ReadBlock(int opt){
  if( feof(inFile) ) return -1;
  if( endOfFile ) return -1;
  if( data.Header == 0 ) return -2;

  /// see the CoMPASS manual v19, P.67

  FillData(&data.BoardID);
  FillData(&data.Channel);
  FillData(&data.TimeStamp);
  
  if( (data.Header & 0x5) != 0 ) FillData(&data.Energy);
  
  if( (data.Header & 0x2) == 1 ) {
    unsigned long long dummy = 0;
    FillData(&dummy);
  }
  
  FillData(&data.Flags);
  
  /// check is wave form exist
  if( (data.Header & 0x8) == 1){
    FillData(&data.WaveformCode);
    FillData(&data.NSample);
    if( opt == 0 ){
      if ( fread(data.Trace, sizeof(data.Trace), 1, inFile) != 1 )  endOfFile = IsEndOfFile();
    }else{ /// skip trace
      fseek(inFile, inFilePos + data.NSample*2, SEEK_SET); /// 2 becasue each trace is 2 bytes
    }
  }
  
  blockID ++;

  inFilePos = ftell(inFile);
  return 1; 
}
void BinReader::ScanNumberOfBlock(){
  
  nBlock = 0;
  int count = 0;
  while( ReadBlock(1) != -1 ){
    nBlock ++;
    int haha = (inFilePos*10/inFileSize)%10;
    if(  haha == count ) {
      inFilePosPrecent[count] = inFilePos;
      blockIDPrecent[count] = blockID;
      count++;
    }
    PrintStatus(10000);
  }
  
  printf("\n\n\n");
  printf("scan complete: number of data Block : %ld\n", nBlock);
  
  inFilePos = 0;
  blockID = -1;
  
  rewind(inFile); ///back to the File begining
  if( isHeaderOK ){
    unsigned short dummy;
    FillData(&dummy);
  }
  endOfFile = false;
  
}
void BinReader::JumptoPrecent(int precent){
  
  if( precent < 0 || precent > 10 ) {
    printf("input precent should be 0 to 10\n");
    return;
  } 
  
  fseek(inFile, inFilePosPrecent[precent], SEEK_SET);
  blockID = blockIDPrecent[precent];
  
}
void BinReader::PrintStatus(int mod){
  
  ///==== event stats, print status every 10000 events
  if ( blockID % mod == 0 ) {
    UpdateFileSize();
    gClock.Stop("timer");
    double time = gClock.GetRealTime("timer");
    gClock.Start("timer");
    printf("Total measurements: \x1B[32m%llu \x1B[0m\nReading Pos: \x1B[32m %.3f/%.3f GB\x1B[0m\nTime used:%3.0f min %5.2f sec\033[A\033[A\r", 
                 blockID, inFilePos/(1024.*1024.*1024.), inFileSize/1024./1024./1024,  TMath::Floor(time/60.), time - TMath::Floor(time/60.)*60.);
  }   
  
}


#endif
