//
// File:         BSMachine.cpp
// Description:  Core of backward subsumption.
// Created:      Feb 26, 2000, 20:20
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#include "BSMachine.hpp"
//============================================================================
#undef DEBUG_NAMESPACE 
#ifdef DEBUG_BS_MACHINE
 #define DEBUG_NAMESPACE "BSMachine"
#endif
#include "debugMacros.hpp"
//============================================================================

using namespace BK;
using namespace Gem;
using namespace VK;

BSMachine::BSMachine() 
  : _clauseEqualizer(),
    _litEqualizer(DOP("BSMachine::_litEqualizer")),
    _symLitEqualizer(DOP("BSMachine::_symLitEqualizer")), 
    _substJoin() 
{
  DOP(_litEqualizer.freeze());
  DOP(_symLitEqualizer.freeze());
};

BSMachine::~BSMachine() { CALL("destructor ~BSMachine()"); };

void BSMachine::init()
{
  CALL("init()");
  _clauseEqualizer.init();
  _litEqualizer.init(DOP("BSMachine::_litEqualizer"));
  _symLitEqualizer.init(DOP("BSMachine::_symLitEqualizer"));
  _substJoin.init(); 
  DOP(_litEqualizer.freeze());
  DOP(_symLitEqualizer.freeze());
};
  
void BSMachine::destroy()
{
  CALL("destroy()");
  _substJoin.destroy();
  _symLitEqualizer.destroy();
  _litEqualizer.destroy();
  _clauseEqualizer.destroy();
  BK_CORRUPT(*this);
};


bool BSMachine::subsumeNextSetMode(ulong& subsumedClNum)
{
 CALL("subsumeNextSetMode(ulong& subsumedClNum)");
 while (_clauseEqualizer.next())
  {
   ulong i;	  
   for (i = 0; i < _nextLit; i++) _litEqualizer[i].load(); 
   for (i = 0; i < _nextSymLit; i++) _symLitEqualizer[i].load();
   if (equalizeLiterals())
    {
     if (_substJoin.joinInSetMode()) { subsumedClNum = _clauseEqualizer.currClauseNum(); return true; };
    };
  };
 return false;         
}; // bool BSMachine::subsumeNextSetMode(ulong& subsumedClNum)

bool BSMachine::subsumeNextMultisetMode(ulong& subsumedClNum)
{
 CALL("subsumeNextMultisetMode(ulong& subsumedClNum)");
 while (_clauseEqualizer.next())
  {
   ulong i;	  
   for (i = 0; i < _nextLit; i++) _litEqualizer[i].load(); 
   for (i = 0; i < _nextSymLit; i++) _symLitEqualizer[i].load();
   if (equalizeLiterals())
    {
     if (_substJoin.joinInMultisetMode()) { subsumedClNum = _clauseEqualizer.currClauseNum(); return true; };
    };
  };
 return false;         
}; // bool BSMachine::subsumeNextMultisetMode(ulong& subsumedClNum)


//============================================================================
#undef DEBUG_NAMESPACE 
#ifdef DEBUG_BS_MACHINE
 #define DEBUG_NAMESPACE "BSMachine::ClauseEqualizer"
#endif
#include "debugMacros.hpp"
//============================================================================


bool BSMachine::ClauseEqualizer::next() 
{ 
  CALL("next()");
  _currClauseNum--; 
  return equalize();
};


inline
bool BSMachine::ClauseEqualizer::equalize() 
{ 
  CALL("ClauseEqualizer::equalize()");
  if (equalizePrimary()) 
    { 
      equalizeSecondary(); 
      ASSERT(properlyEqualized());
      return true; 
    };
  return false;
};    

inline
bool BSMachine::ClauseEqualizer::equalizePrimary()
{ 
  CALL("ClauseEqualizer::equalizePrimary()");
  ASSERT(_firstPrim < _endOfPrim); // at least one primary position must exist
   
  ulong str = _firstPrim;
  ulong secondPrim = _firstPrim + 1;
  bool notEmpty;
  do
    {
      if (_stream[str].equalize(_currClauseNum,notEmpty)) { str++; }
      else
	{

	  
	  //DF; GlobalStopFlag::raise();

	  GlobalStopFlag::check();
	  if (notEmpty)
	    {
	      _stream[_firstPrim].equalize(_currClauseNum,notEmpty);
	      if (notEmpty) { str = secondPrim; }
	      else return false;   
	    }
	  else return false;
	};
    }
  while (str != _endOfPrim);
  return true;
};

inline
void BSMachine::ClauseEqualizer::equalizeSecondary()
{
  CALL("ClauseEqualizer::equalizeSecondary()");
  for (ulong i = 0; i != _firstPrim; i++) ALWAYS(_stream[i].equalize(_currClauseNum));
};

//============================================================================
#undef DEBUG_NAMESPACE 
#ifdef DEBUG_BS_MACHINE
#define DEBUG_NAMESPACE "BSMachine::SubstJoin"
#endif
#include "debugMacros.hpp"
//============================================================================



BSMachine::SubstJoin::SubstJoin() 
  : _subst(DOP("BSMachine::SubstJoin::_subst")), 
  _firstSubstCom(DOP("BSMachine::SubstJoin::_firstSubstCom")),
  _code(DOP("BSMachine::SubstJoin::_code")),
  _registeredVars(), 
  _registeredLiterals(DOP("BSMachine::SubstJoin::_registeredLiterals")),
#ifdef NO_DEBUG 
  _litRegistered(false,(int)0)
#else
  _litRegistered(false,"BSMachine::SubstJoin::_litRegistered")  
#endif     
{
  for (ulong v = 0; v < VampireKernelConst::MaxNumOfVariables; v++) _firstOcc[v].registered = false;
  DOP(_subst.freeze());
  DOP(_firstSubstCom.freeze());
  DOP(_code.freeze());
  DOP(_registeredLiterals.freeze());
  DOP(_litRegistered.freeze());
}; // BSMachine::SubstJoin::SubstJoin() 

BSMachine::SubstJoin::~SubstJoin() {};

void BSMachine::SubstJoin::init()
{
  CALL("init()");
  _subst.init(DOP("BSMachine::SubstJoin::_subst")); 
  _firstSubstCom.init(DOP("BSMachine::SubstJoin::_firstSubstCom"));
  _code.init(DOP("BSMachine::SubstJoin::_code"));
  _registeredVars.init(); 
  _registeredLiterals.init(DOP("BSMachine::SubstJoin::_registeredLiterals"));
#ifdef NO_DEBUG 
  _litRegistered.init(false,(int)0);
#else
  _litRegistered.init(false,"BSMachine::SubstJoin::_litRegistered"); 
#endif     

  for (ulong v = 0; v < VampireKernelConst::MaxNumOfVariables; v++) _firstOcc[v].registered = false;
  DOP(_subst.freeze());
  DOP(_firstSubstCom.freeze());
  DOP(_code.freeze());
  DOP(_registeredLiterals.freeze());
  DOP(_litRegistered.freeze());

}; // void BSMachine::SubstJoin::init()

void BSMachine::SubstJoin::destroy()
{
  CALL("destroy()");
  _litRegistered.destroy();
  _registeredLiterals.destroy();
  _registeredVars.destroy();
  _code.destroy();
  _firstSubstCom.destroy();
  _subst.destroy();
}; // void BSMachine::SubstJoin::destroy()



void BSMachine::SubstJoin::reset(ulong numOfQueryLits, ulong maxlNumOfLitsInIndex)
{
  CALL("reset(ulong numOfQueryLits, ulong maxlNumOfLitsInIndex)");

  DOP(_subst.unfreeze());
  _subst.expand(numOfQueryLits); // numOfQueryLits - 1 must be enough
  DOP(_subst.freeze());


  DOP(_firstSubstCom.unfreeze());
  _firstSubstCom.expand(numOfQueryLits); // numOfQueryLits - 1 must be enough
  DOP(_firstSubstCom.freeze());

  DOP(_code.unfreeze());
  _code.expand(VampireKernelConst::MaxClauseSize + numOfQueryLits); 
  // ^ VampireKernelConst::MaxClauseSize + numOfQueryLits - 1 must be enough
  DOP(_code.freeze());

  DOP(_registeredLiterals.unfreeze());
  _registeredLiterals.expand(maxlNumOfLitsInIndex);
  DOP(_registeredLiterals.freeze());

  DOP(_litRegistered.unfreeze());
  _litRegistered.expand(maxlNumOfLitsInIndex);
  DOP(_litRegistered.freeze());

  _nextSubst = 0;
  _endOfCode = _code.memory();
  while (_registeredVars) 
    {
#ifdef DEBUG_NAMESPACE
      VarOcc* debugVarOcc = _registeredVars.pop();
      ASSERT(MYPTR(debugVarOcc));
      debugVarOcc->registered = false; 
#else
      _registeredVars.pop()->registered = false;
#endif
    };
}; // void BSMachine::SubstJoin::reset(ulong numOfQueryLits, ulong maxlNumOfLitsInIndex)
   
void BSMachine::SubstJoin::nextLit(LitSubst* ls) 
{ 
  CALL("nextLit(LitSubst* ls)");
  _subst[_nextSubst] = ls;
  _endOfCode->nextSubst = true;
  _endOfCode->subst = ls;
  _endOfCode->substNum = _nextSubst; 
  _endOfCode->backtrackCommand = 0;
  _firstSubstCom[_nextSubst] = _endOfCode;
  _endOfCode++;
  _litVariables = 0;
  _maxBacktrackLit = -1;
};

void BSMachine::SubstJoin::endOfLit() 
{ 
  if (_litVariables) 
    { 
      ASSERT(_nextSubst != _maxBacktrackLit);
      if (_maxBacktrackLit != -1) 
        {
	  ASSERT(_maxBacktrackLit >= 0);
	  _firstSubstCom[_nextSubst]->backtrackCommand = _firstSubstCom[_maxBacktrackLit];
	}
      else { ASSERT(!_firstSubstCom[_nextSubst]->backtrackCommand); };
      _nextSubst++; 
    } 
  else _endOfCode--; 
};

void BSMachine::SubstJoin::variable(ulong v)
{
  CALL("variable(ulong v)");
  ASSERT(v < VampireKernelConst::MaxNumOfVariables);
  if (_firstOcc[v].registered)     
    {
      _endOfCode->nextSubst = false;
      _endOfCode->var1 = _litVariables;
      _endOfCode->subst = _subst[_firstOcc[v].subst];
      _endOfCode->substNum = _firstOcc[v].subst;
      _endOfCode->var2 = _firstOcc[v].var;
      _endOfCode->backtrackCommand = _firstSubstCom[_nextSubst];
      _endOfCode++;
      if ((_firstOcc[v].subst != _nextSubst) && (_maxBacktrackLit < _firstOcc[v].subst)) 
        _maxBacktrackLit = _firstOcc[v].subst; 
    }
  else // first occ. of the variable in the query
    {
      _firstOcc[v].registered = true;
      _firstOcc[v].var = _litVariables;
      _firstOcc[v].subst = _nextSubst;
#ifdef DEBUG_NAMESPACE
      _firstOcc[v].queryVar = v;
#endif
      _registeredVars.push(_firstOcc + v);
    };
  _litVariables++;
};         

bool BSMachine::SubstJoin::joinInSetMode()    
{
  CALL("joinInSetMode()");
  const Command* com = _code.memory();

  LitSubst* currSubst;
#ifdef DEBUG_NAMESPACE
  currSubst = 0; // to suppress warnings
#else
#ifdef _SUPPRESS_WARNINGS_
  currSubst = 0; // to suppress warnings
#endif 
#endif

 perform_command:
  if (com == _endOfCode) return true;
 perform_command_wo_check:
  if (com->nextSubst) 
    {
      currSubst = com->subst; 
      currSubst->resetCursor();        
      com++;
      goto perform_command;
    } 
  else // comparison
    {
    perform_comparison:
      if (currSubst->get(com->var1) == com->subst->get(com->var2))
	{
	  com++;
	  goto perform_command;          
	};
      // comparison of terms failed, backtrack needed 
      com = com->backtrackCommand;         
      ASSERT(com && com->nextSubst && (com->subst == currSubst));
      if (currSubst->moveCursor()) // next row in the current substitution table         
	{
	  com++;
	  ASSERT((com < _endOfCode) && (!com->nextSubst)); 
	  goto perform_comparison;
	}
      else // end of the current substitution table, backtrack to another substitution table
	{
	long_backtrack:
	  com = com->backtrackCommand;
	  if (!com) return false;          
	  ASSERT(com->nextSubst && (com->subst != currSubst));           
	  currSubst = com->subst;
	  if (currSubst->moveCursor()) // next row in the current substitution table         
	    {
	      com++; 
	      ASSERT(com < _endOfCode);
	      goto perform_command_wo_check; 
	    };
	  // backtrack again
	  goto long_backtrack;                                  
	};
    };
}; // bool BSMachine::SubstJoin::joinInSetMode()  


bool BSMachine::SubstJoin::joinInMultisetMode()    
{
  CALL("joinInMultisetMode()");
  const Command* com = _code.memory();

  LitSubst* currSubst;
#ifdef DEBUG_NAMESPACE
  currSubst = 0; // to suppress warnings
#else
#ifdef _SUPPRESS_WARNINGS_
  currSubst = 0; // to suppress warnings
#endif 
#endif

  resetLitRegister();
  ulong _litNum;
    
 perform_command:
  if (com == _endOfCode) return true;
 perform_command_wo_check:
  if (com->nextSubst) 
    {
      currSubst = com->subst; 
      currSubst->resetCursor();  

    check_lit_num:
      _litNum = currSubst->getLitNum();
          
      if (_litRegistered[_litNum]) 
	{ 
	  if (currSubst->moveCursor()) goto check_lit_num;   
	  goto long_backtrack;
	};

      registerLit(_litNum);

      com++;

      goto perform_command;
    } 
  else // comparison
    {
    perform_comparison:
      if (currSubst->get(com->var1) == com->subst->get(com->var2))
	{
	  com++;
	  goto perform_command;          
	};
      // comparison of terms failed, backtrack needed 
      com = com->backtrackCommand;         
      ASSERT(com && com->nextSubst && (com->subst == currSubst));

      unregisterLastLit();

    try_next_row:
      if (currSubst->moveCursor()) // next row in the current substitution table         
	{
            
	  _litNum = currSubst->getLitNum();

	  if (_litRegistered[_litNum]) 
	    {
	      goto try_next_row;
	    };
	  registerLit(_litNum);

	  com++;
	  ASSERT((com < _endOfCode) && (!com->nextSubst)); 
	  goto perform_comparison;
	}
      else // end of the current substitution table, backtrack to another substitution table
	{

	long_backtrack:                  
	  com = com->backtrackCommand;
	  if (!com) return false;      


	  ASSERT(com->nextSubst && (com->subst != currSubst));  

	  unregisterLastLit();

	  currSubst = com->subst;
                 
	try_another_row:
	  if (currSubst->moveCursor()) // next row in the current substitution table         
	    {
	      _litNum = currSubst->getLitNum();
                  
	      if (_litRegistered[_litNum]) 
		{
		  goto try_another_row;
		};
	      registerLit(_litNum);
	      
	      com++; 
	      ASSERT(com < _endOfCode);
	      goto perform_command_wo_check; 
	    };          

	  // backtrack again    
	  goto long_backtrack;                                  
	};
    };
}; // bool BSMachine::SubstJoin::joinInMultisetMode()  


//============================================================================
#undef DEBUG_NAMESPACE 
#ifdef DEBUG_BS_MACHINE
#define DEBUG_NAMESPACE "BSMachine::LitEqualizer"
#endif
#include "debugMacros.hpp"
//============================================================================

inline
bool BSMachine::LitEqualizer::equalize() 
{
  CALL("equalize()");

  if (equalizeFirst())
    {
      subst.reset();
      do
	{
	  collectSubst();
	}
      while (equalizeNext()); 
      return true; 
    };
  return false;
}; // bool BSMachine::LitEqualizer::equalize() 
    

inline
bool BSMachine::LitEqualizer::equalizeFirst()
{
  CALL("equalizeFirst()");
  ASSERT(_nextConst || _nextVar); 
  _currLitNum = (_nextConst) ? (_const->topLitNum()) : (_var->topLitNum());
  return equalizeAll();
};

inline
bool BSMachine::LitEqualizer::equalizeNext()
{
  CALL("equalizeNext()");
  if (!_currLitNum) return false;
  _currLitNum--;
  return equalizeAll();
};  

inline
bool BSMachine::LitEqualizer::equalizeAll()
{
  CALL("equalizeAll()");
  bool notEnd;
  ulong str;
 equalize_const:
  if (_nextConst)
    {         
      str = 0;
      do
	{
	  if (_const[str].equalize(_currLitNum,notEnd)) { str++; }
          else 
	    if (notEnd) { str = 0; }
	    else return false;
	}
      while (str != _nextConst);        
    };
  ASSERT(constStreamsNonempty());
  ASSERT(constProperlyEqualized());
    
  // equalize var streams 
  str = 0;
  while (str != _nextVar)
    if (_var[str].equalize(_currLitNum,notEnd)) { str++; }
    else 
      if (notEnd) { goto equalize_const; }
      else return false;       
  ASSERT(varStreamsNonempty());
  ASSERT(varProperlyEqualized());
  return true; 
}; // bool BSMachine::LitEqualizer::equalizeAll()


//============================================================================
#undef DEBUG_NAMESPACE 
#ifdef DEBUG_BS_MACHINE
#define DEBUG_NAMESPACE "BSMachine::LitEqualizer::ConstStream"
#endif
#include "debugMacros.hpp"
//============================================================================

inline
bool BSMachine::LitEqualizer::ConstStream::equalize(ulong& litNum,bool& notEnd)
{
  CALL("equalize(ulong& litNum,bool& notEnd)");
  ASSERT(llist);
  do
    {
      if (llist->hd() == litNum) return true;
      if (llist->hd() < litNum) { notEnd = true; litNum = llist->hd(); return false; }; 
      // llist->hd() > litNum 
      llist = llist->tl(); 
    }
  while (llist);
  notEnd = false;
  return false;  
}; // bool BSMachine::LitEqualizer::ConstStream::equalize(ulong& litNum,bool& notEnd)

//============================================================================
#undef DEBUG_NAMESPACE 
#ifdef DEBUG_BS_MACHINE
#define DEBUG_NAMESPACE "BSMachine::LitEqualizer::VarStream"
#endif
#include "debugMacros.hpp"
//============================================================================


inline
bool BSMachine::LitEqualizer::VarStream::equalize(ulong& litNum,bool& notEnd)
{
  CALL("equalize(ulong& litNum,bool& notEnd)");
  ASSERT(ltlist);
  do
    {
      if (ltlist->hd().el1 == litNum) return true;
      if (ltlist->hd().el1 < litNum) { notEnd = true; litNum = ltlist->hd().el1; return false; }; 
      // ltlist->hd().el1 > litNum 
      ltlist = ltlist->tl(); 
    }
  while (ltlist);
  notEnd = false;
  return false;  
}; // bool BSMachine::LitEqualizer::VarStream::equalize(ulong& litNum,bool& notEnd)

//============================================================================
#undef DEBUG_NAMESPACE 
#ifdef DEBUG_BS_MACHINE
#define DEBUG_NAMESPACE "BSMachine::SymLitEqualizer"
#endif
#include "debugMacros.hpp"
//============================================================================


inline
bool BSMachine::SymLitEqualizer::equalize() 
{
  CALL("equalize()");
  if (equalizeFirst())
    {
      subst.reset();
      do
	{
	  collectSubst();
	}
      while (equalizeNext());

      return true;  
    };
  return false;
}; // bool BSMachine::SymLitEqualizer::equalize() 






inline
void BSMachine::SymLitEqualizer::collectSubst() 
{ 
  CALL("collectSubst()");
     
  ulong i;
  if ((!_currArgNum1) || (!_currArgNum2))
    {
      // only one row
      for (i = 0; i < _nextVar1; i++) subst.set(i,_var1[i].topTerm());
      for (i = 0; i < _nextVar2; i++) subst.set(_nextVar1 + i,_var2[i].topTerm()); 
    }
  else // _currArgNum1 == 1 && _currArgNum2 == 1  
    {
      // two rows possible 
      if (_altArg2) 
        {
	  for (i = 0; i < _nextVar1; i++) subst.set(i,_var1[i].topTerm());
	  for (i = 0; i < _nextVar2; i++) subst.set(_nextVar1 + i,_var2[i].secondTerm());
	  // two rows are still possible
	  if (_altArg1)
	    { 
	      subst.setLitNum(_currLitNum);
	      subst.endOfRow(); 
	      for (i = 0; i < _nextVar1; i++) subst.set(i,_var1[i].secondTerm());
	      for (i = 0; i < _nextVar2; i++) subst.set(_nextVar1 + i,_var2[i].topTerm()); 
	    };
        }
      else
        {
	  ASSERT(_altArg1);
	  for (i = 0; i < _nextVar1; i++) subst.set(i,_var1[i].secondTerm());
	  for (i = 0; i < _nextVar2; i++) subst.set(_nextVar1 + i,_var2[i].topTerm()); 
        };
    };
  subst.setLitNum(_currLitNum);
  subst.endOfRow();  
}; // void collectSubst() 

inline
bool BSMachine::SymLitEqualizer::equalizeFirst()
{
  CALL("equalizeFirst()");
  ASSERT(_nextConst1 || _nextVar1); 
  _currLitNum = (_nextConst1) ? (_const1->topLitNum()) : (_var1->topLitNum());
  _currArgNum1 = 1;
  _currArgNum2 = 1;
  return equalizeAll();
};


inline
bool BSMachine::SymLitEqualizer::equalizeNext()
{
  CALL("equalizeNext()");
  if (!_currLitNum) return false;
  _currLitNum--;
  _currArgNum1 = 1;
  _currArgNum2 = 1;
  return equalizeAll();
};  


inline
bool BSMachine::SymLitEqualizer::alternativeArg1()
{
  CALL("alternativeArg1()");
  ulong i;
  for (i = 0; i < _nextConst1; i++) 
    if ((!(_const1[i].list()->tl())) || (_const1[i].list()->tl()->hd().el1 != _currLitNum)) 
      return false;
  for (i = 0; i < _nextVar1; i++) 
    if ((!(_var1[i].list()->tl())) || (_var1[i].list()->tl()->hd().el1 != _currLitNum)) 
      return false;
  return true;
};


inline
bool BSMachine::SymLitEqualizer::alternativeArg2()
{
  CALL("alternativeArg2()");
  ulong i;
  for (i = 0; i < _nextConst2; i++) 
    if ((!(_const2[i].list()->tl())) || (_const2[i].list()->tl()->hd().el1 != _currLitNum)) 
      return false;
  for (i = 0; i < _nextVar2; i++) 
    if ((!(_var2[i].list()->tl())) || (_var2[i].list()->tl()->hd().el1 != _currLitNum)) 
      return false;
  return true;
};



inline
bool BSMachine::SymLitEqualizer::equalizeAll()
{
  CALL("equalizeAll()");
  bool notEnd;
  ulong str;
 equalize_const:
  if (_nextConst1)
    {         
      str = 0;
      do
	{
	  if (_const1[str].equalize(_currLitNum,_currArgNum1,notEnd)) { str++; }
          else 
	    if (notEnd) { str = 0; }
	    else return false;
	}
      while (str != _nextConst1);        
    };
       
  if (_nextConst2)
    {         
      str = 0;
      do
	{
	  if (_const2[str].equalize(_currLitNum,_currArgNum2,notEnd)) { str++; }
          else 
	    if (notEnd) { goto equalize_const;  }
	    else return false;
	}
      while (str != _nextConst2);        
    };
  ASSERT(constStreamsNonempty());
  ASSERT(constProperlyEqualized());
    
  // equalize var streams 
  str = 0;
  while (str != _nextVar1)
    if (_var1[str].equalize(_currLitNum,_currArgNum1,notEnd)) { str++; }
    else 
      if (notEnd) { goto equalize_const; }
      else return false;    
      
  str = 0;
  while (str != _nextVar2)
    if (_var2[str].equalize(_currLitNum,_currArgNum2,notEnd)) { str++; }
    else 
      if (notEnd) { goto equalize_const; }
      else return false;        

  ASSERT(varStreamsNonempty());
  ASSERT(varProperlyEqualized());

  if (_currArgNum1 + _currArgNum2)
    {
      _altArg1 = alternativeArg1();
      _altArg2 = alternativeArg2();
      if (_currArgNum1 != _currArgNum2) return true;
      // _currArgNum1 == _currArgNum2 == 1
      if (_altArg1 || _altArg2) return true;                  
    };
  if (!_currLitNum) return false;
  _currLitNum--;        
  _currArgNum1 = 1;
  _currArgNum2 = 1;
  goto equalize_const;
}; // bool BSMachine::SymLitEqualizer::equalizeAll()


//============================================================================
#undef DEBUG_NAMESPACE 
#ifdef DEBUG_BS_MACHINE
#define DEBUG_NAMESPACE "BSMachine::SymLitEqualizer::ConstStream"
#endif
#include "debugMacros.hpp"
//============================================================================


inline
bool BSMachine::SymLitEqualizer::ConstStream::equalize(ulong& litNum,ulong& argNum,bool& notEnd)
{
  CALL("equalize(ulong& litNum,ulong& argNum,bool& notEnd)");
  ASSERT(lalist);
  do
    {
      if (lalist->hd().el1 == litNum) 
	{
	  if (lalist->hd().el2 == argNum) return true;
	  if (lalist->hd().el2 < argNum) 
            { 
	      notEnd = true; 
	      argNum = lalist->hd().el2;
	      return false;            
            };
	  // lalist->hd().el2 > argNum 
	}
      else
	{
	  if (lalist->hd().el1 < litNum) 
            {
	      notEnd = true; 
	      litNum = lalist->hd().el1; 
	      argNum = lalist->hd().el2;
	      return false; 
            }; 
	  // lalist->hd().el1 > litNum
	}; 
      // (lalist->hd().el1 > litNum) ||  (lalist->hd().el1 == litNum &&  lalist->hd().el2 > argNum)
      lalist = lalist->tl();
    }
  while (lalist);
  notEnd = false;
  return false;  
}; // bool BSMachine::SymLitEqualizer::ConstStream::equalize(ulong& litNum,ulong& argNum,bool& notEnd)


//============================================================================
#undef DEBUG_NAMESPACE 
#ifdef DEBUG_BS_MACHINE
#define DEBUG_NAMESPACE "BSMachine::SymLitEqualizer::VarStream"
#endif
#include "debugMacros.hpp"
//============================================================================

inline
bool BSMachine::SymLitEqualizer::VarStream::equalize(ulong& litNum,ulong& argNum,bool& notEnd)
{
  CALL("equalize(ulong& litNum,ulong& argNum,bool& notEnd)");
  ASSERT(latlist);
  do
    {
      if (latlist->hd().el1 == litNum) 
	{
	  if (latlist->hd().el2 == argNum) return true;
	  if (latlist->hd().el2 < argNum) 
            { 
	      notEnd = true; 
	      argNum = latlist->hd().el2;
	      return false;            
            };
	  // latlist->hd().el2 > argNum 
	}
      else
	{
	  if (latlist->hd().el1 < litNum) 
            {
	      notEnd = true; 
	      litNum = latlist->hd().el1; 
	      argNum = latlist->hd().el2;
	      return false; 
            }; 
	  // latlist->hd().el1 > litNum
	}; 
      // (latlist->hd().el1 > litNum) ||  (latlist->hd().el1 == litNum &&  latlist->hd().el2 > argNum)
      latlist = latlist->tl();
    }
  while (latlist);
  notEnd = false;
  return false;  
};  // bool BSMachine::VarStream::equalize(ulong& litNum,ulong& argNum,bool& notEnd)




//============================================================================
