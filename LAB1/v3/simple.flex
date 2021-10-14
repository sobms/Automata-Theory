/* this is the scanner example from the JLex website
   (with small modifications to make it more readable) */
import java.util.ArrayList;

%%

%class Lexer
%unicode
%standalone

%{
    ArrayList<String> results = new ArrayList<String>();
%}
    DIGIT=[0-9]
    SPACE=[ \t] //check it
    TYPE_NAME = (int)|(long)|(short)

%state YYINITIAL
%state TYPE
%state VARNAME
%state LITERAL
%%
    <YYINITIAL> {
        {DIGIT}+/{SPACE}+{TYPE_NAME} {results.clear(); results.add(yytext()); yybegin(TYPE);}
        {DIGIT}+/{SPACE}+([a-zA-Z][a-zA-Z0-9]{0,15}) {results.clear(); results.add(yytext()); results.add("int"); yybegin(VARNAME);}
        . {return 0;}
        \n {return 0;}
    }
    <TYPE> {
        {SPACE}+ {}
        {TYPE_NAME}/{SPACE}+([a-zA-Z][a-zA-Z0-9]{0,15}) {results.add(yytext()); yybegin(VARNAME);}
        . {yybegin(YYINITIAL); return 0;}
        \n {yybegin(YYINITIAL); return 0;}
    }
    <VARNAME> {
        {SPACE}+ {}
        [a-zA-Z][a-zA-Z0-9]{0,15}/[\r\n] {results.add(yytext()); yybegin(YYINITIAL); return 1;}
        ([a-zA-Z][a-zA-Z0-9]{0,15})/({SPACE}*={SPACE}*) {results.add(yytext()); yybegin(LITERAL);}
        . {yybegin(YYINITIAL); return 0;}
        \n {yybegin(YYINITIAL); return 0;}
    }
    <LITERAL> {
        {SPACE}*={SPACE}* {}
        {DIGIT}+/[\r\n] {yybegin(YYINITIAL); return 1;} //со знаком \n}
        [a-zA-Z][a-zA-Z0-9]{0,15}/[\r\n] {yybegin(YYINITIAL); return 1;} //со знаком \n
        . {yybegin(YYINITIAL); return 0;}
        \n {yybegin(YYINITIAL); return 0;}
    }
    [^]                              { throw new Error("Illegal character <"+
                                                            yytext()+">"); }