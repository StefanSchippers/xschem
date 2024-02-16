/* File: token.c
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

#include "xschem.h"
#define SPACE(c)  ( c=='\n' || c==' ' || c=='\t' || c=='\0' || c==';' )

enum status {TOK_BEGIN, TOK_TOKEN, TOK_SEP, TOK_VALUE, TOK_END, TOK_ENDTOK};

unsigned int str_hash(const char *tok)
{
  register unsigned int hash = 5381;
  register unsigned int c;

  while ( (c = (unsigned char)*tok++) ) {
    hash += (hash << 5) + c;
  }
  return hash;
}

static char *find_bracket(char *s)
{
 while(*s!='['&& *s!='\0') s++;
 return s;
}

void floater_hash_all_names(void)
{
  int i;
  int_hash_free(&xctx->floater_inst_table);
  int_hash_init(&xctx->floater_inst_table, HASHSIZE);
  for(i=0; i<xctx->instances; ++i) { 
    if(xctx->inst[i].instname && xctx->inst[i].instname[0]) {
      int_hash_lookup(&xctx->floater_inst_table, xctx->inst[i].instname, i, XINSERT);
    }
  }
}  

/* if cmd is wrapped inside tcleval(...) pass the content to tcl
 * for evaluation, return tcl result. If no tcleval(...) found return copy of cmd */
const char *tcl_hook2(const char *cmd)
{
  static char *result = NULL;
  static const char *empty="";
  char *unescaped_res;

  if(cmd == NULL) {
    my_free(_ALLOC_ID_, &result);
    return empty;
  }
  if(strstr(cmd, "tcleval(") == cmd) {
    unescaped_res = str_replace(cmd, "\\}", "}", 0);
    tclvareval("tclpropeval2 {", unescaped_res, "}" , NULL);
    my_strdup2(_ALLOC_ID_, &result, tclresult());
    /* dbg(0, "tcl_hook2: return: %s\n", result);*/
  } else {
    /* dbg(0, "tcl_hook2: return: %s\n", cmd); */
    my_strdup2(_ALLOC_ID_, &result, cmd);
  }
  return result;
}

int is_generator(const char *name)
{
  #ifdef __unix__
  int res = 0;
  static regex_t *re = NULL; 
  
  if(!name) {
    if(re) {
      regfree(re);
      my_free(_ALLOC_ID_, &re);
    }
    return 0;
  }
  if(!re) {
    re = my_malloc(_ALLOC_ID_, sizeof(regex_t));
    regcomp(re, "^[^ \t()]+\\([^()]*\\)[ \t]*$", REG_NOSUB | REG_EXTENDED);
  }
  if(!regexec(re, name, 0 , NULL, 0) ) res = 1;
  dbg(1, "is_generator(%s)=%d\n", name, res);
  /* regfree(&re); */
  return res;
  #else
  if (!name) return 0;
  char cmd[PATH_MAX+100];
  my_snprintf(cmd, S(cmd), "regexp {%s} {%s} [list %s]", "-nocase", "^[^ \\t()]+\\([^()]*\\)[ \\t]*$", name);
  tcleval(cmd);
  int ret = atoi(tclresult());
  if (ret > 0)
      return 1;
  return 0;
  #endif
}

/* cleanup syntax of symbol generators: xxx(a,b,c) --> xxx_a_b_c */
const char *sanitize(const char *name) 
{
  static char *s = NULL;
  static char *empty="";
  if(name == NULL) {
    my_free(_ALLOC_ID_, &s);
    return empty;
  }
  tclvareval("regsub -all { *[(),] *} {", name, "} _", NULL);
  tclvareval("regsub  {_$} {", tclresult(), "} {}", NULL);
  my_strdup2(_ALLOC_ID_, &s, tclresult());
  return s;
}

/* caller must free returned string
 * given xxxx(a,b,c) return /path/to/xxxx a b c
 * if no xxxx generator file found return NULL */
char *get_generator_command(const char *str)
{
  char *cmd = NULL;
  char *gen_cmd = NULL;
  const char *cmd_filename;
  char *spc_idx;
  struct stat buf;

  dbg(1, "get_generator_command(): symgen=%s\n",str);
  cmd = str_chars_replace(str, " (),", ' '); /* transform str="xxx(a,b,c)" into cmd="xxx a b c" */
  spc_idx = strchr(cmd, ' ');
  if(!spc_idx) {
    goto end;
  }
  *spc_idx = '\0';
  cmd_filename = abs_sym_path(cmd, "");
  if(stat(cmd_filename, &buf)) { /* symbol generator not found */
    goto end;
  }
  #ifdef __unix__
  /* my_strdup(_ALLOC_ID_, &gen_cmd, cmd_filename); */
  /* add quotes to protect spaces in cmd path */
  my_mstrcat(_ALLOC_ID_, &gen_cmd, "\"", cmd_filename, "\"", NULL);
  *spc_idx = ' ';
  my_strcat(_ALLOC_ID_, &gen_cmd, spc_idx);
  #else
  /* tclsh "cmd_filename" a b c */
  /* command tclsh is needed so new TCL windows will NOT open */
  /* quotes are needed for filename if filename has spaces */
  *spc_idx = ' ';
  int len = 8 + strlen(cmd_filename) + strlen(spc_idx) + 1; /*8="tclsh "+ "\""*2*/
  gen_cmd = my_malloc(_ALLOC_ID_, len * sizeof(char));
  my_snprintf(gen_cmd, len, "tclsh \"%s\"%s", cmd_filename, spc_idx);
  #endif
  dbg(1, "get_generator_command(): cmd_filename=%s\n", cmd_filename);
  dbg(1, "get_generator_command(): gen_cmd=%s\n", gen_cmd);
  dbg(1, "get_generator_command(): is_symgen=%d\n", is_generator(str));

  end:
  my_free(_ALLOC_ID_, &cmd);
  return gen_cmd;
}

int match_symbol(const char *name)  /* never returns -1, if symbol not found load systemlib/missing.sym */
{
  int i,found;
 
  found=0;
  for(i=0;i<xctx->symbols; ++i) {
    /* dbg(1, "match_symbol(): name=%s, sym[i].name=%s\n",name, xctx->sym[i].name);*/
    if(xctx->x_strcmp(name, xctx->sym[i].name) == 0)
    {
      dbg(1, "match_symbol(): found matching symbol:%s\n",name);
      found=1;break;
    }
  }
  if(!found) {
    dbg(1, "match_symbol(): matching symbol not found: loading %s\n", name);
    load_sym_def(name, NULL); /* append another symbol to the xctx->sym[] array */
  }
  dbg(1, "match_symbol(): returning %d\n",i);
  return i;
}

/* update **s modifying only the token values that are */
/* different between *new and *old */
/* return 1 if s modified 20081221 */
int set_different_token(char **s,const char *new, const char *old)
{
 register int c, state=TOK_BEGIN, space;
 char *token=NULL, *value=NULL;
 size_t sizetok=0, sizeval=0;
 size_t token_pos=0, value_pos=0;
 int quote=0;
 int escape=0;
 int mod;
 const char *my_new;

 mod=0;
 my_new = new;
 dbg(1, "set_different_token(): *s=%s, new=%s, old=%s\n",*s, new, old);
 if(new==NULL) return 0;

 sizeval = sizetok = CADCHUNKALLOC;
 my_realloc(_ALLOC_ID_, &token, sizetok);
 my_realloc(_ALLOC_ID_, &value, sizeval);

 /* parse new string and add / change attributes that are missing / different from old */
 while(1) {
  c=*my_new++;
  space=SPACE(c) ;
  if(c=='"' && !escape) quote=!quote;
  if( (state==TOK_BEGIN || state==TOK_ENDTOK) && !space && c != '=') state=TOK_TOKEN;
  else if( state==TOK_TOKEN && space) state=TOK_ENDTOK;
  else if( (state==TOK_TOKEN || state==TOK_ENDTOK) && c=='=') state=TOK_SEP;
  else if( state==TOK_SEP && !space) state=TOK_VALUE;
  else if( state==TOK_VALUE && space && !quote && !escape) state=TOK_END;
  STR_ALLOC(&value, value_pos, &sizeval);
  STR_ALLOC(&token, token_pos, &sizetok);
  if(state==TOK_TOKEN) token[token_pos++]=(char)c;
  else if(state==TOK_VALUE) {
   value[value_pos++]=(char)c;
  }
  else if(state==TOK_ENDTOK || state==TOK_SEP) {
   if(token_pos) {
     token[token_pos]='\0';
     token_pos=0;
   }
  } else if(state==TOK_END) {
   value[value_pos]='\0';
   value_pos=0;
   if(strcmp(value, get_tok_value(old,token,1))) {
    mod=1;
    my_strdup(_ALLOC_ID_, s, subst_token(*s, token, value) );
   }
   state=TOK_BEGIN;
  }
  escape = (c=='\\' && !escape);
  if(c=='\0') break;
 }

 state = TOK_BEGIN;
 escape = quote = 0;
 token_pos = value_pos = 0;
 /* parse old string and remove attributes that are not present in new */
 while(old) {
  c=*old++;
  space=SPACE(c) ;
  if(c=='"' && !escape) quote=!quote;
  if( (state==TOK_BEGIN || state==TOK_ENDTOK) && !space && c != '=') state=TOK_TOKEN;
  else if( state==TOK_TOKEN && space) state=TOK_ENDTOK;
  else if( (state==TOK_TOKEN || state==TOK_ENDTOK) && c=='=') state=TOK_SEP;
  else if( state==TOK_SEP && !space) state=TOK_VALUE;
  else if( state==TOK_VALUE && space && !quote && !escape) state=TOK_END;
  STR_ALLOC(&value, value_pos, &sizeval);
  STR_ALLOC(&token, token_pos, &sizetok);
  if(state==TOK_TOKEN) token[token_pos++]=(char)c;
  else if(state==TOK_VALUE) {
   value[value_pos++]=(char)c;
  }
  else if(state==TOK_ENDTOK || state==TOK_SEP) {
   if(token_pos) {
     token[token_pos]='\0';
     token_pos=0;
   }
   get_tok_value(new,token,1);
   if(xctx->tok_size == 0 ) {
    mod=1;
    my_strdup(_ALLOC_ID_, s, subst_token(*s, token, NULL) );
   }
  } else if(state==TOK_END) {
   value[value_pos]='\0';
   value_pos=0;
   state=TOK_BEGIN;
  }
  escape = (c=='\\' && !escape);
  if(c=='\0') break;
 }
 my_free(_ALLOC_ID_, &token);
 my_free(_ALLOC_ID_, &value);
 return mod;
}

/* return a string containing the list of all tokens in s */
/* with_quotes: */
/* 0: eat non escaped quotes (") */
/* 1: return unescaped quotes as part of the token value if they are present */
/* 2: eat backslashes */
const char *list_tokens(const char *s, int with_quotes)
{
  static char *token=NULL;
  size_t  sizetok=0;
  register int c, state=TOK_BEGIN, space;
  register size_t token_pos=0;
  int quote=0;
  int escape=0;

  if(s==NULL) {
    my_free(_ALLOC_ID_, &token);
    sizetok = 0;
    return "";
  }
  sizetok = CADCHUNKALLOC;
  my_realloc(_ALLOC_ID_, &token, sizetok);
  token[0] = '\0';
  while(1) {
    c=*s++;
    space=SPACE(c) ;
    if( (state==TOK_BEGIN || state==TOK_ENDTOK) && !space && c != '=') state=TOK_TOKEN;
    else if( state==TOK_TOKEN && space && !quote && !escape) state=TOK_ENDTOK;
    else if( (state==TOK_TOKEN || state==TOK_ENDTOK) && c=='=') state=TOK_SEP;
    else if( state==TOK_SEP && !space) state=TOK_VALUE;
    else if( state==TOK_VALUE && space && !quote && !escape ) state=TOK_END;
    STR_ALLOC(&token, token_pos, &sizetok);
    if(c=='"') {
      if(!escape) quote=!quote;
    }
    if(state==TOK_TOKEN) {
      if(c=='"') {
        if((with_quotes & 1) || escape)  token[token_pos++]=(char)c;
      }
      else if( !(c == '\\' && (with_quotes & 2)) ) token[token_pos++]=(char)c;
      else if(escape && c == '\\') token[token_pos++]=(char)c;
    } else if(state==TOK_VALUE) {
      /* do nothing */
    } else if(state==TOK_ENDTOK || state==TOK_SEP) {
        if(token_pos) {
          token[token_pos++]= ' ';
        }
    } else if(state==TOK_END) {
      state=TOK_BEGIN;
    }
    escape = (c=='\\' && !escape);
    if(c=='\0') {
      if(token_pos) {
        token[token_pos-1]= (c != '\0' ) ? ' ' : '\0';
      }
      return token;
    }
  }
}

int get_sym_pin_number(int sym, const char *pin_name)
{ 
  int n = -1;
  if(isonlydigit(pin_name)) {
    n = atoi(pin_name);
  } 
  else if(pin_name[0]) {
    for(n = 0 ; n < xctx->sym[sym].rects[PINLAYER]; ++n) {
      char *prop = xctx->sym[sym].rect[PINLAYER][n].prop_ptr;
      if(!strcmp(get_tok_value(prop,"name",0), pin_name)) break;
    }  
  }       
  return n;
}   

int get_inst_pin_number(int inst, const char *pin_name)
{
  int n = -1;
  if(isonlydigit(pin_name)) {
    n = atoi(pin_name);
  } 
  else if(pin_name[0] && xctx->inst[inst].ptr >= 0) {
    for(n = 0 ; n < (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER]; ++n) {
      char *prop = (xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][n].prop_ptr;
      if(!strcmp(get_tok_value(prop,"name",0), pin_name)) break;
    } 
  }     
  return n;
}

/* given @#ADD[3:0]:net_name return ADD[3:0] in pin_num_or_name and net_name in pin_attr */
static void get_pin_and_attr(const char *token, char **pin_num_or_name, char **pin_attr)
{
  const char *p = token + 2;
  int bracket = 0, done = 0;
  const char *colon = strchr(token + 2, ':');

  while(colon && *p) {
    if(*p == '[') bracket = 1;
    if(*p == ':' && !bracket) {
      /*   01234567890123456
       *   @#A[3:0]:net_name */
      *pin_num_or_name = my_malloc(_ALLOC_ID_, p - token - 1);
      memcpy(*pin_num_or_name, token + 2, p - token - 2);
      (*pin_num_or_name)[p - token - 2] = '\0';
      my_strdup2(_ALLOC_ID_, pin_attr, p + 1);
      done = 1;
      break;
    }
    if(*p == ']') bracket = 0;
    p++;
  }
  if(!done) {
    my_strdup2(_ALLOC_ID_, pin_num_or_name, token + 2);
    my_strdup2(_ALLOC_ID_, pin_attr, "");
  }
  dbg(1, "get_pin_and_attr(): token=%s, name=%s, attr=%s\n", token,
      *pin_num_or_name ? *pin_num_or_name : "NULL", 
      *pin_attr ? *pin_attr: "NULL");
}

/* state machine that parses a string made up of <token>=<value> ... */
/* couples and returns the value of the given token  */
/* if s==NULL or no match return empty string */
/* NULL tok is used  with NULL s to free internal storage (destructor) */
/* never returns NULL... */
/* with_quotes: */
/* bit 0: */
/* 0: eat unescaped backslashes and unescaped double quotes (") */
/* 1: return backslashes and quotes as part of the token value if they are present */
/* bit 1: */
/* 1: do not perform tcl_hook2 substitution */
/* bit: 2 = 1: same as bit 0 = 1, but remove surrounding "..." quotes, keep everything in between */


const char *get_tok_value(const char *s,const char *tok, int with_quotes)
{
  static char *result=NULL;
  static char *token=NULL;
  static int size=0;
  static int  sizetok=0;
  register int c, space;
  register int token_pos=0, value_pos=0;
  int quote=0, state=TOK_BEGIN;
  int escape=0;
  int cmp = 1;
  static char *translated_tok = NULL;

  xctx->tok_size = 0;
  
  if(s==NULL) {
    if(tok == NULL) {
      my_free(_ALLOC_ID_, &result);
      my_free(_ALLOC_ID_, &token);
      my_free(_ALLOC_ID_, &translated_tok);
      size = sizetok = 0;
      dbg(2, "get_tok_value(): clear static data\n");
    }
    return "";
  }
  if(!tok || !strstr(s, tok)) return "";
  /* dbg(0, "get_tok_value(): looking for <%s> in <%.30s>\n",tok,s); */
  if( size == 0 ) {
    sizetok = size = CADCHUNKALLOC;
    my_realloc(_ALLOC_ID_, &result, size);
    my_realloc(_ALLOC_ID_, &token, sizetok);
  }
  while(1) {
    c=*s++;
    space=SPACE(c) ;
    if( (state==TOK_BEGIN || state==TOK_ENDTOK) && !space && c != '=') state=TOK_TOKEN;
    else if( state==TOK_TOKEN && space && !quote && !escape) state=TOK_ENDTOK;
    else if( (state==TOK_TOKEN || state==TOK_ENDTOK) && c=='=') state=TOK_SEP;
    else if( state==TOK_SEP && !space) state=TOK_VALUE;
    else if( state==TOK_VALUE && space && !quote && !escape ) state=TOK_END;
    /* don't use STR_ALLOC() for efficiency reasons */
    if(value_pos>=size) {
      size+=CADCHUNKALLOC;
      my_realloc(_ALLOC_ID_, &result,size);
    }
    if(token_pos>=sizetok) {
      sizetok+=CADCHUNKALLOC;
      my_realloc(_ALLOC_ID_, &token,sizetok);
    }
    if(c=='"') {
      if(!escape) quote=!quote;
    }
    if(state==TOK_TOKEN) {
      if(!cmp) { /* previous token matched search and was without value, return xctx->tok_size */
        result[0] = '\0';
        return result;
      }
      /* if( (with_quotes & 1) || escape || (c != '\\' && c != '"')) token[token_pos++]=(char)c; */
      token[token_pos++]=(char)c;

    } else if(state == TOK_VALUE) {
      if( with_quotes & 1) result[value_pos++] = (char)c;
      else if(( with_quotes & 4) && (escape || c != '"')) result[value_pos++] = (char)c;
      else if( escape || (c != '\\' && c != '"')) result[value_pos++]=(char)c;
    } else if(state == TOK_ENDTOK || state == TOK_SEP) {
        if(token_pos) {
          token[token_pos] = '\0';
          if( !(cmp = strcmp(token,tok)) ) {
            /* report back also token size, useful to check if requested token exists */
            xctx->tok_size = token_pos;
          }
          /* dbg(2, "get_tok_value(): token=%s\n", token);*/
          token_pos=0;
        }
    } else if(state==TOK_END) {
      result[value_pos]='\0';
      if( !cmp ) {
        if(with_quotes & 2) {
          return result;
        } else {
          my_strdup2(_ALLOC_ID_, &translated_tok, tcl_hook2(result));
          return translated_tok;
        }
      }
      value_pos=0;
      state=TOK_BEGIN;
    }
    escape = (c=='\\' && !escape);
    if(c=='\0') {
      result[0]='\0';
      xctx->tok_size = 0;
      return result;
    }
  }
}

/* return template string excluding name=... and token=value where token listed in extra */
/* drop spiceprefix attribute */
const char *get_sym_template(char *s,char *extra)
{
 static char *result=NULL;
 size_t sizeres=0;
 size_t sizetok=0;
 size_t sizeval=0;
 char *value=NULL;
 char *token=NULL;
 register int c, state=TOK_BEGIN, space;
 register size_t token_pos=0, value_pos=0, result_pos=0;
 int quote=0;
 int escape=0;
 int with_quotes=0;
 size_t l;
/* with_quotes: */
/* 0: eat non escaped quotes (") */
/* 1: return unescaped quotes as part of the token value if they are present */
/* 2: eat backslashes */
/* 3: 1+2  :) */

 dbg(1, "get_sym_template(): s=%s, extra=%s\n", s ? s : "NULL", extra ? extra : "NULL");
 if(s==NULL) {
   my_free(_ALLOC_ID_, &result);
   return "";
 }
 l = strlen(s);
 STR_ALLOC(&result, l+1, &sizeres);
 result[0] = '\0';
 sizetok = sizeval = CADCHUNKALLOC;
 my_realloc(_ALLOC_ID_, &value,sizeval);
 my_realloc(_ALLOC_ID_, &token,sizetok);
 while(1) {
  c=*s++;
  space=SPACE(c) ;
  dbg(1, "state=%d", state);
  if( (state==TOK_BEGIN || state==TOK_ENDTOK) && !space && c != '=') state=TOK_TOKEN;
  else if( state==TOK_TOKEN && space) state=TOK_ENDTOK;
  else if( (state==TOK_TOKEN || state==TOK_ENDTOK) && c=='=') state=TOK_SEP;
  else if( state==TOK_SEP && !space) state=TOK_VALUE;
  else if( state==TOK_VALUE && space && !quote) state=TOK_END;
  dbg(1, " --> state=%d\n", state);
  STR_ALLOC(&value, value_pos, &sizeval);
  STR_ALLOC(&token, token_pos, &sizetok);
  if(c=='"') {
    if(!escape) quote=!quote;
  }
  if(state==TOK_BEGIN) {
    result[result_pos++] = (char)c;
  } else if(state==TOK_TOKEN) {
    /* token[token_pos++]=(char)c; */
    if( (with_quotes & 1) || escape || (c != '\\' && c != '"')) token[token_pos++]=(char)c;
  } else if(state==TOK_VALUE) {
    /* if((with_quotes & 1) || escape)  value[value_pos++]=(char)c; */
    if( (with_quotes & 1) || escape || (c != '\\' && c != '"')) value[value_pos++]=(char)c;
  } else if(state==TOK_END) {
    value[value_pos]='\0';
    if((!extra || !strstr(extra, token)) && strcmp(token,"name") && strcmp(token,"spiceprefix")) {
      memcpy(result+result_pos, value, value_pos+1);
      result_pos+=value_pos;
    }
    result[result_pos++] = (char)c;
    value_pos=0;
    token_pos=0;
    state=TOK_BEGIN;
  } else if(state==TOK_ENDTOK || state==TOK_SEP) {
    if(token_pos) {
      token[token_pos]='\0';
      dbg(1, "token=|%s|\n", token);
      if((!extra || !strstr(extra, token)) && strcmp(token,"name") && strcmp(token,"spiceprefix")) {
        memcpy(result+result_pos, token, token_pos+1);
        result_pos+=token_pos;
        result[result_pos++] = (char)c;
      }
      token_pos=0;
    }
  }
  escape = (c=='\\' && !escape);
  if(c=='\0') {
    if(result[result_pos] != '\0') result[result_pos++] = '\0';
    break;
  }
 }
 my_free(_ALLOC_ID_, &value);
 my_free(_ALLOC_ID_, &token);
 dbg(1, "get_sym_template(): result=|%s|\n", result);
 return result;
}

/* caller is responsible for freeing up storage for return value
 * return NULL if no matching token found */
static char *get_pin_attr_from_inst(int inst, int pin, const char *attr)
{
   size_t attr_size;
   char *pinname = NULL, *pname = NULL, *pin_attr_value = NULL;
   char *pnumber = NULL;
   const char *str;


   dbg(1, "get_pin_attr_from_inst(): inst=%d pin=%d attr=%s\n", inst, pin, attr);
   if(xctx->inst[inst].ptr < 0 ) return NULL;
   pin_attr_value = NULL;
   str = get_tok_value((xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][pin].prop_ptr,"name",0);
   if(str[0]) {
     size_t tok_val_len;
     tok_val_len = strlen(str);
     attr_size = strlen(attr);
     my_strdup(_ALLOC_ID_, &pinname, str);
     pname =my_malloc(_ALLOC_ID_, tok_val_len + attr_size + 30);
     my_snprintf(pname, tok_val_len + attr_size + 30, "%s(%s)", attr, pinname);
     my_free(_ALLOC_ID_, &pinname);
     str = get_tok_value(xctx->inst[inst].prop_ptr, pname, 0);
     my_free(_ALLOC_ID_, &pname);
     if(xctx->tok_size) my_strdup2(_ALLOC_ID_, &pin_attr_value, str);
     else {
       pnumber = my_malloc(_ALLOC_ID_, attr_size + 100);
       my_snprintf(pnumber, attr_size + 100, "%s(%d)", attr, pin);
       str = get_tok_value(xctx->inst[inst].prop_ptr, pnumber, 0);
       dbg(1, "get_pin_attr_from_inst(): pnumber=%s\n", pnumber);
       my_free(_ALLOC_ID_, &pnumber);
       if(xctx->tok_size) my_strdup2(_ALLOC_ID_, &pin_attr_value, str);
     }
   }
   return pin_attr_value; /* caller is responsible for freeing up storage for pin_attr_value */
}

int get_last_used_index(const char *old_basename, const char *brkt)
{
  int retval = 1;
  Int_hashentry *entry;
  size_t size = strlen(old_basename) + strlen(brkt)+40;
  char *refname = my_malloc(_ALLOC_ID_, size);
  my_snprintf(refname, size, "_@%s@%s", old_basename, brkt);
  entry = int_hash_lookup(&xctx->inst_name_table, refname, 0, XLOOKUP);
  if(entry) retval = entry->value;
  my_free(_ALLOC_ID_, &refname);
  return retval;
}

/* if inst == -1 hash all instance names, else do only given instance
 * action can be XINSERT or XDELETE to insert or remove items */
void hash_names(int inst, int action)
{
  int i, xmult, start, stop;
  char *upinst = NULL;
  char *upinst_ptr, *upinst_state, *single_name;
  dbg(1, "hash_names(): inst=%d, action=%d\n", inst, action);
  if(inst == -1) {
    int_hash_free(&xctx->inst_name_table);
    int_hash_init(&xctx->inst_name_table, HASHSIZE);
  }
  if(inst == -1) {
     start = 0;
     stop =  xctx->instances;
  } else {
    start = inst;
    stop = inst + 1;
  }
  if(inst != -1) dbg(1, "hash_names(): start=%d, stop=%d, instname=%s\n",
        start, stop, xctx->inst[inst].instname? xctx->inst[inst].instname : "NULL");
  for(i = start; i < stop; ++i) {
    if(xctx->inst[i].instname && xctx->inst[i].instname[0]) {
      my_strdup(_ALLOC_ID_, &upinst, expandlabel(xctx->inst[i].instname, &xmult));
      strtoupper(upinst);

      upinst_ptr = upinst;
      while( (single_name = my_strtok_r(upinst_ptr, ",", "", 0, &upinst_state)) ) {
        upinst_ptr = NULL;
        dbg(1, "hash_names(): inst %d, name %s --> %d\n", i, single_name, action);
        int_hash_lookup(&xctx->inst_name_table, single_name, i, action);
        dbg(1, "hash_names(): hashing %s from %s\n", single_name, xctx->inst[i].instname);
      }
    }
  }
  if(upinst) my_free(_ALLOC_ID_, &upinst);
}

/* return -1 if name is not used, else return first instance number with same name found
 * old_basename: base name (without [...]) of instance name the new 'name' was built from
 * brkt: pointer to '[...]' part of instance name (or empty string if no [...] found)
 * q: integer number added to 'name' when trying an unused instance name 
 *    (name = old_basename + q + bracket)
 *    or -1 if only testing for unique 'name'.
 */

static int name_is_used(char *name, const char *old_basename, const char *brkt, int q)
{
  int xmult, used = -1;
  char *upinst = NULL;
  char *upinst_ptr, *upinst_state, *single_name;
  Int_hashentry *entry;
  my_strdup(_ALLOC_ID_, &upinst, expandlabel(name, &xmult));
  strtoupper(upinst);
  upinst_ptr = upinst;
  while( (single_name = my_strtok_r(upinst_ptr, ",", "", 0, &upinst_state)) ) {
    upinst_ptr = NULL;
    entry = int_hash_lookup(&xctx->inst_name_table, single_name, 1, XLOOKUP);
    if(entry) {
      used = entry->value;
      break;
    }
  }
  my_free(_ALLOC_ID_, &upinst);
  dbg(1, "name_is_used(%s): return inst %d\n", name, used);

  if(q != -1 && used == -1) {
    size_t size = strlen(old_basename) + strlen(brkt)+40;
    char *refname = my_malloc(_ALLOC_ID_, size);
    my_snprintf(refname, size, "_@%s@%s", old_basename, brkt);
    int_hash_lookup(&xctx->inst_name_table, refname, q, XINSERT);
    my_free(_ALLOC_ID_, &refname);
  }
  return used;
}

/* given a old_prop property string, return a new
 * property string in xctx->inst[i].prop_ptr such that the element name is
 * unique in current design (that is, element name is changed
 * if necessary)
 * if old_prop=NULL return NULL
 * if old_prop does not contain a valid "name" or empty return old_prop
 * hash_names(-1, XINSERT) must be called before using this function */
void new_prop_string(int i, const char *old_prop, int dis_uniq_names)
{
  char *old_name=NULL, *new_name=NULL;
  const char *brkt;
  const char *new_prop;
  size_t old_name_len;
  int n, q, qq;
  char *old_name_base = NULL;
  char *up_new_name = NULL;
  int is_used;
 
  dbg(1, "new_prop_string(): i=%d, old_prop=%s\n", i, old_prop);
  if(old_prop==NULL) {
   my_free(_ALLOC_ID_, &xctx->inst[i].prop_ptr);
   return;
  }
  old_name_len = my_strdup(_ALLOC_ID_, &old_name,get_tok_value(old_prop,"name",0) ); /* added old_name_len */
 
  if(old_name==NULL) {
   my_strdup(_ALLOC_ID_, &xctx->inst[i].prop_ptr, old_prop);  /* changed to copy old props if no name */
   my_strdup2(_ALLOC_ID_, &xctx->inst[i].instname, "");
   return;
  }
  /* don't change old_prop if name does not conflict. */
  /* if no hash_names() is done and inst_table uninitialized --> use old_prop */
  is_used =  name_is_used(old_name, "", "", -1);
  if(dis_uniq_names || is_used == -1 || is_used == i) {
   my_strdup(_ALLOC_ID_, &xctx->inst[i].prop_ptr, old_prop);
   my_strdup2(_ALLOC_ID_, &xctx->inst[i].instname, old_name);
   my_free(_ALLOC_ID_, &old_name);
   return;
  }
  /* old_name is not unique. Find another unique name */
  old_name_base = my_malloc(_ALLOC_ID_, old_name_len+1);
  n = sscanf(old_name, "%[^[0-9]",old_name_base);
  if(!n) old_name_base[0] = '\0'; /* there is no basename (like in "[3:0]" or "12"), set to empty string */
  brkt=find_bracket(old_name); /* if no bracket found will point to end of string ('\0') */
  my_realloc(_ALLOC_ID_, &new_name, old_name_len + 40);
 

  qq = get_last_used_index(old_name_base, brkt); /*  */
  for(q = qq;; ++q) {
    my_snprintf(new_name, old_name_len + 40, "%s%d%s", old_name_base, q, brkt);
    is_used = name_is_used(new_name, old_name_base, brkt, q);
    if(is_used == -1 ) break; 
  }
  my_free(_ALLOC_ID_, &old_name_base);
  dbg(1, "new_prop_string(): new_name=%s\n", new_name);
  new_prop = subst_token(old_prop, "name", new_name);
  dbg(1, "new_prop_string(): old_prop=|%s|\n", old_prop);
  dbg(1, "new_prop_string(): new_prop=|%s|\n", new_prop);
  if(strcmp(new_prop, old_prop) ) {
    my_strdup(_ALLOC_ID_, &xctx->inst[i].prop_ptr, new_prop);
    my_strdup2(_ALLOC_ID_, &xctx->inst[i].instname, new_name);
  }
  my_free(_ALLOC_ID_, &old_name);
  my_free(_ALLOC_ID_, &new_name);
  my_free(_ALLOC_ID_, &up_new_name);
}

void check_unique_names(int rename)
{
  int i, first = 1, modified = 0;
  int newpropcnt = 0;
  char *tmp = NULL;
  int used;

  if(xctx->hilight_nets) {
    xctx->enable_drill=0;
    clear_all_hilights();
    draw();
  }
  int_hash_free(&xctx->inst_name_table);
  int_hash_init(&xctx->inst_name_table, HASHSIZE);

  /* look for duplicates */
  first = 1;
  for(i=0;i<xctx->instances; ++i) {
    if(xctx->inst[i].instname && xctx->inst[i].instname[0]) {
      if(xctx->inst[i].ptr == -1) continue;
      if(!(xctx->inst[i].ptr+ xctx->sym)->type) continue;
      used = name_is_used(xctx->inst[i].instname,"", "", -1);
      hash_names(i, XINSERT_NOREPLACE);
      if( used != -1 && used != i) {
        dbg(0, "check_unique_names(): found duplicate: i=%d name=%s\n", i, xctx->inst[i].instname);
        xctx->inst[i].color = -PINLAYER;
        inst_hilight_hash_lookup(i, -PINLAYER, XINSERT_NOREPLACE);
        if(rename == 1) {
          if(first) {
            bbox(START,0.0,0.0,0.0,0.0);
            modified = 1;
            xctx->push_undo();
            xctx->prep_hash_inst=0;
            xctx->prep_net_structs=0;
            xctx->prep_hi_structs=0;
            first = 0;
          }
          bbox(ADD, xctx->inst[i].x1, xctx->inst[i].y1, xctx->inst[i].x2, xctx->inst[i].y2);
        }
      }
    }
  } /* for(i...) */

  /* rename duplicates */
  if(rename) for(i=0;i<xctx->instances; ++i) {
    if( (xctx->inst[i].color != -10000)) {
      my_strdup(_ALLOC_ID_, &tmp, xctx->inst[i].prop_ptr);
      newpropcnt++;
      new_prop_string(i, tmp, 0);
      hash_names(i, XINSERT);
      symbol_bbox(i, &xctx->inst[i].x1, &xctx->inst[i].y1, &xctx->inst[i].x2, &xctx->inst[i].y2);
      bbox(ADD, xctx->inst[i].x1, xctx->inst[i].y1, xctx->inst[i].x2, xctx->inst[i].y2);
      my_free(_ALLOC_ID_, &tmp);
    }
  } /* for(i...) */
  if(modified) set_modify(1);
  if(rename == 1 && xctx->hilight_nets) {
    bbox(SET,0.0,0.0,0.0,0.0);
    draw();
    bbox(END,0.0,0.0,0.0,0.0);
  }
  redraw_hilights(0);
  int_hash_free(&xctx->inst_name_table);
}

static int is_quoted(const char *s)
{
  int c, escape = 0;
  int openquote = 0;
  int closequote = 0;

  while( (c = *s++) ) {

    if(c == '\\' && !escape) {
      escape = 1;
      c = *s++;
    } else escape = 0;

    if(c == '"' && !escape && !openquote ) {
      openquote = 1;
      continue;
    }
    else if(c == '"' && !escape && openquote && !closequote) {
      closequote = 1;
      continue;
    }
    else if(c == '"' && !escape ) {
      return 0;
    }

    if(!isspace(c) && !openquote) return 0;
    if(!isspace(c) && openquote && closequote) return 0;
  }
  if(openquote && closequote) return 1;
  return 0;
}

static void print_vhdl_primitive(FILE *fd, int inst) /* netlist  primitives, 20071217 */
{
 int i=0, multip, tmp;
 const char *str_ptr;
 register int c, state=TOK_BEGIN, space;
 const char *lab;
 char *template=NULL,*format=NULL,*s, *name=NULL, *token=NULL;
 const char *value;
 size_t sizetok=0;
 size_t token_pos=0;
 int escape=0;
 int no_of_pins=0;
 char *fmt_attr = NULL;
 char *result = NULL;

 my_strdup(_ALLOC_ID_, &template, (xctx->inst[inst].ptr + xctx->sym)->templ);
 my_strdup(_ALLOC_ID_, &name, xctx->inst[inst].instname);
 fmt_attr = xctx->format ? xctx->format : "vhdl_format";
 if(!name) my_strdup(_ALLOC_ID_, &name, get_tok_value(template, "name", 0));
 /* allow format string override in instance */
 my_strdup(_ALLOC_ID_, &format, get_tok_value(xctx->inst[inst].prop_ptr, fmt_attr, 2));
 /* get netlist format rule from symbol */
 if(!xctx->tok_size)
   my_strdup(_ALLOC_ID_, &format, get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr, fmt_attr, 2));
 /* allow format string override in instance */
 if(xctx->tok_size && strcmp(fmt_attr, "vhdl_format"))               
    my_strdup(_ALLOC_ID_, &format, get_tok_value(xctx->inst[inst].prop_ptr, "vhdl_format", 2));
 /* get netlist format rule from symbol */
 if(!xctx->tok_size && strcmp(fmt_attr, "vhdl_format"))               
   my_strdup(_ALLOC_ID_, &format, get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr, "vhdl_format", 2));
 if((name==NULL) || (format==NULL) ) {
   my_free(_ALLOC_ID_, &template);
   my_free(_ALLOC_ID_, &name);
   my_free(_ALLOC_ID_, &format);
   return; /*do no netlist unwanted insts(no format) */
 }
 no_of_pins= (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER];
 s=format;
 dbg(1, "print_vhdl_primitive(): name=%s, format=%s xctx->netlist_count=%d\n",name,format, xctx->netlist_count);

 fprintf(fd, "---- start primitive ");
 lab=expandlabel(name, &tmp);
 fprintf(fd, "%d\n",tmp);
 /* begin parsing format string */
 while(1)
 {
  c=*s++;
  if(c=='\\') {
    escape=1;
    c=*s++;
  }
  else escape=0;
  if(c=='\n' && escape ) c=*s++; /* 20171030 eat escaped newlines */
  space=SPACE(c);

  if( state==TOK_BEGIN && (c=='@' || c=='%') && !escape ) state=TOK_TOKEN;
  else if(state==TOK_TOKEN && token_pos > 1 &&
     (
       ( (space  || c == '%' || c == '@') && !escape ) ||
       ( (!space && c != '%' && c != '@') && escape  )
     )
    ) {
    state=TOK_SEP;
  }

  STR_ALLOC(&token, token_pos, &sizetok);
  if(state==TOK_TOKEN) {
    token[token_pos++]=(char)c; /* 20171029 remove escaping backslashes */
  }
  else if(state==TOK_SEP)                    /* got a token */
  {
   token[token_pos]='\0';
   token_pos=0;

   value = get_tok_value(xctx->inst[inst].prop_ptr, token+1, 0);
   /* xctx->tok_size==0 indicates that token(+1) does not exist in instance attributes */
   if(!xctx->tok_size)
   value=get_tok_value(template, token+1, 0);
   if(!xctx->tok_size && token[0] =='%') {
     my_mstrcat(_ALLOC_ID_, &result, token + 1, NULL);
   } else if(value && value[0]!='\0')
   {  /* instance names (name) and node labels (lab) go thru the expandlabel function. */
      /*if something else must be parsed, put an if here! */

    if(!(strcmp(token+1,"name"))) {
      if( (lab=expandlabel(value, &tmp)) != NULL)
         my_mstrcat(_ALLOC_ID_, &result, "----name(", lab, ")", NULL);
      else
         my_mstrcat(_ALLOC_ID_, &result, value, NULL);
    }
    else if(!(strcmp(token+1,"lab"))) {
      if( (lab=expandlabel(value, &tmp)) != NULL)
         my_mstrcat(_ALLOC_ID_, &result, "----pin(", lab, ")", NULL);
      else
         my_mstrcat(_ALLOC_ID_, &result, value, NULL);
    }
    else my_mstrcat(_ALLOC_ID_, &result, value, NULL);
   }
   else if(strcmp(token,"@symref")==0)
   {
     const char *s = get_sym_name(inst, 9999, 1);
     my_mstrcat(_ALLOC_ID_, &result, s, NULL);
   }
   else if(strcmp(token,"@symname")==0) /* of course symname must not be present  */
                                        /* in hash table */
   {
     const char *s = sanitize(translate(inst, get_sym_name(inst, 0, 0)));
     my_mstrcat(_ALLOC_ID_, &result, s, NULL);
   }
   else if (strcmp(token,"@symname_ext")==0) 
   {
     const char *s = sanitize(translate(inst, get_sym_name(inst, 0, 1)));
     my_mstrcat(_ALLOC_ID_, &result, s, NULL);
   }
   else if(strcmp(token,"@schname_ext")==0) /* of course schname must not be present  */
                                        /* in hash table */
   {
     my_mstrcat(_ALLOC_ID_, &result, xctx->current_name, NULL);
   }
   else if(strcmp(token,"@schname")==0)
   {
     my_mstrcat(_ALLOC_ID_, &result, get_cell(xctx->current_name, 0), NULL);
   }
   else if(strcmp(token,"@topschname")==0) /* of course topschname must not be present in attributes */
   {
     const char *topsch;
     topsch = get_trailing_path(xctx->sch[0], 0, 1);
     my_mstrcat(_ALLOC_ID_, &result, topsch, NULL);
   }
   else if(strcmp(token,"@pinlist")==0) /* of course pinlist must not be present  */
                                        /* in hash table. print multiplicity */
   {                                    /* and node number: m1 n1 m2 n2 .... */
    Int_hashtable table = {NULL, 0};
    int first = 1;
    int_hash_init(&table, 37);
    for(i=0;i<no_of_pins; ++i)
    {
      char *prop = (xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][i].prop_ptr;
      if(strboolcmp(get_tok_value(prop,"vhdl_ignore",0), "true")) {
        const char *name = get_tok_value(prop,"name",0);
        if(!int_hash_lookup(&table, name, 1, XINSERT_NOREPLACE)) {
          if(!first) my_mstrcat(_ALLOC_ID_, &result, " , ", NULL);
          str_ptr =  net_name(inst,i, &multip, 0, 1);
          my_mstrcat(_ALLOC_ID_, &result, "----pin(", str_ptr, ") ", NULL);
          first = 0;
        }
      }
    }
    int_hash_free(&table);
   }
   else if(token[0]=='@' && token[1]=='@') {    /* recognize single pins 15112003 */
    for(i=0;i<no_of_pins; ++i) {
     xSymbol *ptr = xctx->inst[inst].ptr + xctx->sym;
     if(!strcmp( get_tok_value(ptr->rect[PINLAYER][i].prop_ptr,"name",0), token+2)) {
       if(strboolcmp(get_tok_value(ptr->rect[PINLAYER][i].prop_ptr,"vhdl_ignore",0), "true")) {
         str_ptr =  net_name(inst,i, &multip, 0, 1);
         my_mstrcat(_ALLOC_ID_, &result, "----pin(", str_ptr, ") ", NULL);
       }
       break;
     }
    }
   }

   /* reference by pin number instead of pin name, allows faster lookup of the attached net name 
    * @#0, @#1:net_name, @#2:name, ... */
   else if(token[0]=='@' && token[1]=='#') {
     int n;
     char *pin_attr = NULL;
     char *pin_num_or_name = NULL;

     get_pin_and_attr(token, &pin_num_or_name, &pin_attr);
     n = get_inst_pin_number(inst, pin_num_or_name);
     if(n>=0  && pin_attr[0] && n < (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER]) {
       char *pin_attr_value = NULL;
       int is_net_name = !strcmp(pin_attr, "net_name");
       /* get pin_attr value from instance: "pinnumber(ENABLE)=5" --> return 5, attr "pinnumber" of pin "ENABLE"
        *                                   "pinnumber(3)=6       --> return 6, attr "pinnumber" of 4th pin */
       if(!is_net_name) {
         pin_attr_value = get_pin_attr_from_inst(inst, n, pin_attr);
         /* get pin_attr from instance pin attribute string */
         if(!pin_attr_value) {
          my_strdup(_ALLOC_ID_, &pin_attr_value,
             get_tok_value((xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][n].prop_ptr, pin_attr, 0));
         }
       }
       /* @#n:net_name attribute (n = pin number or name) will translate to net name attached  to pin */
       if(!pin_attr_value && is_net_name) {
         prepare_netlist_structs(0);
         my_strdup(_ALLOC_ID_, &pin_attr_value,
              xctx->inst[inst].node && xctx->inst[inst].node[n] ? xctx->inst[inst].node[n] : "?");
       }
       if(!pin_attr_value ) my_strdup(_ALLOC_ID_, &pin_attr_value, "--UNDEF--");
       value = pin_attr_value;
       /* recognize slotted devices: instname = "U3:3", value = "a:b:c:d" --> value = "c" */
       if(value[0] && !strcmp(pin_attr, "pinnumber") ) {
         char *ss;
         int slot;
         char *tmpstr = NULL;
         tmpstr = my_malloc(_ALLOC_ID_, sizeof(xctx->inst[inst].instname));
         if( (ss=strchr(xctx->inst[inst].instname, ':')) ) {
           sscanf(ss+1, "%s", tmpstr);
           if(isonlydigit(tmpstr)) {
             slot = atoi(tmpstr);
             if(strstr(value,":")) value = find_nth(value, ":", "", 0, slot);
           }
         }
         my_free(_ALLOC_ID_, &tmpstr);
       }
       my_mstrcat(_ALLOC_ID_, &result, value, NULL);
       my_free(_ALLOC_ID_, &pin_attr_value);
     }
     else if(n>=0  && n < (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER]) {
       const char *si;
       char *prop = (xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][n].prop_ptr;
       si  = get_tok_value(prop, "verilog_ignore",0);
       if(strboolcmp(si, "true")) {
         str_ptr =  net_name(inst,n, &multip, 0, 1);
         my_mstrcat(_ALLOC_ID_, &result, "----pin(", str_ptr, ") ", NULL);
       }
     }
     my_free(_ALLOC_ID_, &pin_attr);
     my_free(_ALLOC_ID_, &pin_num_or_name);
   }

   else if(!strncmp(token,"@tcleval", 8)) {
     /* char tclcmd[strlen(token)+100] ; */
     size_t s;
     char *tclcmd=NULL;
     s = token_pos + strlen(name) + strlen(xctx->inst[inst].name) + 100;
     tclcmd = my_malloc(_ALLOC_ID_, s);
     Tcl_ResetResult(interp);
     my_snprintf(tclcmd, s, "tclpropeval {%s} {%s} {%s}", token, name, xctx->inst[inst].name);
     tcleval(tclcmd);
     my_mstrcat(_ALLOC_ID_, &result, tclresult(), NULL);
     my_free(_ALLOC_ID_, &tclcmd);
   }

   if(c!='%' && c!='@' && c!='\0' ) {
     char str[2];
     str[0] = (unsigned char) c;
     str[1] = (unsigned char)'\0';
     my_mstrcat(_ALLOC_ID_, &result, str, NULL);
   }
   if(c == '@' || c == '%') s--;
   state=TOK_BEGIN;
  }
  else if(state==TOK_BEGIN && c!='\0') {
    char str[2];
    str[0] = (unsigned char) c;
    str[1] = (unsigned char)'\0';
    my_mstrcat(_ALLOC_ID_, &result, str, NULL);
  }

  if(c=='\0')
  {
   if(result && strstr(result, "tcleval(")== result) {
     dbg(1, "print_vhdl_primitive(): before translate() result=%s\n", result);
     my_strdup(_ALLOC_ID_, &result, translate(inst, result));
     dbg(1, "print_vhdl_primitive(): after  translate() result=%s\n", result);
   }
   if(result) fprintf(fd, "%s", result);
   fputc('\n',fd);
   fprintf(fd, "---- end primitive\n");
   break ;
  }
 }
 my_free(_ALLOC_ID_, &result);
 my_free(_ALLOC_ID_, &template);
 my_free(_ALLOC_ID_, &format);
 my_free(_ALLOC_ID_, &name);
 my_free(_ALLOC_ID_, &token);
}

const char *subst_token(const char *s, const char *tok, const char *new_val)
/* given a string <s> with multiple "token=value ..." assignments */
/* substitute <tok>'s value with <new_val> */
/* if tok not found in s and new_val!=NULL add tok=new_val at end.*/
/* if new_val is empty ('\0') set token value to "" (token="") */
/* if new_val is NULL *remove* 'token (and =val if any)' from s */
/* return the updated string */
{
  static char *result=NULL;
  size_t size=0;
  register int c, state=TOK_BEGIN, space;
  size_t sizetok=0;
  char *token=NULL;
  size_t token_pos=0, result_pos=0, result_save_pos = 0, tmp;
  int quote=0;
  int done_subst=0;
  int escape=0, matched_tok=0;
  char *new_val_copy = NULL;
  size_t new_val_len;

  if(s==NULL && tok == NULL){
    my_free(_ALLOC_ID_, &result);
    return "";
  }
  if((tok == NULL || tok[0]=='\0') && s ){
    my_strdup2(_ALLOC_ID_, &result, s);
    return result;
  }
  /* quote new_val if it contains newlines and not "name" token */
  if(new_val) {
    new_val_len = strlen(new_val);
    if(strcmp(tok, "name") && !is_quoted(new_val) && strpbrk(new_val, ";\n \t")) {
      new_val_copy = my_malloc(_ALLOC_ID_, new_val_len+3);
      my_snprintf(new_val_copy, new_val_len+3, "\"%s\"", new_val);
    }
    else my_strdup(_ALLOC_ID_, &new_val_copy, new_val);
  } else new_val_copy = NULL;
  dbg(1, "subst_token(): %s, %s, %s\n", s ? s : "<NULL>", tok ? tok : "<NULL>", new_val ? new_val : "<NULL>");
  sizetok = size = CADCHUNKALLOC;
  my_realloc(_ALLOC_ID_, &result, size);
  my_realloc(_ALLOC_ID_, &token, sizetok);
  result[0] = '\0';
  while( s ) {
    c=*s++;
    space=SPACE(c);
    if(c == '"' && !escape) quote=!quote;
    /* alloc data */
    STR_ALLOC(&result, result_pos, &size);
    STR_ALLOC(&token, token_pos, &sizetok);

    /* parsing state machine                                    */
    /* states:                                                  */
    /*    TOK_BEGIN TOK_TOKEN TOK_ENDTOK TOK_SEP TOK_VALUE      */
    /*                                                          */
    /*                                                          */
    /* TOK_BEGIN                                                */
    /* |      TOK_TOKEN                                         */
    /* |      |   TOK_ENDTOK                                    */
    /* |      |   |  TOK_SEP                                    */
    /* |      |   |  | TOK_VALUE                                */
    /* |      |   |  | | TOK_BEGIN                              */
    /* |      |   |  | | |   TOK_TOKEN                          */
    /* |      |   |  | | |   |     TOK_ENDTOK                   */
    /* |      |   |  | | |   |     |  TOK_TOKEN                 */
    /* |      |   |  | | |   |     |  |                         */
    /* .......name...=.x1....format...type..=..subcircuit....   */
    /* . : space                                                */

    if(state == TOK_BEGIN && !space && c != '=' ) {
      result_save_pos = result_pos;
      token_pos = 0;
      state = TOK_TOKEN;
    } else if(state == TOK_ENDTOK  && (!space || c == '\0') && c != '=' ) {
      if(!done_subst && matched_tok) {
        if(new_val_copy) { /* add new_val_copy to matching token with no value */
          if(new_val_copy[0]) {
            tmp = strlen(new_val_copy);
          } else {
            new_val_copy = "\"\"";
            tmp = 2;
          }

          STR_ALLOC(&result, tmp+2 + result_pos, &size);
          memcpy(result + result_pos, "=", 1);
          memcpy(result + result_pos+1, new_val_copy, tmp);
          memcpy(result + result_pos+1+tmp, " ", 1);
          result_pos += tmp + 2;
          done_subst = 1;
        } else { /* remove token (and value if any) */
          result_pos = result_save_pos;
          done_subst = 1;
        }
      }
      result_save_pos = result_pos;
      if(c != '\0') state = TOK_TOKEN; /* if end of string remain in TOK_ENDTOK state */
    } else if( state == TOK_TOKEN && space) {
      token[token_pos] = '\0';
      token_pos = 0;
      matched_tok = !strcmp(token, tok) && !done_subst;
      state=TOK_ENDTOK;
      if(c == '\0') {
        s--; /* go to next iteration and process '\0' as TOK_ENDTOK */
        continue;
      }
    } else if(state == TOK_TOKEN && c=='=') {
      token[token_pos] = '\0';
      token_pos = 0;
      matched_tok = !strcmp(token, tok) && !done_subst;
      state=TOK_SEP;
    } else if(state == TOK_ENDTOK && c=='=') {
      state=TOK_SEP;
    } else if( state == TOK_SEP && !space) {
      if(!done_subst && matched_tok) {
        if(new_val_copy) { /* replace token value with new_val_copy */
          if(new_val_copy[0]) {
            tmp = strlen(new_val_copy);
          } else {
            new_val_copy = "\"\"";
            tmp = 2;
          }
          STR_ALLOC(&result, tmp + result_pos, &size);
          memcpy(result + result_pos ,new_val_copy, tmp + 1);
          result_pos += tmp;
          done_subst = 1;
        } else { /* remove token (and value if any) */
          result_pos = result_save_pos;
          done_subst = 1;
        }
      }
      state=TOK_VALUE;
    } else if( state == TOK_VALUE && space && !quote && !escape) {
      state=TOK_BEGIN;
    }
    /* state actions */
    if(state == TOK_BEGIN) {
      result[result_pos++] = (char)c;
    } else if(state == TOK_TOKEN) {
      token[token_pos++] = (char)c;
      result[result_pos++] = (char)c;
    } else if(state == TOK_ENDTOK) {
      result[result_pos++] = (char)c;
    } else if(state == TOK_SEP) {
      result[result_pos++] = (char)c;
    } else if(state==TOK_VALUE) {
      if(!matched_tok) result[result_pos++] = (char)c; /* skip value for matching token */
    }
    escape = (c=='\\' && !escape);
    if(c == '\0') break;
  }
  if(!done_subst) { /* if tok not found add tok=new_val_copy at end */
    if(result_pos == 0 ) result_pos = 1; /* result="" */
    if(new_val_copy) {
      if(!new_val_copy[0]) new_val_copy = "\"\"";
      tmp = strlen(new_val_copy) + strlen(tok) + 2;
      STR_ALLOC(&result, tmp + result_pos, &size);
      if(result_pos > 1 && (result[result_pos - 2] == ' ' || result[result_pos - 2] == '\n')) {
        /* result_pos guaranteed to be > 0 */
        my_snprintf(result + result_pos - 1, size, "%s=%s", tok, new_val_copy );
      } else {
        /* result_pos guaranteed to be > 0 */
        my_snprintf(result + result_pos - 1, size, "\n%s=%s", tok, new_val_copy );
      }
    }
  }
  dbg(2, "subst_token(): returning: %s\n",result);
  my_free(_ALLOC_ID_, &token);
  my_free(_ALLOC_ID_, &new_val_copy);
  return result;
}

const char *get_trailing_path(const char *str, int no_of_dir, int skip_ext)
{
  static char s[PATH_MAX]; /* safe to keep even with multiple schematic windows */
  size_t len;
  size_t ext_pos, dir_pos;
  int n_ext, n_dir, c, i, generator = 0;

  if(str == NULL) return NULL;  
  my_strncpy(s, str, S(s));
  len = strlen(s);

  for(ext_pos=len, dir_pos=len, n_ext=0, n_dir=0, i=(int)len; i>=0; i--) {
    c = s[i];
    if(c=='.' && ++n_ext == 1) {
      if(!generator) ext_pos = i;
      if(generator) s[i] = '_';
    }
    if(c=='/' && ++n_dir==no_of_dir+1) dir_pos = i;
    if(c=='(') generator = 1;
  }
  if(skip_ext) s[ext_pos] = '\0';

  if(dir_pos==len) return s;
  dbg(2, "get_trailing_path(): str=%s, no_of_dir=%d, skip_ext=%d\n",
                   str, no_of_dir, skip_ext);
  dbg(2, "get_trailing_path(): returning: %s\n", s+(dir_pos<len ? dir_pos+1 : 0));
  return s+(dir_pos<len ? dir_pos+1 : 0);
}

/* no extension */
const char *get_cell(const char *str, int no_of_dir)
{
  return get_trailing_path(str, no_of_dir, 1);
}

/* keep extension */
const char *get_cell_w_ext(const char *str, int no_of_dir)
{
  return get_trailing_path(str, no_of_dir, 0);
}

/* in a string with tokens separated by characters in 'sep'
 * count number of tokens. Multiple separators and leading/trailing
 * separators are allowed. */
int count_items(const char *s, const char *sep, const char *quote)
{
  const char *ptr;
  int items = 0;
  int state = 0; /* 1 if item is being processed */
  int c, q = 0, e = 0;

  ptr = s;
  while( (c = *(unsigned char *)ptr++) ) {
    if(!e && c == '\\') {
      e = 1;
      continue;
    }
    if(!e && strchr(quote, c)) q = !q;
    if(e || q || !strchr(sep, c)) { /* not a separator */
      if(!state) items++;
      state = 1;
    } else {
      state = 0;
    }
    e = 0;
  }
  dbg(1, "count_items: s=%s, items=%d\n", s, items);
  return items;
}

void print_vhdl_element(FILE *fd, int inst)
{
  int i=0, multip, tmp, tmp1;
  const char *str_ptr;
  register int c, state=TOK_BEGIN, space;
  const char *lab;
  char *name=NULL;
  char  *generic_value=NULL, *generic_type=NULL;
  char *template=NULL,*s, *value=NULL,  *token=NULL;
  int no_of_pins=0, no_of_generics=0;
  size_t sizetok=0, sizeval=0;
  size_t token_pos=0, value_pos=0;
  int quote=0;
  int escape=0;
  xRect *pinptr;
  const char *fmt_attr = NULL;
  Int_hashtable table = {NULL, 0};
  const char *fmt;

  fmt_attr = xctx->format ? xctx->format : "vhdl_format";

  /* allow format string override in instance */
  fmt = get_tok_value(xctx->inst[inst].prop_ptr, fmt_attr, 2);
  /* get netlist format rule from symbol */
  if(!xctx->tok_size)
    fmt = get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr, fmt_attr, 2);
  /* allow format string override in instance */
  if(!xctx->tok_size && strcmp(fmt_attr, "vhdl_format") )
    fmt = get_tok_value(xctx->inst[inst].prop_ptr, "vhdl_format", 2);
  /* get netlist format rule from symbol */
  if(!xctx->tok_size && strcmp(fmt_attr, "vhdl_format"))
    fmt = get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr, "vhdl_format", 2);

  if(fmt[0]) {
   print_vhdl_primitive(fd, inst);
   return;
  }
  my_strdup(_ALLOC_ID_, &name,xctx->inst[inst].instname);
  if(!name) my_strdup(_ALLOC_ID_, &name, get_tok_value(template, "name", 0));
  if(name==NULL) {
    my_free(_ALLOC_ID_, &name);
    return;
  }
  my_strdup(_ALLOC_ID_, &template, (xctx->inst[inst].ptr + xctx->sym)->templ);
  no_of_pins= (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER];
  no_of_generics= (xctx->inst[inst].ptr + xctx->sym)->rects[GENERICLAYER];

  s=xctx->inst[inst].prop_ptr;

 /* print instance name and subckt */
  dbg(2, "print_vhdl_element(): printing inst name & subcircuit name\n");
  if( (lab = expandlabel(name, &tmp)) != NULL)
    fprintf(fd, "%d %s : %s\n", tmp, lab, sanitize(translate(inst, get_sym_name(inst, 0, 0))) );
  else  /*  name in some strange format, probably an error */
    fprintf(fd, "1 %s : %s\n", name, sanitize(translate(inst, get_sym_name(inst, 0, 0))) );
  dbg(2, "print_vhdl_element(): printing generics passed as properties\n");


  /* -------- print generics passed as properties */

  tmp=0;
  /* 20080213 use generic_type property to decide if some properties are strings, see later */
  my_strdup(_ALLOC_ID_, &generic_type, get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr,"generic_type", 0));

  while(1)
  {
    if (s==NULL) break;
   c=*s++;
   if(c=='\\') {
     escape=1;
     c=*s++;
   }
   else
    escape=0;
   space=SPACE(c);
   if( (state==TOK_BEGIN || state==TOK_ENDTOK) && !space && c != '=') state=TOK_TOKEN;
   else if( state==TOK_TOKEN && space) state=TOK_ENDTOK;
   else if( (state==TOK_TOKEN || state==TOK_ENDTOK) && c=='=') state=TOK_SEP;
   else if( state==TOK_SEP && !space) state=TOK_VALUE;
   else if( state==TOK_VALUE && space && !quote) state=TOK_END;
   STR_ALLOC(&value, value_pos, &sizeval);
   STR_ALLOC(&token, token_pos, &sizetok);
   if(state==TOK_TOKEN) token[token_pos++]=(char)c;
   else if(state==TOK_VALUE) {
     if(c=='"' && !escape) quote=!quote;
     else value[value_pos++]=(char)c;
   }
   else if(state==TOK_ENDTOK || state==TOK_SEP) {
     if(token_pos) {
       token[token_pos]='\0';
       token_pos=0;
     }
   } else if(state==TOK_END) {
     value[value_pos]='\0';
     value_pos=0;
     get_tok_value(template, token, 0);
     if(xctx->tok_size) {
       if(strcmp(token, "name") && value[0] != '\0') /* token has a value */
       {
         if(tmp == 0) {fprintf(fd, "generic map(\n");tmp++;tmp1=0;}
         if(tmp1) fprintf(fd, " ,\n");

         /* 20080213  put "" around string type generics! */
         if( generic_type && !strcmp(get_tok_value(generic_type,token, 0), "string")  ) {
           fprintf(fd, "  %s => \"%s\"", token, value);
         } else {
           fprintf(fd, "  %s => %s", token, value);
         }
         /* /20080213 */

         tmp1=1;
       }
     }
     state=TOK_BEGIN;
   }
   if(c=='\0')  /* end string */
   {
    break ;
   }
  }

  /* -------- end print generics passed as properties */
      dbg(2, "print_vhdl_element(): printing generic maps \n");

     /* print generic map */
     for(i=0;i<no_of_generics; ++i)
     {
       if(!xctx->inst[inst].node || !xctx->inst[inst].node[no_of_pins+i]) continue;
       my_strdup(_ALLOC_ID_, &generic_type,get_tok_value(
         (xctx->inst[inst].ptr + xctx->sym)->rect[GENERICLAYER][i].prop_ptr,"type",0));
       my_strdup(_ALLOC_ID_, &generic_value,   xctx->inst[inst].node[no_of_pins+i] );
       /*my_strdup(_ALLOC_ID_, &generic_value, get_tok_value( */
       /*  (xctx->inst[inst].ptr + xctx->sym)->rect[GENERICLAYER][i].prop_ptr,"value") ); */
       str_ptr = get_tok_value(
         (xctx->inst[inst].ptr + xctx->sym)->rect[GENERICLAYER][i].prop_ptr,"name",0);
    if(generic_value) {                  /*03062002 dont print generics if unassigned */
       if(tmp) fprintf(fd, " ,\n");
       if(!tmp) fprintf(fd, "generic map (\n");
       fprintf(fd,"   %s => %s",
                             str_ptr ? str_ptr : "<NULL>",
                             generic_value ? generic_value : "<NULL>"  );
       tmp=1;
    }
  }
  if(tmp) fprintf(fd, "\n)\n");
   dbg(2, "print_vhdl_element(): printing port maps \n");
  /* print port map */
  fprintf(fd, "port map(\n" );
  tmp=0;
  pinptr = (xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER];
  int_hash_init(&table, 37);
  for(i=0;i<no_of_pins; ++i)
  {
    if(strboolcmp(get_tok_value(pinptr[i].prop_ptr,"vhdl_ignore",0), "true")) {
      const char *name = get_tok_value(pinptr[i].prop_ptr, "name", 0);
      if(!int_hash_lookup(&table, name, 1, XINSERT_NOREPLACE)) {
        if( (str_ptr =  net_name(inst,i, &multip, 0, 1)) )
        {
          if(tmp) fprintf(fd, " ,\n");
          fprintf(fd, "   %s => %s",
            get_tok_value((xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][i].prop_ptr,"name",0),
            str_ptr);
          tmp=1;
        }
      }
    }
  }
  int_hash_free(&table);
  fprintf(fd, "\n);\n\n");
   dbg(2, "print_vhdl_element(): ------- end ------ \n");
  my_free(_ALLOC_ID_, &name);
  my_free(_ALLOC_ID_, &generic_value);
  my_free(_ALLOC_ID_, &generic_type);
  my_free(_ALLOC_ID_, &template);
  my_free(_ALLOC_ID_, &value);
  my_free(_ALLOC_ID_, &token);
}

void print_generic(FILE *fd, char *ent_or_comp, int symbol)
{
  int tmp;
  register int c, state=TOK_BEGIN, space;
  char *template=NULL, *s, *value=NULL,  *token=NULL;
  char *type=NULL, *generic_type=NULL, *generic_value=NULL;
  const char *str_tmp;
  int i;
  size_t sizetok=0, sizeval=0;
  size_t token_pos=0, value_pos=0;
  int quote=0;
  int escape=0;
  int token_number=0;

  my_strdup(_ALLOC_ID_, &template, xctx->sym[symbol].templ);
  if( !template || !(template[0]) ) {
    my_free(_ALLOC_ID_, &template);
    return;
  }
  my_strdup(_ALLOC_ID_, &generic_type, get_tok_value(xctx->sym[symbol].prop_ptr,"generic_type",0));
  dbg(2, "print_generic(): symbol=%d template=%s \n", symbol, template);

  fprintf(fd, "%s %s ",ent_or_comp, sanitize(get_cell(xctx->sym[symbol].name, 0)));
  if(!strcmp(ent_or_comp,"entity"))
   fprintf(fd, "is\n");
  else
   fprintf(fd, "\n");
  s=template;
  tmp=0;
  while(1)
  {
   c=*s++;
   if(c=='\\')
   {
     escape=1;
     c=*s++;
   }
   else
    escape=0;
   space=SPACE(c);
   if( (state==TOK_BEGIN || state==TOK_ENDTOK) && !space && c != '=') state=TOK_TOKEN;
   else if( state==TOK_TOKEN && space) state=TOK_ENDTOK;
   else if( (state==TOK_TOKEN || state==TOK_ENDTOK) && c=='=') state=TOK_SEP;
   else if( state==TOK_SEP && !space) state=TOK_VALUE;
   else if( state==TOK_VALUE && space && !quote) state=TOK_END;
   STR_ALLOC(&value, value_pos, &sizeval);
   STR_ALLOC(&token, token_pos, &sizetok);
   if(state==TOK_TOKEN) token[token_pos++]=(char)c;
   else if(state==TOK_VALUE)
   {
    if(c=='"' && !escape) quote=!quote;
    else value[value_pos++]=(char)c;
   }
   else if(state==TOK_ENDTOK || state==TOK_SEP) {
     if(token_pos) {
       token[token_pos]='\0';
       token_pos=0;
     }
   } else if(state==TOK_END)                    /* got a token */
   {
    token_number++;
    value[value_pos]='\0';
    value_pos=0;
    my_strdup(_ALLOC_ID_, &type, get_tok_value(generic_type,token,0));

    if(value[0] != '\0') /* token has a value */
    {
     if(token_number>1)
     {
       if(!tmp) {fprintf(fd, "generic (\n");}
       if(tmp) fprintf(fd, " ;\n");
       if(!type || strcmp(type,"string") ) { /* print "" around string values 20080418 check for type==NULL */
         fprintf(fd, "  %s : %s := %s", token, type? type:"integer", value);
       } else {
         fprintf(fd, "  %s : %s := \"%s\"", token, type? type:"integer", value);
       }                                         /* /20080213 */

       tmp=1;
     }
    }
    state=TOK_BEGIN;
   }
   if(c=='\0')  /* end string */
   {
    break ;
   }
  }

  for(i=0;i<xctx->sym[symbol].rects[GENERICLAYER]; ++i)
  {
    my_strdup(_ALLOC_ID_, &generic_type,
       get_tok_value(xctx->sym[symbol].rect[GENERICLAYER][i].prop_ptr,"generic_type",0));
    my_strdup(_ALLOC_ID_, &generic_value, 
       get_tok_value(xctx->sym[symbol].rect[GENERICLAYER][i].prop_ptr,"value", 0) );
    str_tmp = get_tok_value(xctx->sym[symbol].rect[GENERICLAYER][i].prop_ptr,"name",0);
    if(!tmp) fprintf(fd, "generic (\n");
    if(tmp) fprintf(fd, " ;\n");
    fprintf(fd,"  %s : %s",str_tmp ? str_tmp : "<NULL>",
                             generic_type ? generic_type : "<NULL>"  );
    if(generic_value &&generic_value[0])
      fprintf(fd," := %s", generic_value);
    tmp=1;
  }
  if(tmp) fprintf(fd, "\n);\n");
  my_free(_ALLOC_ID_, &template);
  my_free(_ALLOC_ID_, &value);
  my_free(_ALLOC_ID_, &token);
  my_free(_ALLOC_ID_, &type);
  my_free(_ALLOC_ID_, &generic_type);
  my_free(_ALLOC_ID_, &generic_value);
}


void print_verilog_param(FILE *fd, int symbol)
{
 register int c, state=TOK_BEGIN, space;
 char *template=NULL, *s, *value=NULL,  *generic_type=NULL, *token=NULL;
 size_t sizetok=0, sizeval=0;
 size_t token_pos=0, value_pos=0;
 int quote=0;
 int escape=0;
 int token_number=0;
 char *extra = NULL;

 my_strdup(_ALLOC_ID_, &template, xctx->sym[symbol].templ); /* 20150409 20171103 */
 if( !template || !(template[0]) )  {
   my_free(_ALLOC_ID_, &template);
   return;
 }
 my_strdup(_ALLOC_ID_, &generic_type, get_tok_value(xctx->sym[symbol].prop_ptr,"generic_type",0));
 my_strdup(_ALLOC_ID_, &extra, get_tok_value(xctx->sym[symbol].prop_ptr,"extra",0) );
 dbg(2, "print_verilog_param(): symbol=%d template=%s \n", symbol, template);

 s=template;
 while(1)
 {
  c=*s++;
  if(c=='\\')
  {
    escape=1;
    c=*s++;
  }
  else
   escape=0;
  space=SPACE(c);
  if( (state==TOK_BEGIN || state==TOK_ENDTOK) && !space && c != '=') state=TOK_TOKEN;
  else if( state==TOK_TOKEN && space) state=TOK_ENDTOK;
  else if( (state==TOK_TOKEN || state==TOK_ENDTOK) && c=='=') state=TOK_SEP;
  else if( state==TOK_SEP && !space) state=TOK_VALUE;
  else if( state==TOK_VALUE && space && !quote) state=TOK_END;

  STR_ALLOC(&value, value_pos, &sizeval);
  STR_ALLOC(&token, token_pos, &sizetok);
  if(state==TOK_TOKEN) token[token_pos++]=(char)c;
  else if(state==TOK_VALUE)
  {
   if(c=='"' && !escape) quote=!quote;
   else value[value_pos++]=(char)c;
  }
  else if(state==TOK_ENDTOK || state==TOK_SEP) {
    if(token_pos) {
      token[token_pos]='\0';
      token_pos=0;
    }
  } else if(state==TOK_END)                    /* got a token */
  {
   token_number++;
   value[value_pos]='\0';
   value_pos=0;

   if(value[0] != '\0') /* token has a value */
   {
    if(token_number>1 && (!extra || !strstr(extra, token)))
    {
      /* 20080915 put "" around string params */
      if( !generic_type || strcmp(get_tok_value(generic_type,token, 0), "time")  ) {
        if( generic_type && !strcmp(get_tok_value(generic_type,token, 0), "string")  ) {
          fprintf(fd, "  parameter   %s = \"%s\" ;\n", token,  value);
        }
        else  {
          fprintf(fd, "  parameter   %s = %s ;\n", token,  value);
        }
      }
    }
   }
   state=TOK_BEGIN;
  }
  if(c=='\0')  /* end string */
  {
   break ;
  }
 }
 my_free(_ALLOC_ID_, &template);
 my_free(_ALLOC_ID_, &generic_type);
 my_free(_ALLOC_ID_, &value);
 my_free(_ALLOC_ID_, &token);
 my_free(_ALLOC_ID_, &extra);
}




void print_tedax_subckt(FILE *fd, int symbol)
{
 int i=0, multip;
 int no_of_pins=0;
 const char *str_ptr=NULL;

  no_of_pins= xctx->sym[symbol].rects[PINLAYER];

  for(i=0;i<no_of_pins; ++i)
  {
    if(strboolcmp(get_tok_value(xctx->sym[symbol].rect[PINLAYER][i].prop_ptr,"tedax_ignore",0), "true")) {
      str_ptr=
        expandlabel(get_tok_value(xctx->sym[symbol].rect[PINLAYER][i].prop_ptr,"name",0), &multip);
      fprintf(fd, "%s ", str_ptr);
    }
  }
}

/* This function is used to generate the @pinlist replacement getting port order
 * from the spice_sym_def attribute (either directly or by loading the provided .include file), 
 * checking with the corresponding symbol pin name and getting the net name attached to it.
 * Any name mismatch is reported, in this case the function does nothing and the default xschem
 * symbol port ordering will be used. */
static int has_included_subcircuit(int inst, int symbol, char **result)
{
  char *spice_sym_def = NULL;
  int ret = 0;

  my_strdup2(_ALLOC_ID_, &spice_sym_def, get_tok_value(xctx->inst[inst].prop_ptr, "spice_sym_def", 0));
  if(!spice_sym_def[0]) {
    my_strdup2(_ALLOC_ID_, &spice_sym_def, get_tok_value(xctx->sym[symbol].prop_ptr, "spice_sym_def", 0));
  }
  if(xctx->tok_size) {
    char *symname = NULL;
    my_strdup2(_ALLOC_ID_, &symname, get_tok_value(xctx->inst[inst].prop_ptr, "schematic", 0));
    if(!symname[0]) {
      my_strdup2(_ALLOC_ID_, &symname, get_tok_value(xctx->sym[symbol].prop_ptr, "schematic", 0));
    }
    if(!symname[0]) {
      my_strdup2(_ALLOC_ID_, &symname, xctx->sym[symbol].name);
    }
    tclvareval("has_included_subcircuit {", get_cell(symname, 0), "} {",
               spice_sym_def, "}", NULL);
    my_free(_ALLOC_ID_, &symname);

    if(tclresult()[0]) {
      char *pin, *save;
      char *pinlist_ptr;
      char *pinlist = NULL;
      const char *net;
      char *tmp_result = NULL;
      int i, no_of_pins = (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER], multip; 
      int symbol_pins = 0;
      int instance_pins = 0;
      Int_hashentry *entry;
      Int_hashtable table = {NULL, 0};
      int done_first = -1;

      int_hash_init(&table, 6247);
      my_strdup2(_ALLOC_ID_, &pinlist, tclresult());
      dbg(1, "included subcircuit: pinlist=%s\n", pinlist);
      for(i = 0;i < no_of_pins; ++i) {
        char *prop = (xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][i].prop_ptr;
        int spice_ignore = !strboolcmp(get_tok_value(prop, "spice_ignore", 0), "true");
        const char *name = get_tok_value(prop, "name", 0);
        if(!spice_ignore) {
          int mult;
          char *name_expanded_ptr, *name_expanded = NULL;
          my_strdup2(_ALLOC_ID_, &name_expanded, expandlabel(name, &mult));
          name_expanded_ptr = name_expanded;
          while((pin = my_strtok_r(name_expanded_ptr, ",", "", 0, &save))) {
            int_hash_lookup(&table, pin, i, XINSERT_NOREPLACE);
            name_expanded_ptr = NULL;
          }
          my_free(_ALLOC_ID_, &name_expanded);
        }
      }

      pinlist_ptr = pinlist;
      while( (pin = my_strtok_r(pinlist_ptr, " ", "", 0, &save)) ) {
        instance_pins++;
        entry = int_hash_lookup(&table, pin, 0, XLOOKUP);
        if(entry) {
          i = entry->value;
          symbol_pins++;
          if(done_first != i) {
            net =  net_name(inst, i, &multip, 0, 1);
            my_mstrcat(_ALLOC_ID_, &tmp_result, "?", my_itoa(multip), " ", net, " ", NULL);
            done_first = i;
          }
        }
        pinlist_ptr = NULL;
      }
      int_hash_free(&table);
      if(instance_pins == symbol_pins) {
        ret = 1;
        my_mstrcat(_ALLOC_ID_, result, tmp_result, NULL);
      } else {
        dbg(0, "has_included_subcircuit(): symbol and .subckt pins do not match. Discard .subckt port order\n");
        if(has_x)
           tcleval("alert_ {has_included_subcircuit(): "
                   "symbol and .subckt pins do not match. Discard .subckt port order}");
      }
      if(tmp_result) my_free(_ALLOC_ID_, &tmp_result);
      my_free(_ALLOC_ID_, &pinlist);
    }
  }
  my_free(_ALLOC_ID_, &spice_sym_def);
  return ret;
}

void print_spice_subckt_nodes(FILE *fd, int symbol)
{
 int i=0, multip;
 const char *str_ptr=NULL;
 register int c, state=TOK_BEGIN, space;
 char *format=NULL, *format1 = NULL, *s, *token=NULL;
 int pin_number;
 size_t sizetok=0;
 size_t token_pos=0;
 int escape=0;
 int no_of_pins=0;
 char *result = NULL;
 const char *tclres, *fmt_attr = NULL;

 fmt_attr = xctx->format ? xctx->format : "format";
 my_strdup(_ALLOC_ID_, &format1, get_tok_value(xctx->sym[symbol].prop_ptr, fmt_attr, 2));
 if(!xctx->tok_size && strcmp(fmt_attr, "format") )
   my_strdup(_ALLOC_ID_, &format1, get_tok_value(xctx->sym[symbol].prop_ptr, "format", 2));
 dbg(1, "print_spice_subckt(): format1=%s\n", format1);

 /* can not do this, since @symname is used as a token later in format parser */
 /* my_strdup(_ALLOC_ID_, &format1,
  * str_replace(format1, "@symname", get_cell(xctx->sym[symbol].name, 0), '\\')); */

 if(format1 && strstr(format1, "tcleval(") == format1) {
    tclres = tcl_hook2(format1);
    if(!strcmp(tclres, "?\n")) {
      char *ptr = strrchr(format1 + 8, ')');
      *ptr = '\0';
      my_strdup(_ALLOC_ID_, &format,  format1 + 8);
    } else my_strdup(_ALLOC_ID_, &format,  tclres);
 } else {
   my_strdup(_ALLOC_ID_, &format,  format1);
 }
 if(format1) my_free(_ALLOC_ID_, &format1);
 dbg(1, "print_spice_subckt(): format=%s\n", format);
 if( format==NULL ) {
   return; /* no format */
 }
 no_of_pins= xctx->sym[symbol].rects[PINLAYER];
 s=format;

 /* begin parsing format string */
 while(1)
 {
  c=*s++;
  if(c=='\\') {
    escape=1;
    c=*s++;
  }
  else escape=0;
  if(c=='\n' && escape ) c=*s++; /* 20171030 eat escaped newlines */
  space=SPACE(c);
  if( state==TOK_BEGIN && (c=='@' || c=='%')  && !escape) state=TOK_TOKEN;
  else if(state==TOK_TOKEN && token_pos > 1 &&
     (
       ( (space  || c == '%' || c == '@') && !escape ) ||
       ( (!space && c != '%' && c != '@') && escape  )
     )
    ) {
    state = TOK_SEP;
  }

  STR_ALLOC(&token, token_pos, &sizetok);
  if(state==TOK_TOKEN) {
    token[token_pos++]=(char)c;
  }
  else if(state==TOK_SEP)                    /* got a token */
  {
   token[token_pos]='\0';
   token_pos=0;
   if(!strcmp(token, "@spiceprefix")) {
     /* do nothing */
   }
   else if(!strcmp(token, "@name")) {
     /* do nothing */
   }
   else if(strcmp(token, "@symname")==0) {
     break ;
   }
   else if(strcmp(token, "@pinlist")==0) {
     Int_hashtable table = {NULL, 0};
     int_hash_init(&table, 37);
     for(i=0;i<no_of_pins; ++i)
     {
       if(strboolcmp(get_tok_value(xctx->sym[symbol].rect[PINLAYER][i].prop_ptr,"spice_ignore",0), "true")) {
         const char *name = get_tok_value(xctx->sym[symbol].rect[PINLAYER][i].prop_ptr,"name",0);
         if(!int_hash_lookup(&table, name, 1, XINSERT_NOREPLACE)) {
           str_ptr= expandlabel(name, &multip);
           /* fprintf(fd, "%s ", str_ptr); */
           my_mstrcat(_ALLOC_ID_, &result, str_ptr, " ", NULL);
         }
       }
     }
     int_hash_free(&table);
   }
   else if(token[0]=='@' && token[1]=='@') {    /* recognize single pins 15112003 */
     char *prop=NULL;
     for(i = 0; i<no_of_pins; ++i) {
       prop = xctx->sym[symbol].rect[PINLAYER][i].prop_ptr;
       if(!strcmp(get_tok_value(prop, "name",0), token + 2)) break;
     }
     if(i<no_of_pins && strboolcmp(get_tok_value(prop,"spice_ignore",0), "true")) {
       /* fprintf(fd, "%s ", expandlabel(token+2, &multip)); */
       my_mstrcat(_ALLOC_ID_, &result, expandlabel(token+2, &multip), " ", NULL);
     }
   }
   /* reference by pin number instead of pin name, allows faster lookup of the attached net name 20180911 */
   else if(token[0]=='@' && token[1]=='#') {
     char *pin_attr = NULL;
     char *pin_num_or_name = NULL;
     get_pin_and_attr(token, &pin_num_or_name, &pin_attr);
     pin_number = get_sym_pin_number(symbol, pin_num_or_name);
     if(pin_number >= 0 && pin_number < no_of_pins) {
       if(strboolcmp(get_tok_value(xctx->sym[symbol].rect[PINLAYER][pin_number].prop_ptr,"spice_ignore",0), "true")) {
       str_ptr =  get_tok_value(xctx->sym[symbol].rect[PINLAYER][pin_number].prop_ptr,"name",0);
       /* fprintf(fd, "%s ",  expandlabel(str_ptr, &multip)); */
       my_mstrcat(_ALLOC_ID_, &result, expandlabel(str_ptr, &multip), " ", NULL);
       }
     }
     my_free(_ALLOC_ID_, &pin_attr);
     my_free(_ALLOC_ID_, &pin_num_or_name);
   
   }
   /* this will print the other @parameters, usually "extra" nodes so they will be in the order
    * specified by the format string. The 'extra' attribute is no more used to print extra nodes
    * in spice_block_netlist(). */
   else if(token[0] == '@') { /* given previous if() conditions not followed by @ or # */
     /* if token not followed by white space it is not an extra node */
     if( ( (space  || c == '%' || c == '@') && !escape ) ) {
       /* fprintf(fd, "%s ",  token + 1); */
       my_mstrcat(_ALLOC_ID_, &result, token + 1, " ", NULL);
     }
   }
   /* if(c!='%' && c!='@' && c!='\0' ) fputc(c,fd); */ 
   if(c == '@' || c =='%') s--;
   state=TOK_BEGIN;
  }
                 /* 20151028 dont print escaping backslashes */
  else if(state==TOK_BEGIN && c!='\0') {
   /* do nothing */
  }
  if(c=='\0')
  {
   my_mstrcat(_ALLOC_ID_, &result, "\n", NULL);
   break ;
  }
 }
 if(result) {
   fprintf(fd, "%s", result);
   my_free(_ALLOC_ID_, &result);
 }
 my_free(_ALLOC_ID_, &format1);
 my_free(_ALLOC_ID_, &format);
 my_free(_ALLOC_ID_, &token);
}

int print_spice_element(FILE *fd, int inst)
{
  int i=0, multip, itmp;
  const char *str_ptr=NULL;
  register int c, state=TOK_BEGIN, space;
  char *template=NULL,*format=NULL, *s, *name=NULL,  *token=NULL;
  const char *lab, *value = NULL;
  /* char *translatedvalue = NULL; */
  size_t sizetok=0;
  size_t token_pos=0;
  int escape=0;
  int no_of_pins=0;
  char *result = NULL;
  size_t size = 0;
  char *spiceprefixtag = NULL; 
  const char *fmt_attr = NULL;

  size = CADCHUNKALLOC;
  my_realloc(_ALLOC_ID_, &result, size);
  result[0] = '\0';

  my_strdup(_ALLOC_ID_, &template, (xctx->inst[inst].ptr + xctx->sym)->templ);
  my_strdup(_ALLOC_ID_, &name,xctx->inst[inst].instname);
  if (!name) my_strdup(_ALLOC_ID_, &name, get_tok_value(template, "name", 0));

  fmt_attr = xctx->format ? xctx->format : "format";
  /* allow format string override in instance */
  my_strdup(_ALLOC_ID_, &format, get_tok_value(xctx->inst[inst].prop_ptr, fmt_attr, 2));
  /* get netlist format rule from symbol */
  if(!xctx->tok_size)
    my_strdup(_ALLOC_ID_, &format, get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr, fmt_attr, 2));
  /* allow format string override in instance */
  if(!xctx->tok_size && strcmp(fmt_attr, "format") )
    my_strdup(_ALLOC_ID_, &format, get_tok_value(xctx->inst[inst].prop_ptr, "format", 2));
  /* get netlist format rule from symbol */
  if(!xctx->tok_size && strcmp(fmt_attr, "format"))
     my_strdup(_ALLOC_ID_, &format, get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr, "format", 2));
  if ((name==NULL) || (format==NULL)) {
    my_free(_ALLOC_ID_, &template);
    my_free(_ALLOC_ID_, &format);
    my_free(_ALLOC_ID_, &name);
    my_free(_ALLOC_ID_, &result);
    return 0; /* do no netlist unwanted insts(no format) */
  }
  no_of_pins= (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER];
  s=format;
  dbg(1, "print_spice_element(): name=%s, format=%s xctx->netlist_count=%d\n",name,format, xctx->netlist_count);
  /* begin parsing format string */
  while(1)
  {
    /* always make room for some characters so the single char writes to result do not need reallocs */
    c=*s++;
    if(c=='\\') {
      escape=1;
      c=*s++;
    }
    else escape=0;

    if (c=='\n' && escape) c=*s++; /* 20171030 eat escaped newlines */
    space=SPACE(c);
    if ( state==TOK_BEGIN && (c=='@'|| c=='%')  && !escape ) state=TOK_TOKEN;
    else if(state==TOK_TOKEN && token_pos > 1 &&
       (
         ( (space  || c == '%' || c == '@') && !escape ) ||
         ( (!space && c != '%' && c != '@') && escape  )
       )
      ) {
      dbg(1, "print_spice_element(): c=%c, space=%d, escape=%d token_pos=%d\n", c, space, escape, token_pos);
      state=TOK_SEP;
    }
    STR_ALLOC(&token, token_pos, &sizetok);
    if(state==TOK_TOKEN) {
      token[token_pos++]=(char)c;
    }
    else if (state==TOK_SEP)                    /* got a token */
    {
      char *val = NULL;
      size_t token_exists = 0;
      token[token_pos]='\0';
      token_pos=0;

      /* if spiceprefix==0 and token == @spiceprefix then set empty value */
      if (!tclgetboolvar("spiceprefix") && !strcmp(token, "@spiceprefix")) {
        value=NULL;
      } else {
        size_t tok_val_len;
        size_t tok_size;
        dbg(1, "print_spice_element(): token: |%s|\n", token);
        my_strdup2(_ALLOC_ID_, &val, get_tok_value(xctx->inst[inst].prop_ptr, token+1, 0));
        tok_size =  xctx->tok_size;
        value = val;
        if(strchr(value, '@')) {
          /* Symbol format string contains model=@modp, 
           * instance attributes does not contain a modp=xxx, 
           * look up modp in **parent** instance prop_ptr and symbol template attribute */

          value = translate3(val, xctx->inst[inst].prop_ptr,
                                  xctx->hier_attr[xctx->currsch - 1].prop_ptr,
                                  xctx->hier_attr[xctx->currsch - 1].templ);
        }
        tok_val_len = strlen(value);
        if(!strcmp(token, "@spiceprefix") && value[0]) {
          my_realloc(_ALLOC_ID_, &spiceprefixtag, tok_val_len+22);
          my_snprintf(spiceprefixtag, tok_val_len+22, "**** spice_prefix %s\n", value);
          value = spiceprefixtag;
        }
        xctx->tok_size = tok_size;
        /* xctx->tok_size==0 indicates that token(+1) does not exist in instance attributes */

        if (!xctx->tok_size) value=get_tok_value(template, token+1, 0);
        token_exists = xctx->tok_size;

        if(!strcmp("@savecurrent", token)) {
          token_exists = 0; /* processed later */
          value = NULL;
        }
      }
      if(!token_exists && token[0] =='%') {
        /* result_pos += my_snprintf(result + result_pos, tmp, "%s", token + 1); */
        my_mstrcat(_ALLOC_ID_, &result, token + 1, NULL);
        /* fputs(token + 1, fd); */
      } else if (value && value[0]!='\0') {
         /* instance names (name) and node labels (lab) go thru the expandlabel function. */
        /*if something else must be parsed, put an if here! */

        if (!(strcmp(token+1,"name") && strcmp(token+1,"lab"))  /* expand name/labels */
              && ((lab = expandlabel(value, &itmp)) != NULL)) {
          /* result_pos += my_snprintf(result + result_pos, tmp, "%s", lab); */
          my_mstrcat(_ALLOC_ID_, &result, lab, NULL);
          /* fputs(lab,fd); */


 
        } else { 

          /* result_pos += my_snprintf(result + result_pos, tmp, "%s", value); */
          my_mstrcat(_ALLOC_ID_, &result, value, NULL);
          /* fputs(value,fd); */
        }
      }
      else if(strcmp(token,"@symref")==0) 
      {
        const char *s = get_sym_name(inst, 9999, 1);
        /* result_pos += my_snprintf(result + result_pos, tmp, "%s", s); */
        my_mstrcat(_ALLOC_ID_, &result, s, NULL);
      }
      else if (strcmp(token,"@symname")==0) /* of course symname must not be present in attributes */
      {
        const char *s = sanitize(translate(inst, get_sym_name(inst, 0, 0)));
        /* result_pos += my_snprintf(result + result_pos, tmp, "%s", s); */
        my_mstrcat(_ALLOC_ID_, &result, s, NULL);
        /* fputs(s,fd); */
      }
      else if (strcmp(token,"@symname_ext")==0) /* of course symname_ext must not be present in attributes */
      {
        const char *s = sanitize(translate(inst, get_sym_name(inst, 0, 1)));
        /* result_pos += my_snprintf(result + result_pos, tmp, "%s", s); */
        my_mstrcat(_ALLOC_ID_, &result, s, NULL);
        /* fputs(s,fd); */
      }
      else if(strcmp(token,"@topschname")==0) /* of course topschname must not be present in attributes */
      {
        const char *topsch;
        topsch = get_trailing_path(xctx->sch[0], 0, 1);
        /* result_pos += my_snprintf(result + result_pos, tmp, "%s", topsch); */
        my_mstrcat(_ALLOC_ID_, &result, topsch, NULL);
      }
      else if(strcmp(token,"@schname_ext")==0) /* of course schname must not be present in attributes */
      {
        /* result_pos += my_snprintf(result + result_pos, tmp, "%s", xctx->current_name); */
        my_mstrcat(_ALLOC_ID_, &result, xctx->current_name, NULL);
        /* fputs(xctx->current_name, fd); */
      }
      else if(strcmp(token,"@savecurrent")==0)
      {
        char *instname = xctx->inst[inst].instname;

        const char *sc = get_tok_value(xctx->inst[inst].prop_ptr, "savecurrent", 0);
        if(!sc[0]) sc = get_tok_value(template, "savecurrent", 0);
        if(!strboolcmp(sc , "true")) {
          /* result_pos += my_snprintf(result + result_pos, tmp, "\n.save I( ?1 %s )", instname); */
          my_mstrcat(_ALLOC_ID_, &result, "\n.save I( ?1 ", instname, " )", NULL);
        }
      }
      else if(strcmp(token,"@schname")==0) /* of course schname must not be present in attributes */
      {
        const char *schname = get_cell(xctx->current_name, 0);
        /* result_pos += my_snprintf(result + result_pos, tmp, "%s", schname); */
        my_mstrcat(_ALLOC_ID_, &result, schname, NULL);
      }
      else if(strcmp(token,"@pinlist")==0) /* of course pinlist must not be present in attributes */
                                           /* print multiplicity */
      {                                    /* and node number: m1 n1 m2 n2 .... */
        if(!has_included_subcircuit(inst, xctx->inst[inst].ptr, &result)) {
          Int_hashtable table = {NULL, 0};
          int_hash_init(&table, 37);
          for(i=0;i<no_of_pins; ++i)
          {
            char *prop = (xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][i].prop_ptr;
            int spice_ignore = !strboolcmp(get_tok_value(prop, "spice_ignore", 0), "true");
            const char *name = get_tok_value(prop, "name", 0);
            if(!spice_ignore) {
              if(!int_hash_lookup(&table, name, 1, XINSERT_NOREPLACE)) {
                str_ptr =  net_name(inst, i, &multip, 0, 1);
  
                /* result_pos += my_snprintf(result + result_pos, tmp, "?%d %s ", multip, str_ptr); */
                my_mstrcat(_ALLOC_ID_, &result, "?", my_itoa(multip), " ", str_ptr, " ", NULL);
              }
            }
          }
          int_hash_free(&table);
        }
      }
      else if(token[0]=='@' && token[1]=='@') {    /* recognize single pins 15112003 */
        for(i=0;i<no_of_pins; ++i) {
          char *prop = (xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][i].prop_ptr;
          if (!strcmp( get_tok_value(prop,"name",0), token+2)) {
            if(strboolcmp(get_tok_value(prop,"spice_ignore",0), "true")) {
              str_ptr =  net_name(inst,i, &multip, 0, 1);

              /* result_pos += my_snprintf(result + result_pos, tmp, "?%d %s ", multip, str_ptr); */
              my_mstrcat(_ALLOC_ID_, &result, "?", my_itoa(multip), " ", str_ptr, " ", NULL);
            }
            break;
          }
        }
      }
      /* reference by pin number instead of pin name, allows faster lookup of the attached net name 
       * @#0, @#1:net_name, @#2:name, ... */
      else if(token[0]=='@' && token[1]=='#') {
        int n;
        char *pin_attr = NULL;
        char *pin_num_or_name = NULL;
   
        get_pin_and_attr(token, &pin_num_or_name, &pin_attr);
        n = get_inst_pin_number(inst, pin_num_or_name);
        if(n>=0  && pin_attr[0] && n < (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER]) {
          char *pin_attr_value = NULL;
          int is_net_name = !strcmp(pin_attr, "net_name");
          /* get pin_attr value from instance: "pinnumber(ENABLE)=5" --> return 5, attr "pinnumber" of pin "ENABLE"
           *                                   "pinnumber(3)=6       --> return 6, attr "pinnumber" of 4th pin */
          if(!is_net_name) {
            pin_attr_value = get_pin_attr_from_inst(inst, n, pin_attr);
            /* get pin_attr from instance pin attribute string */
            if(!pin_attr_value) {
             my_strdup(_ALLOC_ID_, &pin_attr_value,
                get_tok_value((xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][n].prop_ptr, pin_attr, 0));
            }
          }
          /* @#n:net_name attribute (n = pin number or name) will translate to net name attached  to pin */
          if(!pin_attr_value && is_net_name) {
            prepare_netlist_structs(0);
            my_strdup(_ALLOC_ID_, &pin_attr_value,
                 xctx->inst[inst].node && xctx->inst[inst].node[n] ? xctx->inst[inst].node[n] : "?");
          }
          if(!pin_attr_value ) my_strdup(_ALLOC_ID_, &pin_attr_value, "--UNDEF--");
          value = pin_attr_value;
          /* recognize slotted devices: instname = "U3:3", value = "a:b:c:d" --> value = "c" */
          if(value[0] && !strcmp(pin_attr, "pinnumber") ) {
            char *ss;
            int slot;
            char *tmpstr = NULL;
            tmpstr = my_malloc(_ALLOC_ID_, sizeof(xctx->inst[inst].instname));
            if( (ss=strchr(xctx->inst[inst].instname, ':')) ) {
              sscanf(ss+1, "%s", tmpstr);
              if(isonlydigit(tmpstr)) {
                slot = atoi(tmpstr);
                if(strstr(value,":")) value = find_nth(value, ":", "", 0, slot);
              }
            }
            my_free(_ALLOC_ID_, &tmpstr);
          }
          /* result_pos += my_snprintf(result + result_pos, tmp, "%s", value); */
          my_mstrcat(_ALLOC_ID_, &result, value, NULL);
          my_free(_ALLOC_ID_, &pin_attr_value);
        }
        else if(n>=0  && n < (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER]) {
          const char *si;
          char *prop = (xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][n].prop_ptr;
          si  = get_tok_value(prop, "spice_ignore",0);
          if(strboolcmp(si, "true")) {
            str_ptr =  net_name(inst,n, &multip, 0, 1);

            /* result_pos += my_snprintf(result + result_pos, tmp, "?%d %s ", multip, str_ptr); */
            my_mstrcat(_ALLOC_ID_, &result, "?", my_itoa(multip), " ", str_ptr, " ", NULL);
          }
        }
        my_free(_ALLOC_ID_, &pin_attr);
        my_free(_ALLOC_ID_, &pin_num_or_name);
      }
      else if (!strncmp(token,"@tcleval", 8)) {
        size_t s;
        char *tclcmd=NULL;
        const char *res;
        s = token_pos + strlen(name) + strlen(xctx->inst[inst].name) + 100;
        tclcmd = my_malloc(_ALLOC_ID_, s);
        Tcl_ResetResult(interp);
        my_snprintf(tclcmd, s, "tclpropeval {%s} {%s} {%s}", token, name, xctx->inst[inst].name);
        dbg(1, "print_spice_element(): tclpropeval {%s} {%s} {%s}", token, name, xctx->inst[inst].name);
        res = tcleval(tclcmd);

        /* result_pos += my_snprintf(result + result_pos, tmp, "%s", res); */
        my_mstrcat(_ALLOC_ID_, &result, res, NULL);
        /* fprintf(fd, "%s", tclresult()); */
        my_free(_ALLOC_ID_, &tclcmd);
      } /* /20171029 */


      if(c != '%' && c != '@' && c!='\0' ) {
        char str[2];
        str[0] = (unsigned char) c;
        str[1] = '\0';
       
        /* result_pos += my_snprintf(result + result_pos, 2, "%c", c); */ /* no realloc needed */ 
        my_mstrcat(_ALLOC_ID_, &result, str, NULL);
        /* fputc(c,fd); */
      }
      if(c == '@' || c == '%' ) s--;
      state=TOK_BEGIN;
      my_free(_ALLOC_ID_, &val);
    }
    else if(state==TOK_BEGIN && c!='\0') {
      char str[2];
      str[0] = (unsigned char) c;
      str[1] = '\0';
      /* result_pos += my_snprintf(result + result_pos, 2, "%c", c); */ /* no realloc needed */
      my_mstrcat(_ALLOC_ID_, &result, str, NULL);
      /* fputc(c,fd); */
    }
    if(c=='\0')
    {
      char str[2];
      str[0] = '\n';
      str[1] = '\0';
      /* result_pos += my_snprintf(result + result_pos, 2, "%c", '\n'); */ /* no realloc needed */
      my_mstrcat(_ALLOC_ID_, &result, str, NULL);
      /* fputc('\n',fd); */
      break;
    }
  } /* while(1) */


  /* if result is like: 'tcleval(some_string)' pass it thru tcl evaluation so expressions
   * can be calculated */

  /* do one level of substitutions to resolve @params and equations*/
  if(result && strstr(result, "tcleval(")== result) {
    dbg(1, "print_spice_element(): before translate() result=%s\n", result);
    my_strdup(_ALLOC_ID_, &result, translate(inst, result));
    dbg(1, "print_spice_element(): after  translate() result=%s\n", result);
  }


  /* can't remember what the f**k this is supposed to do. 
     why eval( and not tcleval( ? 
     disable until some regression pops out
  */
  #if 0
  *  /* do a second round of substitutions, but without calling tcl */
  *  if(result && strstr(result, "eval(") == result) {
  *    char *c = strrchr(result, ')');
  *    if(c) while(1) { /* shift following characters back 1 char */
  *      *c = (char)c[1];
  *      c++;
  *      if(!*c) break;
  *    }
  *    my_strdup2(_ALLOC_ID_, &result, translate(inst, result+5));
  *  }
  #endif


  if(result) fprintf(fd, "%s", result);
  my_free(_ALLOC_ID_, &template);
  my_free(_ALLOC_ID_, &format);
  my_free(_ALLOC_ID_, &name);
  my_free(_ALLOC_ID_, &token);
  my_free(_ALLOC_ID_, &result);
  my_free(_ALLOC_ID_, &spiceprefixtag);
  /* my_free(_ALLOC_ID_, &translatedvalue); */
  return 1;
}

void print_tedax_element(FILE *fd, int inst)
{
 int i=0, multip;
 const char *str_ptr=NULL;
 register int c, state=TOK_BEGIN, space;
 char *template=NULL,*format=NULL,*s, *name=NULL, *token=NULL;
 const char *value;
 char *extra=NULL, *extra_pinnumber=NULL;
 char *numslots=NULL;
 const char *extra_token, *extra_token_val;
 char *extra_ptr;
 char *extra_pinnumber_token, *extra_pinnumber_ptr;
 char *saveptr1, *saveptr2;
 const char *tmp;
 int instance_based=0;
 size_t sizetok=0;
 size_t token_pos=0; 
 int escape=0;
 int no_of_pins=0;
 int subcircuit = 0;

 my_strdup(_ALLOC_ID_, &extra, get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr,"extra",0));
 my_strdup(_ALLOC_ID_, &extra_pinnumber, get_tok_value(xctx->inst[inst].prop_ptr,"extra_pinnumber",0));
 if(!extra_pinnumber) my_strdup(_ALLOC_ID_, &extra_pinnumber,
         get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr,"extra_pinnumber",0));
 my_strdup(_ALLOC_ID_, &template,
     (xctx->inst[inst].ptr + xctx->sym)->templ);
 my_strdup(_ALLOC_ID_, &numslots, get_tok_value(xctx->inst[inst].prop_ptr,"numslots",0));
 if(!numslots) my_strdup(_ALLOC_ID_, &numslots, get_tok_value(template,"numslots",0));
 if(!numslots) my_strdup(_ALLOC_ID_, &numslots, "1");

 my_strdup(_ALLOC_ID_, &name,xctx->inst[inst].instname);
 /* my_strdup(xxx, &name,get_tok_value(xctx->inst[inst].prop_ptr,"name",0)); */
 if(!name) my_strdup(_ALLOC_ID_, &name, get_tok_value(template, "name", 0));

 /* allow format string override in instance */
 my_strdup(_ALLOC_ID_, &format, get_tok_value(xctx->inst[inst].prop_ptr,"tedax_format",2));
 if(!format || !format[0])
   my_strdup(_ALLOC_ID_, &format, get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr,"tedax_format",2));

 no_of_pins= (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER];
 if( !format && !strcmp((xctx->inst[inst].ptr + xctx->sym)->type, "subcircuit") ) {
   char *net = NULL;
   char *pinname = NULL;
   char *pin = NULL;
   char *netbit=NULL;
   char *pinbit = NULL;
   int net_mult;
   int pin_mult;
   int n;
   Int_hashtable table={NULL, 0};
   subcircuit = 1;
   fprintf(fd, "__subcircuit__ %s %s\n",
       sanitize(translate(inst, get_sym_name(inst, 0, 0))), xctx->inst[inst].instname);
   int_hash_init(&table, 37);
   for(i=0;i<no_of_pins; ++i) {
     my_strdup2(_ALLOC_ID_, &net, net_name(inst,i, &net_mult, 0, 1));
     my_strdup2(_ALLOC_ID_, &pinname, 
       get_tok_value((xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][i].prop_ptr,"name",0));
     my_strdup2(_ALLOC_ID_, &pin, expandlabel(pinname, &pin_mult));
     if(!int_hash_lookup(&table, pinname, 1, XINSERT_NOREPLACE)) {
       dbg(1, "#net=%s pinname=%s pin=%s net_mult=%d pin_mult=%d\n", net, pinname, pin, net_mult, pin_mult);
       for(n = 0; n < net_mult; ++n) {
         my_strdup(_ALLOC_ID_, &netbit, find_nth(net, ",", "", 0, n+1));
         my_strdup(_ALLOC_ID_, &pinbit, find_nth(pin, ",", "", 0, n+1));
         fprintf(fd, "__map__ %s -> %s\n", 
           pinbit ? pinbit : "__UNCONNECTED_PIN__", 
           netbit ? netbit : "__UNCONNECTED_PIN__");
       }
     }
   }
   int_hash_free(&table);
   my_free(_ALLOC_ID_, &net);
   my_free(_ALLOC_ID_, &pin);
   my_free(_ALLOC_ID_, &pinname);
   my_free(_ALLOC_ID_, &pinbit);
   my_free(_ALLOC_ID_, &netbit);
   fprintf(fd, "\n");
 }

 if(name==NULL || !format || !format[0]) {
   my_free(_ALLOC_ID_, &extra);
   my_free(_ALLOC_ID_, &extra_pinnumber);
   my_free(_ALLOC_ID_, &template);
   my_free(_ALLOC_ID_, &numslots);
   my_free(_ALLOC_ID_, &format);
   my_free(_ALLOC_ID_, &name);
   return;
 }

 if(!subcircuit) {
   fprintf(fd, "begin_inst %s numslots %s\n", name, numslots);
   for(i=0;i<no_of_pins; ++i) {
     char *pinnumber;
     pinnumber = get_pin_attr_from_inst(inst, i, "pinnumber");
     if(!pinnumber) {
       my_strdup2(_ALLOC_ID_, &pinnumber,
              get_tok_value((xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][i].prop_ptr,"pinnumber",0));
     }
     if(!xctx->tok_size) my_strdup(_ALLOC_ID_, &pinnumber, "--UNDEF--");
     tmp = net_name(inst,i, &multip, 0, 1);
     if(tmp && !strstr(tmp, "__UNCONNECTED_PIN__")) {
       fprintf(fd, "conn %s %s %s %s %d\n",
             name,
             tmp,
             get_tok_value((xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][i].prop_ptr,"name",0),
             pinnumber,
             i+1);
     }
     my_free(_ALLOC_ID_, &pinnumber);
   }
  
   if(extra){
     char netstring[40];
     /* fprintf(errfp, "extra_pinnumber: |%s|\n", extra_pinnumber); */
     /* fprintf(errfp, "extra: |%s|\n", extra); */
     for(extra_ptr = extra, extra_pinnumber_ptr = extra_pinnumber; ; extra_ptr=NULL, extra_pinnumber_ptr=NULL) {
       extra_pinnumber_token=my_strtok_r(extra_pinnumber_ptr, " ", "", 0, &saveptr1);
       extra_token=my_strtok_r(extra_ptr, " ", "", 0, &saveptr2);
       if(!extra_token) break;
       /* fprintf(errfp, "extra_pinnumber_token: |%s|\n", extra_pinnumber_token); */
       /* fprintf(errfp, "extra_token: |%s|\n", extra_token); */
       instance_based=0;
  
       /* alternate instance based extra net naming: net:<pinumber>=netname */
       my_snprintf(netstring, S(netstring), "net:%s", extra_pinnumber_token);
       dbg(1, "print_tedax_element(): netstring=%s\n", netstring);
       extra_token_val=get_tok_value(xctx->inst[inst].prop_ptr, extra_token, 0);
       if(!extra_token_val[0]) extra_token_val=get_tok_value(xctx->inst[inst].prop_ptr, netstring, 0);
       if(!extra_token_val[0]) extra_token_val=get_tok_value(template, extra_token, 0);
       else instance_based=1;
       if(!extra_token_val[0]) extra_token_val="--UNDEF--";
  
       fprintf(fd, "conn %s %s %s %s %d", name, extra_token_val, extra_token, extra_pinnumber_token, i+1);
       ++i;
       if(instance_based) fprintf(fd, " # instance_based");
       fprintf(fd,"\n");
     }
   }
 }
 if(format) {
  s=format;
  dbg(1, "print_tedax_element(): name=%s, tedax_format=%s xctx->netlist_count=%d\n",name,format, xctx->netlist_count);
  /* begin parsing format string */
  while(1)
  {
   c=*s++;
   if(c=='\\') {
     escape=1;
     c=*s++;
   }
   else escape=0;
   if(c=='\n' && escape ) c=*s++; /* 20171030 eat escaped newlines */
   space=SPACE(c);

   if( state==TOK_BEGIN && (c=='%' || c=='@') && !escape) state=TOK_TOKEN;
   else if(state==TOK_TOKEN && token_pos > 1 &&
      (
        ( (space  || c == '%' || c == '@') && !escape ) ||
        ( (!space && c != '%' && c != '@') && escape  )
      )
     ) {
     state=TOK_SEP;
   }

   STR_ALLOC(&token, token_pos, &sizetok);
   if(state==TOK_TOKEN) {
     token[token_pos++]=(char)c; /* 20171029 remove escaping backslashes */
   }
   else if(state==TOK_SEP)                   /* got a token */
   {
    token[token_pos]='\0';
    token_pos=0;

    value = get_tok_value(xctx->inst[inst].prop_ptr, token+1, 0);
     /* xctx->tok_size==0 indicates that token(+1) does not exist in instance attributes */
    if(!xctx->tok_size) value=get_tok_value(template, token+1, 0);
    if(!xctx->tok_size && token[0] =='%') {
      fputs(token + 1, fd);
    } else if(value[0]!='\0')
    {
      fputs(value,fd);
    }
    else if(strcmp(token,"@symref")==0)
    {
      const char *s = get_sym_name(inst, 9999, 1);
      fputs(s, fd);
    }
    else if(strcmp(token,"@symname")==0)        /* of course symname must not be present  */
                                        /* in hash table */
    {
      const char *s = sanitize(translate(inst, get_sym_name(inst, 0, 0)));
      fputs(s, fd);
    }
    else if (strcmp(token,"@symname_ext")==0) 
    {
      const char *s = sanitize(translate(inst, get_sym_name(inst, 0, 1)));
      fputs(s, fd);
    }
    else if(strcmp(token,"@schname_ext")==0)        /* of course schname must not be present  */
                                                /* in hash table */
    {
     /* fputs(xctx->sch[xctx->currsch],fd); */
     fputs(xctx->current_name, fd);
    }
    else if(strcmp(token,"@schname")==0)        /* of course schname must not be present  */
                                                /* in hash table */
    {
     fputs(get_cell(xctx->current_name, 0), fd);
    }
    else if(strcmp(token,"@topschname")==0) /* of course topschname must not be present in attributes */
    {
      const char *topsch;
      topsch = get_trailing_path(xctx->sch[0], 0, 1);
      fputs(topsch, fd);
    }
    else if(strcmp(token,"@pinlist")==0)
                                        /* print multiplicity */
    {                                   /* and node number: m1 n1 m2 n2 .... */
     for(i=0;i<no_of_pins; ++i)
     {
       str_ptr =  net_name(inst,i, &multip, 0, 1);
       /* fprintf(errfp, "inst: %s  --> %s\n", name, str_ptr); */
       fprintf(fd, "?%d %s ", multip, str_ptr);
     }
    }
    else if(token[0]=='@' && token[1]=='@') {    /* recognize single pins 15112003 */
     for(i=0;i<no_of_pins; ++i) {
      if(!strcmp(
           get_tok_value((xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][i].prop_ptr,"name",0),
           token+2
          )
        ) {
        str_ptr =  net_name(inst,i, &multip, 0, 1);
        fprintf(fd, "%s", str_ptr);
        break;
      }
     }
    }
    /* this allow to print in netlist any properties defined for pins.
     * @#n:property, where 'n' is the pin index (starting from 0) and
     * 'property' the property defined for that pin (property=value)
     * in case this property is found the value for it is printed.
     * if device is slotted (U1:m) and property value for pin
     * is also slotted ('a:b:c:d') then print the m-th substring.
     * if property value is not slotted print entire value regardless of device slot.
     * slot numbers start from 1
     */
    else if(token[0]=='@' && token[1]=='#') {
      int n;
      char *pin_attr = NULL;
      char *pin_num_or_name = NULL;
   
      get_pin_and_attr(token, &pin_num_or_name, &pin_attr);
      n = get_inst_pin_number(inst, pin_num_or_name);
      if(n>=0  && pin_attr[0] && n < (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER]) {
        char *pin_attr_value = NULL;
        int is_net_name = !strcmp(pin_attr, "net_name");
        /* get pin_attr value from instance: "pinnumber(ENABLE)=5" --> return 5, attr "pinnumber" of pin "ENABLE"
         *                                   "pinnumber(3)=6       --> return 6, attr "pinnumber" of 4th pin */
        if(!is_net_name) {
          pin_attr_value = get_pin_attr_from_inst(inst, n, pin_attr);
          /* get pin_attr from instance pin attribute string */
          if(!pin_attr_value) {
           my_strdup(_ALLOC_ID_, &pin_attr_value,
              get_tok_value((xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][n].prop_ptr, pin_attr, 0));
          }
        }
        /* @#n:net_name attribute (n = pin number or name) will translate to net name attached  to pin */
        if(!pin_attr_value && is_net_name) {
          prepare_netlist_structs(0);
          my_strdup(_ALLOC_ID_, &pin_attr_value,
               xctx->inst[inst].node && xctx->inst[inst].node[n] ? xctx->inst[inst].node[n] : "?");
        }
        if(!pin_attr_value ) my_strdup(_ALLOC_ID_, &pin_attr_value, "--UNDEF--");
        value = pin_attr_value;
        /* recognize slotted devices: instname = "U3:3", value = "a:b:c:d" --> value = "c" */
        if(value[0] && !strcmp(pin_attr, "pinnumber")) {
          char *ss;
          int slot;
          char *tmpstr = NULL;
          tmpstr = my_malloc(_ALLOC_ID_, sizeof(xctx->inst[inst].instname));
          if( (ss=strchr(xctx->inst[inst].instname, ':')) ) {
            sscanf(ss+1, "%s", tmpstr);
            if(isonlydigit(tmpstr)) {
              slot = atoi(tmpstr);
              if(strstr(value,":")) value = find_nth(value, ":", "", 0, slot);
            }
          }
          my_free(_ALLOC_ID_, &tmpstr);
        }
        fprintf(fd, "%s", value);
        my_free(_ALLOC_ID_, &pin_attr_value);
      }
      else if(n>=0  && n < (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER]) {
        const char *si;
        char *prop = (xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][n].prop_ptr;
        si  = get_tok_value(prop, "tedax_ignore",0);
        if(strboolcmp(si, "true")) {
          str_ptr =  net_name(inst,n, &multip, 0, 1);
          fprintf(fd, "%s", str_ptr);
        }
      }
      my_free(_ALLOC_ID_, &pin_attr);
      my_free(_ALLOC_ID_, &pin_num_or_name);
    }
    else if(!strncmp(token,"@tcleval", 8)) {
      /* char tclcmd[strlen(token)+100] ; */
      size_t s;
      char *tclcmd=NULL;
      s = token_pos + strlen(name) + strlen(xctx->inst[inst].name) + 100;
      tclcmd = my_malloc(_ALLOC_ID_, s);
      Tcl_ResetResult(interp);
      my_snprintf(tclcmd, s, "tclpropeval {%s} {%s} {%s}", token, name, xctx->inst[inst].name);
      tcleval(tclcmd);
      fprintf(fd, "%s", tclresult());
      my_free(_ALLOC_ID_, &tclcmd);
      /* fprintf(errfp, "%s\n", tclcmd); */
    } /* /20171029 */


    if(c!='%' && c!='@' && c!='\0') fputc(c,fd);
    if(c == '@' || c == '%' ) s--;
    state=TOK_BEGIN;
   }
   else if(state==TOK_BEGIN && c!='\0')  fputc(c,fd);
   if(c=='\0')
   {
    fputc('\n',fd);
    break ;
   }
  }
 } /* if(format) */
 if(!subcircuit) fprintf(fd,"end_inst\n");
 my_free(_ALLOC_ID_, &extra);
 my_free(_ALLOC_ID_, &extra_pinnumber);
 my_free(_ALLOC_ID_, &template);
 my_free(_ALLOC_ID_, &numslots);
 my_free(_ALLOC_ID_, &format);
 my_free(_ALLOC_ID_, &name);
 my_free(_ALLOC_ID_, &token);
}

/* print verilog element if verilog_format is specified */
static void print_verilog_primitive(FILE *fd, int inst) /* netlist switch level primitives, 15112003 */
{
  int i=0, multip, tmp;
  const char *str_ptr;
  register int c, state=TOK_BEGIN, space;
  const char *lab;
  char *template=NULL,*format=NULL,*s=NULL, *name=NULL, *token=NULL;
  const char *value;
  size_t sizetok=0;
  size_t token_pos=0;
  int escape=0;
  int no_of_pins=0;
  int symbol = xctx->inst[inst].ptr;
  const char *fmt_attr = NULL;
  char *result = NULL;

  my_strdup(_ALLOC_ID_, &template,
      (xctx->inst[inst].ptr + xctx->sym)->templ);

  my_strdup(_ALLOC_ID_, &name,xctx->inst[inst].instname);
  if(!name) my_strdup(_ALLOC_ID_, &name, get_tok_value(template, "name", 0));

  fmt_attr = xctx->format ? xctx->format : "verilog_format";
  /* allow format string override in instance */
  my_strdup(_ALLOC_ID_, &format, get_tok_value(xctx->inst[inst].prop_ptr, fmt_attr, 2));
  /* get netlist format rule from symbol */
  if(!xctx->tok_size)
    my_strdup(_ALLOC_ID_, &format, get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr, fmt_attr, 2));
  /* allow format string override in instance */
  if(!xctx->tok_size && strcmp(fmt_attr, "verilog_format") )
    my_strdup(_ALLOC_ID_, &format, get_tok_value(xctx->inst[inst].prop_ptr, "verilog_format", 2));
  /* get netlist format rule from symbol */
  if(!xctx->tok_size && strcmp(fmt_attr, "verilog_format"))
     my_strdup(_ALLOC_ID_, &format, get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr, "verilog_format", 2));
  if((name==NULL) || (format==NULL) ) {
    my_free(_ALLOC_ID_, &template);
    my_free(_ALLOC_ID_, &name);
    my_free(_ALLOC_ID_, &format);
    return; /*do no netlist unwanted insts(no format) */
  }
  no_of_pins= (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER];
  s=format;
  dbg(1, "print_verilog_primitive(): name=%s, format=%s xctx->netlist_count=%d\n",name,format, xctx->netlist_count);

  fprintf(fd, "---- start primitive ");
  lab=expandlabel(name, &tmp);
  fprintf(fd, "%d\n",tmp);
  /* begin parsing format string */
  while(1)
  {
   c=*s++;
   if(c=='\\') {
     escape=1;
     c=*s++;
   }
   else escape=0;
   if(c=='\n' && escape ) c=*s++; /* 20171030 eat escaped newlines */
   space=SPACE(c);
   if( state==TOK_BEGIN && (c=='@' || c=='%') && !escape ) state=TOK_TOKEN;
   else if(state==TOK_TOKEN && token_pos > 1 &&
      (
        ( (space  || c == '%' || c == '@') && !escape ) || 
        ( (!space && c != '%' && c != '@') && escape  )
      )
     ) { 
     state=TOK_SEP;
   }

   STR_ALLOC(&token, token_pos, &sizetok);
   if(state==TOK_TOKEN) {
      token[token_pos++]=(char)c;
   }
   else if(state==TOK_SEP)                    /* got a token */
   {
    token[token_pos]='\0';
    token_pos=0;

    value = get_tok_value(xctx->inst[inst].prop_ptr, token+1, 0);
    /* xctx->tok_size==0 indicates that token(+1) does not exist in instance attributes */
    if(!xctx->tok_size)
    value=get_tok_value(template, token+1, 0);
    if(!xctx->tok_size && token[0] =='%') {
      my_mstrcat(_ALLOC_ID_, &result, token + 1, NULL);
    } else if(value && value[0]!='\0') {
       /* instance names (name) and node labels (lab) go thru the expandlabel function. */
       /*if something else must be parsed, put an if here! */

     if(!(strcmp(token+1,"name"))) {
       if( (lab=expandlabel(value, &tmp)) != NULL)
          my_mstrcat(_ALLOC_ID_, &result, "----name(", lab, ")", NULL);
       else
          my_mstrcat(_ALLOC_ID_, &result, value, NULL);
     }
     else if(!(strcmp(token+1,"lab"))) {
       if( (lab=expandlabel(value, &tmp)) != NULL)
          my_mstrcat(_ALLOC_ID_, &result, "----pin(", lab, ")", NULL);
       else
          my_mstrcat(_ALLOC_ID_, &result, value, NULL);
     }
     else my_mstrcat(_ALLOC_ID_, &result, value, NULL);
    }
    else if(strcmp(token,"@symref")==0)
    {
      const char *s = get_sym_name(inst, 9999, 1);
      my_mstrcat(_ALLOC_ID_, &result, s, NULL);
    }
    else if(strcmp(token,"@symname")==0) /* of course symname must not be present  */
                                         /* in hash table */
    {
      const char *s = sanitize(translate(inst, get_sym_name(inst, 0, 0)));
      my_mstrcat(_ALLOC_ID_, &result, s, NULL);
    }
    else if (strcmp(token,"@symname_ext")==0) 
    {
      const char *s = sanitize(translate(inst, get_sym_name(inst, 0, 1)));
      my_mstrcat(_ALLOC_ID_, &result, s, NULL);
    }
    else if(strcmp(token,"@schname_ext")==0) /* of course schname must not be present  */
                                         /* in hash table */
    {
      my_mstrcat(_ALLOC_ID_, &result, xctx->current_name, NULL);
    }
    else if(strcmp(token,"@schname")==0) /* of course schname must not be present  */
                                         /* in hash table */
    {
      my_mstrcat(_ALLOC_ID_, &result, get_cell(xctx->current_name, 0), NULL);
    }
    else if(strcmp(token,"@topschname")==0) /* of course topschname must not be present in attributes */
    {
      const char *topsch;
      topsch = get_trailing_path(xctx->sch[0], 0, 1);
      my_mstrcat(_ALLOC_ID_, &result, topsch, NULL);
    }
    else if(strcmp(token,"@pinlist")==0) /* of course pinlist must not be present  */
                                         /* in hash table. print multiplicity */
    {                                    /* and node number: m1 n1 m2 n2 .... */
     Int_hashtable table = {NULL, 0};
     int first = 1;
     int_hash_init(&table, 37);
     for(i=0;i<no_of_pins; ++i) {
       if(strboolcmp(get_tok_value(xctx->sym[symbol].rect[PINLAYER][i].prop_ptr,"verilog_ignore",0), "true")) {
         const char *name = get_tok_value(xctx->sym[symbol].rect[PINLAYER][i].prop_ptr,"name",0);
         if(!int_hash_lookup(&table, name, 1, XINSERT_NOREPLACE)) {
           if(!first) my_mstrcat(_ALLOC_ID_, &result, " , ", NULL);
           str_ptr =  net_name(inst,i, &multip, 0, 1);
           my_mstrcat(_ALLOC_ID_, &result, "----pin(", str_ptr, ") ", NULL);
           first = 0;
         }
       }
     }
     int_hash_free(&table);
    }
    else if(token[0]=='@' && token[1]=='@') {    /* recognize single pins 15112003 */
     for(i=0;i<no_of_pins; ++i) {
      char *prop = (xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][i].prop_ptr;
      if(!strcmp( get_tok_value(prop,"name",0), token+2)) {
        str_ptr =  net_name(inst,i, &multip, 0, 1);
        my_mstrcat(_ALLOC_ID_, &result, "----pin(", str_ptr, ") ", NULL);
        break;
      }
     }
    }

    /* reference by pin number instead of pin name, allows faster lookup of the attached net name 
     * @#0, @#1:net_name, @#2:name, ... */
    else if(token[0]=='@' && token[1]=='#') {
      int n;
      char *pin_attr = NULL;
      char *pin_num_or_name = NULL;
 
      get_pin_and_attr(token, &pin_num_or_name, &pin_attr);
      n = get_inst_pin_number(inst, pin_num_or_name);
      if(n>=0  && pin_attr[0] && n < (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER]) {
        char *pin_attr_value = NULL;
        int is_net_name = !strcmp(pin_attr, "net_name");
        /* get pin_attr value from instance: "pinnumber(ENABLE)=5" --> return 5, attr "pinnumber" of pin "ENABLE"
         *                                   "pinnumber(3)=6       --> return 6, attr "pinnumber" of 4th pin */
        if(!is_net_name) {
          pin_attr_value = get_pin_attr_from_inst(inst, n, pin_attr);
          /* get pin_attr from instance pin attribute string */
          if(!pin_attr_value) {
           my_strdup(_ALLOC_ID_, &pin_attr_value,
              get_tok_value((xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][n].prop_ptr, pin_attr, 0));
          }
        }
        /* @#n:net_name attribute (n = pin number or name) will translate to net name attached  to pin */
        if(!pin_attr_value && is_net_name) {
          prepare_netlist_structs(0);
          my_strdup(_ALLOC_ID_, &pin_attr_value,
               xctx->inst[inst].node && xctx->inst[inst].node[n] ? xctx->inst[inst].node[n] : "?");
        }
        if(!pin_attr_value ) my_strdup(_ALLOC_ID_, &pin_attr_value, "--UNDEF--");
        value = pin_attr_value;
        /* recognize slotted devices: instname = "U3:3", value = "a:b:c:d" --> value = "c" */
        if(value[0] && !strcmp(pin_attr, "pinnumber") ) {
          char *ss;
          int slot;
          char *tmpstr = NULL;
          tmpstr = my_malloc(_ALLOC_ID_, sizeof(xctx->inst[inst].instname));
          if( (ss=strchr(xctx->inst[inst].instname, ':')) ) {
            sscanf(ss+1, "%s", tmpstr);
            if(isonlydigit(tmpstr)) {
              slot = atoi(tmpstr);
              if(strstr(value,":")) value = find_nth(value, ":", "", 0, slot);
            }
          }
          my_free(_ALLOC_ID_, &tmpstr);
        }
        my_mstrcat(_ALLOC_ID_, &result, value, NULL);
        my_free(_ALLOC_ID_, &pin_attr_value);
      }
      else if(n>=0  && n < (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER]) {
        const char *si;
        char *prop = (xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][n].prop_ptr;
        si  = get_tok_value(prop, "verilog_ignore",0);
        if(strboolcmp(si, "true")) {
          str_ptr =  net_name(inst,n, &multip, 0, 1);
          my_mstrcat(_ALLOC_ID_, &result, "----pin(", str_ptr, ") ", NULL);
        }
      }
      my_free(_ALLOC_ID_, &pin_attr);
      my_free(_ALLOC_ID_, &pin_num_or_name);
    }

    else if(!strncmp(token,"@tcleval", 8)) {
      /* char tclcmd[strlen(token)+100] ; */
      size_t s;
      char *tclcmd=NULL;
      s = token_pos + strlen(name) + strlen(xctx->inst[inst].name) + 100;
      tclcmd = my_malloc(_ALLOC_ID_, s);
      Tcl_ResetResult(interp);
      my_snprintf(tclcmd, s, "tclpropeval {%s} {%s} {%s}", token, name, xctx->inst[inst].name);
      tcleval(tclcmd);
      my_mstrcat(_ALLOC_ID_, &result, tclresult(), NULL);
      my_free(_ALLOC_ID_, &tclcmd);
    }
    if(c!='%' && c!='@' && c!='\0') {
      char str[2];
      str[0] = (unsigned char) c;
      str[1] = '\0';
      my_mstrcat(_ALLOC_ID_, &result, str, NULL);
    }
    if(c == '@' || c == '%') s--;
    state=TOK_BEGIN;
   }
   else if(state==TOK_BEGIN && c!='\0')  {
     char str[2];
     str[0] = (unsigned char) c;
     str[1] = '\0';
     my_mstrcat(_ALLOC_ID_, &result, str, NULL);
   }
   if(c=='\0')
   {
    /* do one level of substitutions to resolve @params and equations*/
    if(result && strstr(result, "tcleval(")== result) {
      dbg(1, "print_verilog_primitive(): before translate() result=%s\n", result);
      my_strdup(_ALLOC_ID_, &result, translate(inst, result));
      dbg(1, "print_verilog_primitive(): after  translate() result=%s\n", result);
    }
    if(result) fprintf(fd, "%s", result);
    fputc('\n',fd);
    fprintf(fd, "---- end primitive\n");
    break ;
   }
  }
  my_free(_ALLOC_ID_, &result);
  my_free(_ALLOC_ID_, &template);
  my_free(_ALLOC_ID_, &format);
  my_free(_ALLOC_ID_, &name);
  my_free(_ALLOC_ID_, &token);
}

/* verilog module instantiation:
     cmos_inv
     #(
     .WN ( 1.5e-05 ) ,
     .WP ( 4.5e-05 ) ,
     .LLN ( 3e-06 ) ,
     .LLP ( 3e-06 )
     )
     Xinv (
      .A( AA ),
      .Z( Z )
     );
*/
void print_verilog_element(FILE *fd, int inst)
{
 int i=0, multip, tmp;
 const char *str_ptr;
 const char *lab;
 char *name=NULL, *symname = NULL;
 char  *generic_type=NULL;
 char *template=NULL, *verilogprefix = NULL, *s;
 int no_of_pins=0;
 int  tmp1 = 0;
 register int c, state=TOK_BEGIN, space;
 char *value=NULL,  *token=NULL, *extra = NULL, *v_extra = NULL;
 char *extra_ptr, *saveptr1, *extra_token;
 size_t sizetok=0, sizeval=0;
 size_t token_pos=0, value_pos=0;
 int quote=0;
 const char *fmt_attr = NULL;
 Int_hashtable table = {NULL, 0};
 const char *fmt;

 fmt_attr = xctx->format ? xctx->format : "verilog_format";

 /* allow format string override in instance */
 fmt = get_tok_value(xctx->inst[inst].prop_ptr, fmt_attr, 2);
 /* get netlist format rule from symbol */
 if(!xctx->tok_size)
   fmt = get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr, fmt_attr, 2);
 /* allow format string override in instance */
 if(!xctx->tok_size && strcmp(fmt_attr, "verilog_format") )
   fmt = get_tok_value(xctx->inst[inst].prop_ptr, "verilog_format", 2);
 /* get netlist format rule from symbol */
 if(!xctx->tok_size && strcmp(fmt_attr, "verilog_format"))
   fmt = get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr, "verilog_format", 2);

 if(fmt[0]) {
  print_verilog_primitive(fd, inst);
  return;
 }

 my_strdup(_ALLOC_ID_, &name,xctx->inst[inst].instname);
 if(!name) my_strdup(_ALLOC_ID_, &name, get_tok_value(template, "name", 0));
 if(name==NULL) {
   my_free(_ALLOC_ID_, &name);
   return;
 }
 /* verilog_extra is the list of additional nodes passed as attributes */
 my_strdup(_ALLOC_ID_, &v_extra, get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr, "verilog_extra", 0));
 /* extra is the list of attributes NOT to consider as instance parameters */
 my_strdup(_ALLOC_ID_, &extra, get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr, "extra", 0));
 my_strdup(_ALLOC_ID_, &verilogprefix, 
    get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr, "verilogprefix", 0));
 if(verilogprefix) {
   my_strdup(_ALLOC_ID_, &symname, verilogprefix);
   my_strcat(_ALLOC_ID_, &symname, get_sym_name(inst, 0, 0));
 } else {
   my_strdup(_ALLOC_ID_, &symname, get_sym_name(inst, 0, 0));
 }
 my_free(_ALLOC_ID_, &verilogprefix);
 my_strdup(_ALLOC_ID_, &template, (xctx->inst[inst].ptr + xctx->sym)->templ);
 no_of_pins= (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER];

 /* 20080915 use generic_type property to decide if some properties are strings, see later */
 my_strdup(_ALLOC_ID_, &generic_type, get_tok_value((xctx->inst[inst].ptr + xctx->sym)->prop_ptr,"generic_type",0));
 s=xctx->inst[inst].prop_ptr;
/* print instance  subckt */
 dbg(2, "print_verilog_element(): printing inst name & subcircuit name\n");
 fprintf(fd, "%s\n", sanitize(symname));
 my_free(_ALLOC_ID_, &symname);
 /* -------- print generics passed as properties */
 tmp=0;
 while(1)
 {
   if (s==NULL) break;
  c=*s++;
  if(c=='\\')
  {
    c=*s++;
  }
  space=SPACE(c);
  if( (state==TOK_BEGIN || state==TOK_ENDTOK) && !space && c != '=') state=TOK_TOKEN;
  else if( state==TOK_TOKEN && space) state=TOK_ENDTOK;
  else if( (state==TOK_TOKEN || state==TOK_ENDTOK) && c=='=') state=TOK_SEP;
  else if( state==TOK_SEP && !space) state=TOK_VALUE;
  else if( state==TOK_VALUE && space && !quote) state=TOK_END;

  STR_ALLOC(&value, value_pos, &sizeval);
  STR_ALLOC(&token, token_pos, &sizetok);
  if(state==TOK_TOKEN) token[token_pos++]=(char)c;
  else if(state==TOK_VALUE)
  {
    value[value_pos++]=(char)c;
  }
  else if(state==TOK_ENDTOK || state==TOK_SEP) {
    if(token_pos) {
      token[token_pos]='\0';
      token_pos=0;
    }
  } else if(state==TOK_END)
  {
   value[value_pos]='\0';
   value_pos=0;
   get_tok_value(template, token, 0);
   dbg(1, "token=%s, extra=%s\n", token, extra);
   if(strcmp(token, "name") && xctx->tok_size && (!extra || !strstr(extra, token))) {
     if(value[0] != '\0') /* token has a value */
     {
       if(strcmp(token,"spice_ignore") && strcmp(token,"vhdl_ignore") && strcmp(token,"tedax_ignore")) {
         if(tmp == 0) {
           fprintf(fd, "#(\n---- start parameters\n");
           ++tmp;
           tmp1=0;
         }
         /* skip attributes of type time (delay="20 ns") that have VHDL syntax */
         if( !generic_type || strcmp(get_tok_value(generic_type,token, 0), "time")  ) {
           if(tmp1) fprintf(fd, " ,\n");
           if( generic_type && !strcmp(get_tok_value(generic_type,token, 0), "string")  ) {
             fprintf(fd, "  .%s ( \"%s\" )", token, value);
           } else {
             fprintf(fd, "  .%s ( %s )", token, value);
           }
           tmp1=1;
         }
       }
     }
   }
   state=TOK_BEGIN;
  }
  if(c=='\0')  /* end string */
  {
   break ;
  }
 }
 if(tmp) fprintf(fd, "\n---- end parameters\n)\n");

 /* -------- end print generics passed as properties */

/* print instance name */
 if( (lab = expandlabel(name, &tmp)) != NULL)
   fprintf(fd, "---- instance %s (\n", lab );
 else  /*  name in some strange format, probably an error */
   fprintf(fd, "---- instance %s (\n", name );

  dbg(2, "print_verilog_element(): printing port maps \n");
 /* print port map */
 tmp=0;
 int_hash_init(&table, 37);
 for(i=0;i<no_of_pins; ++i)
 {
   xSymbol *ptr = xctx->inst[inst].ptr + xctx->sym;
   if(strboolcmp(get_tok_value(ptr->rect[PINLAYER][i].prop_ptr,"verilog_ignore",0), "true")) {
     const char *name = get_tok_value(ptr->rect[PINLAYER][i].prop_ptr, "name", 0);
     if(!int_hash_lookup(&table, name, 1, XINSERT_NOREPLACE)) {
       if( (str_ptr =  net_name(inst,i, &multip, 0, 1)) )
       {
         if(tmp) fprintf(fd,"\n");
         fprintf(fd, "  ?%d %s %s ", multip, get_tok_value(ptr->rect[PINLAYER][i].prop_ptr,"name",0), str_ptr);
         tmp=1;
       }
     }
   }
 }
 int_hash_free(&table);
 if(v_extra) {
   const char *val;
   for(extra_ptr = v_extra; ; extra_ptr=NULL) {
     extra_token=my_strtok_r(extra_ptr, " ", "", 0, &saveptr1);
     if(!extra_token) break;

     val = get_tok_value(xctx->inst[inst].prop_ptr, extra_token, 0);
     if(!val[0]) val = get_tok_value(template, extra_token, 0);
     if(tmp) fprintf(fd,"\n");
     fprintf(fd, "  ?%d %s %s ", 1, extra_token, val);
     tmp = 1;
   }
 }


 fprintf(fd, "\n);\n\n");
 dbg(2, "print_verilog_element(): ------- end ------ \n");
 my_free(_ALLOC_ID_, &name);
 my_free(_ALLOC_ID_, &generic_type);
 my_free(_ALLOC_ID_, &template);
 my_free(_ALLOC_ID_, &value);
 my_free(_ALLOC_ID_, &token);
 my_free(_ALLOC_ID_, &extra);
 my_free(_ALLOC_ID_, &v_extra);
}


const char *net_name(int i, int j, int *multip, int hash_prefix_unnamed_net, int erc)
{
 int tmp, k;
 char errstr[2048];
 char unconn[50];
 char str_node[40]; /* 20161122 overflow safe */

 xSymbol *sym = xctx->inst[i].ptr + xctx->sym;
 int no_of_pins= sym->rects[PINLAYER];
 char *pinname = NULL;


 /* if merging a ngspice_probe.sym element it contains a @@p token, 
  * so translate calls net_name, but we are placing the merged objects, 
  * no net name is assigned yet */
 if(!xctx->inst[i].node) {
   return expandlabel("", multip);
 }
 if(xctx->inst[i].node && xctx->inst[i].node[j] == NULL)
 {
   my_strdup(_ALLOC_ID_, &pinname, get_tok_value( sym->rect[PINLAYER][j].prop_ptr,"name",0));
   /* before reporting unconnected pin try to locate duplicated pin and use it if found */
   for(k = 0; k < no_of_pins; ++k) {
     const char *duplicated_pinname;
     if(k == j) continue;
     duplicated_pinname =  get_tok_value( sym->rect[PINLAYER][k].prop_ptr,"name",0);
     if(!strcmp(duplicated_pinname , pinname)) {
       my_strdup(_ALLOC_ID_, &pinname, duplicated_pinname);
       j = k;
       break;
     }
   }
 }
 /* can not merge this if() with previous one, since j may be changed here */
 if(xctx->inst[i].node && xctx->inst[i].node[j] == NULL)
 {
   expandlabel(pinname, multip);
   if(pinname) my_free(_ALLOC_ID_, &pinname);
   if(erc) {
     my_snprintf(errstr, S(errstr), "Warning: unconnected pin,  Inst idx: %d, Pin idx: %d  Inst:%s\n",
                 i, j, xctx->inst[i].instname ) ;
     statusmsg(errstr,2);
     if(!xctx->netlist_count && xctx->netlist_type != CAD_TEDAX_NETLIST) {
       xctx->inst[i].color = -PINLAYER;
       xctx->hilight_nets=1;
     }
   }
   if(*multip <= 1) 
     my_snprintf(unconn, S(unconn), "__UNCONNECTED_PIN__%d", xctx->netlist_unconn_cnt++);
   else
     my_snprintf(unconn, S(unconn), "__UNCONNECTED_PIN__%d[%d:0]", xctx->netlist_unconn_cnt++, *multip - 1);
   return expandlabel(unconn, &tmp);
 }
 else { /* xctx->inst[i].node[j] not NULL */
   if(pinname) my_free(_ALLOC_ID_, &pinname);
   if((xctx->inst[i].node[j])[0] == '#') /* unnamed net */
   {
     /* get unnamed node multiplicity ( minimum multip found in circuit) */
     *multip = get_unnamed_node(3, 0, atoi((xctx->inst[i].node[j])+4) );
     dbg(2, "net_name(): node = %s  n=%d multip=%d\n",
     xctx->inst[i].node[j], atoi(xctx->inst[i].node[j]), *multip);
     if(hash_prefix_unnamed_net) {
       if(*multip>1)   /* unnamed is a bus */
         my_snprintf(str_node, S(str_node), "%s[%d:0]", (xctx->inst[i].node[j]), *multip-1);
       else
         my_snprintf(str_node, S(str_node), "%s", (xctx->inst[i].node[j]) );
     } else {
       if(*multip>1)   /* unnamed is a bus */
         my_snprintf(str_node, S(str_node), "%s[%d:0]", (xctx->inst[i].node[j])+1, *multip-1);
       else
         my_snprintf(str_node, S(str_node), "%s", (xctx->inst[i].node[j])+1 );
     }
     expandlabel(
        get_tok_value( (xctx->inst[i].ptr + xctx->sym)->rect[PINLAYER][j].prop_ptr,"name",0), multip);
     return expandlabel(str_node, &tmp);
   }
   else
   {
     expandlabel(
        get_tok_value( (xctx->inst[i].ptr + xctx->sym)->rect[PINLAYER][j].prop_ptr,"name",0), multip);
     return expandlabel(xctx->inst[i].node[j], &tmp);
   }
 }
}

int isonlydigit(const char *s)
{
  char c;
  int res = 1;
  int first = 1;
  if(s == NULL || *s == '\0') return 0;
  while( (c = *s++) ) {
    if(first == 1) {
      first = 0;
      if(c == '-') {
        continue;
      }
    }
    if(c < '0' || c > '9') {
      res = 0;
      break;
    }
  }
  return res;
}


/* remove leading and trailing characters specified in 'sep' */
char *trim_chars(const char *str, const char *sep)
{
  static char *result = NULL;
  static size_t result_size = 0;
  size_t len;
  char *ptr;
  char *last;

  if(str == NULL) {
    my_free(_ALLOC_ID_, &result);
    result_size = 0;
    return NULL;
  }
  len = strlen(str) + 1;
  /* allocate storage for result */
  if(len > result_size) {
    result_size = len + CADCHUNKALLOC;
    my_realloc(_ALLOC_ID_, &result, result_size);
  }
  memcpy(result, str, len);
  if(*str == '\0')return result;
  ptr = result;
  while (*ptr) {
    if(!strchr(sep, *ptr)) break;
    ptr++;
  }
  if(*ptr == '\0') return ptr;
  last = ptr + strlen(ptr) -1;
  while(strchr(sep, *last) && last > result) {
    last--;
  }
  last[1] = '\0';
  return ptr;
}

/* find nth field in str separated by sep. 1st field is position 1
 * separators inside quotes are not considered as field separators 
 * if keep_quote == 1 keep quoting characters  and backslashes in returned field
 * if keep_quote == 4 same as above but remove surrounding "..."
 * find_nth("aaa,bbb,ccc,ddd", ",", 0, 2)  --> bbb
 * find_nth("aaa, \"bbb, \" ccc\" , ddd", " ,", "\"", 0, 2)  --> bbb, " ccc
 * find_nth("aaa, \"bbb, \" ccc\" , ddd", " ,", "\"", 1, 2)  --> "bbb, \" ccc"
 * find_nth("aaa, \"bbb, \" ccc\" , ddd", " ,", "\"", 4, 2)  --> bbb, \" ccc
 */
char *find_nth(const char *str, const char *sep, const char *quote, int keep_quote, int n)
{
  static char *result=NULL; /* safe to keep even with multiple schematic windows */
  static size_t result_size = 0; /* safe to keep even with multiple schematic windows */
  int i, q = 0, e = 0; /* e: escape */
  int result_pos;
  size_t len;
  int count = 0, first_nonsep=1;

  /* clean up static data */
  if(!str) {
    my_free(_ALLOC_ID_, &result);
    result_size = 0;
    return NULL;
  }
  /* allocate storage for result */
  len = strlen(str) + 1;
  if(len > result_size) {
    result_size = len + CADCHUNKALLOC;
    my_realloc(_ALLOC_ID_, &result, result_size);
  }

  result_pos = 0;
  for(i = 0; str[i]; i++) {
    if(!e && strchr(quote, str[i])) {
      q = !q;
      if(keep_quote != 1) {
        continue;
      }
    }
    if(!e && str[i] =='\\') { /* only recognize escape if there are some quoting chars */
      e = 1;
      continue;
    }
    if(!e && !q && strchr(sep, str[i])) {
      first_nonsep = 1;
      if(count == n) { /* first == 1 --> separators at beginning are not preceded by a field */
        break; /* we have found the 'count'th field, return. */
      }
    } else {
      if(first_nonsep) count++; /* found a new field */
      first_nonsep=0;
      if(count == n) {
        if(e == 1 && keep_quote) result[result_pos++] = '\\';
        result[result_pos++] = str[i]; /* if field matches requested one store result */
      }
    }
    e = 0;
  }
  result[result_pos++] = '\0';
  return result;
}

/* given a token like @#pin:attr get value of pin attribute 'attr'
 * if only @#pin is given return name of net attached to 'pin'
 * caller should free returned string */
static char *get_pin_attr(const char *token, int inst, int s_pnetname)
{
  char *value = NULL;
  int n;
  char *pin_attr = NULL;
  char *pin_num_or_name = NULL;

  if(xctx->inst[inst].ptr < 0) return NULL;
  get_pin_and_attr(token, &pin_num_or_name, &pin_attr);
  n = get_inst_pin_number(inst, pin_num_or_name);
  if(n>=0  && pin_attr[0] && n < (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER]) {
    char *pin_attr_value = NULL;
    int is_net_name = !strcmp(pin_attr, "net_name");
    /* get pin_attr value from instance: "pinnumber(ENABLE)=5" --> return 5, attr "pinnumber" of pin "ENABLE"
     *                                   "pinnumber(3)=6       --> return 6, attr "pinnumber" of 4th pin */
    if(!is_net_name) {
      pin_attr_value = get_pin_attr_from_inst(inst, n, pin_attr);
      /* get pin_attr from instance pin attribute string */
      if(!pin_attr_value) {
       my_strdup(_ALLOC_ID_, &pin_attr_value,
          get_tok_value((xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][n].prop_ptr, pin_attr, 0));
      }
    }
    /* @#n:net_name attribute (n = pin number or name) will translate to net name attached  to pin
     * if 'net_name=true' attribute is set in instance or symbol */
    if(!pin_attr_value && is_net_name) {
      char *instprop = xctx->inst[inst].prop_ptr;
      char *symprop = (xctx->inst[inst].ptr + xctx->sym)->prop_ptr;
      if(s_pnetname && (!strboolcmp(get_tok_value(symprop, "net_name", 0), "true") ||
         !strboolcmp(get_tok_value(instprop, "net_name", 0), "true"))) {
         prepare_netlist_structs(0);
         my_strdup2(_ALLOC_ID_, &pin_attr_value,
              xctx->inst[inst].node && xctx->inst[inst].node[n] ? xctx->inst[inst].node[n] : "?");
      /* do not show net_name: set to empty string */
      } else {
         my_strdup2(_ALLOC_ID_, &pin_attr_value, "");
      }
    }

    /* @#n:resolved_net attribute (n = pin number or name) will translate to hierarchy-resolved net */
    if(!pin_attr_value && !strcmp(pin_attr, "resolved_net")) {
      char *rn = NULL;
      char *instprop = xctx->inst[inst].prop_ptr;
      char *symprop = (xctx->inst[inst].ptr + xctx->sym)->prop_ptr;
      dbg(1, "translate(): resolved_net: %s, symbol %s\n", xctx->current_name, xctx->inst[inst].name);
      if(s_pnetname && (!strboolcmp(get_tok_value(symprop, "net_name", 0), "true") ||
         !strboolcmp(get_tok_value(instprop, "net_name", 0), "true"))) {
        prepare_netlist_structs(0);
        if(xctx->inst[inst].node && xctx->inst[inst].node[n]) {
          rn = resolved_net(xctx->inst[inst].node[n]);
        }
        my_strdup2(_ALLOC_ID_, &pin_attr_value, rn ? rn : "?");
        if(rn) my_free(_ALLOC_ID_, &rn);
      } else {
         my_strdup2(_ALLOC_ID_, &pin_attr_value, "");
      }
    }

    if(!pin_attr_value ) my_strdup(_ALLOC_ID_, &pin_attr_value, "--UNDEF--");
    my_strdup2(_ALLOC_ID_, &value, pin_attr_value);
    /* recognize slotted devices: instname = "U3:3", value = "a:b:c:d" --> value = "c" */
    if(pin_attr_value[0] && !strcmp(pin_attr, "pinnumber") ) {
      char *ss;
      int slot;
      char *tmpstr = NULL;
      if( xctx->inst[inst].instname && (ss=strchr(xctx->inst[inst].instname, ':')) ) {
        tmpstr = my_malloc(_ALLOC_ID_, sizeof(xctx->inst[inst].instname));
        sscanf(ss+1, "%s", tmpstr);
        if(isonlydigit(tmpstr)) {
          slot = atoi(tmpstr);
          if(strstr(value,":")) my_strdup2(_ALLOC_ID_, &value, find_nth(value, ":", "", 0, slot));
        }
        my_free(_ALLOC_ID_, &tmpstr);
      }
    }
    my_free(_ALLOC_ID_, &pin_attr_value);
    my_free(_ALLOC_ID_, &pin_attr_value);
  }
  else if(n>=0  && n < (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER]) {
    const char *str_ptr=NULL;
    int multip;
    size_t tmp;
    str_ptr =  net_name(inst,n, &multip, 0, 1);
    tmp = strlen(str_ptr) +100 ; /* always make room for some extra chars 
                                  * so 1-char writes to result do not need reallocs */

    value = my_malloc(_ALLOC_ID_, tmp);
    my_snprintf(value, tmp, "?%d %s ", multip, str_ptr);
  }
  my_free(_ALLOC_ID_, &pin_attr);
  my_free(_ALLOC_ID_, &pin_num_or_name);
  return value;
}

/* substitute given tokens in a string with their corresponding values */
/* ex.: name=@name w=@w l=@l ---> name=m112 w=3e-6 l=0.8e-6 */
/* if s==NULL return emty string */
const char *translate(int inst, const char* s)
{
 static const char *empty="";
 static char *translated_tok = NULL;
 static char *result=NULL; /* safe to keep even with multiple schematics */
 size_t size=0;
 size_t tmp;
 register int c, state=TOK_BEGIN, space;
 char *token=NULL;
 const char *tmp_sym_name;
 size_t sizetok=0;
 size_t result_pos=0, token_pos=0;
 struct stat time_buf;
 struct tm *tm;
 char file_name[PATH_MAX];
 const char *value;
 int escape=0;
 char date[200];
 int sp_prefix;
 int s_pnetname;
 int level;
 Lcc *lcc;
 char *value1 = NULL;
 int sim_is_xyce;
 char *instname = NULL;

 s_pnetname = tclgetboolvar("show_pin_net_names");
 sp_prefix = tclgetboolvar("spiceprefix");
 if(!s || !xctx || !xctx->inst) {
   my_free(_ALLOC_ID_, &result);
   my_free(_ALLOC_ID_, &translated_tok);
   return empty;
 }
 instname = xctx->inst[inst].instname ? xctx->inst[inst].instname : "";
 sim_is_xyce = tcleval("sim_is_xyce")[0] == '1' ? 1 : 0;
 level = xctx->currsch;
 lcc = xctx->hier_attr;
 size=CADCHUNKALLOC;
 my_realloc(_ALLOC_ID_, &result,size);
 result[0]='\0';

 dbg(1, "translate(): substituting props in <%s>, instance <%s>\n", s ? s : "NULL" , instname);

 while(1)
 {
  c=*s++;
  if(c=='\\') {
    escape=1;
    c=*s++; /* do not remove: breaks translation of format strings in netlists (escaping %) */
  }
  else escape=0;
  space=SPACE(c);
  if( state==TOK_BEGIN && (c=='@' || c=='%' ) && !escape  ) state=TOK_TOKEN; /* 20161210 escape */
  else if(state==TOK_TOKEN && token_pos > 1 &&
     (
       ( (space  || c == '%' || c == '@') && !escape ) ||
       ( (!space && c != '%' && c != '@') && escape  )
     )                          
    ) state=TOK_SEP;

  STR_ALLOC(&result, result_pos, &size);
  STR_ALLOC(&token, token_pos, &sizetok);
  if(state==TOK_TOKEN) token[token_pos++]=(char)c;
  else if(state==TOK_SEP)
  {
   token[token_pos]='\0';
   if(!strcmp(token, "@name")) {
     tmp = strlen(instname);
     STR_ALLOC(&result, tmp + result_pos, &size);
     memcpy(result+result_pos, instname, tmp+1);
     result_pos+=tmp;
   } else if(strcmp(token,"@symref")==0) {
    tmp_sym_name = get_sym_name(inst, 9999, 1);
    tmp_sym_name=tmp_sym_name ? tmp_sym_name : "";
    tmp=strlen(tmp_sym_name);
    STR_ALLOC(&result, tmp + result_pos, &size);
    memcpy(result+result_pos,tmp_sym_name, tmp+1);
    result_pos+=tmp;
   } else if(strcmp(token,"@symname")==0) {
    tmp_sym_name = get_sym_name(inst, 0, 0);
    tmp_sym_name=tmp_sym_name ? tmp_sym_name : "";
    tmp=strlen(tmp_sym_name);
    STR_ALLOC(&result, tmp + result_pos, &size);
    memcpy(result+result_pos,tmp_sym_name, tmp+1);
    result_pos+=tmp;
   } else if(strcmp(token,"@path")==0) {
    const char *path = xctx->sch_path[xctx->currsch] + 1;
    tmp=strlen(path);
    STR_ALLOC(&result, tmp + result_pos, &size);
    memcpy(result+result_pos, path, tmp+1);
    result_pos+=tmp;
   } else if(strcmp(token,"@symname_ext")==0) {
    tmp_sym_name = get_sym_name(inst, 0, 1);
    tmp_sym_name=tmp_sym_name ? tmp_sym_name : "";
    tmp=strlen(tmp_sym_name);
    STR_ALLOC(&result, tmp + result_pos, &size);
    memcpy(result+result_pos,tmp_sym_name, tmp+1);
    result_pos+=tmp;
   /* recognize single pins 15112003 */
   } else if(token[0]=='@' && token[1]=='@' && xctx->inst[inst].ptr >= 0) {
     int i, multip;
     int no_of_pins= (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER];
     prepare_netlist_structs(0);
     for(i=0;i<no_of_pins; ++i) {
       char *prop = (xctx->inst[inst].ptr + xctx->sym)->rect[PINLAYER][i].prop_ptr;
       if (!strcmp( get_tok_value(prop,"name",0), token+2)) {
         if(strboolcmp(get_tok_value(prop,"spice_ignore",0), "true")) {
           const char *str_ptr =  net_name(inst,i, &multip, 0, 0);
           tmp = strlen(str_ptr) +100 ;
           STR_ALLOC(&result, tmp + result_pos, &size);
           result_pos += my_snprintf(result + result_pos, tmp, "%s", str_ptr);
         }
         break;
       }
     }
   } else if(token[0]=='@' && token[1]=='#') {
     value = get_pin_attr(token, inst, s_pnetname);
     if(value) {
       tmp=strlen(value);
       STR_ALLOC(&result, tmp + result_pos, &size);
       memcpy(result+result_pos, value, tmp+1);
       result_pos+=tmp;
       my_free(_ALLOC_ID_, &value);
     }
   } else if(strcmp(token,"@sch_last_modified")==0 && xctx->inst[inst].ptr >= 0) {

    get_sch_from_sym(file_name, xctx->inst[inst].ptr + xctx->sym, inst, 0);
    if(!stat(file_name , &time_buf)) {
      tm=localtime(&(time_buf.st_mtime) );
      tmp=strftime(date, sizeof(date), "%Y-%m-%d  %H:%M:%S", tm);
      STR_ALLOC(&result, tmp + result_pos, &size);
      memcpy(result+result_pos, date, tmp+1);
      result_pos+=tmp;
    }
   } else if(strcmp(token,"@sym_last_modified")==0) {
    my_strncpy(file_name, abs_sym_path(tcl_hook2(xctx->inst[inst].name), ""), S(file_name));
    if(!stat(file_name , &time_buf)) {
      tm=localtime(&(time_buf.st_mtime) );
      tmp=strftime(date, sizeof(date), "%Y-%m-%d  %H:%M:%S", tm);
      STR_ALLOC(&result, tmp + result_pos, &size);
      memcpy(result+result_pos, date, tmp+1);
      result_pos+=tmp;
    }
   } else if(strcmp(token,"@time_last_modified")==0) {
    my_strncpy(file_name, abs_sym_path(xctx->sch[xctx->currsch], ""), S(file_name));
    if(!stat(file_name , &time_buf)) {
      tm=localtime(&(time_buf.st_mtime) );
      tmp=strftime(date, sizeof(date), "%Y-%m-%d  %H:%M:%S", tm);
      STR_ALLOC(&result, tmp + result_pos, &size);
      memcpy(result+result_pos, date, tmp+1);
      result_pos+=tmp;
    }
   } else if(strcmp(token,"@schname_ext")==0) {
     /* tmp=strlen(xctx->sch[xctx->currsch]);*/
     tmp = strlen(xctx->current_name);
     STR_ALLOC(&result, tmp + result_pos, &size);
     /* memcpy(result+result_pos,xctx->sch[xctx->currsch], tmp+1); */
     memcpy(result+result_pos, xctx->current_name, tmp+1);
     result_pos+=tmp;
   } else if(strcmp(token,"@schname")==0) {
     const char *schname = get_cell(xctx->current_name, 0);
     tmp = strlen(schname);
     STR_ALLOC(&result, tmp + result_pos, &size);
     memcpy(result+result_pos, schname, tmp+1);
     result_pos+=tmp;
   } else if(strcmp(token,"@topschname")==0)  {
      const char *topsch;
      topsch = get_trailing_path(xctx->sch[0], 0, 1);
      tmp = strlen(topsch);
      STR_ALLOC(&result, tmp + result_pos, &size);
      memcpy(result+result_pos, topsch, tmp+1);
      result_pos+=tmp;
   } else if(strcmp(token,"@prop_ptr")==0 && xctx->inst[inst].prop_ptr) {
     tmp=strlen(xctx->inst[inst].prop_ptr);
     STR_ALLOC(&result, tmp + result_pos, &size);
     memcpy(result+result_pos,xctx->inst[inst].prop_ptr, tmp+1);
     result_pos+=tmp;
   }
   else if(strcmp(token,"@spice_get_voltage")==0 && xctx->inst[inst].ptr >= 0)
   {
     int start_level; /* hierarchy level where waves were loaded */
     int live = tclgetboolvar("live_cursor2_backannotate");
     if(live && (start_level = sch_waves_loaded()) >= 0 && xctx->raw->annot_p>=0) {
       int multip;
       int no_of_pins= (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER];
       if(no_of_pins == 1) {
         char *fqnet = NULL;
         const char *path =  xctx->sch_path[xctx->currsch] + 1;
         char *net = NULL;
         size_t len;
         int idx;
         double val;
         const char *valstr;
         if(path) {
           /* skip path components that are above the level where raw file was loaded */
           prepare_netlist_structs(0);
           if(xctx->inst[inst].lab) {
             my_strdup2(_ALLOC_ID_, &net, expandlabel(xctx->inst[inst].lab, &multip));
           }
           if(net == NULL || net[0] == '\0') {
             my_strdup2(_ALLOC_ID_, &net, net_name(inst, 0, &multip, 0, 0));
           }
           if(multip == 1 && net && net[0]) {
             char *rn;
             dbg(1, "translate() @spice_get_voltage: inst=%s\n", instname);
             dbg(1, "                                net=%s\n", net);
             rn = resolved_net(net);
             if(rn) {
               my_strdup2(_ALLOC_ID_, &fqnet, rn);
               if(rn) my_free(_ALLOC_ID_, &rn);
               strtolower(fqnet);
               dbg(1, "translate() @spice_get_voltage: fqnet=%s start_level=%d\n", fqnet, start_level);
               idx = get_raw_index(fqnet);
               if(idx >= 0) {
                 val = xctx->raw->cursor_b_val[idx];
               }
               if(idx < 0) {
                 valstr = "";
                 xctx->tok_size = 0;
                 len = 0;
               } else {
                 valstr = dtoa_eng(val);
                 len = xctx->tok_size;
               }
               if(len) {
                 STR_ALLOC(&result, len + result_pos, &size);
                 memcpy(result+result_pos, valstr, len+1);
                 result_pos += len;
               }
               dbg(1, "inst %d, net=%s, fqnet=%s idx=%d valstr=%s\n", inst,  net, fqnet, idx, valstr);
               if(fqnet) my_free(_ALLOC_ID_, &fqnet);
             }
           }
           if(net) my_free(_ALLOC_ID_, &net);
         } 
       }
     }
   }
   else if(strncmp(token,"@spice_get_voltage(", 19)==0 )
   {
     int start_level; /* hierarchy level where waves were loaded */
     int live = tclgetboolvar("live_cursor2_backannotate");
     dbg(1, "--> %s\n", token);
     if(live && (start_level = sch_waves_loaded()) >= 0 && xctx->raw->annot_p>=0) {
       char *fqnet = NULL;
       const char *path =  xctx->sch_path[xctx->currsch] + 1;
       char *net = NULL;
       char *global_net;
       size_t len;
       int idx, n, multip;
       double val;
       const char *valstr;
       tmp = strlen(token) + 1;
       if(path) {
         int skip = 0;
         /* skip path components that are above the level where raw file was loaded */
         while(*path && skip < start_level) {
           if(*path == '.') skip++;
           ++path;
         }
         net = my_malloc(_ALLOC_ID_, tmp);
         n = sscanf(token + 19, "%[^)]", net);
         expandlabel(net, &multip);
         if(n == 1 && multip == 1) {
           len = strlen(path) + strlen(instname) + strlen(net) + 2;
           dbg(1, "net=%s\n", net);
           fqnet = my_malloc(_ALLOC_ID_, len);


           global_net = strrchr(net, '.');
           if(global_net == NULL) global_net = net;
           else global_net++;

           if(record_global_node(3, NULL, global_net)) {
             strtolower(net);
             my_snprintf(fqnet, len, "%s", global_net);
           } else {
             strtolower(net);
             my_snprintf(fqnet, len, "%s%s.%s", path, instname, net);
           }
           strtolower(fqnet);
           dbg(1, "translate(): net=%s, fqnet=%s start_level=%d\n", net, fqnet, start_level);
           idx = get_raw_index(fqnet);
           if(idx >= 0) {
             val = xctx->raw->cursor_b_val[idx];
           }
           if(idx < 0) {
             valstr = "";
             xctx->tok_size = 0;
             len = 0;
           } else {
             valstr = dtoa_eng(val);
             len = xctx->tok_size;
           }
           if(len) {
             STR_ALLOC(&result, len + result_pos, &size);
             memcpy(result+result_pos, valstr, len+1);
             result_pos += len;
           }
           dbg(1, "inst %d, net=%s, fqnet=%s idx=%d valstr=%s\n", inst,  net, fqnet, idx, valstr);
           my_free(_ALLOC_ID_, &fqnet);
         }
         my_free(_ALLOC_ID_, &net);
       }
     }
   }
   else if(strncmp(token,"@spice_get_current(", 19)==0 )
   {
     int start_level; /* hierarchy level where waves were loaded */
     int live = tclgetboolvar("live_cursor2_backannotate");
     if(live && (start_level = sch_waves_loaded()) >= 0 && xctx->raw->annot_p>=0) {
       char *fqdev = NULL;
       const char *path =  xctx->sch_path[xctx->currsch] + 1;
       char *dev = NULL;
       size_t len;
       int idx, n;
       double val;
       const char *valstr;
       tmp = strlen(token) + 1;
       if(path) {
         int skip = 0;
         /* skip path components that are above the level where raw file was loaded */
         while(*path && skip < start_level) {
           if(*path == '.') skip++;
           ++path;
         }
         dev = my_malloc(_ALLOC_ID_, tmp);
         n = sscanf(token + 19, "%[^)]", dev);
         if(n == 1) {
           strtolower(dev);
           len = strlen(path) + strlen(instname) +
                 strlen(dev) + 21; /* some extra chars for i(..) wrapper */
           dbg(1, "dev=%s\n", dev);
           fqdev = my_malloc(_ALLOC_ID_, len);
           if(!sim_is_xyce) {
             int prefix, vsource;
             char *prefix_ptr = strrchr(dev, '.'); /* last '.' in dev */
             if(prefix_ptr) prefix = prefix_ptr[1]; /* character after last '.' */
             else prefix=dev[0];
             dbg(1, "prefix=%c, path=%s\n", prefix, path);
             vsource = (prefix == 'v') || (prefix == 'e');
             if(vsource) my_snprintf(fqdev, len, "i(%c.%s%s.%s)", prefix, path, instname, dev);
             else if(prefix == 'd')
               my_snprintf(fqdev, len, "i(@%c.%s%s.%s[id])", prefix, path, instname, dev);
             else if(prefix == 'i')
               my_snprintf(fqdev, len, "i(@%c.%s%s.%s[current])", prefix, path, instname, dev);
             else my_snprintf(fqdev, len, "i(@%c.%s%s.%s[i])", prefix, path, instname, dev);
           } else {
             my_snprintf(fqdev, len, "i(%s%s.%s)", path, instname, dev);
           }
           strtolower(fqdev);
           dbg(1, "fqdev=%s\n", fqdev);
           idx = get_raw_index(fqdev);
           if(idx >= 0) {
             val = xctx->raw->cursor_b_val[idx];
           }
           if(idx < 0) {
             valstr = "";
             xctx->tok_size = 0;
             len = 0;
           } else {
             valstr = dtoa_eng(val);
             len = xctx->tok_size;
           }
           if(len) {
             STR_ALLOC(&result, len + result_pos, &size);
             memcpy(result+result_pos, valstr, len+1);
             result_pos += len;
           }
           dbg(1, "inst %d, dev=%s, fqdev=%s idx=%d valstr=%s\n", inst,  dev, fqdev, idx, valstr);
           my_free(_ALLOC_ID_, &fqdev);
         } /* if(n == 1) */
         my_free(_ALLOC_ID_, &dev);
       } /* if(path) */
     } /* if((start_level = sch_waves_loaded()) >= 0 && xctx->raw->annot_p>=0) */
   }
   else if(strcmp(token,"@spice_get_diff_voltage")==0  && xctx->inst[inst].ptr >= 0)
   {
     int start_level; /* hierarchy level where waves were loaded */
     int live = tclgetboolvar("live_cursor2_backannotate");
     if(live && (start_level = sch_waves_loaded()) >= 0 && xctx->raw->annot_p>=0) {
       int multip;
       int no_of_pins= (xctx->inst[inst].ptr + xctx->sym)->rects[PINLAYER];
       if(no_of_pins == 2) {
         char *fqnet1 = NULL, *fqnet2 = NULL;
         const char *path =  xctx->sch_path[xctx->currsch] + 1;
         const char *net1, *net2;
         size_t len;
         int idx1, idx2;
         double val = 0.0, val1 = 0.0, val2 = 0.0;
         const char *valstr;
         if(path) {
           int skip = 0;
           /* skip path components that are above the level where raw file was loaded */
           while(*path && skip < start_level) {
             if(*path == '.') skip++;
             ++path;
           }
           prepare_netlist_structs(0);
           net1 = net_name(inst, 0, &multip, 0, 0);
           len = strlen(path) + strlen(net1) + 1;
           dbg(1, "net1=%s\n", net1);
           fqnet1 = my_malloc(_ALLOC_ID_, len);
           my_snprintf(fqnet1, len, "%s%s", path, net1);
           strtolower(fqnet1);
           net2 = net_name(inst, 1, &multip, 0, 0);
           len = strlen(path) + strlen(net2) + 1;
           dbg(1, "net2=%s\n", net2);
           fqnet2 = my_malloc(_ALLOC_ID_, len);
           my_snprintf(fqnet2, len, "%s%s", path, net2);
           strtolower(fqnet2);
           dbg(1, "translate(): fqnet1=%s start_level=%d\n", fqnet1, start_level);
           dbg(1, "translate(): fqnet2=%s start_level=%d\n", fqnet2, start_level);
           idx1 = get_raw_index(fqnet1);
           idx2 = get_raw_index(fqnet2);
           if(idx1 < 0 || idx2 < 0) {
             valstr = "";
             xctx->tok_size = 0;
             len = 0;
           } else {
             val = xctx->raw->cursor_b_val[idx1] - xctx->raw->cursor_b_val[idx2];
             valstr = dtoa_eng(val);
             len = xctx->tok_size;
           }
           if(len) {
             STR_ALLOC(&result, len + result_pos, &size);
             memcpy(result+result_pos, valstr, len+1);
             result_pos += len;
           }
           dbg(1, "inst %d, fqnet1=%s fqnet2=%s idx1=%d idx2=%d, val1=%g val2=%g valstr=%s\n",
               inst, fqnet1, fqnet2, idx1, idx2, val1, val2, valstr);
           my_free(_ALLOC_ID_, &fqnet1);
           my_free(_ALLOC_ID_, &fqnet2);
         }
       }
     }
   }
   else if(strcmp(token,"@spice_get_current")==0 )
   {
     int start_level; /* hierarchy level where waves were loaded */
     int live = tclgetboolvar("live_cursor2_backannotate");
     if(live && (start_level = sch_waves_loaded()) >= 0 && xctx->raw->annot_p>=0) {
       char *fqdev = NULL;
       const char *path =  xctx->sch_path[xctx->currsch] + 1;
       char *dev = NULL;
       size_t len;
       int idx;
       double val;
       const char *valstr;
       if(path) {
         int skip = 0;
         /* skip path components that are above the level where raw file was loaded */
         while(*path && skip < start_level) {
           if(*path == '.') skip++;
           ++path;
         }
         my_strdup2(_ALLOC_ID_, &dev, instname);
         strtolower(dev);
         len = strlen(path) + strlen(dev) + 21; /* some extra chars for i(..) wrapper */
         dbg(1, "dev=%s\n", dev);
         fqdev = my_malloc(_ALLOC_ID_, len);
         if(!sim_is_xyce) {
           int prefix=dev[0];
           int vsource = (prefix == 'v') || (prefix == 'e');
           if(path[0]) {
             if(vsource) my_snprintf(fqdev, len, "i(%c.%s%s)", prefix, path, dev);
             else if(prefix=='d') my_snprintf(fqdev, len, "i(@%c.%s%s[id])", prefix, path, dev);
             else if(prefix=='i') my_snprintf(fqdev, len, "i(@%c.%s%s[current])", prefix, path, dev);
             else my_snprintf(fqdev, len, "i(@%c.%s%s[i])", prefix, path, dev);
           } else {
             if(vsource) my_snprintf(fqdev, len, "i(%s)", dev);
             else if(prefix == 'd') my_snprintf(fqdev, len, "i(@%s[id])", dev);
             else if(prefix == 'i') my_snprintf(fqdev, len, "i(@%s[current])", dev);
             else my_snprintf(fqdev, len, "i(@%s[i])", dev);
           }
         } else {
           my_snprintf(fqdev, len, "i(%s%s)", path, dev);
         }
         dbg(1, "fqdev=%s\n", fqdev);
         strtolower(fqdev);
         idx = get_raw_index(fqdev);
         if(idx >= 0) {
           val = xctx->raw->cursor_b_val[idx];
         }
         if(idx < 0) {
           valstr = "";
           xctx->tok_size = 0;
           len = 0;
         } else {
           valstr = dtoa_eng(val);
           len = xctx->tok_size;
         }
         if(len) {
           STR_ALLOC(&result, len + result_pos, &size);
           memcpy(result+result_pos, valstr, len+1);
           result_pos += len;
         }
         dbg(1, "inst %d, dev=%s, fqdev=%s idx=%d valstr=%s\n", inst,  dev, fqdev, idx, valstr);
         my_free(_ALLOC_ID_, &fqdev);
         my_free(_ALLOC_ID_, &dev);
       }
     }
   }
   else if(strcmp(token,"@schvhdlprop")==0 && xctx->schvhdlprop)
   {
     tmp=strlen(xctx->schvhdlprop);
     STR_ALLOC(&result, tmp + result_pos, &size);
     memcpy(result+result_pos,xctx->schvhdlprop, tmp+1);
     result_pos+=tmp;
   }

   else if(strcmp(token,"@schprop")==0 && xctx->schprop)
   {
     tmp=strlen(xctx->schprop);
     STR_ALLOC(&result, tmp + result_pos, &size);
     memcpy(result+result_pos,xctx->schprop, tmp+1);
     result_pos+=tmp;
   }
   /* /20100217 */

   else if(strcmp(token,"@schsymbolprop")==0 && xctx->schsymbolprop)
   {
     tmp=strlen(xctx->schsymbolprop);
     STR_ALLOC(&result, tmp + result_pos, &size);
     memcpy(result+result_pos,xctx->schsymbolprop, tmp+1);
     result_pos+=tmp;
   }
   else if(strcmp(token,"@schtedaxprop")==0 && xctx->schtedaxprop)
   {
     tmp=strlen(xctx->schtedaxprop);
     STR_ALLOC(&result, tmp + result_pos, &size);
     memcpy(result+result_pos,xctx->schtedaxprop, tmp+1);
     result_pos+=tmp;
   }
   /* /20100217 */

   else if(strcmp(token,"@schverilogprop")==0 && xctx->schverilogprop)
   {
     tmp=strlen(xctx->schverilogprop);
     STR_ALLOC(&result, tmp + result_pos, &size);
     memcpy(result+result_pos,xctx->schverilogprop, tmp+1);
     result_pos+=tmp;
   /* if spiceprefix==0 and token == @spiceprefix then set empty value */
   } else if(!sp_prefix && !strcmp(token, "@spiceprefix")) {
     /* add nothing */
   } else {
     value = get_tok_value(xctx->inst[inst].prop_ptr, token+1, 0);
     if(!xctx->tok_size && xctx->inst[inst].ptr >= 0) {
       value=get_tok_value((xctx->inst[inst].ptr + xctx->sym)->templ, token+1, 0);
     }
     if(!xctx->tok_size) { /* above lines did not find a value for token */
       if(token[0] =='%') {
         /* no definition found -> subst with token without leading % */
         tmp=token_pos -1 ; /* we need token_pos -1 chars, ( strlen(token+1) ) , excluding leading '%' */
         STR_ALLOC(&result, tmp + result_pos, &size);
         /* dbg(2, "translate(): token=%s, token_pos = %d\n", token, token_pos); */
         memcpy(result+result_pos, token + 1, tmp+1);
         result_pos+=tmp;
       }
     } else {
       int i = level;
       my_strdup2(_ALLOC_ID_, &value1, value);
       /* recursive substitution of value using parent level prop_str attributes */
       while(i > 0) {
         const char *tok = get_tok_value(lcc[i-1].prop_ptr, value1, 0);
         if(xctx->tok_size && tok[0]) {
           dbg(1, "tok=%s\n", tok);
           my_strdup2(_ALLOC_ID_, &value1, tok);
         } else {
           tok = get_tok_value(lcc[i-1].templ,  value1, 0);
           if(xctx->tok_size && tok[0]) {
             dbg(1, "from parent template: tok=%s\n", tok);
             my_strdup2(_ALLOC_ID_, &value1, tok);
           }
         }
         dbg(1, "2 translate(): lcc[%d].prop_ptr=%s, value1=%s\n", i-1, lcc[i-1].prop_ptr, value1);
         i--;
       }
       tmp=strlen(value1);
       STR_ALLOC(&result, tmp + result_pos, &size);
       memcpy(result+result_pos, value1, tmp+1);
       result_pos+=tmp;
       my_free(_ALLOC_ID_, &value1);
     }
   }
   token_pos = 0;
   if(c == '@' || c == '%') s--;
   else result[result_pos++]=(char)c;
   state=TOK_BEGIN;
  } /* else if(state==TOK_SEP) */
  else if(state==TOK_BEGIN) result[result_pos++]=(char)c;
  if(c=='\0')
  {
   result[result_pos]='\0';
   break;
  }
 } /* while(1) */
 dbg(2, "translate(): returning %s\n", result);
 my_free(_ALLOC_ID_, &token);

 /* if result is like: 'tcleval(some_string)' pass it thru tcl evaluation so expressions
  * can be calculated */
 my_strdup2(_ALLOC_ID_, &translated_tok, tcl_hook2(result));
 return translated_tok;
}

const char *translate2(Lcc *lcc, int level, char* s)
{
  static const char *empty="";
  static char *result = NULL;
  int i, escape = 0;
  register int c, state = TOK_BEGIN, space;
  const char *tmp_sym_name;
  size_t sizetok = 0, result_pos = 0, token_pos = 0, size = 0, tmp = 0;
  char  *token = NULL, *value = NULL;

  if(!s) {
    my_free(_ALLOC_ID_, &result);
    return empty;
  }
  size = CADCHUNKALLOC;
  my_realloc(_ALLOC_ID_, &result, size);
  result[0] = '\0';
  dbg(1, "translate2(): s=%s, level=%d\n", s, level);
  while (1) {
    c = *s++;
    if (c == '\\') {
      escape = 1;
      /* we keep backslashes as they should mark end of tokens as for example in: @token1\xxxx@token2
         these backslashes will be 'eaten' at drawing time by translate() */
      /* c = *s++; */
    }
    else escape = 0;
    space = SPACE(c);
    if( state==TOK_BEGIN && (c=='@' || c=='%' ) && !escape  ) state=TOK_TOKEN;
    else if(state==TOK_TOKEN && token_pos > 1 &&
       (
         ( (space  || c == '%' || c == '@') && !escape ) ||
         ( (!space && c != '%' && c != '@') && escape  )
       )   
      ) state = TOK_SEP;
    STR_ALLOC(&result, result_pos, &size);
    STR_ALLOC(&token, token_pos, &sizetok);
    if (state == TOK_TOKEN) token[token_pos++] = (char)c;
    else if (state == TOK_SEP) {
      token[token_pos] = '\0';
      token_pos = 0;

      dbg(1, "translate2(): lcc[%d].prop_ptr=%s token=%s\n", level, lcc[level].prop_ptr, token);
      /* if spiceprefix==0 and token == @spiceprefix then set empty value */
      if(!tclgetboolvar("spiceprefix") && !strcmp(token, "@spiceprefix")) {
        if(value) my_free(_ALLOC_ID_, &value);
        xctx->tok_size = 0;
      } else if(token[0] == '@' && (token[1] == '@' || token[1] == '#')) { /* get rid of pin attribute info */
        if(value) my_free(_ALLOC_ID_, &value);
        xctx->tok_size = 0;
      } else {
        my_strdup2(_ALLOC_ID_, &value, get_tok_value(lcc[level].prop_ptr, token + 1, 0));
        /* propagate %xxx tokens to upper levels if no value found */
        if(!value[0]  && token[0] == '%') {
          my_strdup2(_ALLOC_ID_, &value, token + 1);
          xctx->tok_size = 1; /* just to tell %xxx token was found */
        }
        dbg(1, "translate2(): lcc[%d].prop_ptr=%s value=%s\n", level, lcc[level].prop_ptr, value);
      }
      if(xctx->tok_size && value[0]) {
        i = level;
        /* recursive substitution of value using parent level prop_str attributes */
        while(i > 1) {
          const char *upperval = get_tok_value(lcc[i-1].prop_ptr, value, 0);
          dbg(1, "translate2(): lcc[%d].prop_ptr=%s upperval=%s\n", i-1, lcc[i-1].prop_ptr, upperval);
          if(xctx->tok_size && upperval[0]) {
            my_strdup2(_ALLOC_ID_, &value, upperval);
          } else {
            break;
          }
          i--;
        }
        tmp = strlen(value);
        STR_ALLOC(&result, tmp + 1 + result_pos, &size); /* +1 to add leading '%' */
        /* prefix substituted token with a '%' so it will be recognized by translate()
         * for last level translation with instance placement prop_ptr attributes at
         * drawing/netlisting time. */
        memcpy(result + result_pos , "%", 1);
        memcpy(result + result_pos + 1 , value, tmp + 1);
        result_pos += tmp + 1;
      }
      else if (strncmp(token, "@spice_get_voltage", 18) == 0 ||
               strncmp(token, "@spice_get_current", 18) == 0) { /* return unchanged */
        tmp = strlen(token);
        STR_ALLOC(&result, tmp + result_pos, &size);
        memcpy(result + result_pos, token, tmp + 1);
        result_pos += tmp;
      }
      else if(strcmp(token,"@path")==0) {
        char *path = NULL;
        my_strdup2(_ALLOC_ID_, &path, "@path@name\\.");
        if(level > 1) { /* add parent LCC instance names (X1, Xinv etc) */
          int i;
          for(i = 1; i <level; ++i) {
            const char *instname = get_tok_value(lcc[i].prop_ptr, "name", 0);
            my_strcat(_ALLOC_ID_, &path, instname);
            my_strcat(_ALLOC_ID_, &path, ".");
          }
        }
        dbg(1, "path=%s\n", path);
        tmp=strlen(path);
        STR_ALLOC(&result, tmp + result_pos, &size);
        memcpy(result+result_pos, path, tmp+1);
        my_free(_ALLOC_ID_, &path);
        result_pos+=tmp;
      }
      else if (strcmp(token, "@symname") == 0) {
        tmp_sym_name = lcc[level].symname ? get_cell(lcc[level].symname, 0) : "";
        tmp = strlen(tmp_sym_name);
        STR_ALLOC(&result, tmp + result_pos, &size);
        memcpy(result + result_pos, tmp_sym_name, tmp + 1);
        result_pos += tmp;
      }
      else if (strcmp(token, "@symname_ext") == 0) {
        tmp_sym_name = lcc[level].symname ? get_cell_w_ext(lcc[level].symname, 0) : "";
        tmp = strlen(tmp_sym_name);
        STR_ALLOC(&result, tmp + result_pos, &size);
        memcpy(result + result_pos, tmp_sym_name, tmp + 1);
        result_pos += tmp;
      }
      if (c == '%' || c == '@') s--; /* push back to input for next token */
      else result[result_pos++] = (char)c;
      state = TOK_BEGIN;
    }
    else if (state == TOK_BEGIN) result[result_pos++] = (char)c;
    if (c == '\0') {
      result[result_pos] = '\0';
      break;
    }
  } /* while(1) */
  my_free(_ALLOC_ID_, &token);
  my_free(_ALLOC_ID_, &value);
  dbg(1, "translate2(): result=%s\n", result);
  /* return tcl_hook2(result); */
  return result;
}



/* substitute given tokens in a string with their corresponding values */
/* ex.: name=@name w=@w l=@l ---> name=m112 w=3e-6 l=0.8e-6 */
/* using s1, s2, s3 in turn to resolve @tokens */
/* if no definition for @token is found return @token as is in s */
/* if s==NULL return emty string */
const char *translate3(const char *s, const char *s1, const char *s2, const char *s3)
{
 static const char *empty="";
 static char *translated_tok = NULL;
 static char *result=NULL; /* safe to keep even with multiple schematics */
 register int c, state=TOK_BEGIN, space;
 char *token=NULL;
 size_t sizetok=0;
 size_t token_pos=0;
 const char *value;
 int escape=0;
 char *value1 = NULL;


 if(!s || !xctx) {
   my_free(_ALLOC_ID_, &result);
   my_free(_ALLOC_ID_, &translated_tok);
   return empty;
 }
 dbg(2, "translate3():\n   s=%s\n   s1=%s\n   s2=%s\n   s3=%s\n", s, s1, s2, s3);
 my_strdup2(_ALLOC_ID_, &result, "");

 while(1) {
  c=*s++;
  if(c=='\\') {
    escape=1;
    c=*s++; /* do not remove: breaks translation of format strings in netlists (escaping %) */
  }
  else escape=0;
  space=SPACE(c);
  if( state==TOK_BEGIN && (c=='@' || c=='%' ) && !escape  ) state=TOK_TOKEN; /* 20161210 escape */
  else if(state==TOK_TOKEN && token_pos > 1 &&
     (
       ( (space  || c == '%' || c == '@') && !escape ) ||
       ( (!space && c != '%' && c != '@') && escape  )
     )                          
    ) state=TOK_SEP;

  STR_ALLOC(&token, token_pos, &sizetok);
  if(state==TOK_TOKEN) token[token_pos++]=(char)c;
  else if(state==TOK_SEP) {
   token[token_pos]='\0';
   value = get_tok_value(s1, token+1, 0);
   if(!xctx->tok_size && s2) {
     value=get_tok_value(s2, token+1, 0);
   }
   if(!xctx->tok_size && s3) {
     value=get_tok_value(s3, token+1, 0);
   }
   if(!xctx->tok_size) { /* above lines did not find a value for token */
     /* no definition found -> keep token */
     my_strcat(_ALLOC_ID_, &result, token);
   } else {
     my_strdup2(_ALLOC_ID_, &value1, value);
     my_strcat(_ALLOC_ID_, &result, value1);
     my_free(_ALLOC_ID_, &value1);
   }
   token_pos = 0;
   if(c == '@' || c == '%') s--;
   else {
     char ch[2];
     ch[0] = (char)c;
     ch[1] = '\0';
     my_strcat(_ALLOC_ID_, &result, ch);
   }
   state=TOK_BEGIN;
  } /* else if(state==TOK_SEP) */
  else if(state==TOK_BEGIN) {
   char ch[2];
   ch[0] = (char)c;
   ch[1] = '\0';
   my_strcat(_ALLOC_ID_, &result, ch);
  }
  if(c=='\0') {
   my_strcat(_ALLOC_ID_, &result, "");
   break;
  }
 } /* while(1) */
 dbg(2, "translate3(): returning %s\n", result);
 my_free(_ALLOC_ID_, &token);

 /* if result is like: 'tcleval(some_string)' pass it thru tcl evaluation so expressions
  * can be calculated */
 my_strdup2(_ALLOC_ID_, &translated_tok, tcl_hook2(result));
 return translated_tok;
}

