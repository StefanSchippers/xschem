/* File: expandlabel.y
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

/* label parser */

%{
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct          /* used in expandlabel.y */
{
 char *str;             /* label name */
 int m;                /* label multiplicity, number of wires */
} Stringptr;


#define YYERROR_VERBOSE 
#define SIGN(x) ( (x) < 0 ? -1: 1 )
#define INITIALIDXSIZE 8

extern Stringptr dest_string; /* 20140108 */

static int idxsize=INITIALIDXSIZE;
extern int yylex();
extern FILE *errfp;
extern void *my_malloc(int id, size_t size);
extern void my_free(int id, void *ptr);
extern void my_realloc(int id, void *ptr,size_t size);
extern void my_strdup(int id, char **dest, char *src);
extern int debug_var;
extern void dbg(int level, char *fmt, ...);
extern int  my_snprintf(char *str, int size, const char *fmt, ...);
extern int yyparse_error;



static void yyerror (const char *s)  /* Called by yyparse on error */
{
  if(yyparse_error == 0 ) yyparse_error = 1;
  dbg(0, "yyerror(): yyparse():%s\n", s);
}

static char *expandlabel_strdup(char *src)
{
 char *ptr;
 if(src==NULL || src[0]=='\0') {
   ptr=NULL;
   my_strdup(121, &ptr,"");
   return ptr;
 }
 else
 {
  ptr=NULL;
  my_strdup(122, &ptr,src);
  dbg(3, "expandlabel_strdup: duplicated %lu string %s\n",(unsigned long)ptr,src);
  return ptr;
 }
}

static char *my_strcat2(char *s1, char c, char *s2)
/* concatenates s1 and s2, with c in between */
{
 int l1=0,l2=0;
 char *res;

 if(s1!=NULL) l1=strlen(s1);
 if(s1!=NULL) l2=strlen(s2);
 res=my_malloc(123, l1+l2+2); /* 2 strings plus 'c' and '\0' */
 
 /* 20180923 */
 memcpy(res, s1, l1);
 res[l1] = c;
 memcpy(res + l1 + 1, s2, l2+1);
 /* *res='\0'; */
 /* if(l1) strcpy(res, s1); */
 /* res[l1]=c; */
 /* if(l2) strcpy(res+l1+1, s2); */
 return res;

}
/* */
/* example: */
/* if n==3 and s="a,b,c" return "a,a,a,b,b,b,c,c,c" */
/* */
static char *my_strmult2(int n, char *s)
/* if n==0 returns "\0" */
{
 register int i, len;
 register char *pos,*prev;
 char *str, *ss;

 dbg(3, "my_strmult2: n=%d s=%s\n", n, s);
 if(n==0) return expandlabel_strdup("");
 len=strlen(s);
 prev=s;
 ss = str=my_malloc(124,  (len+1)*n);
 str[0]='\0';
 for(pos=s;pos<=s+len;pos++) {
   if(*pos==',' || *pos=='\0') {
     for(i=0;i<n;i++) {
       memcpy(ss, prev, pos-prev); /* 20180923 */
       if(i == n - 1 && *pos == '\0') {
         ss[pos - prev] = '\0';
       } else {
         ss[pos - prev] = ',';
       }
       ss += pos-prev+1; /* 20180923 */
     }
     /* if(*pos==',') strcat(str,","); */
     prev=pos+1;
   }
 }
 return str;
}

/* */
/* example: */
/* if n==3 and s="a,b,c" return "a,b,c,a,b,c,a,b,c" */
/* */
static char *my_strmult(int n, char *s)
/* if n==0 returns "\0" */
{
 register int i, len;
 register char *pos;
 char *str;

 if(n==0) return expandlabel_strdup("");
 len=strlen(s);
 str=pos=my_malloc(125,  (len+1)*n);
 for(i=1;i<=n;i++)
 {
  /* strcpy(pos,s); */
  memcpy(pos, s, len); /* 20180923 */
  pos[len]=',';
  pos+=len+1;
 } 
 *(pos-1)='\0';
 return str;
}

static char *my_strbus(char *s, int *n)
{
 int i,l;
 int tmplen;
 char *res=NULL;
 char *tmp=NULL;
 my_realloc(126, &res, n[0]*(strlen(s)+20));
 my_realloc(127, &tmp, strlen(s)+30);
 l=0;
 for(i=1;i<n[0];i++)
 {
  tmplen = sprintf(tmp, "%s[%d],", s, n[i]);
  /* strcpy(res+l,tmp); */
  memcpy(res+l,tmp, tmplen+1); /* 20180923 */
  l+=tmplen;
 }
 my_free(735, &tmp);
 sprintf(res+l, "%s[%d]", s, n[i]);
 return res;
}

static void check_idx(int **ptr,int n)
{
 if(n>=idxsize) 
 {
  idxsize*=2;
  dbg(3, "check_idx(): reallocating idx array: size=%d\n",idxsize);
  my_realloc(128, ptr, idxsize*sizeof(int));
 }
}

static char *my_strbus_nobracket(char *s, int *n)
{
 int i,l;
 int tmplen;
 char *res=NULL;
 char *tmp=NULL;
 my_realloc(107, &res, n[0]*(strlen(s)+20));
 my_realloc(108, &tmp, strlen(s)+30);
 l=0;
 for(i=1;i<n[0];i++)
 {
  tmplen = sprintf(tmp, "%s%d,", s, n[i]);
  /* strcpy(res+l,tmp); */
  memcpy(res+l,tmp, tmplen+1); /* 20180923 */
  l+=tmplen;
 }
 my_free(736, &tmp);
 sprintf(res+l, "%s%d", s, n[i]);
 return res;
}

%}


%union{
int   val;  /* For returning numbers.*/
Stringptr ptr;  /* strings, that is, identifiers */
char * str;
int  *idx;  /* for bus index & bus index ranges */
}

/* BISON Declarations: terminal tokens*/
%token <val> B_NUM
%token <val> B_CAR
%token <val> B_IDXNUM
%token <val> B_DOUBLEDOT
%token <str> B_NAME
%token <str> B_LINE
/* BISON Declarations: non terminal symbols*/
%type <ptr> list
%type <idx> index
%type <idx> index_nobracket

/* operator precedences (bottom = highest)  and associativity  */
%left B_DOUBLEDOT
%left B_CAR
%left ','
%left ':'
%left '*'

/* Grammar follows */
%%
input:    /* empty string. allows ctrl-D as input */
        | input line
;
line:     list          {
                         my_strdup(129,  &(dest_string.str),$1.str); /*19102004 */
                         my_free(737, &$1.str); /*19102004 */
                         dest_string.m=$1.m;
                        }
        | B_NUM         {
                         char n[40];
                         dbg(3, "yyparse(): B_NUM = %d\n", $1);
                         sprintf(n, "%d", $1);
                         my_strdup(158,  &(dest_string.str),n); /*19102004 */
                         dest_string.m = 1;
                        }
          
;
list:     B_NAME        { 
                         dbg(3, "yyparse(): B_NAME (%lu) \n", (unsigned long) $1);
                         dbg(3, "yyparse(): B_NAME, $1=%s\n", $1);
                         $$.str = expandlabel_strdup($1); /* 19102004 prima era =$1 */
                         my_free(738, &$1);  /*191020004 */
                         $$.m = 1;
                        }
        | B_LINE        {
                         dbg(3, "yyparse(): B_LINE\n");
                         $$.str = expandlabel_strdup($1); /* 19102004 prima era =$1 */
                         my_free(739, &$1);  /*191020004 */
                         $$.m = 1;
                        }
        | list '*' B_NUM{
                         dbg(3, "yyparse(): list * B_NUM\n");
                         dbg(3, "yyparse(): |%s| %d \n",$1.str,$3);
                         $$.str=my_strmult2($3,$1.str);
                         dbg(3, "yyparse(): |%s|\n",$$.str);
                         $$.m = $3 * $1.m;
                         my_free(740, &$1.str);
                        }
        | B_NUM '*' list{
                         dbg(3, "yyparse(): B_NUM * list\n");
                         $$.str=my_strmult($1,$3.str);
                         $$.m = $1 * $3.m;
                         my_free(741, &$3.str);
                        }
        | list ',' list { 
                         dbg(3, "yyparse(): list , list\n");
                         $$.str=my_strcat2($1.str, ',', $3.str);
                         $$.m = $1.m + $3.m;
                         my_free(742, &$1.str);
                         my_free(743, &$3.str);
                        }
        | list B_CAR list
                        {
                         dbg(3, "yyparse(): list B_CAR list\n");
                         $$.str=my_strcat2($1.str, $2, $3.str);
                         $$.m = $1.m + $3.m;
                         my_free(744, &$1.str);
                         my_free(745, &$3.str);
                        }
        | '(' list ')'  {
                         dbg(3, "yyparse(): ( list )\n");
                         $$=$2;
                        }
        | B_NAME  '[' B_NAME  ']' 
                        {
                         int size = strlen($1) + strlen($3) + 3;
                         $$.str = my_malloc(81, size);
                         $$.m=-1;
                         my_snprintf($$.str, size, "%s[%s]", $1, $3);
                         my_free(746, &$1);
                         my_free(747, &$3);
                        }
        | B_NAME  '[' index  ']' 
                        {
                         dbg(3, "yyparse(): making bus: n=%d\n",$3[0]);
                         dbg(3, "yyparse(): B_NAME[ index ] , $1=%s $3=%d\n", $1, *$3);
                         $$.str=my_strbus($1,$3);
                         my_free(748, &$1); 
                         dbg(3, "yyparse(): done making bus: n=%d\n",$3[0]);
                         $$.m=$3[0];
                         my_free(749, &$3); /*19102004 */
                         idxsize=INITIALIDXSIZE;
                        }
        | B_NAME  '[' index_nobracket  ']' 
                        {
                         dbg(3, "yyparse(): making nobracket bus: n=%d\n",$3[0]);
                         $$.str=my_strbus_nobracket($1,$3);
                         my_free(750, &$1);
                         dbg(3, "yyparse(): done making nobracket bus: n=%d\n",$3[0]);
                         $$.m=$3[0];
                         my_free(751, &$3); /*19102004 */
                         idxsize=INITIALIDXSIZE;
                        }
;
index:    B_IDXNUM ':' B_IDXNUM ':' B_IDXNUM
                        {
                         int i;                                 /* 20120229 */
                         int sign;

                         sign = SIGN($3-$1);
                         $$=my_malloc(130, INITIALIDXSIZE*sizeof(int));
                         $$[0]=0;
                         dbg(3, "yyparse(): parsing first idx range\n");
                         for(i=$1;;i+=sign*$5)
                         {
                          check_idx(&$$,++$$[0]);
                          $$[$$[0]]=i;

                          if(sign==1 && i>=$3) break;
                          if(sign==-1 && i<=$3) break;
                         }                                      /* /20120229 */
                        }
        | B_IDXNUM ':' B_IDXNUM
                        {
                         int i;
                         $$=my_malloc(131, INITIALIDXSIZE*sizeof(int));
                         $$[0]=0;
                         dbg(3, "yyparse(): parsing first idx range\n");
                         for(i=$1;;i+=SIGN($3-$1))
                         {
                          check_idx(&$$,++$$[0]);
                          $$[$$[0]]=i;
                          if(i==$3) break;
                         }
                        }
        | B_IDXNUM      { 
                         dbg(3, "yyparse(): parsing first idx item\n");
                         $$=my_malloc(132, INITIALIDXSIZE*sizeof(int));
                         $$[0]=0;
                          check_idx(&$$, ++$$[0]);
                         $$[$$[0]]=$1;
                        }
        | index ',' B_IDXNUM ':' B_IDXNUM ':' B_IDXNUM
                        {
                         int i;                                 /* 20120229 */
                         int sign;

                         sign = SIGN($5-$3);
                         dbg(3, "yyparse(): parsing comma sep idx range\n");
                         for(i=$3;;i+=sign*$7)
                         {
                          check_idx(&$$, ++$$[0]);
                          $$[$$[0]]=i;
                          if(sign==1 && i>=$5) break;
                          if(sign==-1 && i<=$5) break;
                         }                                      /* /20120229 */
                        }
        | index ',' B_IDXNUM ':' B_IDXNUM
                        {
                         int i;
                         dbg(3, "yyparse(): parsing comma sep idx range\n");
                         for(i=$3;;i+=SIGN($5-$3))
                         {
                          check_idx(&$$, ++$$[0]);
                          $$[$$[0]]=i;
                          if(i==$5) break;
                         }
                        }
        | index ',' B_IDXNUM
                        {
                         dbg(3, "yyparse(): parsing comma sep idx list\n");
                          check_idx(&$$, ++$$[0]);
                         $$[$$[0]]=$3;
                        }
;
index_nobracket: B_IDXNUM B_DOUBLEDOT B_IDXNUM
                        {
                         int i;
                         $$=my_malloc(85, INITIALIDXSIZE*sizeof(int));
                         $$[0]=0;
                         dbg(3, "yyparse(): doubledot\n");
                         for(i=$1;;i+=SIGN($3-$1))
                         {
                          check_idx(&$$,++$$[0]);
                          $$[$$[0]]=i;
                          if(i==$3) break;
                         }
                        }
	| B_IDXNUM B_DOUBLEDOT B_IDXNUM B_DOUBLEDOT B_IDXNUM
			{
                         int i;
                         int sign;

                         sign = SIGN($3-$1);
                         $$=my_malloc(109, INITIALIDXSIZE*sizeof(int));
                         $$[0]=0;
                         dbg(3, "yyparse(): parsing first idx range\n");
                         for(i=$1;;i+=sign*$5)
                         {
                          check_idx(&$$,++$$[0]);
                          $$[$$[0]]=i;

                          if(sign==1 && i>=$3) break;
                          if(sign==-1 && i<=$3) break;
                         }
			}

%%

