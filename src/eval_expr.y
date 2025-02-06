%{
#include <stdio.h>
#include <math.h>  /* For math functions, cos(), sin(), etc. */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "xschem.h"

static const char *str, *strptr;
static char *ret = NULL;

/* Data type for links in the chain of functions. */
struct symrec
{
  char *name;  /* name of symbol */
  double (*fnctptr)();  /* value of a FNCT */
  struct symrec *next;    /* link field */
};

typedef struct symrec symrec;

/* The symbol table: a chain of `struct symrec'. */
symrec *sym_table = (symrec *)0;

static symrec *getsym(char *sym_name);
static symrec *putsym(char *sym_name);
static int kklex();
static void kkerror(char *s);
static double toint(double x);
static void get_expr(double x);

struct fn
{
  char *fname;
  double (*fnct)();
};

struct fn fn_array[]
  = {
      {"int" , toint},
      {"sin" , sin},
      {"cos" , cos},
      {"asin", asin},
      {"acos", acos},
      {"atan", atan},
      {"log"  , log10},
      {"ln"  , log},
      {"exp" , exp},
      {"sqrt", sqrt},
      {0     , 0}
    };
%}

%define api.prefix {kk}
%union {
double     val; /* For returning numbers. */
symrec  *tptr; /* For returning symbol-table pointers */
}

%token STREND 0
%token <val>  NUM /* Simple double precision number */
%token <tptr> FNCT /* Variable and Function */
%type  <val>  exp
%right '='
%left '-' '+'
%left '*' '/' '%'
%left NEG /* Negation--unary minus */
%right '^' /* Exponentiation */

/* Grammar follows */
%%

line:   
        |      exp STREND        {get_expr($1);}
        | '\'' exp '\'' STREND   {get_expr($2);}
;

exp:      NUM                { $$ = $1;}
        | FNCT '(' exp ')'   { $$ = $1 ? (*($1->fnctptr))($3) : 0.0;}
        | exp '+' exp        { $$ = $1 + $3;		 }
        | exp '-' exp        { $$ = $1 - $3;}
        | exp '*' exp        { $$ = $1 * $3;}
        | exp '%' exp        { $$ = (int)$1 % (int)$3;}
        | exp '/' exp        { $$ = $1 / $3;}
        | '-' exp  %prec NEG { $$ = -$2;}
        | exp '^' exp        { $$ = pow ($1, $3);}
        | '(' exp ')'        { $$ = $2;}
;
/* End of grammar */
%%

static void get_expr(double x)
{
  char xx[100];
  my_snprintf(xx, S(xx), "%.15g", x);
  my_mstrcat(_ALLOC_ID_, &ret, xx, NULL);
  strptr = str;
}

static double toint(double x)
{
  if(x < 0.0) return ceil(x);
  return floor(x);
}

static void kkerror(char *s)  /* Called by kkparse on error */
{
  /* printf("error: |%s|\n\n   |%s|\n", s, strptr); */
  my_mstrcat(_ALLOC_ID_, &ret, strptr, NULL);
}

static symrec *getsym(char *sym_name)
{
  symrec *ptr;
  for (ptr = sym_table; ptr; ptr = ptr->next)
    if (strcmp (ptr->name,sym_name) == 0) return ptr;
  return NULL;
}

symrec * putsym (char *sym_name)
{
  symrec *ptr;
  ptr = (symrec *) my_malloc(_ALLOC_ID_, sizeof (symrec));
  ptr->name = (char *) my_malloc(_ALLOC_ID_, strlen (sym_name) + 1);
  strcpy (ptr->name,sym_name);
  ptr->next = (struct symrec *)sym_table;
  sym_table = ptr;
  return ptr;
}

void eval_expr_init_table(void)  /* puts arithmetic functions in table. */
{
  int i;
  symrec *ptr;
  for (i = 0; fn_array[i].fname != 0; i++)
  {
    ptr = putsym (fn_array[i].fname);
    ptr->fnctptr = fn_array[i].fnct;
  }
}

void eval_expr_clear_table(void)
{
  symrec *ptr;
  for (ptr = sym_table; ptr; ptr = ptr->next) {
    symrec *tmp = ptr;
    my_free(_ALLOC_ID_, &(tmp->name));
    my_free(_ALLOC_ID_, &tmp);
  }
}

static int kklex()
{
  int c;

  if(!str) { return 0; }
  /* Ignore whitespace, get first nonwhite character.  */
  while ((c = *str++) == ' ' || c == '\t' || c == '\n');
  if (c == 0) {
    str = NULL;
    return 0;
  }
  /* Char starts a number => parse the number.         */
  if (c == '.' || isdigit (c))
  {
    char s[100] ="";
    int rd = 0;
    str--; 

    sscanf(str, "%99[.0-9a-zA-Z_]%n", s, &rd);
    kklval.val = atof_spice(s);
    str += rd;
    /* printf("yylex: NUM: %s\n", s); */
    return NUM;
  }
  /* Char starts an identifier => read the name.       */
  if(isalpha(c)) {
    symrec *s;
    int length = 40; /* max length of function names */
    char symbuf[41];
    int i = 0;
    /* Initially make the buffer int enough
     * for a 40-character symbol name.  */
    do
    {
      symbuf[i++] = (char) c; /* Add this character to the buffer.*/
      c = *str++; /* Get another character.*/
    }
    while (c != 0 && isalnum(c) && i < length);
    str--;
    symbuf[i] = '\0';
    s = getsym (symbuf);
    kklval.tptr = s;
    /* printf("yylex: FNCT=%s\n", symbuf); */
    return FNCT;
  }
  /* Any other character is a token by itself.        */
  return c;
}

char *eval_expr(const char *s)
{
  if(ret) my_free(_ALLOC_ID_, &ret);
  strptr = str = s;
  kkparse();
  return ret;
}
