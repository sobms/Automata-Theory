%{
    #include "parser.hh"
    #include <stdlib.h>
    #include <string> 
    void yyerror(const char *); 
%}

%option yylineno
%% 

[1-9][0-9]*     {               
                    yylval.intval = atoi(yytext);
                    yylloc.first_line = yylloc.last_line = yylineno;
                    return INT;
                } 

0               {
                    yylval.intval = atoi(yytext);
                    yylloc.first_line = yylloc.last_line = yylineno;
                    return INT;
                }

(true)|(false)  {
                    if (strcmp(yytext,"true") == 0) {
                        yylval.boolval = true;
                    } else {
                        yylval.boolval = false;
                    }
                    yylloc.first_line = yylloc.last_line = yylineno;
                    return BOOL;     
                }

(EMPTY)|(WALL)|(EXIT)|(UNDEF)   {
                                    std::string str = std::string(yytext);
                                    if (str == "EMPTY") {
                                        yylval.intval = int(cellval::EMPTY);
                                    }
                                    else if (str == "WALL") {
                                        yylval.intval = int(cellval::WALL);
                                    }
                                    else if (str == "EXIT") {
                                        yylval.intval = int(cellval::EXIT);
                                    }
                                    else if (str == "UNDEF") {
                                        yylval.intval = int(cellval::UNDEF);
                                    }
                                    yylloc.first_line = yylloc.last_line = yylineno;
                                    return CELL;
                                }

"VARDECLARATION"    { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return VARDECLARATION; 
                    }
"VAR"               { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return VAR; 
                    }
"CONST"             {
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return CONST; 
                    }
"TYPE"              { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return TYPE;
                    }
"DIMENSION"         { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return DIMENSION; 
                    }
"count"             { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return COUNT; 
                    }
"DIMENSIONS"        { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return DIMENSIONS; 
                    }
"VALUE"             { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return VALUE; 
                    }
"VALUES"            { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return VALUES; 
                    }
"DIM"               { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return DIM; 
                    }
"INDEX"             { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return INDEX; 
                    }
"ASSIGN"            { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return ASSIGN; 
                    }
"TO"                { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return TO; 
                    }
"ADD"               { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return ADD; 
                    }
"MUL"               { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return MUL; 
                    }
"SUB"               { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return SUB; 
                    }
"DIV"               { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return DIV; 
                    }
"NOT"               { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return NOT; 
                    }
"OR"                { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return OR; 
                    }
"AND"               { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return AND; 
                    }
"MAX"               { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return MAX; 
                    }
"MIN"               { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return MIN; 
                    }
"EQ"                { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return EQ; 
                    }
"WHILE"             { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return WHILE; 
                    }
"CHECK"             { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return CHECK; 
                    }
"DO"                { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return DO; 
                    }
"SWITCH"            { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return SWITCH; 
                    }
"CONDITION"         { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return CONDITION; 
                    }
"LEFT"              { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return LEFT; 
                    }
"RIGHT"             { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return RIGHT; 
                    }
"UP"                { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return UP; 
                    }
"DOWN"              { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return DOWN; 
                    }
"SENDDRONS"         { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return SENDDRONS; 
                    }
"GETDRONSCOUNT"     { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return GETDRONSCOUNT; 
                    }
"FUNC"              { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return FUNC; 
                    }
"name"              { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return NAME; 
                    }
"CALL"              { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return CALL; 
                    }
"PROGRAM"           { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return PROGRAM; 
                    }
"LOOK"              {
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return LOOK; 
                    }
"SETUSED"           {   
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return SETUSED; 
                    }
"SETSTART"          {
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return SETSTART;
                    }

[-<>\/=\[\]]        { 
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return *yytext;
                    }

(INT)|(BOOL)|(CELL) {
                        std::string str = std::string(yytext);
                        if (str == "INT") {
                            yylval.intval = int(type_t::INTVAL);
                        }
                        else if (str == "BOOL") {
                            yylval.intval = int(type_t::BOOLVAL);
                        }
                        else if (str == "CELL") {
                            yylval.intval = int(type_t::CELLVAL);
                        }
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return TYPE_VAL;
                    }

[ \t]+              {
                        yylloc.first_line = yylloc.last_line = yylineno;
                        return SPACE;
                    }

[a-zA-Z][a-zA-Z0-9]{0,15}   {
                                yylval.strval = new std::string(yytext);
                                yylloc.first_line = yylloc.last_line = yylineno;
                                return VARIABLE_NAME;
                            }


\n              {
                    yylloc.first_line = yylloc.last_line = yylineno;
                } 

.               { 
                    yylloc.first_line = yylloc.last_line = yylineno;
                    yyerror("Unknown character");
                }

%% 

int yywrap(void) 
{
    return 1; 
}