/* this is the scanner example from the JLex website
   (with small modifications to make it more readable) */
import java.util.ArrayList;

%%

%class Lexer
%unicode
%standalone

%{
    public static ArrayList<String> clone(ArrayList<String> list) {
        ArrayList<String> copy = new ArrayList<String>();
        for (int i = 0; i < list.size(); i+=1) {
            copy.add(new String(list.get(i)));
        }
        return copy;
    }
    ArrayList<String> results = new ArrayList<String>();
    ArrayList<ArrayList<String>> lines_res = new ArrayList<ArrayList<String>>();
    ArrayList<Integer> answers = new ArrayList<Integer>();
%}
    DIGIT=[0-9]
    SPACE=[ \t] //check it
    TYPE_NAME = (int)|(long)|(short)

%state YYINITIAL
%state TYPE
%state VARNAME
%state LITERAL
%state TALE
%%
    <YYINITIAL> {
        {DIGIT}+/{SPACE}+{TYPE_NAME} {results.clear(); results.add(yytext()); yybegin(TYPE);}
        {DIGIT}+/{SPACE}+([a-zA-Z][a-zA-Z0-9]{0,15}) {results.clear(); results.add(yytext()); results.add("int"); yybegin(VARNAME);}
        .|(\r) {answers.add(0); yybegin(TALE);}
        <<EOF>> {return 0;}

    }
    <TYPE> {
        {SPACE}+ {}
        {TYPE_NAME}/{SPACE}+([a-zA-Z][a-zA-Z0-9]{0,15}) {results.add(yytext()); yybegin(VARNAME);}
        .|(\r) {answers.add(0); yybegin(TALE);}
        <<EOF>> { return 0;}
    }
    <VARNAME> {
        {SPACE}+ {}
        [a-zA-Z][a-zA-Z0-9]{0,15}\r\n {String variable = yytext(); results.add(variable.substring(0,variable.length()-2)); yybegin(YYINITIAL); answers.add(1); lines_res.add(results.clone());}
        ([a-zA-Z][a-zA-Z0-9]{0,15})/({SPACE}*={SPACE}*) {results.add(yytext()); yybegin(LITERAL);}
        .|(\r) {answers.add(0); yybegin(TALE);}
        <<EOF>> {return 0;}
    }
    <LITERAL> {
        {SPACE}*={SPACE}* {}
        {DIGIT}+\r\n {yybegin(YYINITIAL); answers.add(1); lines_res.add(results.clone());}
        [a-zA-Z][a-zA-Z0-9]{0,15}\r\n\ {yybegin(YYINITIAL); answers.add(1); lines_res.add(results.clone());}
        .|(\r) {answers.add(0); yybegin(TALE);}
        <<EOF>> {return 0;}
    }
    <TALE> {
        .|(\r) {}
        \n {yybegin(YYINITIAL);}
    }
    [^]                              { throw new Error("Illegal character <"+
                                                            yytext()+">"); }