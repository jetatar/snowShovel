/********************************************************************
* evdat/G__evdat.h
* CAUTION: DON'T CHANGE THIS FILE. THIS FILE IS AUTOMATICALLY GENERATED
*          FROM HEADER FILES LISTED IN G__setup_cpp_environmentXXX().
*          CHANGE THOSE HEADER FILES AND REGENERATE THIS FILE.
********************************************************************/
#ifdef __CINT__
#error evdat/G__evdat.h/C is only for compilation. Abort cint.
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define G__ANSIHEADER
#define G__DICTIONARY
#define G__PRIVATE_GVALUE
#include "G__ci.h"
#include "FastAllocString.h"
extern "C" {
extern void G__cpp_setup_tagtableG__evdat();
extern void G__cpp_setup_inheritanceG__evdat();
extern void G__cpp_setup_typetableG__evdat();
extern void G__cpp_setup_memvarG__evdat();
extern void G__cpp_setup_globalG__evdat();
extern void G__cpp_setup_memfuncG__evdat();
extern void G__cpp_setup_funcG__evdat();
extern void G__set_cpp_environmentG__evdat();
}


#include "TObject.h"
#include "TMemberInspector.h"
#include "evdat/TSnCompressor.h"
#include "evdat/TSnEvent.h"
#include "evdat/TSnEventHeader.h"
#include "evdat/TSnEventMetadata.h"
#include "evdat/TSnRawWaveform.h"
#include "evdat/TSnStatusUpdate.h"
#include "evdat/TSnTrgInfo.h"
#include "evdat/NSnCRCUtils.h"
#include "evdat/NSnConstants.h"
#include "evdat/TSnPowerReading.h"
#include "evdat/TSnHeartbeat.h"
#include "evdat/TSnClockSetInfo.h"
#include "evdat/NSnHashUtils.h"
#include "evdat/TSnWindData.h"
#include <algorithm>
namespace std { }
using namespace std;

#ifndef G__MEMFUNCBODY
#endif

extern G__linked_taginfo G__G__evdatLN_TClass;
extern G__linked_taginfo G__G__evdatLN_TBuffer;
extern G__linked_taginfo G__G__evdatLN_TMemberInspector;
extern G__linked_taginfo G__G__evdatLN_TObject;
extern G__linked_taginfo G__G__evdatLN_TString;
extern G__linked_taginfo G__G__evdatLN_vectorlEROOTcLcLTSchemaHelpercOallocatorlEROOTcLcLTSchemaHelpergRsPgR;
extern G__linked_taginfo G__G__evdatLN_reverse_iteratorlEvectorlEROOTcLcLTSchemaHelpercOallocatorlEROOTcLcLTSchemaHelpergRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__evdatLN_vectorlETVirtualArraymUcOallocatorlETVirtualArraymUgRsPgR;
extern G__linked_taginfo G__G__evdatLN_reverse_iteratorlEvectorlETVirtualArraymUcOallocatorlETVirtualArraymUgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__evdatLN_TSnCompressor;
extern G__linked_taginfo G__G__evdatLN_TTimeStamp;
extern G__linked_taginfo G__G__evdatLN_TSnTrgInfo;
extern G__linked_taginfo G__G__evdatLN_TSnTrgInfocLcLETrgBit_t;
extern G__linked_taginfo G__G__evdatLN_TSnEventHeader;
extern G__linked_taginfo G__G__evdatLN_NSnConstants;
extern G__linked_taginfo G__G__evdatLN_TSnRawWaveform;
extern G__linked_taginfo G__G__evdatLN_TSnEvent;
extern G__linked_taginfo G__G__evdatLN_TSnEventMetadata;
extern G__linked_taginfo G__G__evdatLN_TSnStatusUpdate;
extern G__linked_taginfo G__G__evdatLN_TSnStatusUpdatecLcLTObsoletePars;
extern G__linked_taginfo G__G__evdatLN_NSnCRCUtils;
extern G__linked_taginfo G__G__evdatLN_TSnPowerReading;
extern G__linked_taginfo G__G__evdatLN_TSnPowerReadingcLcLTObsoletePars;
extern G__linked_taginfo G__G__evdatLN_TSnHeartbeat;
extern G__linked_taginfo G__G__evdatLN_TSnClockSetInfo;
extern G__linked_taginfo G__G__evdatLN_NSnHashUtils;
extern G__linked_taginfo G__G__evdatLN_TSnWindData;

/* STUB derived class for protected member access */