#include "TSnCommIncFile.h"

ClassImp(TSnCommIncFile);

void TSnCommIncFile::PrintEvts(Option_t* option) const {
   Printf("Received %u/%u events.",fNevtsRcvd,fNevtsSent);
}

void TSnCommIncFile::PrintPwrs(Option_t* option) const {
   Printf("Received %u/%u power readings.",fNpwrsRcvd,fNpwrsSent);
}

void TSnCommIncFile::PrintMbedCRC(Option_t* option) const {
   Printf("Station CRC = [%u]",fMbedCRC);
}

void TSnCommIncFile::PrintLocalCRC(Option_t* option) const {
   Printf("Local CRC   = [%u]",fLocalCRC);
}

void TSnCommIncFile::Print(Option_t* option) const {
   TSnCommIncoming::Print(option);
   PrintMbedCRC(option);
   PrintLocalCRC(option);
   PrintEvts(option);
   PrintPwrs(option);
   Printf("From file [%s] (version [%hhu])",
          fFilename.Data(), fFvers);
}
