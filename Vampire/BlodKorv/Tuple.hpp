//
// File:         Tuple.hpp
// Description:  Generic tuples.
// Created:      Feb 26, 2000, 16:40
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#ifndef TUPLE_H
//============================================================================
#define TUPLE_H 
#include <iostream>
#include "jargon.hpp"
//============================================================================
namespace BK 
{
template <class Type1,class Type2>
class Tuple2
{
 public:
  Tuple2() {};
  Tuple2(const Type1& e1,const Type2& e2) : el1(e1), el2(e2) {};
  ~Tuple2() {};

 public:
  Type1 el1;
  Type2 el2;
  ostream& output(ostream& str) const 
    {
      return str << '<' << el1 << ',' << el2 << '>';
    };   
  bool operator<(const Tuple2& t)
    {
      return (el1 < t.el1) || ((el1 == t.el1) && (el2 < t.el2));
    };

  bool operator>(const Tuple2& t)
    {
      return (el1 > t.el1) || ((el1 == t.el1) && (el2 > t.el2));
    };

  bool operator<=(const Tuple2& t)
  {
    return (el1 < t.el1) || ((el1 == t.el1) && (el2 <= t.el2));
  };

  bool operator>=(const Tuple2& t)
  {
    return (el1 > t.el1) || ((el1 == t.el1) && (el2 >= t.el2));
  };
  bool operator==(const Tuple2& t)
  {
    return (el1 == t.el1) && (el2 == t.el2);
  };   
}; // template <class Type1,class Type2> class Tuple2



template <class Type1,class Type2,class Type3>
class Tuple3
{
 public:
  Tuple3() {};
  Tuple3(const Type1& e1,const Type2& e2,const Type3& e3)
    : el1(e1), el2(e2), el3(e3)
    {
    };
  ~Tuple3() {};
  Type1 el1;
  Type2 el2;
  Type3 el3;
  ostream& output(ostream& str) const 
    {
      return str << '<' << el1 << ',' << el2 << ',' << el3 << '>';
    };
}; // template <class Type1,class Type2,class Type3> class Tuple3

template <class Type1,class Type2,class Type3,class Type4>
class Tuple4
{
 public:
  Tuple4() {};
  Tuple4(const Type1& e1,const Type2& e2,const Type3& e3,const Type4& e4)
    : el1(e1), el2(e2), el3(e3), el4(e4)
    {
    };
  Type1 el1;
  Type2 el2;
  Type3 el3;
  Type4 el4;
  ostream& output(ostream& str) const 
    {
      return str << '<' << el1 << ',' << el2 << ',' << el3 << ',' << el4 << '>';
    };
}; // template <class Type1,class Type2,class Type3,class Type4> class Tuple4

}; // namespace BK

namespace std
{
template <class Type1,class Type2>
ostream& operator<<(ostream& str,const BK::Tuple2<Type1,Type2>& t)
{
  return t.output(str);
};

template <class Type1,class Type2,class Type3>
ostream& operator<<(ostream& str,const BK::Tuple3<Type1,Type2,Type3>& t)
{
  return t.output(str);
};

template <class Type1,class Type2,class Type3,class Type4>
ostream& operator<<(ostream& str,const BK::Tuple4<Type1,Type2,Type3,Type4>& t)
{
  return t.output(str);
};
};

//=============================================================================
#endif
