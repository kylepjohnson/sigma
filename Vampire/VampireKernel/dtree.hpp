#ifndef DTREE_H
//=====================================================
#define DTREE_H
#ifndef NO_DEBUG_VIS
 #include <iostream>
#endif
#include "jargon.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "GlobAlloc.hpp"
#include "Term.hpp"
//==================================================
#undef DEBUG_NAMESPACE  
#ifdef DEBUG_DTREE
 #define DEBUG_NAMESPACE "DTREE_TAG"
#endif
#include "debugMacros.hpp"
//=================================================

namespace VK
{
class DTREE_TAG
 {
  private:
   TERM symbol;
  public:
   DTREE_TAG() {};
   DTREE_TAG(TERM* p) { symbol.SetFirst(p); }; // for leaves
   inline DTREE_TAG& operator= (TERM* p) // for leaves
    {
     symbol.SetFirst(p);
     return *this; 
    }; 
   
   DTREE_TAG(const TERM& sym) // for intermediate nodes
    : symbol(sym) 
    { 
    };
   ~DTREE_TAG() {};
    
   inline DTREE_TAG& operator= (const DTREE_TAG& tag) // // for intermediate nodes
    {
     symbol = tag.symbol;
     return *this;
    };
    
   inline bool IsLeaf() const { return symbol.IsReference(); };

   inline bool equal(const TERM& sym) const // for intermediate nodes only
    {
     return symbol == sym;
    };
   inline bool Greater(const TERM& sym) const // for intermediate nodes only
    {
     if (symbol.Flag() == sym.Flag())
      return symbol > sym;
     return symbol.Flag() > sym.Flag();
    };

   
   inline bool equal(const DTREE_TAG& tag) const // for intermediate nodes only
    {
     return equal(tag.symbol);
    };

   inline bool Greater(const DTREE_TAG& tag) const // for intermediate nodes only
    {
     return Greater(tag.symbol);
    }; 
          
    
   inline TERM* Term() const { return symbol.First(); }; // for leaves
   inline void SetTerm(TERM* t) { symbol.SetFirst(t); }; // for leaves
   #ifndef NO_DEBUG_VIS
    ostream& output(ostream& str) const { return str << symbol; };
   #endif 
 }; // class DTREE_TAG
}; // namespace VK
//==================================================
#undef DEBUG_NAMESPACE  
#ifdef DEBUG_DTREE
 #define DEBUG_NAMESPACE "DTREE_NODE"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
class DTREE_NODE
 {
  private:
   DTREE_TAG _tag;
   DTREE_NODE* _next;
   DTREE_NODE* _less;
   DTREE_NODE* _greater;
   DTREE_NODE() {};
  public:
   DTREE_NODE(TERM* p) : _tag(p), _next(0) {}; // for making leaves
   DTREE_NODE(const TERM& sym)  // for intermediate nodes
    : _tag(sym),
      _less(0), 
      _greater(0)
    {};
   ~DTREE_NODE() {};
   inline void Copy(const DTREE_NODE& node) // for intermediate nodes
    {
     _tag = node._tag;
     _next = node._next;
     _less = node._less;
     _greater = node._greater;
    };  
    
   inline void* operator new(size_t)
    {
     return BK::GlobAlloc::allocate(sizeof(DTREE_NODE));
    };
   inline void operator delete(void* obj)
    {
     BK::GlobAlloc::deallocate(obj,sizeof(DTREE_NODE));
    };
   DTREE_TAG& Tag() { return _tag; };
   const DTREE_TAG& Tag() const { return _tag; }; 
   inline TERM* Term() const { return _tag.Term(); }; // for leaves
   inline void SetTerm(TERM* t) { _tag.SetTerm(t); };
   inline bool IsLeaf() const { return _tag.IsLeaf(); };
   inline DTREE_NODE*& Next() { return _next; }; // for intermediate 
   inline DTREE_NODE* const & Next() const { return _next; }; // for intermediat
   inline long& NumOfClauses() { return (long&)_next; }; // for leaves
   inline DTREE_NODE*& less() { return _less; }; // for intermediate
   inline DTREE_NODE* const & less() const { return _less; }; // for intermediate
   inline DTREE_NODE*& greater() { return _greater; }; // for intermediate
   inline DTREE_NODE* const & greater() const { return _greater; }; // for intermediate

   inline void*& Reserved1() { return (void*&)_less; };
   inline void*& Reserved2() { return (void*&)_greater; };
   
   inline bool Fork() { return (less() || greater()); }; 
   #ifndef NO_DEBUG_VIS
    ostream& output(ostream& str) const
    {
     str << '[' << (unsigned long)this << "] ";
     if (IsLeaf()) return str << "LEAF " << Term();
     return Tag().output(str) << " N[" << (unsigned long)Next()
                              << "] L[" << (unsigned long)less()
                              << "] G[" << (unsigned long)greater() << "] "; 
    };
   #endif
 }; // class DTREE_NODE
}; // namespace VK

//==================================================
#undef DEBUG_NAMESPACE  
#ifdef DEBUG_DTREE
 #define DEBUG_NAMESPACE "DTREE"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{

class DTREE
 {
  private:
   static inline DTREE_NODE* Minimal(DTREE_NODE* node);
   static inline DTREE_NODE* Merge(DTREE_NODE* less,DTREE_NODE* greater);
  public:
   static inline DTREE_NODE** Track(const TERM& sym,DTREE_NODE** node);
   static inline DTREE_NODE* Track(const TERM& sym,DTREE_NODE* node);
   static inline DTREE_NODE* Track(const DTREE_TAG& tag,DTREE_NODE* node);
   static inline DTREE_NODE** Track(TERM::IteratorWithDepth& trm,DTREE_NODE** node);
   static inline void InfoForDeletion(TERM::Iterator& trm,
                                      DTREE_NODE** tree,
                                      DTREE_NODE**& branch_to_destroy,
                                      DTREE_NODE*& leaf,
                                      DTREE_NODE*& last_fork);
   static inline DTREE_NODE* CreateBranch(TERM::IteratorWithDepth& trm,
                                          DTREE_NODE*& leaf);
   static inline void DestroyBranchWithoutForks(DTREE_NODE* branch);
   static inline void ReduceFork(DTREE_NODE* fork);
   static inline void Destroy(DTREE_NODE* tree,void (*DestroyLeafContent)(DTREE_NODE* leaf));
  public: 
   class SURFER; 
  public: // for debugging
   #ifdef DEBUG_NAMESPACE
    static bool BranchWithNoForks(DTREE_NODE* branch)
     {
      CALL("BranchWithNoForks(DTREE_NODE* branch)");
      // branch must be nonnull here
      ASSERT(branch);
      if (branch->IsLeaf()) { return true; };
      if ((branch->less()) || (branch->greater())) { return false; };
      return BranchWithNoForks(branch->Next());
     };
   #endif
 }; // class DTREE
}; // namespace VK
//==================================================
#undef DEBUG_NAMESPACE  
#ifdef DEBUG_DTREE
 #define DEBUG_NAMESPACE "DTREE::SURFER"
#endif
#include "debugMacros.hpp"
//=================================================
namespace VK
{
class DTREE::SURFER
 {
  private:
   DTREE_NODE** node;
  public:
   SURFER() 
   {
   };
   ~SURFER() {};
   inline void Reset(DTREE_NODE** tree,unsigned long init_depth)
    {
     node = tree;
    };
   inline bool Read(const TERM& sym)
    {
     CALL("Read(const TERM& sym)");   
     node = DTREE::Track(sym,node);
     if (*node)
      {
       node = &((*node)->Next());
       return true;
      }
     else return false;
    }; 
   inline void Write(const TERM& sym)
    {
     CALL("Write(const TERM& sym)");
     *node = new DTREE_NODE(sym);
     node = &((*node)->Next());
    }; 
   inline void CreateLeaf(TERM* t)
    {
     *node = new DTREE_NODE(t);
    };
   inline DTREE_NODE* Leaf() { return *node; };
  
 }; // class DTREE::SURFER

//*************** Definitions for DTREE: *****************************

inline DTREE_NODE* DTREE::Minimal(DTREE_NODE* node)
 {
  CALL("Minimal(DTREE_NODE* node)");
  // node->less() must be nonnull here
  ASSERT(node->less());
  next_less:
   node = node->less();
   if (node->less()) { goto next_less; }
   else { return node; };
 }; // DTREE_NODE* DTREE::Minimal(DTREE_NODE* node)

inline DTREE_NODE* DTREE::Merge(DTREE_NODE* less,DTREE_NODE* greater)
 {
  CALL("Merge(DTREE_NODE* less,DTREE_NODE* greater)");
  ASSERT(less);
  ASSERT(greater);
  if (less->greater())
   {
    if (greater->less())
     {
      Minimal(greater)->less() = less;
      return greater;
     }
    else
     {
      greater->less() = less;
      return greater;
     };
   }
  else
   {
    less->greater() = greater;
    return less;
   };
 }; // DTREE_NODE* DTREE::Merge(DTREE_NODE* less,DTREE_NODE* greater)

inline void DTREE::ReduceFork(DTREE_NODE* fork)
 {
  CALL("ReduceFork(DTREE_NODE* fork)");
  // either fork->less() or fork->greater() must be nonnull here
  ASSERT(fork);
  ASSERT(fork->Fork());
  
  DTREE_NODE* node_to_delete;
  if (fork->less())
   {
    if (fork->greater())
     {
      node_to_delete = Merge(fork->less(),fork->greater());
      fork->Copy(*node_to_delete);
     }
    else
     {
      node_to_delete = fork->less();
      fork->Copy(*(fork->less()));
     };
   }
  else 
   {
    node_to_delete = fork->greater();
    fork->Copy(*(fork->greater()));
   };
  delete node_to_delete;
 }; // void DTREE::ReduceFork(DTREE_NODE* fork) 

inline DTREE_NODE** DTREE::Track(const TERM& sym,DTREE_NODE** node)
 {
  check_node:
   if (*node)
    {
     if ((*node)->Tag().equal(sym)) { return node; };
     if ((*node)->Tag().Greater(sym))
      {
       node = &((*node)->less());
      }
     else
      {
       node = &((*node)->greater());
      }; 
     goto check_node; 
    }
   else { return node; };
 }; // DTREE_NODE** DTREE::Track(const TERM& sym,DTREE_NODE** node)

inline DTREE_NODE* DTREE::Track(const TERM& sym,DTREE_NODE* node)
 {
  check_node:
   if (node)
    {
     if (node->Tag().equal(sym)) { return node; };
     if (node->Tag().Greater(sym))
      {
       node = node->less();
      }
     else
      {
       node = node->greater();
      }; 
     goto check_node; 
    }
   else { return 0; };
 }; // DTREE_NODE* DTREE::Track(const TERM& sym,DTREE_NODE* node)

inline DTREE_NODE* DTREE::Track(const DTREE_TAG& tag,DTREE_NODE* node)
 {
  check_node:
   if (node)
    {
     if (node->Tag().equal(tag)) { return node; };
     if (node->Tag().Greater(tag))
      {
       node = node->less();
      }
     else
      {
       node = node->greater();
      }; 
     goto check_node; 
    }
   else { return 0; };
 }; // DTREE_NODE* DTREE::Track(const DTREE_TAG& tag,DTREE_NODE* node)

inline DTREE_NODE** DTREE::Track(TERM::IteratorWithDepth& trm,DTREE_NODE** node)
 {
  CALL("Track(TERM::IteratorWithDepth& trm,DTREE_NODE** node)");
  // trm must be nonempty here 
  track_sym:
   node = Track(trm.Symbol(),node);
   if (*node)
    {
     node = &((*node)->Next());
     if (trm.Next())
      {
       // *node must be an intermediate node here
       ASSERT(!((*node)->IsLeaf()));
       goto track_sym;
      }
     else 
      {
       // *node must be a leaf here
       ASSERT((*node)->IsLeaf());
       return node; 
      }; 
    }
   else { return node; };
 }; // DTREE_NODE** DTREE::Track(TERM::IteratorWithDepth& trm,DTREE_NODE** node)
 
inline DTREE_NODE* DTREE::CreateBranch(TERM::IteratorWithDepth& trm,
                                       DTREE_NODE*& leaf)
 {
  // trm must be nonempty here 
  DTREE_NODE* branch = new DTREE_NODE(trm.Symbol()); //,trm.Depth());
  DTREE_NODE* last_node = branch;
  next_sym:
   if (trm.Next())
    {
     last_node->Next() = new DTREE_NODE(trm.Symbol()); //,trm.Depth());
     last_node = last_node->Next();
     goto next_sym;
    }
   else
    {
     leaf = new DTREE_NODE(0);
     last_node->Next() = leaf;
    };   
  return branch;
 }; // DTREE_NODE* DTREE::CreateBranch(TERM::IteratorWithDepth& trm,DTREE_NODE*& leaf)                                         

inline void DTREE::DestroyBranchWithoutForks(DTREE_NODE* branch)
 {
  CALL("DestroyBranchWithoutForks(DTREE_NODE* branch)");
  // branch must be nonnul here
  ASSERT(branch);
  DTREE_NODE* node_to_delete;
  next_node:
   if (branch->IsLeaf()) { delete branch; }
   else
    {
     node_to_delete = branch;
     branch = branch->Next();
     delete node_to_delete;
     goto next_node;
    };   
 }; // void DTREE::DestroyBranchWithoutForks(DTREE_NODE* branch)

inline void DTREE::Destroy(DTREE_NODE* tree,void (*DestroyLeafContent)(DTREE_NODE* leaf))
 {
  if (tree)
   {    
    if (tree->IsLeaf()) 
     {
      DestroyLeafContent(tree);
      delete tree;
     }
    else
     {
      if (tree->less()) Destroy(tree->less(),DestroyLeafContent);
      if (tree->greater()) Destroy(tree->greater(),DestroyLeafContent);
      if (tree->Next()) Destroy(tree->Next(),DestroyLeafContent);
      delete tree;
     };
   };
 }; // void DTREE::Destroy(DTREE_NODE* tree,void (*DestroyLeafContent)(DTREE* leaf))

inline void DTREE::InfoForDeletion(TERM::Iterator& trm,
                                   DTREE_NODE** tree,
                                   DTREE_NODE**& branch_to_destroy,
                                   DTREE_NODE*& leaf,
                                   DTREE_NODE*& last_fork)
 {
  CALL("InfoForDeletion(TERM::Iterator&,DTREE_NODE**,DTREE_NODE**&,DTREE_NODE*&,DTREE_NODE*&)");
  // *tree must be nonnull here and not a leaf
  
  ASSERT(*tree);
  ASSERT(!((*tree)->IsLeaf()));
  
  // trm must be nonempty and contain all arguments of an indexed term
  
  last_fork = 0; 
  branch_to_destroy = tree;
  DTREE_NODE** current_branch;
  DTREE_NODE* current_node;
  
  track_sym:
   current_branch = Track(trm.Symbol(),tree);
   current_node = *current_branch;
  
   // current_node is nonnull here and contains symbol equal to trm.Symbol()
   ASSERT(current_node);
   ASSERT(!(current_node->IsLeaf()));
   ASSERT(current_node->Tag().equal(trm.Symbol()));
   
   if (current_node->Fork())  
    {
     branch_to_destroy = &(current_node->Next());
     last_fork = current_node;
    }
   else
    {
     if (current_branch != tree) 
      { 
       // this means that *tree is marked
       // by a symbol different from trm.Symbol()
       ASSERT(!((*tree)->Tag().equal(trm.Symbol())));
       branch_to_destroy = current_branch;
       last_fork = 0;
      }
     else
      {
       // the node *tree is marked by trm.Symbol()
       // nothing to do here
       ASSERT((*tree)->Tag().equal(trm.Symbol()));
      };
    };
  
   tree = &(current_node->Next());
 
   ASSERT(*tree);

   if (trm.Next()) { goto track_sym; };
   
   leaf = *tree;
   
   // postcondition: leaf is a leaf,
   ASSERT(leaf->IsLeaf());
 
   // *branch_to_destroy is nonnull and not a leaf
   // *branch_to_destroy is the beginning of a branch with no forks
   ASSERT(*branch_to_destroy);
   ASSERT(BranchWithNoForks(*branch_to_destroy));
 
   // if last_fork is nonnull:
   //  1) last_fork is an intermediate node
   //  2) last_fork is a fork
   //  3) branch_to_destroy == &(last_fork->Next())
   ASSERT((!last_fork) || (!(last_fork->IsLeaf())));
   ASSERT((!last_fork) || (last_fork->Fork()));
   ASSERT((!last_fork) || (branch_to_destroy == &(last_fork->Next()))); 

   // if (*branch_to_destroy)->IsLeaf(), then 
   // last_fork is nonnull and &(last_fork->Next()) == branch_to_destroy
   ASSERT((*branch_to_destroy)->IsLeaf() ? (last_fork != 0): true);
   ASSERT((*branch_to_destroy)->IsLeaf() ? (&(last_fork->Next()) == branch_to_destroy) : true);

 }; // void DTREE::InfoForDeletion(TERM::Iterator& trm,...

}; // namespace VK

//=====================================================================
#endif














