// $ANTLR 2.7.1: "TPTP.g" -> "TPTPLexer.java"$

  // head of lexer file
#include <iostream>

  //#include "term.h"
  //#include "literal.h"
  //#include "clause.h"
  //#include "formula.h"
  //#include "int_name_table.h"
  #include "unit.h"
  //#include "signature.h"
  //#include "problem.h"

  //class Atom;
  //class Formula;

public interface TPTPLexerTokenTypes {
	int EOF = 1;
	int NULL_TREE_LOOKAHEAD = 3;
	int UPPER = 4;
	int LOWER = 5;
	int DIGIT = 6;
	int WHITE = 7;
	int NUMBER = 8;
	int WORDCHAR = 9;
	int CHARACTER = 10;
	int SKIP_RULE = 11;
	int COMMENT_LINE = 12;
	int LPAR = 13;
	int RPAR = 14;
	int LBRA = 15;
	int RBRA = 16;
	int COMMA = 17;
	int COLON = 18;
	int DOT = 19;
	int AND = 20;
	int NOT = 21;
	int OR = 22;
	int IFF = 23;
	int IMP = 24;
	int XOR = 25;
	int FORALL = 26;
	int EXISTS = 27;
	int PP = 28;
	int MM = 29;
	int NAME = 30;
	int VAR = 31;
	int T_IMP = 32;
	int LITERAL_input_formula = 33;
	int LITERAL_input_clause = 34;
}
