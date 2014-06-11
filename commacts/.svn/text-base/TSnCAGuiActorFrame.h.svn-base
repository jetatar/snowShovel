#ifndef SNS_TSnCAGuiActorFrame
#define SNS_TSnCAGuiActorFrame

class TSnCAGuiActsList;
class TSnCAGuiActsAvailList;

#include <TGFrame.h>
class TGLabel;
class TGTextButton;

class TSnCAGuiActorFrame : public TGCompositeFrame {
 private:
   TGLabel*               fAvailLbl;
   TGLabel*               fActorLbl;
   TSnCAGuiActsAvailList* fAvailList;
   TSnCAGuiActsList*      fActorList;
   TGTextButton*          fToActorBtn;
   TGTextButton*          fFromActorBtn;
   TGTextButton*          fActnUpBtn;
   TGTextButton*          fActnDownBtn;
   TGLabel*               fActDescLbl;
   TGTextButton*          fSaveBtn;
   

   void     BuildGui(const Char_t* a);
   
 public:
   TSnCAGuiActorFrame() : 
      fAvailLbl(0), fActorLbl(0), fAvailList(0), fActorList(0),
      fToActorBtn(0), fFromActorBtn(0), fActnUpBtn(0), fActnDownBtn(0),
      fActDescLbl(0), fSaveBtn(0) {}
   TSnCAGuiActorFrame(const Char_t* actionBase,
                      const TGWindow *p = 0, 
                      const UInt_t w = 1, 
                      const UInt_t h = 1,
                      const UInt_t options = 0,
                      const Pixel_t back = GetDefaultFrameBackground()) :
      TGCompositeFrame(p, w, h, options, back),
      fAvailLbl(0), fActorLbl(0), fAvailList(0), fActorList(0),
      fToActorBtn(0), fFromActorBtn(0), fActnUpBtn(0), fActnDownBtn(0),
      fActDescLbl(0), fSaveBtn(0) {
      BuildGui(actionBase);
   }
   TSnCAGuiActorFrame(const Char_t* actionBase,
                      TGClient *c, 
                      const Window_t id,
                      const TGWindow *parent = 0) :
      TGCompositeFrame(c, id, parent) {
      BuildGui(actionBase);
   }
   virtual ~TSnCAGuiActorFrame();
   
   
   const TGLabel*               GetAvailLbl() const { return fAvailLbl; }
   const TGLabel*               GetActorLbl() const { return fActorLbl; }
   const TSnCAGuiActsAvailList* GetAvailList() const { return fAvailList; }
   const TSnCAGuiActsList*      GetActorList() const { return fActorList; }
   const TGTextButton*          GetToActorBtn() const { return fToActorBtn; }
   const TGTextButton*          GetFromActorBtn() const { return fFromActorBtn;}
   const TGTextButton*          GetActnUpBtn() const { return fActnUpBtn; }
   const TGTextButton*          GetActnDownBtn() const { return fActnDownBtn; }
   const TGLabel*               GetActDescLbl() const { return fActDescLbl; }
   const TGTextButton*          GetSaveBtn() const { return fSaveBtn; }
   
   // intentionally no setters
   
   
   ClassDef(TSnCAGuiActorFrame, 1); // a gui frame to ease the building of a comm actor
};

#endif // SNS_TSnCAGuiActorFrame
