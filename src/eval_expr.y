%{
#include <stdio.h>
#include <math.h>  /* For math functions, cos(), sin(), etc. */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "xschem.h"

static const char *str, *strptr;
static char *ret = NULL;
static int engineering = 0;
static int dbglev = 1;

/* Data type for links in the chain of functions. */
struct symrec
{
  char *name;  /* name of symbol */
  double (*fnctptr)(double);  /* value of a FNCT */
  double value;
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
static void get_char(int c);
static double rnd6(double x) {return round_to_n_digits(x, 6);}

struct fn
{
  char *fname;
  double (*fnct)(double);
  double value;
};
static int lex_state = 0;
struct fn fn_array[]
  = {
      {"int"      , toint, 0},
      {"sin"      , sin, 0},
      {"cos"      , cos, 0},
      {"exp"      , exp, 0},
      {"asin"     , asin, 0},
      {"acos"     , acos, 0},
      {"atan"     , atan, 0},
      {"log"      , log10, 0},
      {"ln"       , log, 0},
      {"exp"      , exp, 0},
      {"sqrt"     , sqrt, 0},
      {"round"    , rnd6, 0},
      {"pi"       , NULL, 3.141592653589793},
      {"e"        , NULL, 2.718281828459045},
      {"k"        , NULL, 1.380649e-23},
      {"h"        , NULL, 6.62607e-34},
      {"echarge"  , NULL, 1.60217646e-19},
      {"abszero"  , NULL, 273.15},
      {"c"        , NULL, 2.99792458e8},
      {0          , 0, 0}
    };
%}


/* %define api.prefix {kk} */
%union {
int          c;
double     val; /* For returning numbers. */
symrec  *tptr; /* For returning symbol-table pointers */
}

%token STREND 0
%token <c> XCHAR
%token EXPR       /* expr( */
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
input:
        | input line 
;

line:   
          XCHAR                   {get_char($1);}
        | EXPR exp ')'            {get_expr($2);lex_state = 0;}
        | EXPR '\'' exp '\'' ')'  {get_expr($3);lex_state = 0;}
        | EXPR exp error          
;

exp:      NUM                     {$$ = $1;}
        | FNCT '(' exp ')'        {$$ = $1 ? ($1->fnctptr ? (*($1->fnctptr))($3) : 0.0) : 0.0;}
        | FNCT                    {$$ = $1 ? $1->value : 0.0;}
        | exp '+' exp             {$$ = $1 + $3;		 }
        | exp '-' exp             {$$ = $1 - $3;}
        | exp '*' exp             {$$ = $1 * $3;}
        | exp '%' exp             {$$ = (int)$1 % (int)$3;}
        | exp '/' exp             {$$ = $1 / $3;}
        | '-' exp %prec NEG       {$$ = -$2;}
        | exp '^' exp             {$$ = pow ($1, $3);}
        | '(' exp ')'             {$$ = $2;}
;
/* End of grammar */
%%

static void get_char(int c)
{
  char s[2];
  dbg(dbglev, "get_char: %c |%s|\n", c, str);
  s[0] = (char)c;
  s[1] = '\0';
  my_mstrcat(_ALLOC_ID_, &ret, s, NULL);
  strptr = str;
}

static void get_expr(double x)
{
  char xx[100];
  dbg(dbglev,"get_expr(): x=%g\n", x);
  if(engineering) {
    my_snprintf(xx, S(xx), "%s", dtoa_eng(x));
  } else {
    my_snprintf(xx, S(xx), "%.15g", x);
  }
  my_mstrcat(_ALLOC_ID_, &ret, xx, NULL);
  strptr = str;
}

static double toint(double x)
{
  if(x < 0.0) return ceil(x);
  return floor(x);
}

/* ad=expr(3*xa) pd=expr(2*(3+xa)) --> ad=3*xa pd=2*(3+xa) */
static void remove_expr(char *s)
{
  char *ptr = s;
  int plev = 0;
  while(*ptr) {
    if(strstr(ptr, "expr(") == ptr) {
      ptr += 5;
      plev++;
    }
    else if(strstr(ptr, "expr_eng(") == ptr) {
      ptr += 9;
      plev++;
    }
    if(*ptr == '(') plev++;
    if(*ptr == ')') {
      plev--;
      if(plev == 0) ptr++;
      if(!ptr) break;
    }
    *s = *ptr;
    ptr++;
    s++;
  }
  *s = *ptr;
}


static void kkerror(char *s)  /* Called by kkparse on error */
{
  char *ss = NULL;
  dbg(dbglev, "error: |%s|\n\n   |%s|\n", s, str ? str : "<NULL>");
  my_strdup2(_ALLOC_ID_, &ss, strptr);
  remove_expr(ss);
  my_mstrcat(_ALLOC_ID_, &ret, ss, NULL);
  my_free(_ALLOC_ID_, &ss);
  lex_state = 0;
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
    ptr->value = fn_array[i].value;
  }
}

void eval_expr_clear_table(void)
{
  symrec *ptr = sym_table;
  while(ptr) {
    symrec *tmp = ptr;
    ptr = ptr->next;
    my_free(_ALLOC_ID_, &(tmp->name));
    my_free(_ALLOC_ID_, &tmp);
  }
}
    

static int kklex()
{
  int c;

  if(!str) { return 0; }
  if(strstr(str, "expr(") == str) {
     lex_state = 1; 
     str += 5;
     dbg(dbglev, "lex(): EXPR\n");
     engineering = 0;
     return EXPR;
  }

  else if(strstr(str, "expr_eng(") == str) {
     lex_state = 1;
     str += 9;
     dbg(dbglev, "lex(): EXPR_ENG\n");
     engineering = 1;
     return EXPR;
  }

  if(!lex_state) {
    c = *str++;
    if(c) {
      kklval.c = c;
      dbg(dbglev, "lex(): XCHAR; %c\n", c);
      return XCHAR;
    } else {
     dbg(dbglev, "lex(): STREND\n");
     return STREND;
    }
  }
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

    sscanf(str, "%99[.0-9a-zA-Z_-]%n", s, &rd);
    kklval.val = atof_eng(s);
    str += rd;
    dbg(dbglev, "lex(): NUM: %s\n", s);
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
    s = getsym(symbuf);
    kklval.tptr = s;
    dbg(dbglev, "ylex: FNCT=%s\n", symbuf);
    if(s) return FNCT;
    return 0; /* error : undefined identifier */
  }
  /* Any other character is a token by itself.        */
  return c;
}

char *eval_expr(const char *s)
{
  lex_state = 0;
  if(ret) my_free(_ALLOC_ID_, &ret);
  strptr = str = s;
  kkparse();
  return ret;
}
