//
// File:         BackParaCandidates.cpp
// Description:  Indexed objects for back paramodulation.   
// Created:      May 25, 2000, 14:20
// Author:       Alexandre Riazanov
// mail:         riazanov@cs.man.ac.uk
//============================================================================
#include "BackParaCandidates.hpp"
//============================================================================
using namespace BK;
using namespace VK;

#ifdef DEBUG_ALLOC_OBJ_TYPE

ClassDesc BackParaCandidates::SkList::_classDesc("BackParaCandidates::SkList",
                                                   BackParaCandidates::SkList::minSize(),
                                                   BackParaCandidates::SkList::maxSize());

ClassDesc BackParaCandidates::SkList::Node::_classDesc("BackParaCandidates::SkList::Node",
                                                        BackParaCandidates::SkList::Node::minSize(),
                                                        BackParaCandidates::SkList::Node::maxSize());

 
ClassDesc BackParaCandidates::LitRedexPair::_classDesc("BackParaCandidates::LitRedexPair",
							BackParaCandidates::LitRedexPair::minSize(),
							BackParaCandidates::LitRedexPair::maxSize());
#endif


//============================================================================
