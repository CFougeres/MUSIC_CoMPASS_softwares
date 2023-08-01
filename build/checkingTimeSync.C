#include "DAQTopology.h"
#include "Builder.h"
using namespace std;
void checkingTimeSync()
{
    printf("=====================================\n");
    printf("===      checkingTimeSync         ===\n");
    printf("=====================================\n");

    //USER INPUTS
    int applied_extraction_inputs = extraction_inputs();
    int RunNumber = param_inputs[29][0];    int FileNumber = param_inputs[30][0];     double limE[2]={param_inputs[26][0],param_inputs[26][1]};
    double TimeMin= param_inputs[31][0];//ms
    TChain* MUSICdata = new TChain("Data_R");
    string fileName =  pathRun + Form("/RootFiles/Raw/Data_R_%i_%i.root",RunNumber,RunNumber,FileNumber );
    cout<<fileName<<endl;    MUSICdata->Add(fileName.c_str());     double run_portion=1.0; if(TimeMin<1){run_portion=0.5;}
    Double_t shiftTime[NBoard]; int looked_for_shift = extraction_ShiftBoardTime(RunNumber,shiftTime);
    if(looked_for_shift==0){for(int b=0;b<NBoard;b++){shiftTime[b]=0;} cout<<"!!! time shifts per board not found !!!"<<endl;}
    if(looked_for_shift==1){for(int b=0;b<NBoard;b++){cout<<Form("Board shift %i ",b)<<shiftTime[b]<<endl;}}

    //"""""""""""""""""""""
    //Data tree structure
    //"""""""""""""""""""""
    UShort_t Channel;    ULong64_t Timestamp;    UShort_t Board;    UShort_t Energy;
    MUSICdata->SetBranchStatus("Channel", 1);MUSICdata->SetBranchAddress("Channel", &Channel);
    MUSICdata->SetBranchStatus("Timestamp", 1);MUSICdata->SetBranchAddress("Timestamp", &Timestamp);
    MUSICdata->SetBranchStatus("Board", 1);MUSICdata->SetBranchAddress("Board", &Board);
    MUSICdata->SetBranchStatus("Energy", 1);MUSICdata->SetBranchAddress("Energy", &Energy);
    int nStat = MUSICdata->GetEntries();   cout<<nStat<< endl;

    //"""""""""""""""""""""
    //time alignment
    //"""""""""""""""""""""
	TH2F* ChTimeBefore = new TH2F("ChTimeBefore", Form(";Time (ms);Channel (Bd*%i+Ch)",NChannel),20000,TimeMin,TimeMin+20,NBoard*NChannel+1,0,NBoard*NChannel+1);
    TH2F* ChTimeAfter = new TH2F("ChTimeAfter", Form(";Time (ms);Channel (Bd*%i+Ch)",NChannel),20000,TimeMin,TimeMin+20,NBoard*NChannel+1,0,NBoard*NChannel+1);
    TH1F* dThisto = new TH1F("dThisto", ";#DeltaT_{B0 - B>0} (#mus)",5000 ,-100,100);
    Double_t* timestampVec = new  Double_t[nStat];    Int_t* entryVec = new Int_t[nStat];
    int sortZero = 0;int sortLength=nStat-1;
    for(int j=0;j<nStat;j++){
        MUSICdata->GetEntry(j);
        if(Energy>limE[0] && Energy<limE[1]){  ChTimeBefore->Fill((Timestamp*1e-12)*1000. , Board*NChannel+Channel);}
        timestampVec[j]= (Timestamp - shiftTime[Board]*1e+12);
        entryVec[j]=j;
    }
    quicksort(timestampVec, entryVec,sortZero,sortLength);
    int PositiveTime=0;
    for(int j=0;j<nStat;j++){
        if(timestampVec[j]>=0){
            PositiveTime=j;
            break;
        }
    }
    //"""""""""""""""""""""
    //checking synchronization
    //"""""""""""""""""""""
    Double_t TimeEv = timestampVec[PositiveTime]*1e-12;
    int  temp_index;    double timeB0=0; double timeBother=0;double dt=0;
    //"""""""""""""""""""""
    //Plot definition
    //"""""""""""""""""""""
   for(int i=0;i<run_portion*nStat;i++){
       MUSICdata->GetEntry(entryVec[i]);             temp_index=Map_DAQCha_to_MUSICStrip[Board][Channel];
       if(Energy>limE[0] && Energy<limE[1]){
           ChTimeAfter->Fill((timestampVec[i]*1e-12)*1e+3, Board*NChannel+Channel);
           if(Board==0){ if(temp_index==0 ||  temp_index==17){timeB0= (timestampVec[i]*1e-12)*1e+6;}}
           if(Board>0){
               if(temp_index<NChannel){if(temp_index%2==0){timeBother= (timestampVec[i]*1e-12)*1e+6;}}
               if(temp_index>NChannel-1){if((temp_index-NChannel)%2==1){timeBother= (timestampVec[i]*1e-12)*1e+6;}}
           }
           dt=timeB0-timeBother;
           dThisto->Fill(dt);
       }
    }
    TCanvas* c= new TCanvas("c","c",2000,800);c->Divide(1,2);
    TCanvas* c2= new TCanvas("c2","c2",500,500);
    gStyle->SetPalette(kThermometer);
    c->cd(1);ChTimeBefore->Draw("colz");ChTimeBefore->GetXaxis()->CenterTitle();ChTimeBefore->GetYaxis()->CenterTitle();
    c->cd(2);ChTimeAfter->Draw("colz");ChTimeAfter->GetXaxis()->CenterTitle();ChTimeAfter->GetYaxis()->CenterTitle();
    c2->cd();dThisto->Draw();dThisto->GetXaxis()->CenterTitle();

}
