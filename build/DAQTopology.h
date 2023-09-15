#ifndef DAQTOPOLOGY_H
#define DAQTOPOLOGY_H

#define MaxN 1000
#define NChannel 16
#define NBoard 1
#define NAnode 8

//100=empty
// 1->8 anodes
//9 Ca1
//10 Ca4
//20 Silicon
int Map_DAQCha_to_MONICAsignal[NBoard][NChannel]={{1, 2, 100, 3, 4, 100, 5, 100, 6, 7, 100, 8, 9, 20, 100, 10}};

int temp_index;

double strip[NAnode]={1, 2, 3, 4, 5, 6, 7, 8};


#endif
