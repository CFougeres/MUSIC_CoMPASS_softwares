#ifndef ONLINEVISU_H
#define ONLINEVISU_H

#include "DAQTopology.h"
#include "Builder.h"

TH1F * hBoard;
TH1F * hChannel[NBoard];
TH1F * heleft[NChannel];
TH1F * heright[NChannel];
TH2F* MapFull;
TH2F* MapLeft;
TH2F* MapRight;
TH2F* S0A1l;
TH2F* S0cath;
TH2F* S0grid;
TH2F* A1A2;
TH2F* MapTime;
TH1F* Si_E;
TH1F* Si_calibE;

TCanvas* cSi;
TCanvas* cside;
TCanvas* cfull;
TCanvas * ctim;
TCanvas * Musicleft;
TCanvas * Musicright;
TCanvas * cmult;
TCanvas * cindi;

double calibE(TH1F* calibHisto, TH1F* rawHisto, Double_t param[]){
    TAxis* hax= rawHisto->GetXaxis();
    TRandom*r = new TRandom1();
    int NBins=rawHisto->GetNbinsX();
    double E, counts, rawE,drawE, dE;
    for(int b=0;b<NBins;b++){
        rawE=hax->GetBinCenter(b);    counts= rawHisto->GetBinContent(b);
        drawE=hax->GetBinWidth(b);
        dE=drawE*param[1];//+param[0];
        for(int cc=0;cc<counts;cc++){
            E= rawE*param[1]+param[0];
            E=r->Uniform(E-dE/2., E+dE/2.);
            calibHisto->Fill(E);
        }
    }
    return 1.0;
}
#endif
