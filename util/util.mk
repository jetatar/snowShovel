#
# $Id$
#
# See $ARK/build/template/module.mk.example for a list
# of all possible variables and their meaning.
# 

utilDH := \
	TSnAxisDetails.h \
	TSnPlotBrowser.h \
	TSnMath.h \
	TSnSpectral.h \

utilLIBEXTRA := \
	`root-config --glibs` \

# needed to provide these template specializations to Cint
# so, e.g. TSnSpectral::Spectrum, can be used from the interpreter
utilDHEXTRA := \
	TBranch.h \
	TH1.h \
