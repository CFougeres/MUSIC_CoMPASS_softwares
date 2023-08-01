//##############################################################
//Script to convert .Root list Trees to Eventlike .Root Trees
//##############################################################
#include "normEsegment.h"
using namespace std;

void normEsegment()
{
    printf("=====================================\n");
    printf("===         normEsegment          ===\n");
    printf("=====================================\n");

    //USER INPUTS
    int applied_extraction_inputs = extraction_inputs();
    int RunNumber = param_inputs[24][0];    int FileNumber = param_inputs[25][0];
    double segEBeamrange[2]= {param_inputs[26][0],param_inputs[26][1]};
    double extEBeamrange[2]= {param_inputs[27][0],param_inputs[27][1]};

    TChain* MUSICdata = new TChain("Data_R");
    string fileName =  pathRun + Form("/RootFiles/Raw/Data_R_%i_%i.root",RunNumber,RunNumber,FileNumber);
    cout<<fileName<<endl;    MUSICdata->Add(fileName.c_str());
    MUSICdata->SetBranchStatus("*", 0);
    //"""""""""""""""""""""
    //Data tree structure
    //"""""""""""""""""""""
    UShort_t Channel;    UShort_t Board;    UShort_t Energy;
    MUSICdata->SetBranchStatus("Channel", 1);MUSICdata->SetBranchAddress("Channel", &Channel);
    MUSICdata->SetBranchStatus("Board", 1);MUSICdata->SetBranchAddress("Board", &Board);
    MUSICdata->SetBranchStatus("Energy", 1);MUSICdata->SetBranchAddress("Energy", &Energy);
    int nStat = MUSICdata->GetEntries();
    cout<<nStat<<endl;
    int temp_index;
    //"""""""""""""""""""""
    //Histogram building
    //"""""""""""""""""""""
    Eraw_lim[0]= new TH1F("Eraw_lim0","S0;#DeltaE (a.u.)", binEr, Er[0], Er[1]);
    Eraw_lim[1]= new TH1F("Eraw_lim1","S17;#DeltaE (a.u.)", binEr, Er[0], Er[1]);
    gCalibLim[0]= new TF1(Form("gCaliblim%i",0),"gaus",  1.5*extEBeamrange[0], 1.5*extEBeamrange[1]);
    gCalibLim[1] = new TF1(Form("gCaliblim%i",1),"gaus",  1.5*extEBeamrange[0], 1.5*extEBeamrange[1]);
    sLim[0] = new TSpectrum(); sLim[1] = new TSpectrum();
    for(int l=0;l<NChannel;l++){
        s[0][l] = new TSpectrum(); s[1][l] = new TSpectrum();
        Eraw_seg[0][l]= new TH1F(Form("Eraw_segleft%i",l),Form("Left S%i;#DeltaE (a.u.)",l+1),  binEr, Er[0], Er[1]);
        Eraw_seg[1][l]= new TH1F(Form("Eraw_segright%i",l),Form("Right S%i;#DeltaE (a.u.)",l+1),  binEr, Er[0], Er[1]);
        gCalib[0][l]= new TF1(Form("gCalib_left%i",l),"gaus",  1.5*segEBeamrange[0], 1.5*segEBeamrange[1]);
        gCalib[1][l] = new TF1(Form("gCalib_right%i",l),"gaus", 1.5*segEBeamrange[0], 1.5*segEBeamrange[1]);
    }
    for(int i=0;i<2;i++){
        cStripSeg[i]= new TCanvas(Form("cStripSeg%i",i),Form("cStripSeg%i",i),2000,800);cStripSeg[i]->Divide(4,4);
    }
    cStripLim = new TCanvas("cStripLim","cStripLim",400,400);cStripLim->Divide(2,2);cStripLim ->cd(1);
    //"""""""""""""""""""""
    //Extraction data
    //"""""""""""""""""""""
    for(int i=0;i<nStat;i++){
        MUSICdata->GetEntry(i);
		temp_index=Map_DAQCha_to_MUSICStrip[Board][Channel];
		if(temp_index==33){  Eraw_lim[0]->Fill(Energy);}
		if(temp_index==35){  Eraw_lim[1]->Fill(Energy);}
		if(temp_index<NChannel){ Eraw_seg[0][temp_index]->Fill(Energy);}
		if(temp_index<NChannel*2 && temp_index>NChannel-1){Eraw_seg[1][temp_index-NChannel]->Fill(Energy);}
    }
    cout<<"data extraction ok "<<endl;
    //"""""""""""""""""""""
    //Fitting beam peaks
    //"""""""""""""""""""""
    for(int l=0;l<2;l++){
        Eraw_lim[l]->SetAxisRange(extEBeamrange[0],extEBeamrange[1],"X");
        NPeaksLim = sLim[l]->Search(Eraw_lim[l], 2, "", 0.9);
        XPeaksLim[l] = sLim[l]->GetPositionX();
        cout<<"Peaks found in S"<<17*l<<" "<<NPeaks <<endl;
        Eraw_lim[l]->Fit(Form("gCaliblim%i",l),"","", XPeaksLim[l][0]*0.9, XPeaksLim[l][0]*1.1);
        Eraw_lim[l]->SetAxisRange(Er[0],Er[1],"X");
    }
    cout<<"calib. external strips done"<<endl;
    for(int l=0;l<NChannel;l++){
        Eraw_seg[0][l]->SetAxisRange(segEBeamrange[0],segEBeamrange[1],"X");
        Eraw_seg[1][l]->SetAxisRange(segEBeamrange[0],segEBeamrange[1],"X");
        //LEFT
        NPeaks = s[0][l]->Search(Eraw_seg[0][l], 2, "", 0.9);   XPeaks[0][l] = s[0][l]->GetPositionX();
        cout<<"Peaks found in left S"<<l+1<<" "<<NPeaks <<endl;
        Eraw_seg[0][l]->Fit(Form("gCalib_left%i",l),"","Q", XPeaks[0][l][0]*0.9, XPeaks[0][l][0]*0.9);
        //RIGHT
        NPeaks = s[1][l]->Search(Eraw_seg[1][l], 2, "", 0.9);  XPeaks[1][l] = s[1][l]->GetPositionX();
        cout<<"Peaks found in right S"<<l+1<<" "<<NPeaks <<endl;
        Eraw_seg[1][l]->Fit(Form("gCalib_right%i",l),"","Q", XPeaks[1][l][0]*0.9, XPeaks[1][l][0]*0.9);
        Eraw_seg[0][l]->SetAxisRange(Er[0],Er[1],"X");        Eraw_seg[1][l]->SetAxisRange(Er[0],Er[1],"X");
    }
    cout<<"calib. segments done"<<endl;
    cout<<"centroid S0 "<< gCalibLim[0]->GetParameter(1)<<endl;     cout<<"centroid S7 "<< gCalibLim[1]->GetParameter(1) <<endl;
    cout<<"left - right results"<<endl;
    for(int l=0;l<NChannel;l++){
        cout<< gCalib[0][l]->GetParameter(1)<<" "<< gCalib[0][l]->GetParameter(2)<< " "<< gCalib[1][l]->GetParameter(1)<<" "<<  gCalib[1][l]->GetParameter(2)<<endl;
    }
    //"""""""""""""""""""""
    //Plotting
    //"""""""""""""""""""""
   // gStyle->SetOptLogy(1);
    for(int i=0;i<2;i++){
        cStripSeg[i]= new TCanvas(Form("cStripSeg%i",i),Form("cStripSeg%i",i),2000,800);cStripSeg[i]->Divide(4,4);
    }
    for(int j=0;j<NChannel;j++){
        cStripSeg[0]->cd(j+1);Eraw_seg[0][j]->Draw();
        cStripSeg[1]->cd(j+1);Eraw_seg[1][j]->Draw();
    }
    cStripLim->cd(1);Eraw_lim[0]->Draw();
    cStripLim->cd(2); Eraw_lim[1]->Draw();
}
