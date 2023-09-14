#ifndef ONLINEVISU_H
#define ONLINEVISU_H

#include "DAQTopology.h"
#include "Builder.h"

int Nrun;
int run_number[MaxN];
int files_per_run[ MaxN];
int init_file[MaxN];

TH1F* TimingRate;
TH2F* hA4_P23 ;
TH2F* hA5_P23 ;
TH2F* hA5_P67 ;
TH2F* hA8_P67 ;
TH2F* EvtMultiplicity ;

TH1F* hA[NAnode];
TH1F* hGrid ;
TH1F* hCath ;
TH1F*  hSi;

TCanvas* csi;
TCanvas* crate;
TCanvas* cposi;
TCanvas * cindi;

auto legend = new TLegend(0.1, 0.2, 0.4, 0.4);

int Er[2]={0,16000};
double bin_raw_to_MeV=4.;
int binEr= int((Er[1]-Er[0])/bin_raw_to_MeV);
int Tr[2];
int binTr;



#endif
