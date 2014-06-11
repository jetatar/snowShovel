# $Id:$
#

ReadMe for the TAM Example Module
---------------------------------

Event ROOT Tutorial
-------------------

This example is based on files created by the ROOT Event tutorial 
(http://root.cern.ch/root/html/examples/MainEvent.cxx.html). The 
tutorial creates root files with a tree "T" that is made of a 
single super-branch "event". Our example TAM module takes these trees 
and reads the data in them to create pT, Bx, and Ntrack histograms.
The example macro runs the example module on a single tree or -if
two trees are given on its input- loads a mixer plugin to mix 
the two trees and process them in the example module.  


Creating the input Event.root file(s) 
-------------------------------------

For our example, the ROOT trees of the ROOT tutorial are created 
automatically using the included rootlogon.C. It will create
libEvent.so, as well as two files (Event1.root and Event2.root)
with 400 events each in the current directory.

Note:
The files can also be created by running the command "make Event" 
in the $ROOTSYS/test directory where the required files are located.
This command links the C++ program and creates a script to make
the desired ROOT files. The ROOT files are then created using 
the command 
"./Event [Number of Events] [Compression Level] [Split Level]".
This command will create a file Event.root with the "T" tree
and "event" branch. 


Running the Example Module in ROOT
------------------------------------

The example module can be run on root by executing the macro
runExampleMod.C. Executing the macro in ROOT is done 
by using the commands in the root session:

- for a single input file 
  .x runExampleMod.C+("Event1.root")

- when two input files are to be mixed
  .x runExampleMod.C+("Event1.root","Event2.root") 


Using TBrowser to Browse Module Output 
---------------------------------------

By using the TBrowser command "new TBrowser" browsing the output 
of the module can be done using a GUI. The TAM documentation provides
a sample screenshot of the output. (This will work when modules are not 
deleted in the macro.)


Summmary of Included Files
------------------------------

runExampleMod.C            - Macro that can executed to run the example module on a single
                             file or two input files.
ExampleMod.[h/cxx]         - Example module header and class implementation files. The class 
                             inherits from TAModule and create/fills histograms.

Files needed for the tree mixing:

TreeMixer.[h/cxx]          - Header and class implementation files. Does actual mixing of trees and returns
                             the mixed data. It contains an internal TreeDataLoader class that loads actual 
                             data from tree and returns it to the TreeMixer.
MixerBranchLoader.[h/cxx]  - Header and class implementation files. Loads mixed Branch.
MixerLoader.[h/cxx]        - Header and class implementation files.Creates Mixer Branch Loader  
