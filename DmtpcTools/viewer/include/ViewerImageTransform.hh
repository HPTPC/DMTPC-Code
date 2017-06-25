#ifndef DMTPC_SKIM_VIEWER_IMAGE_TRANSFORM_HH
#define DMTPC_SKIM_VIEWER_IMAGE_TRANSFORM_HH
#include "TGraph.h"
#include "TGFrame.h"
#include "TH2F.h"
#include "TRootEmbeddedCanvas.h"
#include "TStopwatch.h"
#include "TGNumberEntry.h"
#include "TGLabel.h"
#include "TCanvas.h"
#include "TGTextView.h"
#include "TObjArray.h"
#include <list>
#include <vector>

/** An image transform is something that can be used to transform an image that takes parameters. This class
 *  makes that somewhat automated. 
 *  
 *  
 *
 *  To write a new transform, right now the following steps are recommended:
 *
 *  Create a static function in SkimViewerImageTransform.cc of the type:
 *  
 *    static char * myTransformFn(const THF * image, const std::vector<void *> parameters); 
 *
 *    Each parameter is passed by pointer cast as void * to allow all types. You need to cast them appropriately (see examples). Calls to Draw inside 
 *    your static method will draw in the image transform canvas. The return value is printed out to the output box. 
 *    
 *  Create a case in the Factory function using a name for your function. 
 *
 *    Here, you define the parameters and instantiate the SkimViewerImageTransform class using your function pointer and the parameter vector.
 *
 *    See examples if this doesn't make sense. 
 *
 *    It is easiest to use the static methods of the Parameter class. The parameters will be presented to the user via the GUI and passed to your function. 
 *
 *    Additionally, a list of TObject*'s is passed as the last parameter. This is intended to be used for any elements you need persisted (e.g. TGraphs * ) outside
 *    of your static method. The list is cleared and the elements deleted before each update. 
 *
 *  In SkimViewerFrame, add a menu entry for your transform. 
 *
 *    To do this, you must define an integer constant for your menu entry, you must add a case in the HandleMenu method, and you must add the menu item to the PopupMenu in the 
 *    SkimViewerFrame constructor. 
 *    
 *
 *
 *  **/ 

namespace dmtpc
{
namespace viewer
{
class ViewerImageTransform : public TGMainFrame
{
  public:

    /** This is not how you do polymorphism in C++, but defining umpteen classes is probably overkill. This class is used to define parameters to be displayed in the GUI. 
     *  The types are 
     *     Double 
     *     Int
     *     Bool (checkbox)  
     *     String 
     *     File (same as String except with a file entry dialog) 
     *     Enum (choices). 
     *
     * */ 
    class Parameter
    {
      public:
        /** Probably better to use the static methods to create a new parameter rather than deal with these explicitly **/ 
        char * name; 
        enum {param_double, param_int, param_string, param_enum, param_file, param_bool} type; 
        union { double d; int i; char * str; char * start_dir; bool b;} default_value;
        union {char ** enum_choices; char * file_suffix; TGNumberFormat::EAttribute number_type;} aux;
        union {int nchoices;  int field_size;} aux2;

        static Parameter makeDoubleParameter(char * name, double default_value, TGNumberFormat::EAttribute = TGNumberFormat::kNEAAnyNumber, int field_size =5 ); 
        static Parameter makeIntParameter(char * name, int default_value, TGNumberFormat::EAttribute = TGNumberFormat::kNEAAnyNumber, int field_size = 5);
        static Parameter makeBoolParameter(char * name, bool default_value = false); 
        static Parameter makeStringParameter(char * name, char * default_value); 
        static Parameter makeFileParameter(char * name, char * start_dir, char * file_suffix = 0);
        static Parameter makeEnumParameter(char * name, int n_choices, char ** choices, int default_choice = 0); 
    };


    ViewerImageTransform(const TGWindow * p,
                                  UInt_t w, UInt_t h,
                                  const char * name,
                                  char * (*fp) (const TH2*, const std::vector<void*> *),
                                  std::vector< dmtpc::viewer::ViewerImageTransform::Parameter> parameters, 
                                  std::list< dmtpc::viewer::ViewerImageTransform*> * store = 0
                                  ); 

    ~ViewerImageTransform(); 
    void Update();   
    void SetImage(const TH2 * img) { current_image = const_cast<TH2*>(img); Update();}

    /** Create a DmtpcSkimViewerImageTransform of the given type. 
     *  store is a list storing handles to DmtpcSkimViewerImageTransforms. If nonzero, the destructor of the transform will remove the transform's
     *  pointer from that list. This is necessary to communicate back to the DmtpcSkimViewerFrame that the window was closed and should no longer
     *  be given information from new tracks. 
     * **/  
    static ViewerImageTransform * Factory(char * name, std::list< dmtpc::viewer::ViewerImageTransform*> * store = 0); 


    /** Used by the file choose widget */
    void FileChoose(int id); 
          

  private:
   char * (*_fp)(const TH2*,const std::vector<void*> *); 
   std::list< dmtpc::viewer::ViewerImageTransform*> * _store; 
   TH2 * current_image; 
   std::vector< dmtpc::viewer::ViewerImageTransform::Parameter> _params; 
   char * _name;  
   TRootEmbeddedCanvas * canvas; 
   TObjArray * inputs; 
   TGTextView * output; 
   std::list<TGraph> _edgelist; 
   TStopwatch watch; 
   ClassDef(ViewerImageTransform,0); 
};

}
}

#endif
