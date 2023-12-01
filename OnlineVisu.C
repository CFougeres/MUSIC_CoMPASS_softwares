/**************************************
 * root script for online visualization 
 * of MUSIC data ==1
 * of Si data ==0
***************************************/
#include "build/OnlineVisu.h"

using namespace std;

void OnlineVisu()
{
    printf("=====================================\n");
    printf("===          OnlineVisu           ===\n");
    printf("=====================================\n");
  
    //USER INPUTS
    int applied_extraction_inputs = extraction_inputs();
    int RunNumber = param_inputs[12][0];    int FileNumber = param_inputs[13][0];
    int DetectorType=param_inputs[14][0];       double TimeWindow=param_inputs[1][0];
    int applied_timeSync=param_inputs[2][0]; int applied_calib=param_inputs[4][0];
    Double_t calibSi[2]={param_inputs[5][0],param_inputs[6][1]};
    double normMUSIC = param_inputs[3][0];    Double_t BeamExternal[2]={param_inputs[24][0],param_inputs[24][1]};
    Double_t shiftTime[NBoard]; int looked_for_shift;
    if(applied_timeSync==1){
        looked_for_shift=extraction_ShiftBoardTime(RunNumber,shiftTime);
        if(looked_for_shift==0){for(int b=0;b<NBoard;b++){shiftTime[b]=0;} cout<<"!!! time shifts per board not found !!!"<<endl;}
        if(looked_for_shift==1){for(int b=0;b<NBoard;b++){cout<<Form("Board shift %i ",b)<<shiftTime[b]<<endl;}}
    }

    TChain* Tree_data = new TChain("Data_R"); 	string data_path;
    if(DetectorType==1){
		data_path = pathRun + Form("/RootFiles/Raw/Data_R_%i_%i.root",RunNumber,FileNumber);
	}
    if(DetectorType==0){
        if(FileNumber==0){        data_path = pathRun+ Form("/DAQ/runSi_%i/RAW/DataR_runSi_%i.root",RunNumber, RunNumber);}
        if(FileNumber>0){        data_path = pathRun+ Form("/DAQ/runSi_%i/RAW/DataR_runSi_%i_%i.root",RunNumber, RunNumber,FileNumber);}
    }
	cout<<data_path<<endl;    Tree_data->Add(data_path.c_str());
    //"""""""""""""""""""""
    //Data tree structure
    //""""""""""""""""""""" 
    UShort_t Channel; Tree_data->SetBranchAddress("Channel", &Channel);
    ULong64_t Timestamp;     Tree_data->SetBranchAddress("Timestamp", &Timestamp);
    UShort_t Board;    Tree_data->SetBranchAddress("Board", &Board);
    UShort_t Energy;     Tree_data->SetBranchAddress("Energy", &Energy);
    int nStat = Tree_data->GetEntries();
    cout<<nStat<<endl;
    double proportion_of_file_for_MUSICPlot=1;
    //"""""""""""""""""""""
    //Plot definition
    //"""""""""""""""""""""
    if(DetectorType==1){
        gStyle->SetOptLogz(1);
        if(applied_calib==1){Er[0]=0; Er[1]=normMUSIC*4; bin_raw_to_MeV= 0.1; binEr= int((Er[1]-Er[0])/bin_raw_to_MeV);}
        hBoard = new TH1F ("hBoard", "BoardID",NBoard , 0, NBoard );
        for(int i = 0 ; i < NBoard ; i++){	hChannel[i] = new TH1F(Form("hChannel%0d", i), Form("Board %0d;Channel", i), NChannel, 0, NChannel);}
        for( int i = 0 ; i < NChannel ; i++){
            heleft[i] = new TH1F(Form("heleft%0d",i), Form("Strip %0d;#DeltaE (a.u.)",i+1), binEr, Er[0], Er[1]);
            heright[i] = new TH1F(Form("heright%0d", i), Form("Strip %0d;#DeltaE (a.u.)",i+1), binEr, Er[0], Er[1]);
        }
        MapFull = new TH2F("MapFull",";Strip; #DeltaE (a.u.)", NChannel+2,0,NChannel+2,binEr, Er[0], Er[1]);
        MapLeft = new TH2F("MapLeft","Left;Strip; #DeltaE (a.u.)", NChannel+2,0,NChannel+2, binEr, Er[0], Er[1]);
        MapRight = new TH2F("MapRight","Right;Strip; #DeltaE (a.u.)",  NChannel+2,0,NChannel+2, binEr, Er[0], Er[1]);
        MapTime = new TH2F("MapRTime",Form(";Time (#mus); Board*%i+Channel",NChannel), 5000,0,5000,NBoard*NChannel+1,0,NBoard*NChannel+1);
        S0A1l = new TH2F("S0A1l",";#DeltaE_{0} (a.u.); #DeltaE_{1} left (a.u.)", binEr, Er[0], Er[1], binEr, Er[0], Er[1]);
        S0cath = new TH2F("S0cath",";#DeltaE_{0} (a.u.); Cathode (a.u.)",binEr, Er[0], Er[1],16000/4,0,16000);
        S0grid = new TH2F("S0grid",";#DeltaE_{0} (a.u.);Grid (a.u.)",binEr, Er[0], Er[1],16000/4,0,16000);
        A1A2 = new TH2F("A1A2",";#DeltaE_{1} (a.u.);#DeltaE_{2} (a.u.)",binEr, Er[0], Er[1], binEr, Er[0], Er[1]);
        cside= new TCanvas("cside","cside",800,800);cside->Divide(1,2);
        cfull= new TCanvas("cfull","cfull",800,800);
        ctim= new TCanvas("ctim","ctim",800,800);
        Musicleft = new TCanvas("Musicleft", "Musicleft", 4*400, 4*400);  Musicleft->Divide(4,4);
        Musicright = new TCanvas("Musicright", "Musicright", 4*400, 4*400);  Musicright->Divide(4,4);
        cmult= new TCanvas("cmult", "cmult", 2*400, 3*400); cmult->Divide(2,3);
        cindi= new TCanvas("cindi","cindi",800,800);cindi->Divide(2,2);
        //"""""""""""""""""""""
        //Calibration
        //"""""""""""""""""""""
        Double_t gainStp17=1.0; Double_t gainStp0=1.0;        Double_t gainCath=1.;        Double_t gainStpL[NChannel]={1.0}; Double_t gainStpR[NChannel]={1.0};
        Double_t centroidL[NChannel]; Double_t centroidR[NChannel];        Double_t sigmaL[NChannel]; Double_t sigmaR[NChannel];
        string fileCoeffCalib;  int looked_for_calib_coef;
        if(applied_calib==1){
            fileCoeffCalib = pathRun+ Form("/build/calibCoef/run%i.dat",RunNumber);
            looked_for_calib_coef = extraction_calib_coef(fileCoeffCalib,  centroidL, sigmaL, centroidR, sigmaR);
            gainStp0 = normMUSIC/BeamExternal[0];
            gainStp17= normMUSIC/BeamExternal[1];
            for(int k=0;k<NChannel;k++){  gainStpL[k]=normMUSIC/centroidL[k]; gainStpR[k]=normMUSIC/centroidR[k]; }
        }
        if(applied_calib==0){  for(int k=0;k<NChannel;k++){   gainStpL[k]=1; gainStpR[k]=1;  }}
        //"""""""""""""""""""""
        //READING EVENTS
        //"""""""""""""""""""""
        double grid=0; double cath=0; double left1=0; double right2=0; double s0=0; double s17=0;
        double shiftedTime;     int temp_index;
        Tree_data->GetEntry(0);
        if(applied_timeSync==1){ shiftedTime = (Timestamp - shiftTime[Board]*1e+12)*1e-12;}
        if(applied_timeSync==0){ shiftedTime = Timestamp *1e-12;}
        Double_t TimeEv =shiftedTime;
        double  diffTimeEvent;
        for(int j=0;j<nStat*proportion_of_file_for_MUSICPlot;j++){
            Tree_data->GetEntry(j);
            temp_index=Map_DAQCha_to_MUSICStrip[Board][Channel];
            hBoard->Fill(Board);hChannel[Board]->Fill(Channel);
            //timing
            if(applied_timeSync==1){ shiftedTime = (Timestamp - shiftTime[Board]*1e+12)*1e-12;}
            if(applied_timeSync==0){ shiftedTime = Timestamp *1e-12;}
            MapTime->Fill( shiftedTime*1000.*1000, Board*NChannel+Channel);
            diffTimeEvent= (shiftedTime-TimeEv)*1e+6;
            if(diffTimeEvent>TimeWindow){
                S0A1l->Fill(s0,left1); S0cath->Fill(s0,cath);S0grid->Fill(s0,grid);
                A1A2->Fill(left1,right2);
                grid=0;cath=0;  left1=0;  right2=0; s0=0; s17=0;
                TimeEv=shiftedTime;
            }
            if(temp_index==32){grid=Energy;}
            if(temp_index==34){cath=Energy;}
            if(temp_index==33){
                MapRight->Fill(0., Energy*gainStp0);                MapLeft->Fill(0., Energy*gainStp0);
                MapFull->Fill(0., Energy*gainStp0);
                s0= Energy*gainStp0;
            }
            if(temp_index==35){
                MapRight->Fill(17., Energy*gainStp17);                MapLeft->Fill(17., Energy*gainStp17);
                MapFull->Fill(17., Energy*gainStp17);
                s17= Energy*gainStp17;
            }
            if(temp_index<NChannel){
                MapLeft->Fill(temp_index+1, Energy*gainStpL[temp_index]);
                if(temp_index%2==0){
                    MapFull->Fill(temp_index+1, Energy*gainStpL[temp_index]);
                }
                if(temp_index==0){left1=Energy*gainStpL[temp_index];}
                heleft[temp_index]->Fill(Energy*gainStpL[temp_index]);
            }
            if(temp_index<NChannel*2 && temp_index>NChannel-1){
                MapRight->Fill(temp_index-NChannel+1, Energy*gainStpR[temp_index-NChannel]);
                if(temp_index%2==1){
                    MapFull->Fill(temp_index-NChannel+1, Energy*gainStpR[temp_index-NChannel]);
                }
                if(temp_index==NChannel+1){right2=Energy*gainStpR[temp_index-NChannel];}
                heright[temp_index-NChannel]->Fill(Energy*gainStpR[temp_index-NChannel]);
            }
        }
        gStyle->SetOptStat("nei");
        gStyle->SetPalette(kThermometer);
        cmult->cd(1); hBoard->Draw();   hBoard->GetYaxis()->SetRangeUser(0., 1.3*hBoard->GetMaximum());
        for( int i = 0; i < NBoard; i++){cmult->cd(i+2); hChannel[i]->Draw();hChannel[i]->GetYaxis()->SetRangeUser(0, 1.3*hChannel[i]->GetMaximum());}
        for( int i = 0; i < NChannel; i++){
            Musicleft->cd(i+1);    heleft[i]->Draw("h");
            Musicright->cd(i+1);    heright[i]->Draw("h");
        }
        ctim->cd();MapTime->Draw("colz");MapTime->GetYaxis()->CenterTitle();MapTime->GetXaxis()->CenterTitle();MapTime->GetYaxis()->SetTitleOffset(1.5);
        cfull->cd();MapFull->Draw("colz");MapFull->GetYaxis()->CenterTitle();MapFull->GetXaxis()->CenterTitle();MapFull->GetYaxis()->SetTitleOffset(1.6);
        cside->cd(1);MapLeft->Draw("colz");MapLeft->GetYaxis()->CenterTitle();MapLeft->GetXaxis()->CenterTitle();MapLeft->GetYaxis()->SetTitleOffset(1.5);
        cside->cd(2);MapRight->Draw("colz");MapRight->GetYaxis()->CenterTitle();MapRight->GetXaxis()->CenterTitle();MapRight->GetYaxis()->SetTitleOffset(1.5);
        cindi->cd(1);S0A1l->Draw("colz");S0A1l->GetYaxis()->CenterTitle();S0A1l->GetXaxis()->CenterTitle();S0A1l->GetYaxis()->SetTitleOffset(1.6);
        cindi->cd(2);A1A2->Draw("colz");A1A2->GetYaxis()->CenterTitle();A1A2->GetXaxis()->CenterTitle();A1A2->GetYaxis()->SetTitleOffset(1.6);
        cindi->cd(3);S0cath->Draw("colz");S0cath->GetYaxis()->CenterTitle();S0cath->GetXaxis()->CenterTitle();S0cath->GetYaxis()->SetTitleOffset(1.6);
        cindi->cd(4);S0grid->Draw("colz");S0grid->GetYaxis()->CenterTitle();S0grid->GetXaxis()->CenterTitle();S0grid->GetYaxis()->SetTitleOffset(1.6);
    }
    if(DetectorType==0){
        cSi= new TCanvas("cSi","cSi",800,800);cSi->Divide(1,2);        cSi->cd(1);
        Si_E = new TH1F("Si_E",";E (a.u.);Couns/1Channel ",  (Er[1]-Er[0]), Er[0], Er[1]);
        Tree_data->Draw("Energy>>Si_E","Channel==3 && Board==3");
        cSi->cd(1);Si_E->Draw();Si_E->GetYaxis()->CenterTitle();Si_E->GetXaxis()->CenterTitle();Si_E->SetLineColor(4);
        if(applied_calib==1){
            Si_calibE = new TH1F("SicalibE",";E (MeV);Counts/15keV",  600/0.015,0, 600);
            double applied_calibration= calibE(Si_calibE,Si_E,calibSi);
            cSi->cd(2); Si_calibE->Draw(); Si_calibE->GetYaxis()->CenterTitle(); Si_calibE->GetXaxis()->CenterTitle(); Si_calibE->SetLineColor(2);
        }

    }
}
