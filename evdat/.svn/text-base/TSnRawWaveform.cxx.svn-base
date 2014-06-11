#include "TSnRawWaveform.h"

#include <TString.h>

#include "NSnCRCUtils.h"

#include <stdexcept>

ClassImp(TSnRawWaveform);

void TSnRawWaveform::Print(Option_t* option) const {
  // use option "d" to print the waveform data
  
  CalcCRC();
  TString op(option);
  op.ToLower();
  Printf("  station CRC   = %u", fStationCRC);
  Printf("  calc    CRC   = %u", fCRC);
  if (op.Contains("d")) {
    const UShort_t* d = fData;
    for (UChar_t ch=0; ch<NSnConstants::kNchans; ch++) {
      printf("  data, ch%hhu: ",ch);
      for (UChar_t sm=0; sm<NSnConstants::kNsamps; sm++, d++) {
	printf("%hu ", *d);
      }
      printf("\n");
    }
  }
}

UShort_t TSnRawWaveform::GetIndexOf(const UChar_t ch, const UChar_t smp) {
   // get the absolute index (in fData) of the specified channel and sample
   //
   // check that the channel and sample number are valid
   // throws and error and returns 0 if not
   
   if (ch<NSnConstants::kNchans) {
      if (smp<NSnConstants::kNsamps) {
         return GetUncheckedIndex(ch,smp);
      } else {
         throw std::out_of_range(Form("<TSnRawWaveform::GetIndexOf>: "
                                   "Sample [%hhu] out of range.", smp));
      }
   } else {
      throw std::out_of_range(Form("<TSnRawWaveform::GetIndexOf>: "
                                   "Channel [%hhu] out of range.", ch));
   }
   return 0;
}

void TSnRawWaveform::CalcCRC() const {
    // CRC made using union on a little endian (mbed) processor

   fCRC = NSnCRCUtils::GetCRC32for(fData, NSnConstants::kTotSamps);
}

