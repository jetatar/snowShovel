//
// $Id$
//
//

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

//#pragma link C++ function myFunc(TMyArg);
//#pragma link C++ global gMGlobal;
//#pragma link C++ typedef MyTypedef_t;

#pragma link C++ class TSnPQConnection+;
#pragma link C++ class TSnPQDbKey+;
#pragma link C++ class TSnPQIntKey+;
#pragma link C++ class TSnPQShortKey+;
#pragma link C++ class TSnPQStringKey+;
#pragma link C++ class TSnPQComponentKey+;
#pragma link C++ class TSnPQCompAfarKey+;
#pragma link C++ class TSnPQCompAmpKey+;
#pragma link C++ class TSnPQCompAntennaKey+;
#pragma link C++ class TSnPQCompFpgaKey+;
#pragma link C++ class TSnPQCompDigitizerKey+;
#pragma link C++ class TSnPQCompDaughtercardKey+;
#pragma link C++ class TSnPQCompIridiumKey+;
#pragma link C++ class TSnPQCompMbedKey+;
#pragma link C++ class TSnPQCompMotherboardKey+;
#pragma link C++ class TSnPQDacSetKey+;
#pragma link C++ class TSnPQPlaSetKey+;
#pragma link C++ class TSnPQDAQConfigKey+;
#pragma link C++ class TSnPQTrigSetupKey+;
#pragma link C++ class TSnPQAmpOnSetKey+;
#pragma link C++ class TSnPQCommWinSetKey+;

#ifdef USE_FOR_AUTLOADING // add the namespace to the rootmap
#pragma link C++ class NSnPQSimpleKey;
#else
#pragma link C++ namespace NSnPQSimpleKey;
#endif

#pragma link C++ enum TSnPQConnection::EPQConnType;
#pragma link C++ enum NSnPQSimpleKey::EValType;

// define the ones that ROOT didn't bother to define
// UShort is already in dbdat, which we link against
#pragma link C++ class TParameter<Short_t>+;
#pragma link C++ class TParameter<UInt_t>+;
#pragma link C++ class TParameter<ULong_t>+;
#pragma link C++ class TParameter<ULong64_t>+;

#endif // __CINT__
