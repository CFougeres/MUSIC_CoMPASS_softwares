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
		Tree_data[r]= new TChain("Data_R");
        run_number[r]= int(param_inputs[4][r]);
        files_per_run[r]=int(param_inputs[5][r]);
        init_file[r]=int(param_inputs[6][r]);
        for(int f=init_file[r];f<files_per_run[r];f++){
            if(f==0){fileName = pathRun + Form("/DAQ/run_%i/RAW/DataR_run_%i.root",run_number[r], run_number[r]);}
            if(f>0){ fileName= pathRun+Form("/DAQ/run_%i/RAW/DataR_run_%i_%i.root",run_number[r], run_number[r],f);}
            cout<<fileName<<endl;;
            Tree_data[r]->Add(fileName.c_str());
        }
    }
    RangePositionCoeff= param_inputs[7][0];
    GainA3toA2= param_inputs[8][0];
	GainA7toA6= param_inputs[9][0];
	
    //"""""""""""""""""""""
    //Data tree structure
    //""""""""""""""""""""" 
    double proportion_of_file_for_Plot=1; 
	for(int r=0;r<Nrun;r++){
		Tree_data[r]->SetBranchAddress("Channel", &Channel);   Tree_data[r]->SetBranchAddress("Board", &Board);
		Tree_data[r]->SetBranchAddress("Timestamp", &Timestamp);  Tree_data[r]->SetBranchAddress("Energy", &Energy);
		nStat[r] = Tree_data[r]->GetEntries();        cout<<nStat[r]<<endl;
		//"""""""""""""""""""""
		//Time sorting
		//"""""""""""""""""""""
		timestampVec[r] = new  Double_t[nStat[r]];    entryVec[r] = new Int_t[nStat[r]];
		sortZero = 0;sortLength=nStat[r]-1;
		for(int j=0;j<nStat[r];j++){
			Tree_data[r]->GetEntry(j);
			timestampVec[r][j]= Timestamp;
			entryVec[r][j]=j;
		}
		quicksort(timestampVec[r],entryVec[r],sortZero,sortLength);cout<<"sorted "<<endl;
		if(timestampVec[r][0]*1e-12<Tr[0]){Tr[0]=timestampVec[r][0]*1e-12;};
		Tr[1]+=timestampVec[r][nStat[r]-1]*1e-12;
	}
	binTr=(Tr[1]-Tr[0]);
    //"""""""""""""""""""""
    //Plot definition
    //"""""""""""""""""""""
   // gStyle->SetOptLogy(1);
    TimingRate = new TH1F("TimingRate","Timing;Timestamp (s);Counts/(9x1s)",binTr, Tr[0], Tr[1]);
    EvtMultiplicity = new TH2F("EvtMultiplicity",";Evt time (s);Multiplicity",binTr,Tr[0],Tr[1],16,0,15);
    hA4_P23 = new TH2F("hA4_P23",";P23 (arb. u.); #DeltaE_{4} (arb. u.)",binEr, -0.5*Er[1], 0.5*Er[1],binEr, Er[0], Er[1]);
    hA5_P23 = new TH2F("hA5_P23",";P23 (arb. u.); #DeltaE_{5} (arb. u.)",binEr, -0.5*Er[1], 0.5*Er[1],binEr, Er[0], Er[1]);
    hA5_P67 = new TH2F("hA5_P67",";P67 (arb. u.); #DeltaE_{5} (arb. u.)",binEr, -0.5*Er[1], 0.5*Er[1],binEr, Er[0], Er[1]);
    hA8_P67 = new TH2F("hA8_P68",";P67 (arb. u.); #DeltaE_{8} (arb. u.)",binEr, -0.5*Er[1], 0.5*Er[1],binEr, Er[0], Er[1]);

    for( int i = 0 ; i < NAnode; i++){
        hA[i] = new TH1F(Form("hA%0d",i), Form("A%0d;#DeltaE (a.u.)",i+1), binEr, Er[0], Er[1]);
    }
    hCa4 = new TH1F("hCa4","Ca4;#DeltaE (a.u.)", binEr, Er[0], Er[1]);
    hCa1 = new TH1F("hCa1","Ca1;#DeltaE (a.u.)", binEr, Er[0], Er[1]);
    hSi = new TH1F("hSi", "hSi;#DeltaE (a.u.)", binEr, Er[0], Er[1]);
    csi = new TCanvas("csi","csi",800,800);
    crate= new TCanvas("crate","crate",1200,1200);crate->Divide(1,2);
    cposi= new TCanvas(" cposi", " cposi", 1600, 1600);  cposi->Divide(2,2);
    cindi= new TCanvas("cindi","cindi",1600,1600);cindi->Divide(3,3);
    //"""""""""""""""""""""
    //Reading events
    //"""""""""""""""""""""
    P23=0;  P67=0; evt_mult=0;
    for(int r=0;r<Nrun;r++){
    Tree_data[r]->GetEntry(entryVec[r][0]);
    evt_time =(timestampVec[r][0])*1e-12;
    temp_index=Map_DAQCha_to_MONICAsignal[0][Channel];
    if(temp_index<NAnode+1){  dE_anode[temp_index-1]=Energy;TimingRate->Fill(tempTime+evt_time);  hA[temp_index-1]->Fill(dE_anode[temp_index-1]);evt_mult+=1;    }
    if(temp_index==NAnode+1){ Ca1=Energy;TimingRate->Fill(tempTime+evt_time);   hCa1->Fill(Ca1);evt_mult+=1;  }
    if(temp_index==NAnode+2){ Ca4=Energy;  hCa4->Fill(Ca4);}// TimingRate->Fill(tempTime+evt_time);  evt_mult+=1; }
    if(temp_index==20){Si_dE=Energy; TimingRate->Fill(tempTime+evt_time);   hSi->Fill(Si_dE);evt_mult+=1; }
    if(r==0){tempTime=0;}
    if(r>0){tempTime=timestampVec[r-1][nStat[r-1]-1]*1e-12;}
    for(int j=1;j<nStat[r]*proportion_of_file_for_Plot;j++){
        Tree_data[r]->GetEntry(entryVec[r][j]);
        if(timestampVec[r][j]-timestampVec[r][j-1]<0){ std::cout<<"issue time sorting "<<std::endl;break;}
        diffTimeEvent=(timestampVec[r][j]*1e-12-evt_time)*1e6;
        if(diffTimeEvent>TimeWindow){
            EvtMultiplicity->Fill(tempTime+evt_time, evt_mult);
            evt_time=(timestampVec[r][j])*1e-12;
            if(dE_anode[1]*dE_anode[2]>0){P23 = RangePositionCoeff*((dE_anode[1]-GainA3toA2*dE_anode[2])/(dE_anode[1]+GainA3toA2*dE_anode[2]));}
           // cout<<P23 <<endl;
            if(dE_anode[5]*dE_anode[6]>0) {P67 =  RangePositionCoeff*((dE_anode[5]-GainA7toA6*dE_anode[6])/(dE_anode[5]+GainA7toA6*dE_anode[6]));}
            hA4_P23->Fill(P23, dE_anode[3]); hA5_P23->Fill(P23,dE_anode[4]);
            hA5_P67->Fill(P67, dE_anode[4]); hA8_P67->Fill(P67, dE_anode[7]);
            for(int a=0;a<NAnode;a++){
                dE_anode[a]=0.;
            }
            evt_mult=0;  P23=0;  P67=0;
        }
        temp_index=Map_DAQCha_to_MONICAsignal[0][Channel];
        if(temp_index<NAnode+1){   dE_anode[temp_index-1]=Energy;TimingRate->Fill(tempTime+evt_time);  hA[temp_index-1]->Fill( dE_anode[temp_index-1]);evt_mult+=1;        }
        if(temp_index==NAnode+1){  Ca1=Energy;TimingRate->Fill(tempTime+evt_time);   hCa1->Fill(Ca1);evt_mult+=1;  }
        if(temp_index==NAnode+2){  Ca4=Energy;    hCa4->Fill(Ca4);}//evt_mult+=1; TimingRate->Fill(evt_time); }
        if(temp_index==20){Si_dE=Energy; TimingRate->Fill(tempTime+evt_time);   hSi->Fill(Si_dE); evt_mult+=1;}
    }
	}
    gStyle->SetOptStat("nei");
    gStyle->SetPalette(kThermometer);
    csi->cd();hSi->Draw();
    hSi->GetXaxis()->CenterTitle();hSi->GetYaxis()->CenterTitle();
    crate->cd(1);TimingRate->Scale(1./9.);TimingRate->Draw("HIST");
    TimingRate->GetXaxis()->CenterTitle();TimingRate->GetYaxis()->CenterTitle();TimingRate->GetYaxis()->SetRangeUser(0,200);
    crate->cd(2); EvtMultiplicity->Draw("colz");
    EvtMultiplicity->GetXaxis()->CenterTitle();EvtMultiplicity->GetYaxis()->CenterTitle();
    cposi->cd(1); hA4_P23->Draw("colz"); gPad->SetLogz();
    hA4_P23->GetXaxis()->CenterTitle(); hA4_P23->GetYaxis()->CenterTitle(); hA4_P23->GetYaxis()->SetTitleOffset(0.9);
    cposi->cd(2); hA5_P23->Draw("colz"); gPad->SetLogz();
    hA5_P23->GetXaxis()->CenterTitle(); hA5_P23->GetYaxis()->CenterTitle(); hA5_P23->GetYaxis()->SetTitleOffset(0.9);
    cposi->cd(3);hA5_P67->Draw("colz");gPad->SetLogz();
    hA5_P67->GetXaxis()->CenterTitle(); hA5_P67->GetYaxis()->CenterTitle(); hA5_P67->GetYaxis()->SetTitleOffset(0.9);
    cposi->cd(4);hA8_P67->Draw("colz");gPad->SetLogz();
    hA8_P67->GetXaxis()->CenterTitle(); hA8_P67->GetYaxis()->CenterTitle();hA8_P67->GetYaxis()->SetTitleOffset(0.9);
    for(int a=0;a<NAnode;a++){
        cindi->cd(a+1); hA[a]->Draw(); gPad->SetLogy();
        hA[a]->GetXaxis()->CenterTitle();hA[a]->GetYaxis()->CenterTitle();
    }
    cindi->cd(9);  hCa4->Draw();  gPad->SetLogy();  hCa4->SetLineColor(4);
    hCa4->GetXaxis()->CenterTitle();    hCa4->GetYaxis()->CenterTitle();
    hCa1->Draw("same");      hCa1->SetLineColor(2);
    legend->AddEntry("hCa1","Ca1","l"); legend->AddEntry("hCa4","Ca4","l");       legend->Draw("same");
 
}
