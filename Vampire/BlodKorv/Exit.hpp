//
// File:         Exit.hpp
// Description:  Hookable exit.
// Created:      Apr 11, 2000, 16:00
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//====================================================
#ifndef EXIT_H
//=================================================
#define EXIT_H
#include <cstdlib>
#include <iostream>
//=================================================
namespace BK 
{
class Exit
{
 public:
  static void exit(int code);
}; // class Exit
}; // namespace BK


//=================================================
#endif
