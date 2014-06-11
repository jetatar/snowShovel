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

#pragma link C++ class TSnComponent+;
#pragma link C++ class TSnCompAfar+;
#pragma link C++ class TSnCompAmp+;
#pragma link C++ class TSnCompAntenna+;
#pragma link C++ class TSnCompFpga+;
#pragma link C++ class TSnCompDigitizer+;
#pragma link C++ class TSnCompDaughtercard+;
#pragma link C++ class TSnCompIridium+;
#pragma link C++ class TSnCompMbed+;
#pragma link C++ class TSnCompMotherboard+;
#pragma link C++ class TSnDacSet+;
#pragma link C++ class TSnPlaSet+;
#pragma link C++ class TSnAmpOnSet+;
#pragma link C++ class TSnDAQConfig+;
#pragma link C++ class TSnDAQConfig::TObsoletePars+;
#pragma link C++ class TSnRunInfo+;
#pragma link C++ class TSnRunInfo::TObsoletePars+;
#pragma link C++ class TSnNetAdrSet+;
#pragma link C++ class TSnCommWinSet+;
#pragma link C++ class TSnTriggerSetup+;
#pragma link C++ class TSnSignalStrengthInfo+;

#pragma link C++ enum  TSnDAQConfig::EDatPackBit;
#pragma link C++ enum  TSnDAQConfig::ERunModeBit;
#pragma link C++ enum  TSnDAQConfig::EPowerModeBit;
#pragma link C++ enum  TSnCommWinSet::ESendDataBit;

// define the one we need, that ROOT didn't define
#pragma link C++ class TParameter<UShort_t>+;

// help CINT with templates
//#pragma link C++ class   std::vector< Bool_t >;   - already def somewhere
//#pragma link C++ class   std::vector< UShort_t >; - already def somewhere
#pragma link C++ class   std::vector< TString >;
#pragma link C++ class   std::vector< std::vector< UShort_t > >;

#pragma link C++ typedef TSnDacSet::ChDacs_t;
#pragma link C++ typedef TSnDacSet::DacSet_t;
#pragma link C++ typedef TSnDacSet::DacTypes_t;
#pragma link C++ typedef TSnPlaSet::PlaSet_t;
#pragma link C++ typedef TSnAmpOnSet::AmpOnSet_t;

#endif // __CINT__
