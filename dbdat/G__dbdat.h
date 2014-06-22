/********************************************************************
* dbdat/G__dbdat.h
* CAUTION: DON'T CHANGE THIS FILE. THIS FILE IS AUTOMATICALLY GENERATED
*          FROM HEADER FILES LISTED IN G__setup_cpp_environmentXXX().
*          CHANGE THOSE HEADER FILES AND REGENERATE THIS FILE.
********************************************************************/
#ifdef __CINT__
#error dbdat/G__dbdat.h/C is only for compilation. Abort cint.
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
extern void G__cpp_setup_tagtableG__dbdat();
extern void G__cpp_setup_inheritanceG__dbdat();
extern void G__cpp_setup_typetableG__dbdat();
extern void G__cpp_setup_memvarG__dbdat();
extern void G__cpp_setup_globalG__dbdat();
extern void G__cpp_setup_memfuncG__dbdat();
extern void G__cpp_setup_funcG__dbdat();
extern void G__set_cpp_environmentG__dbdat();
}


#include "TObject.h"
#include "TMemberInspector.h"
#include "dbdat/TSnComponent.h"
#include "dbdat/TSnCompAfar.h"
#include "dbdat/TSnCompAmp.h"
#include "dbdat/TSnCompAntenna.h"
#include "dbdat/TSnCompFpga.h"
#include "dbdat/TSnCompDigitizer.h"
#include "dbdat/TSnCompDaughtercard.h"
#include "dbdat/TSnCompIridium.h"
#include "dbdat/TSnCompMbed.h"
#include "dbdat/TSnCompMotherboard.h"
#include "dbdat/TSnDacSet.h"
#include "dbdat/TSnPlaSet.h"
#include "dbdat/TSnAmpOnSet.h"
#include "dbdat/TSnDAQConfig.h"
#include "dbdat/TSnRunInfo.h"
#include "dbdat/TSnNetAdrSet.h"
#include "dbdat/TSnCommWinSet.h"
#include "dbdat/TSnTriggerSetup.h"
#include "dbdat/TSnSignalStrengthInfo.h"
#include "TParameter.h"
#include <algorithm>
namespace std { }
using namespace std;

#ifndef G__MEMFUNCBODY
#endif

extern G__linked_taginfo G__G__dbdatLN_TClass;
extern G__linked_taginfo G__G__dbdatLN_TBuffer;
extern G__linked_taginfo G__G__dbdatLN_TMemberInspector;
extern G__linked_taginfo G__G__dbdatLN_TObject;
extern G__linked_taginfo G__G__dbdatLN_TNamed;
extern G__linked_taginfo G__G__dbdatLN_TCollection;
extern G__linked_taginfo G__G__dbdatLN_TString;
extern G__linked_taginfo G__G__dbdatLN_vectorlEunsignedsPshortcOallocatorlEunsignedsPshortgRsPgR;
extern G__linked_taginfo G__G__dbdatLN_vectorlEboolcOallocatorlEboolgRsPgR;
extern G__linked_taginfo G__G__dbdatLN_vectorlEROOTcLcLTSchemaHelpercOallocatorlEROOTcLcLTSchemaHelpergRsPgR;
extern G__linked_taginfo G__G__dbdatLN_reverse_iteratorlEvectorlEROOTcLcLTSchemaHelpercOallocatorlEROOTcLcLTSchemaHelpergRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__dbdatLN_vectorlETVirtualArraymUcOallocatorlETVirtualArraymUgRsPgR;
extern G__linked_taginfo G__G__dbdatLN_reverse_iteratorlEvectorlETVirtualArraymUcOallocatorlETVirtualArraymUgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__dbdatLN_TTimeStamp;
extern G__linked_taginfo G__G__dbdatLN_TSnComponent;
extern G__linked_taginfo G__G__dbdatLN_TSnCompAfar;
extern G__linked_taginfo G__G__dbdatLN_TSnCompAmp;
extern G__linked_taginfo G__G__dbdatLN_TSnCompAntenna;
extern G__linked_taginfo G__G__dbdatLN_TSnCompFpga;
extern G__linked_taginfo G__G__dbdatLN_TSnCompDigitizer;
extern G__linked_taginfo G__G__dbdatLN_TRef;
extern G__linked_taginfo G__G__dbdatLN_TSnCompDaughtercard;
extern G__linked_taginfo G__G__dbdatLN_TSnCompIridium;
extern G__linked_taginfo G__G__dbdatLN_TSnCompMbed;
extern G__linked_taginfo G__G__dbdatLN_TSnCompMotherboard;
extern G__linked_taginfo G__G__dbdatLN_TSnDacSet;
extern G__linked_taginfo G__G__dbdatLN_vectorlEvectorlEunsignedsPshortcOallocatorlEunsignedsPshortgRsPgRcOallocatorlEvectorlEunsignedsPshortcOallocatorlEunsignedsPshortgRsPgRsPgRsPgR;
extern G__linked_taginfo G__G__dbdatLN_vectorlEvectorlEunsignedsPshortcOallocatorlEunsignedsPshortgRsPgRcOallocatorlEvectorlEunsignedsPshortcOallocatorlEunsignedsPshortgRsPgRsPgRsPgRcLcLiterator;
extern G__linked_taginfo G__G__dbdatLN_reverse_iteratorlEvectorlEvectorlEunsignedsPshortcOallocatorlEunsignedsPshortgRsPgRcOallocatorlEvectorlEunsignedsPshortcOallocatorlEunsignedsPshortgRsPgRsPgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__dbdatLN_vectorlETStringcOallocatorlETStringgRsPgR;
extern G__linked_taginfo G__G__dbdatLN_vectorlETStringcOallocatorlETStringgRsPgRcLcLiterator;
extern G__linked_taginfo G__G__dbdatLN_reverse_iteratorlEvectorlETStringcOallocatorlETStringgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__dbdatLN_TSnPlaSet;
extern G__linked_taginfo G__G__dbdatLN_TSnAmpOnSet;
extern G__linked_taginfo G__G__dbdatLN_TSnCommWinSet;
extern G__linked_taginfo G__G__dbdatLN_TSnCommWinSetcLcLESendDataBit;
extern G__linked_taginfo G__G__dbdatLN_TMap;
extern G__linked_taginfo G__G__dbdatLN_TSnTriggerSetup;
extern G__linked_taginfo G__G__dbdatLN_TSnDAQConfig;
extern G__linked_taginfo G__G__dbdatLN_TSnDAQConfigcLcLEDatPackBit;
extern G__linked_taginfo G__G__dbdatLN_TSnDAQConfigcLcLEPowerModeBit;
extern G__linked_taginfo G__G__dbdatLN_TSnDAQConfigcLcLERunModeBit;
extern G__linked_taginfo G__G__dbdatLN_TSnDAQConfigcLcLTObsoletePars;
extern G__linked_taginfo G__G__dbdatLN_TSnRunInfo;
extern G__linked_taginfo G__G__dbdatLN_TSnRunInfocLcLTObsoletePars;
extern G__linked_taginfo G__G__dbdatLN_TSnNetAdrSet;
extern G__linked_taginfo G__G__dbdatLN_TSnSignalStrengthInfo;
extern G__linked_taginfo G__G__dbdatLN_iteratorlEbidirectional_iterator_tagcOTObjectmUcOlongcOconstsPTObjectmUmUcOconstsPTObjectmUaNgR;
extern G__linked_taginfo G__G__dbdatLN_maplEstringcOTObjArraymUcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOTObjArraymUgRsPgRsPgR;
extern G__linked_taginfo G__G__dbdatLN_TParameterlEboolgR;
extern G__linked_taginfo G__G__dbdatLN_TParameterlEunsignedsPshortgR;
extern G__linked_taginfo G__G__dbdatLN_TParameterlEunsignedsPshortgRcLcLEStatusBits;

/* STUB derived class for protected member access */
typedef vector<vector<unsigned short,allocator<unsigned short> >,allocator<vector<unsigned short,allocator<unsigned short> > > > G__vectorlEvectorlEunsignedsPshortcOallocatorlEunsignedsPshortgRsPgRcOallocatorlEvectorlEunsignedsPshortcOallocatorlEunsignedsPshortgRsPgRsPgRsPgR;
typedef vector<TString,allocator<TString> > G__vectorlETStringcOallocatorlETStringgRsPgR;
typedef TParameter<unsigned short> G__TParameterlEunsignedsPshortgR;