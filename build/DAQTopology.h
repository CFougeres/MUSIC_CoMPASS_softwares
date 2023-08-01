#ifndef DAQTOPOLOGY_H
#define DAQTOPOLOGY_H

#define MaxN 1000
#define NChannel 16
#define NBoard 4
#define Nstrip 18
string pathRun  = gSystem->pwd();
//Left 0->15
//Right 16->31
//Individual{32=Grid, 33=S0, 34=cathode, 35=S17, 36=Si_dE, 100>pulser},
//100=empty
int Map_DAQCha_to_MUSICStrip[NBoard][NChannel]={{34,100, 1, 100, 33, 101, 5, 100, 0, 100, 9, 100, 17, 13, 100, 32},
                                    {2, 100, 16, 100, 21, 102, 20, 100, 8, 100, 24, 100, 27, 28, 100, 14},
                                    {19, 100, 3, 100, 6, 103, 7, 100, 25, 100, 11, 100, 12, 15, 100, 10},
                                    {4, 100, 18, 36, 23, 104, 22, 100, 29, 100, 26, 100, 31, 30, 100, 35}};
int Er[2]={0,16000};
double bin_raw_to_MeV=4.;
int binEr= int((Er[1]-Er[0])/bin_raw_to_MeV);
double strip[Nstrip]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};

#endif
