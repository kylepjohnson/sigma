//
// File:         fs_command.hpp
// Description:  Command of the abstract machine for forward subsumption.
// Created:      Feb 15, 2000, 12:30
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#ifndef FS_COMMAND_H
//===========================================================
#define FS_COMMAND_H
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "GlobAlloc.hpp"
#include "DestructionMode.hpp"
#include "Term.hpp"
#include "BitSet.hpp" 
#ifdef DEBUG_ALLOC_OBJ_TYPE
#  include "ClassDesc.hpp"
#endif
//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_FS_COMMAND
 #define DEBUG_NAMESPACE "FS_COMMAND"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{

class Clause;
class Flatterm;
const ulong MAX_FS_CODE_DEPTH = VampireKernelConst::MaxClauseSize*2 + 1;

typedef BK::BitSet<BK::GlobAlloc> FunSet;

class FS_COMMAND
 {
  public:
   enum TAG 
    {
     LIT_HEADER,
     PROP_LIT,
     UNORD_EQ, 
     ORD_EQ, 
     SECOND_EQ_ARG, 
     FUNC, 
     VAR, 
     COMPARE,
     COMPARE_NF, // COMPARE without fork 
     SIG_FILTER,
     SUCCESS 
    }; 
  private: // structure
   union 
    {
     ulong header_num;
     ulong func; // to be casted into TERM 
     const Flatterm** var1;
     Clause* clause;
     FunSet* sig;     
    } arg1;
   union
    { 
     const Flatterm** var2;
     bool swap_flag;
     ulong numOfDiffVars; 
    } arg2;
   FS_COMMAND* fork; 
   FS_COMMAND* next; 
   TAG tag;
   #ifdef DEBUG_ALLOC_OBJ_TYPE
    static BK::ClassDesc _classDesc;  
   #endif
  public:
   FS_COMMAND(TAG t) : fork(0), next(0), tag(t) 
   {
    CALL("constructor FS_COMMAND(TAG t)");
   };
   ~FS_COMMAND() 
    {
     CALL("destructor ~FS_COMMAND()");
     if (BK::DestructionMode::isThorough()) 
       { 
	 if (IsSigFilter()) DestroySigFilter();
       };
    };
   inline void* operator new(size_t)
    {
     CALL("operator new(size_t)");
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerAllocated(sizeof(FS_COMMAND)));
      return BK::GlobAlloc::allocate(sizeof(FS_COMMAND),&_classDesc);
     #else
      return BK::GlobAlloc::allocate(sizeof(FS_COMMAND)); 
     #endif
    };
   inline void operator delete(void* obj)
    {
     CALL("operator delete(void* obj)"); 
     #ifdef DEBUG_ALLOC_OBJ_TYPE
      ALWAYS(_classDesc.registerDeallocated(sizeof(FS_COMMAND)));
      BK::GlobAlloc::deallocate(obj,sizeof(FS_COMMAND),&_classDesc);
     #else
      BK::GlobAlloc::deallocate(obj,sizeof(FS_COMMAND));
     #endif
    };
   static ulong minSize() { return sizeof(FS_COMMAND); };
   static ulong maxSize() { return sizeof(FS_COMMAND); }; 
   inline TAG Tag() const { return tag; };
   inline void SetTag(TAG t) { tag = t; };
   inline bool IsLitHeader() const { return (tag == LIT_HEADER); };
   inline bool IsPropLit() const { return (tag == PROP_LIT); };
   inline bool IsFunc() const { return (tag == FUNC); };
   inline bool IsVar() const { return (tag == VAR); };
   inline bool IsCompare() const { return (tag == COMPARE); };
   inline bool IsCompareNF() const { return (tag == COMPARE_NF); };
   inline bool IsSigFilter() const { return (tag == SIG_FILTER); };   
   inline bool IsSuccess() const { return (tag == SUCCESS); };
   bool NextLitOrSuccess() const 
    {
     return ((tag == LIT_HEADER)
             || (tag == PROP_LIT) 
             || (tag == ORD_EQ)
             || (tag == UNORD_EQ)
             || (tag == SUCCESS));
    };
   bool NextLitOrSuccessOrCompare() const
    { 
     return (NextLitOrSuccess() || tag == COMPARE || tag == COMPARE_NF );
    };
   bool VarOrFunc() const 
    {
     return ((tag == VAR) || (tag == FUNC)); 
    };

   inline bool SkeletonHeaderNum(ulong& hd_num) const
    {
     switch (tag)
      {
       case LIT_HEADER :
       case PROP_LIT :
       case UNORD_EQ :
        hd_num = HeaderNum();
        return true;
       case ORD_EQ :
	hd_num = TERM::UnordHeaderNum(HeaderNum());
        return true;
       case FUNC :
        hd_num = Func().HeaderNum();
        return true; 
       default : return false;
      }; 
    };

   inline FS_COMMAND* Next() const { return next; };
   inline void SetNext(FS_COMMAND* c) { next = c; };
   inline FS_COMMAND** NextAddr() { return &(next); };
   
   inline FS_COMMAND* Fork() const { return fork; };
   inline void SetFork(FS_COMMAND* c) { fork = c; };
   inline FS_COMMAND** ForkAddr() { return &(fork); };
   
       // Methods for LIT_HEADER, PROP_LIT, UNORD_EQ and ORD_EQ
   inline ulong HeaderNum() const { return arg1.header_num; };
   inline void SetHeader(TERM h) { arg1.header_num = h.HeaderNum(); }; 
   static inline FS_COMMAND* NewLIT_HEADER(TERM header)
    {
     FS_COMMAND* res = new FS_COMMAND(LIT_HEADER);
     res->SetHeader(header);
     return res;
    };
   static inline FS_COMMAND* NewPROP_LIT(TERM header)
    {
     FS_COMMAND* res = new FS_COMMAND(PROP_LIT);
     res->SetHeader(header);
     return res;
    }; 
   static inline FS_COMMAND* NewUNORD_EQ(TERM header)
    {
     FS_COMMAND* res = new FS_COMMAND(UNORD_EQ);
     res->SetHeader(header);
     return res;
    }; 
   static inline FS_COMMAND* NewORD_EQ(TERM header)
    {
     FS_COMMAND* res = new FS_COMMAND(ORD_EQ);
     res->SetHeader(header);
     return res;
    };
      
       // for UNORD_EQ only
   inline bool SwapFlag() const { return arg2.swap_flag; };
   inline void SwapOn() { arg2.swap_flag = true; };
   inline void SwapOff() { arg2.swap_flag = false; };    
     
       // Methods for SECOND_EQ_ARG
   static inline FS_COMMAND* NewSECOND_EQ_ARG()
    {
     return new FS_COMMAND(SECOND_EQ_ARG);
    };
        
       // Methods for FUNC: 
   inline TERM Func() const { return TERM(arg1.func); };
   inline void SetFunc(TERM f) { arg1.func = f.Content(); };
   static inline FS_COMMAND* NewFUNC(TERM f)
    {
     FS_COMMAND* res = new FS_COMMAND(FUNC);
     res->SetFunc(f);
     return res;
    };
       // Methods for VAR:
   inline const Flatterm** Var() const { return arg1.var1; };
   inline void SetVar(const Flatterm** v) { arg1.var1 = v; };  
   static inline FS_COMMAND* NewVAR(const Flatterm** v)
    {
     FS_COMMAND* res = new FS_COMMAND(VAR);
     res->SetVar(v);
     return res; 
    };  
      
       // Methods for COMPARE and COMPARE_NF:
   inline const Flatterm** Var1() const { return arg1.var1; };
   inline void SetVar1(const Flatterm** v) { arg1.var1 = v; };    
   inline const Flatterm** Var2() const { return arg2.var2; };
   inline void SetVar2(const Flatterm** v) { arg2.var2 = v; };
   inline bool SameVars(const Flatterm** v1,const Flatterm** v2)
    {
     return ((Var1() == v1) && (Var2() == v2));
    }; 
   static inline FS_COMMAND* NewCOMPARE(const Flatterm** v1,const Flatterm** v2)
    {
     CALL("NewCOMPARE(const Flatterm** v1,const Flatterm** v2)");
     ASSERT(v1 < v2);
     FS_COMMAND* res = new FS_COMMAND(COMPARE);
     res->SetVar1(v1);
     res->SetVar2(v2);
     return res;
    }; 
   static inline FS_COMMAND* NewCOMPARE_NF(const Flatterm** v1,const Flatterm** v2)
    {
     CALL("NewCOMPARE_NF(const Flatterm** v1,const Flatterm** v2)");     
     ASSERT(v1 < v2);
     FS_COMMAND* res = new FS_COMMAND(COMPARE_NF);
     res->SetVar1(v1);
     res->SetVar2(v2);
     return res;
    };
   
      // Methods for SIG_FILTER:
   inline FunSet* Functors() const { return arg1.sig; }; 
   inline void SetFunctors(FunSet* funs) { arg1.sig = funs; }; 
   inline void DestroySigFilter() { delete arg1.sig; };      
   static inline FS_COMMAND* NewSIG_FILTER(FunSet* funs) 
    {
     FS_COMMAND* res = new FS_COMMAND(SIG_FILTER);
     res->arg1.sig = funs;
     return res;
    };   

      // Methods for SUCCESS:
   inline Clause* TheClause() const { return arg1.clause; };
   inline void* IndexedObject() const { return static_cast<void*>(arg1.clause); }; 
   inline void SetClause(Clause* cl) { arg1.clause = cl; };
   inline void SetIndexedObject(void* obj) { arg1.clause = static_cast<Clause*>(obj); };
   inline ulong NumOfDiffVars() const { return arg2.numOfDiffVars; };
   inline void SetNumOfDiffVars(ulong n) { arg2.numOfDiffVars = n; };
   ulong NumOfLits() const { return (ulong)next; };
   void SetNumOfLits(ulong n) { next = reinterpret_cast<FS_COMMAND*>(n); }; 
   static inline FS_COMMAND* NewSUCCESS(Clause* cl)
    {
     FS_COMMAND* res = new FS_COMMAND(SUCCESS);
     res->SetClause(cl);
     return res;
    };  
   
     // Destruction of a tree
   static void DestroyTree(FS_COMMAND* tree); 
   static void DestroyTree(FS_COMMAND* tree,void (*destroyIndexedObj)(void*));

  public: 
   #ifndef NO_DEBUG_VIS
    static ostream& output(ostream& str,const FunSet& fs);
    ostream& output(ostream& str,const Flatterm** vars) const;
    static ostream& outputTree(ostream& str,FS_COMMAND* tree,const Flatterm** subst);
   #endif
 }; // class FS_COMMAND

}; // namespace VK
//==========================================================
#endif
