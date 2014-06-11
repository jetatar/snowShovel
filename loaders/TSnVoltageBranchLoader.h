#ifndef SNS_TSnVoltageBranchLoader
#define SNS_TSnVoltageBranchLoader

#include "TSnTimeLinkBranchLoader.h"
#include "TSnPowerReading.h"
#include "TSnEventHeader.h"

class TSnVoltageBranchLoader : public TSnTimeLinkBranchLoader {
 private:
  TSnPowerReading* fPwr; //! my power reading
  TSnEventHeader*  fHdr; //! main event header

 public:
  TSnVoltageBranchLoader() : fPwr(0), fHdr(0) {}
  TSnVoltageBranchLoader(TAMBranchInfo* binfo,
			 const Char_t* linkedTreeName,
			 const Char_t* linkedTimeBranchName,
			 const Char_t* mainTimeBranchName,
			 const TSnTimeLinkBranchLoader::EMethod_t method) :
    TSnTimeLinkBranchLoader(binfo, linkedTreeName, linkedTimeBranchName,
			    mainTimeBranchName, method),
    fPwr(0), fHdr(0) {
  }
  virtual ~TSnVoltageBranchLoader() {}
  
  inline
  virtual TTimeStamp* GetMyTime() const {
    return (fPwr!=0) ? &(fPwr->GetTimeStamp()) : 0;
  }
  inline
    virtual TTimeStamp* GetMainTime() const {
    return (fHdr!=0) ? &(fHdr->GetTime()) : 0;
  }
  
  virtual void SetPointerToMyLinkData() {
    SetPointerToLinkDataIn(fPwr, GetMyBranch());
  }
  virtual void SetPointerToMainLinkData() {
    SetPointerToLinkDataIn(fHdr, GetMainBranch());
  }
  

  ClassDef(TSnVoltageBranchLoader, 1); // a time link branch loader that knows how to get the time from a TSnPowerReading
};

#endif // SNS_TSnVoltageBranchLoader
