//
// File:         UITree.hpp
// Description:  Unification index trees.
//               See also UTerm.hpp, UVariable.hpp
// Created:      Apr 26, 2003, Alexandre Riazanov, riazanov@cs.man.ac.uk
// Status:       Sketch.
//============================================================================
#ifndef UI_TREE_H
#define UI_TREE_H 
//============================================================================
#ifndef NO_DEBUG_VIS
#  include <iostream>
#endif
#include "jargon.hpp"
#include "DestructionMode.hpp"
#include "GlobAlloc.hpp"
#include "VampireKernelDebugFlags.hpp"
#include "UTerm.hpp"
#include "UVariable.hpp"
//============================================================================
#ifdef DEBUG_UI_TREE
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "UITree"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK 
{
  class UITree
  {
  public:
    class Domain;
    class Node;
    class InnerNode;
    class LeafNode;
  public:
    static Domain* currentDomain() { return _currentDomain; }; 
  private:
    static Domain* _currentDomain;
    friend class Domain;
  }; // class UITree

}; // namespace VK 


//============================================================================
#ifdef DEBUG_UI_TREE
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "UITree::Node"
#endif
#include "debugMacros.hpp"
//============================================================================


namespace VK 
{
  class UITree::Node
  {
  public:
    class Descriptor;
  public:
    Node(const Descriptor* desc) : _descriptor(desc) {};
    ~Node() {};
    const Descriptor* descriptor() const { return _descriptor; };
  private:
    const Descriptor* _descriptor;
  }; // class UITree::Node
}; // namespace VK 


//============================================================================
#ifdef DEBUG_UI_TREE
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "UITree::InnerNode"
#endif
#include "debugMacros.hpp"
//============================================================================


namespace VK 
{
  class UITree::InnerNode : public UITree::Node 
  {
  public:
    InnerNode(const Descriptor* desc) : 
      Node(desc), 
      _next(0),
      _fork(0)
    {
    };
    ~InnerNode() {};
    Node* next() { return _next; };
    const Node* next() const { return _next; };
    Node** nextAddr() { return &_next; };
      
    Node* fork() { return _fork; };
    const Node* fork() const { return _fork; };
    Node** forkAddr() { return &_fork; };
      
  private:
    Node* _next;
    Node* _fork;
  }; // class UITree::InnerNode 
}; // namespace VK 




//============================================================================
#ifdef DEBUG_UI_TREE
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "UITree::LeafNode"
#endif
#include "debugMacros.hpp"
//============================================================================


namespace VK 
{
  class UITree::LeafNode : public UITree::Node 
  {
  public:
    LeafNode();
    ~LeafNode() {};
    const void* info() const { return _info; };
    void* info() { return _info; };
  private:
    void* _info;
  }; // class UITree::LeafNode

}; // namespace VK 



//============================================================================
#ifdef DEBUG_UI_TREE
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "UITree::Domain"
#endif
#include "debugMacros.hpp"
//============================================================================


namespace VK 
{
  class UITree::Domain
  {
  public:
    Domain() {};
    ~Domain() 
    {      
      if (BK::DestructionMode::isThorough())
	{
	  
	};
    };
    
    void activate()
    {
      CALL("activate()");
      _previousActive = UITree::_currentDomain;
      UITree::_currentDomain = this;
    };

    void deactivate()
    {
      CALL("activate()");
      ASSERT(this);
      UITree::_currentDomain = _previousActive;
    };

    const UITree::Node::Descriptor* leafDescriptor() const { return 0; }; // temporary

  private:
    Domain* _previousActive;
  }; // class UITree::Domain
}; // namespace VK 

//============================================================================
#ifdef DEBUG_UI_TREE
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "UITree::Node::Descriptor"
#endif
#include "debugMacros.hpp"
//============================================================================


namespace VK 
{
  class UITree::Node::Descriptor
  {

  }; // class UITree::Node::Descriptor

}; // namespace VK

//============================================================================
#ifdef DEBUG_UI_TREE
#  undef DEBUG_NAMESPACE
#  define DEBUG_NAMESPACE "UITree::LeafNode"
#endif
#include "debugMacros.hpp"
//============================================================================

namespace VK 
{
  inline
  UITree::LeafNode::LeafNode() : 
    UITree::Node(UITree::currentDomain()->leafDescriptor()),
	    _info(0)
  {
    CALL("constructor Leaf()");
  };
}; // namespace VK 

//============================================================================
#endif
