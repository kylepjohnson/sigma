//
// File:         DestructionMode.hpp
// Description:  Global flag, regulating destruction. 
// Created:      May 23, 2002, Alexandre Riazanov, riazanov@cs.man.ac.uk
//====================================================
#ifndef DESTRUCTION_MODE_H
#define DESTRUCTION_MODE_H
//=================================================
namespace BK 
{

class DestructionMode
{
 public:
  class InitStatic
  {
   public:
    InitStatic() 
     {    
      if (!_count) { _isThorough = true; _count++; }; 
     };
    ~InitStatic() {};
   private:
    static long _count; 
  };
 public:
  static bool isFast() { return !_isThorough; };
  static bool isThorough() { return _isThorough; };
  static void makeFast() { _isThorough = false; };
  static void makeThorough() { _isThorough = true; };
 private:
  static bool _isThorough;
  friend class InitStatic;
};

static DestructionMode::InitStatic destructionModeInitStatic;


}; // namespace BK

//=================================================
#endif
