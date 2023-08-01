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

Double_t index_inputs[500]={0};
Double_t param_inputs[500][100]={0};

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
int extraction_calib_coef(string file,  Double_t centL[], Double_t sigL[], Double_t centR[], Double_t sigR[]) {
    ifstream in;
    in.open(file.c_str());
    Int_t nlines = 0;
    while (1) {
        in >>   centL[nlines]   >> sigL[nlines]   >> centR[nlines]   >> sigR[nlines]  ; // MeV/a.u.
        if (!in.good()) break;
        nlines++;
    }
    in.close();
    return 1;
}
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
        if(nlines==0 || nlines ==4  || nlines == 9 || nlines == 13 || nlines == 20 || nlines == 23 || nlines == 28){
            in >> index_inputs[nlines] >> variable[nlines] ;
            cout<<index_inputs[nlines]<<" "<<variable[nlines] <<endl;
        }
       if(nlines==6 || nlines ==7 ){
           ws(in);
           getline(in,line);
           parseFloats(line,list_temp);
           list.push_back(list_temp);
          // cout<<list_temp[0]<<" "<<list_temp[1]<<endl;
           nlist++;
           list_temp.clear();
        }
        if(nlines==16 || nlines== 22 || nlines== 26 || nlines== 27){
            in >> index_inputs[nlines] >> variable[nlines] >>   param_inputs[nlines][0]  >>  param_inputs[nlines][1];
            cout<<index_inputs[nlines]<<" "<<variable[nlines] <<" "<<  param_inputs[nlines][0]<<" "<<  param_inputs[nlines][1] <<endl;
        }
        if(nlines==18 || nlines==19){
            in >> index_inputs[nlines] >> variable[nlines] >>  param_inputs[nlines][0]  >>  param_inputs[nlines][1] >>  param_inputs[nlines][2] >>  param_inputs[nlines][3];
            cout<<index_inputs[nlines]<<" "<<variable[nlines] <<" "<<  param_inputs[nlines][0]<<" "<< param_inputs[nlines][1] <<" "<<  param_inputs[nlines][2]<<" "<<  param_inputs[nlines][3] <<endl;
        }
        if(nlines!=0 && nlines!=4 && nlines!=6 && nlines!=7 && nlines!=9 && nlines!=13 && nlines!=16 && nlines!=18 && nlines!=19 && nlines!=20 && nlines!=22 && nlines!=23 && nlines!=26 && nlines!=27 && nlines!=28){
            in >> index_inputs[nlines] >> variable[nlines] >>  param_inputs[nlines][0];
            cout<<index_inputs[nlines]<<" "<<variable[nlines] <<" "<<  param_inputs[nlines][0] <<endl;
       }
        if (!in.good()) break;
        nlines++;
    }
    in.close();
    cout<<"\nRunNumber "<<" FilesPerRun " <<endl;
    for(int r=0;r<param_inputs[5][0];r++){
        param_inputs[6][r]= int(list[0][r+1]);
        param_inputs[7][r]=int(list[1][r+1]);
        cout<< param_inputs[6][r]<<" "<<param_inputs[7][r]<<endl;
    }
    
    return 1;
}
    
int extraction_ShiftBoardTime(int RunNumber, Double_t shiftTime[NBoard]){
    int find =0;
    int Run_list[MaxN]; Double_t Shift_list[MaxN][NBoard];
    string pathSys = gSystem->pwd();
    string file=  pathRun + "/build/ShiftBoardTime.dat";  cout<<file<<endl;
    ifstream in;
    in.open(file.c_str());
    Int_t nlines = 0;
    while (1) {
        in >>  Run_list[nlines]   >> Shift_list[nlines][0]   >>  Shift_list[nlines][1] >> Shift_list[nlines][2] >> Shift_list[nlines][3] ; // sec
        if (!in.good()) break;
        nlines++;
    }
    in.close();
    for(int n=0;n<nlines+1;n++){
        cout<<RunNumber<<endl;
        if(Run_list[n]==RunNumber){
            for(int b=0;b<NBoard;b++){shiftTime[b]=Shift_list[n][b];}
            find=1;
           
        }
    }
    return find;
}


#endif
