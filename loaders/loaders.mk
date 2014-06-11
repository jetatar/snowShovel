#
# $Id$
#
# See $SNS/build/template/module.mk.example for a list
# of all possible variables and their meaning.
# 

loadersDH := \
	TSnLinkedTreeBranchLoader.h \
	TSnMetadataLinkBranchLoader.h \
	TSnTimeLinkBranchLoader.h \
	TSnConfigTreeLoader.h \
	TSnVoltageTreeLoader.h \
	TSnHeartbeatTreeLoader.h \
	TSnVoltageBranchLoader.h \
	TSnHeartbeatBranchLoader.h \
	TSnAltTreeLoader.h \
	TSnAltTreeBranchLoader.h \
	TSnAltTreeRandomLoader.h \
	TSnAltTreeCyclicLoader.h \
	TSnAltTreeShuffleLoader.h \
	TSnAltTreeSingleEntryLoader.h \
	TSnLoadFromFileMod.h \
	TSnWindTreeLoader.h \
	TSnWindBranchLoader.h \

# loaders depends on sigproc for the raw tree maker branch names :(
loadersLIBEXTRA := \
	`root-config --glibs` \
	-L$(SNS)/lib -lsigproc -levdat \

loadersLIBDEP := \
	sigproc evdat \
	


