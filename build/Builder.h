#ifndef BUILDER_H
#define BUILDER_H

#include "DAQTopology.h"
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

string pathRun  = gSystem->pwd();
string fileName;
string path_to_con;

Double_t index_inputs[500]={0};
Double_t param_inputs[500][100]={0};
int applied_extraction_inputs;

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
    cout<<pathRun<<endl;    string path_input = pathRun +"/inputs.dat";     cout<<path_input<<endl;
    char variable[500][200]={0};
    vector<vector<double>> list;
    vector<double> list_temp;
    ifstream in;
    in.open(path_input.c_str());
    Int_t nlines = 0;   string line; int nlist = 0;
    while(1){
        if(nlines==0 || nlines == 2|| nlines == 8){
            in >> index_inputs[nlines] >> variable[nlines] ;
            cout<<index_inputs[nlines]<<" "<<variable[nlines] <<endl;
        }
       if(nlines==4 || nlines ==5  || nlines ==6 ){
           ws(in);
           getline(in,line);
           parseFloats(line,list_temp);
           list.push_back(list_temp);
           nlist++;
           list_temp.clear();
        }
        if(nlines!=0 && nlines!=2 && nlines!=4&& nlines!=5 && nlines!=6 && nlines!=8){
            in >> index_inputs[nlines] >> variable[nlines] >>  param_inputs[nlines][0];
            cout<<index_inputs[nlines]<<" "<<variable[nlines] <<" "<<  param_inputs[nlines][0] <<endl;
       }
        if (!in.good()) break;
        nlines++;
    }
    in.close();
    cout<<"\NumberOfRun " <<" RunsNumbers "<<" FilesPerRun " << " nitSubFiles " <<endl;
    for(int r=0;r<param_inputs[3][0];r++){
        param_inputs[4][r]= int(list[0][r+1]);
        param_inputs[5][r]=int(list[1][r+1]);
        param_inputs[6][r]=int(list[2][r+1]);
        cout<< param_inputs[4][r]<<" "<<param_inputs[5][r]<<" "<<param_inputs[6][r]<<endl;
    }
    
    return 1;
}
#endif

double TimeWindow;
double  diffTimeEvent=0;


void quicksort(Double_t* arr1, Int_t* arr2, int& left, int& right){
    auto i = left;
    auto j = right;
    auto tmp = arr1[0];
    auto pivot = arr1[(int)((left + right)/2)];

    // Partition
    while (i <= j) {
      while (arr1[i] < pivot)
        i++;
      while (arr1[j] > pivot)
        j--;
      if (i <= j) {
        // swap arr1 elements
        tmp = arr1[i];
        arr1[i] = arr1[j];
        arr1[j] = tmp;
        // swap arr2 elements
        tmp = arr2[i];
        arr2[i] = arr2[j];
        arr2[j] = tmp;
        i++;
        j--;
      }
    };
    if (left < j)
      quicksort(arr1, arr2, left, j);
    if (i < right)
      quicksort(arr1, arr2, i, right);
    return;
}

UShort_t Channel;
ULong64_t Timestamp;
UShort_t Board;
UShort_t Energy;
TChain* Tree_data = new TChain("Data_R");
int nStat;
int sortZero;
int sortLength;
Double_t dE_anode[NAnode]={0};
Double_t Cathode; Double_t Grid;
Double_t evt_time; Double_t evt_mult;
Double_t P23; Double_t P67;
Double_t Si_dE;
double RangePositionCoeff;

double run_portion = 1.;
TFile* outputFile[10];  TTree* Tree_Event[10];

