/****************************************************/
/* File: tiny.l                                     */
/* Lex specification for TINY                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
notletter   [^a-z^A-Z]
notiden     {notletter}+
identifier  {letter}+
newline     \n
whitespace  [ \t]+
numid       {number}+{identifier}+
idnum       {identifier}+{number}+
numidnum      {numid}{number}
idnumid      {idnum}{identifier}

lexerr      ({numid}|{idnum}|{numidnum}|{idnumid})+

%%

"if"            {return IF;}
"then"          {return THEN;}
"else"          {return ELSE;}
"end"           {return END;}
"repeat"        {return REPEAT;}
"until"         {return UNTIL;}
"read"          {return READ;}
"write"         {return WRITE;}
"void"          {return VOID;}
"while"         {return WHILE;}
"int"           {return INT;}
"return"        {return RETURN;}
"=="            {return ASSIGN;}
"="             {return EQ;}
"!="            {return NEQ;}
"<"             {return LT;}
">"             {return RT;}
"<="            {return LEQ;}
">="            {return REQ;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return OVER;}
"("             {return LPAREN;}
")"             {return RPAREN;}
";"             {return SEMI;}
"["             {return LSQBRAC;}
"]"             {return RSQBRAC;}
"{"             {return LBRAC;}
"}"             {return RBRAC;}
","             {return COMMA;}
{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lineno++;
                  return NLSP;}
{whitespace}    {/* skip whitespace */ 
                  return NLSP;}
"/*"             { char c;
                  do
                  { c = input();
                  if(c == '*'){
                    while((c = input())=='*') {}
                    if(c =='/') break;
                  }
                    if (c == EOF) return CMTERR;
                    if (c == '\n') lineno++;
                  } while (c);
                }
{lexerr}        {return LEXERR;}
.               {return ERROR;}

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan && currentToken != NLSP) {
    fprintf(listing,"    %d\t ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}

