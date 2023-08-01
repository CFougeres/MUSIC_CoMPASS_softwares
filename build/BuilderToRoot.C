//##############################################################
//Script for using the BinReader.h to convert .BIN MUSIC_Run to .Root list Trees
//##############################################################
#include "BuilderToRoot.h"
using namespace std;
void BuilderToRoot()     //us unit
{
    printf("=====================================\n");
    printf("===       Builder to Root         ===\n");
    printf("=====================================\n");
    //USER INPUTS
    int applied_extraction_inputs = extraction_inputs();
    int Nrun  = param_inputs[5][0];int init_file = param_inputs[8][0];
    int run_number[MaxNSample] ; int files_per_run[MaxNSample] ;
    for(int r=0;r<Nrun;r++){
        run_number[r]= int(param_inputs[6][r]);
        files_per_run[r]=int(param_inputs[7][r]);
        cout<<run_number[r]<<" "<<files_per_run[r]<<endl;
    }
  
    TString fileName ;  BinReader * reader[Nrun][MaxNSample];
    for(int r=0;r<Nrun;r++){
		for(int f=0;f<files_per_run[r];f++){
			if(f==0){
				if(init_file==0){fileName = pathRun + Form("/DAQ/run_%i/RAW/DataR_run_%i.BIN",run_number[r], run_number[r]);}
				if(init_file>0){	
					fileName = pathRun+  Form("/DAQ/run_%i/RAW/DataR_run_%i_%i.BIN",run_number[r], run_number[r], init_file);
				}
			}
			if(f>0){ 
				fileName= pathRun+Form("/DAQ/run_%i/RAW/DataR_run_%i_%i.BIN",run_number[r], run_number[r],f+init_file);
			}
			printf(fileName);
			reader[r][f]= new BinReader(fileName);  
            if(f>0)reader[r][f]->SetHeader(HeaderSubfile);
            if(f==0 && init_file>0)reader[r][f]->SetHeader(HeaderSubfile);
			reader[r][f]->ScanNumberOfBlock();
			printf("################## Number of data Block : %llu \n", reader[r][f]->GetNumberOfBlock());
			}
	}
    //TREE storing
    //(only first run file for latter on time synchronization)
    TTree* MUSICdata[Nrun][MaxNSample];
    unsigned long long Timestamp;
	unsigned short Energy; 	unsigned short Board;  	unsigned short Channel; 
    for(int r=0;r<Nrun;r++){
        for(int f=0;f<files_per_run[r];f++){
            MUSICdata[r][f] = new TTree("Data_R","Data_R");
            MUSICdata[r][f]->Branch("Channel", &Channel,"Channel/s");MUSICdata[r][f]->SetBranchAddress("Channel", &Channel);
            MUSICdata[r][f]->Branch("Timestamp", &Timestamp,"Timestamp/l");MUSICdata[r][f]->SetBranchAddress("Timestamp", &Timestamp);
            MUSICdata[r][f]->Branch("Board",&Board,"Board/s");MUSICdata[r][f]->SetBranchAddress("Board", &Board);
            MUSICdata[r][f]->Branch("Energy", &Energy,"Energy/s");MUSICdata[r][f]->SetBranchAddress("Energy", &Energy);
        }
    }
	
    //"""""""""""""""""""""
    //time alignment
    //"""""""""""""""""""""
    Double_t* timestampVec[Nrun][MaxNSample];  Double_t* energyVec[Nrun][MaxNSample];  Double_t* boardVec[Nrun][MaxNSample]; Double_t* channelVec[Nrun][MaxNSample];
    Int_t* entryVec[Nrun][MaxNSample];
    int PositiveTime[Nrun][MaxNSample];
	TFile* outputFileSi[Nrun][MaxNSample];
    int sortZero = 0;int sortLength;
    for(int r=0;r<Nrun;r++){
		for(int f=0;f<files_per_run[r];f++){
		timestampVec[r][f] = new  Double_t[reader[r][f]->GetNumberOfBlock()]; entryVec[r][f] = new Int_t[reader[r][f]->GetNumberOfBlock()];
		energyVec[r][f] = new  Double_t[reader[r][f]->GetNumberOfBlock()]; 
		boardVec[r][f] = new  Double_t[reader[r][f]->GetNumberOfBlock()]; 
		channelVec[r][f] = new  Double_t[reader[r][f]->GetNumberOfBlock()]; 
		for(int j = 0; j < reader[r][f]->GetNumberOfBlock();j++){
			reader[r][f]->ReadBlock();
			if( j < 5 ) reader[r][f]->data.Print();
			timestampVec[r][f][j]= reader[r][f]->data.TimeStamp ;
			energyVec[r][f][j]= reader[r][f]->data.Energy;
			channelVec[r][f][j]= reader[r][f]->data.Channel;
			boardVec[r][f][j]= reader[r][f]->data.BoardID;
			entryVec[r][f][j]=j;
		}
        sortLength=reader[r][f]->GetNumberOfBlock()-1;
		quicksort(timestampVec[r][f], entryVec[r][f],sortZero,sortLength);
		std::cout<<"sorted  run "<<run_number[r]<< " file "<<f+init_file<<std::endl;
		for(int j=0;j<reader[r][f]->GetNumberOfBlock();j++){
			if(timestampVec[r][f][j]>=0){
				PositiveTime[r][f]=j;
				break;
			}
		}
	}
}
	double Entemp;	unsigned short ch;    unsigned short bd;
    //"""""""""""""""""""""
    //Saving raw format
    //"""""""""""""""""""""
    string path_to_con = pathRun +"/RootFiles/Raw/";
    TFile* outputFile[Nrun][MaxNSample];
    for(int r=0;r<Nrun;r++){
        for(int f=0;f<files_per_run[r];f++){
            for( int i = 0; i < reader[r][f]->GetNumberOfBlock(); i++){
                ch = channelVec[r][f][entryVec[r][f][i]]; bd = boardVec[r][f][entryVec[r][f][i]]; Entemp=energyVec[r][f][entryVec[r][f][i]];
                Channel=ch; Board =bd;  Timestamp = timestampVec[r][f][i];
                Energy=Entemp;
                MUSICdata[r][f]->Fill();
            }
            printf("run%i_file%i done", run_number[r],f+init_file);
            gSystem->cd(path_to_con.c_str());
            outputFile[r][f]= new TFile(Form("Data_R_%i_%i.root",run_number[r],f+init_file), "RECREATE");
            MUSICdata[r][f]->Write();
            outputFile[r][f]->Close();
        }
    }
}
