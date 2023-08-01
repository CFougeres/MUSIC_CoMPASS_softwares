/**************************************
 * General simple identification of reactions in
 * a MUSIC individual strip
***************************************/
#include "MUSICevent.h"
using namespace std;

void MUSICevent()
{
    printf("=====================================\n");
    printf("===          MUSICevent           ===\n");
    printf("=====================================\n");
    
    //********************************************
    // USER conditions
    //********************************************
    int applied_extraction_inputs = extraction_inputs();
    double beam_param[3];//(centroid, sigma, CL)
    beam_param[0]= param_inputs[4][0];     beam_param[1]= param_inputs[4][1];     beam_param[2]=3.0;
    int strip_choice= param_inputs[5][0];
    double thresholdSmooth = param_inputs[7][0];
    double thresholdJump = param_inputs[8][0];
    int lengthsDEE[2] = {int(param_inputs[6][0]),int(param_inputs[6][1])};
    //********************************************
    // Definition data tree
    //********************************************
    int Nrun  = param_inputs[0][0];int init_file = param_inputs[3][0];
    int run_number[MaxN] ; int files_per_run[MaxN] ;
    for(int r=0;r<Nrun;r++){
        run_number[r]= int(param_inputs[1][r]);
        files_per_run[r]=int(param_inputs[2][r]);
        cout<<run_number[r]<<" "<<files_per_run[r]<<endl;
    }
    TChain* MUSICEvent = new TChain("tree"); 	 int nStat;     string fileName ;
    for(int r=0;r<Nrun;r++){
        for(int f=init_file;f<files_per_run[r];f++){
            for(int t=0;t<10;t++){
                fileName= pathRun+Form("/RootFiles/Event/Run_%i/file%i/tree_%i.root",run_number[r],f,t);
                MUSICEvent->Add(fileName.c_str()); std::cout<<fileName<<std::endl;
            }
        }
    }
    MUSICEvent->SetBranchAddress("de_l", &de_l);    MUSICEvent->SetBranchAddress("de_r", &de_r);
    MUSICEvent->SetBranchAddress("stp0",&stp0);		MUSICEvent->SetBranchAddress("stp17",&stp17);
    MUSICEvent->SetBranchAddress("grid",&grid);		MUSICEvent->SetBranchAddress("cath",&cath);
    MUSICEvent->SetBranchAddress("evt_time",&evt_time);		MUSICEvent->SetBranchAddress("evt_mult",&evt_mult);
    nStat = MUSICEvent->GetEntries();
    
    //********************************************
    // Definition distribution to plot
    //********************************************
    Er[0]=0; Er[1]= beam_param[0]*4; bin_raw_to_MeV= 0.1; binEr= int((Er[1]-Er[0])/bin_raw_to_MeV);
    gStyle->SetOptLogz(1);
    cPIDbeam = new TCanvas("cPIDbeam","cPIDbeam",800,800);
    cdEE = new TCanvas(" cdEE"," cdEE",800,800);
    cTraces = new TCanvas("cTraces","cTraces",1200,800);
    hPIDBeam = new TH2F("hPIDBeam",";Grid (arb. u.);#DeltaE_{0} (arb. u.)", 16000/4,0,16000, binEr, Er[0], Er[1]);
    hdEE = new TH2F("hdEE",Form(";#DeltaE_{%i - %i} (a.u.); #DeltaE_{%i - %i} (a.u.)",strip_choice+1,strip_choice+lengthsDEE[1],strip_choice+1,strip_choice+lengthsDEE[0]),  lengthsDEE[1]*binEr,lengthsDEE[1]*Er[0], lengthsDEE[1]*Er[1], lengthsDEE[0]*binEr,  lengthsDEE[0]*Er[0], lengthsDEE[0]* Er[1]);
    hdEE->GetXaxis()->CenterTitle();  hdEE->GetYaxis()->CenterTitle();
    hdEE->GetYaxis()->SetTitleSize(0.05);  hdEE->GetYaxis()->SetLabelSize(0.04);     hdEE->GetYaxis()->SetTitleOffset(0.9);
    hdEE->GetXaxis()->SetTitleOffset(0.92);    hdEE->GetXaxis()->SetTitleSize(0.05);  hdEE->GetXaxis()->SetLabelSize(0.04);
    hPIDBeam->GetXaxis()->CenterTitle(); hPIDBeam->GetYaxis()->CenterTitle();
    hPIDBeam->GetYaxis()->SetTitleSize(0.05); hPIDBeam->GetYaxis()->SetLabelSize(0.04);    hPIDBeam->GetYaxis()->SetTitleOffset(0.9);
    hPIDBeam->GetXaxis()->SetTitleOffset(0.92);   hPIDBeam->GetXaxis()->SetTitleSize(0.05); hPIDBeam->GetXaxis()->SetLabelSize(0.04);
    
    //********************************************
    //Event looking
    //********************************************
    for(int i=0;i<nStat;i++){
        MUSICEvent->GetEntry(i);
        hPIDBeam->Fill(grid,stp0);
        mult=0;  rej_DERIVA=0;     Eres=0.; 		dE= 0;    prod_Eanode=1.0;	compteur_beam_strip=0.;
        calibEtot[0]=stp0;        calibEtot[Nstrip-1]=stp17;   deriva_calibEtot[0]=0.;
        for(int a=0;a<Nstrip-2;a++){
            if(a%2==0){mult= mult+de_r[a];}
            if(a%2==1){mult= mult+de_l[a];}
            calibEtot[a+1] = (de_l[a]+ de_r[a]);   deriva_calibEtot[a+1]=calibEtot[a+1]-calibEtot[a];
            prod_Eanode=prod_Eanode*(de_l[a]+ de_r[a]);
            if(a<strip_choice+lengthsDEE[1]+1 && a>strip_choice){
                Eres+=calibEtot[a];
                if(a<strip_choice+lengthsDEE[0]+1){dE+=calibEtot[a];};
            }
        }
        if(prod_Eanode>0 && mult<1){
            if(compteur_beam<max_beam_built){
                for(int a=0;a<Nstrip;a++){
                    if(TMath::Abs(calibEtot[a]-beam_param[0])<beam_param[1]*beam_param[2]){compteur_beam_strip +=1;}
                }
                if(compteur_beam_strip==Nstrip){
                    beam[compteur_beam] = new TGraph(Nstrip,strip,calibEtot);
                    compteur_beam = compteur_beam+1;
                }
            }
            if(TMath::Abs(calibEtot[strip_choice-1]-beam_param[0])<beam_param[1]*beam_param[2]){
                if(deriva_calibEtot[strip_choice]>=thresholdJump){
                    for(int k=strip_choice+1;k<strip_choice+lengthsDEE[0]+2;k++){
                        if(TMath::Abs(deriva_calibEtot[k])>thresholdSmooth){rej_DERIVA+=1;}
                    }
                    if(rej_DERIVA<1){
                        hdEE->Fill(Eres,dE);
                    }
                    if(compteur_reaction< max_reac_built){
                        reac[compteur_reaction] = new TGraph(Nstrip,strip,calibEtot);
                        compteur_reaction+=1;
                    }
                }
            }
        }
    }
    
    gStyle->SetPalette(kThermometer);
    cPIDbeam->cd();hPIDBeam->Draw("colz");
    cdEE->cd();hdEE->Draw("colz");
    cTraces->cd(); auto legend = new TLegend(0.1, 0.2, 0.4, 0.4);
    if(compteur_beam*compteur_reaction>0){
        legend->AddEntry(beam[0],"Beam-like");        legend->AddEntry(reac[0],"Reaction-like");
        beam[0]->Draw("AC");
        beam[0]->GetYaxis()->SetRangeUser(0, beam_param[0]*4);        beam[0]->GetXaxis()->SetRangeUser(0,Nstrip-1);
        beam[0]->SetTitle(";Strip;#DeltaE (MeV)");
        beam[0]->SetLineColor(1);
        beam[0]->GetYaxis()->CenterTitle();    beam[0]->GetXaxis()->CenterTitle();
        beam[0]->GetYaxis()->SetTitleSize(0.05);beam[0]->GetYaxis()->SetLabelSize(0.04);
        beam[0]->GetXaxis()->SetLabelSize(0.04);beam[0]->GetXaxis()->SetTitleSize(0.05);
        for(int a=1;a<compteur_beam;a++){
            beam[a]->Draw("same");            beam[a]->SetLineColor(1);
        }
        for(int a=0;a<compteur_reaction;a++){
            reac[a]->Draw("same");
            reac[a]->SetLineColor(2);reac[a]->SetLineStyle(2);
        }
        legend->Draw("same");
    }
    else{cout<<"no traces found"<<endl;}
}
