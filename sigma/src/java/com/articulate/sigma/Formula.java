package com.articulate.sigma;

import java.util.*;
import java.util.regex.*;
import java.io.*;
import java.text.ParseException;

/** This code is copyright Articulate Software (c) 2003.  Some portions
copyright Teknowledge (c) 2003 and reused under the terms of the GNU license.
This software is released under the GNU Public License <http://www.gnu.org/copyleft/gpl.html>.
Users of this code also consent, by use of this code, to credit Articulate Software
and Teknowledge in any writings, briefings, publications, presentations, or 
other representations of any software which incorporates, builds on, or uses this 
code.  Please cite the following article in any publication with references:

Pease, A., (2003). The Sigma Ontology Development Environment, 
in Working Notes of the IJCAI-2003 Workshop on Ontology and Distributed Systems,
August 9, Acapulco, Mexico.
*/

/** Handle operations on an individual formula.  This includes formatting
 *  for presentation as well as pre-processing for sending to the 
 *  inference engine.
 */
public class Formula implements Comparable {

    private static boolean DEBUG = false;

    protected static final String AND    = "and";
    protected static final String OR     = "or";
    protected static final String NOT    = "not";
    protected static final String IF     = "=>";
    protected static final String IFF    = "<=>";
    protected static final String UQUANT = "forall";
    protected static final String EQUANT = "exists";
    protected static final String EQUAL  = "equal";
    protected static final String GT     = "greaterThan";
    protected static final String GTET   = "greaterThanOrEqualTo";
    protected static final String LT     = "lessThan";
    protected static final String LTET   = "lessThanOrEqualTo";

    protected static final String PLUSFN   = "AdditionFn";
    protected static final String MINUSFN  = "SubtractionFn";
    protected static final String TIMESFN  = "MultiplicationFn";
    protected static final String DIVIDEFN = "DivisionFn";
    protected static final String SKFN     = "SkFn";
    protected static final String SK_PREF = "Sk";
    protected static final String FN_SUFF = "Fn";
    protected static final String V_PREF  = "?";
    protected static final String R_PREF  = "@";
    protected static final String VX      = "?X";
    protected static final String VVAR    = "?VAR";
    protected static final String RVAR    = "@ROW";

    protected static final String LP = "(";
    protected static final String RP = ")";
    protected static final String SPACE = " ";

    protected static final String LOG_TRUE  = "TRUE";
    protected static final String LOG_FALSE = "FALSE";

    /** The SUO-KIF logical operators. */
    private static final List<String> LOGICAL_OPERATORS = Arrays.asList(UQUANT, 
                                                                        EQUANT,
                                                                        AND, 
                                                                        OR, 
                                                                        NOT, 
                                                                        IF, 
                                                                        IFF);

    /** SUO-KIF mathematical comparison predicates. */
    private static final List<String> COMPARISON_OPERATORS = Arrays.asList(EQUAL, 
                                                                           GT, 
                                                                           GTET, 
                                                                           LT, 
                                                                           LTET);

    /** The SUO-KIF mathematical functions are implemented in Vampire. */
    private static final List<String> MATH_FUNCTIONS = Arrays.asList(PLUSFN, 
                                                                     MINUSFN, 
                                                                     TIMESFN, 
                                                                     DIVIDEFN);

    /** The source file in which the formula appears. */
    public String sourceFile;   

    /** The line in the file on which the formula starts. */
    public int startLine;       

    /** The line in the file on which the formula ends. */
    public int endLine;     

    /** The length of the file in bytes at the position immediately
     *  after the end of the formula.  This value is used only for
     *  formulas entered via KB.tell().  In general, you should not
     *  count on it being set to a value other than -1L.
     */
    public long endFilePosition = -1L;

    /** The formula. */
    public String theFormula;

    /** 
     * A list of TPTP formulas (Strings) that together constitute the
     * translation of theFormula.  This member is a List, because
     * predicate variable instantiation and row variable expansion
     * might cause theFormula to expand to several TPTP formulas.
     */
    private ArrayList theTptpFormulas = null;

    /** 
     * Returns an ArrayList of the TPTP formulas (Strings) that
     * together constitute the TPTP translation of theFormula.
     *
     * @return An ArrayList of Strings, or an empty ArrayList if no
     * translations have been created or entered.
     */
    public ArrayList getTheTptpFormulas() {
        if (theTptpFormulas == null) {
            theTptpFormulas = new ArrayList();
        }                     
        return theTptpFormulas;
    }

    /** 
     * Clears theTptpFormulas if the ArrayList exists, else does
     * nothing.
     *
     * @return void
     */
    public void clearTheTptpFormulas() {
        if (theTptpFormulas != null) {
            theTptpFormulas.clear();
        }
        return;
    }

    /** A list of clausal (resolution) forms generated from this
     * Formula. 
     */
    private ArrayList theClausalForm = null;

    /** 
     * Returns a List of the clauses that together constitute the
     * resolution form of this Formula.  The list could be empty if
     * the clausal form has not yet been computed.
     *
     * @return ArrayList
     */
    public ArrayList getTheClausalForm() {

        // System.out.println("ENTER Formula.getTheClausalForm(" + this + ")");

        try {
            if (theClausalForm == null) {
                if (isNonEmptyString(theFormula)) {
                    theClausalForm = toNegAndPosLitsWithRenameInfo();
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }

        // System.out.println("  ==> " + theClausalForm);

        return theClausalForm;
    }

    /** 
     * This method clears the list of clauses that together constitute
     * the resolution form of this Formula, and can be used in
     * preparation for recomputing the clauses.
     *
     * @return void
     */
    public void clearTheClausalForm() {
        if (theClausalForm != null) {
            theClausalForm.clear();
        }
        theClausalForm = null;
    }

    /** 
     * Returns a List of List objects.  Each such object contains, in
     * turn, a pair of List objects.  In List object in a pair
     * contains Formula objects.  The Formula objects contained in the
     * first List object (0) of a pair represent negative literals
     * (antecedent conjuncts).  The Formula objects contained in the
     * second List object (1) of a pair represent positive literals
     * (consequent conjuncts).  Taken together, all of the clauses
     * constitute the resolution form of this Formula.
     *
     * @return A List of Lists.
     */
    public ArrayList getClauses() {
        ArrayList clausesWithVarMap = getTheClausalForm();
        if ((clausesWithVarMap == null) || clausesWithVarMap.isEmpty()) {
            return null;
        }
        return (ArrayList) clausesWithVarMap.get(0);
    }

    /** 
     * Returns a map of the variable renames that occurred during the
     * translation of this Formula into the clausal (resolution) form
     * accessible via this.getClauses().
     *
     * @return A Map of String (SUO-KIF variable) key-value pairs.
     */
    public HashMap getVarMap() {
        ArrayList clausesWithVarMap = getTheClausalForm();
        if ((clausesWithVarMap == null) || (clausesWithVarMap.size() < 3)) {
            return null;
        }
        return (HashMap) clausesWithVarMap.get(2);
    }

    /** 
     * Returns the variable in this Formula that corresponds to the
     * clausal form variable passed as input.
     *
     * @return A SUO-KIF variable (String), which may be just the
     * input variable.
     */
    public String getOriginalVar(String var) {
        Map varmap = getVarMap();
        if (varmap == null) {
            return var;
        }
        return getOriginalVar(var, varmap);
    }

    /** ***************************************************************
     * For any given formula, stop generating new pred var
     * instantiations and row var expansions if this threshold value
     * has been exceeded.  The default value is 2000. 
     */
    private static final int AXIOM_EXPANSION_LIMIT = 2000;

    /** ***************************************************************
     * This constant indicates the maximum predicate arity supported
     * by the current implementation of Sigma.
     */
    protected static final int MAX_PREDICATE_ARITY = 7;

    /** ***************************************************************
     * Read a String into the variable 'theFormula'.
     */
    public void read(String s) {
        theFormula = s;
    }

    /** ***************************************************************
     */
    public static String integerToPaddedString(int i, int digits) {

        String result = Integer.toString(i);
        while (result.length() < digits) {
            result = "0" + result;
        }
        return result;
    }

    /** ***************************************************************
     * Read a String into the variable 'theFormula'.
     */
    public String createID() {

        String fname = sourceFile;
        if (!DB.emptyString(fname) && fname.lastIndexOf(File.separator) > -1) 
            fname = fname.substring(fname.lastIndexOf(File.separator)+1);
        return (new Integer(theFormula.hashCode())).toString() + fname;
    }
    
    /** ***************************************************************
     * Copy the Formula.
     */
    private Formula copy() {

        Formula result = new Formula();
        if (sourceFile != null) 
            result.sourceFile = sourceFile.intern();
        result.startLine = startLine;
        result.endLine = endLine;
        if (theFormula != null) 
            result.theFormula = theFormula.intern();
        return result;
    }

    /** ***************************************************************
     * Implement the Comparable interface by defining the compareTo
     * method.  Formulas are equal if their formula strings are equal.
     */
    public int compareTo(Object f) throws ClassCastException {
        if (!f.getClass().getName().equalsIgnoreCase("com.articulate.sigma.Formula")) 
            throw new ClassCastException("Error in Formula.compareTo(): "
                                         + "Class cast exception for argument of class: " 
                                         + f.getClass().getName());
        return theFormula.compareTo(((Formula) f).theFormula);
    }

    /** ***************************************************************
     * Returns true if the Formula contains no unbalanced parentheses
     * or unbalanced quote characters, otherwise returns false.
     * 
     * @return boolean
     */
    public boolean isBalancedList() {
        boolean ans = false;
        try {
            if (this.listP()) {
                if (this.empty()) {
                    ans = true;  
                }
                else {
                    String input = this.theFormula.trim();
                    List quoteChars = Arrays.asList('"', '\'');
                    int i = 0;
                    int len = theFormula.length();
                    int end = len - 1;
                    int pLevel = 0;
                    int qLevel = 0;
                    char prev = '0';
                    char ch = prev;
                    boolean insideQuote = false;
                    char quoteCharInForce = '0';
                    while (i < len) {
                        ch = input.charAt(i);
                        if (!insideQuote) {
                            if (ch == '(') {
                                pLevel++;
                            }
                            else if (ch == ')') {
                                pLevel--;
                            }
                            else if (quoteChars.contains(ch) && (prev != '\\')) {
                                insideQuote = true;
                                quoteCharInForce = ch;
                                qLevel++;
                            }
                        }
                        else if (quoteChars.contains(ch) && (ch == quoteCharInForce) && (prev != '\\')) {
                            insideQuote = false;
                            quoteCharInForce = '0';
                            qLevel--;
                        }
                        prev = ch;
                        i++;
                    }
                    ans = ((pLevel == 0) && (qLevel == 0));
                    // System.out.println("  qLevel == " + qLevel);
                    // System.out.println("  pLevel == " + pLevel);
                }
            }
        }
        catch (Exception ex) {
            System.out.println("ERROR in Formula.isBalancedList(" 
                               + this + "): " + ex.getMessage());
            ex.printStackTrace();
        }
        return ans;
    }


    /** ***************************************************************
     * @return the LISP 'car' of the formula as a String - the first
     * element of the list. Note that this operation has no side
     * effect on the Formula.
     * 
     * Currently (10/24/2007) this method returns the empty string
     * ("") when invoked on an empty list.  Technically, this is
     * wrong.  In most LISPS, the car of the empty list is the empty
     * list (or nil).  But some parts of the Sigma code apparently
     * expect this method to return the empty string when invoked on
     * an empty list.  
     */
    public String car() {
        String ans = null;
        try {
            if (this.listP()) {
                if (this.empty()) {

                    // NS: Clean this up someday. 
                    ans = "";  // this.theFormula;
                }
                else {
                    String input = this.theFormula.trim();
                    StringBuilder sb = new StringBuilder();
                    List quoteChars = Arrays.asList('"', '\'');
                    int i = 1;
                    int len = theFormula.length();
                    int end = len - 1;
                    int level = 0;
                    char prev = '0';
                    char ch = prev;
                    boolean insideQuote = false;
                    char quoteCharInForce = '0';
                    while (i < end) {
                        ch = input.charAt(i);
                        if (!insideQuote) {
                            if (ch == '(') {
                                sb.append(ch);
                                level++;
                            }
                            else if (ch == ')') {
                                sb.append(ch);
                                level--;
                                if (level <= 0) {
                                    break;
                                }
                            }
                            else if (Character.isWhitespace(ch) && (level <= 0)) {
                                if (sb.length() > 0) {
                                    break;
                                }
                            }
                            else if (quoteChars.contains(ch) && (prev != '\\')) {
                                sb.append(ch);
                                insideQuote = true;
                                quoteCharInForce = ch;
                            }
                            else {
                                sb.append(ch);
                            }
                        }
                        else if (quoteChars.contains(ch) 
                                 && (ch == quoteCharInForce) 
                                 && (prev != '\\')) {
                            sb.append(ch);
                            insideQuote = false;
                            quoteCharInForce = '0';
                            if (level <= 0) {
                                break;
                            }
                        }
                        else {
                            sb.append(ch);
                        }
                        prev = ch;
                        i++;
                    }
                    ans = sb.toString();
                }
            }
        }
        catch (Exception ex) {
            System.out.println("ERROR in Formula.car(" + this + "): " + ex.getMessage());
            // ex.printStackTrace();
        }
        return ans;
    }

    /** ***************************************************************
     * Return the LISP 'cdr' of the formula - the rest of a list minus its
     * first element.
     * Note that this operation has no side effect on the Formula.
     */
    public String cdr() {
        String ans = null;
        try {
            if (this.listP()) {
                if (this.empty()) {
                    ans = this.theFormula;
                }
                else {
                    String input = theFormula.trim();
                    List quoteChars = Arrays.asList('"', '\'');
                    int i = 1;
                    int len = theFormula.length();
                    int end = len - 1;
                    int level = 0;
                    char prev = '0';
                    char ch = prev;
                    boolean insideQuote = false;
                    char quoteCharInForce = '0';
                    int carCount = 0;
                    while (i < end) {
                        ch = input.charAt(i);
                        if (!insideQuote) {
                            if (ch == '(') {
                                carCount++;
                                level++;
                            }
                            else if (ch == ')') {
                                carCount++;
                                level--;
                                if (level <= 0) {
                                    break;
                                }
                            }
                            else if (Character.isWhitespace(ch) && (level <= 0)) {
                                if (carCount > 0) {
                                    break;
                                }
                            }
                            else if (quoteChars.contains(ch) && (prev != '\\')) {
                                carCount++;
                                insideQuote = true;
                                quoteCharInForce = ch;
                            }
                            else {
                                carCount++;
                            }
                        }
                        else if (quoteChars.contains(ch) 
                                 && (ch == quoteCharInForce) 
                                 && (prev != '\\')) {
                            carCount++;
                            insideQuote = false;
                            quoteCharInForce = '0';
                            if (level <= 0) {
                                break;
                            }
                        }
                        else {
                            carCount++;
                        }
                        prev = ch;
                        i++;
                    }
                    if (carCount > 0) {
                        int j = i + 1;
                        if (j < end) {
                            ans = "(" + input.substring(j, end).trim() + ")";
                        }
                        else {
                            ans = "()";
                        }
                    }
                }
            }
        }
        catch (Exception ex) {
            System.out.println("ERROR in Formula.cdr(" + this + "): " + ex.getMessage());
            // ex.printStackTrace();
        }
        return ans;
    }

    /** ***************************************************************
     * Returns a new Formula which is the result of 'consing' a String
     * into this Formula, similar to the LISP procedure of the same
     * name.  This procedure is a little bit of a kluge, since this
     * Formula is treated simply as a LISP object (presumably, a LISP
     * list), and could be degenerate or malformed as a Formula.
     *
     * Note that this operation has no side effect on the original Formula.
     *
     * @param obj The String object that will become the 'car' (or
     * head) of the resulting Formula (list).
     *
     * @return a new Formula, or the original Formula if the cons fails.
     */
    private Formula cons(String obj) {
        Formula ans = this;
        try {
            String fStr = this.theFormula;
            if (isNonEmptyString(obj) && isNonEmptyString(fStr)) {
                String theNewFormula = null;
                if (this.listP()) {
                    if (this.empty()) {
                        theNewFormula = ("(" + obj + ")");
                    }
                    else {
                        theNewFormula = ("("
                                         + obj
                                         + " "
                                         + fStr.substring(1, (fStr.length() - 1))
                                         + ")");
                    }
                }
                else {
                    // This should never happen during clausification, but
                    // we include it to make this procedure behave
                    // (almost) like its LISP namesake.
                    theNewFormula = ("(" + obj + " . " + fStr + ")");
                }
                if (theNewFormula != null) {
                    ans = new Formula();
                    ans.read(theNewFormula);
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return ans;
    }

    /** ***************************************************************
     * Returns the LISP 'cdr' of the formula as a new Formula, if
     * possible, else returns null.
     *
     * Note that this operation has no side effect on the Formula.
     *
     * @return a Formula, or null.
     */
    public Formula cdrAsFormula() {
        String thisCdr = this.cdr();
        if (listP(thisCdr)) {
            Formula f = new Formula();
            f.read(thisCdr);
            return f;
        }
        return null;
    }

    /** ***************************************************************
     * Returns the LISP 'cadr' (the second list element) of the
     * formula.
     * 
     * Note that this operation has no side effect on the Formula.
     *
     * @return a String, or the empty string if the is no cadr.
     *
     */
    public String cadr() {
        return this.getArgument(1);
    }

    /** ***************************************************************
     * Returns the LISP 'cddr' of the formula - the rest of the rest,
     * or the list minus its first two elements.
     * 
     * Note that this operation has no side effect on the Formula.
     *
     * @return a String, or null.
     *
     */
    public String cddr() {
        Formula fCdr = this.cdrAsFormula();
        if (fCdr != null) {
            return fCdr.cdr();
        }
        return null;
    }

    /** ***************************************************************
     * Returns the LISP 'cddr' of the formula as a new Formula, if
     * possible, else returns null.
     *
     * Note that this operation has no side effect on the Formula.
     *
     * @return a Formula, or null.
     */
    public Formula cddrAsFormula() {
        String thisCddr = this.cddr();
        if (listP(thisCddr)) {
            Formula f = new Formula();
            f.read(thisCddr);
            return f;
        }
        return null;
    }      

    /** ***************************************************************
     * Returns the LISP 'caddr' of the formula, which is the third
     * list element of the formula.
     * 
     * Note that this operation has no side effect on the Formula.
     *
     * @return a String, or the empty string if there is no caddr.
     *
     */
    public String caddr() {
        return this.getArgument(2);
    }

    /** ***************************************************************
     * Test whether the String is a LISP atom.
     */
    public static boolean atom(String s) {

        boolean ans = false;
        if (isNonEmptyString(s)) {
            String str = s.trim();
            ans = ( (str.startsWith("\"") && str.endsWith("\""))
                    ||
                    (!str.contains(")") && !str.matches(".*\\s.*")) );
        }
        return ans;
    }

    /** ***************************************************************
     * Test whether the Formula is a LISP atom.
     */
    public boolean atom() {
     
        return Formula.atom(theFormula);
    }

    /** ***************************************************************
     * Test whether the Formula is an empty list.
     */
    public boolean empty() {

        return Formula.empty(theFormula);
    }

    /** ***************************************************************
     * Test whether the String is an empty formula.
     */
    public static boolean empty(String s) {
        return (listP(s) && s.matches("\\(\\s*\\)"));
    }

    /** ***************************************************************
     * Test whether the Formula is a list.
     */
    public boolean listP() {

        return Formula.listP(theFormula);
    }

    /** ***************************************************************
     * Test whether the String is a list.
     */
    public static boolean listP(String s) {

        boolean ans = false;
        if (isNonEmptyString(s)) {
            String str = s.trim();
            ans = (str.startsWith("(") && str.endsWith(")"));
        }
        return ans;
    }

    /** ***************************************************************
     * @see #validArgs() validArgs below for documentation
     */
    private String validArgsRecurse(Formula f, String filename, Integer lineNo) {

        //System.out.println("INFO in Formula.validArgsRecurse(): Formula: " + f.theFormula);
        if (f.theFormula == "" || !f.listP() || f.atom() || f.empty()) return "";
        String pred = f.car();
        String rest = f.cdr();
        Formula restF = new Formula();
        restF.read(rest);
        int argCount = 0;
        while (!restF.empty()) {
            argCount++;
            String arg = restF.car();
            Formula argF = new Formula();
            argF.read(arg);
            String result = validArgsRecurse(argF, filename, lineNo);
            if (result != "") 
                return result;
            restF.theFormula = restF.cdr();
        }
        if (pred.equals(AND) || pred.equals(OR)) {
            if (argCount < 2) 
                return "Too few arguments for 'and' or 'or' in formula: \n" + f.toString() + "\n";
        }
        else if (pred.equals(UQUANT) || pred.equals(EQUANT)) {
            if (argCount != 2) 
                return "Wrong number of arguments for 'exists' or 'forall' in formula: \n" + f.toString() + "\n";
            else {
                Formula quantF = new Formula();
                quantF.read(rest);
                if (!listP(quantF.car())) 
                    return "No parenthesized variable list for 'exists' or 'forall' in formula: \n" + f.toString() + "\n";
            }
        }
        else if (pred.equals(IFF) || pred.equals(IF)) {
            if (argCount != 2) 
                return "Wrong number of arguments for '<=>' or '=>' in formula: \n" + f.toString() + "\n";
        }
        else if (pred.equals(EQUAL)) {
            if (argCount != 2) 
                return "Wrong number of arguments for 'equals' in formula: \n" + f.toString() + "\n";
        }
        else if (// !(isVariable(pred)) 
                 // && 
                 (KBmanager.getMgr().getPref("holdsPrefix").equalsIgnoreCase("yes")
                  && (argCount > (MAX_PREDICATE_ARITY + 1)))
                 ||
                 (!KBmanager.getMgr().getPref("holdsPrefix").equalsIgnoreCase("yes")
                  && (argCount > MAX_PREDICATE_ARITY))) {
            String location = "";
            if ((filename != null) && (lineNo != null)) {
                location = (" near line " + lineNo + " in " + filename);
            }
            KBmanager.getMgr().setError(KBmanager.getMgr().getError()
                                        + "\n<br/>Maybe too many arguments" 
                                        + location + ": "
                                        + f.toString()
                                        + "\n<br/>");
        }
        return "";
    }

    /** ***************************************************************
     * Test whether the Formula uses logical operators and predicates
     * with the correct number of arguments.  "equals", "<=>", and
     * "=>" are strictly binary.  "or", and "and" are binary or
     * greater. "not" is unary.  "forall" and "exists" are unary with
     * an argument list.  Warn if we encounter a formula that has more
     * arguments than MAX_PREDICATE_ARITY.
     *
     * @param filename If not null, denotes the name of the file being
     * parsed.
     *
     * @param lineNo If not null, indicates the location of the
     * expression (formula) being parsed in the file being read.
     *
     * @return an empty String if there are no problems or an error message
     * if there are.
     */
    public String validArgs(String filename, Integer lineNo) {

        if (theFormula == null || theFormula == "") 
            return "";
        Formula f = new Formula();
        f.read(theFormula);
        String result = validArgsRecurse(f, filename, lineNo);
        //System.out.println("INFO in Formula.validArgs(): result: " + result);
        return result;
    }

    /** ***************************************************************
     * Test whether the Formula uses logical operators and predicates
     * with the correct number of arguments.  "equals", "<=>", and
     * "=>" are strictly binary.  "or", and "and" are binary or
     * greater. "not" is unary.  "forall" and "exists" are unary with
     * an argument list.  Warn if we encounter a formula that has more
     * arguments than MAX_PREDICATE_ARITY.
     *
     * @return an empty String if there are no problems or an error message
     * if there are.
     */
    public String validArgs() {
        return this.validArgs(null, null);
    }

    /** ***************************************************************
     * Not yet implemented!  Test whether the Formula has variables that are not properly 
     * quantified.  The case tested for is whether a quantified variable
     * in the antecedent appears in the consequent or vice versa.
     * 
     *  @return an empty String if there are no problems or an error message
     *  if there are.
     */
    public String badQuantification() {
        return "";
    }

    /** ***************************************************************
     * Parse a String into an ArrayList of Formulas. The String must be
     * a LISP-style list.
     * @return an ArrayList of Formulas
     */
    private ArrayList parseList(String s) {

        //System.out.println("INFO in Formula.parseList(): s " + s);
        ArrayList result = new ArrayList();
        Formula f = new Formula();
        f.read("(" + s + ")");
        if (f.empty()) 
            return result;
        while (!f.empty()) {
            //System.out.println("INFO in Formula.parseList(): f " + f.theFormula);
            String car = f.car();
            f.read(f.cdr());
            Formula newForm = new Formula();
            newForm.read(car);
            result.add(newForm);
        }
        return result;
    }

    /** ***************************************************************
     * Compare two lists of formulas, testing whether they are equal,
     * without regard to order.  (B A C) will be equal to (C B A). The
     * method iterates through one list, trying to find a match in the other
     * and removing it if a match is found.  If the lists are equal, the 
     * second list should be empty once the iteration is complete.
     * Note that the formulas being compared must be lists, not atoms, and
     * not a set of formulas unenclosed by parentheses.  So, "(A B C)" 
     * and "(A)" are valid, but "A" is not, nor is "A B C".
     */
    private boolean compareFormulaSets(String s) {
        // an ArrayList of Formulas
        ArrayList thisList = parseList(this.theFormula.substring(1,this.theFormula.length()-1));  
        ArrayList sList = parseList(s.substring(1,s.length()-1));
        if (thisList.size() != sList.size()) 
            return false;

        for (int i = 0; i < thisList.size(); i++) {
            for (int j = 0; j < sList.size(); j++) {
                if (((Formula) thisList.get(i)).logicallyEquals(((Formula) sList.get(j)).theFormula)) {
                    // System.out.println("INFO in Formula.compareFormulaSets(): " + 
                    //       ((Formula) thisList.get(i)).toString() + " equal to " +
                    //       ((Formula) sList.get(j)).theFormula);
                    sList.remove(j);
                    j = sList.size();
                }
            }
        }
        return sList.size() == 0;
    }

    /** ***************************************************************
     * Test if the contents of the formula are equal to the argument
     * at a deeper level than a simple string equals.  The only logical
     * manipulation is to treat conjunctions and disjunctions as unordered
     * bags of clauses. So (and A B C) will be logicallyEqual(s) for example,
     * to (and B A C).  Note that this is a fairly time-consuming operation
     * and should not generally be used for comparing large sets of formulas.
     */
    public boolean logicallyEquals(String s) {

        if (this.equals(s)) 
            return true;
        if (Formula.atom(s) && s.compareTo(theFormula) != 0) 
            return false;
    
        Formula form = new Formula();
        form.read(this.theFormula);
        Formula sform = new Formula();    
        sform.read(s);

        if (form.car().intern() == "and" || form.car().intern() == "or") {
            if (sform.car().intern() != sform.car().intern())
                return false;
            form.read(form.cdr());
            sform.read(sform.cdr());
            return form.compareFormulaSets(sform.theFormula);
        }
        else {
            Formula newForm = new Formula();
            newForm.read(form.car());
            Formula newSform = new Formula();
            newSform.read(sform.cdr());
            return newForm.logicallyEquals(sform.car()) && 
                newSform.logicallyEquals(form.cdr());
        }
    }

    /** ***************************************************************
     * Test if the contents of the formula are equal to the String argument.
     * Normalize all variables.
     */
    public boolean equals(String s) {

        String f = theFormula;
        Formula form = new Formula();
        Formula sform = new Formula();
    
        form.theFormula = f;
        s = normalizeVariables(s).intern();
        sform.read(s);
        s = sform.toString().trim().intern();

        form.theFormula = normalizeVariables(theFormula);
        f = form.toString().trim().intern();
        // System.out.println("INFO in Formula.equals(): Comparing " + s + " to " + f);
        return (f == s);
    }
    
    /** ***************************************************************
     * Test if the contents of the formula are equal to the argument.
     */
    public boolean deepEquals(Formula f) {

        return (f.theFormula.intern() == theFormula.intern()) &&
            (f.sourceFile.intern() == sourceFile.intern());
    }

    /** ***************************************************************
     * Return the numbered argument of the given formula.  The first
     * element of a formula (i.e. the predicate position) is number 0. 
     * Returns the empty string if there is no such argument position.
     */
    public String getArgument(int argnum) {

        String ans = "";
        try {
            Formula form = new Formula();
            form.read(theFormula);
            for (int i = 0 ; form.listP() ; i++) {
                ans = form.car();
                if (i == argnum) { break; }
                form.read(form.cdr());
            }
            if (ans == null) { ans = ""; }
        }
        catch (Exception ex) {
            System.out.println("ERROR in Formula.getArgument(" + this + ", " + argnum + ")");
            ex.printStackTrace();
        }
        return ans;
    }

    /** ***************************************************************
     * Returns a non-negative int value indicating the top-level list
     * length of this Formula if it is a proper listP(), else returns
     * -1.  One caveat: This method assumes that neither null nor the
     * empty string are legitimate list members in a wff.  The return
     * value is likely to be wrong if this assumption is mistaken.
     *
     * @return A non-negative int, or -1.
     */
    public int listLength() {
        int ans = -1;
        try {
            if (this.listP()) {
                int idx = 0;
                while (isNonEmptyString(this.getArgument(idx))) {
                    ans = ++idx;
                }
            }
        }
        catch (Exception ex) {
            System.out.println("ERROR in Formula.listLength(" + this + ")");
            ex.printStackTrace();
        }
        return ans;
    }

    /** ***************************************************************
     * Return all the arguments in a simple formula as a list, starting
     * at the given argument.  If formula is complex (i.e. an argument
     * is a function or sentence), then return null.  If the starting
     * argument is greater than the number of arguments, also return
     * null.
     */
    public ArrayList argumentsToArrayList(int start) {

        if (theFormula.indexOf('(',1) != -1) 
            return null;
        int index = start;
        ArrayList result = new ArrayList();
        String arg = getArgument(index);
        while (arg != null && arg != "" && arg.length() > 0) {
            result.add(arg.intern());
            index++;
            arg = getArgument(index);
        }
        if (index == start) 
            return null;
        return result;
    }

    /** ***************************************************************
     * Translate SUMO inequalities to the typical inequality symbols that 
     * the theorem prover requires.
     */
    private String translateInequalities(String s) {
    
        if (s.equalsIgnoreCase("greaterThan")) return ">";
        if (s.equalsIgnoreCase("greaterThanOrEqualTo")) return ">=";
        if (s.equalsIgnoreCase("lessThan")) return "<";
        if (s.equalsIgnoreCase("lessThanOrEqualTo")) return "<=";
        return "";
    }

    /** ***************************************************************
     * Collect all the quantified variables in the input, which
     * is the String representation of a Formula.
     *
     * @return An ArrayList of variables (Strings).
     */
    private ArrayList collectQuantifiedVariables(String theFormula) {
        ArrayList quantVariables = new ArrayList();
        int startIndex = -1;
        int tmpIndex = -1;
        int forallIndex = theFormula.indexOf("(forall (?",startIndex);
        int existsIndex = theFormula.indexOf("(exists (?",startIndex);
        int kappaFnIndex = theFormula.indexOf("(KappaFn ?",startIndex);
        while ((forallIndex != -1) || (existsIndex != -1) || (kappaFnIndex != -1)) {
            if ((forallIndex < existsIndex && forallIndex != -1) || existsIndex == -1) {
                tmpIndex = forallIndex;
            }
            else {
                tmpIndex = existsIndex;
            }
            if ((tmpIndex < kappaFnIndex && tmpIndex != -1) || kappaFnIndex == -1) {
                startIndex = tmpIndex + 9;
            }
            else {
                startIndex = kappaFnIndex + 9;
            }

            int i = startIndex;
            while ((theFormula.charAt(i) != ')') && 
                   (theFormula.charAt(i) != '(') &&
                   (i < theFormula.length())) {
                i++;
                if (theFormula.charAt(i) == ' ') {
                    if (!quantVariables.contains(theFormula.substring(startIndex,i).intern()))
                        quantVariables.add(theFormula.substring(startIndex,i));
                    //System.out.println(theFormula.substring(startIndex,i));
                    startIndex = i+1;
                }
            }
            //System.out.println(startIndex);
            //System.out.println(i);
            if (i < theFormula.length()) {
                if (!quantVariables.contains(theFormula.substring(startIndex,i).intern()))
                    quantVariables.add(theFormula.substring(startIndex,i).intern());
                //System.out.println(theFormula.substring(startIndex,i));
                startIndex = i+1;
            }
            else {
                startIndex = theFormula.length();
            }
            forallIndex = theFormula.indexOf("(forall (?",startIndex);
            existsIndex = theFormula.indexOf("(exists (?",startIndex);
            kappaFnIndex = theFormula.indexOf("(KappaFn ?",startIndex);
        }
        return quantVariables;
    }

    /** ***************************************************************
     * Collect all the quantified variables in this Formula.
     *
     * @return An ArrayList of variables (Strings).
     */
    private ArrayList collectQuantifiedVariables() {

        return this.collectQuantifiedVariables(this.theFormula);
    }

    /** ***************************************************************
     * Collect all the unquantified variables in a formula
     */
    private ArrayList collectUnquantifiedVariables(String theFormula, ArrayList quantVariables) {

        int startIndex = 0;
        ArrayList unquantVariables = new ArrayList();

        while (theFormula.indexOf(V_PREF,startIndex) != -1) {
            startIndex = theFormula.indexOf(V_PREF,startIndex);
            int spaceIndex = theFormula.indexOf(SPACE,startIndex);
            int parenIndex = theFormula.indexOf(RP,startIndex);
            int i;
            if ((spaceIndex < parenIndex && spaceIndex != -1) || parenIndex == -1) 
                i = spaceIndex;
            else
                i = parenIndex;
            if (!quantVariables.contains(theFormula.substring(startIndex,i).intern()) &&
                !unquantVariables.contains(theFormula.substring(startIndex,i).intern())) {
                unquantVariables.add(theFormula.substring(startIndex,i).intern());    
                //System.out.println(theFormula.substring(startIndex,i));
            }
            startIndex = i;
        }
        return unquantVariables;
    }

    /** ***************************************************************
     * Makes implicit quantification explicit.  
     *
     * @param query controls whether to add universal or existential
     * quantification.  If true, add existential.
     * 
     * @result the formula as a String, with explicit quantification
     */
    public String makeQuantifiersExplicit(boolean query) {
        String result = this.theFormula;
        try {
            List avoid = Arrays.asList("documentation",
                                       "comment",
                                       "localDocumentation",
                                       "externalImage",
                                       "format",
                                       "termFormat");
            String arg0 = this.car();
            if (!avoid.contains(arg0)) {
                ArrayList quantVariables = collectQuantifiedVariables(theFormula);
                ArrayList unquantVariables = collectUnquantifiedVariables(theFormula,
                                                                          quantVariables);
                if (!unquantVariables.isEmpty()) {   // Quantify all the unquantified variables
                    StringBuilder sb = new StringBuilder();
                    sb.append((query ? "(exists (" : "(forall ("));
                    boolean afterTheFirst = false;
                    for (Iterator itu = unquantVariables.iterator(); itu.hasNext();) {
                        if (afterTheFirst) sb.append(" ");
                        sb.append(itu.next().toString());
                        afterTheFirst = true;
                    }
                    //System.out.println("INFO in Formula.makeQuantifiersExplicit(): result: " + 
                    //    quant.toString() + ") " + theFormula + ")");
                    sb.append(") ");
                    sb.append(this.theFormula);
                    sb.append(")");
                    result = sb.toString();
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return result;
    }

    /** ***************************************************************
     *
     * @param kb - The KB used to compute variable arity relations.
     *
     * @return Returns true if this Formula contains any variable
     * arity relations, else returns false.
     */
    protected boolean containsVariableArityRelation(KB kb) {

        boolean ans = false;
        try {
            Set relns = kb.getCachedRelationValues("instance", "VariableArityRelation", 2, 1);
            if (relns == null)
                relns = new HashSet();                 
            relns.addAll(KB.VA_RELNS);
            String r = null;
            Iterator it = relns.iterator();
            while (it.hasNext()) {
                r = (String) it.next();
                ans = (this.theFormula.indexOf(r) != -1);
                if (ans) { break; }
            }            
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return ans;
    }

    /** ***************************************************************
     *
     * @param kb - The KB used to compute variable arity relations.
     *
     * @return A new version of the Formula in which every
     * VariableArityRelation has been renamed to include a numeric
     * suffix corresponding to the actual number of arguments in the
     * Formula.
     */
    protected Formula renameVariableArityRelations(KB kb) {
        Formula result = this;
        try {
            if (this.listP()) {
                StringBuilder sb = new StringBuilder();
                Formula f = new Formula();
                f.read(this.theFormula);
                int flen = f.listLength();
                String suffix = ("_" + (flen - 1));
                String arg = null;
                sb.append("(");
                for (int i = 0 ; i < flen ; i++) {
                    arg = f.getArgument(i);
                    if (i > 0) {
                        sb.append(" ");
                    }
                    if ((i == 0)
                        && kb.isVariableArityRelation(arg) 
                        && !(arg.endsWith(suffix))) {
                        arg += suffix;
                    }
                    else if (listP(arg)) {
                        Formula argF = new Formula();
                        argF.read(arg);
                        arg = argF.renameVariableArityRelations(kb).theFormula;
                    }
                    sb.append(arg);
                }
                sb.append(")");
                f = new Formula();
                f.read(sb.toString());
                result = f;
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return result;
    }

    /** ***************************************************************
     * Find all the row variables in an input String.
     * @return a TreeSet of row variable names, without their '@' designator
     * */
    private TreeSet findRowVars(String input) {

        Formula f = new Formula();
        f.read(input);
        return f.findRowVars();
    }

    /** ***************************************************************
     * Find all the row variables in this Formula.  
     * @return a TreeSet of row variable names, without their '@' designator
     * */
    private TreeSet findRowVars() {

        TreeSet result = new TreeSet();
        int i = 0;
        Formula f = new Formula();
        f.read(this.theFormula);

        while (f.listP() && !f.empty()) {
            String arg = f.car();
            if (arg.charAt(0) == '@') 
                result.add(arg.substring(1));
            else {
                Formula argF = new Formula();
                argF.read(arg);
                if (argF.listP()) 
                    result.addAll(argF.findRowVars());
            }
            f.read(f.cdr());
        }
        return result;
    }

    /** ***************************************************************
     * Expand row variables, keeping the information about the original
     * source formula.  Each variable is treated like a macro that
     * expands to up to seven regular variables.  For example
     *
     * (=>
     *    (and
     *       (subrelation ?REL1 ?REL2)
     *       (holds__ ?REL1 @ROW))
     *    (holds__ ?REL2 @ROW))
     *
     * would become 
     *
     * (=>
     *    (and
     *       (subrelation ?REL1 ?REL2)
     *       (holds__ ?REL1 ?ARG1))
     *    (holds__ ?REL2 ?ARG1))
     * 
     * (=>
     *    (and
     *       (subrelation ?REL1 ?REL2)
     *       (holds__ ?REL1 ?ARG1 ?ARG2))
     *    (holds__ ?REL2 ?ARG1 ?ARG2))
     * etc.
     * 
     * @return an ArrayList of Formulas, or an empty ArrayList.
     */
    protected ArrayList expandRowVars(KB kb) {

        /*
          boolean trace = this.theFormula.contains("immediateSubclass");
          if (trace) {
          System.out.println("ENTER Formula.expandRowVars(" 
          + this.theFormula + ", "
          + kb.name + ")");
          }
        */

        ArrayList resultList = new ArrayList();
        try {

            // This is a kluge, but right here is the best place to
            // perform this check.  Continue only if this is not an
            // obvious tautology.  If the test immediately below
            // fails, this method will just return an empty List.
            if (!this.isRule() || !this.getArgument(1).equals(this.getArgument(2))) {

                Formula f = new Formula();
                f.read(this.theFormula);
                if (f.theFormula.indexOf('@') == -1) {
                    f.sourceFile = this.sourceFile;
                    resultList.add(f);
                }
                else {
                    TreeSet rowVars = f.findRowVars();
                    Iterator it = rowVars.iterator();
                    StringBuilder result = new StringBuilder(f.theFormula);
                    long t1 = 0L;

                    // Iterate through the row variables
                    while (it.hasNext()) {
                        String row = (String) it.next();

                        t1 = System.currentTimeMillis();
                        int[] range = this.getRowVarExpansionRange(kb, row);

                        /*
                          if (trace) {
                          if (range != null) {
                          System.out.print("  range ==");
                          for (int ri = 0; ri < range.length; ri++) {
                          if (ri > 0) System.out.print(",");
                          System.out.print(" " + range[ri]);
                          }
                          System.out.println("");
                          }
                          }
                        */

                        // Increment the timer for getRowVarExpansionRange().
                        KB.ppTimers[3] += (System.currentTimeMillis() - t1);

                        boolean hasVariableArityRelation = (range[0] == 0);

                        t1 = System.currentTimeMillis();
                        range[1] = adjustExpansionCount(hasVariableArityRelation, range[1], row);
                        // Increment the timer for adjustExpansionCount().
                        KB.ppTimers[5] += (System.currentTimeMillis() - t1);

                        StringBuilder rowResult = new StringBuilder();
                        StringBuilder rowReplace = new StringBuilder();
                        for (int j = 1 ; j < range[1] ; j++) {
                            if (rowReplace.toString().length() > 0) {
                                rowReplace = rowReplace.append(" ");
                            }
                            rowReplace = rowReplace.append("?" + row + (new Integer(j)).toString());
                            if (hasVariableArityRelation) {
                                rowResult = 
                                    rowResult.append(result.toString().replaceAll("\\@" + row, 
                                                                                  rowReplace.toString()) + "\n");
                            }
                        }
                        if (!hasVariableArityRelation) {
                            rowResult = 
                                rowResult.append(result.toString().replaceAll("\\@" + row, 
                                                                              rowReplace.toString()) + "\n");
                        }
                        result = new StringBuilder(rowResult.toString());
                    }
                    ArrayList al = parseList(result.toString());

                    // System.out.println("INFO in Formula.expandRowVars(" + this + ")");
                    // System.out.println("  al == " + al);

                    Formula newF = null;
                    for (int i = 0; i < al.size(); i++) {
                        newF = (Formula) al.get(i);
                        // Copy the source file information for each expanded formula.
                        newF.sourceFile = this.sourceFile;
                        resultList.add(newF);
                    }
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }

        /*
          if (trace) {
          System.out.println("EXIT Formula.expandRowVars("
          + this.theFormula + ", "
          + kb.name + ")");
          System.out.println("  ==> " + resultList);
          }
        */

        return resultList;
    }

    /** ***************************************************************
     * This method attempts to revise the number of row var expansions
     * to be done, based on the occurrence of forms such as (<pred>
     * @ROW1 ?ITEM).  Note that variables such as ?ITEM throw off the
     * default expected expansion count, and so must be dealt with to
     * prevent unnecessary expansions.
     *
     * @param variableArity Indicates whether the overall expansion
     * count for the Formula is governed by a variable arity relation,
     * or not.
     *
     * @param count The default expected expansion count, possibly to
     * be revised.
     *
     * @param var The row variable to be expanded.
     *
     * @return An int value, the revised expansion count.  In most
     * cases, the count will not change.
     *
     */
    private int adjustExpansionCount(boolean variableArity, int count, String var) {

        // System.out.println("INFO in Formula.adjustExpansionCount(" + this + " ...)");

        int revisedCount = count;
        try {
            if (isNonEmptyString(var)) {
                String rowVar = var;
                if (! var.startsWith("@")) {
                    rowVar = ("@" + var);
                }
                List accumulator = new ArrayList();
                List working = new ArrayList();
                if (this.listP() && !this.empty()) {
                    accumulator.add(this);
                }
                while (!(accumulator.isEmpty())) {
                    working.clear();
                    working.addAll(accumulator);
                    accumulator.clear();
                    for (int i = 0 ; i < working.size() ; i++) {
                        Formula f = (Formula) working.get(i);
                        List literal = f.literalToArrayList();

                        // System.out.println(literal);

                        int len = literal.size();
                        if (literal.contains(rowVar) && !(isVariable(f.car()))) {
                            if (!variableArity && (len > 2)) {
                                revisedCount = (count - (len - 2));
                            }
                            else if (variableArity) {
                                revisedCount = (10 - len);
                            }
                        }
                        if (revisedCount < 2) {
                            revisedCount = 2;
                        }
                        while (!(f.empty())) {
                            String arg = f.car();
                            Formula argF = new Formula();
                            argF.read(arg);
                            if (argF.listP() && !(argF.empty())) {
                                accumulator.add(argF);
                            }
                            f = f.cdrAsFormula();
                        }
                    }
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        // System.out.println("    -> " + revisedCount);
        return revisedCount;
    }

    /** ***************************************************************
     * Returns a two-place int[] indicating the low and high points of
     * the expansion range (number of row var instances) for the input
     * row var.
     *
     * @param kb A KB required for processing.
     *
     * @param rowVar The row var (String) to be expanded.
     *
     * @return A two-place int[] object.  The int[] indicates a
     * numeric range.  int[0] holds the start (lowest number) in the
     * range, and int[1] holds the highest number.  The default is
     * [1,8].  If the Formula does not contain
     *
     */
    private int[] getRowVarExpansionRange(KB kb, String rowVar) {

        // System.out.println("INFO in Formula.getRowVarExpansionRange(" + this + " ...)");

        int[] ans = new int[2];
        ans[0] = 1;
        ans[1] = 8;
        try {
            if (isNonEmptyString(rowVar)) {
                String var = rowVar;
                if (!var.startsWith("@")) {
                    var = "@" + var;
                }
                Map minMaxMap = this.getRowVarsMinMax(kb);
                int[] newArr = (int[]) minMaxMap.get(var);
                if (newArr != null) {
                    ans = newArr;
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        // System.out.print("INFO in getRowVarExpansionRange(" + this + ", " + kb + ", " + rowVar + ")");
        // System.out.println(" -> [" + ans[0] + "," + ans[1] + "]");
        return ans;
    }

    /** ***************************************************************
     * Applied to a SUO-KIF Formula with row variables, this method
     * returns a Map containing an int[] of length 2 for each row var
     * that indicates the minimum and maximum number of row var
     * expansions to perform.
     *
     * @param kb A KB required for processing.
     *
     * @return A Map in which the keys are distinct row variables and
     * the values are two-place int[] objects.  The int[] indicates a
     * numeric range.  int[0] is the start (lowest number) in the
     * range, and int[1] is the end.  If the Formula contains no row
     * vars, the Map is empty.
     *
     */
    private Map getRowVarsMinMax(KB kb) {

        /*
          boolean trace = this.theFormula.contains("(immediateSubclass");
          if (trace) {
          System.out.println("ENTER Formula.getRowVarsMinMax(" 
          + this.theFormula + ", " 
          + kb.name + ")");
          }
        */

        Map ans = new HashMap();
        try {

            long t1 = System.currentTimeMillis();
            ArrayList clauseData = this.getTheClausalForm();
          
            /*  
                if (trace) {
                System.out.println("  getTheClausalForm() == " + clauseData);
                }
            */

            // Increment the timer for toNegAndPosLitsWithRenameInfo().
            KB.ppTimers[4] += (System.currentTimeMillis() - t1);

            if (!((clauseData instanceof ArrayList) && (clauseData.size() > 2))) {
                return ans;
            }

            // System.out.println();
            // System.out.println("clauseData == " + clauseData);
            // System.out.println();

            ArrayList clauses = (ArrayList) clauseData.get(0);

            // System.out.println();
            // System.out.println("clauses == " + clauses);
            // System.out.println("clauses.size() == " + clauses.size());
            // System.out.println();

            if (!(clauses instanceof ArrayList) || clauses.isEmpty()) {
                return ans;
            }

            Map varMap = (Map) clauseData.get(2);
            Map rowVarRelns = new HashMap();
            for (int i = 0 ; i < clauses.size() ; i++) {
                ArrayList clause = (ArrayList) clauses.get(i);

                // if (trace) {
                //     System.out.println("  clause == " + clause);
                // }

                if ((clause != null) && !clause.isEmpty()) {
 
                    // First we get the neg lits.  It may be that
                    // we should use *only* the neg lits for this
                    // task, but we will start by combining the neg
                    // lits and pos lits into one list of literals
                    // and see how that works.
                    ArrayList literals = (ArrayList) clause.get(0);
                    ArrayList posLits = (ArrayList) clause.get(1);
                    literals.addAll(posLits);

                    // if (trace) {
                    //     System.out.println("  literals == " + literals);
                    // }

                    for (int j = 0 ; j < literals.size() ; j++) {
                        Formula litF = (Formula) literals.get(j);
                        litF.computeRowVarsWithRelations(rowVarRelns, varMap);
                    }
                }

                // if (trace) {
                //     System.out.println("  rowVarRelns == " + rowVarRelns);
                // }

                if (!rowVarRelns.isEmpty()) {
                    Iterator kit = rowVarRelns.keySet().iterator();
                    while (kit.hasNext()) {
                        String rowVar = (String) kit.next();
                        String origRowVar = getOriginalVar(rowVar, varMap);
                        int[] minMax = (int[]) ans.get(origRowVar);
                        if (minMax == null) {
                            minMax = new int[ 2 ];
                            minMax[0] = 0;
                            minMax[1] = 8;
                            ans.put(origRowVar, minMax);
                        }
                        TreeSet val = (TreeSet) rowVarRelns.get(rowVar);
                        Iterator vit = val.iterator();
                        while (vit.hasNext()) {
                            String reln = (String) vit.next();
                            int arity = kb.getValence(reln);
                            if (arity < 1) {
                                // It's a VariableArityRelation or we
                                // can't find an arity, so do nothing.
                                ;
                            }
                            else {
                                minMax[0] = 1;
                                if ((arity + 1) < minMax[1]) {
                                    minMax[1] = (arity + 1);
                                }
                            }
                        }
                    }
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }

        /*
          if (trace) {
          System.out.println("EXIT Formula.getRowVarsMinMax(" 
          + this.theFormula + ", " 
          + kb.name + ")");
          System.out.println("  ==> " + ans);
          }
        */

        return ans;
    }

    /** ***************************************************************
     * Finds all the relations in this Formula that are applied to row
     * variables, and a specific arity might be computed.  Note that
     * results are accumulated in varsToRelns, and the variable
     * correspondences (if any) in varsToVars are used to compute the
     * results.
     *
     * @param varsToRelns A Map for accumulating row var data for one
     * Formula literal.  The keys are row variables (Strings) and the
     * values are TreeSets containing relations (Strings) that might
     * help to constrain the row var during row var expansion.
     * 
     * @param varsToVars A Map of variable correspondences, the leaves
     * of which might include row variables
     *
     * @return void
     * */
    protected void computeRowVarsWithRelations(Map varsToRelns, Map varsToVars) {
        try {
            Formula f = this;
            if (f.listP() && !f.empty()) {
                String relation = f.car();
                if (!(isVariable(relation) || relation.equals("SkFn"))) {
                    Formula newF = f.cdrAsFormula();
                    while (newF.listP() && !newF.empty()) {
                        String term = newF.car();
                        String rowVar = term;
                        if (isVariable(rowVar)) {
                            if (rowVar.startsWith("?") && (varsToVars != null)) {
                                rowVar = getOriginalVar(term, varsToVars);
                            }
                        }
                        if (rowVar.startsWith("@")) {
                            TreeSet relns = (TreeSet) varsToRelns.get(term);
                            if (relns == null) {
                                relns = new TreeSet();
                                varsToRelns.put(term, relns);
                                varsToRelns.put(rowVar, relns);
                            }
                            relns.add(relation);
                        }
                        else {
                            Formula termF = new Formula();
                            termF.read(term);
                            termF.computeRowVarsWithRelations(varsToRelns, varsToVars);
                        }
                        newF = newF.cdrAsFormula();
                    }
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return;
    }


    /** ***************************************************************
     * Convert an ArrayList of Formulas to an ArrayList of Strings.
     */
    private ArrayList formulasToStrings(ArrayList list) {

        ArrayList result = new ArrayList();
        for (int i = 0; i < list.size(); i++) {
            result.add(((Formula) list.get(i)).theFormula);
        }
        return result;
    }

    /** ***************************************************************
     * Test whether a Formula is a functional term
     */
    private boolean isFunctionalTerm() {
        boolean ans = false;
        if (this.listP()) {
            String pred = this.car();
            ans = ((pred.length() > 2) && pred.endsWith(FN_SUFF));
        }
        return ans;
    }

    /** ***************************************************************
     * Test whether an Object is a variable
     */
    public static boolean isVariable(Object term) {

        return (isNonEmptyString(term)
                && (((String)term).startsWith(V_PREF) 
                    || ((String)term).startsWith(R_PREF)));
    }

    /** ***************************************************************
     * Returns true only if this Formula starts with "=>" or "<=>",
     * else returns false.  It would be better to test for the
     * occurrence of at least one positive literal with one or more
     * negative literals, but this test would require converting the
     * Formula to clausal form.
     */
    private boolean isRule() {
        return (this.listP() && Arrays.asList(IF, IFF).contains(this.car()));
    }

    /** ***************************************************************
     * Test whether a list with a predicate is a quantifier list
     */
    private static boolean isQuantifierList(String listPred, String previousPred) {

        return ((previousPred.equals(EQUANT) || previousPred.equals(UQUANT)) &&
                (listPred.startsWith(R_PREF) || listPred.startsWith(V_PREF)));
    }

    /** ***************************************************************
     * Test whether a predicate is a logical quantifier
     */
    public static boolean isQuantifier(String pred) {

        return (isNonEmptyString(pred)
                && (pred.equals(EQUANT) 
                    || pred.equals(UQUANT)));
    }

    /** ***************************************************************
     * A static utility method.
     * @param obj Any object, but should be a String.
     * @return true if obj is a SUO-KIF commutative logical operator,
     * else false.
     */
    public static boolean isCommutative(String obj) {

        return (isNonEmptyString(obj)
                && (obj.equals(AND) 
                    || obj.equals(OR)));
    }

    /** ***************************************************************
     * Returns the dual logical operator of op, or null if op is not
     * an operator or has no dual.
     *
     * @param term A String, assumed to be a SUO-KIF logical operator
     *
     * @return A String, the dual operator of op, or null.
     */
    protected static String getDualOperator(String op) {
        String ans = null;
        if (op instanceof String) {
            String[][] duals = { { UQUANT, EQUANT },
                                 { EQUANT, UQUANT },
                                 { AND,    OR     },
                                 { OR,     AND    },
                                 { NOT,    ""     },
                                 { "",     NOT    },
                                 { LOG_TRUE,  LOG_FALSE  },
                                 { LOG_FALSE, LOG_TRUE   }
            };
            for (int i = 0; i < duals.length; i++) {
                if (op.equals(duals[i][0])) ans = duals[i][1];
            }
        }
        return ans;
    }

    /** ***************************************************************
     * Returns true if term is a standard FOL logical operator, else
     * returns false.
     *
     * @param term A String, assumed to be an atomic SUO-KIF term.
     */
    public static boolean isLogicalOperator(String term) {

        return (isNonEmptyString(term) && LOGICAL_OPERATORS.contains(term));
    }

    /** ***************************************************************
     * Returns true if term is a SUO-KIF predicate for comparing two
     * (typically numeric) terms, else returns false.
     *
     * @param term A String.
     */
    public static boolean isComparisonOperator(String term) {

        return (isNonEmptyString(term) && COMPARISON_OPERATORS.contains(term));
    }

    /** ***************************************************************
     * Returns true if term is a SUO-KIF mathematical function, else
     * returns false.
     *
     * @param term A String.
     */
    public static boolean isMathFunction(String term) {

        return (isNonEmptyString(term) && MATH_FUNCTIONS.contains(term));
    }

    /** ***************************************************************
     * Returns true if term is a SUO-KIF function, else returns false.
     * Note that this test is purely syntactic, and could fail for
     * functions that do not adhere to the convention of ending all
     * functions with "Fn".
     *
     * @param term A String.
     */
    public static boolean isFunction(String term) {
        return (isNonEmptyString(term) && term.endsWith(FN_SUFF));
    }

    /** ***************************************************************
     * Returns true if term is a SUO-KIF Skolem term, else returns false.
     *
     * @param term A String.
     *
     * @return true or false
     */
    public static boolean isSkolemTerm(String term) {
        return (isNonEmptyString(term)
                && term.trim().matches("^.?" + SK_PREF + "\\S*\\s*\\d+"));
    }

    /** ***************************************************************
     * @param obj Any object
     * @return true if obj is a non-empty String, else false.
     */
    public static boolean isNonEmptyString(Object obj) {
        return StringUtil.isNonEmptyString(obj);
    }

    /** ***************************************************************
     * @return An ArrayList (ordered tuple) representation of the
     * Formula, in which each top-level element of the Formula is
     * either an atom (String) or another list.
     */
    public ArrayList literalToArrayList() {
        ArrayList tuple = new ArrayList();
        try {
            Formula f = this;
            if (f.listP()) {
                while (!f.empty()) {
                    tuple.add(f.car());
                    f = f.cdrAsFormula();
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return tuple;
    }

    /** ***************************************************************
     * This method returns all SUO-KIF variables that occur in the
     * Formula.
     *
     * @see gatherVariables(TreeSet accumulator)
     * 
     * @return A TreeSet containing variables (Strings), or an empty
     * TreeSet if no variables can be found.
     */
    private TreeSet gatherVariables() {
        return this.gatherVariables(null);
    }

    /** ***************************************************************
     * @see gatherVariables()
     *
     * @param accumulator A TreeSet used for storing variables
     * (Strings).
     * 
     * @return A TreeSet containing variables (Strings), or an empty
     * TreeSet if no variables can be found.
     */
    private TreeSet gatherVariables(TreeSet accumulator) {
        if (accumulator == null) {
            accumulator = new TreeSet();
        }
        try {
            if (this.listP() && !this.empty()) {
                String arg0 = this.car();
                Formula arg0F = new Formula();
                arg0F.read(arg0);
                arg0F.gatherVariables(accumulator);
                this.cdrAsFormula().gatherVariables(accumulator);
            }
            else if (isVariable(this.theFormula)) {
                accumulator.add(this.theFormula);
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return accumulator;
    }

    /** ***************************************************************
     * A + is appended to the type if the parameter must be a class
     *
     * @return the type for each argument to the given predicate, where
     * ArrayList element 0 is the result, if a function, 1 is the 
     * first argument, 2 is the second etc.
     */
    private ArrayList getTypeList(String pred, KB kb) {

        //System.out.println("INFO in Formula.getTypeList(): pred: " + pred);
        ArrayList result = null;
        try {
            if (kb.sortalTypeCache != null) {
                String key = "gtl" + pred + kb.name;
                result = ((ArrayList) (kb.sortalTypeCache.get(key)));
                if (result != null) {
                    return result;
                }
                else {
                    result = new ArrayList();
                    kb.sortalTypeCache.put(key, result);
                }
            }
            else {
                result = new ArrayList();
            }
            int valence = kb.getValence(pred);
            int len = MAX_PREDICATE_ARITY + 1;
            if (valence == 0) {
                len = 2;
            }
            else if (valence > 0) {
                len = valence + 1;
            }
            String[] r = new String[len];

            ArrayList al = kb.askWithRestriction(0,"domain",1,pred);
            ArrayList al2 = kb.askWithRestriction(0,"domainSubclass",1,pred);
            ArrayList al3 = kb.askWithRestriction(0,"range",1,pred);
            ArrayList al4 = kb.askWithRestriction(0,"rangeSubclass",1,pred);
            r = addToTypeList(pred,al,r,false);
            r = addToTypeList(pred,al2,r,true);
            r = addToTypeList(pred,al3,r,false);
            r = addToTypeList(pred,al4,r,true);
            for (int i = 0; i < r.length; i++) 
                result.add(r[i]);
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return result;
    }

    /** ***************************************************************
     * A utility helper method for computing predicate data types.
     */
    private String[] addToTypeList(String pred, ArrayList al, String[] result, boolean classP) {
        try {
            Formula f = null;
            // If the relations in al start with "(range", argnum will
            // be 0, and the arg position of the desired classnames
            // will be 2.
            int argnum = 0;
            int clPos = 2;
            for (int i = 0; i < al.size(); i++) {
                f = (Formula) al.get(i);
                //System.out.println("INFO in addToTypeList(): formula: " + f.theFormula);
                if (f.theFormula.startsWith("(domain")) {
                    argnum = Integer.parseInt(f.getArgument(2));
                    clPos = 3;
                }
                String cl = f.getArgument(clPos);
                String errStr = null;
                String mgrErrStr = null;
                if ((argnum < 0) || (argnum >= result.length)) {
                    errStr = "Possible arity confusion for " + pred;
                    mgrErrStr = KBmanager.getMgr().getError();
                    System.out.println("WARNING in Formula.addToTypeList(): "
                                       + errStr + ": al == " + al + ", result.length == " + result.length 
                                       + ", classP == \"" + classP + "\"");
                    if (mgrErrStr.equals("") || (mgrErrStr.indexOf(errStr) == -1)) {
                        KBmanager.getMgr().setError(mgrErrStr + "\n<br/>" + errStr + "\n<br/>");
                    }
                }
                else if (StringUtil.emptyString(result[argnum])) {
                    if (classP) { cl += "+"; }
                    result[argnum] = cl;
                }
                else {
                    if (!cl.equals(result[argnum])) {
                        errStr = ("Multiple types asserted for argument " + argnum 
                                  + " of " + pred + ": " + cl + ", " + result[argnum]);
                        mgrErrStr = KBmanager.getMgr().getError();
                        System.out.println("Error in Formula.addToTypeList(): " + errStr);
                        if (mgrErrStr.equals("") || (mgrErrStr.indexOf(errStr) == -1)) {
                            KBmanager.getMgr().setError(KBmanager.getMgr().getError() 
                                                        + "\n<br/>" + errStr + "\n<br/>");
                        }
                    }
                }
            }
        }
        catch (Exception ex) {
            System.out.println(ex.getMessage());
            ex.printStackTrace();
        }
        return result;
    }

    /** ***************************************************************
     * Find the argument type restriction for a given predicate and
     * argument number that is inherited from one of its
     * super-relations.  A "+" is appended to the type if the
     * parameter must be a class.  Argument number 0 is used for the
     * return type of a Function.
     */
    private String findType(int numarg, String pred, KB kb) {

        // System.out.println("INFO in Formula.findType(" + numarg + ", " + pred + ")");

        if (DEBUG) {
            System.out.println("ENTER findType(" + numarg + ", " + pred + ", " + kb.name + ")");
        }

        String result = null;
        boolean isCached = false;
        boolean cacheResult = false;
        try {
            String key = null;
            if (kb.sortalTypeCache != null) {
                key = "ft" + numarg + pred + kb.name;
                result = ((String) (kb.sortalTypeCache.get(key)));
                isCached = (result != null);
                cacheResult = !isCached;
            }
            if (result == null) {
                boolean found = false;
                Set accumulator = new HashSet();
                accumulator.add(pred);
                List parents = new ArrayList();
                Iterator it = null;
                String newPred = null;
                while (!found && !accumulator.isEmpty()) {
                    parents.clear();
                    parents.addAll(accumulator);
                    accumulator.clear();
                    List axioms = null;
                    Formula f = null;
                    it = parents.iterator();
                    while (!found && it.hasNext()) {
                        newPred = (String) it.next();
                        if (numarg > 0) {
                            axioms = kb.askWithRestriction(0,"domain",1,newPred);
                            for (int j = 0; j < axioms.size(); j++) {
                                f = (Formula) axioms.get(j);
                                int argnum = Integer.parseInt(f.getArgument(2));
                                if (argnum == numarg) {
                                    result = f.getArgument(3);
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                axioms = kb.askWithRestriction(0,"domainSubclass",1,newPred);
                                for (int j = 0; j < axioms.size(); j++) {
                                    f = (Formula) axioms.get(j);
                                    int argnum = Integer.parseInt(f.getArgument(2));
                                    if (argnum == numarg) {
                                        result = f.getArgument(3) + "+";
                                        found = true;
                                        break;
                                    }
                                }
                            }
                        }
                        else if (numarg == 0) {
                            axioms = kb.askWithRestriction(0,"range",1,newPred);
                            if (!axioms.isEmpty()) {
                                f = (Formula) axioms.get(0);
                                result = f.getArgument(2);
                                found = true;
                            }
                            if (!found) {
                                axioms = kb.askWithRestriction(0,"rangeSubclass",1,newPred);
                                if (!axioms.isEmpty()) {
                                    f = (Formula) axioms.get(0);
                                    result = f.getArgument(2) + "+";
                                    found = true;
                                }
                            }
                        }
                        if (!found) {
                            Set newParents = kb.getCachedRelationValues("subrelation",newPred,1,2);
                            if ((newParents != null) && !newParents.isEmpty()) {
                                accumulator.addAll(newParents);
                                accumulator.remove(newPred);
                            }
                        }
                    }
                }
                if (cacheResult && (kb.sortalTypeCache != null)) {
                    kb.sortalTypeCache.put(key, result);
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }

        if (DEBUG) {
            System.out.println("EXIT findType(" + numarg + ", " + pred + ", " + kb.name + ")");
            System.out.println("  "
                               + (isCached ? "cached " : "")
                               + "result == " 
                               + result);
        }
        return result;
    }

    /** ***************************************************************
     * This method tries to remove all but the most specific relevant
     * classes from a List of sortal classes.
     *
     * @param types A List of classes (class name Strings) that
     * constrain the value of a SUO-KIF variable.
     *
     * @param kb The KB used to determine if any of the classes in the
     * List types are redundant.
     *
     * @return void
     */
    private void winnowTypeList(List types, KB kb) {

        long t1 = 0L;
        if (DEBUG) {
            t1 = System.currentTimeMillis();
            System.out.println("ENTER winnowTypeList(" + types + ", " + kb.name + ")");
        }

        try {
            if ((types instanceof List) && (types.size() > 1)) {
                Object[] valArr = types.toArray();
                String clX = null;
                String clY = null;
                for (int i = 0; i < valArr.length; i++) {
                    boolean stop = false;
                    for (int j = 0; j < valArr.length; j++) {
                        if (i != j) {
                            clX = (String) valArr[i];
                            clY = (String) valArr[j];
                            if (kb.isSubclass(clX, clY)) {
                                types.remove(clY);
                                if (types.size() < 2) {
                                    stop = true;
                                    break;
                                }
                            }
                        }
                    }
                    if (stop) break;
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }

        if (DEBUG) {
            System.out.println("EXIT winnowTypeList(" + types + ", " + kb.name + ")");
            System.out.println("  time == " 
                               + ((System.currentTimeMillis() - t1) / 1000.0)
                               + " seconds");
        }

        return;
    }

    /** ***************************************************************
     * Does much of the real work for addTypeRestrictions() by
     * recursing through the Formula and collecting type constraint
     * information for the variable var.
     *
     * @param ios A List of classes (class name Strings) of which any
     * binding for var must be an instance.
     *
     * @param scs A List of classes (class name Strings) of which any
     * binding for var must be a subclass.
     *
     * @param var A SUO-KIF variable.
     *
     * @param kb The KB used to determine predicate and variable arg
     * types.
     *
     * @return void
     */
    private void computeTypeRestrictions(List ios, List scs, String var, KB kb) {

        long t1 = 0L;
        if (DEBUG) {
            t1 = System.currentTimeMillis();
            System.out.println("ENTER computeTypeRestrictions(" 
                               + this + ", "
                               + ios + ", "
                               + scs + ", "
                               + var + ", "
                               + kb.name + ")");
        }

        String pred = null;
        try {
            if (!this.listP() || !this.theFormula.contains(var)) 
                return;
            Formula f = new Formula();
            f.read(this.theFormula);
            pred = f.car();
            if (isQuantifier(pred)) {
                String arg2 = f.getArgument(2);
                if (arg2.contains(var)) {
                    Formula nextF = new Formula();
                    nextF.read(arg2);
                    nextF.computeTypeRestrictions(ios, scs, var, kb);
                }
            }
            else if (isLogicalOperator(pred)) {
                int len = f.listLength();
                for (int i = 1; i < len; i++) {
                    String argI = f.getArgument(i);
                    if (argI.contains(var)) {
                        Formula nextF = new Formula();
                        nextF.read(argI);
                        nextF.computeTypeRestrictions(ios, scs, var, kb);
                    }
                }
            }
            else {
                int len = f.listLength();
                int valence = kb.getValence(pred);
                List types = getTypeList(pred,kb);
                int numarg = 0;
                for (int i = 1; i < len; i++) {
                    numarg = i;
                    if (valence == 0) { // pred is a VariableArityRelation
                        numarg = 1;
                    }
                    String arg = f.getArgument(i);
                    if (arg.contains(var)) {
                        if (listP(arg)) {
                            Formula nextF = new Formula();
                            nextF.read(arg);
                            nextF.computeTypeRestrictions(ios, scs, var, kb);
                        }
                        else if (var.equals(arg)) {
                            String type = null;
                            if (numarg < types.size()) {
                                type = (String) types.get(numarg);
                            }
                            if (type == null) {
                                type = findType(numarg,pred,kb);
                            }
                            if (StringUtil.isNonEmptyString(type) && !type.startsWith("Entity")) {
                                boolean sc = false;
                                while (type.endsWith("+")) {
                                    sc = true;
                                    type = type.substring(0, type.length() - 1);
                                }
                                if (sc) {
                                    if (!scs.contains(type)) {
                                        scs.add(type);
                                    }
                                }
                                else if (!ios.contains(type)) {
                                    ios.add(type);
                                }
                            }
                        }
                    }
                }

                String arg1 = null;
                String arg2 = null;
                String term = null;
                String cl = null;

                // Special treatment for equal
                if (pred.equals("equal")) {
                    arg1 = f.getArgument(1);
                    arg2 = f.getArgument(2);
                    if (var.equals(arg1)) { term = arg2; }
                    else if (var.equals(arg2)) { term = arg1; }
                    if (isNonEmptyString(term)) {
                        if (listP(term)) {
                            Formula nextF = new Formula();
                            nextF.read(term);
                            if (nextF.isFunctionalTerm()) {
                                String fn = nextF.car();
                                List classes = getTypeList(fn, kb);
                                if (!classes.isEmpty()) {
                                    cl = (String) classes.get(0);
                                }
                                if (cl == null) {
                                    cl = findType(0, fn, kb);
                                }
                                if (StringUtil.isNonEmptyString(cl) && !cl.startsWith("Entity")) {
                                    boolean sc = false;
                                    while (cl.endsWith("+")) {
                                        sc = true;
                                        cl = cl.substring(0, cl.length() - 1);
                                    }
                                    if (sc) {
                                        if (!scs.contains(cl)) {
                                            scs.add(cl);
                                        }
                                    }
                                    else if (!ios.contains(cl)) {
                                        ios.add(cl);
                                    }
                                }
                            }
                        }
                        else {
                            Set instanceOfs = kb.getCachedRelationValues("instance", term, 1, 2);
                            if ((instanceOfs != null) && !instanceOfs.isEmpty()) {
                                Iterator it = instanceOfs.iterator();
                                String io = null;
                                while (it.hasNext()) {
                                    io = (String) it.next();
                                    if (!io.equals("Entity") && !ios.contains(io)) {
                                        ios.add(io);
                                    }
                                }
                            }
                        }
                    }
                }
                // Special treatment for instance or subclass, only if var.equals(arg1)
                // and arg2 is a functional term.
                else if (Arrays.asList("instance", "subclass").contains(pred)) {
                    arg1 = f.getArgument(1);
                    arg2 = f.getArgument(2);
                    if (var.equals(arg1) && listP(arg2)) {
                        Formula nextF = new Formula();
                        nextF.read(arg2);
                        if (nextF.isFunctionalTerm()) {
                            String fn = nextF.car();
                            List classes = getTypeList(fn, kb);
                            if (!classes.isEmpty()) {
                                cl = (String) classes.get(0);
                            }
                            if (cl == null) {
                                cl = findType(0, fn, kb);
                            }
                            if (StringUtil.isNonEmptyString(cl) && !cl.startsWith("Entity")) {
                                while (cl.endsWith("+")) {
                                    cl = cl.substring(0, cl.length() - 1);
                                }
                                if (pred.equals("subclass")) {
                                    if (!scs.contains(cl)) {
                                        scs.add(cl);
                                    }
                                }
                                else if (!ios.contains(cl)) {
                                    ios.add(cl);
                                }
                            }
                        }
                    }
                }
            }
        }
        catch (Exception ex) {
            System.out.println("Error in Formula.computeTypeRestrictions(" 
                               + this + ", " 
                               + ios + ", "
                               + scs + ", "
                               + var + ", "
                               + kb.name + ")");
            System.out.println("  pred == " + pred);
            System.out.println(ex.getMessage());
            ex.printStackTrace();
        }

        if (DEBUG) {
            System.out.println("EXIT computeTypeRestrictions("
                               + this + ", "
                               + ios + ", "
                               + scs + ", "
                               + var + ", "
                               + kb.name + " )");
            System.out.println("  time == " 
                               + ((System.currentTimeMillis() - t1) / 1000.0)
                               + " seconds");
        }
        return;
    }

    /** ***************************************************************
     * When invoked on a Formula that begins with explicit universal
     * quantification, this method returns a String representation of
     * the Formula with type constraints added for the top level
     * quantified variables, if possible.  Otherwise, a String
     * representation of the original Formula is returned.
     *
     * @param shelf A List of quaternary ArrayLists, each of which
     * contains type information about a variable
     *
     * @param kb The KB used to determine predicate and variable arg
     * types.
     *
     * @return A String representation of a Formula, with type
     * restrictions added.
     */
    private String insertTypeRestrictionsU(List shelf, KB kb) {

        long t1 = 0L;
        if (DEBUG) {
            t1 = System.currentTimeMillis();
            System.out.println("ENTER insertTypeRestrictionsU(" 
                               + this + ", " 
                               + shelf + ", "
                               + kb.name + ")");
        }

        String result = this.theFormula;
        try {
            String varlist = this.getArgument(1);
            Formula varlistF = new Formula();
            varlistF.read(varlist);
            List newShelf = makeNewShelf(shelf);
            int vlen = varlistF.listLength();
            for (int i = 0; i < vlen; i++) {
                addVarDataQuad(varlistF.getArgument(i), "U", newShelf);
            }

            String arg2 = this.getArgument(2);
            Formula nextF = new Formula();
            nextF.read(arg2);

            String processedArg2 = nextF.insertTypeRestrictionsR(newShelf, kb);

            Set constraints = new LinkedHashSet();
            StringBuilder sb = new StringBuilder();

            List quad = null;
            String var = null;
            String token = null;
            List ios = null;
            List scs = null;
            Iterator it2 = null;
            String constraint = null;
            for (Iterator it = newShelf.iterator(); it.hasNext();) {
                quad = (List) it.next();
                var = (String) quad.get(0);
                token = (String) quad.get(1);
                if (token.equals("U")) {
                    ios = (List) quad.get(2);
                    scs = (List) quad.get(3);
                    if (!ios.isEmpty()) {
                        winnowTypeList(ios, kb);
                        for (it2 = ios.iterator(); it2.hasNext();) {
                            sb.setLength(0);
                            sb.append("(instance ");
                            sb.append(var);
                            sb.append(" ");
                            sb.append(it2.next().toString());
                            sb.append(")");
                            constraint = sb.toString();
                            if (!processedArg2.contains(constraint)) {
                                constraints.add(constraint);
                            }
                        }
                    }
                    if (!scs.isEmpty()) {
                        winnowTypeList(scs, kb);
                        for (it2 = scs.iterator(); it2.hasNext();) {
                            sb.setLength(0);
                            sb.append("(subclass ");
                            sb.append(var);
                            sb.append(" ");
                            sb.append(it2.next().toString());
                            sb.append(")");
                            constraint = sb.toString();
                            if (!processedArg2.contains(constraint)) {
                                constraints.add(constraint);
                            }
                        }
                    }
                }
            }
            sb.setLength(0);
            sb.append("(forall ");
            sb.append(varlistF.theFormula);
            if (constraints.isEmpty()) {
                sb.append(" ");
                sb.append(processedArg2);
            }
            else {
                sb.append(" (=>");
                int clen = constraints.size();
                if (clen > 1) {
                    sb.append(" (and");
                }
                for (it2 = constraints.iterator(); it2.hasNext();) {
                    sb.append(" ");
                    sb.append(it2.next().toString());
                }
                if (clen > 1) {
                    sb.append(")");
                }
                sb.append(" ");
                sb.append(processedArg2);
                sb.append(")");
            }
            sb.append(")");
            result = sb.toString();
        }
        catch (Exception ex) {
            ex.printStackTrace();
            result = this.theFormula;
        }

        if (DEBUG) {
            System.out.println("EXIT insertTypeRestrictionsU(" 
                               + this + ", " 
                               + shelf + ", "
                               + kb.name + ")");
            System.out.println("  time == " 
                               + ((System.currentTimeMillis() - t1) / 1000.0)
                               + " seconds");
            System.out.println("  result == " + result);
        }
        return result;
    }

    /** ***************************************************************
     * When invoked on a Formula that begins with explicit existential
     * quantification, this method returns a String representation of
     * the Formula with type constraints added for the top level
     * quantified variables, if possible.  Otherwise, a String
     * representation of the original Formula is returned.
     *
     * @param shelf A List of quaternary ArrayLists, each of which
     * contains type information about a variable
     *
     * @param kb The KB used to determine predicate and variable arg
     * types.
     *
     * @return A String representation of a Formula, with type
     * restrictions added.
     */
    private String insertTypeRestrictionsE(List shelf, KB kb) {

        long t1 = 0L;
        if (DEBUG) {
            t1 = System.currentTimeMillis();
            System.out.println("ENTER insertTypeRestrictionsE(" 
                               + this + ", " 
                               + shelf + ", "
                               + kb.name + ")");
        }

        String result = this.theFormula;
        try {
            String varlist = this.getArgument(1);
            Formula varlistF = new Formula();
            varlistF.read(varlist);
            List newShelf = makeNewShelf(shelf);
            int vlen = varlistF.listLength();
            for (int i = 0; i < vlen; i++) {
                addVarDataQuad(varlistF.getArgument(i), "E", newShelf);
            }

            String arg2 = this.getArgument(2);
            Formula nextF = new Formula();
            nextF.read(arg2);

            String processedArg2 = nextF.insertTypeRestrictionsR(newShelf, kb);
            nextF.read(processedArg2);

            Set constraints = new LinkedHashSet();
            StringBuilder sb = new StringBuilder();

            List quad = null;
            String var = null;
            String token = null;
            List ios = null;
            List scs = null;
            Iterator it2 = null;
            String constraint = null;
            for (Iterator it = newShelf.iterator(); it.hasNext();) {
                quad = (List) it.next();
                var = (String) quad.get(0);
                token = (String) quad.get(1);
                if (token.equals("E")) {
                    ios = (List) quad.get(2);
                    scs = (List) quad.get(3);
                    if (!ios.isEmpty()) {
                        winnowTypeList(ios, kb);
                        for (it2 = ios.iterator(); it2.hasNext();) {
                            sb.setLength(0);
                            sb.append("(instance ");
                            sb.append(var);
                            sb.append(" ");
                            sb.append(it2.next().toString());
                            sb.append(")");
                            constraint = sb.toString();
                            if (!processedArg2.contains(constraint)) {
                                constraints.add(constraint);
                            }
                        }
                    }
                    if (!scs.isEmpty()) {
                        winnowTypeList(scs, kb);
                        for (it2 = scs.iterator(); it2.hasNext();) {
                            sb.setLength(0);
                            sb.append("(subclass ");
                            sb.append(var);
                            sb.append(" ");
                            sb.append(it2.next().toString());
                            sb.append(")");
                            constraint = sb.toString();
                            if (!processedArg2.contains(constraint)) {
                                constraints.add(constraint);
                            }
                        }
                    }
                }
            }
            sb.setLength(0);
            sb.append("(exists ");
            sb.append(varlistF.theFormula);
            if (constraints.isEmpty()) {
                sb.append(" ");
                sb.append(processedArg2);
            }
            else {
                sb.append(" (and");
                int clen = constraints.size();
                for (it2 = constraints.iterator(); it2.hasNext();) {
                    sb.append(" ");
                    sb.append(it2.next().toString());
                }
                if (nextF.car().equals("and")) {
                    int nextFLen = nextF.listLength();
                    for (int k = 1; k < nextFLen; k++) {
                        sb.append(" "); 
                        sb.append(nextF.getArgument(k));
                    }
                }
                else {
                    sb.append(" ");
                    sb.append(nextF.theFormula);
                }
                sb.append(")");
            }
            sb.append(")");
            result = sb.toString();
        }
        catch (Exception ex) {
            ex.printStackTrace();
            result = this.theFormula;
        }

        if (DEBUG) {
            System.out.println("EXIT insertTypeRestrictionsE(" 
                               + this + ", " 
                               + shelf + ", "
                               + kb.name + ")");
            System.out.println("  time == " 
                               + ((System.currentTimeMillis() - t1) / 1000.0)
                               + " seconds");
            System.out.println("  result == " + result);
        }
        return result;
    }

    /** ***************************************************************
     * When invoked on a Formula, this method returns a String
     * representation of the Formula with type constraints added for
     * all explicitly quantified variables, if possible.  Otherwise, a
     * String representation of the original Formula is returned.
     *
     * @param shelf A List, each element of which is a quaternary List
     * containing a SUO-KIF variable String, a token "U" or "E"
     * indicating how the variable is quantified, a List of instance
     * classes, and a List of subclass classes
     *
     * @param kb The KB used to determine predicate and variable arg
     * types.
     *
     * @return A String representation of a Formula, with type
     * restrictions added.
     */
    private String insertTypeRestrictionsR(List shelf, KB kb) {

        long t1 = 0L;
        if (DEBUG) {
            t1 = System.currentTimeMillis();
            System.out.println("ENTER insertTypeRestrictionsR(" 
                               + this + ", " 
                               + shelf + ", "
                               + kb.name + ")");
        }

        String result = this.theFormula;
        try {

            if (listP(this.theFormula) 
                && !empty(this.theFormula)
                && this.theFormula.matches(".*\\?\\w+.*")) {
                StringBuilder sb = new StringBuilder();
                Formula f = new Formula();
                f.read(this.theFormula);
                int len = f.listLength();
                String arg0 = f.car();
                if (isQuantifier(arg0) && (len == 3)) {
                    if (arg0.equals("forall")) {
                        sb.append(f.insertTypeRestrictionsU(shelf, kb));
                    }
                    else {
                        sb.append(f.insertTypeRestrictionsE(shelf, kb));
                    }
                }
                else {
                    sb.append("(");
                    String argI = null;
                    for (int i = 0; i < len; i++) {
                        argI = f.getArgument(i);
                        if (i > 0) {
                            sb.append(" ");
                            if (isVariable(argI)) {
                                String type = findType(i, arg0, kb);
                                if (StringUtil.isNonEmptyString(type)
                                    && !type.startsWith("Entity")) {
                                    boolean sc = false;
                                    while (type.endsWith("+")) {
                                        sc = true;
                                        type = type.substring(0, type.length() - 1);
                                    }
                                    if (sc) {
                                        addScForVar(argI, type, shelf);
                                    }
                                    else {
                                        addIoForVar(argI, type, shelf);
                                    }
                                }
                            }
                        }
                        Formula nextF = new Formula();
                        nextF.read(argI);
                        sb.append(nextF.insertTypeRestrictionsR(shelf, kb));
                    }
                    sb.append(")");
                }
                result = sb.toString();
            }
        }
        catch (Exception ex) {
            System.out.println(ex.getMessage());
            ex.printStackTrace();
            result = this.theFormula;
        }

        if (DEBUG) {
            System.out.println("EXIT insertTypeRestrictionsR(" 
                               + this + ", " 
                               + shelf + ", "
                               + kb.name + ")");
            System.out.println("  time == " 
                               + ((System.currentTimeMillis() - t1) / 1000.0)
                               + " seconds");
            System.out.println("  result == " + result);
        }

        return result;
    }

    private void addVarDataQuad(String var, String quantToken, List shelf) {
        try {
            ArrayList quad = new ArrayList();
            quad.add(var);              // e.g., "?X"
            quad.add(quantToken);       // "U" or "E"
            quad.add(new ArrayList());  // ios
            quad.add(new ArrayList());  // scs
            shelf.add(0, quad);
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return;
    }

    private ArrayList getIosForVar(String var, List shelf) {
        ArrayList result = null;
        try {
            ArrayList quad = null;
            for (Iterator si = shelf.iterator(); si.hasNext();) {
                quad = (ArrayList) si.next();
                if (var.equals((String) (quad.get(0)))) {
                    result = (ArrayList) (quad.get(2));
                    break;
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return result;
    }

    private ArrayList getScsForVar(String var, List shelf) {
        ArrayList result = null;
        try {
            ArrayList quad = null;
            for (Iterator si = shelf.iterator(); si.hasNext();) {
                quad = (ArrayList) si.next();
                if (var.equals((String) (quad.get(0)))) {
                    result = (ArrayList) (quad.get(3));
                    break;
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return result;
    }

    private void addIoForVar(String var, String io, List shelf) {
        try {
            if (StringUtil.isNonEmptyString(io)) {
                ArrayList ios = getIosForVar(var, shelf);
                if ((ios != null) && !ios.contains(io)) {
                    ios.add(io);
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return;
    }

    private void addScForVar(String var, String sc, List shelf) {
        try {
            if (StringUtil.isNonEmptyString(sc)) {
                ArrayList scs = getScsForVar(var, shelf);
                if ((scs != null) && !scs.contains(sc)) {
                    scs.add(sc);
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return;
    }

    private ArrayList makeNewShelf(List shelf) {
        return new ArrayList(shelf);
    }

    /** ***************************************************************
     * Add clauses for every variable in the antecedent to restrict its
     * type to the type restrictions defined on every relation in which
     * it appears.  For example
     * (=>
     *   (foo ?A B)
     *   (bar B ?A))
     * 
     * (domain foo 1 Z) 
     * 
     * would result in
     * 
     * (=>
     *   (instance ?A Z)
     *   (=>
     *     (foo ?A B)
     *     (bar B ?A)))
     */
    private String addTypeRestrictions(KB kb) { 

        // System.out.println("INFO in Formula.addTypeRestrictions(" + this + ")");

        if (DEBUG) {
            System.out.println("ENTER addTypeRestrictions(" + this + ", " + kb.name + ")");
        }

        String result = this.theFormula;
        try {
            Formula f = new Formula();
            long t1 = System.currentTimeMillis();
            f.read(this.makeQuantifiersExplicit(false));
            
            if (DEBUG) {
                System.out.println("  f == " + f);
            }
            
            long t2 = System.currentTimeMillis();
            result = f.insertTypeRestrictionsR(new ArrayList(), kb);
            long t3 = System.currentTimeMillis();
            // Time makeQuantifiersExplicit
            KB.ppTimers[7] += (t2 - t1);
            // Time insertTypeRestrictions
            KB.ppTimers[8] += (t3 - t2);
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }

        if (DEBUG) {
            System.out.println("EXIT addTypeRestrictions(" + this + ", " + kb.name + ")");
            System.out.println("  result == " + result);
        }

        return result;
    }

    /** ***************************************************************
     * This method returns a HashMap that maps each variable in this
     * Formula to an ArrayList that contains a pair of ArrayLists.
     * The first ArrayList of the pair contains the names of types
     * (classes) of which the variable must be an instance.  The
     * second ArrayList of the pair contains the names of types of
     * which the variable must be a subclass.  Either list in the pair
     * could be empty.  If the only instance or subclass sortal that
     * can be computed for a variable is Entity, the lists will be
     * empty.
     * 
     * @param kb The KB used to compute the sortal constraints for
     * each variable.
     *
     * @return A HashMap
     */
    public HashMap computeVariableTypes(KB kb) { 

        if (DEBUG) {
            System.out.println("ENTER computeVariableTypes(" + this + ", " + kb.name + ")");
        }

        HashMap result = new HashMap();
        try {
            Formula f = new Formula();
            f.read(this.makeQuantifiersExplicit(false));
            f.computeVariableTypesR(result, kb);
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }

        if (DEBUG) {
            System.out.println("EXIT computeVariableTypes(" + this + ", " + kb.name + ")");
            System.out.println("  result == " + result);
        }

        return result;
    }

    /** ***************************************************************
     * A recursive utility method used to collect type information for
     * the variables in this Formula.
     * 
     * @param map A HashMap used to store type information for the
     * variables in this Formula.
     *
     * @param kb The KB used to compute the sortal constraints for
     * each variable.
     *
     * @return void
     */
    private void computeVariableTypesR(HashMap map, KB kb) {

        if (DEBUG) {
            System.out.println("ENTER computeVariableTypesR(" + this + ", " 
                               + map + ", " + kb.name + ")");
        }

        try {
            if (this.listP() && !this.empty()) {
                int len = this.listLength();
                String arg0 = this.car();
                if (isQuantifier(arg0) && (len == 3)) {
                    this.computeVariableTypesQ(map, kb);
                }
                else {
                    for (int i = 0; i < len; i++) {
                        Formula nextF = new Formula();
                        nextF.read(this.getArgument(i));
                        nextF.computeVariableTypesR(map, kb);
                    }
                }
            }
        }
        catch (Exception ex) {
            System.out.println(ex.getMessage());
            ex.printStackTrace();
        }

        if (DEBUG) {
            System.out.println("EXIT computeVariableTypesR(" + this + ", " 
                               + map + ", " + kb.name + ")");
        }
        return;
    }

    /** ***************************************************************
     * A recursive utility method used to collect type information for
     * the variables in this Formula, which is assumed to have forall
     * or exists as its arg0.
     * 
     * @param map A HashMap used to store type information for the
     * variables in this Formula.
     *
     * @param kb The KB used to compute the sortal constraints for
     * each variable.
     *
     * @return void
     */
    private void computeVariableTypesQ(HashMap map, KB kb) {

        if (DEBUG) {
            System.out.println("ENTER computeVariableTypesQ(" + this + ", " 
                               + map + ", " + kb.name + ")");
        }
        try {
            Formula varlistF = new Formula();
            varlistF.read(this.getArgument(1));
            // System.out.println("varlistF == " + varlistF);
            int vlen = varlistF.listLength();
            // System.out.println("vlen == " + vlen);
            Formula nextF = new Formula();
            nextF.read(this.getArgument(2));
            // System.out.println("nextF == " + nextF);
            String var = null;
            for (int i = 0; i < vlen; i++) {
                ArrayList types = new ArrayList();
                ArrayList ios = new ArrayList();
                ArrayList scs = new ArrayList();
                var = varlistF.getArgument(i);
                // System.out.println("i == " + i + ", var == " + var);
                nextF.computeTypeRestrictions(ios, scs, var, kb);
                if (!ios.isEmpty()) {
                    winnowTypeList(ios, kb);
                }
                if (!scs.isEmpty()) {
                    winnowTypeList(scs, kb);
                }
                types.add(ios);
                types.add(scs);
                map.put(var, types);
            }
            nextF.computeVariableTypesR(map, kb);
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }

        if (DEBUG) {
            System.out.println("EXIT computeVariableTypesQ(" + this + ", " 
                               + map + ", " + kb.name + ")");
        }

        return;
    }

    /** ***************************************************************
     * Pre-process a formula before sending it to the theorem prover. This includes
     * ignoring meta-knowledge like documentation strings, translating
     * mathematical operators, quoting higher-order formulas, expanding
     * row variables and prepending the 'holds__' predicate.
     * @return an ArrayList of Formula(s)
     */
    private String preProcessRecurse(Formula f, String previousPred, boolean ignoreStrings, 
                                     boolean translateIneq, boolean translateMath) {

        // System.out.println("INFO in Formula.preProcessRecurse(" + this + ")");

        StringBuilder result = new StringBuilder();
        try {
            if (f.listP() && !f.empty()) {

                String prefix = "";
                String pred = f.car();
                // Formula predF = new Formula();
                // predF.read(pred);

                if (isQuantifier(pred)) {

                    // The list of quantified variables.
                    result.append(" ");
                    result.append(f.cadr());

                    // The formula following the list of variables.
                    String next = f.caddr();
                    Formula nextF = new Formula();
                    nextF.read(next);
                    result.append(" ");
                    result.append(preProcessRecurse(nextF,
                                                    "",
                                                    ignoreStrings,
                                                    translateIneq,
                                                    translateMath));
                }
                else {
                    Formula restF = f.cdrAsFormula();
                    int argCount = 1;
                    while (!restF.empty()) {
                        argCount++;
                        String arg = restF.car();

                        //System.out.println("INFO in preProcessRecurse(): arg: " + arg);
                        Formula argF = new Formula();
                        argF.read(arg);
                        if (argF.listP()) {
                            String res = preProcessRecurse(argF,pred,ignoreStrings,translateIneq,translateMath);
                            result.append(" ");
                            if (!isLogicalOperator(pred) &&
                                !isComparisonOperator(pred) &&
                                !isMathFunction(pred) &&
                                !argF.isFunctionalTerm()) {
                                result.append("`");     
                            }
                            result.append(res);
                        }
                        else {
                            result.append(" " + arg);
                        }
                        restF.theFormula = restF.cdr();
                    }

                    if (KBmanager.getMgr().getPref("holdsPrefix").equals("yes")) {
                        if (!isLogicalOperator(pred) && !isQuantifierList(pred,previousPred)) {
                            prefix = "holds_";    
                        }    
                        if (f.isFunctionalTerm()) {
                            prefix = "apply_";  
                        }
                        if (pred.equals("holds")) {
                            pred = "";
                            argCount--;
                            prefix = prefix + argCount + "__ ";
                        }
                        else {
                            if (!isLogicalOperator(pred) && 
                                !isQuantifierList(pred,previousPred) && 
                                !isMathFunction(pred) && 
                                !isComparisonOperator(pred)) {
                                prefix = prefix + argCount + "__ ";
                            }
                            else {
                                prefix = "";
                            }
                        }
                    }
                }
                result.insert(0, pred);
                result.insert(0, prefix);
                result.insert(0, "(");
                result.append(")");
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return result.toString();
    }

    /** ***************************************************************
     * Pre-process a formula before sending it to the theorem
     * prover. This includes ignoring meta-knowledge like
     * documentation strings, translating mathematical operators,
     * quoting higher-order formulas, expanding row variables and
     * prepending the 'holds__' predicate.
     *
     * @param isQuery If true the Formula is a query and should be
     *                existentially quantified, else the Formula is a
     *                statement and should be universally quantified
     *
     * @param kb The KB to be used for processing this Formula
     *
     * @return an ArrayList of Formula(s), which could be empty.
     *
     */
    public ArrayList preProcess(boolean isQuery, KB kb) {

        if (DEBUG) {
            System.out.println("ENTER Formula.preProcess(" 
                               + this + ", " 
                               + isQuery + ", " 
                               + kb.name + ")");
        }

        ArrayList results = new ArrayList();

        try {

            if (isNonEmptyString(this.theFormula)) {

                KBmanager mgr = KBmanager.getMgr();

                if (!this.isBalancedList()) {
                    String errStr = "Unbalanced parentheses or quotes";
                    System.out.println("WARNING in Formula.preProcess(" 
                                       + this.theFormula
                                       + ", "
                                       + isQuery 
                                       + ", " 
                                       + kb.name + ")");
                    System.out.println("  " + errStr);
                    mgr.setError(mgr.getError()
                                 + "\n<br/>" 
                                 + errStr 
                                 + " in "
                                 + this.theFormula
                                 + "\n<br/>");
                    return results;
                }

                boolean ignoreStrings = false;
                boolean translateIneq = true;
                boolean translateMath = true;
                ArrayList accumulator = new ArrayList();

                boolean addHoldsPrefix = 
                    mgr.getPref("holdsPrefix").equalsIgnoreCase("yes");

                Formula f = new Formula();
                f.read(this.theFormula);
                if (f.car().equals("documentation") || f.car().equals("comment")) {
                    f.theFormula = StringUtil.replaceNonAsciiChars(f.theFormula);
                }

                long t1 = -1L;
                long tnaplwriVal = KB.ppTimers[4];

                ArrayList predVarInstantiations = new ArrayList();

                if (addHoldsPrefix) {
                    predVarInstantiations.add(f);
                }
                else {
                    // Do pred var instantiations if we are not adding holds
                    // prefixes.
                    t1 = System.currentTimeMillis();

                    predVarInstantiations.addAll(f.instantiatePredVars(kb));

                    // If the list of pred var instatiations is empty, add the
                    // original formula to the list for further processing below.  
                    if (predVarInstantiations.isEmpty()) {
                        predVarInstantiations.add(f);
                    }
                    else {
                        // If the formula contains a pred var that can't be
                        // instantiated and so has been marked "reject", don't add
                        // anything.
                        Object obj0 = predVarInstantiations.get(0);
                        String errStr = "No predicate instantiations for";
                        if (isNonEmptyString(obj0) && ((String) obj0).equalsIgnoreCase("reject")) {
                            predVarInstantiations.clear();
                            System.out.println("INFO in Formula.preProcess()");
                            System.out.println("  " + errStr);
                            System.out.println(this);
                            errStr += ("\n<br/> " + this.htmlFormat(kb));
                            mgr.setError(mgr.getError()
                                         + "\n<br/>" 
                                         + errStr 
                                         + "\n<br/>");
                        }
                    }
                    // Increment the timer for pred var instantiation.
                    KB.ppTimers[1] += (System.currentTimeMillis() - t1);
                }

                // Make sure every SetOrClass is stated to be such.
                String arg0 = f.car();
                if (!isQuery && StringUtil.isNonEmptyString(arg0)) {
                    int start = -1;
                    if (arg0.equals("subclass")) start = 0;
                    if (arg0.equals("instance")) start = 1;
                    if (start > -1) {
                        List args = Arrays.asList(f.getArgument(1), f.getArgument(2));
                        int argslen = args.size();
                        String ioStr = null;
                        Formula ioF = null;
                        String arg = null;
                        for (int i = start; i < argslen; i++) {
                            arg = (String) args.get(i);
                            if (!isVariable(arg)) {
                                ioStr = ("(instance " + arg + " SetOrClass)").intern();
                                ioF = new Formula();
                                ioF.read(ioStr);
                                ioF.sourceFile = this.sourceFile;
                                if (!kb.formulaMap.containsKey(ioStr) 
                                    && !predVarInstantiations.contains(ioF)) {
                                    if (kb.sortalTypeCache != null) {
                                        if (kb.sortalTypeCache.get(ioStr) == null) {
                                            kb.sortalTypeCache.put(ioStr, ioStr);
                                            predVarInstantiations.add(ioF);
                                        }
                                    }
                                    else {
                                        predVarInstantiations.add(ioF);
                                    }
                                }
                            }
                        }
                    }
                }

                if (DEBUG) {
                    System.out.println("  predVarInstantiations == " + predVarInstantiations);
                }

                // We do this to avoid adding up time spent in
                // Formula.toNegAndPosLitsWtihRenameInfo() while doing pred
                // var instantiation.  What we really want to know is how much
                // time this method contributes to the total time for row var
                // expansion.
                KB.ppTimers[4] = tnaplwriVal;

                // Iterate over the instantiated predicate formulas, doing row
                // var expansion on each.  If no predicate instantiations can
                // be generated, the ArrayList predVarInstantiations will
                // contain just the original input formula.
                t1 = System.currentTimeMillis();
                int pviN = predVarInstantiations.size();
                Iterator it = null;
                if ((pviN > 0) && (pviN < AXIOM_EXPANSION_LIMIT)) {
                    ArrayList rowVarExpansions = null;
                    for (it = predVarInstantiations.iterator(); it.hasNext();) {
                        f = (Formula) it.next();
                        // System.out.println("f == " + f);
                        rowVarExpansions = f.expandRowVars(kb);
                        if (rowVarExpansions != null) {
                            accumulator.addAll(rowVarExpansions);

                            // System.out.println("  accumulator == " + accumulator);

                            if (accumulator.size() > AXIOM_EXPANSION_LIMIT) {
                                break;
                            }
                        }
                    }
                }

                if (DEBUG) {
                    System.out.println("  accumulator == " + accumulator);
                }

                // Increment the timer for row var expansion.
                KB.ppTimers[2] += (System.currentTimeMillis() - t1);

                // Iterate over the formulas resulting from row var
                // expansion, passing each to preProcessRecurse for
                // further processing.
                if (!accumulator.isEmpty()) {
                    boolean addSortals = mgr.getPref("typePrefix").equalsIgnoreCase("yes");
                    Formula fnew = null;
                    String theNewFormula = null;
                    for (it = accumulator.iterator(); it.hasNext();) {
                        fnew = (Formula) it.next();
                        t1 = System.currentTimeMillis();
                        arg0 = this.getArgument(0);
                        if (addSortals
                            && !isQuery 
                            // isLogicalOperator(arg0) ||
                            && fnew.theFormula.matches(".*\\?\\w+.*")) {
                            fnew.read(fnew.addTypeRestrictions(kb));
                        }
                        // Increment the timer for adding type restrictions.
                        KB.ppTimers[0] += (System.currentTimeMillis() - t1);

                        t1 = System.currentTimeMillis();
                        theNewFormula = fnew.preProcessRecurse(fnew,
                                                               "",
                                                               ignoreStrings,
                                                               translateIneq,
                                                               translateMath);
                        fnew.read(theNewFormula);
                        // Increment the timer for preProcessRecurse().
                        KB.ppTimers[6] += (System.currentTimeMillis() - t1);

                        if (fnew.isOkForInference(isQuery, kb)) {
                            fnew.sourceFile = this.sourceFile;
                            results.add(fnew);
                        }
                        else {
                            System.out.println("WARNING in Formula.preProcess()");
                            System.out.println("  REJECTING " + theNewFormula);
                            mgr.setError(mgr.getError()
                                         + "\n<br/>Formula rejected for inference:<br/>"
                                         + fnew.htmlFormat(kb)
                                         + "<br/>\n");
                        }
                    }
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }

        if (DEBUG) {
            System.out.println("EXIT Formula.preProcess(" 
                               + this + ", " 
                               + isQuery + ", " 
                               + kb.name + ")");
            System.out.println("  results == " + results);
        }

        return results;
    }

    /** ***************************************************************
     * Returns true if this Formula appears not to have any of the
     * characteristics that would cause it to be rejected during
     * translation to TPTP form, or cause problems during inference.
     * Otherwise, returns false.
     *
     * @param query true if this Formula represents a query, else
     * false.
     *
     * @param kb The KB object to be used for evaluating the
     * suitability of this Formula.
     *
     * @return boolean
     */
    private boolean isOkForInference(boolean query, KB kb) {

        boolean pass = false;
        // kb isn't used yet, because the checks below are purely
        // syntactic.  But it probably will be used in the future.
        try {
            pass = !(// (equal ?X ?Y ?Z ...) - equal is strictly binary. 
                     this.theFormula.matches(".*\\(\\s*equal\\s+\\?\\w+\\s+\\?\\w+\\s+\\?\\w+.*") 

                     // The formula contains non-ASCII characters.
                     // was: this.theFormula.matches(".*[\\x7F-\\xFF].*")
                     || this.theFormula.matches(".*[^\\p{ASCII}].*")

                     // (<relation> ?X ...) - no free variables in an
                     // atomic formula that doesn't contain a string
                     // unless the formula is a query.
                     || (!query 
                         && !isLogicalOperator(this.car())
                         // The formula does not contain a string.
                         && !this.theFormula.matches("^\\(\\s*.*\\\".*\\)$")
                         // The formula contains a free variable.
                         && this.theFormula.matches("^\\(\\s*.*\\?\\w+.*\\)$"))
     
                     // ... add more patterns here, as needed.
                     || false
                     );
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return pass;
    }

    /** ***************************************************************
     * Compare the given formula to the query and return whether
     * they are the same.
     */
    public static boolean isQuery(String query, String formula) {

        boolean result = false;

        Formula f = new Formula();
        f.read(formula);
        result = f.equals(query);
        return result;
    }

    /** ***************************************************************
     * Compare the given formula to the negated query and return whether
     * they are the same (minus the negation).
     */
    public static boolean isNegatedQuery(String query, String formula) {

        boolean result = false;

        //System.out.println("INFO in Formula.isNegatedQuery(): Comparing |" + query + "| to |" + formula + "|");

        formula = formula.trim();
        if (formula.substring(0,4).compareTo("(not") != 0) 
            return false;
        formula = formula.substring(5,formula.length()-1);
        Formula f = new Formula();
        f.read(formula);
        result = f.equals(query);
        //System.out.print("INFO in Formula.isNegatedQuery(): ");
        //System.out.println(result);
        return result;
    }

    /** ***************************************************************
     * Remove the 'holds' prefix wherever it appears.
     */
    public static String postProcess(String s) {

        s = s.replaceAll("holds_\\d__ ","");
        s = s.replaceAll("apply_\\d__ ","");    
        return s;
    }

    /** ***************************************************************
     * Format a formula for either text or HTML presentation by inserting
     * the proper hyperlink code, characters for indentation and end of line.
     * A standard LISP-style pretty printing is employed where an open
     * parenthesis triggers a new line and added indentation.
     *
     * @param hyperlink - the URL to be referenced to a hyperlinked term.
     * @param indentChars - the proper characters for indenting text.
     * @param eolChars - the proper character for end of line.
     */
    public String format(String hyperlink, String indentChars, String eolChars) {

        boolean inQuantifier = false;
        StringBuilder token = new StringBuilder();
        StringBuilder formatted = new StringBuilder();
        int indentLevel = 0;
        boolean inToken = false;
        boolean inVariable = false;
        boolean inVarlist = false;
        boolean inComment = false;

        if (isNonEmptyString(theFormula)) {
            theFormula = theFormula.trim();
        }

        for (int i = 0; i < theFormula.length(); i++) {
            // System.out.println("INFO in format(): " + formatted.toString());
            if (!inComment) {
                if (theFormula.charAt(i) == '(' && !inQuantifier && (indentLevel != 0 || i > 1)) {
                    if (i > 0 && Character.isWhitespace(theFormula.charAt(i-1))) { 
                        //System.out.println("INFO in format(): Deleting at end of : |" + formatted.toString() + "|");
                        formatted = formatted.deleteCharAt(formatted.length()-1);
                    }
                    formatted.append(eolChars);
                    for (int j = 0; j < indentLevel; j++) 
                        formatted.append(indentChars);                    
                }
                if (theFormula.charAt(i) == '(' && indentLevel == 0 && i == 0) 
                    formatted.append(theFormula.charAt(i));
                if (Character.isJavaIdentifierStart(theFormula.charAt(i)) && !inToken && !inVariable) {
                    token = new StringBuilder(theFormula.charAt(i));
                    inToken = true;
                }
                if ((Character.isJavaIdentifierPart(theFormula.charAt(i)) || theFormula.charAt(i) == '-') && inToken)
                    token = token.append(theFormula.charAt(i));
                if (theFormula.charAt(i) == '(') {
                    if (inQuantifier) {
                        inQuantifier = false;
                        inVarlist = true;
                        token = new StringBuilder();
                    }
                    else
                        indentLevel++;
                }
                if (theFormula.charAt(i) == '"') 
                    inComment = true;    // The next character will be handled in the "else" clause of this primary "if"
                if (theFormula.charAt(i) == ')') {
                    if (!inVarlist)
                        indentLevel--;
                    else
                        inVarlist = false;
                }
                if (token.toString().compareTo("exists") == 0 || token.toString().compareTo("forall") == 0)
                    inQuantifier = true;
                if (!Character.isJavaIdentifierPart(theFormula.charAt(i)) && inVariable) 
                    inVariable = false;
                if (theFormula.charAt(i) == '?' || theFormula.charAt(i) == '@')
                    inVariable = true;
                if (!(Character.isJavaIdentifierPart(theFormula.charAt(i)) || theFormula.charAt(i) == '-') && inToken) {
                    inToken = false;
                    if (hyperlink != "")
                        formatted.append("<a href=\"" + hyperlink + "&term=" + token + "\">" + token + "</a>");
                    else
                        formatted.append(token);
                    token = new StringBuilder();
                }
                if (!inToken && i>0 && !(Character.isWhitespace(theFormula.charAt(i)) && theFormula.charAt(i-1) == '(')) {
                    if (Character.isWhitespace(theFormula.charAt(i))) { 
                        if (!Character.isWhitespace(theFormula.charAt(i-1)))
                            formatted.append(" ");
                    }
                    else
                        formatted.append(theFormula.charAt(i));
                }
            }
            else {     // In a comment
                formatted.append(theFormula.charAt(i));
                if (i > 70 && theFormula.charAt(i) == '/')      // add spaces to long URL strings 
                    formatted.append(" ");                
                if (theFormula.charAt(i) == '"') 
                    inComment = false;
            }
        }
        if (inToken) {    // A term which is outside of parenthesis, typically, a binding.
            if (hyperlink != "")
                formatted.append("<a href=\"" + hyperlink + "&term=" + token + "\">" + token + "</a>");
            else
                formatted.append(token);
        }
        return formatted.toString();
    }

    /** ***************************************************************
     * Format a formula for text presentation.
     * @deprecated
     */
    public String textFormat() {

        return format("","  ",new Character((char) 10).toString());
    }

    /** ***************************************************************
     * Format a formula for text presentation.
     */
    public String toString() {

        return format("","  ",new Character((char) 10).toString());
    }

    /** ***************************************************************
     * Format a formula for HTML presentation.
     */
    public String htmlFormat(String html) {

        return format(html,"&nbsp;&nbsp;&nbsp;&nbsp;","<br>\n");
    }

    /** ***************************************************************
     * Format a formula for HTML presentation.
     */
    public String htmlFormat(KB kb) {
        String fKbHref = "";
        try {
            KBmanager mgr = KBmanager.getMgr();
            String hostname = mgr.getPref("hostname");
            if (!isNonEmptyString(hostname)) {
                hostname = "localhost";
            }
            String port = mgr.getPref("port");
            if (!isNonEmptyString(port)) {
                port = "8080";
            }
            String kbHref = ("http://" + hostname + ":" + port + "/sigma/Browse.jsp?kb=" + kb.name);
            fKbHref = format(kbHref,"&nbsp;&nbsp;&nbsp;&nbsp;","<br>\n");
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return fKbHref;
    }

    /** ***************************************************************
     * Format a formula as a prolog statement.  Note that only tuples
     * are converted properly at this time.  Statements with any embedded
     * formulas or functions will be rejected with a null return.
     */
    public String toProlog() {

        if (!listP()) { 
            System.out.println("INFO in Fomula.toProlog(): Not a formula: " + theFormula);
            return "";
        }
        if (empty()) { 
            System.out.println("INFO in Fomula.toProlog(): Empty formula: " + theFormula);
            return "";
        }
        StringBuilder result = new StringBuilder();
        String relation = car();
        Formula f = new Formula();
        f.theFormula = cdr();
        if (!Formula.atom(relation)) { 
            System.out.println("INFO in Fomula.toProlog(): Relation not an atom: " + relation);
            return "";
        }
        result.append(relation + "('");
        while (!f.empty()) {
            String arg = f.car();
            f.theFormula = f.cdr();
            if (!Formula.atom(arg)) { 
                System.out.println("INFO in Formula.toProlog(): Argument not an atom: " + arg);
                return "";
            }
            result.append(arg + "'");
            if (!f.empty()) 
                result.append(",'");
            else
                result.append(").");
        }
        return result.toString();
    }

    public static final String termMentionSuffix  = "__m";
    public static final String termSymbolPrefix   = "s__";
    public static final String termVariablePrefix = "V__";
  
    private static List renameExceptions = Arrays.asList("en");

    /** ***************************************************************
     * Encapsulates translateWord_1, which translates the logical
     * operators and inequalities in SUO-KIF to their TPTP
     * equivalents.
     *
     * @param st is the StreamTokenizer_s that contains the current token
     * @return the String that is the translated token
     */
    private static String translateWord(StreamTokenizer_s st,boolean hasArguments) {
        String result = null;
        try {

            if (DEBUG) {
                System.out.println("ENTER Formula.translateWord("
                                   + st.toString()
                                   + ", "
                                   + hasArguments
                                   + ")");
                System.out.println("  st.ttype == " + st.ttype);
                System.out.println("  st.sval == " + st.sval);
                System.out.println("  st.nval == " + st.nval);
            }

            result = translateWord_1(st, hasArguments);

        }
        catch (Exception ex) {
            System.out.println("Error in Formula.translateWord(" 
                               + st.toString() 
                               + ", " 
                               + hasArguments
                               + "): "
                               + ex.getMessage());
            System.out.println("  st.ttype == " + st.ttype);
            System.out.println("  st.sval == " + st.sval);
            System.out.println("  st.nval == " + st.nval);
            ex.printStackTrace();
        }

        if (DEBUG) {
            System.out.println("EXIT Formula.translateWord("
                               + st.toString()
                               + ", "
                               + hasArguments
                               + ")");
            System.out.println("  st.ttype == " + st.ttype);
            System.out.println("  st.sval == " + st.sval);
            System.out.println("  st.nval == " + st.nval);
            System.out.println("  result == " + result);
        }

        return result;
    }


    /** ***************************************************************
     * Convert the logical operators and inequalities in SUO-KIF to 
     * their TPTP equivalents
     * @param st is the StreamTokenizer_s that contains the current token
     * @return the String that is the translated token
     */
    private static String translateWord_1(StreamTokenizer_s st,boolean hasArguments) {

        int translateIndex;

        List<String> kifOps = Arrays.asList(UQUANT, EQUANT, NOT, AND, OR, IF, IFF);
        List<String> tptpOps = Arrays.asList("! ", "? ", "~ ", " & ", " | ", " => ", " <=> ");

        List<String> kifPredicates = 
            Arrays.asList(LOG_TRUE, LOG_FALSE,
                          EQUAL,
                          "<=","<",">",">=",
                          "lessThanOrEqualTo","lessThan","greaterThan","greaterThanOrEqualTo");

        List<String> tptpPredicates = Arrays.asList("$true","$false",
                                                    "equal",
                                                    "lesseq","less","greater","greatereq",
                                                    "lesseq","less","greater","greatereq");

        List<String> kifFunctions = Arrays.asList(TIMESFN, DIVIDEFN, PLUSFN, MINUSFN);
        List<String> tptpFunctions = Arrays.asList("times","divide","plus","minus");

        List<String> kifRelations = new ArrayList<String>();
        kifRelations.addAll(kifPredicates);
        kifRelations.addAll(kifFunctions);

        //DEBUG System.out.println("Translating word " + st.sval + " with hasArguments " + hasArguments);

        // Context creeps back in here whether we want it or not.  We
        // consult the KBmanager to determine if holds prefixing is
        // turned on, or not.  If it is on, then we do not want to add
        // the "mentions" suffix to relation names used as arguments
        // to other relations.
        KBmanager mgr = null;
        boolean holdsPrefixInUse = false;
        String mentionSuffix = Formula.termMentionSuffix;
        String symbolPrefix = Formula.termSymbolPrefix;
        String variablePrefix = Formula.termVariablePrefix;
        try {
            mgr = KBmanager.getMgr();
            holdsPrefixInUse = ((mgr != null) && mgr.getPref("holdsPrefix").equalsIgnoreCase("yes"));
            if (holdsPrefixInUse && !kifRelations.contains(st.sval)) {
                mentionSuffix = ""; 
            }
        }
        catch (Exception ex) {
            //---Be silent if there is a problem getting the KBmanager.
        }

        //----Places single quotes around strings, and replace \n by space
        if (st.ttype == 34) {
            return("'" + 
                   st.sval.replaceAll("[\n\t\r\f]"," ").replaceAll("'","") + "'");
        }
        //----Fix variables to have leading V_
        if (st.sval.charAt(0) == '?' || st.sval.charAt(0) == '@') {
            return(termVariablePrefix + st.sval.substring(1).replace('-','_'));
        }
        //----Translate special predicates
        translateIndex = 0;
        while (translateIndex < kifPredicates.size() &&
               !st.sval.equals(kifPredicates.get(translateIndex))) {
            translateIndex++;
        }
        if (translateIndex < kifPredicates.size()) {
            // return((hasArguments ? "$" : "") + tptpPredicates[translateIndex]);
            return(tptpPredicates.get(translateIndex) + (hasArguments ? "" : mentionSuffix));
        }
        //----Translate special functions
        translateIndex = 0;
        while (translateIndex < kifFunctions.size() &&
               !st.sval.equals(kifFunctions.get(translateIndex))) {
            translateIndex++;
        }
        if (translateIndex < kifFunctions.size()) {
            // return((hasArguments ? "$" : "") + tptpFunctions[translateIndex]);
            return(tptpFunctions.get(translateIndex) + (hasArguments ? "" : mentionSuffix));
        }
        //----Translate operators
        translateIndex = 0;
        while (translateIndex < kifOps.size() &&
               !st.sval.equals(kifOps.get(translateIndex))) {
            translateIndex++;
        }
        if (translateIndex < kifOps.size()) {
            return(tptpOps.get(translateIndex));
        }
        //----Do nothing to numbers
        if (st.ttype == StreamTokenizer.TT_NUMBER ||
            (st.sval != null && (Character.isDigit(st.sval.charAt(0)) ||
                                 (st.sval.charAt(0) == '-' &&
                                  Character.isDigit(st.sval.charAt(1)))))) {
            return(st.sval);
            //SANITIZE return("n" + st.sval.replace('-','n').replaceAll("[.]","dot"));
        }

        //----Fix other symbols to have leading s_
        // return("s_" + st.sval.substring(1).replace('-','_'));
        String term = st.sval;

        //----Add a "mention" suffix to relation names that occur as arguments
        //----to other relations.
        if (!hasArguments) {
            if ((Character.isLowerCase(st.sval.charAt(0)) && !st.sval.endsWith(mentionSuffix))
                || st.sval.endsWith("Fn")) {
                if (!renameExceptions.contains(term)) {
                    term += mentionSuffix;
                }
            }
        }
        //return("s_" + term.replace('-','_'));
        return(termSymbolPrefix + term.replace('-','_'));
    }

    /** ***************************************************************
     * @param st is the StreamTokenizer_s that contains the current token
     * for which the arity is desired
     *
     * @return the integer arity of the given logical operator
     */
    private static int operatorArity(StreamTokenizer_s st) {

        int translateIndex;
        String kifOps[] = {"forall", "exists", "not", "and", "or", "=>", "<=>"};

        translateIndex = 0; 
        while (translateIndex < kifOps.length && 
               !st.sval.equals(kifOps[translateIndex])) {
            translateIndex++;
        }
        if (translateIndex <= 2) {
            return(1);      
        } else {
            if (translateIndex < kifOps.length) {
                return(2);          
            } else {
                return(-1);          
            }
        }
    }

    /** ***************************************************************
     */
    private static void incrementTOS(Stack countStack) {

        countStack.push(new Integer((Integer)countStack.pop()+1));
    }

    /** ***************************************************************                             
     * Add the current token, if a variable, to the list of variables                               
     * @param variables is the list of variables                                                    
     */
    private static void addVariable(StreamTokenizer_s st,Vector variables) {

        String tptpVariable;

	if (st.sval.charAt(0) == '?' || st.sval.charAt(0) == '@') {
            tptpVariable = translateWord(st,false);
            if (variables.indexOf(tptpVariable) == -1) {
		variables.add(tptpVariable);
            }
        }
    }

    /** ***************************************************************
     * Parse a single formula into TPTP format
     */
    public static String tptpParseSUOKIFString(String suoString) {

        /*
          System.out.println("ENTER Formula.tptpParseSUOKIFString(\""
          + suoString
          + "\")");
        */

        StreamTokenizer_s st = null;
        String translatedFormula = null;

        try {
            int parenLevel;
            boolean inQuantifierVars;
            boolean lastWasOpen;
            boolean inHOL;
            int inHOLCount;
            Stack operatorStack = new Stack();
            Stack countStack = new Stack();
            Vector quantifiedVariables = new Vector();
            Vector allVariables = new Vector();
            int index;
            int arity;
            String quantification;

            StringBuilder tptpFormula = new StringBuilder();

            parenLevel = 0;
            countStack.push(0);
            lastWasOpen = false;
            inQuantifierVars = false;
            inHOL = false;
            inHOLCount = 0;

            st = new StreamTokenizer_s(new StringReader(suoString));
            KIF.setupStreamTokenizer(st);

            do {
                st.nextToken();
                //----Open bracket
                if (st.ttype==40) {
                    if (lastWasOpen) {    //----Should not have ((in KIF
                        System.out.println("ERROR: Double open bracket at " + tptpFormula);
                        throw new ParseException("Parsing error in " + suoString,0);
                    }
                    //----Track nesting of ()s for hol__, so I know when to close the '
                    if (inHOL) {
                        inHOLCount++;
                    }
                    lastWasOpen = true;
                    parenLevel++;
                    //----Operators
                } else if (st.ttype == StreamTokenizer.TT_WORD && 
                           (arity = operatorArity(st)) > 0) {
                    //----Operators must be preceded by a (
                    if (!lastWasOpen) {   
                        System.out.println("ERROR: Missing ( before " + 
                                           st.sval + " at " + tptpFormula);
                        return(null);
                    }
                    //----This is the start of a new term - put in the infix operator if not the
                    //----first term for this operator
                    if ((Integer)(countStack.peek()) > 0) {

                        // System.out.println("  1 : countStack == " + countStack);
                        // System.out.println("  1 : operatorStack == " + operatorStack);

                        tptpFormula.append((String)operatorStack.peek()); 
                    }
                    //----If this is the start of a hol__ situation, quote it all
                    if (inHOL && inHOLCount == 1) {
                        tptpFormula.append("'");
                    }
                    //----()s around all operator expressions
                    tptpFormula.append("(");      
                    //----Output unary as prefix
                    if (arity == 1) {
                        tptpFormula.append(translateWord(st,false));
                        //----Note the new operator (dummy) with 0 operands so far
                        countStack.push(new Integer(0));
                        operatorStack.push(",");
                        //----Check if the next thing will be the quantified variables
                        if (st.sval.equals("forall") || st.sval.equals("exists")) {
                            inQuantifierVars = true;
                        }
                        //----Binary operator
                    } else if (arity == 2) {
                        //----Note the new operator with 0 operands so far
                        countStack.push(new Integer(0));
                        operatorStack.push(translateWord(st,false));
                    }
                    lastWasOpen = false;      
                    //----Back tick - token translation to TPTP. Everything gets ''ed 
                } else if (st.ttype == 96) {
                    //----They may be nested - only start the situation at the outer one
                    if (!inHOL) {
                        inHOL = true;
                        inHOLCount = 0;
                    }
                    //----Quote - Term token translation to TPTP
                } else if (st.ttype == 34 ||
                           st.ttype == StreamTokenizer.TT_NUMBER || 
                           (st.sval != null && (Character.isDigit(st.sval.charAt(0)))) ||
                           st.ttype == StreamTokenizer.TT_WORD) {      
                    //----Start of a predicate or variable list
                    if (lastWasOpen) {
                        //----Variable list
                        if (inQuantifierVars) {
                            tptpFormula.append("[");
                            tptpFormula.append(translateWord(st,false));
                            incrementTOS(countStack);
                            //----Predicate
                        } else {
                            //----This is the start of a new term - put in the infix operator if not the
                            //----first term for this operator
                            if ((Integer)(countStack.peek()) > 0) {
                                tptpFormula.append((String)operatorStack.peek());
                            }
                            //----If this is the start of a hol__ situation, quote it all
                            if (inHOL && inHOLCount == 1) {
                                tptpFormula.append("'");
                            }
                            //----Predicate or function and (
                            tptpFormula.append(translateWord(st,true));
                            tptpFormula.append("(");
                            //----Note the , for between arguments with 0 arguments so far
                            countStack.push(new Integer(0));
                            operatorStack.push(",");
                        }
                        //----Argument or quantified variable
                    } else {
                        //----This is the start of a new term - put in the infix operator if not the
                        //----first term for this operator
                        if ((Integer)(countStack.peek()) > 0) {
                            tptpFormula.append((String)operatorStack.peek());
                        }
                        //----Output the word
                        tptpFormula.append(translateWord(st,false));
                        //----Increment counter for this level
                        incrementTOS(countStack);
                    }
                    //----Collect variables that are used and quantified
                    if (isNonEmptyString(st.sval) && (st.sval.charAt(0) == '?' || st.sval.charAt(0) == '@')) {
                        if (inQuantifierVars) {
                            addVariable(st,quantifiedVariables);
                        } else {
                            addVariable(st,allVariables);
                        }
                    }
                    lastWasOpen = false; 
                    //----Close bracket.
                } else if (st.ttype==41) {
                    //----Track nesting of ()s for hol__, so I know when to close the '
                    if (inHOL) {
                        inHOLCount--;
                    }
                    //----End of quantified variable list
                    if (inQuantifierVars) {
                        //----Fake restarting the argument list because the quantified variable list
                        //----does not use the operator from the surrounding expression
                        countStack.pop();
                        countStack.push(0);
                        tptpFormula.append("] : ");
                        inQuantifierVars = false;
                        //----End of predicate or operator list
                    } else {
                        //----Pop off the stacks to reveal the next outer layer
                        countStack.pop();
                        operatorStack.pop();
                        //----Close the expression
                        tptpFormula.append(")");  
                        //----If this closes a HOL expression, close the '
                        if (inHOL && inHOLCount == 0) {
                            tptpFormula.append("'");
                            inHOL = false;
                        }
                        //----Note that another expression has been completed
                        incrementTOS(countStack);
                    }
                    lastWasOpen = false;
      
                    parenLevel--;
                    //----End of the statement being processed. Universally quantify free variables
                    if (parenLevel == 0) {
                        //findFreeVariables(allVariables,quantifiedVariables);
                        allVariables.removeAll(quantifiedVariables);
                        if (allVariables.size() > 0) {
                            quantification = "! [";
                            for (index = 0; index < allVariables.size(); index++) {
                                if (index > 0) {
                                    quantification += ",";
                                }
                                quantification += (String)allVariables.elementAt(index);
                            }
                            quantification += "] : ";
                            tptpFormula.insert(0,"( " + quantification);
                            tptpFormula.append(" )");
                        }
                        if (StringUtil.emptyString(translatedFormula)) {
                            translatedFormula = "( " + tptpFormula.toString() + " )";
                        } else {
                            translatedFormula += "& ( " + tptpFormula.toString() + " )";
                        }

                        if ((Integer)(countStack.pop()) != 1) {
                            System.out.println("Error in KIF.tptpParse(): Not one formula");
                        }
                    } else if (parenLevel < 0) {
                        System.out.print("ERROR: Extra closing bracket at " + 
                                         tptpFormula.toString());
                        throw new ParseException("Parsing error in " + suoString,0);
                    }      
                } else if (st.ttype != StreamTokenizer.TT_EOF) {
                    System.out.println("ERROR: Illegal character '" +
                                       (char)st.ttype + "' at " + tptpFormula.toString());
                    throw new ParseException("Parsing error in " + suoString,0);
                }      
            } while (st.ttype != StreamTokenizer.TT_EOF);

            //----Bare word like $false didn't get done by a closing)
            if (StringUtil.emptyString(translatedFormula)) {
                translatedFormula = tptpFormula.toString();
            }
        }
        catch (Exception ex2) {
            System.out.println("Error in Formula.tptpParseSUOKIFString(" + suoString + ")");
            System.out.println("  st.sval == " + st.sval);
            System.out.println("  message == " + ex2.getMessage());
            ex2.printStackTrace();
        }

        /*
          System.out.println("  ==> " + translatedFormula);
          System.out.println("EXIT Formula.tptpParseSUOKIFString(\""
          + suoString
          + "\")");
        */

        return translatedFormula;
    }

    /** ***************************************************************
     * Parse formulae into TPTP format
     */
    public void tptpParse(boolean query, KB kb, List preProcessedForms) 
        throws ParseException, IOException {
        if (DEBUG) {
            System.out.println("ENTER Formula.tptpParse(" 
                               + this
                               + ", "
                               + query
                               + ", "
                               + kb
                               + ", "
                               + preProcessedForms
                               + ")");
        }

        try {
            KBmanager mgr = KBmanager.getMgr();

            if (kb == null) 
                kb = new KB("",mgr.getPref("kbDir"));

            if (!this.isBalancedList()) {
                String errStr = "Unbalanced parentheses or quotes";
                System.out.println("Error in Formula.tptpParse(" 
                                   + this.theFormula
                                   + ", "
                                   + query 
                                   + ", " 
                                   + kb.name 
                                   + ", "
                                   + preProcessedForms
                                   + ")");
                System.out.println("  " + errStr);
                mgr.setError(mgr.getError()
                             + ("\n<br/>" 
                                + errStr 
                                + " in "
                                + this.theFormula
                                + "\n<br/>"));
                return;
            }
      
            List processed;
            if (preProcessedForms != null) {
                processed = preProcessedForms;
            } else {
                processed = this.preProcess(query, kb);
            }

            if (DEBUG) {
                System.out.println("  processed == " + processed);
            }

            //     System.out.println("INFO in Formula.tptpParse(" + this.theFormula + ")");
            //     System.out.println("  processed == " + processed);

            if (processed != null) {
                this.clearTheTptpFormulas();

                //----Performs function on each current processed axiom
                Formula f = null;
                for (Iterator g = processed.iterator(); g.hasNext();) {
                    f = (Formula) g.next();

                    if (DEBUG) {
                        System.out.println("  f == " + f);
                    }

                    this.getTheTptpFormulas().add(tptpParseSUOKIFString(f.theFormula));
                }

                //         System.out.println("INFO in Formula.tptpParse(" + this.theFormula + ")");
                //         System.out.println("  theTptpFormulas == " + this.getTheTptpFormulas());

            }
        }
        catch (Exception ex) {
            System.out.println("Error in Formula.tptpParse("
                               + this
                               + ", "
                               + query
                               + ", "
                               + kb
                               + ", "
                               + preProcessedForms
                               + "): "
                               + ex.getMessage());
            ex.printStackTrace();
            if (ex instanceof ParseException) {
                throw (ParseException) ex;
            }    
            if (ex instanceof IOException) {
                throw (IOException) ex;
            }
        }
        if (DEBUG) {
            System.out.println("EXIT Formula.tptpParse(" 
                               + this
                               + ", "
                               + query
                               + ", "
                               + kb
                               + ", "
                               + preProcessedForms
                               + ")");
        }

        return;
    }

    /** ***************************************************************
     * Parse formulae into TPTP format
     */
    public void tptpParse(boolean query, KB kb) throws ParseException, IOException {
        this.tptpParse(query, kb, null);
        return;
    }


    ///////////////////////////////////////////////////////
    /*
      START of instantiatePredVars(KB kb) implementation.
    */
    ///////////////////////////////////////////////////////

    /** ***************************************************************
     * Returns an ArrayList of the Formulae that result from replacing
     * all arg0 predicate variables in the input Formula with
     * predicate names.
     *
     * @param kb A KB that is used for processing the Formula.
     * 
     * @return An ArrayList of Formulas, or an empty ArrayList if no
     * instantiations can be generated.
     */
    public ArrayList instantiatePredVars(KB kb) {

        // boolean trace = this.theFormula.contains("(subrelation");
        // if (trace) {
        //     System.out.println("ENTER Formula.instantiatePredVars(" + kb.name + ")");
        // }

        ArrayList ans = new ArrayList();

        try {
            if (this.listP()) {
                String arg0 = this.getArgument(0);


                // First we do some checks to see if it is worth
                // processing the formula.
                if (isLogicalOperator(arg0)
                    && this.theFormula.matches(".*\\(\\s*\\?.*")) {
       
                    // Get all query lits for all pred vars, indexed by
                    // var.
                    List indexedQueryLits = this.prepareIndexedQueryLiterals(kb);

                    if (indexedQueryLits == null) {
                        ans.add(this);
                    }
                    else {

                        List substForms = new ArrayList();
                        List varQueryTuples = null;
                        List substTuples = null;
                        List litsToRemove = null;

                        // First, gather all substitutions.
                        Iterator it1 = indexedQueryLits.iterator();
                        while (it1.hasNext()) {
                            varQueryTuples = (List) it1.next();
                            substTuples = computeSubstitutionTuples(kb, varQueryTuples);
                            if ((substTuples instanceof List) && !(substTuples.isEmpty())) {
                                if (substForms.isEmpty()) {
                                    substForms.add(substTuples);
                                }
                                else {
                                    int stSize = substTuples.size();
                                    int iSize = -1;
                                    int sfSize = substForms.size();
                                    int sfLast = (sfSize - 1);
                                    for (int i = 0 ; i < sfSize ; i++) {
                                        iSize = ((List) substForms.get(i)).size();
                                        if (stSize < iSize) {
                                            substForms.add(i, substTuples);
                                            break;
                                        }
                                        if (i == sfLast) {
                                            substForms.add(substTuples);
                                        }
                                    }
                                }
                            }
                        }

                        if (!substForms.isEmpty()) {

                            // Try to simplify the Formula.
                            Formula f = this;
                            it1 = substForms.iterator();
                            Iterator it2 = null;
                            while (it1.hasNext()) {
                                substTuples = (List) it1.next();
                                litsToRemove = (List) substTuples.get(0);
                                it2 = litsToRemove.iterator();
                                while (it2.hasNext()) {
                                    List lit = (List) it2.next();
                                    f = f.maybeRemoveMatchingLits(lit);
                                }
                            }

                            // Now generate pred var instantions from the
                            // possibly simplified formula.
                            List templates = new ArrayList();
                            templates.add(f.theFormula);
                            Set accumulator = new HashSet();

                            String template = null;
                            String var = null;
                            String term = null;
                            ArrayList quantVars = null;
                            int i = 0;

                            // Iterate over all var plus query lits forms, getting
                            // a list of substitution literals.
                            it1 = substForms.iterator();
                            while (it1.hasNext()) {
                                substTuples = (List) it1.next();

                                if ((substTuples instanceof List) && !(substTuples.isEmpty())) {

                                    // Iterate over all ground lits ...

                                    // Remove litsToRemove, which we have
                                    // already used above.
                                    litsToRemove = (List) substTuples.remove(0);

                                    // Remove and hold the tuple that
                                    // indicates the variable substitution
                                    // pattern.
                                    List varTuple = (List) substTuples.remove(0);

                                    it2 = substTuples.iterator();
                                    while (it2.hasNext()) {
                                        List groundLit = (List) it2.next();

                                        // Iterate over all formula templates,
                                        // substituting terms from each ground lit
                                        // for vars in the template.
                                        Iterator it3 = templates.iterator();
                                        while (it3.hasNext()) {
                                            template = (String) it3.next();
                                            quantVars = collectQuantifiedVariables(template);
                                            for (i = 0 ; i < varTuple.size() ; i++) {
                                                var = (String) varTuple.get(i);
                                                if (isVariable(var)) {

                                                    term = (String) groundLit.get(i);

                                                    // Don't replace variables that
                                                    // are explicitly quantified.
                                                    if (!quantVars.contains(var)) {

                                                        List patternStrings = 
                                                            Arrays.asList("(\\W*\\()(\\s*holds\\s+\\" + var + ")(\\W+)",
                                                                          // "(\\W*\\()(\\s*\\" + var + ")(\\W+)",
                                                                          "(\\W*)(\\" + var + ")(\\W+)" 
                                                                          );
                                                        List patterns = new ArrayList();
                                                        for (int j = 0; j < patternStrings.size(); j++) {
                                                            patterns.add(Pattern.compile((String)(patternStrings.get(j))));
                                                        }
                                                        Pattern p = null;
                                                        Matcher m = null;
                                                        for (int j = 0 ; j < patterns.size() ; j++) {
                                                            p = (Pattern) patterns.get(j);
                                                            m = p.matcher(template);
                                                            template = m.replaceAll("$1" + term + "$3");
                                                        }
                                                    }
                                                }
                                            }
                                            accumulator.add(template);
                                        }
                                    }
                                    templates.clear();
                                    templates.addAll(accumulator);
                                    accumulator.clear();
                                }
                            }
                            ans.addAll(KB.stringsToFormulas(templates));
                        }
                        if (ans.isEmpty()) {
                            ans.add("reject");
                        }
                    }
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        /*
          if (trace) {
          System.out.println("EXIT Formula.instantiatePredVars(" 
          + this.theFormula + ", "
          + kb.name + ")");
          System.out.println("  ==> " 
          + ((ans.size() > 20) 
          ? (ans.subList(0, 5) 
          + " and " 
          + (ans.size() - 5) 
          + " more ...")
          : ans));
          }
        */
        return ans;
    }

    /** ***************************************************************
     * Returns the number of SUO-KIF variables (only ? variables, not
     * @ROW variables) in the input query literal.
     *
     * @param queryLiteral A List representing a Formula.
     *
     * @return An int.
     */
    private static int getVarCount(List queryLiteral) {
        int ans = 0;
        if (queryLiteral instanceof List) {
            String term = null;
            for (int i = 0 ; i < queryLiteral.size() ; i++) {
                term = (String) queryLiteral.get(i);
                if (term.startsWith("?")) {
                    ans++;
                }
            }
        }
        return ans;
    }

    /** ***************************************************************
     * This method returns an ArrayList of query answer literals.  The
     * first element is an ArrayList of query literals that might be
     * used to simplify the Formula to be instantiated.  The second
     * element is the query literal (ArrayList) that will be used as a
     * template for doing the variable substitutions.  All subsequent
     * elements are ground literals (ArrayLists).
     *
     * @param kb A KB to query for answers.
     *
     * @param queryLits A List of query literals.  The first item in
     * the list will be a SUO-KIF variable (String), which indexes the
     * list.  Each subsequent item is a query literal (List).
     *
     * @return An ArrayList of literals, or an empty ArrayList of no
     * query answers can be found.
     */
    private static ArrayList computeSubstitutionTuples(KB kb, List queryLits) {

        // System.out.println("ENTER computeSubstitutionTuples(" + kb + ", " + queryLits + ")");

        ArrayList result = new ArrayList();
        try {
            if ((kb instanceof KB) 
                && (queryLits instanceof List)
                && !(queryLits.isEmpty())) {

                String idxVar = (String) queryLits.get(0);

                int i = 0;
                int j = 0;

                // Sort the query lits by number of variables.
                ArrayList sortedQLits = new ArrayList();
                for (i = 1 ; i < queryLits.size() ; i++) {
                    ArrayList ql = (ArrayList) queryLits.get(i);
                    int varCount = getVarCount(ql);
                    boolean added = false;
                    for (j = 0 ; j < sortedQLits.size() ; j++) {
                        ArrayList ql2 = (ArrayList) sortedQLits.get(j);
                        if (varCount > getVarCount(ql2)) {
                            sortedQLits.add(j, ql);
                            added = true;
                            break;
                        }           
                    }
                    if (! added) {
                        sortedQLits.add(ql);
                    }
                }

                // Literals that will be used to try to simplify the
                // formula before pred var instantiation.
                ArrayList simplificationLits = new ArrayList();

                // The literal that will serve as the pattern for
                // extracting var replacement terms from answer
                // literals.
                ArrayList keyLit = null;

                // The list of answer literals retrieved using the
                // query lits, possibly built up via a sequence of
                // multiple queries.
                ArrayList answers = null;

                Set working = new HashSet();
                ArrayList accumulator = null;
                ArrayList ql = null;

                boolean satisfiable = true;
               
                // The first query lit for which we get an answer is
                // the key lit.
                for (i = 0 ; (i < sortedQLits.size()) && satisfiable ; i++) {
                    ql = (ArrayList) sortedQLits.get(i);
                    accumulator = kb.askWithLiteral(ql);
                    satisfiable = (! ((accumulator == null) || accumulator.isEmpty()));

                    // System.out.println(ql + " accumulator == " + accumulator);

                    if (satisfiable) {

                        simplificationLits.add(ql);

                        if (keyLit == null) {
                            keyLit = ql;
                            answers = KB.formulasToArrayLists(accumulator);
                        }
                        else {  // if (accumulator.size() < answers.size()) {
                            accumulator = KB.formulasToArrayLists(accumulator);

                            // Winnow the answers list.
                            working.clear();
                            ArrayList ql2 = null;
                            int varPos = ql.indexOf(idxVar);
                            for (j = 0 ; j < accumulator.size() ; j++) {
                                ql2 = (ArrayList) accumulator.get(j);
                                working.add((String)ql2.get(varPos));
                            }
                            accumulator.clear();
                            accumulator.addAll(answers);
                            answers.clear();
                            varPos = keyLit.indexOf(idxVar);
                            for (j = 0 ; j < accumulator.size() ; j++) {
                                ql2 = (ArrayList) accumulator.get(j);
                                if (working.contains((String)ql2.get(varPos))) {
                                    answers.add(ql2);
                                }
                            }
                        }
                    }
                }
                if (satisfiable && (keyLit != null)) {
                    result.add(simplificationLits);
                    result.add(keyLit);
                    result.addAll(answers);
                }
                else {
                    result.clear();
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }

        //     System.out.println("EXIT computeSubstitutionTuples(" + kb + ", " + queryLits + ")");
        //     System.out.println("  -> " + result);

        return result;
    }

    /** ***************************************************************
     * This method returns an ArrayList in which each element is
     * another ArrayList.  The head of each element is a variable.
     * The subsequent objects in each element are query literals
     * (ArrayLists).
     *
     * @param kb The KB used for computations involving assertions.
     *
     * @return An ArrayList, or null if the input formula contains no
     * predicate variables.
     */
    private ArrayList prepareIndexedQueryLiterals(KB kb) {

        // System.out.println("ENTER prepareIndexedQueryLiterals(" + this + ")");

        ArrayList ans = null;
        HashMap varsWithTypes = this.gatherPredVars(kb);
        // System.out.println("vars == " + vars);

        if (! varsWithTypes.isEmpty()) {

            String yOrN = (String) varsWithTypes.get("arg0");

            // If the formula doesn't contain any arg0 pred vars, do
            // nothing.
            if (isNonEmptyString(yOrN) && yOrN.equalsIgnoreCase("yes")) {

                ans = new ArrayList();
           
                // Try to simplify the formula.
                ArrayList varWithTypes = null;
                ArrayList indexedQueryLits = null;

                String var = null;
                Iterator it = varsWithTypes.keySet().iterator();
                while (it.hasNext()) {
                    var = (String) it.next();
                    if (isVariable(var)) {
                        varWithTypes = (ArrayList) varsWithTypes.get(var);
                        indexedQueryLits = gatherPredVarQueryLits(kb, varWithTypes);
                        if (! indexedQueryLits.isEmpty()) {
                            ans.add(indexedQueryLits);
                        }
                    }
                }
            }
        }

        //     System.out.println("EXIT prepareIndexedQueryLiterals(" + this + ")");
        //     System.out.println("  -> " + ans);

        return ans;
    }

    /** ***************************************************************
     * This method collects and returns all predicate variables that
     * occur in the Formula.
     *
     * @param kb The KB to be used for computations involving
     * assertions.
     *
     * @return a HashMap in which the keys are predicate variables,
     * and the values are ArrayLists containing one or more class
     * names that indicate the type constraints tha apply to the
     * variable.  If no predicate variables can be gathered from the
     * Formula, the HashMap will be empty.  The first element in each
     * ArrayList is the variable itself.  Subsequent elements are the
     * types of the variable.  If no types for the variable can be
     * determined, the ArrayList will contain just the variable.
     *
     */
    protected HashMap gatherPredVars(KB kb) {

        // System.out.println("ENTER gatherPredVars(" +  this + ")");

        HashMap ans = new HashMap();
        try {
            if (isNonEmptyString(this.theFormula)) {
                List accumulator = new ArrayList();
                List working = new ArrayList();
                if (this.listP() && !(this.empty())) {
                    accumulator.add(this);
                }
                while (! accumulator.isEmpty()) {
                    working.clear();
                    working.addAll(accumulator);
                    accumulator.clear();
                    Formula f = null;
                    String arg0 = null;
                    String arg2 = null;
                    ArrayList vals = null;
                    int len = -1;
                    for (int i = 0 ; i < working.size() ; i++) {
                        f = (Formula) working.get(i);
                        len = f.listLength();
                        arg0 = f.getArgument(0);
                        if (isQuantifier(arg0)
                            || arg0.equals("holdsDuring")
                            || arg0.equals("KappaFn")) {
                            if (len > 2) {
                                arg2 = f.getArgument(2);

                                Formula newF = new Formula();
                                newF.read(arg2);
                                if (f.listP() && !f.empty()) {
                                    accumulator.add(newF);
                                }
                            }
                            else {
                                System.out.println("INFO in Formula.gatherPredVars(" + this + ")");
                                System.out.println("Is this malformed? " + f.theFormula);
                            }
                        }
                        else if (arg0.equals("holds")) {
                            accumulator.add(f.cdrAsFormula());
                        }
                        else if (isVariable(arg0)) {
                            vals = (ArrayList) ans.get(arg0);
                            if (vals == null) {
                                vals = new ArrayList();
                                ans.put(arg0, vals);
                                vals.add(arg0);
                            }
                            // Record the fact that we found at least
                            // one variable in the arg0 position.
                            ans.put("arg0", "yes");
                        }
                        else { 
                            String argN = null;
                            Formula argF = null;
                            String argType = null;
                            boolean[] signature = kb.getRelnArgSignature(arg0);
                            for (int j = 1 ; j < len ; j++) {
                                argN = f.getArgument(j);
                                if ((signature != null) && signature[j] && isVariable(argN)) {
                                    vals = (ArrayList) ans.get(argN);
                                    if (vals == null) {
                                        vals = new ArrayList();
                                        ans.put(argN, vals);
                                        vals.add(argN);
                                    }
                                    argType = kb.getArgType(arg0, j);
                                    if (! ((argType == null) || vals.contains(argType))) {
                                        vals.add(argType);
                                    }
                                }
                                else {
                                    argF = new Formula();
                                    argF.read(argN);
                                    if (argF.listP() && !(argF.empty())) {
                                        accumulator.add(argF);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }

        // System.out.println("INFO in gatherPredVars(" +  this + ")");
        // System.out.println("  -> " + ans);

        return ans;
    }

    /** ***************************************************************
     * This method tries to remove literals from the Formula that
     * match litArr.  It is intended for use in simplification of this
     * Formula during predicate variable instantiation, and so only
     * attempts removals that are likely to be safe in that context.
     *
     * @param litArr A List object representing a SUO-KIF atomic
     * formula.
     *
     * @return A new Formula with at least some occurrences of litF
     * removed, or the original Formula if no removals are possible.
     */
    private Formula maybeRemoveMatchingLits(List litArr) {
        Formula f = KB.literalListToFormula(litArr);
        return maybeRemoveMatchingLits(f);
    }

    /** ***************************************************************
     * This method tries to remove literals from the Formula that
     * match litF.  It is intended for use in simplification of this
     * Formula during predicate variable instantiation, and so only
     * attempts removals that are likely to be safe in that context.
     *
     * @param litF A SUO-KIF literal (atomic Formula).
     *
     * @return A new Formula with at least some occurrences of litF
     * removed, or the original Formula if no removals are possible.
     */
    private Formula maybeRemoveMatchingLits(Formula litF) {

        // System.out.println("ENTER maybeRemoveMatchingLits(" + litF + ") ");
        Formula result = null;
        try {
            Formula f = this;
            if (f.listP() && !f.empty()) {
                StringBuilder litBuf = new StringBuilder();
                String arg0 = f.car();
                if (f.isRule()
                    // arg0.equals("<=>") || 
                    // arg0.equals("=>") 
                    ) {
                    String arg1 = f.getArgument(1);
                    String arg2 = f.getArgument(2);
                    if (arg1.equals(litF.theFormula)) {
                        Formula arg2F = new Formula();
                        arg2F.read(arg2);
                        litBuf.append(arg2F.maybeRemoveMatchingLits(litF).theFormula);
                    }
                    else if (arg2.equals(litF.theFormula)) {
                        Formula arg1F = new Formula();
                        arg1F.read(arg1);
                        litBuf.append(arg1F.maybeRemoveMatchingLits(litF).theFormula);
                    }
                    else {
                        Formula arg1F = new Formula();
                        arg1F.read(arg1);
                        Formula arg2F = new Formula();
                        arg2F.read(arg2);
                        litBuf.append("(" + arg0 + " " 
                                      + arg1F.maybeRemoveMatchingLits(litF).theFormula + " "
                                      + arg2F.maybeRemoveMatchingLits(litF).theFormula + ")");
                    }
                }
                else if (isQuantifier(arg0)
                         || arg0.equals("holdsDuring") 
                         || arg0.equals("KappaFn")) {
                    Formula arg2F = new Formula();
                    arg2F.read(f.caddr());
                    litBuf.append("(" + arg0 + " " + f.cadr() + " " 
                                  + arg2F.maybeRemoveMatchingLits(litF).theFormula + ")");
                }
                else if (isCommutative(arg0)) {
                    List litArr = f.literalToArrayList();
                    if (litArr.contains(litF.theFormula)) {
                        litArr.remove(litF.theFormula);
                    }
                    String args = "";
                    int len = litArr.size();
                    for (int i = 1 ; i < len ; i++) {
                        Formula argF = new Formula();
                        argF.read((String) litArr.get(i));
                        args += (" " + argF.maybeRemoveMatchingLits(litF).theFormula);
                    }
                    if (len > 2) {
                        args = ("(" + arg0 + args + ")");
                    }
                    else {
                        args = args.trim();
                    }
                    litBuf.append(args);
                }
                else {
                    litBuf.append(f.theFormula);
                }
                Formula newF = new Formula();
                newF.read(litBuf.toString());
                result = newF;
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        if (result == null) {
            result = this;
        }

        // System.out.println("EXIT maybeRemoveMatchingLits(" + litF + ")");
        // System.out.println("  -> " + result);

        return result;
    }

    /** ***************************************************************
     * Return true if the input predicate can take relation names a
     * arguments, else returns false.
     */
    private boolean isPossibleRelnArgQueryPred (KB kb, String predicate) {
        return (isNonEmptyString(predicate)
                && ((kb.getRelnArgSignature(predicate) != null)
                    ||
                    predicate.equals("instance")
                    )
                );
    }

    /** ***************************************************************
     * This method collects and returns literals likely to be of use
     * as templates for retrieving predicates to be substituted for
     * var.
     *
     * @param varWithTypes A List containing a variable followed,
     * optionally, by class names indicating the type of the variable.
     *
     * @return An ArrayList of literals (Lists) with var at the head.
     * The first element of the ArrayList is the variable (String).
     * Subsequent elements are Lists corresponding to SUO-KIF
     * formulas, which will be used as query templates.
     *
     */
    private ArrayList gatherPredVarQueryLits(KB kb, List varWithTypes) {

        // System.out.println("ENTER gatherPredVarQueryLits(" +  this + ", " + kb + ", " + varWithTypes + ")");

        ArrayList ans = new ArrayList();
        try {
            String var = (String) varWithTypes.get(0);
            Set added = new HashSet();

            // Get the clauses for this Formula.
            StringBuilder litBuf = new StringBuilder();
            List clauses = getClauses();
            Map varMap = getVarMap();
            String qlString = null;
            ArrayList queryLit = null;

            if (clauses != null) {
                Iterator it2 = null;
                Formula f = null;
                Iterator it1 = clauses.iterator();
                while (it1.hasNext()) {
                    List clause = (List) it1.next();
                    List negLits = (List) clause.get(0);
                    // List poslits = (List) clause.get(1);

                    if (! negLits.isEmpty()) {
                        int flen = -1;
                        String arg = null;
                        String arg0 = null;
                        String term = null;
                        String origVar = null;
                        boolean working = true;
                        for (int ci = 0 ; 
                             ci < 1 ;
                             // (ci < clause.size()) && ans.isEmpty() ; 
                             ci++) {
                            // Try the neglits first.  Then try the poslits only
                            // if there still are no resuls.
                            it2 = ((List)(clause.get(ci))).iterator();
                            while (it2.hasNext()) {
                                f = (Formula) it2.next();
                                if (f.theFormula.matches(".*SkFn\\s+\\d+.*") 
                                    || f.theFormula.matches(".*Sk\\d+.*")) {
                                    continue;
                                }
                                flen = f.listLength();
                                arg0 = f.getArgument(0);

                                // System.out.println("  var == " + var);
                                // System.out.println("  f.theFormula == " + f.theFormula);
                                // System.out.println("  arg0 == " + arg0);

                                if (isNonEmptyString(arg0)) {

                                    // If arg0 corresponds to var, then var
                                    // has to be of type Predicate, not of
                                    // types Function or List.
                                    if (isVariable(arg0)) {
                                        origVar = getOriginalVar(arg0, varMap);
                                        if (origVar.equals(var) 
                                            && !varWithTypes.contains("Predicate")) {
                                            varWithTypes.add("Predicate");
                                        }
                                    }
                                    else {
                                        queryLit = new ArrayList();
                                        queryLit.add(arg0);
                                        boolean foundVar = false;
                                        for (int i = 1 ; i < flen ; i++) {
                                            arg = f.getArgument(i);
                                            if (! listP(arg)) {
                                                if (isVariable(arg)) {
                                                    arg = getOriginalVar(arg, varMap);
                                                    if (arg.equals(var)) {
                                                        foundVar = true;
                                                    }
                                                }
                                                queryLit.add(arg);
                                            }
                                        }
                                        if (queryLit.size() != flen) {
                                            continue;
                                        }

                                        // If the literal does not start with a
                                        // variable or with "holds" and does not
                                        // contain Skolem terms, but does contain
                                        // the variable in which we're interested,
                                        // it is probably suitable as a query
                                        // template, or might serve as a starting
                                        // place.  Use it, or a literal obtained
                                        // with it.
                                        if (isPossibleRelnArgQueryPred(kb, arg0) && foundVar) {
                                            // || arg0.equals("disjoint")) 
                                            term = "";
                                            if (queryLit.size() > 2) {
                                                term = (String) queryLit.get(2);
                                            }
                                            if (! (arg0.equals("instance") && term.equals("Relation"))) {
                                                if (! added.contains(queryLit.toString().intern())) {
                                                    ans.add(queryLit);
                                                    added.add(queryLit.toString().intern());
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // If we have previously collected type info for the variable,
            // convert that info query lits now.
            String argType = null;
            int vtLen = varWithTypes.size();
            if (vtLen > 1) {
                for (int j = 1 ; j < vtLen ; j++) {
                    argType = (String) varWithTypes.get(j);
                    if (! argType.equals("Relation")) {
                        queryLit = new ArrayList();
                        queryLit.add("instance");
                        queryLit.add(var);
                        queryLit.add(argType);
                        qlString = queryLit.toString().intern();
                        if (!added.contains(qlString)) {
                            ans.add(queryLit);
                            added.add(qlString);
                        }
                    }
                }
            }

            // Add the variable to the front of the answer list, if it contains
            // any query literals.
            if (! ans.isEmpty()) {
                ans.add(0, var);
            }

            //         System.out.println("EXIT gatherPredVarQueryLits(" + this + ", " + kb + ", " + varWithTypes + ")");
            //         System.out.println("  -> " + ans);
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return ans;
    }


    ///////////////////////////////////////////////////////
    /*
      END of instantiatePredVars(KB kb) implementation.
    */
    ///////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////
    /*
      START of clausify() implementation.

      The code in the section below implements an algorithm for
      translating SUO-KIF expressions to clausal form.  The public
      methods are:

      public Formula clausify()
      public ArrayList clausifyWithRenameInfo()
      public ArrayList toNegAndPosLitsWithRenameInfo()
    */
    ///////////////////////////////////////////////////////

    /** ***************************************************************
     * This static variable holds the int value that is used to
     * generate unique variable names.
     */
    private static int VAR_INDEX = 0;

    /** ***************************************************************
     * This static variable holds the int value that is used to
     * generate unique Skolem terms.
     */
    private static int SKOLEM_INDEX = 0;

    /** ***************************************************************
     * This method converts the SUO-KIF Formula to a version of
     * clausal (resolution, conjunctive normal) form with Skolem
     * functions, following the procedure described in Logical
     * Foundations of Artificial Intelligence, by Michael Genesereth
     * and Nils Nilsson, 1987, pp. 63-66.
     *
     * <p>A literal is an atomic formula.  (Note, however, that
     * because SUO-KIF allows higher-order formulae, not all SUO-KIF
     * literals are really atomic.)  A clause is a disjunction of
     * literals that share no variable names with literals in any
     * other clause in the KB.  Note that even a relatively simple
     * SUO-KIF formula might generate multiple clauses.  In such
     * cases, the Formula returned will be a conjunction of clauses.
     * (A KB is understood to be a conjunction of clauses.)  In all
     * cases, the Formula returned by this method should be a
     * well-formed SUO-KIF Formula if the input (original formula) is
     * a well-formed SUO-KIF Formula.  Rendering the output in true
     * (LISPy) clausal form would require an additional step, the
     * removal of all commutative logical operators, and the result
     * would not be well-formed SUO-KIF.</p>
     *
     * @see clausifyWithRenameInfo()
     * @see toNegAndPosLitsWithRenameInfo()
     * @see toCanonicalClausalForm();
     *
     * @return A SUO-KIF Formula in clausal form, or null if a clausal
     * form cannot be generated.
     */
    public Formula clausify() {

        // System.out.println("ENTER Formula.clausify(" + this + ")");

        Formula ans = null;
        try {
            ans = equivalencesOut();
            ans = ans.implicationsOut();
            ans = ans.negationsIn();
            ans = ans.renameVariables();
            ans = ans.existentialsOut();
            ans = ans.universalsOut();
            ans = ans.disjunctionsIn();
            ans = ans.standardizeApart();
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }

        // System.out.println("  ==> " + ans);

        return ans;
    }

    /** ***************************************************************
     * This method converts the SUO-KIF Formula to a version of
     * clausal (resolution, conjunctive normal) form with Skolem
     * functions, following the procedure described in Logical
     * Foundations of Artificial Intelligence, by Michael Genesereth
     * and Nils Nilsson, 1987, pp. 63-66.
     *
     * <p>It returns an ArrayList that contains three items: The new
     * clausal-form Formula, the original (input) SUO-KIF Formula, and
     * a Map containing a graph of all the variable substitions done
     * during the conversion to clausal form.  This Map makes it
     * possible to retrieve the correspondence between the variables
     * in the clausal form and the variables in the original
     * Formula.</p>
     *
     * @see clausify()
     * @see toNegAndPosLitsWithRenameInfo()
     * @see toCanonicalClausalForm();
     *
     * @return A three-element ArrayList, [<Formula>, <Formula>,
     * <Map>], in which some elements might be null if a clausal form
     * cannot be generated.
     */
    public ArrayList clausifyWithRenameInfo() {
        ArrayList result = new ArrayList();
        Formula ans = null;
        try {
            HashMap topLevelVars  = new HashMap();
            HashMap scopedRenames = new HashMap();
            HashMap allRenames    = new HashMap();
            HashMap standardizedRenames = new HashMap();
            ans = equivalencesOut();
            ans = ans.implicationsOut();
            ans = ans.negationsIn();
            ans = ans.renameVariables(topLevelVars, scopedRenames, allRenames);
            ans = ans.existentialsOut();
            ans = ans.universalsOut();
            ans = ans.disjunctionsIn();
            ans = ans.standardizeApart(standardizedRenames);
            allRenames.putAll(standardizedRenames);
            result.add(ans);
            result.add(this);
            result.add(allRenames);
            // resetClausifyIndices();
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return result;
    }

    /** ***************************************************************
     * This method converts the SUO-KIF Formula to an ArrayList of
     * clauses.  Each clause is an ArrayList containing an ArrayList
     * of negative literals, and an ArrayList of positive literals.
     * Either the neg lits list or the pos lits list could be empty.
     * Each literal is a Formula object.
     *
     * The first object in the returned ArrayList is an ArrayList of
     * clauses.
     *
     * The second object in the returned ArrayList is the original
     * (input) Formula object (this).
     *
     * The third object in the returned ArrayList is a Map that
     * contains a graph of all the variable substitions done during
     * the conversion of this Formula to clausal form.  This Map makes
     * it possible to retrieve the correspondences between the
     * variables in the clausal form and the variables in the original
     * Formula.
     *
     * @see clausify()
     * @see clausifyWithRenameInfo()
     * @see toCanonicalClausalForm();
     *
     * @return A three-element ArrayList, 
     *
     * [ 
     *   // 1. clauses
     *   [ 
     *     // a clause
     *     [ 
     *       // negative literals
     *       [ Formula1, Formula2, ..., FormulaN ],
     *       // positive literals
     *       [ Formula1, Formula2, ..., FormulaN ] 
     *     ],
     *
     *     // another clause
     *     [ 
     *       // negative literals
     *       [ Formula1, Formula2, ..., FormulaN ],
     *       // positive literals
     *       [ Formula1, Formula2, ..., FormulaN ] 
     *     ],
     *
     *     ...,
     *   ],
     *
     *   // 2.
     *   <the-original-Formula>,
     *
     *   // 3.
     *   {a-Map-of-variable-renamings},
     *
     * ]
     *
     */
    public ArrayList toNegAndPosLitsWithRenameInfo() {

        // System.out.println("INFO in Formula.toNegAndPosLitsWithRenameInfo(" + this + ")");

        ArrayList ans = new ArrayList();
        try {
            List clausesWithRenameInfo = this.clausifyWithRenameInfo();
            if (clausesWithRenameInfo.size() == 3) {
                Formula clausalForm = (Formula) clausesWithRenameInfo.get(0);
                ArrayList clauses = clausalForm.operatorsOut();
                if ((clauses != null) && !clauses.isEmpty()) {

                    // System.out.println("\nclauses == " + clauses);
                    ArrayList newClauses = new ArrayList();
                    ArrayList negLits = null;
                    ArrayList posLits = null;
                    ArrayList literals = null;
                    Formula clause = null;
                    for (Iterator it = clauses.iterator(); it.hasNext();) {
                        negLits = new ArrayList();
                        posLits = new ArrayList();
                        literals = new ArrayList();
                        literals.add(negLits);
                        literals.add(posLits);
                        clause = (Formula) it.next();
                        if (clause.listP()) {
                            while (!clause.empty()) {
                                boolean isNegLit = false;
                                String lit = clause.car();
                                Formula litF = new Formula();
                                litF.read(lit);
                                if (litF.listP() && litF.car().equals(NOT)) {
                                    litF.read(litF.cadr());
                                    isNegLit = true;
                                }
                                if (litF.theFormula.equals(LOG_FALSE)) {
                                    isNegLit = true;
                                }
                                if (isNegLit) {
                                    negLits.add(litF);
                                }
                                else {
                                    posLits.add(litF);
                                }
                                // System.out.println("clause 1 == " + clause);
                                clause = clause.cdrAsFormula();
                                // System.out.println("clause 2 == " + clause);
                            }
                        }
                        else if (clause.theFormula.equals(LOG_FALSE)) {
                            negLits.add(clause);
                        }
                        else {
                            posLits.add(clause);
                        }
                        newClauses.add(literals);
                    }
                    // Collections.sort(negLits);
                    // Collections.sort(posLits);
                    ans.add(newClauses);
                }
                if (ans.size() == 1) {
                    int cwriLen = clausesWithRenameInfo.size();
                    for (int j = 1; j < cwriLen; j++) {          
                        ans.add(clausesWithRenameInfo.get(j));
                    }
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return ans;
    }

    /** ***************************************************************
     * This method converts the SUO-KIF Formula to a canonical version
     * of clausal (resolution, conjunctive normal) form with Skolem
     * functions, following the procedure described in Logical
     * Foundations of Artificial Intelligence, by Michael Genesereth
     * and Nils Nilsson, 1987, pp. 63-66.  In canonical form, all
     * variables are renamed from index 0 and all literals are
     * alphabetically sorted.
     *
     * @see clausify()
     * @see clausifyWithRenameInfo()
     * @see toNegAndPosLitsWithRenameInfo()
     *
     * @return The Formula in a canonicalized version of clausal form
     */
    public Formula toCanonicalClausalForm() {
        Formula ans = new Formula();
        try {
            List clauseData = this.toNegAndPosLitsWithRenameInfo();
            if (clauseData.isEmpty()) 
                ans = this;
            else {
                List clauses = (List) clauseData.get(0);
                if (!clauses.isEmpty()) {
                    StringBuilder sb = new StringBuilder();
                    List sortedClauses = new ArrayList();
                    Iterator itc = null;
                    for (itc = clauses.iterator(); itc.hasNext();) {
                        List clause = (List) itc.next();
                        if (!clause.isEmpty() && (clause.size() == 2)) {
                            sb.setLength(0);
                            Iterator itl = null;
                            List neglits = (List) clause.get(0);
                            if (neglits.size() > 1) Collections.sort(neglits);
                            int i = 0;
                            for (itl = neglits.iterator(); itl.hasNext(); i++) {
                                if (i > 0) sb.append(SPACE);
                                sb.append(LP);
                                sb.append(NOT);
                                sb.append(SPACE);
                                sb.append(itl.next().toString());
                                sb.append(RP);
                            }
                            List poslits = (List) clause.get(1);
                            if (!poslits.isEmpty()) {
                                Collections.sort(poslits);
                                for (itl = poslits.iterator(); itl.hasNext(); i++) {
                                    if (i > 0) sb.append(SPACE);
                                    sb.append(itl.next().toString());
                                }
                            }
                            if (i > 1) {
                                sb.insert(0, SPACE);
                                sb.insert(0, OR);
                                sb.insert(0, LP);
                                sb.append(RP);
                            }
                            sortedClauses.add(sb.toString());
                        }
                    }
                    Collections.sort(sortedClauses);
                    sb.setLength(0);
                    int j = 0;
                    for (itc = sortedClauses.iterator(); itc.hasNext(); j++) {
                        if (j > 0) sb.append(SPACE);
                        sb.append(itc.next().toString());
                    }
                    if (j > 1) {
                        sb.insert(0, SPACE);
                        sb.insert(0, AND);
                        sb.insert(0, LP);
                        sb.append(RP);
                    }
                    ans.read(normalizeVariables(sb.toString()));
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return ans;
    }

    /** ***************************************************************
     * <p>This method returns an open Formula that constitutes a KIF
     * query expression, which is generated from the canonicalized
     * negation of the original Formula.  The original Formula is
     * assumed to be a "rule", having both positive and negative
     * literals when converted to canonical clausal form.  This method
     * returns a query expression that can be used to test the
     * validity of the original Formula.  If the query succeeds
     * (evaluates to true, or retrieves variable bindings), then the
     * negation of the original Formula is valid, and the original
     * Formula must be invalid.</p>
     *
     * <p>Note that this method will work reliably only for standard
     * first order KIF expressions, and probably will not produce
     * expected (or sane) results if used with non-standard KIF.</p>
     *
     * @see Formula.toCanonicalClausalForm()
     *
     * @return A String representing an open KIF query expression.
     */
    protected Formula toOpenQueryForNegatedDualForm() {
        Formula result = this;
        try {

            // 1. Make quantifiers explicit.
            Formula qF = new Formula();
            qF.read(this.makeQuantifiersExplicit(false));

            // 2. Negate the formula.
            Formula negQF = new Formula();
            negQF.read("(not " + qF.theFormula + ")");

            // 3. Generate the canonical clausal form of the negation
            // of the formula we want to check.
            Formula canonNegF = new Formula();
            canonNegF.read(negQF.toCanonicalClausalForm().theFormula);

            // 4. Finally, normalize variables, treat Skolem terms as
            // variables.
            Formula queryF = new Formula();
            queryF.read(normalizeVariables(canonNegF.theFormula,
                                           true));

            result = queryF;
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return result;
    }

    /** ***************************************************************
     * This method returns a canonical version of this Formula,
     * assumed to be a KIF "special" form, in which all internal
     * first-order KIF formulae are replaced by their canonical
     * versions, and all variables are renamed, in left to right
     * depth-first order of occurrence, starting from index 1.  This
     * method is intended to be applied to higher-order constructs
     * that expand the syntax of KIF.  To canonicalize ordinary
     * first-order KIF formulae, just use
     * <code>Formula.toCanonicalClausalForm()</code>.
     *
     * @see Formula.toCanonicalClausalForm()
     *
     * @param preserveSharedVariables If true, all variable sharing
     * between embedded expressions will be preserved, even though all
     * variables will still be renamed as part of the canonicalization
     * process
     *
     * @return A String representing the canonicalized version of this
     * special, non-standard KIF expression.
     */
    protected Formula toCanonicalKifSpecialForm(boolean preserveSharedVariables) {
        Formula result = this;
        try {
            // First, obfuscate all variables to prevent them from
            // being renamed during conversion to clausal form.
            String flist = this.theFormula;
            String vpref = V_PREF;
            String rpref = R_PREF;
            String vrepl = "vvvv";
            String rrepl = "rrrr";
            if (preserveSharedVariables) {
                if (flist.contains(vpref)) flist = flist.replace(vpref, vrepl);
                if (flist.contains(rpref)) flist = flist.replace(rpref, rrepl);
            }
            StringBuilder sb = new StringBuilder();
            if (Formula.listP(flist)) {
                if (Formula.empty(flist)) {
                    sb.append(flist);
                }
                else {
                    Formula f = new Formula();
                    f.read(flist);
                    String arg0 = f.car();
                    if (isLogicalOperator(arg0)) {
                        sb.append(f.toCanonicalClausalForm().theFormula);
                    }
                    else {
                        List tuple = f.literalToArrayList();
                        sb.append(LP);
                        int i = 0;
                        for (Iterator it = tuple.iterator(); it.hasNext(); i++) {
                            Formula nextF = new Formula();
                            nextF.read((String) it.next());
                            if (i > 0) sb.append(SPACE);
                            sb.append(nextF.toCanonicalKifSpecialForm(false).theFormula);
                        }
                        sb.append(RP);
                    }
                }
            }
            else {
                sb.append(flist);
            }

            flist = sb.toString();
            if (preserveSharedVariables) {
                flist = flist.replace(vrepl, vpref);
                flist = flist.replace(rrepl, rpref);
            }
            flist = normalizeVariables(flist);
            Formula canonSF = new Formula();
            canonSF.read(flist);
            result = canonSF;
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return result;
    }

    /** ***************************************************************
     * Returns a String in which all variables and row variables have
     * been normalized -- renamed, in depth-first order of occurrence,
     * starting from index 1 -- to support comparison of Formulae for
     * equality.
     *
     * @see normalizeVariables_1();
     *
     * @param input A String representing a SUO-KIF Formula, possibly
     * containing variables to be normalized
     * 
     * @return A String, typically representing a SUO-KIF Formula or
     * part of a Formula, in which the original variables have been
     * replaced by normalized forms
     */
    protected static String normalizeVariables(String input) {
        return normalizeVariables(input, false);
    }

    /** ***************************************************************
     * Returns a String in which all variables and row variables have
     * been normalized -- renamed, in depth-first order of occurrence,
     * starting from index 1 -- to support comparison of Formulae for
     * equality.
     *
     * @see normalizeVariables_1();
     *
     * @param input A String representing a SUO-KIF Formula, possibly
     * containing variables to be normalized
     * 
     * @param replaceSkolemTerms If true, all Skolem terms in input
     * are treated as variables and are replaced with normalized
     * variable terms
     * 
     * @return A String, typically representing a SUO-KIF Formula or
     * part of a Formula, in which the original variables have been
     * replaced by normalized forms
     */
    protected static String normalizeVariables(String input, boolean replaceSkolemTerms) {
        String result = input;
        try {
            int[] idxs = {1, 1};
            Map vmap = new HashMap();
            result = normalizeVariables_1(input, idxs, vmap, replaceSkolemTerms);
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return result;
    }

    /** ***************************************************************
     * An internal helper method for normalizeVariables(String input).
     *
     * @see normalizeVariables();
     *
     * @param input A String possibly containing variables to be
     * normalized
     *
     * @param idxs A two-place int[] in which int[0] is the current
     * variable index, and int[1] is the current row variable index
     *
     * @param vmap A Map in which the keys are old variables and the
     * values are new variables
     * 
     * @return A String, typically a representing a SUO-KIF Formula or
     * part of a Formula.
     */
    protected static String normalizeVariables_1(String input, 
                                                 int[] idxs, 
                                                 Map vmap, 
                                                 boolean replaceSkolemTerms) {
        String result = "";
        try {
            String vbase = VVAR;
            String rvbase = (RVAR + "VAR");
            StringBuilder sb = new StringBuilder();
            String flist = input.trim();
            boolean isSkolem = isSkolemTerm(flist);
            if ((replaceSkolemTerms && isSkolem) || isVariable(flist)) {
                String newvar = (String) vmap.get(flist);
                if (newvar == null) {
                    newvar = ((flist.startsWith(V_PREF) || isSkolem)
                              ? (vbase + idxs[0]++)
                              : (rvbase + idxs[1]++));
                    vmap.put(flist, newvar);
                }
                sb.append(newvar);
            }
            else if (Formula.listP(flist)) {
                if (Formula.empty(flist)) {
                    sb.append(flist);
                }
                else {
                    Formula f = new Formula();
                    f.read(flist);
                    List tuple = f.literalToArrayList();
                    sb.append(LP);
                    int i = 0;
                    for (Iterator it = tuple.iterator(); it.hasNext(); i++) {
                        if (i > 0) sb.append(SPACE);
                        sb.append(normalizeVariables_1((String) it.next(),
                                                       idxs,
                                                       vmap,
                                                       replaceSkolemTerms));
                    }
                    sb.append(RP);
                }
            }
            else {
                sb.append(flist);
            }

            result = sb.toString();
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return result;
    }

    /** ***************************************************************
     * This method converts every occurrence of '<=>' in the Formula
     * to a conjunct with two occurrences of '=>'.
     * 
     * @return A Formula with no occurrences of '<=>'.
     *
     */
    private Formula equivalencesOut() {
        Formula ans = this;
        try {
            String theNewFormula = null;
            if (this.listP() && !(this.empty())) {
                String head = this.car();
                if (isNonEmptyString(head) && listP(head)) {
                    Formula headF = new Formula();
                    headF.read(head);
                    String newHead = headF.equivalencesOut().theFormula;
                    theNewFormula = this.cdrAsFormula().equivalencesOut().cons(newHead).theFormula;
                }
                else if (head.equals(IFF)) {
                    String second = this.cadr();
                    Formula secondF = new Formula();
                    secondF.read(second);
                    String newSecond = secondF.equivalencesOut().theFormula;
                    String third = this.caddr();
                    Formula thirdF = new Formula();
                    thirdF.read(third);
                    String newThird = thirdF.equivalencesOut().theFormula;
           
                    theNewFormula = ("(and (=> "
                                     + newSecond
                                     + " "
                                     + newThird
                                     + ") (=> "
                                     + newThird
                                     + " "
                                     + newSecond
                                     + "))");
                }
                else {
                    theNewFormula = this.cdrAsFormula().equivalencesOut().cons(head).theFormula;
                }
                if (theNewFormula != null) {
                    ans = new Formula();
                    ans.read(theNewFormula);
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return ans;
    }

    /** ***************************************************************
     * This method converts every occurrence of '(=> LHS RHS' in the
     * Formula to a disjunct of the form '(or (not LHS) RHS)'.
     * 
     * @return A Formula with no occurrences of '=>'.
     *
     */
    private Formula implicationsOut() {
        Formula ans = this;
        try {
            String theNewFormula = null;
            if (this.listP() && !this.empty()) {
                String head = this.car();
                if (isNonEmptyString(head) && listP(head)) {
                    Formula headF = new Formula();
                    headF.read(head);
                    String newHead = headF.implicationsOut().theFormula;
                    theNewFormula = this.cdrAsFormula().implicationsOut().cons(newHead).theFormula;
                }
                else if (head.equals(IF)) {
                    String second = this.cadr();
                    Formula secondF = new Formula();
                    secondF.read(second);
                    String newSecond = secondF.implicationsOut().theFormula;
                    String third = this.caddr();
                    Formula thirdF = new Formula();
                    thirdF.read(third);
                    String newThird = thirdF.implicationsOut().theFormula;
                    theNewFormula = ("(or (not " + newSecond + ") " + newThird + ")");
                }
                else {
                    theNewFormula = this.cdrAsFormula().implicationsOut().cons(head).theFormula;
                }
                if (theNewFormula != null) {
                    ans = new Formula();
                    ans.read(theNewFormula);
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return ans;
    }

    /** ***************************************************************
     * This method 'pushes in' all occurrences of 'not', so that each
     * occurrence has the narrowest possible scope, and also removes
     * from the Formula all occurrences of '(not (not ...))'.
     *
     * @see negationsIn_1().
     *
     * @return A Formula with all occurrences of 'not' accorded
     * narrowest scope, and no occurrences of '(not (not ...))'.
     */
    private Formula negationsIn() {
        Formula f = this;
        Formula ans = negationsIn_1();

        // Here we repeatedly apply negationsIn_1() until there are no
        // more changes.
        while (!f.theFormula.equals(ans.theFormula)) {     

            /*
              System.out.println();
              System.out.println("f.theFormula == " + f.theFormula);
              System.out.println("ans.theFormula == " + ans.theFormula);
              System.out.println();
            */

            f = ans;
            ans = f.negationsIn_1();
        }
        return ans;
    }

    /** ***************************************************************
     * This method is used in negationsIn().  It recursively 'pushes
     * in' all occurrences of 'not', so that each occurrence has the
     * narrowest possible scope, and also removes from the Formula all
     * occurrences of '(not (not ...))'.
     *
     * @see negationsIn().
     *
     * @return A Formula with all occurrences of 'not' accorded
     * narrowest scope, and no occurrences of '(not (not ...))'.
     */
    private Formula negationsIn_1() {  
        // System.out.println("INFO in negationsIn_1(" + theFormula + ")");
        try {
            if (this.listP()) {
                if (this.empty()) { return this; }
                String arg0 = this.car();
                String arg1 = this.cadr();
                if (arg0.equals(NOT) && listP(arg1)) {
                    Formula arg1F = new Formula();
                    arg1F.read(arg1);
                    String arg0_of_arg1 = arg1F.car();
                    if (arg0_of_arg1.equals(NOT)) {
                        String arg1_of_arg1 = arg1F.cadr();
                        Formula arg1_of_arg1F = new Formula();
                        arg1_of_arg1F.read(arg1_of_arg1);
                        return arg1_of_arg1F;
                    }
                    if (isCommutative(arg0_of_arg1)) {
                        String newOp = (arg0_of_arg1.equals(AND) ? OR : AND);
                        return arg1F.cdrAsFormula().listAll("(not ", ")").cons(newOp);
                    }
                    if (isQuantifier(arg0_of_arg1)) {
                        String vars = arg1F.cadr();
                        String arg2_of_arg1 = arg1F.caddr();
                        String quant = (arg0_of_arg1.equals(UQUANT) ? EQUANT : UQUANT);
                        arg2_of_arg1 = ("(not " + arg2_of_arg1 + ")");
                        Formula arg2_of_arg1F = new Formula();
                        arg2_of_arg1F.read(arg2_of_arg1);
                        String theNewFormula = ("(" + quant + " " + vars + " " 
                                                + arg2_of_arg1F.negationsIn_1().theFormula + ")");
                        Formula newF = new Formula();
                        newF.read(theNewFormula);
                        return newF;
                    }
                    String theNewFormula = ("(not " + arg1F.negationsIn_1().theFormula + ")");
                    Formula newF = new Formula();
                    newF.read(theNewFormula);
                    return newF;
                }
                if (isQuantifier(arg0)) {
                    String arg2 = this.caddr();
                    Formula arg2F = new Formula();
                    arg2F.read(arg2);
                    String newArg2 = arg2F.negationsIn_1().theFormula;
                    String theNewFormula = ("(" + arg0 + " " + arg1 + " " + newArg2 + ")");
                    Formula newF = new Formula();
                    newF.read(theNewFormula);
                    return newF;
                }
                if (listP(arg0)) {
                    Formula arg0F = new Formula();
                    arg0F.read(arg0);
                    return this.cdrAsFormula().negationsIn_1().cons(arg0F.negationsIn_1().theFormula);
                }
                return this.cdrAsFormula().negationsIn_1().cons(arg0);
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return this;
    }

    /** ***************************************************************
     * This method augments each element of the Formula by
     * concatenating optional Strings before and after the element.
     *
     * Note that in most cases the input Formula will be simply a
     * list, not a well-formed SUO-KIF Formula, and that the output
     * will therefore not necessarily be a well-formed Formula.
     *
     * @param before A String that, if present, is prepended to every
     * element of the Formula.
     *
     * @param after A String that, if present, is postpended to every
     * element of the Formula.
     * 
     * @return A Formula, or, more likely, simply a list, with the
     * String values corresponding to before and after added to each
     * element.
     *
     */
    private Formula listAll(String before, String after) {
        Formula ans = this;
        String theNewFormula = null;
        if (this.listP()) {
            theNewFormula = "";
            Formula f = this;
            while (!(f.empty())) {
                String element = f.car();
                if (isNonEmptyString(before)) {
                    element = (before + element);
                }
                if (isNonEmptyString(after)) {
                    element += after;
                }
                theNewFormula += (SPACE + element);
                f = f.cdrAsFormula();
            }
            theNewFormula = (LP + theNewFormula.trim() + RP);
            if (isNonEmptyString(theNewFormula)) {
                ans = new Formula();
                ans.read(theNewFormula);
            }
        }
        return ans;
    }

    /** ***************************************************************
     * This method increments VAR_INDEX and then returns the new int
     * value.  If VAR_INDEX is already at Integer.MAX_VALUE, then
     * VAR_INDEX is reset to 0.
     * 
     * @return An int value between 0 and Integer.MAX_VALUE inclusive.
     */
    private static int incVarIndex() {
        int oldVal = VAR_INDEX;
        if (oldVal == Integer.MAX_VALUE) {
            VAR_INDEX = 0;
        }
        else {
            ++VAR_INDEX;
        }
        return VAR_INDEX;
    }

    /** ***************************************************************
     * This method increments SKOLEM_INDEX and then returns the new int
     * value.  If SKOLEM_INDEX is already at Integer.MAX_VALUE, then
     * SKOLEM_INDEX is reset to 0.
     * 
     * @return An int value between 0 and Integer.MAX_VALUE inclusive.
     */
    private static int incSkolemIndex() {
        int oldVal = SKOLEM_INDEX;
        if (oldVal == Integer.MAX_VALUE) {
            SKOLEM_INDEX = 0;
        }
        else {
            ++SKOLEM_INDEX;
        }
        return SKOLEM_INDEX;
    }

    /** ***************************************************************
     * This method returns a new SUO-KIF variable String, modifying
     * any digit suffix to ensure that the variable will be unique.
     *
     * @param prefix An optional variable prefix string.
     * 
     * @return A new SUO-KIF variable.
     */
    private static String newVar(String prefix) {
        String base = VX;
        String varIdx = Integer.toString(incVarIndex());
        if (isNonEmptyString(prefix)) {
            List woDigitSuffix = KB.getMatches(prefix, "var_with_digit_suffix");
            if (woDigitSuffix != null) {
                base = (String) woDigitSuffix.get(0);
            }
            else if (prefix.startsWith(RVAR)) {
                base = RVAR;
            }
            else if (prefix.startsWith(VX)) {
                base = VX;
            }
            else {
                base = prefix;
            }
            if (!(base.startsWith(V_PREF) || base.startsWith(R_PREF))) {
                base = (V_PREF + base);
            }
        }
        return (base + varIdx);
    }

    /** ***************************************************************
     * This method returns a new SUO-KIF variable String, adding a
     * digit suffix to ensure that the variable will be unique.
     *
     * @return A new SUO-KIF variable
     */
    private static String newVar() {
        return newVar(null);
    }

    /** ***************************************************************
     * This method returns a new SUO-KIF row variable String,
     * modifying any digit suffix to ensure that the variable will be
     * unique.
     *
     * @return A new SUO-KIF row variable.
     */
    private static String newRowVar() {
        return newVar(RVAR);
    }

    /** ***************************************************************
     * This method returns a new Formula in which all variables have
     * been renamed to ensure uniqueness.
     *
     * @see clausify()
     * @see renameVariables(Map topLevelVars, Map scopedRenames)
     *
     * @return A new SUO-KIF Formula with all variables renamed.
     */
    private Formula renameVariables() {
        HashMap topLevelVars = new HashMap();
        HashMap scopedRenames = new HashMap();
        HashMap allRenames = new HashMap();
        return renameVariables(topLevelVars, scopedRenames, allRenames);
    }

    /** ***************************************************************
     * This method returns a new Formula in which all variables have
     * been renamed to ensure uniqueness.
     *
     * @see renameVariables().
     *
     * @param topLevelVars A Map that is used to track renames of
     * implicitly universally quantified variables.
     *
     * @param scopedRenames A Map that is used to track renames of
     * explicitly quantified variables.
     *
     * @param allRenames A Map from all new vars in the Formula to
     * their old counterparts.
     *
     * @return A new SUO-KIF Formula with all variables renamed.
     */
    private Formula renameVariables(Map topLevelVars, Map scopedRenames, Map allRenames) {

        try {
            if (this.listP()) {
                if (this.empty()) { return this; }
                String arg0 = this.car();
                if (isQuantifier(arg0)) {
           
                    // Copy the scopedRenames map to protect
                    // variable scope as we descend below this
                    // quantifier.
                    Map newScopedRenames = new HashMap(scopedRenames);

                    String oldVars = this.cadr();
                    Formula oldVarsF = new Formula();
                    oldVarsF.read(oldVars);
                    String newVars = "";
                    while (!oldVarsF.empty()) {
                        String oldVar = oldVarsF.car();
                        String newVar = newVar();
                        newScopedRenames.put(oldVar, newVar);
                        allRenames.put(newVar, oldVar);
                        newVars += (SPACE + newVar);
                        oldVarsF = oldVarsF.cdrAsFormula();
                    }
                    newVars = (LP + newVars.trim() + RP);
                    String arg2 = this.caddr();
                    Formula arg2F = new Formula();
                    arg2F.read(arg2);
                    String newArg2 = arg2F.renameVariables(topLevelVars,
                                                           newScopedRenames,
                                                           allRenames).theFormula;
                    String theNewFormula = (LP + arg0 + SPACE + newVars + SPACE + newArg2 + RP);
                    Formula newF = new Formula();
                    newF.read(theNewFormula);
                    return newF;
                }
                Formula arg0F = new Formula();
                arg0F.read(arg0);
                String newArg0 = arg0F.renameVariables(topLevelVars,
                                                       scopedRenames,
                                                       allRenames).theFormula;
                String newRest = this.cdrAsFormula().renameVariables(topLevelVars,
                                                                     scopedRenames,
                                                                     allRenames).theFormula;
                Formula newRestF = new Formula();
                newRestF.read(newRest);
                String theNewFormula = newRestF.cons(newArg0).theFormula;
                Formula newF = new Formula();
                newF.read(theNewFormula);
                return newF;
            }
            if (isVariable(this.theFormula)) {
                String rnv = (String) scopedRenames.get(this.theFormula);
                if (!isNonEmptyString(rnv)) {
                    rnv = (String) topLevelVars.get(this.theFormula);
                    if (!isNonEmptyString(rnv)) {
                        rnv = newVar();
                        topLevelVars.put(this.theFormula, rnv);
                        allRenames.put(rnv, this.theFormula);
                    }
                }
                Formula newF = new Formula();
                newF.read(rnv);
                return newF;
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return this;
    }

    /** ***************************************************************
     * This method returns a new, unique skolem term with each
     * invocation.
     *
     * @param vars A sorted TreeSet of the universally quantified
     * variables that potentially define the skolem term.  The set may
     * be empty.
     *
     * @return A String.  The string will be a skolem functional term
     * (a list) if vars cotains variables.  Otherwise, it will be an
     * atomic constant.
     */
    private String newSkolemTerm(TreeSet vars) {
        String ans = SK_PREF;
        int idx = incSkolemIndex();
        if ((vars != null) && !vars.isEmpty()) {
            ans += (FN_SUFF + SPACE + idx);
            for (Iterator it = vars.iterator(); it.hasNext();) {
                String var = (String) it.next();
                ans += (SPACE + var);
            }
            ans = (LP + ans + RP);
        }
        else {
            ans += idx;
        }
        return ans;
    }      

    /** ***************************************************************
     * This method returns a new Formula in which all existentially
     * quantified variables have been replaced by Skolem terms.
     *
     * @see existentialsOut(Map evSubs, TreeSet iUQVs, TreeSet scopedUQVs)
     * @see collectIUQVars(TreeSet iuqvs, TreeSet scopedVars)
     *
     * @return A new SUO-KIF Formula without existentially quantified
     * variables.
     */
    private Formula existentialsOut() {

        // Existentially quantified variable substitution pairs:
        // var -> skolem term.
        Map evSubs = new HashMap();

        // Implicitly universally quantified variables.
        TreeSet iUQVs = new TreeSet();

        // Explicitly quantified variables.
        TreeSet scopedVars = new TreeSet();

        // Explicitly universally quantified variables.
        TreeSet scopedUQVs = new TreeSet();

        // Collect the implicitly universally qualified variables from
        // the Formula.
        collectIUQVars(iUQVs, scopedVars);

        // Do the recursive term replacement, and return the results.
        return existentialsOut(evSubs, iUQVs, scopedUQVs);
    }

    /** ***************************************************************
     * This method returns a new Formula in which all existentially
     * quantified variables have been replaced by Skolem terms.
     *
     * @see existentialsOut()
     *
     * @param evSubs A Map of variable - skolem term substitution
     * pairs.
     *
     * @param iUQVs A TreeSet of implicitly universally quantified
     * variables.
     *
     * @param scopedUQVs A TreeSet of explicitly universally
     * quantified variables.
     *
     * @return A new SUO-KIF Formula without existentially quantified
     * variables.
     */
    private Formula existentialsOut(Map evSubs, TreeSet iUQVs, TreeSet scopedUQVs) {
        /*
          System.out.println("ENTER Formula.existentialsOut(" 
          + this.theFormula + ", " 
          + evSubs + ", " 
          + iUQVs + ", " 
          + scopedUQVs + ")");
        */
        try {
            if (this.listP()) {
                if (this.empty()) { return this; }
                String arg0 = this.car();
                if (arg0.equals(UQUANT)) {
           
                    // Copy the scoped variables set to protect
                    // variable scope as we descend below this
                    // quantifier.
                    TreeSet newScopedUQVs = new TreeSet(scopedUQVs);

                    String varList = this.cadr();           
                    Formula varListF = new Formula();
                    varListF.read(varList);
                    while (!(varListF.empty())) {
                        String var = varListF.car();
                        newScopedUQVs.add(var);
                        varListF.read(varListF.cdr());
                    }
                    String arg2 = this.caddr();
                    Formula arg2F = new Formula();
                    arg2F.read(arg2);
                    String theNewFormula = ("(forall " 
                                            + varList 
                                            + " " 
                                            + arg2F.existentialsOut(evSubs, 
                                                                    iUQVs, 
                                                                    newScopedUQVs).theFormula 
                                            + ")");
                    this.read(theNewFormula);
                    return this;
                }
                if (arg0.equals(EQUANT)) {

                    // Collect the relevant universally quantified
                    // variables.
                    TreeSet uQVs = new TreeSet(iUQVs);
                    uQVs.addAll(scopedUQVs);

                    // Collect the existentially quantified
                    // variables.
                    ArrayList eQVs = new ArrayList();
                    String varList = this.cadr();           
                    Formula varListF = new Formula();
                    varListF.read(varList);
                    while (!(varListF.empty())) {
                        String var = varListF.car();
                        eQVs.add(var);
                        varListF.read(varListF.cdr());
                    }
           
                    // For each existentially quantified variable,
                    // create a corresponding skolem term, and
                    // store the pair in the evSubs map.
                    for (int i = 0 ; i < eQVs.size() ; i++) {
                        String var = (String) eQVs.get(i);
                        String skTerm = newSkolemTerm(uQVs);
                        evSubs.put(var, skTerm);
                    }
                    String arg2 = this.caddr();
                    Formula arg2F = new Formula();
                    arg2F.read(arg2);
                    return arg2F.existentialsOut(evSubs, iUQVs, scopedUQVs);
                }
                Formula arg0F = new Formula();
                arg0F.read(arg0);
                String newArg0 = arg0F.existentialsOut(evSubs, 
                                                       iUQVs, 
                                                       scopedUQVs).theFormula;
                return this.cdrAsFormula().existentialsOut(evSubs, 
                                                           iUQVs, 
                                                           scopedUQVs).cons(newArg0);
            }
            if (isVariable(this.theFormula)) {
                String newTerm = (String) evSubs.get(this.theFormula);
                if (isNonEmptyString(newTerm)) {
                    this.read(newTerm);
                }
                return this;
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return this;
    }

    /** ***************************************************************
     * This method collects all variables in Formula that appear to be
     * only implicitly universally quantified and adds them to the
     * TreeSet iuqvs.  Note the iuqvs must be passed in.
     *
     * @param iuqvs A TreeSet for accumulating variables that appear
     * to be implicitly universally quantified.
     *
     * @param scopedVars A TreeSet containing explicitly quantified
     * variables.
     *
     * @return void
     */
    private void collectIUQVars(TreeSet iuqvs, TreeSet scopedVars) {

        // System.out.println("INFO in collectIUQVars(" + this.theFormula + ", " + iuqvs + ", " + scopedVars + ")");
        try {
            if (this.listP() && !this.empty()) {
                String arg0 = this.car();
                if (isQuantifier(arg0)) {
           
                    // Copy the scopedVars set to protect variable
                    // scope as we descend below this quantifier.
                    TreeSet newScopedVars = new TreeSet(scopedVars);

                    String varList = this.cadr();
                    Formula varListF = new Formula();
                    varListF.read(varList);
                    while (!(varListF.empty())) {
                        String var = varListF.car();
                        newScopedVars.add(var);
                        varListF = varListF.cdrAsFormula();
                    }
                    String arg2 = this.caddr();
                    Formula arg2F = new Formula();
                    arg2F.read(arg2);
                    arg2F.collectIUQVars(iuqvs, newScopedVars);
                }
                else {
                    Formula arg0F = new Formula();
                    arg0F.read(arg0);
                    arg0F.collectIUQVars(iuqvs, scopedVars);
                    this.cdrAsFormula().collectIUQVars(iuqvs, scopedVars);
                }
            }
            else if (isVariable(this.theFormula) 
                     && !(scopedVars.contains(this.theFormula))) {
                iuqvs.add(this.theFormula);
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return;
    }

    /** ***************************************************************
     * This method returns a new Formula in which explicit univeral
     * quantifiers have been removed.
     *
     * @see clausify()
     *
     * @return A new SUO-KIF Formula without explicit universal
     * quantifiers.
     */
    private Formula universalsOut() {
        // System.out.println("INFO in universalsOut(" + this.theFormula + ")");
        try {
            if (this.listP()) {
                if (this.empty()) { return this; }
                String arg0 = this.car();
                if (arg0.equals(UQUANT)) {
                    String arg2 = this.caddr();
                    this.read(arg2);
                    return this.universalsOut();
                }
                Formula arg0F = new Formula();
                arg0F.read(arg0);
                String newArg0 = arg0F.universalsOut().theFormula;
                return this.cdrAsFormula().universalsOut().cons(newArg0);
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return this;
    }

    /** ***************************************************************
     * This method returns a new Formula in which nested 'and', 'or',
     * and 'not' operators have been unnested:
     *
     * (not (not <literal> ...)) -> <literal>
     *
     * (and (and <literal-sequence> ...)) -> (and <literal-sequence> ...)
     *
     * (or (or <literal-sequence> ...)) -> (or <literal-sequence> ...)
     *
     * @see clausify()
     * @see nestedOperatorsOut_1()
     *
     * @return A new SUO-KIF Formula in which nested commutative
     * operators and 'not' have been unnested.
     */
    private Formula nestedOperatorsOut() {
        Formula f = this;
        Formula ans = nestedOperatorsOut_1();

        // Here we repeatedly apply nestedOperatorsOut_1() until there are no
        // more changes.
        while (!f.theFormula.equals(ans.theFormula)) {     

            /*
              System.out.println();
              System.out.println("f.theFormula == " + f.theFormula);
              System.out.println("ans.theFormula == " + ans.theFormula);
              System.out.println();
            */

            f = ans;
            ans = f.nestedOperatorsOut_1();
        }
        return ans;
    }

    /** ***************************************************************
     *
     * @see clausify()
     * @see nestedOperatorsOut_1()
     *
     * @return A new SUO-KIF Formula in which nested commutative
     * operators and 'not' have been unnested.
     */
    private Formula nestedOperatorsOut_1() {

        // System.out.println("INFO in nestedOperatorsOut_1(" + this.theFormula + ")");
        try {
            if (this.listP()) {
                if (this.empty()) { return this; }
                String arg0 = this.car();
                if (isCommutative(arg0) || arg0.equals(NOT)) {
                    ArrayList literals = new ArrayList();
                    Formula restF = this.cdrAsFormula();
                    while (!(restF.empty())) {
                        String lit = restF.car();
                        Formula litF = new Formula();
                        litF.read (lit);
                        if (litF.listP()) {
                            String litFarg0 = litF.car();
                            if (litFarg0.equals(arg0)) {
                                if (arg0.equals(NOT)) {
                                    String theNewFormula = litF.cadr();
                                    Formula newF = new Formula();
                                    newF.read(theNewFormula);
                                    return newF.nestedOperatorsOut_1();
                                }
                                Formula rest2F = litF.cdrAsFormula();
                                while (!(rest2F.empty())) {
                                    String rest2arg0 = rest2F.car();
                                    Formula rest2arg0F = new Formula();
                                    rest2arg0F.read(rest2arg0);
                                    literals.add(rest2arg0F.nestedOperatorsOut_1().theFormula);
                                    rest2F = rest2F.cdrAsFormula();
                                }
                            }
                            else {
                                literals.add(litF.nestedOperatorsOut_1().theFormula);
                            }
                        }
                        else {
                            literals.add(lit);
                        }
                        restF = restF.cdrAsFormula();
                    }
                    String theNewFormula = (LP + arg0);
                    for (int i = 0 ; i < literals.size() ; i++) {
                        theNewFormula += (SPACE + (String)literals.get(i));
                    }
                    theNewFormula += RP;
                    Formula newF = new Formula();
                    newF.read(theNewFormula);
                    return newF;
                }
                Formula arg0F = new Formula();
                arg0F.read(arg0);
                String newArg0 = arg0F.nestedOperatorsOut_1().theFormula;
                return this.cdrAsFormula().nestedOperatorsOut_1().cons(newArg0);
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return this;
    }

    /** ***************************************************************
     * This method returns a new Formula in which all occurrences of
     * 'or' have been accorded the least possible scope.
     *
     * (or P (and Q R)) -> (and (or P Q) (or P R))
     *
     * @see clausify()
     * @see disjunctionsIn_1()
     *
     * @return A new SUO-KIF Formula in which occurrences of 'or' have
     * been 'moved in' as far as possible.
     */
    private Formula disjunctionsIn() {
        Formula f = this;
        Formula ans = nestedOperatorsOut().disjunctionsIn_1();

        // Here we repeatedly apply disjunctionIn_1() until there are no
        // more changes.
        while (! f.theFormula.equals(ans.theFormula)) {     

            /* 
               System.out.println();
               System.out.println("f.theFormula == " + f.theFormula);
               System.out.println("ans.theFormula == " + ans.theFormula);
               System.out.println();
            */

            f = ans;
            ans = f.nestedOperatorsOut().disjunctionsIn_1();
        }
        return ans;
    }
 
    /** ***************************************************************
     *
     * @see clausify()
     * @see disjunctionsIn()
     *
     * @return A new SUO-KIF Formula in which occurrences of 'or' have
     * been 'moved in' as far as possible.
     */
    private Formula disjunctionsIn_1() {

        // System.out.println("INFO in disjunctionsIn_1(" + this.theFormula + ")");
        try {
            if (this.listP()) {
                if (this.empty()) { return this; }
                String arg0 = this.car();
                if (arg0.equals(OR)) {
                    List disjuncts = new ArrayList();
                    List conjuncts = new ArrayList();
                    Formula restF = this.cdrAsFormula();
                    while (!(restF.empty())) {
                        String disjunct = restF.car();
                        Formula disjunctF = new Formula();
                        disjunctF.read(disjunct);
                        if (disjunctF.listP() 
                            && disjunctF.car().equals(AND) 
                            && conjuncts.isEmpty()) {
                            Formula rest2F = disjunctF.cdrAsFormula().disjunctionsIn_1();
                            while (!(rest2F.empty())) {
                                conjuncts.add(rest2F.car());
                                rest2F = rest2F.cdrAsFormula();
                            }
                        }
                        else {
                            disjuncts.add(disjunct);
                        }
                        restF = restF.cdrAsFormula();
                    }

                    if (conjuncts.isEmpty()) { return this; }

                    Formula resultF = new Formula();
                    resultF.read("()");
                    String disjunctsString = "";
                    for (int i = 0 ; i < disjuncts.size() ; i++) {
                        disjunctsString += (SPACE + (String)disjuncts.get(i));
                    }
                    disjunctsString = (LP + disjunctsString.trim() + RP);
                    Formula disjunctsF = new Formula();
                    disjunctsF.read(disjunctsString);
                    for (int ci = 0 ; ci < conjuncts.size() ; ci++) {
                        String newDisjuncts = 
                            disjunctsF.cons((String)conjuncts.get(ci)).cons(OR).disjunctionsIn_1().theFormula;
                        resultF = resultF.cons(newDisjuncts);
                    }
                    resultF = resultF.cons(AND);
                    return resultF;
                }
                Formula arg0F = new Formula();
                arg0F.read(arg0);
                String newArg0 = arg0F.disjunctionsIn_1().theFormula;
                return this.cdrAsFormula().disjunctionsIn_1().cons(newArg0);
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return this;
    }

    /** ***************************************************************
     * This method returns an ArrayList of clauses.  Each clause is a
     * LISP list (really, a Formula) containing one or more Formulas.
     * The LISP list is assumed to be a disjunction, but there is no
     * 'or' at the head.
     *
     * @see clausify()
     *
     * @return An ArrayList of LISP lists, each of which contains one
     * or more Formulas.
     */
    private ArrayList operatorsOut() {
        // System.out.println("INFO in operatorsOut(" + this.theFormula + ")");
        ArrayList result = new ArrayList();
        try {
            ArrayList clauses = new ArrayList();
            if (isNonEmptyString(this.theFormula)) {
                if (this.listP()) {
                    String arg0 = this.car();
                    if (arg0.equals(AND)) {
                        Formula restF = this.cdrAsFormula();
                        while (!(restF.empty())) {
                            String fStr = restF.car();
                            Formula newF = new Formula();
                            newF.read(fStr);
                            clauses.add(newF);
                            restF = restF.cdrAsFormula();
                        }
                    }
                }
                if (clauses.isEmpty()) {
                    clauses.add(this);
                }
                for (int i = 0 ; i < clauses.size() ; i++) {
                    Formula clauseF = new Formula();
                    clauseF.read("()");
                    Formula f = (Formula) clauses.get(i);
                    if (f.listP()) {
                        if (f.car().equals(OR)) {
                            f = f.cdrAsFormula();
                            while (!(f.empty())) {
                                String lit = f.car();
                                clauseF = clauseF.cons(lit);
                                f = f.cdrAsFormula();
                            }
                        }
                    }
                    if (clauseF.empty()) {
                        clauseF = clauseF.cons(f.theFormula);
                    }
                    result.add(clauseF);
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return result;
    }

    /** ***************************************************************
     * This method returns a Formula in which variables for separate
     * clauses have been 'standardized apart'.
     *
     * @see clausify()
     * @see standardizeApart(Map renameMap)
     * @see standardizeApart_1(Map renames, Map reverseRenames)
     *
     * @return A Formula.
     */
    private Formula standardizeApart() {
        HashMap reverseRenames = new HashMap();
        return this.standardizeApart(reverseRenames);
    }

    /** ***************************************************************
     * This method returns a Formula in which variables for separate
     * clauses have been 'standardized apart'.
     *
     * @see clausify()
     * @see standardizeApart()
     * @see standardizeApart_1(Map renames, Map reverseRenames)
     *
     * @param renameMap A Map for capturing one-to-one variable rename
     * correspondences.  Keys are new variables.  Values are old
     * variables.
     *
     * @return A Formula.
     */
    private Formula standardizeApart(Map renameMap) {
       
        Formula result = this;
        try {
            Map reverseRenames = null;
            if (renameMap instanceof Map) {
                reverseRenames = renameMap;
            }
            else {
                reverseRenames = new HashMap();
            }

            // First, break the Formula into separate clauses, if
            // necessary.
            ArrayList clauses = new ArrayList();
            if (isNonEmptyString(this.theFormula)) {
                if (this.listP()) {
                    String arg0 = this.car();
                    if (arg0.equals(AND)) {
                        Formula restF = this.cdrAsFormula();
                        while (!(restF.empty())) {
                            String fStr = restF.car();
                            Formula newF = new Formula();
                            newF.read(fStr);
                            clauses.add(newF);
                            restF = restF.cdrAsFormula();
                        }
                    }
                }
                if (clauses.isEmpty()) {
                    clauses.add(this);
                }

                // 'Standardize apart' by renaming the variables in
                // each clause.
                int n = clauses.size();
                for (int i = 0 ; i < n ; i++) {
                    HashMap renames = new HashMap();
                    Formula oldClause = (Formula) clauses.remove(0);
                    clauses.add(oldClause.standardizeApart_1(renames,reverseRenames));
                }

                // Construct the new Formula to return.
                if (n > 1) {
                    String theNewFormula = "(and";
                    for (int i = 0 ; i < n ; i++) {
                        Formula f = (Formula) clauses.get(i);
                        theNewFormula += (SPACE + f.theFormula);
                    }
                    theNewFormula += RP;
                    Formula newF = new Formula();
                    newF.read(theNewFormula);
                    result = newF;
                }
                else {
                    result = (Formula) clauses.get(0);
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return result;
    }

    /** ***************************************************************
     * This is a helper method for standardizeApart(renameMap).  It
     * assumes that the Formula will be a single clause.
     *
     * @see clausify()
     * @see standardizeApart()
     * @see standardizeApart(Map renameMap)
     *
     * @param renames A Map of correspondences between old variables
     * and new variables.
     *
     * @param reverseRenames A Map of correspondences between new
     * variables and old variables.
     *
     * @return A Formula
     */
    private Formula standardizeApart_1(Map renames, Map reverseRenames) {
        Formula ans = this;
        try {
            if (this.listP() && !(this.empty())) {
                String arg0 = this.car();
                Formula arg0F = new Formula();
                arg0F.read(arg0);
                arg0F = arg0F.standardizeApart_1(renames, reverseRenames);
                ans = cdrAsFormula().standardizeApart_1(renames,
                                                        reverseRenames).cons(arg0F.theFormula);
            }
            else if (isVariable(this.theFormula)) {
                String rnv = (String) renames.get(this.theFormula);
                if (!isNonEmptyString(rnv)) {
                    rnv = newVar();
                    renames.put(this.theFormula, rnv);
                    reverseRenames.put(rnv, this.theFormula);
                }
                Formula rnvF = new Formula();
                rnvF.read(rnv);
                ans = rnvF;
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        return ans;
    }

    /** ***************************************************************
     * This method finds the original variable that corresponds to a new
     * variable.  Note that the clausification algorithm has two variable
     * renaming steps, and that after variables are standardized apart an
     * original variable might correspond to multiple clause variables.
     *
     * @param var A SUO-KIF variable (String)
     *
     * @param varMap A Map (graph) of successive new to old variable
     * correspondences.
     * 
     * @return The original SUO-KIF variable corresponding to the input.
     *
     **/
    private static String getOriginalVar(String var, Map varMap) {

        // System.out.println("INFO in getOriginalVar(" + var + ", " + varMap + ")");

        String ans = null;
        try {
            String next = null;
            if (isNonEmptyString(var) && (varMap instanceof Map)) {
                ans = var;
                next = (String) varMap.get(ans);
                while (! ((next == null) || next.equals(ans))) {
                    ans = next;
                    next = (String) varMap.get(ans);        
                }
                if (ans == null) {
                    ans = var;
                }
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        // System.out.println("  -> " + ans);
        return ans;
    }

    ///////////////////////////////////////////////////////
    /*
      END of clausify() implementation.
    */
    ///////////////////////////////////////////////////////

    /** ***************************************************************
     * A test method.
     */
    public static void main(String[] args) {

        BufferedWriter bw = null;
        try {
            long t1 = System.currentTimeMillis();
            int count = 0;
            String inpath = args[0];
            String outpath = args[1];
            if (isNonEmptyString(inpath) && isNonEmptyString(outpath)) {
                File infile = new File(inpath);
                if (infile.exists()) {
                    KIF kif = new KIF();
                    kif.setParseMode(KIF.RELAXED_PARSE_MODE);
                    kif.readFile(infile.getCanonicalPath());
                    if (! kif.formulas.isEmpty()) {
                        File outfile = new File(outpath);
                        if (outfile.exists()) { outfile.delete(); }
                        bw = new BufferedWriter(new FileWriter(outfile, true));
                        Iterator it = kif.formulas.values().iterator();
                        Iterator it2 = null;
                        Formula f = null;
                        Formula clausalForm = null;
                        while (it.hasNext()) {
                            it2 = ((List) it.next()).iterator();
                            while (it2.hasNext()) {
                                f = (Formula) it2.next();
                                clausalForm = f.clausify();
                                if (clausalForm != null) {
                                    bw.write(clausalForm.theFormula);
                                    bw.newLine();
                                    count++;
                                }
                            }
                        }
                        try {
                            bw.flush();
                            bw.close();
                            bw = null;
                        }
                        catch (Exception bwe) {
                            bwe.printStackTrace();
                        }
                    }
                }
            }
            long dur = (System.currentTimeMillis() - t1);
            System.out.println(count 
                               + " clausal forms written in "
                               + (dur / 1000.0)
                               + " seconds");
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
        finally {
            if (bw != null) {
                try {
                    bw.close();
                }
                catch (Exception e2) {
                    e2.printStackTrace();
                }
            }
        }
        return;
    }

}  // Formula.java
