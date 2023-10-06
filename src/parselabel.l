/* File: parselabel.l
 * 
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
 * simulation.
 * Copyright (C) 1998-2023 Stefan Frederik Schippers
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */


%option never-interactive
%option noyywrap
%option noinput
%option nounput
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAD_SPICE_NETLIST 1


static int parselabel_debug = 0;
#include "xschem.h"

/* 
extern int xctx;
extern int has_x;
extern FILE *errfp;
extern size_t my_strdup(int id, char **dest, const char *src);
extern void my_free(int id, void *ptr);
extern char bus_char[];
extern const char *tcleval(const char str[]);
extern void *my_malloc(int id, size_t size);
extern int  my_snprintf(char *str, int size, const char *fmt, ...);
*/
#ifndef STRINGPTR
#define STRINGPTR
typedef struct          /* used in expandlabel.y */
{
 char *str;             /* label name */
 int m;                 /* label multiplicity, number of wires */
} Stringptr;
#endif
/* commented 20170412, fixes problems with older bison/flex versions */
/* #define YYPARSE_PARAM */
#include "expandlabel.h" /* Bison header file */

Stringptr dest_string={NULL,0};  /*19102004; */

extern int yyparse(void) ;
extern int yylex();

void clear_expandlabel_data(void)
{
  my_free(_ALLOC_ID_, &dest_string.str);
}

static void str_char_replace(char s[], char chr, char repl_chr)
{
     int i=0;
     while(s[i]!='\0')
     {
           if(s[i]==chr)
           {
               s[i]=repl_chr;
           }  
           i++; 
     }
}

void parse(const char *s)
{
  YY_BUFFER_STATE buf;
  buf=yy_scan_string(s);
  while(yylex()){
    fprintf(errfp, "token:|%s|\n", yytext);
  }
  yy_delete_buffer(buf);
}

const char *expandlabel(const char *s, int *m)
{
 YY_BUFFER_STATE buf;
 my_free(_ALLOC_ID_, &dest_string.str);     /* 30102004  delete 'memory' of previous execution */

 if(!s) {
   *m = -1;
   return NULL;
 }
 if(parselabel_debug >= 3) fprintf(errfp, "expandlabel(): entering\n");
 if(!strpbrk(s, "*,.:")) { /* shortcut: nothing to parse / expand */
   *m = 1;
   my_strdup2(_ALLOC_ID_, &dest_string.str, s);
   return dest_string.str;
 }
 buf=yy_scan_string(s);
 yyparse();
 yy_delete_buffer(buf);
 if(yyparse_error==1) {
   char *cmd = NULL;
   size_t l;
   yyparse_error = -1;
   l = strlen(s)+400;
   cmd = my_malloc(_ALLOC_ID_, l);
   fprintf(errfp, "syntax error in %s\n", s);
   my_snprintf(cmd, l, 
     "tk_messageBox -icon error -type ok -parent [xschem get topwindow] "
     "-message {Syntax error in identifier expansion: %s\nschematic:\n%s}", s, xctx->sch[xctx->currsch]);
   if(has_x) tcleval(cmd);
   my_free(_ALLOC_ID_, &cmd);

 }
 if(parselabel_debug >= 3) 
   fprintf(errfp, "expandlabel(): returning %s  from %s mult=%d\n",dest_string.str, s, dest_string.m);
 if(dest_string.str) {
   *m = dest_string.m;
   if(xctx->netlist_type == CAD_SPICE_NETLIST && bus_char[0] && bus_char[1]) {
     str_char_replace(dest_string.str, '[', bus_char[0]);
     str_char_replace(dest_string.str, ']', bus_char[1]);
   }
 } else {
   *m=-1;
   my_strdup2(_ALLOC_ID_, &dest_string.str, s);
 }
 return dest_string.str;
}


%}
/*
   Lexical analyzer
*/

%x index
%x trailer
%x mult

SP [ \t\n]*
NOTSP [^ \t\n]
INT -?[0-9]+
DDOT {SP}".."{SP}
CLN {SP}":"{SP}
MULTIP {SP}"*"{SP}
LAB [-(a-zA-Z_%$~"+#!/\\<>]
LAB_NODASH [(a-zA-Z_%$~"+#!/\\<>]
  /* includes numbers */
LAB_NUM [-a-zA-Z_%$~"+#!/\\<>0-9]
  /* includes numbers and space and *+(). */
IDX_LAB_NUM_SP [-a-zA-Z_%$~"#!/\\<> \t\n0-9*+().]
  /* identifier, may start with a number */
IDX_ID_N ({LAB_NUM}+({IDX_LAB_NUM_SP}*{LAB_NUM})*)
  /* identifier, not starting with a number */
IDX_ID (-?{LAB_NODASH}+{IDX_LAB_NUM_SP}*)
  /* includes numbers and space and +().  and :*/
LAB_NUM_SP [-a-zA-Z_%$~"#!:/\\<> \t\n0-9+().]
  /* identifier, may start with a number */
ID_NUM ({LAB_NUM}+({LAB_NUM_SP}*{LAB_NUM})*\??)
  /* identifier, not starting with a number */
ID ({LAB}+{LAB_NUM_SP}*\??)
  /* ~#diUV=9_(\#-hgvUY=) */
ID_EXT_PARENTHESIS ([-~"#+/=_a-zA-Z][-#!@\\/:.=_+a-zA-Z0-9]*\([-~"#!@\\/:.=_+a-zA-Z0-9]*\))

%%
  /* this action resets initial condition at start of line. This is extremely useful if previously lexer
   * bailed out in the middle of some non INITIAL start condition due to yyparse() syntax errors */
<mult,index>^.           {
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): matched: ^[^*] |%s|, push back\n",yytext);
                          yyless(0);    /* push entire token back to input */
                          BEGIN(INITIAL); /* reset parser */
                         }

<mult>{
  /* postfix multiplier */
{INT}                   {
                          sscanf(yytext, "%d",&yylval.val);
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): postmult B_NUM: |%s|\n", yytext);
                          BEGIN(INITIAL);
                          return B_NUM;
                        }
}  /* end <mult> */



<trailer>{
  /* recognize characters after a bus label: AA[33:31]_xx  --> _xx */
{IDX_ID_N}		{
                          yylval.ptr.str=NULL;/*19102004 */
                          my_strdup(_ALLOC_ID_, &yylval.ptr.str, yytext); /* freed after use in expandlabel.y */
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): B_TRAILER: |%s|\n", yytext);
                          BEGIN(INITIAL);
                          return B_TRAILER;
			}
}

  /* node indexes: "3:2" "5:1:2" "5..1" "7..1..2" "b:a" "c:b:a" .... */
<index>{
{INT}                   {
                          sscanf(yytext, "%d",&yylval.val);
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): B_IDXNUM: |%s|\n", yytext);
                          return B_IDXNUM;
                        }
{DDOT}                  {
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): B_DOUBLEDOT: |%s|\n", yytext);
                          return B_DOUBLEDOT;
                        }

  /* end vector node index "...]" followed by trailing chars (excluding , and *) : A[33:31]_xx */
\]/[^*,)]		{

                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): close bracket2: %s\n", yytext);
                          BEGIN(trailer);
                          return yytext[0];
			}

  /* end vector node index "...]" */
\]                      {
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): close bracket1: %s\n", yytext);
                          BEGIN(INITIAL);
                          return yytext[0];
                        }
{NOTSP}                 {
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): idx character: |%s|\n", yytext);
                          return yytext[0];
                        }
  /* recognize AA[aa:bb:33] or AA[33:cc:dd] or AA[aa..bb..11] .... */
({IDX_ID}{DDOT}{IDX_ID_N}{DDOT}{IDX_ID_N})|({IDX_ID_N}{DDOT}{IDX_ID}{DDOT}{IDX_ID_N})|({IDX_ID_N}{DDOT}{IDX_ID_N}{DDOT}{IDX_ID}) {
                          yylval.ptr.str=NULL;
                          my_strdup(_ALLOC_ID_, &yylval.ptr.str, yytext);
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): B_NAME3: |%s|\n", yytext);
                          return B_NAME;
                        }

({IDX_ID}{CLN}{IDX_ID_N}{CLN}{IDX_ID_N})|({IDX_ID_N}{CLN}{IDX_ID}{CLN}{IDX_ID_N})|({IDX_ID_N}{CLN}{IDX_ID_N}{CLN}{IDX_ID}) {
                          yylval.ptr.str=NULL;
                          my_strdup(_ALLOC_ID_, &yylval.ptr.str, yytext);
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): B_NAME2: |%s|\n", yytext);
                          return B_NAME;
                        }

  /* recognize AA[width-1:0], AA[0:width-1], AA[width-1..4], AA[3..width+3], AA[aa:bb] AA[aa..bb] */
({IDX_ID}{CLN}{IDX_ID_N})|({IDX_ID_N}{CLN}{IDX_ID})|({IDX_ID}{DDOT}{IDX_ID_N})|({IDX_ID_N}{DDOT}{IDX_ID}) {
                          yylval.ptr.str=NULL;
                          my_strdup(_ALLOC_ID_, &yylval.ptr.str, yytext);
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): B_NAME1: |%s|\n", yytext);
                          return B_NAME;
                        }
}  /* end <index> */

  /* a comment, return as LINE token */
^\*.*                   {
                          yylval.ptr.str=NULL; /*19102004 */
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): B_LINE: |%s|\n",yytext);
                          my_strdup(_ALLOC_ID_, &yylval.ptr.str, yytext); /* freed after use in expandlabel.y */
                          BEGIN(INITIAL);
                          return B_LINE;
                        }
  /* prefix multiplier */
{INT}/{MULTIP}          {
                          sscanf(yytext, "%d",&yylval.val);
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): premult B_NUM: |%s|\n", yytext);
                          BEGIN(INITIAL);
                          return B_NUM;
                        }
  /* a number: if not a multiplier nor a node index return as a node name */
{INT}                   {
                          yylval.ptr.str=NULL;
                          my_strdup(_ALLOC_ID_, &yylval.ptr.str, yytext);
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): B_NAME4: |%s|\n", yytext);
                          BEGIN(INITIAL);
                          return B_NAME;
                        }
{SP}                    {
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): skipping: |%s|\n", yytext);
                        }
  /* comma separator between nodes: chop spaces "a,b ,  c" */
{SP},{SP}               {
                          yylval.val=',';
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): comma: |%s|\n", yytext);
                          BEGIN(INITIAL);
                          return ',';
                        }

  /* recognize the most esotheric identifiers */
{ID_NUM}|{ID_EXT_PARENTHESIS} {
                          yylval.ptr.str=NULL;/*19102004 */
                          my_strdup(_ALLOC_ID_, &yylval.ptr.str, yytext); /* freed after use in expandlabel.y */
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): B_NAME0: |%s|\n", yytext);
                          BEGIN(INITIAL);
                          return B_NAME;
                        }
  /* start vector node index: "aa[" */
\[                      {
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): open bracket: %s\n", yytext);
                          BEGIN(index);
                          return yytext[0];
                        }
  /* "*(aa,bb,cc)" or "*aaa" prefix multiplication*/
{SP}\*{SP}/({ID}|[(])   {
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): pre *: |%s|\n", yytext);
                          BEGIN(INITIAL);
                          return '*';
                        }
  /* "*16" postfix multiplication */
{MULTIP}/{INT}          {
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): post *: |%s|\n", yytext);
                          BEGIN(mult);
                          return '*';
                        }
{NOTSP}                 {
                          if(parselabel_debug >= 3) fprintf(errfp, "yylex(): character: |%s|\n", yytext);
                          BEGIN(INITIAL);
                          return yytext[0];
                        }

<*><<EOF>>              {
                          BEGIN(INITIAL);
                          return 0;
                        }
%%

