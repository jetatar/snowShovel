/********************************************************************
* ana/G__ana.h
* CAUTION: DON'T CHANGE THIS FILE. THIS FILE IS AUTOMATICALLY GENERATED
*          FROM HEADER FILES LISTED IN G__setup_cpp_environmentXXX().
*          CHANGE THOSE HEADER FILES AND REGENERATE THIS FILE.
********************************************************************/
#ifdef __CINT__
#error ana/G__ana.h/C is only for compilation. Abort cint.
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
extern void G__cpp_setup_tagtableG__ana();
extern void G__cpp_setup_inheritanceG__ana();
extern void G__cpp_setup_typetableG__ana();
extern void G__cpp_setup_memvarG__ana();
extern void G__cpp_setup_globalG__ana();
extern void G__cpp_setup_memfuncG__ana();
extern void G__cpp_setup_funcG__ana();
extern void G__set_cpp_environmentG__ana();
}


#include "TObject.h"
#include "TMemberInspector.h"
#include "ana/TSnCorrelateChans.h"
#include "ana/TSnCorrelateWvs.h"
#include "ana/TSnCorrelateWaves.h"
#include "ana/TSnCluster.h"
#include "ana/TSnCorrelateWavesMod.h"
#include "ana/TSnSelEvtsOnCC.h"
#include "ana/TSnPlotCCMod.h"
#include "ana/TSnPlotCorrelations.h"
#include "ana/TSnNumHighFFTBins.h"
#include "ana/TSnSelEvtsOnHighFFTBins.h"
#include "ana/TSnCountEventsMod.h"
#include <algorithm>
namespace std { }
using namespace std;

#ifndef G__MEMFUNCBODY
#endif

extern G__linked_taginfo G__G__anaLN_TClass;
extern G__linked_taginfo G__G__anaLN_TBuffer;
extern G__linked_taginfo G__G__anaLN_TMemberInspector;
extern G__linked_taginfo G__G__anaLN_TObject;
extern G__linked_taginfo G__G__anaLN_TNamed;
extern G__linked_taginfo G__G__anaLN_TString;
extern G__linked_taginfo G__G__anaLN_vectorlEfloatcOallocatorlEfloatgRsPgR;
extern G__linked_taginfo G__G__anaLN_vectorlEdoublecOallocatorlEdoublegRsPgR;
extern G__linked_taginfo G__G__anaLN_vectorlEROOTcLcLTSchemaHelpercOallocatorlEROOTcLcLTSchemaHelpergRsPgR;
extern G__linked_taginfo G__G__anaLN_reverse_iteratorlEvectorlEROOTcLcLTSchemaHelpercOallocatorlEROOTcLcLTSchemaHelpergRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__anaLN_TObjArray;
extern G__linked_taginfo G__G__anaLN_vectorlETVirtualArraymUcOallocatorlETVirtualArraymUgRsPgR;
extern G__linked_taginfo G__G__anaLN_reverse_iteratorlEvectorlETVirtualArraymUcOallocatorlETVirtualArraymUgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__anaLN_iteratorlEbidirectional_iterator_tagcOTObjectmUcOlongcOconstsPTObjectmUmUcOconstsPTObjectmUaNgR;
extern G__linked_taginfo G__G__anaLN_TTask;
extern G__linked_taginfo G__G__anaLN_vectorlETAMBranchInfocLcLBranchPtr_tmUcOallocatorlETAMBranchInfocLcLBranchPtr_tmUgRsPgR;
extern G__linked_taginfo G__G__anaLN_reverse_iteratorlEvectorlETAMBranchInfocLcLBranchPtr_tmUcOallocatorlETAMBranchInfocLcLBranchPtr_tmUgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__anaLN_TAModule;
extern G__linked_taginfo G__G__anaLN_pairlEdoublecOdoublegR;
extern G__linked_taginfo G__G__anaLN_maplEstringcOTObjArraymUcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOTObjArraymUgRsPgRsPgR;
extern G__linked_taginfo G__G__anaLN_vectorlETIteratormUcOallocatorlETIteratormUgRsPgR;
extern G__linked_taginfo G__G__anaLN_reverse_iteratorlEvectorlETIteratormUcOallocatorlETIteratormUgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__anaLN_TSnCalWvData;
extern G__linked_taginfo G__G__anaLN_TH2F;
extern G__linked_taginfo G__G__anaLN_TSnEventMetadata;
extern G__linked_taginfo G__G__anaLN_TSnEventHeader;
extern G__linked_taginfo G__G__anaLN_TH1F;
extern G__linked_taginfo G__G__anaLN_TSnCorrelateChans;
extern G__linked_taginfo G__G__anaLN_TSnRecoResult;
extern G__linked_taginfo G__G__anaLN_TArrayF;
extern G__linked_taginfo G__G__anaLN_TArrayI;
extern G__linked_taginfo G__G__anaLN_TSnPowerReading;
extern G__linked_taginfo G__G__anaLN_TSnWindData;
extern G__linked_taginfo G__G__anaLN_TSnCorrCoef;
extern G__linked_taginfo G__G__anaLN_TSnCorrelateWvs;
extern G__linked_taginfo G__G__anaLN_TSnClust;
extern G__linked_taginfo G__G__anaLN_TSnCorrelateWaves;
extern G__linked_taginfo G__G__anaLN_TSnClusterTimes;
extern G__linked_taginfo G__G__anaLN_vectorlEpairlEdoublecOdoublegRcOallocatorlEpairlEdoublecOdoublegRsPgRsPgR;
extern G__linked_taginfo G__G__anaLN_reverse_iteratorlEvectorlEpairlEdoublecOdoublegRcOallocatorlEpairlEdoublecOdoublegRsPgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__anaLN_TSnCluster;
extern G__linked_taginfo G__G__anaLN_TSnCorrelateWavesMod;
extern G__linked_taginfo G__G__anaLN_TSnSelEvtsOnCC;
extern G__linked_taginfo G__G__anaLN_TSnRunEvtOut;
extern G__linked_taginfo G__G__anaLN_pairlEunsignedsPintcOunsignedsPintgR;
extern G__linked_taginfo G__G__anaLN_vectorlEpairlEunsignedsPintcOunsignedsPintgRcOallocatorlEpairlEunsignedsPintcOunsignedsPintgRsPgRsPgR;
extern G__linked_taginfo G__G__anaLN_vectorlEpairlEunsignedsPintcOunsignedsPintgRcOallocatorlEpairlEunsignedsPintcOunsignedsPintgRsPgRsPgRcLcLiterator;
extern G__linked_taginfo G__G__anaLN_reverse_iteratorlEvectorlEpairlEunsignedsPintcOunsignedsPintgRcOallocatorlEpairlEunsignedsPintcOunsignedsPintgRsPgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__anaLN_TSnPlotCCMod;
extern G__linked_taginfo G__G__anaLN_TSnPlotCorrelations;
extern G__linked_taginfo G__G__anaLN_TSnCalFFTData;
extern G__linked_taginfo G__G__anaLN_TSnNumHighFFTBins;
extern G__linked_taginfo G__G__anaLN_TSnSelEvtsOnHighFFTBins;
extern G__linked_taginfo G__G__anaLN_TSnCountEventsMod;

/* STUB derived class for protected member access */
typedef pair<unsigned int,unsigned int> G__pairlEunsignedsPintcOunsignedsPintgR;
typedef vector<pair<unsigned int,unsigned int>,allocator<pair<unsigned int,unsigned int> > > G__vectorlEpairlEunsignedsPintcOunsignedsPintgRcOallocatorlEpairlEunsignedsPintcOunsignedsPintgRsPgRsPgR;