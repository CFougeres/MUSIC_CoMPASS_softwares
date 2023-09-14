/**************************************
 * root script for online visualization
***************************************/
#include "build/OnlineVisu.h"

using namespace std;

void OnlineVisu()
{
    printf("=====================================\n");
    printf("===          OnlineVisu           ===\n");
    printf("=====================================\n");
  

    //"""""""""""""""""""""
    //Users' inputs
    //"""""""""""""""""""""
    applied_extraction_inputs = extraction_inputs();
    TimeWindow =param_inputs[1][0];
    Nrun  = param_inputs[3][0];
    for(int r=0;r<Nrun;r++){
        run_number[r]= int(param_inputs[4][r]);
        files_per_run[r]=int(param_inputs[5][r]);
        init_file[r]=int(param_inputs[6][r]);
        for(int f=init_file[r];f<files_per_run[r];f++){
            if(f==0){fileName = pathRun + Form("/DAQ/run_%i/RAW/DataR_run_%i.root",run_number[r], run_number[r]);}
            if(f>0){ fileName= pathRun+Form("/DAQ/run_%i/RAW/DataR_run_%i_%i.root",run_number[r], run_number[r],f);}
            cout<< fileName<<endl;;
            Tree_data->Add(fileName.c_str());
        }
    }
    RangePositionCoeff= param_inputs[7][0];
    //"""""""""""""""""""""
    //Data tree structure
    //""""""""""""""""""""" 
    Tree_data->SetBranchAddress("Channel", &Channel);    Tree_data->SetBranchAddress("Board", &Board);
    Tree_data->SetBranchAddress("Timestamp", &Timestamp);  Tree_data->SetBranchAddress("Energy", &Energy);
    nStat = Tree_data->GetEntries();        cout<<nStat<<endl;
    double proportion_of_file_for_Plot=1;
    //"""""""""""""""""""""
    //Time sorting
    //"""""""""""""""""""""
    Double_t* timestampVec = new  Double_t[nStat];    Int_t* entryVec = new Int_t[nStat];
    sortZero = 0;sortLength=nStat-1;
    for(int j=0;j<nStat;j++){
        Tree_data->GetEntry(j);
        timestampVec[j]= Timestamp;
        entryVec[j]=j;
    }
    quicksort(timestampVec,entryVec,sortZero,sortLength);cout<<"sorted "<<endl;
    Tr[0]=timestampVec[0]*1e-12;    Tr[1]=timestampVec[nStat-1]*1e-12;
    binTr=(Tr[1]-Tr[0])/0.001;
    //"""""""""""""""""""""
    //Plot definition
    //"""""""""""""""""""""
   // gStyle->SetOptLogy(1);
    TimingRate = new TH1F("TimingRate","Timing;Timestamp (s);Counts/1#mus",binTr, Tr[0], Tr[1]);
    EvtMultiplicity = new TH2F("EvtMultiplicity",";Evt time (s);Multiplicity",binTr,Tr[0],Tr[1],16,0,15);
    hA4_P23 = new TH2F("hA4_P23","; #DeltaE_{4} (arb. u.);P23 (arb. u.)",binEr, Er[0], Er[1],2*binEr, -Er[1], Er[1]);
    hA5_P23 = new TH2F("hA5_P23","; #DeltaE_{5} (arb. u.);P23 (arb. u.)",binEr, Er[0], Er[1],2*binEr, -Er[1], Er[1]);
    hA5_P67 = new TH2F("hA5_P67","; #DeltaE_{5} (arb. u.);P67 (arb. u.)",binEr, Er[0], Er[1],2*binEr, -Er[1], Er[1]);
    hA8_P67 = new TH2F("hA8_P68","; #DeltaE_{8} (arb. u.);P67 (arb. u.)",binEr, Er[0], Er[1],2*binEr, -Er[1], Er[1]);

    for( int i = 0 ; i < NAnode; i++){
        hA[i] = new TH1F(Form("hA%0d",i), Form("A%0d;#DeltaE (a.u.)",i+1), binEr, Er[0], Er[1]);
    }
    hGrid = new TH1F("hGrid","Grid;#DeltaE (a.u.)", binEr, Er[0], Er[1]);
    hCath = new TH1F("hCath","Cathode;#DeltaE (a.u.)", binEr, Er[0], Er[1]);
    hSi = new TH1F("hSi", "hSi;#DeltaE (a.u.)", binEr, Er[0], Er[1]);
    csi = new TCanvas("csi","csi",800,800);
    crate= new TCanvas("crate","crate",1200,1200);crate->Divide(1,2);
    cposi= new TCanvas(" cposi", " cposi", 1600, 1600);  cposi->Divide(2,2);
    cindi= new TCanvas("cindi","cindi",1600,1600);cindi->Divide(3,3);
    //"""""""""""""""""""""
    //Reading events
    //"""""""""""""""""""""
    P23=0;  P67=0; evt_mult=0;
    Tree_data->GetEntry(entryVec[0]);
    evt_time =(timestampVec[0])*1e-12;
    temp_index=Map_DAQCha_to_MONICAsignal[0][Channel];
    if(temp_index<NAnode+1){  dE_anode[temp_index-1]=Energy;TimingRate->Fill(evt_time);  hA[temp_index-1]->Fill( dE_anode[temp_index-1]);evt_mult+=1;    }
    if(temp_index==NAnode+1){ Cathode=Energy;TimingRate->Fill(evt_time);   hCath->Fill(Cathode);evt_mult+=1;  }
    if(temp_index==NAnode+2){ Grid=Energy;  TimingRate->Fill(evt_time);   hGrid->Fill(Grid);evt_mult+=1; }
    if(temp_index==20){Si_dE=Energy; TimingRate->Fill(evt_time);   hSi->Fill(Si_dE);evt_mult+=1; }
    for(int j=1;j<nStat*proportion_of_file_for_Plot;j++){
        Tree_data->GetEntry(entryVec[j]);
        if(timestampVec[j]-timestampVec[j-1]<0){ std::cout<<"issue time sorting "<<std::endl;break;}
        diffTimeEvent=(timestampVec[j]*1e-12-evt_time)*1e6;
        if(diffTimeEvent>TimeWindow){
            EvtMultiplicity->Fill( evt_time, evt_mult);
            evt_time=(timestampVec[j])*1e-12;
            if(dE_anode[1]*dE_anode[2]>0){P23 = RangePositionCoeff*((dE_anode[1]-dE_anode[2])/(dE_anode[1]+dE_anode[2]));}
           // cout<<P23 <<endl;
            if(dE_anode[5]*dE_anode[6]>0) {P67 =  RangePositionCoeff*((dE_anode[5]-dE_anode[6])/(dE_anode[5]+dE_anode[6]));}
            hA4_P23->Fill(dE_anode[3], P23); hA5_P23->Fill(dE_anode[4], P23);
            hA5_P67->Fill(dE_anode[4], P67); hA8_P67->Fill(dE_anode[7], P67);
            for(int a=0;a<NAnode;a++){
                dE_anode[a]=0.;
            }
            evt_mult=0;  P23=0;  P67=0;
        }
        temp_index=Map_DAQCha_to_MONICAsignal[0][Channel];
        if(temp_index<NAnode+1){   dE_anode[temp_index-1]=Energy;TimingRate->Fill(evt_time);  hA[temp_index-1]->Fill( dE_anode[temp_index-1]);evt_mult+=1;        }
        if(temp_index==NAnode+1){  Cathode=Energy;TimingRate->Fill(evt_time);   hCath->Fill(Cathode);evt_mult+=1;  }
        if(temp_index==NAnode+2){  Grid=Energy;  TimingRate->Fill(evt_time);   hGrid->Fill(Grid);evt_mult+=1; }
        if(temp_index==20){Si_dE=Energy; TimingRate->Fill(evt_time);   hSi->Fill(Si_dE); evt_mult+=1;}
    }
    gStyle->SetOptStat("nei");
    gStyle->SetPalette(kThermometer);
    csi->cd();hSi->Draw();
    hSi->GetXaxis()->CenterTitle();hSi->GetYaxis()->CenterTitle();
    crate->cd(1);TimingRate->Draw();
    TimingRate->GetXaxis()->CenterTitle();TimingRate->GetYaxis()->CenterTitle();
    crate->cd(2); EvtMultiplicity->Draw("colz");
    EvtMultiplicity->GetXaxis()->CenterTitle();EvtMultiplicity->GetYaxis()->CenterTitle();
    cposi->cd(1); hA4_P23->Draw("colz"); gPad->SetLogz();
    hA4_P23->GetXaxis()->CenterTitle(); hA4_P23->GetYaxis()->CenterTitle();
    cposi->cd(2); hA5_P23->Draw("colz"); gPad->SetLogz();
    hA5_P23->GetXaxis()->CenterTitle(); hA5_P23->GetYaxis()->CenterTitle();
    cposi->cd(3);hA5_P67->Draw("colz");gPad->SetLogz();
    hA5_P67->GetXaxis()->CenterTitle(); hA5_P67->GetYaxis()->CenterTitle();
    cposi->cd(4);hA8_P67->Draw("colz");gPad->SetLogz();
    hA8_P67->GetXaxis()->CenterTitle(); hA8_P67->GetYaxis()->CenterTitle();
    for(int a=0;a<NAnode;a++){
        cindi->cd(a+1); hA[a]->Draw(); gPad->SetLogy();
        hA[a]->GetXaxis()->CenterTitle();hA[a]->GetYaxis()->CenterTitle();
    }
    cindi->cd(9);    hGrid->Draw();  gPad->SetLogy();   hGrid->SetLineColor(4);
    hGrid->GetXaxis()->CenterTitle();    hGrid->GetYaxis()->CenterTitle();
    hCath->Draw("same");      hCath->SetLineColor(2);
    legend->AddEntry("hCath","Cathode","l"); legend->AddEntry("hGrid","Grid","l");       legend->Draw("same");
 
}
