#include "or_premise.hpp"

//================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_OR_PREMISE
 #define DEBUG_NAMESPACE "OR_PREMISE"
#endif
#include "debugMacros.hpp"
//=================================================

using namespace VK;

OR_PREMISE::OR_PREMISE() 
  : _resolvent(0), 
  _termMemory(DOP("OR_PREMISE::_termMemory")),
  _maxLiteral(DOP("OR_PREMISE::_maxLiteral")),
  _litIsInherentlySelected(DOP("OR_PREMISE::_litIsInherentlySelected")),
  _litIsMarked(DOP("OR_PREMISE::_litIsMarked"))
{
  CALL("constructor OR_PREMISE()");
  DOP(_termMemory.freeze());
  DOP(_maxLiteral.freeze());
  DOP(_litIsInherentlySelected.freeze());
  DOP(_litIsMarked.freeze());
}; // OR_PREMISE::OR_PREMISE() 

OR_PREMISE::~OR_PREMISE() { CALL("destructor ~OR_PREMISE()"); };

void OR_PREMISE::init() 
{ 
  CALL("init()");
  _resolvent = 0; 
  _termMemory.init(DOP("OR_PREMISE::_termMemory")); 
  DOP(_termMemory.freeze());
  _maxLiteral.init(DOP("OR_PREMISE::_maxLiteral"));
  DOP(_maxLiteral.freeze());
  _litIsInherentlySelected.init(DOP("OR_PREMISE::_litIsInherentlySelected"));
  DOP(_litIsInherentlySelected.freeze());

  _litIsMarked.init(DOP("OR_PREMISE::_litIsMarked")); 
  DOP(_litIsMarked.freeze());

}; // void OR_PREMISE::init() 


void OR_PREMISE::destroy()
{
  CALL("destroy()");
  _litIsMarked.destroy();
  _litIsInherentlySelected.destroy();
  _maxLiteral.destroy();
  _termMemory.destroy();
}; // void OR_PREMISE::destroy()


OR_PREMISE::OR_PREMISE(NewClause* res) 
  : _resolvent(res), 
  _termMemory(DOP("OR_PREMISE::_termMemory")),
  _maxLiteral(DOP("OR_PREMISE::_maxLiteral")),
  _litIsInherentlySelected(DOP("OR_PREMISE::_litIsInherentlySelected")),
  _litIsMarked(DOP("OR_PREMISE::_litIsMarked"))
{
  CALL("constructor OR_PREMISE(NewClause* res)"); 
  DOP(_termMemory.freeze());
  DOP(_maxLiteral.freeze());
  DOP(_litIsInherentlySelected.freeze());
  DOP(_litIsMarked.freeze());
}; // OR_PREMISE::OR_PREMISE(NewClause* res) 


void OR_PREMISE::init(NewClause* res) 
{ 
  _resolvent = res; 
  _termMemory.init(DOP("OR_PREMISE::_termMemory")); 
  DOP(_termMemory.freeze());
  _maxLiteral.init(DOP("OR_PREMISE::_maxLiteral"));
  DOP(_maxLiteral.freeze());
  _litIsInherentlySelected.init(DOP("OR_PREMISE::_litIsInherentlySelected"));
  DOP(_litIsInherentlySelected.freeze());

  _litIsMarked.init(DOP("OR_PREMISE::_litIsMarked")); 
  DOP(_litIsMarked.freeze());

}; // void OR_PREMISE::init(NewClause* res) 


void OR_PREMISE::Load(ulong ind,Clause* c)
{
  CALL("Load(ulong ind,Clause* c)");
  ASSERT(!c->isAnswer());
  DOP(_termMemory.unfreeze());
  _termMemory.expand(c->size()); 
  DOP(_termMemory.freeze());

  _freeTermMemory = _termMemory.memory();
  //free_memory = memory;

  DOP(_maxLiteral.unfreeze());
  _maxLiteral.expand(c->numOfAllLiterals());
  DOP(_maxLiteral.freeze()); 

  DOP(_litIsInherentlySelected.unfreeze());
  _litIsInherentlySelected.expand(c->numOfAllLiterals());
  DOP(_litIsInherentlySelected.freeze());

  DOP(_litIsMarked.unfreeze());
  _litIsMarked.expand(c->numOfAllLiterals());
  DOP(_litIsMarked.freeze());

  clause = c;
  index = ind;

  _allLiterals = clause->LitList(); 
  _firstNonselectedLit = _allLiterals->nth(_allLiterals->numOfSelectedLits());
  last_max = _allLiterals->numOfSelectedLits() - 1;
  ulong max_lit = 0; 
  for(LiteralList::Iterator iter(_allLiterals);iter.currentElement() != _firstNonselectedLit;iter.next())
    {
      _maxLiteral[max_lit] = _freeTermMemory;
      _litIsMarked[max_lit] = false;
      StandardTermToPrefix(ind,iter.currentLiteral(),_freeTermMemory); 
      _litIsInherentlySelected[max_lit] = iter.currentElement()->isInherentlySelected();
      max_lit++;
    };   
}; // void OR_PREMISE::Load(ulong ind,Clause* c)

void OR_PREMISE::LoadOptimized(ulong ind,Clause* c)
{
  CALL("LoadOptimized(ulong ind,Clause* c)");
  // if there is only one maximal literal we do not transform it
  // into prefix representation
  
  ASSERT(!c->isAnswer());
  DOP(_termMemory.unfreeze());
  _termMemory.expand(c->size()); 
  DOP(_termMemory.freeze());
   
  _freeTermMemory = _termMemory.memory();
  //free_memory = memory;

  DOP(_maxLiteral.unfreeze());
  _maxLiteral.expand(c->numOfAllLiterals());
  DOP(_maxLiteral.freeze()); 

  DOP(_litIsInherentlySelected.unfreeze());
  _litIsInherentlySelected.expand(c->numOfAllLiterals());
  DOP(_litIsInherentlySelected.freeze());

  DOP(_litIsMarked.unfreeze());
  _litIsMarked.expand(c->numOfAllLiterals());
  DOP(_litIsMarked.freeze());

  clause = c;
  index = ind;

  _allLiterals = clause->LitList(); 
  _firstNonselectedLit = _allLiterals->nth(_allLiterals->numOfSelectedLits());
  last_max = _allLiterals->numOfSelectedLits() - 1;

  if (last_max)
    {
      ulong max_lit = 0; 
      for(LiteralList::Iterator iter(_allLiterals);iter.currentElement() != _firstNonselectedLit;iter.next())
	{
	  _maxLiteral[max_lit] = _freeTermMemory;
	  _litIsMarked[max_lit] = false;
	  StandardTermToPrefix(ind,iter.currentLiteral(),_freeTermMemory); 

	  _litIsInherentlySelected[max_lit] = iter.currentElement()->isInherentlySelected();
	  max_lit++;
	};   
    }
  else
    {
      _maxLiteral[0] = 0; // just in case
      _litIsMarked[0] = false; // just in case
    };
}; // void OR_PREMISE::LoadOptimized(ulong ind,Clause* c) 


#ifndef NO_DEBUG_VIS  
ostream& OR_PREMISE::output(ostream& str) const
{
  str <<  "<<<<<< PREMISE >>>>>>>\n"
      << " Clause : " << clause << "\n Index = " << index
      << "\n Maximal literals: \n";
  for (ulong ml = 0; ml <= last_max; ml++)
    {
      str << "  [" << _litIsMarked[ml] << "] ";
      Prefix::outputTerm(str,_maxLiteral[ml]);
      str << '\n';
    };
  return str; 
}; // ostream& OR_PREMISE::output(ostream& str) const
#endif

//=================================================
