#ifndef NORMESEGMENT_H
#define NORMESEGMENT_H

#include "DAQTopology.h"
#include "Builder.h"

TH1F* Eraw_seg[2][NChannel];
TH1F* Eraw_lim[2];
TF1* gCalib[2][NChannel];
TF1* gCalibLim[2] ;
TSpectrum* s[2][NChannel];
TSpectrum* sLim[2];
Int_t NPeaks;
Int_t NPeaksLim;
Double_t* XPeaks[2][NChannel];
Double_t* XPeaksLim[2];
TCanvas* cStripSeg[2];
TCanvas* cStripLim;
#endif
