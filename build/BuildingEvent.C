//##############################################################
//Script to convert .Root list Trees to Eventlike .Root Trees
//##############################################################
#include "DAQTopology.h"
#include "Builder.h"

using namespace std;
int BuildingEvent(int RunNumber, int file)
{
    printf("=====================================\n");
    printf("===     Builder of Events Tree    ===\n");
    printf("=====================================\n");
    
    //"""""""""""""""""""""
    //Users' inputs
    //"""""""""""""""""""""
    applied_extraction_inputs = extraction_inputs();
    TimeWindow  = param_inputs[1][0];
    RangePositionCoeff= param_inputs[7][0];
    if(file==0){fileName = pathRun + Form("/DAQ/run_%i/RAW/DataR_run_%i.root",RunNumber,RunNumber);}
    if(file>0){ fileName= pathRun+Form("/DAQ/run_%i/RAW/DataR_run_%i_%i.root",RunNumber, RunNumber,file);}
    cout<< fileName<<endl;;
    Tree_data->Add(fileName.c_str());
    //"""""""""""""""""""""
    //Data tree structure
    //"""""""""""""""""""""
    Tree_data->SetBranchAddress("Channel", &Channel);    Tree_data->SetBranchAddress("Board", &Board);
    Tree_data->SetBranchAddress("Timestamp", &Timestamp);  Tree_data->SetBranchAddress("Energy", &Energy);
    nStat = Tree_data->GetEntries();        cout<<nStat<<endl;
	for(int t=0;t<10;t++){
        Tree_Event[t]=new TTree("tree", "tree");
        Tree_Event[t]->Branch("dE_anode", &dE_anode,Form("dE_anode[%i]/D",NAnode)); Tree_Event[t]->SetBranchAddress("dE_anode", &dE_anode);
        Tree_Event[t]->Branch("P23", &P23, "P23/D");    Tree_Event[t]->SetBranchAddress("P23",&P23);
        Tree_Event[t]->Branch("P67", &P67, "P67/D");    Tree_Event[t]->SetBranchAddress("P67",&P67);
        Tree_Event[t]->Branch("Si_dE", &Si_dE, "Si_dE/D");  Tree_Event[t]->SetBranchAddress("Si_dE",&Si_dE);
        Tree_Event[t]->Branch("Ca1", &Ca1, "Ca1/D");   Tree_Event[t]->SetBranchAddress("Ca1",&Ca1);
        Tree_Event[t]->Branch("Ca4", &Ca4, "Ca4/D");   Tree_Event[t]->SetBranchAddress("Ca4",&Ca4);
        Tree_Event[t]->Branch("evt_time", &evt_time, "evt_time/D");   Tree_Event[t]->SetBranchAddress("evt_time",&evt_time);
        Tree_Event[t]->Branch("evt_mult", &evt_mult, "evt_mult/D");   Tree_Event[t]->SetBranchAddress("evt_mult",&evt_mult);
    }

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
    
    //"""""""""""""""""""""
    //Building event
    //"""""""""""""""""""""
    string path_to_con = pathRun + Form("/RootFiles/Event/Run_%i/file%i/",RunNumber,file);
    for(int s=0;s<NAnode;s++){        dE_anode[s]=0.0;    }
    P23=0.0;P67=0.0;Ca4=0.0;Ca1=0.0;evt_time=0.; evt_mult=0;Si_dE=0;
    int compteur_tree;     compteur_tree=0;
    double  diffTimeEvent;
    gSystem->cd(path_to_con.c_str());
    outputFile[compteur_tree] = new TFile(Form("tree_%i.root",compteur_tree), "RECREATE");
    Tree_data->GetEntry(entryVec[0]);
    evt_time =(timestampVec[0])*1e-12;
    temp_index=Map_DAQCha_to_MONICAsignal[0][Channel];
    if(temp_index<NAnode+1){  dE_anode[temp_index-1]=Energy; evt_mult+=1;}
    if(temp_index==NAnode+1){ Ca1=Energy;evt_mult+=1;}
    if(temp_index==NAnode+2){ Ca4=Energy; evt_mult+=1; }
    if(temp_index==20){Si_dE=Energy; evt_mult+=1; }
    for(int i = double(nStat)*double(compteur_tree*0.1)+1;i<run_portion*nStat;i++){
        if(i/double(nStat)*100.>(compteur_tree+1)*10){
            cout<<" new tree "<< endl;
            Tree_Event[compteur_tree]->Write();
            outputFile[compteur_tree]->Close();
            compteur_tree=compteur_tree+1;
            gSystem->cd(path_to_con.c_str());
            outputFile[compteur_tree] = new TFile(Form("tree_%i.root",compteur_tree), "RECREATE");
        }
        if(timestampVec[i]-timestampVec[i-1]<0){ std::cout<<"issue time sorting "<<std::endl;break;}
        Tree_data->GetEntry(entryVec[i]);
        diffTimeEvent=(timestampVec[i]*1e-12-evt_time)*1e6;
        if(diffTimeEvent>TimeWindow){
            cout<<i/double(nStat)*100.<<endl;
            if(dE_anode[1]*dE_anode[2]>0){P23 =  RangePositionCoeff*((dE_anode[1]-dE_anode[2])/(dE_anode[1]+dE_anode[2]));}
            if(dE_anode[5]*dE_anode[6]>0) {P67 =  RangePositionCoeff*((dE_anode[5]-dE_anode[6])/(dE_anode[5]+dE_anode[6]));}
            Tree_Event[compteur_tree]->Fill();
            //reinitialisation event
            P23=0.0;P67=0.0;Ca4=0.0;Ca1=0.0;Si_dE=0;
            evt_mult=0; evt_time=timestampVec[i]*1e-12;
            for(int a=0;a<NAnode;a++){
                dE_anode[a]=0.;
            }
        }
        temp_index=Map_DAQCha_to_MONICAsignal[0][Channel];
        if(temp_index<NAnode+1){ dE_anode[temp_index-1]=Energy;evt_mult+=1;    }
        if(temp_index==NAnode+1){  Ca1=Energy;evt_mult+=1;}
        if(temp_index==NAnode+2){  Ca4=Energy;  evt_mult+=1;}
        if(temp_index==20){Si_dE=Energy; evt_mult+=1; }
    }
    Tree_Event[compteur_tree]->Write();
    outputFile[compteur_tree]->Close();
    std::cout<<"run final time "<<timestampVec[int(run_portion*nStat)-1]*1e-12<<std::endl;
    return 1;
}
