#ifndef OR_MACHINE_H 
//==========================================================================
#define OR_MACHINE_H
#include "jargon.hpp"
#include "iu_command.hpp"
#include "prefix.hpp"
#include "variables.hpp"
#include "or_index.hpp"
#include "iu_machine.hpp"
//==========================================================================

namespace VK
{

class OR_MACH : public IU_MACH
 {
  public:
   OR_MACH() {};
   OR_MACH(OR_INDEX* resIndex) : _ordResIndex(resIndex) {};
   ~OR_MACH() {};
   void init() 
   {
     CALL("init()");
     IU_MACH::init();
   };
   void init(OR_INDEX* resIndex) { IU_MACH::init(); _ordResIndex = resIndex; };
   void destroy()
   {
     CALL("destroy()");
     IU_MACH::destroy();
   };
   void assignResIndex(OR_INDEX* resIndex) { _ordResIndex = resIndex; };
   bool FindFirstLeaf()
    {
     return IU_MACH::FindFirstLeaf(*(_ordResIndex->Tree(Code()->Symbol().Func())));
    }; 
  private:
   OR_INDEX* _ordResIndex;
 }; // class OR_MACH : public IU_MACH

}; // namespace VK

//==========================================================================
#endif

