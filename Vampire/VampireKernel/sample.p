
%symbol_weight('function',f,2,1).
%symbol_weight(function,g,2,2).



input_clause(cl,axiom,
	     [
	      ++p(f(a,f(g(b,c),g(c,b))))
	     ]).

input_clause(cl,axiom,
	     [
	      --p(a)
	     ]).


input_clause(cl,axiom,
	     [
	      ++equal(f(g(X1,X2),f(X0,X0)),
	              f(f(X1,X2),g(X0,X0))) 
	     ]).


input_clause(cl,axiom,
	     [
	      ++equal(f(X0,f(g(X1,X2),g(X2,X1))),
	              f(g(X1,X2),f(X0,X0)))
	     ]).




	     