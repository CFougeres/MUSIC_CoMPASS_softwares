//##############################################################
//Script to convert .Root list Trees to Eventlike .Root Trees
//##############################################################
#include "DAQTopology.h"
#include "Builder.h"

using namespace std;
int BuildingEvent(int RunNumber, int file)
{
    printf("=====================================\n");
    printf("===       Builder to Root         ===\n");
    printf("=====================================\n");
    //USER INPUTS
    int applied_extraction_inputs = extraction_inputs();
    double Ibeam = param_inputs[21][0]; int applied_calib = param_inputs[3][0]; double TimeWindow  = param_inputs[1][0];
    double normMUSIC  = param_inputs[2][0];
    Double_t BeamExternal[2]={param_inputs[22][0], param_inputs[22][1]};
    
    TChain* MUSICdata = new TChain("Data_R");
    string fileName =  pathRun + Form("/RootFiles/Raw/Data_R_%i_%i.root",RunNumber,file);
    cout<<fileName<<endl;    MUSICdata->Add(fileName.c_str());
    //"""""""""""""""""""""
    //Structure saved converted file
    //"""""""""""""""""""""
    double run_portion = 1.;
	TFile* outputFile[10];  TTree* MUSICEvent[10];
    //"""""""""""""""""""""
    //Data tree structure
    //"""""""""""""""""""""
    MUSICdata->SetBranchStatus("*", 0);
    Double_t de_l[NChannel];    Double_t de_r[NChannel];
    Double_t stp0;    Double_t stp17;    Double_t cath; Double_t grid;
    Double_t evt_time; Double_t evt_mult;
    unsigned short Channel;
    unsigned long long Timestamp;
    unsigned short Board;
    unsigned short Energy;
    MUSICdata->SetBranchStatus("Channel", 1);MUSICdata->SetBranchAddress("Channel", &Channel);
    MUSICdata->SetBranchStatus("Timestamp", 1);MUSICdata->SetBranchAddress("Timestamp", &Timestamp);
    MUSICdata->SetBranchStatus("Board", 1);MUSICdata->SetBranchAddress("Board", &Board);
    MUSICdata->SetBranchStatus("Energy", 1);MUSICdata->SetBranchAddress("Energy", &Energy);
	for(int t=0;t<10;t++){
        MUSICEvent[t]=new TTree("tree", "tree");
        MUSICEvent[t]->Branch("de_l", &de_l,Form("de_l[%i]/D",NChannel)); MUSICEvent[t]->SetBranchAddress("de_l", &de_l);
        MUSICEvent[t]->Branch("de_r", &de_r,Form("de_r[%i]/D",NChannel));   MUSICEvent[t]->SetBranchAddress("de_r", &de_r);
        MUSICEvent[t]->Branch("stp0", &stp0, "stp0/D");    MUSICEvent[t]->SetBranchAddress("stp0",&stp0);
        MUSICEvent[t]->Branch("stp17", &stp17, "stp17/D");  MUSICEvent[t]->SetBranchAddress("stp17",&stp17);
        MUSICEvent[t]->Branch("cath", &cath, "cath/D");   MUSICEvent[t]->SetBranchAddress("cath",&cath);
        MUSICEvent[t]->Branch("grid", &grid, "grid/D");   MUSICEvent[t]->SetBranchAddress("grid",&grid);
        MUSICEvent[t]->Branch("evt_time", &evt_time, "evt_time/D");   MUSICEvent[t]->SetBranchAddress("evt_time",&evt_time);
        MUSICEvent[t]->Branch("evt_mult", &evt_mult, "evt_mult/D");   MUSICEvent[t]->SetBranchAddress("evt_mult",&evt_mult);
    }
    int nStat = MUSICdata->GetEntries();
    std::cout<<nStat<<std::endl;
    
    Double_t shiftTime[NBoard]; int looked_for_shift = extraction_ShiftBoardTime(RunNumber,shiftTime);
    if(looked_for_shift==0){for(int b=0;b<NBoard;b++){shiftTime[b]=0;} cout<<"!!! time shifts per board not found !!!"<<endl;}
    if(looked_for_shift==1){for(int b=0;b<NBoard;b++){cout<<Form("Board shift %i ",b)<<shiftTime[b]<<endl;}}

    //"""""""""""""""""""""
    //Calibration
    //"""""""""""""""""""""
    Double_t gainStp17 =1.0; Double_t gainStp0=1.0;        Double_t gainCath=1.;        Double_t gainStpL[NChannel]={1.0}; Double_t gainStpR[NChannel]={1.0};
    Double_t centroidL[NChannel]; Double_t centroidR[NChannel];        Double_t sigmaL[NChannel]; Double_t sigmaR[NChannel];
    string fileCoeffCalib;  int looked_for_calib_coef;
    if(applied_calib==1){
        fileCoeffCalib = pathRun+ Form("/build/calibCoef/run%i.dat",RunNumber);
        looked_for_calib_coef = extraction_calib_coef(fileCoeffCalib,  centroidL, sigmaL, centroidR, sigmaR);
        gainStp0 = normMUSIC/BeamExternal[0];
        gainStp17= normMUSIC/BeamExternal[1];
        for(int k=0;k<NChannel;k++){   gainStpL[k]=normMUSIC/centroidL[k]; gainStpR[k]=normMUSIC/centroidR[k]; }
    }
    //"""""""""""""""""""""
    //time alignment
    //"""""""""""""""""""""
    Double_t* timestampVec = new  Double_t[nStat];    Int_t* entryVec = new Int_t[nStat];
    int sortZero = 0;int sortLength=nStat-1;
    for(int j=0;j<nStat;j++){
        MUSICdata->GetEntry(j);
        timestampVec[j]= (Timestamp - shiftTime[Board]*1e+12);
        entryVec[j]=j;
    }
    quicksort(timestampVec, entryVec,sortZero,sortLength);
    std::cout<<"sorted "<<std::endl;
    int PositiveTime=0;
    for(int j=0;j<nStat;j++){
        if(timestampVec[j]>=0){
            PositiveTime=j;
            break;
        }
    }
 
    Double_t TimeEv = timestampVec[PositiveTime]*1e-12;
    //"""""""""""""""""""""
    //Building event
    //"""""""""""""""""""""
    string path_to_con = pathRun + Form("/RootFiles/Event/Run_%i/file%i/",RunNumber,file);
    int event_nb=0; int event_rej_nb=0; int event_rej=0;
    int temp_index;
    for(int s=0;s<NChannel;s++){
            de_l[s]=0.0;            de_r[s]=0.0;
    }
    stp0=0.0;stp17=0.0;grid=0.0;cath=0.0;evt_time=0.; evt_mult=0;
    int compteur_tree;     compteur_tree=0;
    double  diffTimeHit,  diffTimeEvent;
    gSystem->cd(path_to_con.c_str());
    outputFile[compteur_tree] = new TFile(Form("tree_%i.root",compteur_tree), "RECREATE");
    for(int i = double(nStat)*double(compteur_tree*0.1)+1;i<run_portion*nStat;i++){
        MUSICdata->GetEntry(entryVec[i]);
        if(i/double(nStat)*100.>(compteur_tree+1)*10){
            std::cout<<" new tree "<<  std::endl;
            MUSICEvent[compteur_tree]->Write();
            outputFile[compteur_tree]->Close();
            compteur_tree=compteur_tree+1;
            gSystem->cd(path_to_con.c_str());
            outputFile[compteur_tree] = new TFile(Form("tree_%i.root",compteur_tree), "RECREATE");
        }
        if(timestampVec[i+1]-timestampVec[i]<0){ std::cout<<"issue time sorting "<<std::endl;break;}
        //tempTime =  ((Timestamp*1e-12) - shiftTime[Board])*pow(10,6);
        //if(timestampVec[i]>0){
        diffTimeEvent= (timestampVec[i]*1e-12-TimeEv)*1e+6;
        if(diffTimeEvent>TimeWindow){
            std::cout<<i/double(nStat)*100.<<  std::endl;
            evt_time=TimeEv+(TimeWindow*1e-6)/2.;
            if(event_rej==0 && evt_mult>0){
                MUSICEvent[compteur_tree]->Fill();
                event_nb+=1;
            }
            if(event_rej==1){
                event_rej_nb+=1;
                std::cout<<"rejection "<<event_rej_nb<<std::endl;
            }
            //reinitialisation event
            for(int s=0;s<NChannel;s++){
				de_l[s]=0.0;            de_r[s]=0.0;
			}
			stp0=0.0;stp17=0.0;grid=0.0;cath=0.0;evt_time=0.;
            event_rej=0;evt_mult=0;
            TimeEv=timestampVec[i]*1e-12;
        }
        temp_index=Map_DAQCha_to_MUSICStrip[Board][Channel];
		if(temp_index==32){grid=Energy;evt_mult+=1;}
		if(temp_index==33 && stp0>0){event_rej=1;}
        if(temp_index==33 && stp0==0){stp0=Energy*gainStp0;evt_mult+=1;}
		if(temp_index==34){cath=Energy;evt_mult+=1;}
		if(temp_index==35){stp17=Energy*gainStp17;evt_mult+=1;}
		//left side
		if(temp_index<NChannel && de_l[temp_index]>0){event_rej=1;}
		if(temp_index<NChannel && de_l[temp_index]==0){de_l[temp_index]=Energy*gainStpL[temp_index];evt_mult+=1;}
		//right side
		if(temp_index<NChannel*2 && temp_index>NChannel-1  && de_r[temp_index-NChannel]>0){event_rej=1;}
		if(temp_index<NChannel*2 && temp_index>NChannel-1 && de_r[temp_index-NChannel]==0){de_r[temp_index-NChannel]=Energy*gainStpR[temp_index-NChannel]; evt_mult+=1;}
    }
    MUSICEvent[compteur_tree]->Write();
    outputFile[compteur_tree]->Close();
    std::cout<<"build back events "<<event_nb<<std::endl;
    std::cout<<"rejected events "<<event_rej_nb<<std::endl;
    std::cout<<"ratio rej wrt built "<<double(event_rej_nb)/double(event_rej_nb+event_nb)*100.<<std::endl;
    std::cout<<"run final time "<<timestampVec[int(run_portion*nStat)-1]*1e-12<<std::endl;
    Double_t time_converted= timestampVec[int(run_portion*nStat)-1]*1e-12-timestampVec[PositiveTime]*1e-12;
    std::cout<<"building expected eff "<<double(event_nb)/(Ibeam*time_converted)*100.<<std::endl;
    return 1;
}
