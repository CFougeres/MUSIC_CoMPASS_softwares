#ifndef ANALYSIS_H
#define ANALYSIS_H

#define max_beam_built 50
#define max_reac_built 200

#include "../build/DAQTopology.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TSystem.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <TTreeReader.h>
#include <TStopwatch.h>
#include <TGraph.h>
#include <TRandom3.h>

Double_t de_l[NChannel];
Double_t de_r[NChannel];
Double_t stp0;
Double_t stp17;
Double_t grid;
Double_t cath;
Double_t evt_time;
Double_t evt_mult;
Double_t si_de; Double_t si_e;

TCanvas* cPIDbeam;
TCanvas* cdEE;
TCanvas* cTraces;
TH2F*  hPIDBeam;
TH2F*  hdEE;
TGraph* beam[max_beam_built];
TGraph* reac[max_reac_built];

int compteur_beam_strip=0;
int compteur_beam=0;int compteur_reaction=0;
double calibEtot[Nstrip];double deriva_calibEtot[Nstrip];

int rej_DERIVA=0;    double prod_Eanode=0.;
double mult=0;
double dE=0;double Eres=0;

Double_t index_inputs[500]={0};
Double_t param_inputs[500][100]={0};

void parseFloats( const string& s , vector<double>& res) {
    istringstream isf(s);
    int comp=0;
    while ( isf.good() ) {
        if(comp==0 || comp>1){
            double value;
            isf >> value;
            res.push_back(value);
           //cout<<res.at(0)<<endl;
        }
        if(comp==1){
            char value[200];
            isf >> value;
           // cout<<value<<endl;
        }
        comp++;
    }
   // return res;
}
//********************************************
int extraction_inputs(){
    cout<<pathRun<<endl;    string path_input = pathRun +"/analysis/inputs_analysis.dat";     cout<<path_input<<endl;
    char variable[500][200]={0};
    vector<vector<double>> list;
    vector<double> list_temp;
    ifstream in;
    in.open(path_input.c_str());
    Int_t nlines = 0;   string line; int nlist = 0;
    while(1){
       if(nlines==1 || nlines ==2){
           ws(in);
           getline(in,line);
           parseFloats(line,list_temp);
           list.push_back(list_temp);
          // cout<<list_temp[0]<<" "<<list_temp[1]<<endl;
           nlist++;
           list_temp.clear();
        }
        if(nlines==4 || nlines== 6){
            in >> index_inputs[nlines] >> variable[nlines] >>   param_inputs[nlines][0]  >>  param_inputs[nlines][1];
            cout<<index_inputs[nlines]<<" "<<variable[nlines] <<" "<<  param_inputs[nlines][0]<<" "<<  param_inputs[nlines][1] <<endl;
        }
        if(nlines!=1 && nlines!=2 && nlines!=4 && nlines!=6){
            in >> index_inputs[nlines] >> variable[nlines] >>  param_inputs[nlines][0];
            cout<<index_inputs[nlines]<<" "<<variable[nlines] <<" "<<  param_inputs[nlines][0] <<endl;
       }
        if (!in.good()) break;
        nlines++;
    }
    in.close();
    cout<<"\nRunNumber "<<" FilesPerRun " <<endl;
    for(int r=0;r<param_inputs[0][0];r++){
        param_inputs[1][r]= int(list[0][r+1]);
        param_inputs[2][r]=int(list[1][r+1]);
        cout<< param_inputs[1][r]<<" "<<param_inputs[2][r]<<endl;
    }
    return 1;
}
#endif
