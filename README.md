**User guide of MONICA/ANL software tools**

Author: C. Fougères (2023)
Contact: chloe.fougeres@gmail.com

- *Requirements*
1. linux based operating system or MacOS
2. CERN/ROOT version 6  (see https://root.cern.ch/).
3. git
4. bash

>>> corresponds to command in a terminal

- *Installation*
>>> git clone -b MONICA_softwares https://github.com/CFougeres/Simulation_MUSIC/MUSIC_CoMPASS_softwares

- *Project structure*
  
inputs.dat                  -->self explanatory

OnlineVisu.C                -->self explanatory

DAQ/                        -->.BIN run from CoMPASS

    DAQ/run_#/RAW/DataR_run_#_##.BIN
              
build/                      --> codes to convert to .Root data

     "CODES"                -->  (see dedicated section)
     "USER INPUTS"          -->  (see dedicated section)
     
RootFiles/                  --> saved .Root Trees (see dedicated section)


documentation/              --> some figures resulting from the codes

- *Codes*
  
(a) in project directory

OnlineVisu.C
>>>root OnlineVisu.C

BuildingEvent.C
>>>root build/BuildingEvent.C
root [1] BuildingEvent(int RunNumber, int file)



(b) in build/directory
- mkRunDir.sh   (creation of eventlike run directory)
>>> bash mkRunDir.sh #Run_number 0
- cpFile.sh     (copy to create of eventlike subfile directories)
>>> bash cpFile.sh #Run_number #Init_file #Number_of_files_to_be_created

- *User inputs*

(a) in project directory

inputs.dat

(b) in build/ directory

DAQTopology.h                      -->Digital Acquisition Map


- *Saved converted data*
  
RootFiles/Event/               -->Eventlike .Root trees
