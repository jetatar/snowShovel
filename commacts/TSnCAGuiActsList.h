#ifndef SNS_TSnCAGuiActsList
#define SNS_TSnCAGuiActsList

#include <TGListBox.h>
class TObjArray;

class TSnCAGuiActsList : public TGListBox {
 private:
   TObjArray*      fActList; //  list of actions (owns actions)

 public:
   TSnCAGuiActsList() : fActList(0) {}
   TSnCAGuiActsList(const TGWindow* p,
                    const Int_t id = -1, 
                    const UInt_t options = kSunkenFrame|kDoubleBorder,
                    const Pixel_t back = GetWhitePixel());
   virtual ~TSnCAGuiActsList();
   
   virtual void  AddAction(const Char_t* a);
   virtual void  RemoveSelected();
      
   ClassDef(TSnCAGuiActsList, 1); // a gui list box of comm actions
};

#endif // SNS_TSnCAGuiActsList
