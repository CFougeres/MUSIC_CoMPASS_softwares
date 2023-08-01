#ifndef TIMESHIFT_H
#define TIMESHIFT_H

#include "DAQTopology.h"
#include "Builder.h"

TCanvas* cGraphs;
TCanvas* cProb;
Int_t MaxEntry=5000000 ;
Int_t FindShift=1 ;

void computeNSD2(TGraph* ref, TGraph* gr,
         Double_t threshDT/*microsec*/,
         Double_t OverlapTMin/*sec*/,
         Double_t OverlapTMax/*sec*/,
         Int_t& npts,
         Double_t& NSD2)
{
  NSD2 = 0;
  npts = 0;
  for (Long64_t p=0; p<ref->GetN(); p++) {
    Double_t tref, dtref, dt;
    ref->GetPoint(p, tref, dtref);
    if (tref>=OverlapTMin && tref<=OverlapTMax && dtref>threshDT) {
      dt = gr->Eval(tref);
      NSD2 += pow(dt-dtref,2);
      npts++;
    }
  }
  if (NSD2>0) {
    NSD2 = sqrt(NSD2);
    NSD2 /= npts;
  }
  else
    NSD2 = 1.0;
  return;
}

Double_t findshift(TGraph* ref, TGraph* gr,
           Double_t OverlapTMin/*sec*/,
           Double_t OverlapTMax/*sec*/,
           Double_t threshDT/*microsec*/)
{
  Double_t shift = 0;
  Int_t iterations = 0;
  TGraph* grShifted = (TGraph*)gr->Clone("grShifted");
  TRandom3 rdm;
  Int_t npts = 0;
  Double_t NSD2 = 0;  // normalized sum of differences squared (NSD2)
  Double_t prevNSD2 = 0;
  Double_t NSD2min = 1;
  Double_t tmin, tmax, aux;
  Double_t tini;
  TGraph* gInvNSD2 = new TGraph();
  
  ref->GetPoint(0, tini, aux);
  
  // First get a coarse distribution of the 1/NSD2
  for (Int_t p=0; p<gr->GetN(); p++) {
    npts = 0;
    NSD2 = 0;
    gr->GetPoint(p, shift, aux);
    shift -= tini;
    
    // shift the graph time
    for (Long64_t ps=0; ps<gr->GetN(); ps++) {
      Double_t tsec, dt;
      gr->GetPoint(ps, tsec, dt);
      grShifted->SetPoint(ps, tsec-shift, dt);
    }
    
    // check that the min/max times are within the overlap limits
    grShifted->GetPoint(0, tmin, aux);
    grShifted->GetPoint(grShifted->GetN()-1, tmax, aux);
    if (tmin<OverlapTMin && tmax>OverlapTMax) {
      // compute NSD2 of grShifted with respect to ref
      computeNSD2(ref, grShifted, threshDT, OverlapTMin, OverlapTMax, npts, NSD2);
      // cout << p << " ";
      // cout.precision(10);
      // cout << shift;
      // cout << " " << npts << " " << NSD2 << endl;
      // Fill histogram with inverse NSD2 whose GetRandom() method
      // will serve guide the search for a precise time shift.
      gInvNSD2->SetPoint(gInvNSD2->GetN(), shift, 1/NSD2);
    }
    //    cout << shift << " " << tmin1 << " " << tmax1 << "\n" << npts << " " << NSD2 << endl;
  }

  // Retreive time shift with largest 1/NSD2
  Double_t bestshift = 0;  // return value
  Double_t maxInvNSD2 = 0;
  Double_t invNSD2 = 0;
  for (Int_t i=0; i<gInvNSD2->GetN(); i++) {
    gInvNSD2->GetPoint(i, shift, invNSD2);
    if (invNSD2>maxInvNSD2) {
      bestshift = shift;
      maxInvNSD2 = invNSD2;
    }
  }
  
  cProb = new TCanvas("cPorb","Prob");
  gInvNSD2->Draw("al*");
  
  return bestshift;
}
#endif
