#
# $Id$
#
# See $SNS/build/template/module.mk.example for a list
# of all possible variables and their meaning.
# 

anaDH := \
	TSnCorrelateChans.h \
	TSnCorrelateWvs.h \
	TSnCorrelateWaves.h \
	TSnCluster.h \
	TSnCorrelateWavesMod.h \
	TSnSelEvtsOnCC.h \
	TSnPlotCCMod.h \
	TSnPlotCorrelations.h \
	TSnNumHighFFTBinsMod.h \
	TSnPlotNumHighFFTBins.h \
	TSnCountEventsMod.h \
	TSnFindPeaksPastThreshMod.h \
	TSnPlotWvfmPeaksMod.h \

anaLIBEXTRA := \
	`root-config --glibs` \
	-L$(SNS)/lib -lrecodat -lsigprocdat -lsigproc \
	-ldbdat -levdat -lutil -ltam -lgeo \

anaLIBDEP := \
	recodat sigprocdat sigproc dbdat evdat util tam geo \
