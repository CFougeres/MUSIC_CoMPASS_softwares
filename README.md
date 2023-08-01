User guide of MUSIC/ANL software tools

Author: C. Fougères (2023)
Contact: chloe.fougeres@gmail.com

References: "Process_layout.pdf"


/#################/
Requirements
1. linux based operating system or MacOS
2. CERN/ROOT version 6  (see https://root.cern.ch/).
3. git

">>>" <=> in a terminal

/#################/
Installation
>>>  git clone https://github.com/CFougeres/MUSIC_CoMPASS_softwares


/#################/
Project structure
inputs.dat                  -->self explanatory
OnlineVisu.C                -->self explanatory
DAQ/                        -->.BIN run from CoMPASS
    run_#/RAW/
              DataR_run_#_##.BIN
build/                      --> codes to convert to .Root data
     "CODES"                -->  (see dedicated section)
     "USER INPUTS"          -->  (see dedicated section)
RootFiles/                  --> saved .Root Trees (see dedicated section)
analysis/                   --> a general example code to identify reaction channels in a given strip
documentation/              --> additional information and some figures resulting from the codes

/#################/
Codes 
(a) in project directory
- OnlineVisu.C
>>>root OnlineVisu.C
- BuilderToRoot.C 
>>>root build/BuilderToRoot.C
- OnlineVisu.C 
>>>root build/OnlineVisu.C 
- timeshift.C 
>>>root build/timeshift.C 
- checkingTimeSync.C 
>>>root build/checkingTimeSync.C 
- normEsegment.C 
>>>root build/normEsegment.C 
- BuildingSimple.C
>>>root build/BuildingSimple.C
root [0] Building_simple(int RunNumber, int file)


(b) in build/ directory
- mkRunDir.sh   (creation of eventlike run directory)
>>> bash mkRunDir.sh #Run_number 0
- cpFile.sh     (copy to create of eventlike subfile directories)
>>> bash cpFile.sh #Run_number #Init_file #Number_of_files_to_be_created

(c) in analysis/ directory
>>>root MUSICEvent.C


/#################/
USER INPUTS
(a) in project directory
- inputs.dat

(b) in build/ directory
- DAQTopology.h                      -->Digital Acquisition Map
- ShiftBoardTime.dat                 -->timeshift per board for each run in order to synchronized them
- calibCoef/run#Run_number.dat       -->beam (centroids, sigma) per left/right (segmented) strip in order to normalize them to norm value defined in inputs.dat
                                        (centroids, sigma) list determined by excuting normEsegment.C 
/#################/
Saved converted data
RootFiles/Raw/                 -->List .Root trees
RootFiles/Event/               -->Eventlike .Root trees
