/* File: parselabel.l
 * 
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
 * simulation.
 * Copyright (C) 1998-2020 Stefan Frederik Schippers
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

int dbg_var = 0;
extern int netlist_type;
extern int has_x;
extern FILE *errfp;
extern void my_strdup(int id, char **dest, char *src);
extern void my_free(int id, void *ptr);
extern char bus_replacement_char[];
extern void tcleval(const char str[]);
extern void *my_malloc(int id, size_t size);
extern int  my_snprintf(char *str, int size, const char *fmt, ...);

typedef struct          /* used in expandlabel.y */
{
 char *str;             /* label name */
 int m;                 /* label multiplicity, number of wires */
} Stringptr;

/* commented 20170412, fixes problems with older bison/flex versions */
/* #define YYPARSE_PARAM */
#include "expandlabel.h" /* Bison header file */

Stringptr dest_string={NULL,0};  /*19102004; */

static int bracket=0;
extern int yyparse(void) ;
extern int yyparse_error;

void clear_expandlabel_data(void)
{
  my_free(868, &dest_string.str);
}

void strreplace(char s[], char chr, char repl_chr)
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

const char *expandlabel(const char *s, int *m)
{
 YY_BUFFER_STATE buf;
 my_free(869, &dest_string.str);     /* 30102004  delete 'memory' of previous execution */

 if(dbg_var >= 3) fprintf(errfp, "expandlabel(): entering\n");
 buf=yy_scan_string(s);
 bracket=0;
 yyparse();
 yy_delete_buffer(buf);
 if(yyparse_error==1) {
   char *cmd = NULL;
   int l;
   yyparse_error = -1;
   l = strlen(s)+120;
   cmd = my_malloc(526, l);
   fprintf(errfp, "syntax error in %s\n", s);
   my_snprintf(cmd, l, "tk_messageBox -icon error -type ok -message {Syntax error in identifier expansion: %s}", s);
   if(has_x) tcleval(cmd);
   my_free(543, &cmd);

 }
 if(dbg_var >= 3) fprintf(errfp, "expandlabel(): returning %s  from %s mult=%d\n",dest_string.str, s, dest_string.m);
 if(dest_string.str)
    *m = dest_string.m;
 else
    *m=-1;
 if(dest_string.str) {
   if(netlist_type == CAD_SPICE_NETLIST && bus_replacement_char[0]) {
     strreplace(dest_string.str, '[', bus_replacement_char[0]);
     strreplace(dest_string.str, ']', bus_replacement_char[1]);
   }
   return dest_string.str;
 } else {
   return s;
 }
}


%}
/*
   Lexical analyzer
*/

%x label
%x next_to_alias
%x rest


ID [-#+_\\/=a-zA-Z0-9]*[-#@+/_\\/=a-zA-Z]+[-#@+_\\/=a-zA-Z0-9]*
ID_NUM [-#+_\\/=a-zA-Z0-9]+

ID_EXT ([-#+/=_a-zA-Z][-#@\\/:.=_+a-zA-Z0-9]*)|([-#+/.=_a-zA-Z][-#@\\/:=_+a-zA-Z0-9]*)
ID_EXT_PARENTHESIS [-#+/=_a-zA-Z][-#@\\/:.=_+a-zA-Z0-9]*\([-#@\\/:.=_+a-zA-Z0-9]*\)

%%
^(alias|ipin|iopin|opin)[+ \n]+[^+\n ]+/[\n +]+ { 
                          yylval.ptr.str=NULL; /*19102004 */
       			  /* these things are freed after use in expandlabel.y */
			  my_strdup(298, &yylval.ptr.str, yytext);
                          BEGIN(next_to_alias);
			  if(dbg_var >= 3) fprintf(errfp, "yylex(): alias, yytext = %s\n", yytext);
                          return B_LINE;
                        }
<next_to_alias>[\n +]+  {       /* get all white space and return a single separator */
                          BEGIN(label);
			  if(dbg_var >= 3) fprintf(errfp, "yylex(): next_to_alias, yytext = %s\n", yytext);
                          yylval.val=yytext[0];
                          return B_CAR;
                        }
^[^*]                   {
                          if(dbg_var >= 3) fprintf(errfp, "yylex(): matched: ^[^*] |%s|, push back\n",yytext);
                          yyless(0);    /* push entire token back to input */
                          BEGIN(label); /* we know that a label follows. */
                                       /* so go and process it. */
                        }
^\*.*                   {              /* a comment, return as LINE token */
                          yylval.ptr.str=NULL; /*19102004 */
                          if(dbg_var >= 3) fprintf(errfp, "yylex(): B_LINE: %s\n",yytext);
                          my_strdup(299, &yylval.ptr.str, yytext); /* freed after use in expandlabel.y */
                          return B_LINE;
                        }
<label>{

[ \n]+                  {       /* this ends the <label> start condition */
                          BEGIN(rest);
                          yylval.val=yytext[0];
                          if(dbg_var >= 3) fprintf(errfp, "yylex(): B_CAR: %s\n",yytext);
                          return B_CAR;
                        }
\.\.                    {
                          if(dbg_var >= 3) fprintf(errfp, "yylex(): B_DOUBLEDOT: %s\n", yytext);
                          return B_DOUBLEDOT;
                        }
[0-9]+{ID_EXT} {
                          yylval.ptr.str=NULL;
                          my_strdup(452, &yylval.ptr.str, yytext);
                          if(dbg_var >= 3) fprintf(errfp, "yylex(): B_NAME 3: %s\n", yytext);
                          return B_NAME;
                        }

  /* recognize AA[aa:bb:33] or AA[33:cc:dd] or AA[aa..bb..11] .... */
({ID}\.\.{ID_NUM}\.\.{ID_NUM})|({ID_NUM}\.\.{ID}\.\.{ID_NUM})|({ID_NUM}\.\.{ID_NUM}\.\.{ID}) {
                          yylval.ptr.str=NULL;
                          my_strdup(428, &yylval.ptr.str, yytext);
                          if(dbg_var >= 3) fprintf(errfp, "yylex(): B_NAME 2: %s\n", yytext);
                          return B_NAME;
                        }

({ID}:{ID_NUM}:{ID_NUM})|({ID_NUM}:{ID}:{ID_NUM})|({ID_NUM}:{ID_NUM}:{ID}) {
                          yylval.ptr.str=NULL;
                          my_strdup(430, &yylval.ptr.str, yytext);
                          if(dbg_var >= 3) fprintf(errfp, "yylex(): B_NAME 2: %s\n", yytext);
                          return B_NAME;
                        }

  /* recognize AA[width-1:0], AA[0:width-1], AA[width-1..4], AA[3..width+3], AA[aa:bb] AA[aa..bb] */
({ID}:{ID_NUM})|({ID_NUM}:{ID})|({ID}\.\.{ID_NUM})|({ID_NUM}\.\.{ID}) {
                          yylval.ptr.str=NULL;
                          my_strdup(92, &yylval.ptr.str, yytext);
                          if(dbg_var >= 3) fprintf(errfp, "yylex(): B_NAME 1: %s\n", yytext);
                          return B_NAME;
                        }

[0-9]+                  {
                          sscanf(yytext, "%d",&yylval.val);
                          if(bracket) {
                            if(dbg_var >= 3) fprintf(errfp, "yylex(): B_IDXNUM: %s\n", yytext);
                            return B_IDXNUM;
                          }
                          else {
                            if(dbg_var >= 3) fprintf(errfp, "yylex(): B_NUM: %s\n", yytext);
                            return B_NUM;
                          }
                        }

                        /* recognize the most esotheric identifiers */
{ID_EXT}|{ID_EXT_PARENTHESIS} {
                          yylval.ptr.str=NULL;/*19102004 */
                          my_strdup(300, &yylval.ptr.str, yytext); /* freed after use in expandlabel.y */
                          if(dbg_var >= 3) fprintf(errfp, "yylex(): B_NAME: %s\n", yytext);
                          return B_NAME;
                        }
\[                      {
                          bracket++;
                          if(dbg_var >= 3) fprintf(errfp, "yylex(): open bracket: %s\n", yytext);
                          return yytext[0];
                        }
\]                      {
                          bracket--;
                          if(dbg_var >= 3) fprintf(errfp, "yylex(): close bracket: %s\n", yytext);
                          return yytext[0];
                        }
[^ ]                    {
                          if(dbg_var >= 3) fprintf(errfp, "yylex(): returning character: %s\n", yytext);
                          return yytext[0];
                        }
} /* end <label> */

<rest>{                 /* treat rest of line as a single LINE token */
(.|\n)*                 {
                          yylval.ptr.str=NULL; /*19102004 */
                          my_strdup(301, &yylval.ptr.str, yytext); /* freed after use in expandlabel.y */
                          if(dbg_var >= 3) fprintf(errfp, "yylex(): <rest> B_LINE: %s\n",yytext);
                          BEGIN(INITIAL);
                          return B_LINE;
                        }

} /* end rest */
<*><<EOF>>              {
                          BEGIN(INITIAL);
                          return 0;
                        }
%%

