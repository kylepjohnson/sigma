//
// File:         BSMachine.hpp
// Description:  Core of backward subsumption.
// Created:      Feb 26, 2000
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
// Revised:      Dec 03, 2001.
//               1) The arrays BSMachine::{_litEqualizer,_symLitEqualizer}
//               are now represented by extendable arrays (Array<..>). 
//               2) The arrays BSMachine::LitSubst::{_matrix,_litNum}
//               are now represented by extendable arrays (Array<..>). 
// Revised:      Dec 04, 2001. 
//               The arrays SubstJoin::{_subst,_firstSubstCom,_code,_litRegistered}
//               and stack SubstJoin::_registeredLiterals have been
//               extendable.
// Revised:      Dec 05,  2001. 
//               To avoid future errors, the arrays 
//               BSMachine::{_litEqualizer,_symLitEqualizer}
//               are now represented by StaticCellArray<..> 
//               instead of Array<..>.
//============================================================================
#ifndef BS_MACHINE_H  
//=============================================================================
#define BS_MACHINE_H 
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "VampireKernelConst.hpp"
#include "GlobAlloc.hpp"
#include "GlobalStopFlag.hpp"
#include "Array.hpp"
#include "GIncCountingSort.hpp"
#include "Stack.hpp"
#include "ExpandingStack.hpp"
#include "BSIndex.hpp"
#include "Term.hpp"
#include "Clause.hpp"
#include "StaticCellArray.hpp"
//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BS_MACHINE
 #define DEBUG_NAMESPACE "BSMachine"
#endif
#include "debugMacros.hpp"
//============================================================================
namespace VK
{

class BSMachine
{
 public:
  enum Tag { PropLit, Const, Var, SymLitConst, SymLitVar};
  typedef BSIndex::LTPair LTPair;  
  typedef BSIndex::LAPair LAPair;
  typedef BSIndex::LATTriple LATTriple;
  typedef BSIndex::LList LList;
  typedef BSIndex::LTList LTList;
  typedef BSIndex::LAList LAList;
  typedef BSIndex::LATList LATList;
  typedef BSIndex::CSkipList CSkipList;
  typedef BSIndex::CLSkipList CLSkipList;
  typedef BSIndex::CLTSkipList CLTSkipList;
  typedef BSIndex::CLASkipList CLASkipList;
  typedef BSIndex::CLATSkipList CLATSkipList;
 public:
  BSMachine();
  ~BSMachine();
  void init();
  void destroy();

  bool subsumeNextSetMode(ulong& subsumedClNum);
  bool subsumeNextMultisetMode(ulong& subsumedClNum);
  void resetQuery(const Clause* cl,const BSIndex::Sizes* indexSizes);
  bool endOfQuery();
  bool queryPropLit(const CSkipList* occList);
  void queryNextLit();
  bool queryEndOfLit();
  bool queryConstant(const TERM& c,const CLSkipList* occList);
  bool queryVariable(const TERM& v,const CLTSkipList* occList,bool primary);
  void queryNextSymLit();
  bool queryEndOfSymLit();
  void querySymLitSecondArg() { _symLitEqualizer[_nextSymLit].secondArg(); };
  bool querySymLitConstant(const TERM& c,const CLASkipList* occList);
  bool querySymLitVariable(const TERM& v,const CLATSkipList* occList,bool primary);

  #ifndef NO_DEBUG
   void resetSubstitutionCollection() { _substJoin.resetResultCollection(); }; 
   bool substNextPair(ulong& var,TERM& term) { return _substJoin.resultNextPair(var,term); };
   #ifndef NO_DEBUG_VIS
    ostream& outputSubst(ostream& str) { return _substJoin.outputResult(str); };  
   #endif  
  #endif
  
  #ifndef NO_DEBUG_VIS
   ostream& outputLitEqualizers(ostream& str) const;
  #endif

 private:
  class Stream
  {
   public:
    Stream() 
    {
     CALL("constructor Stream::Stream()");
     // These assumptions must be eliminated!
     ASSERT(sizeof(CSkipList::Surfer) <= sizeof(CLSkipList::Surfer)); 
     ASSERT(sizeof(CLTSkipList::Surfer) <= sizeof(CLSkipList::Surfer));
     ASSERT(sizeof(CLASkipList::Surfer) <= sizeof(CLSkipList::Surfer));
     ASSERT(sizeof(CLATSkipList::Surfer) <= sizeof(CLSkipList::Surfer));
    };
    ~Stream() {};
    const Tag& tag() const { return _tag; };
    void reset(const CSkipList* sl) { _tag = PropLit; propLitSurfer().reset(*sl); };
    void reset(const CLSkipList* sl) { _tag = Const; constSurfer().reset(*sl); };
    void reset(const CLTSkipList* sl) { _tag = Var; varSurfer().reset(*sl); };
    void reset(const CLASkipList* sl) { _tag = SymLitConst; symLitConstSurfer().reset(*sl); };
    void reset(const CLATSkipList* sl) { _tag = SymLitVar; symLitVarSurfer().reset(*sl); };

    bool equalize(ulong& clNum,bool& notEnd);
    bool equalize(ulong clNum);

    const LList* litList() const;

    const LTList* litTermList() const;

    const LAList* litArgList() const;
    
    const LATList* litArgTermList() const;

    bool topClauseNum(ulong& clNum) const;

    #ifndef NO_DEBUG_VIS
     ostream& output(ostream& str) const;
    #endif
   private:
    CSkipList::Surfer& propLitSurfer() { return (CSkipList::Surfer&)_surfer; }; 
    CLSkipList::Surfer& constSurfer() { return _surfer; };
    CLTSkipList::Surfer& varSurfer() { return (CLTSkipList::Surfer&)_surfer; };
    CLASkipList::Surfer& symLitConstSurfer() { return (CLASkipList::Surfer&)_surfer; };
    CLATSkipList::Surfer& symLitVarSurfer() { return (CLATSkipList::Surfer&)_surfer; }; 
    const CSkipList::Surfer& propLitSurfer() const { return (CSkipList::Surfer&)_surfer; }; 
    const CLSkipList::Surfer& constSurfer() const { return _surfer; };
    const CLTSkipList::Surfer& varSurfer() const { return (CLTSkipList::Surfer&)_surfer; };
    const CLASkipList::Surfer& symLitConstSurfer() const { return (CLASkipList::Surfer&)_surfer; };
    const CLATSkipList::Surfer& symLitVarSurfer() const { return (CLATSkipList::Surfer&)_surfer; };
   private:
    Tag _tag;
    CLSkipList::Surfer _surfer;
  };   // class BSMachine::Stream

  class ClauseEqualizer
  {
   public:
    ClauseEqualizer() {};
    ~ClauseEqualizer() {};
    void init()
    {
      _sorting.init();
    };
    void destroy()
    {
      _sorting.destroy();
    };
    ulong currClauseNum() const { return _currClauseNum; };
    void reset()
    {
     _currClauseNum = ULONG_MAX;
     _firstPrim = 0;
     _nextUnsortedPrim = 0;
     _sorting.reset(); 
    };
    bool next();

    Stream* const * map() const { return _map; };
 
    void initEqualizing()
    {
     for (ulong i = 0; i < _nextUnsortedPrim; i++) initPrimary(i);
     _endOfPrim = _firstPrim + _sorting.size();
    };

    bool primary(int num,const CSkipList* occList)
    {
     PrimWrapper* wrapper = _unsortedPrim + _nextUnsortedPrim;
     wrapper->num = num;
     wrapper->tag = PropLit;
     wrapper->list = static_cast<const void*>(occList);
     _nextUnsortedPrim++;
     _sorting.push(occList->size());
     return (bool)(*occList);
    };

    bool primary(int num,const CLSkipList* occList)
    {
     PrimWrapper* wrapper = _unsortedPrim + _nextUnsortedPrim;
     wrapper->num = num;
     wrapper->tag = Const;
     wrapper->list = static_cast<const void*>(occList);
     _nextUnsortedPrim++;
     _sorting.push(occList->size());
     return (bool)(*occList);
    };
   
    bool primary(int num,const CLTSkipList* occList)
    {
     PrimWrapper* wrapper = _unsortedPrim + _nextUnsortedPrim;
     wrapper->num = num;
     wrapper->tag = Var;
     wrapper->list = static_cast<const void*>(occList);
     _nextUnsortedPrim++;
     _sorting.push(occList->size());
     return (bool)(*occList);
    };

    bool primary(int num,const CLASkipList* occList)
    {
     PrimWrapper* wrapper = _unsortedPrim + _nextUnsortedPrim;
     wrapper->num = num;
     wrapper->tag = SymLitConst;
     wrapper->list = static_cast<const void*>(occList);
     _nextUnsortedPrim++;
     _sorting.push(occList->size());
     return (bool)(*occList);
    };

    bool primary(int num,const CLATSkipList* occList)
    {
     PrimWrapper* wrapper = _unsortedPrim + _nextUnsortedPrim;
     wrapper->num = num;
     wrapper->tag = SymLitVar;
     wrapper->list = static_cast<const void*>(occList);
     _nextUnsortedPrim++;
     _sorting.push(occList->size());
     return (bool)(*occList);
    };

    bool secondary(int num,const CSkipList* occList)
    {
     _stream[_firstPrim].reset(occList);
     _map[num] = _stream + _firstPrim;
     _firstPrim++;
     return (bool)(*occList);
    };
    bool secondary(int num,const CLSkipList* occList)
    {
     _stream[_firstPrim].reset(occList);
     _map[num] = _stream + _firstPrim;
     _firstPrim++;
     return (bool)(*occList);
    };
    bool secondary(int num,const CLTSkipList* occList)
    {
     _stream[_firstPrim].reset(occList);
     _map[num] = _stream + _firstPrim;
     _firstPrim++;
     return (bool)(*occList);
    }; 
    bool secondary(int num,const CLASkipList* occList)
    {
     _stream[_firstPrim].reset(occList);
     _map[num] = _stream + _firstPrim;
     _firstPrim++;
     return (bool)(*occList);
    };
    bool secondary(int num,const CLATSkipList* occList)
    {
     _stream[_firstPrim].reset(occList);
     _map[num] = _stream + _firstPrim;
     _firstPrim++;
     return (bool)(*occList);
    };

    #ifndef NO_DEBUG_VIS
     ostream& output(ostream& str) const
     {
      str << "PRIM ";
	  ulong i;
      for (i = _firstPrim; i < _endOfPrim; i++)
       _stream[i].output(str) << " ";
      str << "  SEC ";
      for (i = 0; i < _firstPrim; i++)
       _stream[i].output(str) << " "; 
      return str;
     }; 
    #endif

   private:
    bool equalize();
    bool equalizePrimary();
    void equalizeSecondary();

#ifdef DEBUG_NAMESPACE
    bool properlyEqualized() // for debugging
    {
     ulong clNum;
     for (ulong i = 0; i < _endOfPrim; i++)   
      if (_stream[i].topClauseNum(clNum))
       {
	if (clNum != _currClauseNum) return false;
       }
      else return false;
     return true;
    };
#endif
   private:
    struct PrimWrapper 
    {
     public:
      ulong num;
      Tag tag;
      const void* list;
    };
   private:
    void initPrimary(ulong i) 
    {
     CALL("ClauseEqualizer::initPrimary(ulong i)"); 
     ASSERT(i < _nextUnsortedPrim);
     ulong pos = _firstPrim + _sorting.num()[i];
     switch (_unsortedPrim[i].tag)
      { 
       case PropLit: 
	 _stream[pos].reset(static_cast<const CSkipList*>(_unsortedPrim[i].list)); break;
       case Const: 
	 _stream[pos].reset(static_cast<const CLSkipList*>(_unsortedPrim[i].list)); break; 
       case Var: 
	 _stream[pos].reset(static_cast<const CLTSkipList*>(_unsortedPrim[i].list)); break;        
       case SymLitConst: 
	 _stream[pos].reset(static_cast<const CLASkipList*>(_unsortedPrim[i].list)); break;
       case SymLitVar: 
	 _stream[pos].reset(static_cast<const CLATSkipList*>(_unsortedPrim[i].list)); break;
      };
     _map[_unsortedPrim[i].num] = _stream + pos;
    };
   private:
    Stream _stream[VampireKernelConst::MaxClauseSize];
    Stream* _map[VampireKernelConst::MaxClauseSize];   
    ulong _currClauseNum;
    ulong _firstPrim;
    ulong _endOfPrim;
    BK::GSimpleIncCountingSort<ulong,VampireKernelConst::MaxClauseSize> _sorting;  
    PrimWrapper _unsortedPrim[VampireKernelConst::MaxClauseSize];   
    ulong _nextUnsortedPrim; 
  }; // class BSMachine::ClauseEqualizer


  class LitSubst
  {
   public:
    LitSubst() 
     : _litNum(DOP("BSMachine::LitSubst::_litNum")),
       _matrix(DOP("BSMachine::LitSubst::_matrix"))
    {
     DOP(_litNum.freeze());
     DOP(_matrix.freeze());
    };
    ~LitSubst() {}; 
    void init() 
    { 
     _litNum.init(); 
     _matrix.init(); 
     DOP(_litNum.freeze());
     DOP(_matrix.freeze());
    };
    void destroy()
    {
      _matrix.destroy();
      _litNum.destroy();
    };
    void adjustSizes(ulong maxlNumOfLitsInIndex,
                     ulong numOfVarOccs)
    {
     CALL("adjustSizes(..)");
     _currentRowSize = numOfVarOccs;
     ulong maxRowNum = (maxlNumOfLitsInIndex) 
                        ? maxlNumOfLitsInIndex - 1
                        : 0UL;
     DOP(_litNum.unfreeze());
     _litNum.expand(maxRowNum);
     DOP(_litNum.freeze());
     ulong matrixSize = maxlNumOfLitsInIndex*numOfVarOccs;
     if (matrixSize) matrixSize--;
     DOP(_matrix.unfreeze());
     _matrix.expand(matrixSize);
     DOP(_matrix.freeze());
    };
    void reset() { _nextRow = _matrix.memory(); _nextLitNum = _litNum.memory(); };
    void resetCursor() 
    { 
     CALL("LitSubst::resetCursor()"); 
     ASSERT(_nextRow > _matrix.memory()); 
     _cursor = _matrix.memory();
     _litNumCursor = _litNum.memory(); 
    };   
    void endOfRow() { _nextRow += _currentRowSize; _nextLitNum++; };         
    bool moveCursor() { _cursor += _currentRowSize; _litNumCursor++; return (_cursor != _nextRow); };     
    void set(ulong col,const TERM& t) { _nextRow[col] = t; };    
    const TERM& get(ulong col) const { return _cursor[col]; };  
    void setLitNum(ulong ln) { *_nextLitNum = ln; };
    ulong getLitNum() const { return *_litNumCursor; };

    #ifndef NO_DEBUG_VIS    
     ostream& output(ostream& str,ulong width) const 
     {
      const ulong* ln = _litNum.memory();
      for (const TERM* row = _matrix.memory(); 
           row != _nextRow; 
           row += _currentRowSize) 
       { 
        str << "row " << (row - _matrix.memory())/_currentRowSize;
        str << " lit[" << *ln << "] ";
        for (ulong col = 0; col < width; col++) 
 	str << "  v" << col << " -> " << row[col];      
        str << '\n';
        ln++;
       };
      return str;
     };
    #endif

   private:

    ulong _currentRowSize;

    BK::Array<BK::GlobAlloc,ulong,32UL,LitSubst> _litNum; 
    
    ulong* _nextLitNum;
 
    BK::Array<BK::GlobAlloc,TERM,128UL,LitSubst> _matrix;
    TERM* _nextRow;
    const TERM* _cursor;
    ulong* _litNumCursor;
  }; // class LitSubst

  class SubstJoin
  {
   public: 
    SubstJoin();
    ~SubstJoin();
    void init();
    void destroy();


    void reset(ulong numOfQueryLits, ulong maxlNumOfLitsInIndex);
    void nextLit(LitSubst* ls);
    void endOfLit();
    void variable(ulong v);

    bool joinInSetMode();

    bool joinInMultisetMode();

    #ifndef NO_DEBUG 
     void resetResultCollection() 
     {
      _nextRegistered = _registeredVars.begin();
     };
 
     bool resultNextPair(ulong& var,TERM& term)
     {
      CALL("resultNextPair(ulong& var,TERM& term)");
      if (_nextRegistered == _registeredVars.end()) return false;
      var = (*_nextRegistered)->queryVar;
      term = _subst[(*_nextRegistered)->subst]->get((*_nextRegistered)->var);
      _nextRegistered++;
      return true; 
     };
    
     #ifndef NO_DEBUG_VIS
      ostream& outputResult(ostream& str) 
      {
       resetResultCollection();
       ulong v;
       TERM t;
       while (resultNextPair(v,t)) str << "[X" << v << " -> " << t << "] ";      
       return str;
      };
     #endif
    #endif

    #ifndef NO_DEBUG_VIS
     ostream& outputCode(ostream& str) const 
     {
      for (const Command* com = _code.memory(); com < _endOfCode; com++)
       {
        if (com->nextSubst)
         {
  	  str << "SUBST (" << com->substNum << ", backtrack(";                 
         }
        else
         {       
  	  str << "COMP([" << com->var1 << "]," << com->substNum << '[' << com->var2 << "],backtrack(";        
         };
        if (com->backtrackCommand)
	 {
          ulong i = 0;
	  for (i = 0; _firstSubstCom[i] != com->backtrackCommand;i++) 
           {
	     //ASSERT(i < VampireKernelConst::MaxNumOfLiterals); 
            ASSERT(_firstSubstCom[i]);
           };        
          str << i; 
 	 }
        else str << "STOP";      
        str << ")) ";
       };
      return str;  
     };
    #endif

   private:
    struct VarOcc
    {
     bool registered;
     ulong var;
     long subst;
     #ifndef NO_DEBUG
      ulong queryVar;
     #endif
    };
    struct Command
    {
     bool nextSubst;
     ulong var1;
     LitSubst* subst; // is used for two different purposes!
     long substNum; // is used for two different purposes!
     ulong var2;
     Command* backtrackCommand;      
    };
  
   private: 
    void resetLitRegister()     
    {
     while (_registeredLiterals.nonempty()) _litRegistered[_registeredLiterals.pop()] = false;
    };
 
    void registerLit(ulong ln) { _litRegistered[ln] = true; _registeredLiterals.push(ln); };  
    void unregisterLastLit()
    {
     CALL("unregisterLastLit()");
     ASSERT(_registeredLiterals.nonempty());
     _litRegistered[_registeredLiterals.pop()] = false;
    };
   
   private:
    BK::Array<BK::GlobAlloc,LitSubst*,32UL,SubstJoin> _subst;   

    BK::Array<BK::GlobAlloc,Command*,32UL,SubstJoin> _firstSubstCom;
 
    long _nextSubst;
    ulong _litVariables;
    long _maxBacktrackLit;

    BK::Array<BK::GlobAlloc,Command,128UL,SubstJoin> _code;
    Command* _endOfCode;
    VarOcc _firstOcc[VampireKernelConst::MaxNumOfVariables];
    BK::Stack<VarOcc*,VampireKernelConst::MaxNumOfVariables> _registeredVars;
    const VarOcc* const* _nextRegistered;  
    
    BK::ExpandingStack<BK::GlobAlloc,ulong,32UL,SubstJoin> _registeredLiterals;

    BK::Array<BK::GlobAlloc,bool,32UL,SubstJoin> _litRegistered;

  }; // class SubstJoin


  class LitEqualizer
  {
   public:
    LitEqualizer() : subst() {}; 
    void init() { subst.init(); };   
    void destroy() { subst.destroy(); };
    void reset() 
    {
     _nextConst = 0;
     _nextVar = 0;
    };  
    void constant(ulong strNum) 
    {
     _const[_nextConst].streamNum = strNum;
     _nextConst++;
    };
    void variable(ulong strNum) 
    {
     _var[_nextVar].streamNum = strNum;
     _nextVar++;     
    };  

    void endOfLit(const BSIndex::Sizes* indexSizes)
    {
     CALL("endOfLit(const BSIndex::Sizes* indexSizes)");
     subst.adjustSizes(indexSizes->maxlNumOfNonproplLits(),_nextVar);
    }; // void endOfLit(const BSIndex::Sizes* indexSizes)
 
    void connect(Stream* const * map)
    {
 	 ulong i;	 
     for (i = 0; i < _nextConst; i++) _const[i].stream = map[_const[i].streamNum];
     for (i = 0; i < _nextVar; i++) _var[i].stream = map[_var[i].streamNum];     
    };
    void load()
    {
     ulong i;		 
     for (i = 0; i < _nextConst; i++) _const[i].load();
     for (i = 0; i < _nextVar; i++) _var[i].load(); 
    };   

    bool equalize();
    

    #ifndef NO_DEBUG_VIS
     ostream& output(ostream& str) const
     {
      str << "CONST\n";
	  ulong i; 
      for (i = 0; i < _nextConst; i++) _const[i].output(str << "    ") << '\n';
      str << "VAR\n";
      for (i = 0; i < _nextVar; i++) _var[i].output(str << "    ") << '\n';
      if (_nextVar) subst.output(str << "SUBSTITUTION:\n",_nextVar) << "\nEND OF SUBSTITUTION\n";
      return str;
     };
    #endif

   public:
    LitSubst subst;
   private:
    class ConstStream
    {
     public:
      ConstStream() {};
      ~ConstStream() {};
      void load() 
      {
       CALL("LitEqualizer::ConstStream::load()");
       llist = stream->litList();
       ASSERT(llist);       
      };
      ulong topLitNum() const { return llist->hd(); };
      const LList* list() const { return llist; };
      bool equalize(ulong& litNum,bool& notEnd);

      #ifndef NO_DEBUG_VIS
       ostream& output(ostream& str) const { return stream->output(str) << " {" << llist << '}'; };
      #endif
     public:
      union { Stream* stream; ulong streamNum; };
      const LList* llist;  
    }; // class ConstStream

    class VarStream
    {
     public:
      VarStream() {};
      ~VarStream() {};
      void load() 
      {
       CALL("LitEqualizer::VarStream::load()");
       ltlist = stream->litTermList(); 
       ASSERT(ltlist);       
      };
      ulong topLitNum() const { return ltlist->hd().el1; };
      const TERM& topTerm() const { return ltlist->hd().el2; }; 
      const LTList* list() const { return ltlist; };
      bool equalize(ulong& litNum,bool& notEnd);

      #ifndef NO_DEBUG_VIS
       ostream& output(ostream& str) const { return stream->output(str) << " {" << ltlist << '}'; };
      #endif     

     public:
      union { Stream* stream; ulong streamNum; };
      const LTList* ltlist;
    }; // class VarStream    

   private:
    #ifdef DEBUG_NAMESPACE
     bool constStreamsNonempty() 
     {
      for (ulong i = 0; i < _nextConst; i++) if (!(_const[i].list())) return false;
      return true;
     };
     bool constProperlyEqualized() 
     {
      for (ulong i = 0; i < _nextConst; i++) if (_const[i].topLitNum() != _currLitNum) return false;
      return true; 
     };
     bool varStreamsNonempty()
     {
      for (ulong i = 0; i < _nextVar; i++) if (!(_var[i].list())) return false;
      return true;
     }; 
     bool varProperlyEqualized() 
     {
      for (ulong i = 0; i < _nextVar; i++) if (_var[i].topLitNum() != _currLitNum) return false;
      return true; 
     };
    #endif

    void collectSubst() 
    {
     for (ulong i = 0; i < _nextVar; i++) subst.set(i,_var[i].topTerm());      
     subst.setLitNum(_currLitNum);
     subst.endOfRow();
    };

    bool equalizeFirst();
    bool equalizeNext();
    bool equalizeAll();
    
   private: 
    ConstStream _const[VampireKernelConst::MaxTermSize];
    VarStream _var[VampireKernelConst::MaxTermSize]; 
    ulong _nextConst;
    ulong _nextVar;
    ulong _currLitNum;
  }; // class LitEqualizer 

  class SymLitEqualizer
  {
   public:
    SymLitEqualizer() : subst() {}; 
    ~SymLitEqualizer() {};
    void init() { subst.init(); }; 
    void destroy() { subst.init(); };
    void reset() 
    {
     _nextConst1 = 0;
     _nextConst2 = 0;
     _nextVar1 = 0;
     _nextVar2 = 0;
     _secondArg = false;
    };
    void secondArg() { _secondArg = true; };
    void constant(ulong strNum) 
    {
     if (_secondArg) 
      {
       _const2[_nextConst2].streamNum = strNum;
       _nextConst2++;
      }
     else
      {
       _const1[_nextConst1].streamNum = strNum;
       _nextConst1++;
      };
    };
    void variable(ulong strNum) 
    {
     if (_secondArg) 
      {
       _var2[_nextVar2].streamNum = strNum;
       _nextVar2++; 
      }
     else
      {
       _var1[_nextVar1].streamNum = strNum;
       _nextVar1++; 
      };    
    };

    void endOfLit(const BSIndex::Sizes* indexSizes)
    {
     CALL("endOfLit(const BSIndex::Sizes* indexSizes)");
     subst.adjustSizes(2*indexSizes->maxlNumOfNonproplLits(),_nextVar1 + _nextVar2);   
    }; // void endOfLit(const BSIndex::Sizes* indexSizes)

    void connect(Stream* const * map)
    {
     ulong i;
     for (i = 0; i < _nextConst1; i++) _const1[i].stream = map[_const1[i].streamNum];
     for (i = 0; i < _nextConst2; i++) _const2[i].stream = map[_const2[i].streamNum];
     for (i = 0; i < _nextVar1; i++) _var1[i].stream = map[_var1[i].streamNum];     
     for (i = 0; i < _nextVar2; i++) _var2[i].stream = map[_var2[i].streamNum];  
    };
    void load()
    {
	 ulong i;
     for (i = 0; i < _nextConst1; i++) _const1[i].load();
     for (i = 0; i < _nextConst2; i++) _const2[i].load();
     for (i = 0; i < _nextVar1; i++) _var1[i].load(); 
     for (i = 0; i < _nextVar2; i++) _var2[i].load(); 
    };

    bool equalize();
   
    #ifndef NO_DEBUG_VIS
     ostream& output(ostream& str) const
     {
	  ulong i;	  
      str << "CONST1\n";
      for (i = 0; i < _nextConst1; i++) _const1[i].output(str << "    ") << '\n';
      str << "VAR1\n";
      for (i = 0; i < _nextVar1; i++) _var1[i].output(str << "    ") << '\n';
      str << "CONST2\n";
      for (i = 0; i < _nextConst2; i++) _const2[i].output(str << "    ") << '\n';
      str << "VAR2\n";
      for (i = 0; i < _nextVar2; i++) _var2[i].output(str << "    ") << '\n';
      str << "CURR.LIT. " << _currLitNum << ", ARG1 " << _currArgNum1 << ", ARG2 " << _currArgNum2 << '\n';
      if (_nextVar1 + _nextVar2) subst.output(str << "SUBSTITUTION:\n",_nextVar1 + _nextVar2) << "\nEND OF SUBSTITUTION\n";
      return str;
     };
    #endif
   public:
    LitSubst subst;

   private:
    class ConstStream
    {
     public:
      ConstStream() {};
      ~ConstStream() {};
      void load() 
      {
       CALL("SymLitEqualizer::ConstStream::load()");
       lalist = stream->litArgList();
       ASSERT(lalist);       
      };

      ulong topLitNum() const { return lalist->hd().el1; };
      ulong topArgNum() const { return lalist->hd().el2; };
      const LAList* list() const { return lalist; };
      bool equalize(ulong& litNum,ulong& argNum,bool& notEnd);

      #ifndef NO_DEBUG_VIS
       ostream& output(ostream& str) const { return stream->output(str) << " {" << lalist << '}'; };
      #endif
     public:
      union { Stream* stream; ulong streamNum; };
      const LAList* lalist;  
    }; // class ConstStream
    
    class VarStream
    {
     public:
      VarStream() {};
      ~VarStream() {};
      void load() 
      {
       CALL("SymLitEqualizer::VarStream::load()");
       latlist = stream->litArgTermList(); 
       ASSERT(latlist);       
      };

      ulong topLitNum() const { return latlist->hd().el1; };
      ulong topArgNum() const { return latlist->hd().el2; };
      const TERM& topTerm() const { return latlist->hd().el3; }; 
      const TERM& secondTerm() const { return latlist->tl()->hd().el3; }; 
      const LATList* list() const { return latlist; };
      bool equalize(ulong& litNum,ulong& argNum,bool& notEnd);
      #ifndef NO_DEBUG_VIS
       ostream& output(ostream& str) const { return stream->output(str) << " {" << latlist << '}'; };
      #endif

     public:
      union { Stream* stream; ulong streamNum; };
      const LATList* latlist;
    }; // class VarStream    

   private:
    #ifdef DEBUG_NAMESPACE
     bool constStreamsNonempty() 
     {
      ulong i;		  
      for (i = 0; i < _nextConst1; i++) if (!(_const1[i].list())) return false;
      for (i = 0; i < _nextConst2; i++) if (!(_const2[i].list())) return false; 
      return true;
     };
     bool constProperlyEqualized() 
     {
      ulong i;
      for (i = 0; i < _nextConst1; i++) 
       if ((_const1[i].topLitNum() != _currLitNum) || (_const1[i].topArgNum() != _currArgNum1))
        return false;
      for (i = 0; i < _nextConst2; i++) 
       if ((_const2[i].topLitNum() != _currLitNum) || (_const2[i].topArgNum() != _currArgNum2)) 
        return false;
      return true; 
     };
     bool varStreamsNonempty()
     {
	  ulong i;
      for (i = 0; i < _nextVar1; i++) if (!(_var1[i].list())) return false;
      for (i = 0; i < _nextVar2; i++) if (!(_var2[i].list())) return false;
      return true;
     }; 
     bool varProperlyEqualized() 
     {
	  ulong i;	  
      for (i = 0; i < _nextVar1; i++) 
       if ((_var1[i].topLitNum() != _currLitNum) || (_var1[i].topArgNum() != _currArgNum1))       
        return false;
      for (i = 0; i < _nextVar2; i++) 
       if ((_var2[i].topLitNum() != _currLitNum) || (_var2[i].topArgNum() != _currArgNum2))  
        return false;     
      return true; 
     };
    #endif  

    void collectSubst();

    bool equalizeFirst();
    bool equalizeNext();

    bool alternativeArg1();
    bool alternativeArg2();

    bool equalizeAll();

   
   private:
    ConstStream _const1[VampireKernelConst::MaxTermSize];
    ConstStream _const2[VampireKernelConst::MaxTermSize];
    VarStream _var1[VampireKernelConst::MaxTermSize]; 
    VarStream _var2[VampireKernelConst::MaxTermSize]; 
    ulong _nextConst1;
    ulong _nextConst2;
    ulong _nextVar1;   
    ulong _nextVar2;
    bool _secondArg;
    ulong _currLitNum;
    ulong _currArgNum1;
    ulong _currArgNum2;
    bool _altArg1;
    bool _altArg2;
  }; // class SymLitEqualizer

 private:
  bool equalizeLiterals()
  {
   CALL("equalizeLiterals()");
   //ASSERT(_nextLit <= VampireKernelConst::MaxNumOfLiterals);
   //ASSERT(_nextSymLit <= VampireKernelConst::MaxNumOfLiterals);
   ulong i;
   for (i = 0; i < _nextLit; i++)
    {
     if (!_litEqualizer[i].equalize()) return false;
    };

   for (i = 0; i < _nextSymLit; i++)  
    {
     if (!_symLitEqualizer[i].equalize()) return false;
    };
   return true;
  }; // bool equalizeLiterals()



  #ifndef NO_DEBUG_VIS
   static const char* spell(const Tag& tag) 
   { 
    switch (tag)
     {
      case PropLit: return "PropLit";
      case Const: return "Const";
      case Var: return "Var";
      case SymLitConst: return "SymLitConst";
      case SymLitVar: return "SymLitVar";
      default: return "UnknownTag";
     };
   };
  #endif

 private:
  const Clause* _clause;
  const BSIndex::Sizes* _currentIndexSizes;
  ClauseEqualizer _clauseEqualizer;
  ulong _nextStream;
 
  BK::StaticCellArray<BK::GlobAlloc,LitEqualizer,32UL,BSMachine> _litEqualizer;
 
  BK::StaticCellArray<BK::GlobAlloc,SymLitEqualizer,32UL,BSMachine> _symLitEqualizer;
 

  ulong _nextLit;
  ulong _nextSymLit;
  SubstJoin _substJoin;
 
 #ifndef NO_DEBUG_VIS 
  friend class Stream;
 #endif
}; // class BSMachine 

}; // namespace VK


//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BS_MACHINE
 #define DEBUG_NAMESPACE "BSMachine"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK
{
inline void BSMachine::resetQuery(const Clause* cl,const BSIndex::Sizes* indexSizes) 
{ 
 CALL("resetQuery(const Clause* cl,const BSIndex::Sizes* indexSizes)");
 _clause = cl;
 _currentIndexSizes = indexSizes; 
 _clauseEqualizer.reset();
 _nextStream = 0;
 _nextLit = 0;
 _nextSymLit = 0;
 _substJoin.reset(cl->numOfAllLiterals(),indexSizes->maxlNumOfNonproplLits());

 ulong prevLitEqualizerSz = _litEqualizer.size();
 ASSERT(cl->numOfAllLiterals() > 0UL);

 DOP(_litEqualizer.unfreeze());
 _litEqualizer.expand(cl->numOfAllLiterals() - 1);
 DOP(_litEqualizer.freeze());
 // Initialise the new elements in _litEqualizer :
 ulong i;
 for (i = prevLitEqualizerSz; i < _litEqualizer.size(); i++)
  {
   _litEqualizer[i].init();
  }; 


 ulong prevSymLitEqualizerSz = _symLitEqualizer.size();
 ASSERT(cl->numOfAllLiterals() > 0UL);
  
 DOP(_symLitEqualizer.unfreeze());
 _symLitEqualizer.expand(cl->numOfAllLiterals() - 1);
 DOP(_symLitEqualizer.freeze());

 // Initialise the new elements in _symLitEqualizer : 
 for (i = prevSymLitEqualizerSz; i < _symLitEqualizer.size(); i++)
  {
   _symLitEqualizer[i].init();
  };
 
}; // void BSMachine::resetQuery(const Clause* cl,const BSIndex::Sizes* indexSizes) 

inline bool BSMachine::endOfQuery() 
{
 CALL("endOfQuery()");
 _clauseEqualizer.initEqualizing();
 ulong i;
 for (i = 0; i < _nextLit; i++) _litEqualizer[i].connect(_clauseEqualizer.map()); 
 for (i = 0; i < _nextSymLit; i++) _symLitEqualizer[i].connect(_clauseEqualizer.map());

// _substJoin.outputCode(cout) << "\n"; DF;

 return true;
}; // bool BSMachine::endOfQuery()   

inline bool BSMachine::queryPropLit(const CSkipList* occList) 
{  
 CALL("queryPropLit(const CSkipList* occList)");
 if (!_clauseEqualizer.primary(_nextStream,occList)) return false;
 _nextStream++;
 return true;
}; //  bool BSMachine::queryPropLit(const CSkipList* occList) 

inline void BSMachine::queryNextLit() 
{ 
 CALL("queryNextLit()");
 _litEqualizer[_nextLit].reset(); 
 _substJoin.nextLit(&(_litEqualizer[_nextLit].subst)); 
}; // void BSMachine::queryNextLit()   

inline bool BSMachine::queryEndOfLit() 
{    
 CALL("queryEndOfLit()");
 _litEqualizer[_nextLit].endOfLit(_currentIndexSizes);  
 _nextLit++; 
 _substJoin.endOfLit(); 
 return true; 
}; // bool BSMachine::queryEndOfLit() 

inline bool BSMachine::queryConstant(const TERM& c,const CLSkipList* occList) 
{ 
 CALL("queryConstant(const TERM& c,const CLSkipList* occList)");
 ASSERT(c.IsConstant());
 if (!_clauseEqualizer.primary(_nextStream,occList)) return false;
 _litEqualizer[_nextLit].constant(_nextStream); 
 _nextStream++;
 return true;
}; // bool BSMachine::queryConstant(const TERM& c,const CLSkipList* occList) 

inline bool BSMachine::queryVariable(const TERM& v,const CLTSkipList* occList,bool primary)
{
 CALL("queryVariable(const TERM& v,const CLTSkipList* occList,bool primary)");
 ASSERT(v.isVariable());
 if (primary)
  {
   if (!_clauseEqualizer.primary(_nextStream,occList)) return false; 
  }  
 else if (!_clauseEqualizer.secondary(_nextStream,occList)) return false;
 _litEqualizer[_nextLit].variable(_nextStream); 
 _nextStream++;
 _substJoin.variable(v.var());
 return true;
}; // bool BSMachine::queryVariable(const TERM& v,const CLTSkipList* occList,bool primary)

inline void BSMachine::queryNextSymLit() 
{ 
 CALL("queryNextSymLit()");
 _symLitEqualizer[_nextSymLit].reset(); 
 _substJoin.nextLit(&(_symLitEqualizer[_nextSymLit].subst));  
}; // void BSMachine::queryNextSymLit() 

inline bool BSMachine::queryEndOfSymLit() 
{ 
 CALL("queryEndOfSymLit()");
 _symLitEqualizer[_nextSymLit].endOfLit(_currentIndexSizes);
 _nextSymLit++; 
 _substJoin.endOfLit(); 
 return true; 
}; // bool BSMachine::queryEndOfSymLit() 


inline bool BSMachine::querySymLitConstant(const TERM& c,const CLASkipList* occList)
{
 CALL("querySymLitConstant(const TERM& c,const CLASkipList* occList)");
 ASSERT(c.IsConstant());
 if (!_clauseEqualizer.primary(_nextStream,occList)) return false;
 _symLitEqualizer[_nextSymLit].constant(_nextStream); 
 _nextStream++;
 return true;
}; // bool BSMachine::querySymLitConstant(const TERM& c,const CLASkipList* occList)

inline bool BSMachine::querySymLitVariable(const TERM& v,const CLATSkipList* occList,bool primary)
{  
 CALL("querySymLitVariable(const TERM& v,const CLATSkipList* occList,bool primary)");
 ASSERT(v.isVariable());

 if (primary)
  {
   if (!_clauseEqualizer.primary(_nextStream,occList)) return false; 
  }  
 else if (!_clauseEqualizer.secondary(_nextStream,occList)) return false;
 _symLitEqualizer[_nextSymLit].variable(_nextStream); 
 _nextStream++;
 _substJoin.variable(v.var());
 return true;
}; // bool BSMachine::querySymLitVariable(const TERM& v,const CLATSkipList* occList,bool primary)

  
#ifndef NO_DEBUG_VIS
 inline ostream& BSMachine::outputLitEqualizers(ostream& str) const 
 {
  ulong i;
  for (i = 0; i < _nextLit; i++)
   _litEqualizer[i].output(str << "LITERAL " << i << '\n') << '\n';
  for (i = 0; i < _nextSymLit; i++)
   _symLitEqualizer[i].output(str << "SYM. LIT. " << i << '\n') << '\n';
  return str << '\n';
 }; // ostream& BSMachine::outputLitEqualizers(ostream& str) const 
#endif

}; // namespace VK

//============================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BS_MACHINE
 #define DEBUG_NAMESPACE "BSMachine::Stream"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK
{
inline bool BSMachine::Stream::equalize(ulong& clNum,bool& notEnd)
{
 CALL("Stream::equalize(ulong& clNum,bool& notEnd)");
 switch (_tag)
  {
   case PropLit: 
    if (propLitSurfer().find(clNum)) 
     { 
      ASSERT(propLitSurfer().currNode()->key() == clNum); 
      return true;
     };
    if ((notEnd = (propLitSurfer().currNode() != 0))) 
	  clNum = propLitSurfer().currNode()->key();
    return false;         

    case Const: 
     if (constSurfer().find(clNum))
      {
       ASSERT(constSurfer().currNode()->key() == clNum); 
       return true;
      };
     if ((notEnd = (constSurfer().currNode() != 0))) 
	   clNum = constSurfer().currNode()->key();
     return false; 
    
    case Var:
     if (varSurfer().find(clNum))
      {
       ASSERT(varSurfer().currNode()->key() == clNum); 
       return true;
      };
     if ((notEnd = (varSurfer().currNode() != 0))) 
	   clNum = varSurfer().currNode()->key();
     return false;
         
    case SymLitConst: 
     if (symLitConstSurfer().find(clNum))
      {
       ASSERT(symLitConstSurfer().currNode()->key() == clNum); 
       return true;
      };
     if ((notEnd = (symLitConstSurfer().currNode() != 0))) 
	   clNum = symLitConstSurfer().currNode()->key();
     return false;


    case SymLitVar:
     if (symLitVarSurfer().find(clNum))
      {
       ASSERT(symLitVarSurfer().currNode()->key() == clNum); 
       return true;
      };
     if ((notEnd = (symLitVarSurfer().currNode() != 0))) 
		clNum = symLitVarSurfer().currNode()->key();
     return false;
   };

  #ifdef DEBUG_NAMESPACE
   ICP("ICP0");
   return false;
  #else   
   #ifdef _SUPPRESS_WARNINGS_
    return false;
   #endif 
  #endif
}; // bool BSMachine::Stream::equalize(ulong& clNum,bool& notEnd)


inline bool BSMachine::Stream::equalize(ulong clNum)
{
 CALL("Stream::equalize(ulong clNum)");
 switch (_tag)
  {
   case PropLit: return propLitSurfer().find(clNum);
   case Const: return constSurfer().find(clNum);
   case Var: return varSurfer().find(clNum);
   case SymLitConst: return symLitConstSurfer().find(clNum);
   case SymLitVar: return symLitVarSurfer().find(clNum);
  };

 #ifdef DEBUG_NAMESPACE
  ICP("ICP0");
  return false;
 #else
  #ifdef _SUPPRESS_WARNINGS_
   return false;
  #endif 
 #endif
}; // bool BSMachine::Stream::equalize(ulong clNum)


inline const BSMachine::LList* BSMachine::Stream::litList() const 
{
 CALL("Stream::litList() const");
 ASSERT(_tag == Const); 
 return constSurfer().currNode()->value();
}; // const BSMachine::LList* BSMachine::Stream::litList() const 

inline const BSMachine::LTList* BSMachine::Stream::litTermList() const 
{
 CALL("Stream::litTermList() const");
 ASSERT(_tag == Var); 
 return varSurfer().currNode()->value();
}; // const BSMachine::LTList* BSMachine::Stream::litTermList() const  

inline const BSMachine::LAList* BSMachine::Stream::litArgList() const 
{
 CALL("Stream::litArgList() const");
 ASSERT(_tag == SymLitConst); 
 return symLitConstSurfer().currNode()->value();
}; // const BSMachine::LAList* BSMachine::Stream::litArgList() const
    
inline const BSMachine::LATList* BSMachine::Stream::litArgTermList() const 
{
 CALL("Stream::litArgTermList() const");
 ASSERT(_tag == SymLitVar); 
 return symLitVarSurfer().currNode()->value();
}; // const BSMachine::LATList* BSMachine::Stream::litArgTermList() const 

inline bool BSMachine::Stream::topClauseNum(ulong& clNum) const
{ 
 CALL("Stream::topClauseNum(ulong& clNum)");
 switch (_tag)
  {
   case PropLit: 
    if (propLitSurfer().currNode()) 
     { 
      clNum = propLitSurfer().currNode()->key();
      return true; 
     };
    return false;

   case Const: 
    if (constSurfer().currNode())
     {
      clNum = constSurfer().currNode()->key();  
      return true;
     };
    return false;

   case Var: 
    if (varSurfer().currNode())
     {
      clNum = varSurfer().currNode()->key(); 
      return true;
     };
    return false;

   case SymLitConst: 
    if (symLitConstSurfer().currNode())
     {
      clNum = symLitConstSurfer().currNode()->key();  
      return true;
     };
    return false;

   case SymLitVar:
    if (symLitVarSurfer().currNode())
     {
      clNum = symLitVarSurfer().currNode()->key();  
      return true;
     };
  }; 
 #ifdef DEBUG_NAMESPACE
  ICP("ICP0");
  return false;
 #else
  #ifdef _SUPPRESS_WARNINGS_
   return false;
  #endif 
 #endif
}; // bool BSMachine::Stream::topClauseNum(ulong& clNum) const
    
#ifndef NO_DEBUG_VIS
 inline ostream& BSMachine::Stream::output(ostream& str) const 
 {
  str << spell(_tag) << '{';
  ulong clNum;
  if (topClauseNum(clNum)) str << clNum << ",..";
  return str << "}";
 }; // ostream& BSMachine::Stream::output(ostream& str) const 
#endif



}; // namespace VK





//======================================================================
#undef DEBUG_NAMESPACE
//======================================================================
#endif
