#
# $Id$
#
# See $SNS/build/template/module.mk.example for a list
# of all possible variables and their meaning.
# 

sigprocdatDH := \
	TSnCalibObject.h \
	TSnCalibSet.h \
	TSnPerSampCalibSet.h \
	TSnPerCardCalibSet.h \
	TSnFPNCalibVal.h \
	TSnFPNCalibSet.h \
	TSnGainCalibVal.h \
	TSnDualGainCalib.h \
	TSnDualGainCalibSet.h \
	TSnCalWvData.h \
	TSnInterp1DWvData.h \
	TSnInterp1DSpline3.h \
	TSnStopPosnSet.h \
	TSnCalFFTData.h \

sigprocdatLIBEXTRA := \
	`root-config --glibs` \
	-L$(SNS)/lib -levdat -lutil \

sigprocdatLIBDEP := \
	evdat util \
