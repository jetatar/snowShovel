#
# $Id$
#
# See $SNS/build/template/module.mk.example for a list
# of all possible variables and their meaning.
# 

geoDH := \
	TSnGeoStnSite.h \
	TSnGeoDAQBox.h \
	TSnGeoLPDA.h \
	NSnGeoMaterials.h \

geoLIBEXTRA := \
	`root-config --glibs` \
	-L`root-config --libdir` -lGeom \

