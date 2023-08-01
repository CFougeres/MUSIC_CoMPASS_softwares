
////////////////////////////////////////////////////////////////////////////////
//  _______ _                     _     _  __ _                               //
// |__   __(_)                   | |   (_)/ _| |                              //
//    | |   _ _ __ ___   ___  ___| |__  _| |_| |_                             //
//    | |  | | '_ ` _ \ / _ \/ __| '_ \| |  _| __|                            //
//    | |  | | | | | | |  __/\__ \ | | | | | | |_                             //
//    |_|  |_|_| |_| |_|\___||___/_| |_|_|_|  \__|                            //
//                                                                            //
// Main function of this code.
////////////////////////////////////////////////////////////////////////////////
#include "timeshift.h"

void timeshift(){
    printf("=====================================\n");
    printf("===      Time Shift               ===\n");
    printf("=====================================\n");
    
    //USER INPUTS
    int applied_extraction_inputs = extraction_inputs();
    int RunNumber= int( param_inputs[14][0]);int FileNumber= int( param_inputs[15][0]);
    Double_t OverlapTMin=  param_inputs[16][0];  Double_t OverlapTMax=  param_inputs[16][1]; //SECOND
    Double_t threshDT = param_inputs[17][0]; //MICROSECOND
    UShort_t Board=  param_inputs[19][0];UShort_t Chan=  param_inputs[19][1];  UShort_t minE=  param_inputs[19][2] ; UShort_t maxE=  param_inputs[19][3];
    UShort_t refBoard =  param_inputs[18][0]; UShort_t refChan =  param_inputs[18][1];  UShort_t refMinE=  param_inputs[18][2]; UShort_t refMaxE=  param_inputs[18][3];
    TString infile = pathRun + Form("/RootFiles/Raw/Data_R_%i_%i.root",RunNumber,FileNumber);
    TFile* myFile = new TFile(infile);
    TTree* tree = (TTree*)myFile->Get("Data_R");
    tree->Print();
    // Simple progress monitor
    TStopwatch StpWatch;
    Long64_t numEntries = tree->GetEntries();;
    long double Frac[6];
    int fIndex = 0;
    Frac[0] = 0.01;
    Frac[1] = 0.25;
    Frac[2] = 0.5;
    Frac[3] = 0.75;
    Frac[4] = 0.9;
    Frac[5] = 1.0;

  TTreeReader theReader("Data_R", myFile);
  TTreeReaderValue<ULong64_t> rvTimestamp(theReader, "Timestamp");
  TTreeReaderValue<UShort_t> rvBoard = {theReader, "Board"};
  TTreeReaderValue<UShort_t> rvChannel = {theReader, "Channel"};
  TTreeReaderValue<UShort_t> rvEnergy = {theReader, "Energy"};
 // TTreeReaderValue<UInt_t> rvFlags = {theReader, "Flags"};
  Long64_t nentries = theReader.GetEntries();
  ULong64_t* ts0 = new ULong64_t[nentries]; // reference channel
  ULong64_t* ts1 = new ULong64_t[nentries];
  cout << "Extracting data from " << infile << " ..." << endl;
  Long64_t entry = 0;
  Long64_t e0 = 0;
  Long64_t e1 = 0;
  // Save the timestamps in arrays for selected channels using beam related events (energy cut)
  while(theReader.Next() && entry<MaxEntry) {
    if (*rvTimestamp/1e12<2*OverlapTMax) {
      if (*rvBoard==refBoard && *rvChannel==refChan && *rvEnergy>refMinE && *rvEnergy<refMaxE) {
    ts0[e0] = *rvTimestamp;
    e0++;
      }
      else if (*rvBoard==Board && *rvChannel==Chan && *rvEnergy>minE && *rvEnergy<maxE) {
    ts1[e1] = *rvTimestamp;
    e1++;
      }
    }
  }
  cout << "Within the first " << 2*OverlapTMax << " seconds found:\n"
       << e0 << " entries of Board " << refBoard << " Chan " << refChan << "\n"
       << e1 << " entries of Board " << Board << " Chan " << Chan << "\n" << endl;

  Double_t tshift = 0.0;// 7.2303 - 6.52073 + 0.00534 + 0.08806 + 0.00012; // run_17

  // Make TGraphs for each channel taking only events with deltaT
  // greater than a threshold value (in microseconds). This optimizes
  // the timeshift search, focusing on the large deltaT peaks which
  // are much less frequent and thus easy to spot.
  TGraph* Delta0 = new TGraph();
  Delta0->SetLineColor(kRed);
  TGraph* Delta1 = new TGraph();
  Delta1->SetLineColor(kBlue);
  
  Double_t MaxDeltaT = 0;
  for (Long64_t i=0; i<e0-1; i++) {
    Double_t DTus = (ts0[i+1]-ts0[i])/1e6; // microseconds, hence 1/1e6
    if (DTus>threshDT) {
      Delta0->SetPoint(Delta0->GetN(), ts0[i]/1e12/*sec*/, DTus);
      if (DTus>MaxDeltaT)
    MaxDeltaT = DTus;
    }
  }

  for (Long64_t i=0; i<e1-1; i++) {
    Double_t DTus = (ts1[i+1]-ts1[i])/1e6; // microseconds, hence 1/1e6
    if (DTus>threshDT) {
      Delta1->SetPoint(Delta1->GetN(), ts1[i]/1e12-tshift, DTus);
      if (DTus>MaxDeltaT)
    MaxDeltaT = DTus;
    }
  }
  
  
  Double_t tref, yref, y;
  Double_t NSD2 = 0;  // normalized sum of differences squared (NSD2)
  Double_t NSD2min = 1;
  Double_t tmin0, tmax0, tmin1, tmax1, tmin2, tmax2;
  tmin0 = ts0[0]/1e12;
  tmax0 = ts0[e0-1]/1e12;
  tmin1 = ts1[0]/1e12;
  tmax1 = ts1[e1-1]/1e12;
    
  if (tmin0<=OverlapTMin && tmax0>=OverlapTMax && tmin1<=OverlapTMin && tmax1>=OverlapTMax) {
    if (FindShift) {
      cout << "executing findshift() ..." << endl;
      tshift = findshift(Delta0, Delta1,
             OverlapTMin, OverlapTMax, threshDT);
      cout.precision(10);
      cout << "best timeshift: " << tshift << " sec" << endl;
    }
    else {
      // Just compute the
      Int_t npts = 0;
      computeNSD2(Delta0, Delta1, threshDT, OverlapTMin, OverlapTMax, npts, NSD2);
      cout << tshift << " " << npts << " " << NSD2 << endl;
    }
  }
  else
    cout << "Need more entries for channel 0 or 2" << endl;
  
  
  // Draw the graphs
  cGraphs = new TCanvas("cGraphs","graphs");
  TH2F* hbk = new TH2F("hbk","",10000,OverlapTMin, OverlapTMax, 20,0,1.2*MaxDeltaT);
  hbk->GetXaxis()->SetTitle("Time [s]");
  hbk->GetXaxis()->CenterTitle();
  hbk->GetYaxis()->SetTitle("#DeltaT [#mus]");
  hbk->GetYaxis()->CenterTitle();
  TH2F* hba;
  if (FindShift) {
    cGraphs->Divide(1,2);
    hbk->SetTitle("Before");
    hba = new TH2F("hba","After",10000,OverlapTMin, OverlapTMax, 20,0,1.2*MaxDeltaT);
    hba->GetXaxis()->SetTitle("Time [s]");
    hba->GetXaxis()->CenterTitle();
    hba->GetYaxis()->SetTitle("#DeltaT [#mus]");
    hba->GetYaxis()->CenterTitle();
    cGraphs->cd(1);
    hbk->Draw();
    Delta0->Draw("lp same");
    TGraph* Delta1clone = (TGraph*)Delta1->Clone();Delta1clone->Draw("lp same");

    cGraphs->cd(2);
    hba->Draw();
    Delta0->Draw("lp same");
    for (Int_t i=0; i<Delta1->GetN(); i++) {
      Double_t tsec, DTus;
      Delta1->GetPoint(i, tsec, DTus);
      Delta1->SetPoint(i, tsec-tshift, DTus);
    }
    Delta1->Draw("lp same");
  }
  else {
    hbk->Draw();
    Delta0->Draw("lp same");
    Delta1->Draw("lp same");
  }
}
