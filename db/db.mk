#
# $Id$
#
# See $SNS/build/template/module.mk.example for a list
# of all possible variables and their meaning.
# 

dbDH := \
	TSnDatabase.h \
	NSnDatabase.h \
	TSnConnection.h \
	TSnDbKey.h \

dbLIBEXTRA := \
	`root-config --glibs` \
