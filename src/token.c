/* File: token.c
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

#include "xschem.h"
#define SPACE(c)  ( c=='\n' || c==' ' || c=='\t' || c=='\0' || c==';' )

/* instance name (refdes) hash table, for unique name checking */
struct inst_hashentry {
                  struct inst_hashentry *next;
                  unsigned int hash;
                  char *token;
                  int value;
                 };


static struct inst_hashentry *table[HASHSIZE];

enum status {TOK_BEGIN, TOK_TOKEN, TOK_SEP, TOK_VALUE, TOK_END, TOK_ENDTOK};

/* calculate the hash function relative to string s */
static unsigned int hash(char *tok)
{
  unsigned int hash = 0;
  int c;

  while ( (c = *tok++) )
      hash = c + (hash << 6) + (hash << 16) - hash;
  return hash;
}

int name_strcmp(char *s, char *d) /* compare strings up to '\0' or'[' */
{
 int i=0;
 while(1)
 {
  if(d[i]=='\0' || d[i]=='[')
  {
    if(s[i]!='\0' && s[i]!='[') return 1;
    return 0;
  }
  if(s[i]=='\0' || s[i]=='[')
  {
    if(d[i]!='\0' && d[i]!='[') return 1;
    return 0;
  }
  if(s[i]!=d[i]) return 1;
  i++;
 }
}

/* 20180926 added token_size */
/* remove:
 * 0,XINSERT : lookup and insert in hash table (return NULL if token was not found)
 *    if token was found update value
   3,XINSERT_NOREPLACE : same as XINSERT but do not replace existing value if token found.
 * 1,XDELETE : delete token entry, return NULL
 * 2,XLOOKUP : lookup only
 */
static struct inst_hashentry *inst_hash_lookup(struct inst_hashentry **table, char *token,
                                               int value, int remove, size_t token_size)
{
  unsigned int hashcode;
  unsigned int index;
  struct inst_hashentry *entry, *saveptr, **preventry;
  int s;

  if(token==NULL) return NULL;
  hashcode=hash(token);
  index=hashcode % HASHSIZE;
  entry=table[index];
  preventry=&table[index];
  while(1) {
    if( !entry ) {                         /* empty slot */
      if(remove == XINSERT || remove == XINSERT_NOREPLACE) {            /* insert data */
        s=sizeof( struct inst_hashentry );
        entry=(struct inst_hashentry *) my_malloc(425, s);
        *preventry=entry;
        entry->next=NULL;
        entry->hash=hashcode;
        entry->token=NULL;
        entry->token = my_malloc(426, token_size + 1);
        memcpy(entry->token,token, token_size + 1);
        entry->value = value;
      }
      return NULL; /* token was not in hash */
    }
    if( entry->hash==hashcode && !strcmp(token,entry->token) ) { /* found a matching token */
      if(remove == XDELETE) {              /* remove token from the hash table ... */
        saveptr=entry->next;
        my_free(968, &entry->token);
        my_free(969, &entry);
        *preventry=saveptr;
        return NULL;
      } else if(remove == XINSERT) {
        entry->value = value;
      }
      /* dbg(1, "inst_hash_lookup(): returning: %s , %d\n", entry->token, entry->value); */
      return entry;        /* found matching entry, return the address */
    }
    preventry=&entry->next; /* descend into the list. */
    entry = entry->next;
  }
}

static struct inst_hashentry *inst_free_hash_entry(struct inst_hashentry *entry)
{
  struct inst_hashentry *tmp;
  while( entry ) {
    tmp = entry -> next;
    my_free(970, &(entry->token));
    dbg(3, "inst_free_hash_entry(): removing entry %lu\n", (unsigned long)entry);
    my_free(971, &entry);
    entry = tmp;
  }
  return NULL;
}

static void inst_free_hash(struct inst_hashentry **table) /* remove the whole hash table  */
{
 int i;

 dbg(1, "inst_free_hash(): removing hash table\n");
 for(i=0;i<HASHSIZE;i++)
 {
   table[i] = inst_free_hash_entry( table[i] );
 }
}

void hash_all_names(int n)
{
  int i;
  inst_free_hash(table);
  for(i=0; i<xctx->instances; i++) {
    /* if(i == n) continue; */
    inst_hash_lookup(table, xctx->inst[i].instname, i, XINSERT, strlen(xctx->inst[i].instname));
  }
}


void tcl_hook(char **res)
{
  char * result = *res;
  if(result && strstr(result, "tcleval(")== result) {
    Tcl_VarEval(interp, "tclpropeval2 {", result, "}" , NULL);
    my_strdup2(1198, res, tclresult());
  }
}

void clear_instance_hash()
{
  inst_free_hash(table);
}

void check_unique_names(int rename)
{
  int i, j, first = 1;
  char *tmp = NULL;
  int newpropcnt = 0;
  int mult;
  char *start;
  char *comma_pos;
  char *expanded_instname = NULL;
  struct inst_hashentry *entry;
  int big =  xctx->wires> 2000 || xctx->instances > 2000;
  /* int save_draw; */

  if(hilight_nets) {
    xRect boundbox;
    if(!big) calc_drawing_bbox(&boundbox, 2);
    enable_drill=0;
    delete_hilight_net();
    /* undraw_hilight_net(1); */
    if(!big) {
      bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
      bbox(ADD, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
      bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
    }
    draw();
    if(!big) bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  }
  inst_free_hash(table);
  first = 1;
  for(i=0;i<xctx->instances;i++) {
    if(xctx->inst[i].instname && xctx->inst[i].instname[0]) {
      my_strdup(118, &expanded_instname, expandlabel(xctx->inst[i].instname, &mult));
      comma_pos = 0;
      for(j =0; j< mult; j++) {
        if(j == 0) start = expanded_instname;
        else start = comma_pos;
        comma_pos = strchr(start, ',');
        if(comma_pos) *comma_pos = '\0';
        dbg(1, "check_unique_names(): checking %s\n", start);
        if( (entry = inst_hash_lookup(table, start, i, XINSERT_NOREPLACE, strlen(start)) ) && entry->value != i) {
          xctx->inst[i].flags |=4;
          hilight_nets=1;
          if(rename == 1) {
            if(first) {
              bbox(START,0.0,0.0,0.0,0.0);
              set_modify(1); push_undo();
              prepared_hash_instances=0;
              prepared_netlist_structs=0;
              prepared_hilight_structs=0;
              first = 0;
            }
            bbox(ADD, xctx->inst[i].x1, xctx->inst[i].y1, xctx->inst[i].x2, xctx->inst[i].y2);
          }
        }
        if(comma_pos) {
          *comma_pos=',';
          comma_pos++;
        }
      } /* for(j...) */
      if( (xctx->inst[i].flags & 4) && rename) {
        my_strdup(511, &tmp, xctx->inst[i].prop_ptr);
        new_prop_string(i, tmp, newpropcnt++, !rename);
        my_strdup2(512, &xctx->inst[i].instname, get_tok_value(xctx->inst[i].prop_ptr, "name", 0));
        inst_hash_lookup(table, xctx->inst[i].instname, i, XINSERT, strlen(xctx->inst[i].instname));
        symbol_bbox(i, &xctx->inst[i].x1, &xctx->inst[i].y1, &xctx->inst[i].x2, &xctx->inst[i].y2);
        bbox(ADD, xctx->inst[i].x1, xctx->inst[i].y1, xctx->inst[i].x2, xctx->inst[i].y2);
        my_free(972, &tmp);
      }
      my_free(973, &expanded_instname);
    }
  } /* for(i...) */
  if(rename == 1 && hilight_nets) {
    bbox(SET,0.0,0.0,0.0,0.0);
    draw();
    bbox(END,0.0,0.0,0.0,0.0);
  }
  /* draw_hilight_net(1); */
  redraw_hilights();
  /* draw_window = save_draw; */
}



int match_symbol(const char *name)  /* never returns -1, if symbol not found load systemlib/missing.sym */
{
 int i,found;

 found=0;
 for(i=0;i<xctx->symbols;i++)
 {
  /* dbg(1, "match_symbol(): name=%s, sym[i].name=%s\n",name, xctx->sym[i].name);*/
  if(strcmp(name, xctx->sym[i].name) == 0)
  {
   dbg(1, "match_symbol(): found matching symbol:%s\n",name);
   found=1;break;
  }
 }
 if(!found)
 {
  dbg(1, "match_symbol(): matching symbol not found:%s, loading\n",name);
  load_sym_def(name, NULL); /* append another symbol to the xctx->sym[] array */
 }
 dbg(1, "match_symbol(): returning %d\n",i);
 return i;
}

/* update **s modifying only the token values that are */
/* different between *new and *old */
/* return 1 if s modified 20081221 */
int set_different_token(char **s,const char *new, const char *old, int object, int n)
{
 register int c, state=TOK_BEGIN, space;
 char *token=NULL, *value=NULL;
 int sizetok=0, sizeval=0;
 int token_pos=0, value_pos=0;
 int quote=0;
 int escape=0;
 int mod;
 const char *my_new;

 mod=0;
 my_new = new;
 dbg(1, "set_different_token(): *s=%s, new=%s, old=%s n=%d\n",*s, new, old, n);
 if(new==NULL) return 0;

 sizeval = sizetok = CADCHUNKALLOC;
 my_realloc(427, &token, sizetok);
 my_realloc(429, &value, sizeval);

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
  if(state==TOK_TOKEN) token[token_pos++]=c;
  else if(state==TOK_VALUE) {
   value[value_pos++]=c;
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
    if(event_reporting && object == ELEMENT) {
      char n1[PATH_MAX];
      char n2[PATH_MAX];
      char n3[PATH_MAX];
      printf("xschem setprop %s %s %s\n",
           escape_chars(n1, xctx->inst[n].instname, PATH_MAX),
           escape_chars(n2, token, PATH_MAX),
           escape_chars(n3, value, PATH_MAX)
      );
      fflush(stdout);
    }
    mod=1;
    my_strdup(433, s, subst_token(*s, token, value) );
   }
   state=TOK_BEGIN;
  }
  escape = (c=='\\' && !escape);
  if(c=='\0') break;
 }

 state = TOK_BEGIN;
 escape = quote = token_pos = value_pos = 0;
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
  if(state==TOK_TOKEN) token[token_pos++]=c;
  else if(state==TOK_VALUE) {
   value[value_pos++]=c;
  }
  else if(state==TOK_ENDTOK || state==TOK_SEP) {
   if(token_pos) {
     token[token_pos]='\0';
     token_pos=0;
   }
   get_tok_value(new,token,1);
   if(get_tok_size == 0 ) {

    if(event_reporting && object == ELEMENT) {
      char n1[PATH_MAX];
      char n2[PATH_MAX];
      printf("xschem setprop %s %s\n",
           escape_chars(n1, xctx->inst[n].instname, PATH_MAX),
           escape_chars(n2, token, PATH_MAX)
      );
      fflush(stdout);
    }
    mod=1;
    my_strdup(443, s, subst_token(*s, token, NULL) );
   }
  } else if(state==TOK_END) {
   value[value_pos]='\0';
   value_pos=0;
   state=TOK_BEGIN;
  }
  escape = (c=='\\' && !escape);
  if(c=='\0') break;
 }
 my_free(974, &token);
 my_free(975, &value);
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
  int  sizetok=0;
  register int c, state=TOK_BEGIN, space;
  register int token_pos=0;
  int quote=0;
  int escape=0;

  if(s==NULL) {
    my_free(435, &token);
    sizetok = 0;
    return "";
  }
  sizetok = CADCHUNKALLOC;
  my_realloc(451, &token, sizetok);
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
        if((with_quotes & 1) || escape)  token[token_pos++]=c;
      }
      else if( !(c == '\\' && (with_quotes & 2)) ) token[token_pos++]=c;
      else if(escape && c == '\\') token[token_pos++]=c;
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

/* state machine that parses a string made up of <token>=<value> ... */
/* couples and returns the value of the given token  */
/* if s==NULL or no match return empty string */
/* NULL tok NOT ALLOWED !!!!!!!! */
/* never returns NULL... */
/* with_quotes: */
/* 0: eat non escaped quotes (") */
/* 1: return unescaped quotes as part of the token value if they are present */
/* 2: eat backslashes and unescaped double quotes (") */
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

  if(s==NULL) {
    my_free(976, &result);
    my_free(977, &token);
    size = sizetok = 0;
    get_tok_value_size = get_tok_size = 0;
    return "";
  }
  get_tok_value_size = get_tok_size = 0;
  dbg(2, "get_tok_value(): looking for <%s> in <%s>\n",tok,s);
  if( size == 0 ) {
    sizetok = size = CADCHUNKALLOC;
    my_realloc(454, &result, size);
    my_realloc(457, &token, sizetok);
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
      my_realloc(436, &result,size);
    }
    if(token_pos>=sizetok) {
      sizetok+=CADCHUNKALLOC;
      my_realloc(437, &token,sizetok);
    }
    if(c=='"') {
      if(!escape) quote=!quote;
    }
    if(state==TOK_TOKEN) {
      if(!cmp) { /* previous token matched search and was without value, return get_tok_size */
        result[0] = '\0';
        get_tok_value_size = 0;
        return result;
      }
      if(c=='"') {
        if((with_quotes & 1) || escape)  token[token_pos++]=c;
      }
      else if( !(c == '\\' && (with_quotes & 2)) ) token[token_pos++]=c;
      else if(escape && c == '\\') token[token_pos++]=c;
    } else if(state==TOK_VALUE) {
      if(c=='"') {
        if((with_quotes & 1) || escape)  result[value_pos++]=c;
      }
      else if( !quote || !((c == '\\') && (with_quotes & 2)) ) result[value_pos++]=c; /* skip unescaped backslashes */
      else if( (c == '\\') && escape ) result[value_pos++]=c; /* 20170414 add escaped backslashes */
    } else if(state==TOK_ENDTOK || state==TOK_SEP) {
        if(token_pos) {
          token[token_pos]='\0';
          if( !(cmp = strcmp(token,tok)) ) {
            get_tok_size = token_pos; /* report back also token size, useful to check if requested token exists */
          }
          dbg(2, "get_tok_value(): token=%s\n", token);
          token_pos=0;
        }
    } else if(state==TOK_END) {
      result[value_pos]='\0';
      if( !cmp ) {
        get_tok_value_size = value_pos; /* return also size so to avoid using strlen 20180926 */
        return result;
      }
      value_pos=0;
      state=TOK_BEGIN;
    }
    escape = (c=='\\' && !escape);
    if(c=='\0') {
      result[0]='\0';
      get_tok_size = 0;
      get_tok_value_size = 0; /* return also size so to avoid using strlen 20180926 */
      return result;
    }
  }
}

/* return template string excluding name=... and token=value where token listed in extra */
const char *get_sym_template(char *s,char *extra)
{
 static char *result=NULL;
 int sizeres=0;
 int sizetok=0;
 int sizeval=0;
 char *value=NULL;
 char *token=NULL;
 register int c, state=TOK_BEGIN, space;
 register int token_pos=0, value_pos=0, result_pos=0;
 int quote=0;
 int escape=0;
 int with_quotes=0;
 int l;
/* with_quotes: */
/* 0: eat non escaped quotes (") */
/* 1: return unescaped quotes as part of the token value if they are present */
/* 2: eat backslashes */
/* 3: 1+2  :) */

 if(s==NULL) {
   my_free(978, &result);
   return "";
 }
 l = strlen(s);
 STR_ALLOC(&result, l+1, &sizeres);
 sizetok = sizeval = CADCHUNKALLOC;
 my_realloc(438, &value,sizeval);
 my_realloc(439, &token,sizetok);
 while(1) {
  c=*s++;
  space=SPACE(c) ;
  if( (state==TOK_BEGIN || state==TOK_ENDTOK) && !space && c != '=') state=TOK_TOKEN;
  else if( state==TOK_TOKEN && space) state=TOK_ENDTOK;
  else if( (state==TOK_TOKEN || state==TOK_ENDTOK) && c=='=') state=TOK_SEP;
  else if( state==TOK_SEP && !space) state=TOK_VALUE;
  else if( state==TOK_VALUE && space && !quote) state=TOK_END;
  STR_ALLOC(&value, value_pos, &sizeval);
  STR_ALLOC(&token, token_pos, &sizetok);
  if(state==TOK_BEGIN) {
    result[result_pos++] = c;
  } else if(state==TOK_TOKEN) {
    token[token_pos++]=c;
  } else if(state==TOK_VALUE) {
    if(c=='"') {
     if(!escape) quote=!quote;
     if((with_quotes & 1) || escape)  value[value_pos++]=c;
    }
    else if( (c=='\\') && (with_quotes & 2) )  ;  /* dont store backslash */
    else value[value_pos++]=c;
    escape = (c=='\\' && !escape);

  } else if(state==TOK_END) {
    value[value_pos]='\0';
    if((!extra || !strstr(extra, token)) && strcmp(token,"name")) {
      memcpy(result+result_pos, value, value_pos+1);
      result_pos+=value_pos;
    }
    result[result_pos++] = c;
    value_pos=0;
    token_pos=0;
    state=TOK_BEGIN;
  } else if(state==TOK_ENDTOK || state==TOK_SEP) {
    if(token_pos) {
      token[token_pos]='\0';
      if((!extra || !strstr(extra, token)) && strcmp(token,"name")) {
        memcpy(result+result_pos, token, token_pos+1);
        result_pos+=token_pos;
        result[result_pos++] = c;
      }
      token_pos=0;
    }
  }
  if(c=='\0') {
    break;
  }
 }
 my_free(979, &value);
 my_free(980, &token);
 return result;
}

const char *find_bracket(const char *s)
{
 while(*s!='['&& *s!='\0') s++;
 return s;
}

/* caller is responsible for freeing up storage for return value
 * return NULL if no matching token found */
char *get_pin_attr_from_inst(int inst, int pin, const char *attr)
{
   int attr_size;
   char *pinname = NULL, *pname = NULL, *pin_attr_value = NULL;
   char *pnumber = NULL;
   const char *str;


   dbg(1, "get_pin_attr_from_inst(): inst=%d pin=%d attr=%s\n", inst, pin, attr);
   pin_attr_value = NULL;
   str = get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][pin].prop_ptr,"name",0);
   if(str[0]) {
     attr_size = strlen(attr);
     my_strdup(498, &pinname, str);
     pname =my_malloc(49, get_tok_value_size + attr_size + 30);
     my_snprintf(pname, get_tok_value_size + attr_size + 30, "%s(%s)", attr, pinname);
     my_free(981, &pinname);
     str = get_tok_value(xctx->inst[inst].prop_ptr, pname, 0);
     my_free(982, &pname);
     if(get_tok_size) my_strdup2(51, &pin_attr_value, str);
     else {
       pnumber = my_malloc(52, attr_size + 100);
       my_snprintf(pnumber, attr_size + 100, "%s(%d)", attr, pin);
       str = get_tok_value(xctx->inst[inst].prop_ptr, pnumber, 0);
       dbg(1, "get_pin_attr_from_inst(): pnumber=%s\n", pnumber);
       my_free(983, &pnumber);
       if(get_tok_size) my_strdup2(40, &pin_attr_value, str);
     }
   }
   return pin_attr_value; /* caller is responsible for freeing up storage for pin_attr_value */
}

void new_prop_string(int i, const char *old_prop, int fast, int dis_uniq_names)
{
/* given a old_prop property string, return a new */
/* property string in xctx->inst[i].prop_ptr such that the element name is */
/* unique in current design (that is, element name is changed */
/* if necessary) */
/* if old_prop=NULL return NULL */
/* if old_prop does not contain a valid "name" or empty return old_prop */
 static char prefix;
 char *old_name=NULL, *new_name=NULL;
 const char *tmp;
 const char *tmp2;
 int q,qq;
 static int last[256];
 int old_name_len;
 int new_name_len;
 int n;
 char *old_name_base = NULL;
 struct inst_hashentry *entry;

 dbg(1, "new_prop_string(): i=%d, old_prop=%s, fast=%d\n", i, old_prop, fast);
 if(!fast) { /* on 1st invocation of new_prop_string */
   for(q=1;q<=255;q++) last[q]=1;
 }
 if(old_prop==NULL)
 {
  my_free(984, &xctx->inst[i].prop_ptr);
  return;
 }
 old_name_len = my_strdup(444, &old_name,get_tok_value(old_prop,"name",0) ); /* added old_name_len */

 if(old_name==NULL)
 {
  my_strdup(446, &xctx->inst[i].prop_ptr, old_prop);  /* changed to copy old props if no name */
  return;
 }
 prefix=old_name[0];
 /* don't change old_prop if name does not conflict. */
 if(dis_uniq_names || (entry = inst_hash_lookup(table, old_name, i, XLOOKUP, old_name_len))==NULL ||
     entry->value == i)
 {
  inst_hash_lookup(table, old_name, i, XINSERT, old_name_len);
  my_strdup(447, &xctx->inst[i].prop_ptr, old_prop);
  my_free(985, &old_name);
  return;
 }
 old_name_base = my_malloc(64, old_name_len+1);
 n = sscanf(old_name, "%[^[0-9]",old_name_base);
 tmp=find_bracket(old_name);
 my_realloc(448, &new_name, old_name_len + 40); /* strlen(old_name)+40); */
 qq=fast ?  last[(int)prefix] : 1;
 for(q=qq;;q++)
 {
   if(n >= 1 ) {
     new_name_len = my_snprintf(new_name, old_name_len + 40, "%s%d%s", old_name_base, q, tmp);
   } else {
     new_name_len = my_snprintf(new_name, old_name_len + 40, "%c%d%s", prefix,q, tmp); /* added new_name_len */
   }
   if((entry = inst_hash_lookup(table, new_name, i, XLOOKUP, new_name_len)) == NULL || entry->value == i)
   {
    last[(int)prefix]=q+1;
    break;
   }
 }
 my_free(986, &old_name_base);
 tmp2 = subst_token(old_prop, "name", new_name);
 if(strcmp(tmp2, old_prop) ) {
   my_strdup(449, &xctx->inst[i].prop_ptr, tmp2);
 }
 inst_hash_lookup(table, new_name, i, XINSERT, new_name_len); /* reinsert in hash */
 my_free(987, &old_name);
 my_free(988, &new_name);
}

const char *subst_token(const char *s, const char *tok, const char *new_val)
/* given a string <s> with multiple "token=value ..." assignments */
/* substitute <tok>'s value with <new_val> */
/* if tok not found in s and new_val!=NULL add tok=new_val at end.*/
/* if new_val is empty ('\0') set token value to "" (token="") */
/* if new_val is NULL *remove* 'token (and =val if any)' from s */
{
  static char *result=NULL;
  int size=0;
  register int c, state=TOK_BEGIN, space;
  int sizetok=0;
  char *token=NULL;
  int token_pos=0, result_pos=0, result_save_pos = 0;
  int quote=0, tmp;
  int done_subst=0;
  int escape=0, matched_tok=0;

  if(s==NULL && tok == NULL){
    my_free(989, &result);
    return "";
  }
  if((tok == NULL || tok[0]=='\0') && s ){
    my_strdup2(458, &result, s);
    return result;
  }
  dbg(1, "subst_token(): %s, %s, %s\n", s, tok, new_val);
  sizetok = size = CADCHUNKALLOC;
  my_realloc(1152, &result, size);
  my_realloc(1153, &token, sizetok);
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
    /*    TOK_BEGIN TOK_TOKEN TOK_ENDTOK TOK_SEP TOK_VALUE            */
    /*                                                          */
    /*                                                          */
    /* TOK_BEGIN                                                */
    /* |      TOK_TOKEN                                            */
    /* |      |   TOK_ENDTOK                                       */
    /* |      |   |  TOK_SEP                                 */
    /* |      |   |  | TOK_VALUE                                   */
    /* |      |   |  | | TOK_BEGIN                              */
    /* |      |   |  | | |   TOK_TOKEN                             */
    /* |      |   |  | | |   |     TOK_ENDTOK                      */
    /* |      |   |  | | |   |     |  TOK_TOKEN                    */
    /* |      |   |  | | |   |     |  |                         */
    /* .......name...=.x1....format...type..=..subcircuit....   */
    /* . : space                                                */

    if(state == TOK_BEGIN && !space && c != '=' ) {
      result_save_pos = result_pos;
      token_pos = 0;
      state = TOK_TOKEN;
    } else if(state == TOK_ENDTOK  && (!space || c == '\0') && c != '=' ) {
      if(!done_subst && matched_tok) {
        if(new_val) { /* add new_val to matching token with no value */
          if(new_val[0]) {
            tmp = strlen(new_val);
          } else {
            new_val = "\"\"";
            tmp = 2;
          }

          STR_ALLOC(&result, tmp+2 + result_pos, &size);
          memcpy(result + result_pos, "=", 1);
          memcpy(result + result_pos+1, new_val, tmp);
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
        if(new_val) { /* replace token value with new_val */
          if(new_val[0]) {
            tmp = strlen(new_val);
          } else {
            new_val = "\"\"";
            tmp = 2;
          }
          STR_ALLOC(&result, tmp + result_pos, &size);
          memcpy(result + result_pos ,new_val, tmp + 1);
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
      result[result_pos++] = c;
    } else if(state == TOK_TOKEN) {
      token[token_pos++] = c;
      result[result_pos++] = c;
    } else if(state == TOK_ENDTOK) {
      result[result_pos++] = c;
    } else if(state == TOK_SEP) {
      result[result_pos++] = c;
    } else if(state==TOK_VALUE) {
      if(!matched_tok) result[result_pos++] = c; /* skip value for matching token */
    }
    escape = (c=='\\' && !escape);
    if(c == '\0') break;
  }
  if(!done_subst) { /* if tok not found add tok=new_value at end */
    if(result_pos == 0 ) result_pos = 1; /* result="" */
    if(new_val) {
      if(!new_val[0]) new_val = "\"\"";
      tmp = strlen(new_val) + strlen(tok) + 2;
      STR_ALLOC(&result, tmp + result_pos, &size);
      my_snprintf(result + result_pos - 1, size, " %s=%s", tok, new_val ); /* result_pos guaranteed to be > 0 */
    }
  }
  dbg(2, "subst_token(): returning: %s\n",result);
  my_free(990, &token);
  return result;
}

const char *get_trailing_path(const char *str, int no_of_dir, int skip_ext)
{
  static char s[PATH_MAX];
  size_t len;
  int ext_pos, dir_pos, n_ext, n_dir, c, i;

  my_strncpy(s, str, S(s));
  len = strlen(s);

  for(ext_pos=len, dir_pos=len, n_ext=0, n_dir=0, i=len; i>=0; i--) {
    c = s[i];
    if(c=='.' && ++n_ext==1) ext_pos = i;
    if(c=='/' && ++n_dir==no_of_dir+1) dir_pos = i;
  }
  if(skip_ext) s[ext_pos] = '\0';

  if(dir_pos==len) return s;
  dbg(2, "get_trailing_path(): str=%s, no_of_dir=%d, skip_ext=%d\n",
                   str, no_of_dir, skip_ext);
  dbg(2, "get_trailing_path(): returning: %s\n", s+(dir_pos<len ? dir_pos+1 : 0));
  return s+(dir_pos<len ? dir_pos+1 : 0);
}

/* skip also extension */
const char *skip_dir(const char *str)
{
  return get_trailing_path(str, 0, 1);
}

/* no extension */
const char *get_cell(const char *str, int no_of_dir)
{
  return get_trailing_path(str, no_of_dir, 1);
}

const char *get_cell_w_ext(const char *str, int no_of_dir)
{
  return get_trailing_path(str, no_of_dir, 0);
}


/* not used? */
int count_labels(char *s)
{
  int i=1;
  int c;

  if(s==NULL) return 1;
  while( (c=(*s++)) ) {
    if(c==',') i++;
  }
  return i;
}

void print_vhdl_element(FILE *fd, int inst)
{
  int i=0, mult, tmp, tmp1;
  const char *str_ptr;
  register int c, state=TOK_BEGIN, space;
  const char *lab;
  char *name=NULL;
  char  *generic_value=NULL, *generic_type=NULL;
  char *template=NULL,*s, *value=NULL,  *token=NULL;
  int no_of_pins=0, no_of_generics=0;
  int sizetok=0, sizeval=0;
  int token_pos=0, value_pos=0;
  int quote=0;
  int escape=0;

  if(get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->prop_ptr,"vhdl_format",2)[0] != '\0') {
   print_vhdl_primitive(fd, inst);
   return;
  }
  my_strdup(462, &name,xctx->inst[inst].instname);
  if(!name) my_strdup(58, &name, get_tok_value(template, "name", 0));
  if(name==NULL) {
    my_free(991, &name);
    return;
  }
  my_strdup(461, &template, (xctx->inst[inst].ptr+ xctx->sym)->templ);
  no_of_pins= (xctx->inst[inst].ptr+ xctx->sym)->rects[PINLAYER];
  no_of_generics= (xctx->inst[inst].ptr+ xctx->sym)->rects[GENERICLAYER];

  s=xctx->inst[inst].prop_ptr;

 /* print instance name and subckt */
  dbg(2, "print_vhdl_element(): printing inst name & subcircuit name\n");
  if( (lab = expandlabel(name, &tmp)) != NULL)
    fprintf(fd, "%d %s : %s\n", tmp, lab, skip_dir(xctx->inst[inst].name) );
  else  /*  name in some strange format, probably an error */
    fprintf(fd, "1 %s : %s\n", name, skip_dir(xctx->inst[inst].name) );
  dbg(2, "print_vhdl_element(): printing generics passed as properties\n");


  /* -------- print generics passed as properties */

  tmp=0;
  /* 20080213 use generic_type property to decide if some properties are strings, see later */
  my_strdup(464, &generic_type, get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->prop_ptr,"generic_type",2));

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
   if(state==TOK_TOKEN) token[token_pos++]=c;
   else if(state==TOK_VALUE) {
     if(c=='"' && !escape) quote=!quote;
     else value[value_pos++]=c;
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
     if(get_tok_size) {
       if(strcmp(token, "name") && value[0] != '\0') /* token has a value */
       {
         if(tmp == 0) {fprintf(fd, "generic map(\n");tmp++;tmp1=0;}
         if(tmp1) fprintf(fd, " ,\n");

         /* 20080213  put "" around string type generics! */
         if( generic_type && !strcmp(get_tok_value(generic_type,token, 2), "string")  ) {
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
     for(i=0;i<no_of_generics;i++)
     {
       my_strdup(467, &generic_type,get_tok_value(
         (xctx->inst[inst].ptr+ xctx->sym)->rect[GENERICLAYER][i].prop_ptr,"type",0));
       my_strdup(468, &generic_value,   xctx->inst[inst].node[no_of_pins+i] );
       /*my_strdup(469, &generic_value, get_tok_value( */
       /*  (xctx->inst[inst].ptr+ xctx->sym)->rect[GENERICLAYER][i].prop_ptr,"value") ); */
       str_ptr = get_tok_value(
         (xctx->inst[inst].ptr+ xctx->sym)->rect[GENERICLAYER][i].prop_ptr,"name",0);
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
  for(i=0;i<no_of_pins;i++)
  {
    if(strcmp(get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][i].prop_ptr,"vhdl_ignore",0), "true")) {
      if( (str_ptr =  net_name(inst,i, &mult, 0)) )
      {
        if(tmp) fprintf(fd, " ,\n");
        fprintf(fd, "   %s => %s",
          get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][i].prop_ptr,"name",0),
          str_ptr);
        tmp=1;
      }
    }
  }
  fprintf(fd, "\n);\n\n");
   dbg(2, "print_vhdl_element(): ------- end ------ \n");
  my_free(992, &name);
  my_free(993, &generic_value);
  my_free(994, &generic_type);
  my_free(995, &template);
  my_free(996, &value);
  my_free(997, &token);
}

void print_generic(FILE *fd, char *ent_or_comp, int symbol)
{
  int tmp;
  register int c, state=TOK_BEGIN, space;
  char *template=NULL, *s, *value=NULL,  *token=NULL;
  char *type=NULL, *generic_type=NULL, *generic_value=NULL;
  const char *str_tmp;
  int i, sizetok=0, sizeval=0;
  int token_pos=0, value_pos=0;
  int quote=0;
  int escape=0;
  int token_number=0;

  my_strdup(472, &template, xctx->sym[symbol].templ);
  if( !template || !(template[0]) ) {
    my_free(998, &template);
    return;
  }
  my_strdup(471, &generic_type, get_tok_value(xctx->sym[symbol].prop_ptr,"generic_type",0));
  dbg(2, "print_generic(): symbol=%d template=%s \n", symbol, template);

  fprintf(fd, "%s %s ",ent_or_comp, skip_dir(xctx->sym[symbol].name));
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
   if(state==TOK_TOKEN) token[token_pos++]=c;
   else if(state==TOK_VALUE)
   {
    if(c=='"' && !escape) quote=!quote;
    else value[value_pos++]=c;
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
    my_strdup(475, &type, get_tok_value(generic_type,token,0));

    if(value[0] != '\0') /* token has a value */
    {
     if(token_number>1)
     {
       if(!tmp) {fprintf(fd, "generic (\n");}
       if(tmp) fprintf(fd, " ;\n");
       if(!type || strcmp(type,"string") ) { /* 20080213 print "" around string values 20080418 check for type==NULL */
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

  for(i=0;i<xctx->sym[symbol].rects[GENERICLAYER];i++)
  {
    my_strdup(476, &generic_type,get_tok_value(
              xctx->sym[symbol].rect[GENERICLAYER][i].prop_ptr,"generic_type",0));
    my_strdup(477, &generic_value, get_tok_value(
              xctx->sym[symbol].rect[GENERICLAYER][i].prop_ptr,"value",2) ); /*<< 170402 */
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
  my_free(999, &template);
  my_free(1001, &value);
  my_free(1002, &token);
  my_free(1003, &type);
  my_free(1004, &generic_type);
  my_free(1005, &generic_value);
}


void print_verilog_param(FILE *fd, int symbol)
{
 register int c, state=TOK_BEGIN, space;
 char *template=NULL, *s, *value=NULL,  *generic_type=NULL, *token=NULL;
 int sizetok=0, sizeval=0;
 int token_pos=0, value_pos=0;
 int quote=0;
 int escape=0;
 int token_number=0;

 my_strdup(479, &template, xctx->sym[symbol].templ); /* 20150409 20171103 */
 my_strdup(480, &generic_type, get_tok_value(xctx->sym[symbol].prop_ptr,"generic_type",0));
 if( !template || !(template[0]) )  {
   my_free(1006, &template);
   my_free(1007, &generic_type);
   return;
 }
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
  if(state==TOK_TOKEN) token[token_pos++]=c;
  else if(state==TOK_VALUE)
  {
   if(c=='"' && !escape) quote=!quote;
   else value[value_pos++]=c;
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
    if(token_number>1)
    {

      /* 20080915 put "" around string params */
      if( !generic_type || strcmp(get_tok_value(generic_type,token, 2), "time")  ) {
        if( generic_type && !strcmp(get_tok_value(generic_type,token, 2), "string")  ) {
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
 my_free(1008, &template);
 my_free(1009, &generic_type);
 my_free(1010, &value);
 my_free(1011, &token);
}





void print_spice_subckt(FILE *fd, int symbol)
{
 int i=0, mult;
 const char *str_ptr=NULL;
 register int c, state=TOK_BEGIN, space;
 char *format=NULL,*s, *token=NULL;
 int pin_number;
 int sizetok=0;
 int token_pos=0, escape=0;
 int no_of_pins=0;

 my_strdup(103, &format, get_tok_value(xctx->sym[symbol].prop_ptr,"format",2));
 if( (format==NULL) ) {
   my_free(1012, &format);
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
  if( state==TOK_BEGIN && (c=='@' || c=='$')  && !escape) state=TOK_TOKEN;
  else if(state==TOK_TOKEN && token_pos > 1 &&
     (
       ( (space  || c == '$' || c == '@') && !escape ) ||
       ( (!space && c != '$' && c != '@') && escape  )
     )
    ) {
    state = TOK_SEP;
  }

  STR_ALLOC(&token, token_pos, &sizetok);
  if(state==TOK_TOKEN) {
    token[token_pos++]=c;
  }
  else if(state==TOK_SEP)                    /* got a token */
  {
   token[token_pos]='\0';
   token_pos=0;
   if(!strcmp(token, "@name")) {
     /* do nothing */
   }
   else if(strcmp(token, "@symname")==0) {
     break ;
   }
   else if(strcmp(token, "@pinlist")==0) {
    for(i=0;i<no_of_pins;i++)
    {
      if(strcmp(get_tok_value(xctx->sym[symbol].rect[PINLAYER][i].prop_ptr,"spice_ignore",0), "true")) {
        str_ptr=
          expandlabel(get_tok_value(xctx->sym[symbol].rect[PINLAYER][i].prop_ptr,"name",0), &mult);
        fprintf(fd, "%s ", str_ptr);
      }
    }
   }
   else if(token[0]=='@' && token[1]=='@') {    /* recognize single pins 15112003 */
     char *prop=NULL;
     for(i = 0; i<no_of_pins; i++) {
       prop = xctx->sym[symbol].rect[PINLAYER][i].prop_ptr;
       if(!strcmp(get_tok_value(prop, "name",0), token + 2)) break;
     }
     if(i<no_of_pins && strcmp(get_tok_value(prop,"spice_ignore",0), "true")) {
       fprintf(fd, "%s ", expandlabel(token+2, &mult));
     }
   }
   /* reference by pin number instead of pin name, allows faster lookup of the attached net name 20180911 */
   else if(token[0]=='@' && token[1]=='#') {
     pin_number = atoi(token+2);
     if(pin_number < no_of_pins) {
       if(strcmp(get_tok_value(xctx->sym[symbol].rect[PINLAYER][pin_number].prop_ptr,"spice_ignore",0), "true")) {
       str_ptr =  get_tok_value(xctx->sym[symbol].rect[PINLAYER][pin_number].prop_ptr,"name",0);
       fprintf(fd, "%s ",  expandlabel(str_ptr, &mult));
       }
     }
   }
   /* this will print the other @parameters, usually "extra" nodes so they will be in the order
    * specified by the format string. The 'extra' attribute is no more used to print extra nodes
    * in spice_block_netlist(). */
   else if(token[0] == '@') { /* given previous if() conditions not followed by @ or # */
     fprintf(fd, "%s ",  token + 1);
   }
   if(c!='$' && c!='@' && c!='\0' ) fputc(c,fd);
   if(c == '@' || c =='$') s--;
   state=TOK_BEGIN;
  }
                 /* 20151028 dont print escaping backslashes */
  else if(state==TOK_BEGIN && c!='\0') {
   /* do nothing */
  }
  if(c=='\0')
  {
   break ;
  }
 }
 my_free(1013, &format);
 my_free(1014, &token);
}

void print_spice_element(FILE *fd, int inst)
{
  int i=0, mult, tmp;
  const char *str_ptr=NULL;
  register int c, state=TOK_BEGIN, space;
  char *template=NULL,*format=NULL,*s, *name=NULL,  *token=NULL;
  const char *lab, *value = NULL;
  int pin_number;
  int sizetok=0;
  int token_pos=0, escape=0;
  int no_of_pins=0;
  char *result = NULL;
  int result_pos = 0;
  int size = 0;

  size = CADCHUNKALLOC;
  my_realloc(1211, &result, size);
  result[0] = '\0';

  my_strdup(483, &template, (xctx->inst[inst].ptr+ xctx->sym)->templ);
  my_strdup(484, &name,xctx->inst[inst].instname);
  if (!name) my_strdup(43, &name, get_tok_value(template, "name", 0));

  /* allow format string override in instance */
  my_strdup(470, &format, get_tok_value(xctx->inst[inst].prop_ptr,"format",2));
  if(!format || !format[0])
     my_strdup(486, &format, get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->prop_ptr,"format",2));

  if ((name==NULL) || (format==NULL)) {
    my_free(1015, &template);
    my_free(1016, &format);
    my_free(1017, &name);
    my_free(1193, &result);
    return; /* do no netlist unwanted insts(no format) */
  }
  no_of_pins= (xctx->inst[inst].ptr+ xctx->sym)->rects[PINLAYER];
  s=format;
  dbg(1, "print_spice_element(): name=%s, format=%s netlist_count=%d\n",name,format, netlist_count);
  /* begin parsing format string */
  while(1)
  {
    /* always make room for some characters so the single char writes to result do not need reallocs */
    STR_ALLOC(&result, 100 + result_pos, &size);
    c=*s++;
    if(c=='\\') {
      escape=1;
      c=*s++;
    }
    else escape=0;

    if (c=='\n' && escape) c=*s++; /* 20171030 eat escaped newlines */
    space=SPACE(c);
    if ( state==TOK_BEGIN && (c=='@'|| c=='$')  && !escape ) state=TOK_TOKEN;
    else if(state==TOK_TOKEN && token_pos > 1 &&
       (
         ( (space  || c == '$' || c == '@') && !escape ) ||
         ( (!space && c != '$' && c != '@') && escape  )
       )
      ) {
      dbg(1, "print_spice_element(): c=%c, space=%d, escape=%d roken_pos=%d\n", c, space, escape, token_pos);
      state=TOK_SEP;
    }
    STR_ALLOC(&token, token_pos, &sizetok);
    if(state==TOK_TOKEN) {
      token[token_pos++]=c;
    }
    else if (state==TOK_SEP)                    /* got a token */
    {
      token[token_pos]='\0';
      token_pos=0;

      if (!spiceprefix && !strcmp(token, "@spiceprefix")) {
        value=NULL;
      } else {
        dbg(1, "print_spice_element(): token: |%s|\n", token);
        value = get_tok_value(xctx->inst[inst].prop_ptr, token+1, 2);
        if (value[0] == '\0')
        value=get_tok_value(template, token+1, 0);
      }
      if(!get_tok_size && token[0] =='$') {


        tmp = strlen(token + 1) +100 ; /* always make room for some extra chars 
                                        * so 1-char writes to result do not need reallocs */
        STR_ALLOC(&result, tmp + result_pos, &size);
        result_pos += my_snprintf(result + result_pos, tmp, "%s", token + 1);
        /* fputs(token + 1, fd); */
      } else if (value && value[0]!='\0') {
         /* instance names (name) and node labels (lab) go thru the expandlabel function. */
        /*if something else must be parsed, put an if here! */

        if (!(strcmp(token+1,"name") && strcmp(token+1,"lab"))  /* expand name/labels */
              && ((lab = expandlabel(value, &tmp)) != NULL)) {

          tmp = strlen(lab) +100 ; /* always make room for some extra chars 
                                    * so 1-char writes to result do not need reallocs */
          STR_ALLOC(&result, tmp + result_pos, &size);
          result_pos += my_snprintf(result + result_pos, tmp, "%s", lab);
          /* fputs(lab,fd); */


 
        } else { 

          tmp = strlen(value) +100 ; /* always make room for some extra chars 
                                      * so 1-char writes to result do not need reallocs */
          STR_ALLOC(&result, tmp + result_pos, &size);
          result_pos += my_snprintf(result + result_pos, tmp, "%s", value);
          /* fputs(value,fd); */
        }
      }
      else if (strcmp(token,"@symname")==0) /* of course symname must not be present  */
                                            /* in hash table */
      {
        const char *s = skip_dir(xctx->inst[inst].name);
        tmp = strlen(s) +100 ; /* always make room for some extra chars 
                                * so 1-char writes to result do not need reallocs */
        STR_ALLOC(&result, tmp + result_pos, &size);
        result_pos += my_snprintf(result + result_pos, tmp, "%s", s);
        /* fputs(s,fd); */
      }
      else if (strcmp(token,"@symname_ext")==0) /* of course symname must not be present  */
                                            /* in hash table */
      {
        const char *s = get_cell_w_ext(xctx->inst[inst].name, 0);
        tmp = strlen(s) +100 ; /* always make room for some extra chars 
                                * so 1-char writes to result do not need reallocs */
        STR_ALLOC(&result, tmp + result_pos, &size);
        result_pos += my_snprintf(result + result_pos, tmp, "%s", s);
        /* fputs(s,fd); */
      }
      else if(strcmp(token,"@schname")==0) /* of course schname must not be present  */
                                           /* in hash table */
      {
        tmp = strlen(xctx->current_name) +100 ; /* always make room for some extra chars 
                                                * so 1-char writes to result do not need reallocs */
        STR_ALLOC(&result, tmp + result_pos, &size);
        result_pos += my_snprintf(result + result_pos, tmp, "%s", xctx->current_name);
        /* fputs(xctx->current_name, fd); */

      }
      else if(strcmp(token,"@pinlist")==0) /* of course pinlist must not be present  */
                                           /* in hash table. print multiplicity */
      {                                    /* and node number: m1 n1 m2 n2 .... */
        for(i=0;i<no_of_pins;i++)
        {
          char *prop = (xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][i].prop_ptr;
          if(strcmp(get_tok_value(prop, "spice_ignore", 0), "true")) {
            str_ptr =  net_name(inst,i, &mult, 0);

            tmp = strlen(str_ptr) +100 ; /* always make room for some extra chars 
                                          * so 1-char writes to result do not need reallocs */
            STR_ALLOC(&result, tmp + result_pos, &size);
            result_pos += my_snprintf(result + result_pos, tmp, "?%d %s ", mult, str_ptr);
          }
        }
      }
      else if(token[0]=='@' && token[1]=='@') {    /* recognize single pins 15112003 */
        for(i=0;i<no_of_pins;i++) {
          char *prop = (xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][i].prop_ptr;
          if (!strcmp( get_tok_value(prop,"name",0), token+2)) {
            if(strcmp(get_tok_value(prop,"spice_ignore",0), "true")) {
              str_ptr =  net_name(inst,i, &mult, 0);

              tmp = strlen(str_ptr) +100 ; /* always make room for some extra chars 
                                            * so 1-char writes to result do not need reallocs */
              STR_ALLOC(&result, tmp + result_pos, &size);
              result_pos += my_snprintf(result + result_pos, tmp, "?%d %s ", mult, str_ptr);
            }
            break;
          }
        }
      }
      /* reference by pin number instead of pin name, allows faster lookup of the attached net name 20180911 */
      else if (token[0]=='@' && token[1]=='#') {
        pin_number = atoi(token+2);
        if (pin_number < no_of_pins) {
          const char *si;
          char *prop = (xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][pin_number].prop_ptr;
          si  = get_tok_value(prop, "spice_ignore",0);
          if(strcmp(si, "true")) {
            str_ptr =  net_name(inst,pin_number, &mult, 0);

            tmp = strlen(str_ptr) +100 ; /* always make room for some extra chars 
                                          * so 1-char writes to result do not need reallocs */
            STR_ALLOC(&result, tmp + result_pos, &size);
            result_pos += my_snprintf(result + result_pos, tmp, "?%d %s ", mult, str_ptr);
          }
        }
      }
      else if (!strncmp(token,"@tcleval", 8)) {
        size_t s;
        char *tclcmd=NULL;
        const char *res;
        s = token_pos + strlen(name) + strlen(xctx->inst[inst].name) + 100;
        tclcmd = my_malloc(488, s);
        Tcl_ResetResult(interp);
        my_snprintf(tclcmd, s, "tclpropeval {%s} {%s} {%s}", token, name, xctx->inst[inst].name);
        dbg(1, "print_spice_element(): tclpropeval {%s} {%s} {%s}", token, name, xctx->inst[inst].name);
        res = tcleval(tclcmd);

        tmp = strlen(res) + 100; /* always make room for some extra chars 
                                  * so 1-char writes to result do not need reallocs */
        STR_ALLOC(&result, tmp + result_pos, &size);
        result_pos += my_snprintf(result + result_pos, tmp, "%s", res);
        /* fprintf(fd, "%s", tclresult()); */
        my_free(1018, &tclcmd);
      } /* /20171029 */


      if(c != '$' && c != '@' && c!='\0' ) {
        result_pos += my_snprintf(result + result_pos, 2, "%c", c); /* no realloc needed */
        /* fputc(c,fd); */
      }
      if(c == '@' || c == '$' ) s--;
      state=TOK_BEGIN;
    }
    else if(state==TOK_BEGIN && c!='\0') {
      result_pos += my_snprintf(result + result_pos, 2, "%c", c); /* no realloc needed */
      /* fputc(c,fd); */
    }
    if(c=='\0')
    {
      result_pos += my_snprintf(result + result_pos, 2, "%c", '\n'); /* no realloc needed */
      /* fputc('\n',fd); */
      break;
    }
  } /* while(1) */


  /* if result is like: 'tcleval(some_string)' pass it thru tcl evaluation so expressions
   * can be calculated */

  /* do one level of substitutions to resolve @params and equations*/
  if(result && strstr(result, "tcleval(")== result) {
    dbg(1, "print_spice_element(): before translate()result=%s\n", result);
    my_strdup(22, &result, translate(inst, result));
    dbg(1, "print_spice_element(): after  translate()result=%s\n", result);
  }
  /* do a second round of substitutions, but without calling tcl */
  if(result && strstr(result, "eval(") == result) {
    char *c = strrchr(result, ')');
    if(c) while(1) { /* shift following characters back 1 char */
      *c = c[1];
      c++;
      if(!*c) break;
    }
    my_strdup2(88, &result, translate(inst, result+5));
  }
  fprintf(fd, "%s", result);
  my_free(1019, &template);
  my_free(1020, &format);
  my_free(1021, &name);
  my_free(1022, &token);
  my_free(1194, &result);
}

void print_tedax_element(FILE *fd, int inst)
{
 int i=0, mult;
 const char *str_ptr=NULL;
 register int c, state=TOK_BEGIN, space;
 char *template=NULL,*format=NULL,*s, *name=NULL, *token=NULL;
 const char *value;
 char *extra=NULL, *extra_pinnumber=NULL;
 char *numslots=NULL;
 int pin_number;
 const char *extra_token, *extra_token_val;
 char *extra_ptr;
 char *extra_pinnumber_token, *extra_pinnumber_ptr;
 char *saveptr1, *saveptr2;
 const char *tmp;
 int instance_based=0;
 int sizetok=0;
 int token_pos=0, escape=0;
 int no_of_pins=0;
 /* struct inst_hashentry *ptr; */

 my_strdup(489, &extra, get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->prop_ptr,"extra",2));
 my_strdup(41, &extra_pinnumber, get_tok_value(xctx->inst[inst].prop_ptr,"extra_pinnumber",2));
 if(!extra_pinnumber) my_strdup(490, &extra_pinnumber,
         get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->prop_ptr,"extra_pinnumber",2));
 my_strdup(491, &template,
     (xctx->inst[inst].ptr+ xctx->sym)->templ);
 my_strdup(492, &numslots, get_tok_value(xctx->inst[inst].prop_ptr,"numslots",2));
 if(!numslots) my_strdup(493, &numslots, get_tok_value(template,"numslots",2));
 if(!numslots) my_strdup(494, &numslots, "1");

 my_strdup(495, &name,xctx->inst[inst].instname);
 /* my_strdup(496, &name,get_tok_value(xctx->inst[inst].prop_ptr,"name",0)); */
 if(!name) my_strdup(2, &name, get_tok_value(template, "name", 0));

 /* allow format string override in instance */
 my_strdup(1185, &format, get_tok_value(xctx->inst[inst].prop_ptr,"tedax_format",2));
 if(!format || !format[0])
   my_strdup(497, &format, get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->prop_ptr,"tedax_format",2));
 if(name==NULL || !format || !format[0]) {
   my_free(1023, &extra);
   my_free(1024, &extra_pinnumber);
   my_free(1025, &template);
   my_free(1026, &numslots);
   my_free(1027, &format);
   my_free(1028, &name);
   return;
 }
 no_of_pins= (xctx->inst[inst].ptr+ xctx->sym)->rects[PINLAYER];

 fprintf(fd, "begin_inst %s numslots %s\n", name, numslots);
 for(i=0;i<no_of_pins; i++) {
   char *pinnumber;
   pinnumber = get_pin_attr_from_inst(inst, i, "pinnumber");
   if(!pinnumber) {
     my_strdup2(500, &pinnumber,
            get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][i].prop_ptr,"pinnumber",0));
   }
   if(!get_tok_size) my_strdup(501, &pinnumber, "--UNDEF--");
   tmp = net_name(inst,i, &mult, 0);
   if(tmp && strcmp(tmp, "__UNCONNECTED_PIN__")) {
     fprintf(fd, "conn %s %s %s %s %d\n",
           name,
           tmp,
           get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][i].prop_ptr,"name",0),
           pinnumber,
           i+1);
   }
   my_free(1029, &pinnumber);
 }

 if(extra){
   char netstring[40];
   /* fprintf(errfp, "extra_pinnumber: |%s|\n", extra_pinnumber); */
   /* fprintf(errfp, "extra: |%s|\n", extra); */
   for(extra_ptr = extra, extra_pinnumber_ptr = extra_pinnumber; ; extra_ptr=NULL, extra_pinnumber_ptr=NULL) {
     extra_pinnumber_token=my_strtok_r(extra_pinnumber_ptr, " ", &saveptr1);
     extra_token=my_strtok_r(extra_ptr, " ", &saveptr2);
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
     i++;
     if(instance_based) fprintf(fd, " # instance_based");
     fprintf(fd,"\n");
   }
 }

 if(format) {
  s=format;
  dbg(1, "print_tedax_element(): name=%s, tedax_format=%s netlist_count=%d\n",name,format, netlist_count);
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

   if( state==TOK_BEGIN && (c=='$' || c=='@') && !escape) state=TOK_TOKEN;
   else if(state==TOK_TOKEN && token_pos > 1 &&
      (
        ( (space  || c == '$' || c == '@') && !escape ) ||
        ( (!space && c != '$' && c != '@') && escape  )
      )
     ) {
     state=TOK_SEP;
   }

   STR_ALLOC(&token, token_pos, &sizetok);
   if(state==TOK_TOKEN) {
     token[token_pos++]=c; /* 20171029 remove escaping backslashes */
   }
   else if(state==TOK_SEP)                   /* got a token */
   {
    token[token_pos]='\0';
    token_pos=0;

    value = get_tok_value(xctx->inst[inst].prop_ptr, token+1, 0);
    if(!get_tok_size) value=get_tok_value(template, token+1, 0);
    if(!get_tok_size && token[0] =='$') {
      fputs(token + 1, fd);
    } else if(value[0]!='\0')
    {
      fputs(value,fd);
    }
    else if(strcmp(token,"@symname")==0)        /* of course symname must not be present  */
                                        /* in hash table */
    {
     fputs(skip_dir(xctx->inst[inst].name),fd);
    }
    else if (strcmp(token,"@symname_ext")==0) 
    {
      fputs(get_cell_w_ext(xctx->inst[inst].name, 0), fd);
    }
    else if(strcmp(token,"@schname")==0)        /* of course schname must not be present  */
                                        /* in hash table */
    {
     /* fputs(xctx->sch[xctx->currsch],fd); */
     fputs(xctx->current_name, fd);
    }
    else if(strcmp(token,"@pinlist")==0)        /* of course pinlist must not be present  */
                                        /* in hash table. print multiplicity */
    {                                   /* and node number: m1 n1 m2 n2 .... */
     for(i=0;i<no_of_pins;i++)
     {
       str_ptr =  net_name(inst,i, &mult, 0);
       /* fprintf(errfp, "inst: %s  --> %s\n", name, str_ptr); */
       fprintf(fd, "?%d %s ", mult, str_ptr);
     }
    }
    else if(token[0]=='@' && token[1]=='@') {    /* recognize single pins 15112003 */
     for(i=0;i<no_of_pins;i++) {
      if(!strcmp(
           get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][i].prop_ptr,"name",0),
           token+2
          )
        ) {
        str_ptr =  net_name(inst,i, &mult, 0);
        fprintf(fd, "%s", str_ptr);
        break;
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
    } else if(token[0]=='@' && token[1]=='#') {
      if( strchr(token, ':') )  {

        int n;
        char *subtok = my_malloc(503, sizetok * sizeof(char));
        char *subtok2 = my_malloc(42, sizetok * sizeof(char)+20);
        subtok[0]='\0';
        n=-1;
        sscanf(token+2, "%d:%s", &n, subtok);
        if(n!=-1 && subtok[0]) {
          my_snprintf(subtok2, sizetok * sizeof(char)+20, "%s(%d)", subtok, n);
          value = get_tok_value(xctx->inst[inst].prop_ptr,subtok2,0);
          if( n>=0 && n < (xctx->inst[inst].ptr+ xctx->sym)->rects[PINLAYER]) {
            if(!value[0])
              value = get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][n].prop_ptr,subtok,0);
          }
          if(value[0]) {
            char *ss;
            int slot;
            if( (ss=strchr(xctx->inst[inst].instname, ':')) ) {
              sscanf(ss+1, "%d", &slot);
              if(strstr(value, ":")) value = find_nth(value, ':', slot);
            }
            fprintf(fd, "%s", value);
          }
        }
        my_free(1030, &subtok);
        my_free(1031, &subtok2);
      } else {
        /* reference by pin number instead of pin name, allows faster lookup of the attached net name */
        /* @#n --> return net name attached to pin of index 'n' */
        pin_number = atoi(token+2);
        if(pin_number < no_of_pins) {
          str_ptr =  net_name(inst,pin_number, &mult, 0);
          fprintf(fd, "%s", str_ptr);
        }
      }
    }
    else if(!strncmp(token,"@tcleval", 8)) {
      /* char tclcmd[strlen(token)+100] ; */
      size_t s;
      char *tclcmd=NULL;
      s = token_pos + strlen(name) + strlen(xctx->inst[inst].name) + 100;
      tclcmd = my_malloc(504, s);
      Tcl_ResetResult(interp);
      my_snprintf(tclcmd, s, "tclpropeval {%s} {%s} {%s}", token, name, xctx->inst[inst].name);
      tcleval(tclcmd);
      fprintf(fd, "%s", tclresult());
      my_free(1032, &tclcmd);
      /* fprintf(errfp, "%s\n", tclcmd); */
    } /* /20171029 */


    if(c!='$' && c!='@' && c!='\0') fputc(c,fd);
    if(c == '@' || c == '$' ) s--;
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
 fprintf(fd,"end_inst\n");
 my_free(1033, &extra);
 my_free(1034, &extra_pinnumber);
 my_free(1035, &template);
 my_free(1036, &numslots);
 my_free(1037, &format);
 my_free(1038, &name);
 my_free(1039, &token);
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
 int i=0, mult, tmp;
 const char *str_ptr;
 const char *lab;
 char *name=NULL;
 char  *generic_type=NULL;
 char *template=NULL,*s;
 int no_of_pins=0;
 int  tmp1 = 0;
 register int c, state=TOK_BEGIN, space;
 char *value=NULL,  *token=NULL;
 int sizetok=0, sizeval=0;
 int token_pos=0, value_pos=0;
 int quote=0;

 if(get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->prop_ptr,"verilog_format",2)[0] != '\0') {
  print_verilog_primitive(fd, inst);
  return;
 }
 my_strdup(506, &template,
     (xctx->inst[inst].ptr+ xctx->sym)->templ);

 my_strdup(507, &name,xctx->inst[inst].instname);
 if(!name) my_strdup(3, &name, get_tok_value(template, "name", 0));
 if(name==NULL) {
   my_free(1040, &template);
   my_free(1041, &name);
   return;
 }
 no_of_pins= (xctx->inst[inst].ptr+ xctx->sym)->rects[PINLAYER];

 /* 20080915 use generic_type property to decide if some properties are strings, see later */
 my_strdup(505, &generic_type, get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->prop_ptr,"generic_type",2));

 s=xctx->inst[inst].prop_ptr;

/* print instance  subckt */
  dbg(2, "print_verilog_element(): printing inst name & subcircuit name\n");
  fprintf(fd, "%s\n", skip_dir(xctx->inst[inst].name) );

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
  if(state==TOK_TOKEN) token[token_pos++]=c;
  else if(state==TOK_VALUE)
  {
    value[value_pos++]=c;
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
   if(strcmp(token, "name") && get_tok_size) {
     if(value[0] != '\0') /* token has a value */
     {
       if(strcmp(token,"spice_ignore") && strcmp(token,"vhdl_ignore") && strcmp(token,"tedax_ignore")) {
         if(tmp == 0) {fprintf(fd, "#(\n---- start parameters\n");tmp++;tmp1=0;}
         if(tmp1) fprintf(fd, " ,\n");
         if( !generic_type || strcmp(get_tok_value(generic_type,token, 2), "time")  ) {
           if( generic_type && !strcmp(get_tok_value(generic_type,token, 2), "string")  ) {
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
 for(i=0;i<no_of_pins;i++)
 {
   xSymbol *ptr = xctx->inst[inst].ptr+ xctx->sym;
   if(strcmp(get_tok_value(ptr->rect[PINLAYER][i].prop_ptr,"verilog_ignore",0), "true")) {
     if( (str_ptr =  net_name(inst,i, &mult, 0)) )
     {
       if(tmp) fprintf(fd,"\n");
       fprintf(fd, "  ?%d %s %s ", mult,
         get_tok_value(ptr->rect[PINLAYER][i].prop_ptr,"name",0),
         str_ptr);
       tmp=1;
     }
   }
 }
 fprintf(fd, "\n);\n\n");
 dbg(2, "print_verilog_element(): ------- end ------ \n");
 my_free(1042, &name);
 my_free(1043, &generic_type);
 my_free(1044, &template);
 my_free(1045, &value);
 my_free(1046, &token);
}


const char *net_name(int i, int j, int *mult, int hash_prefix_unnamed_net)
{
 int tmp;
 char errstr[2048];
 static const char unconn[]="__UNCONNECTED_PIN__";
 char str_node[40]; /* 20161122 overflow safe */
 if(xctx->inst[i].node && xctx->inst[i].node[j]!=NULL)
 {
  if((xctx->inst[i].node[j])[0] == '#') /* unnamed net */
  {
   /* get unnamed node multiplicity ( minimum mult found in circuit) */
   *mult = get_unnamed_node(3, 0, atoi((xctx->inst[i].node[j])+4) );
    dbg(2, "net_name(): node = %s  n=%d mult=%d\n",
     xctx->inst[i].node[j], atoi(xctx->inst[i].node[j]), *mult);
   if(hash_prefix_unnamed_net) {
     if(*mult>1)   /* unnamed is a bus */
      my_snprintf(str_node, S(str_node), "%s[%d:0]", (xctx->inst[i].node[j]), *mult-1);
     else
      my_snprintf(str_node, S(str_node), "%s", (xctx->inst[i].node[j]) );
   } else {
     if(*mult>1)   /* unnamed is a bus */
      my_snprintf(str_node, S(str_node), "%s[%d:0]", (xctx->inst[i].node[j])+1, *mult-1);
     else
      my_snprintf(str_node, S(str_node), "%s", (xctx->inst[i].node[j])+1 );
   }
   expandlabel(get_tok_value(
           (xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][j].prop_ptr,"name",1), mult);
   return expandlabel(str_node, &tmp);
  }
  else
  {
   expandlabel(get_tok_value(
           (xctx->inst[i].ptr+ xctx->sym)->rect[PINLAYER][j].prop_ptr,"name",1), mult);
   return expandlabel(xctx->inst[i].node[j], &tmp);
  }
 }
 else
 {
   *mult=1;

   my_snprintf(errstr, S(errstr), "Warning: unconnected pin,  Inst idx: %d, Pin idx: %d  Inst:%s\n",
               i, j, xctx->inst[i].instname ) ;
   statusmsg(errstr,2);
   if(!netlist_count) {
     xctx->inst[i].flags |=4;
     hilight_nets=1;
   }
   return unconn;
 }
}


void print_vhdl_primitive(FILE *fd, int inst) /* netlist  primitives, 20071217 */
{
 int i=0, mult, tmp;
 const char *str_ptr;
 register int c, state=TOK_BEGIN, space;
 const char *lab;
 char *template=NULL,*format=NULL,*s, *name=NULL, *token=NULL;
 const char *value;
 int pin_number;
 int sizetok=0;
 int token_pos=0, escape=0;
 int no_of_pins=0;
 /* struct inst_hashentry *ptr; */

 my_strdup(513, &template, (xctx->inst[inst].ptr+ xctx->sym)->templ);
 my_strdup(514, &name, xctx->inst[inst].instname);
 if(!name) my_strdup(50, &name, get_tok_value(template, "name", 0));

 /* allow format string override in instance */
 my_strdup(1000, &format, get_tok_value(xctx->inst[inst].prop_ptr,"vhdl_format",2));
 if(!format || !format[0])
   my_strdup(516, &format, get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->prop_ptr,"vhdl_format",2));
 if((name==NULL) || (format==NULL) ) {
   my_free(1047, &template);
   my_free(1048, &name);
   my_free(1151, &format);
   return; /*do no netlist unwanted insts(no format) */
 }
 no_of_pins= (xctx->inst[inst].ptr+ xctx->sym)->rects[PINLAYER];
 s=format;
 dbg(1, "print_vhdl_primitive(): name=%s, format=%s netlist_count=%d\n",name,format, netlist_count);

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

  if( state==TOK_BEGIN && (c=='@' || c=='$') && !escape ) state=TOK_TOKEN;
  else if(state==TOK_TOKEN && token_pos > 1 &&
     (
       ( (space  || c == '$' || c == '@') && !escape ) ||
       ( (!space && c != '$' && c != '@') && escape  )
     )
    ) {
    state=TOK_SEP;
  }

  STR_ALLOC(&token, token_pos, &sizetok);
  if(state==TOK_TOKEN) {
    token[token_pos++]=c; /* 20171029 remove escaping backslashes */
  }
  else if(state==TOK_SEP)                    /* got a token */
  {
   token[token_pos]='\0';
   token_pos=0;

   value = get_tok_value(xctx->inst[inst].prop_ptr, token+1, 2);
   if(value[0] == '\0')
   value=get_tok_value(template, token+1, 0);
   if(!get_tok_size && token[0] =='$') {
     fputs(token + 1, fd);
   } else if(value && value[0]!='\0')
   {  /* instance names (name) and node labels (lab) go thru the expandlabel function. */
      /*if something else must be parsed, put an if here! */

    if(!(strcmp(token+1,"name"))) {
      if( (lab=expandlabel(value, &tmp)) != NULL)
         fprintf(fd, "----name(%s)", lab);
      else
         fprintf(fd, "%s", value);
    }
    else if(!(strcmp(token+1,"lab"))) {
      if( (lab=expandlabel(value, &tmp)) != NULL)
         fprintf(fd, "----pin(%s)", lab);
      else
         fprintf(fd, "%s", value);
    }
    else  fprintf(fd, "%s", value);
   }
   else if(strcmp(token,"@symname")==0) /* of course symname must not be present  */
                                        /* in hash table */
   {
    fprintf( fd, "%s",skip_dir(xctx->inst[inst].name) );
   }
   else if (strcmp(token,"@symname_ext")==0) 
   {
     fputs(get_cell_w_ext(xctx->inst[inst].name, 0), fd);
   }
   else if(strcmp(token,"@schname")==0) /* of course schname must not be present  */
                                        /* in hash table */
   {
     /* fputs(xctx->sch[xctx->currsch],fd); */
     fputs(xctx->current_name, fd);
   }
   else if(strcmp(token,"@pinlist")==0) /* of course pinlist must not be present  */
                                        /* in hash table. print multiplicity */
   {                                    /* and node number: m1 n1 m2 n2 .... */
    for(i=0;i<no_of_pins;i++)
    {
      char *prop = (xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][i].prop_ptr;
      if(strcmp(get_tok_value(prop,"vhdl_ignore",0), "true")) {
        str_ptr =  net_name(inst,i, &mult, 0);
        fprintf(fd, "----pin(%s) ", str_ptr);
      }
    }
   }
   else if(token[0]=='@' && token[1]=='@') {    /* recognize single pins 15112003 */
    for(i=0;i<no_of_pins;i++) {
     xSymbol *ptr = xctx->inst[inst].ptr+ xctx->sym;
     if(!strcmp( get_tok_value(ptr->rect[PINLAYER][i].prop_ptr,"name",0), token+2)) {
       if(strcmp(get_tok_value(ptr->rect[PINLAYER][i].prop_ptr,"vhdl_ignore",0), "true")) {
         str_ptr =  net_name(inst,i, &mult, 0);
         fprintf(fd, "----pin(%s) ", str_ptr);
       }
       break;
     }
    }
   }
   /* reference by pin number instead of pin name, allows faster lookup of the attached net name 20180911 */
   else if(token[0]=='@' && token[1]=='#') {
       pin_number = atoi(token+2);
       if(pin_number < no_of_pins) {
         char *prop = (xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][pin_number].prop_ptr;
         if(strcmp(get_tok_value(prop,"vhdl_ignore",0), "true")) {
           str_ptr =  net_name(inst,pin_number, &mult, 0);
           fprintf(fd, "----pin(%s) ", str_ptr);
         }
       }
   }
   else if(!strncmp(token,"@tcleval", 8)) {
     /* char tclcmd[strlen(token)+100] ; */
     size_t s;
     char *tclcmd=NULL;
     s = token_pos + strlen(name) + strlen(xctx->inst[inst].name) + 100;
     tclcmd = my_malloc(518, s);
     Tcl_ResetResult(interp);
     my_snprintf(tclcmd, s, "tclpropeval {%s} {%s} {%s}", token, name, xctx->inst[inst].name);
     tcleval(tclcmd);
     fprintf(fd, "%s", tclresult());
     my_free(1049, &tclcmd);
   }

   if(c!='$' && c!='@' && c!='\0' ) fputc(c,fd);
   if(c == '@' || c == '$') s--;
   state=TOK_BEGIN;
  }
  else if(state==TOK_BEGIN && c!='\0')  fputc(c,fd);

  if(c=='\0')
  {
   fputc('\n',fd);
   fprintf(fd, "---- end primitive\n");
   break ;
  }
 }
 my_free(1050, &template);
 my_free(1051, &format);
 my_free(1052, &name);
 my_free(1053, &token);
}

/* print verilog element if verilog_format is specified */
void print_verilog_primitive(FILE *fd, int inst) /* netlist switch level primitives, 15112003 */
{
  int i=0, mult, tmp;
  const char *str_ptr;
  register int c, state=TOK_BEGIN, space;
  const char *lab;
  char *template=NULL,*format=NULL,*s=NULL, *name=NULL, *token=NULL;
  const char *value;
  int sizetok=0;
  int token_pos=0, escape=0;
  int no_of_pins=0;
  /* struct inst_hashentry *ptr; */

  my_strdup(519, &template,
      (xctx->inst[inst].ptr+ xctx->sym)->templ);

  my_strdup(520, &name,xctx->inst[inst].instname);
  if(!name) my_strdup(4, &name, get_tok_value(template, "name", 0));

  /* allow format string override in instance */
  my_strdup(1186, &format, get_tok_value(xctx->inst[inst].prop_ptr,"verilog_format",2));
  if(!format || !format[0])
    my_strdup(522, &format, get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->prop_ptr,"verilog_format",2));
  if((name==NULL) || (format==NULL) ) {
    my_free(1054, &template);
    my_free(1055, &name);
    my_free(1056, &format);
    return; /*do no netlist unwanted insts(no format) */
  }
  no_of_pins= (xctx->inst[inst].ptr+ xctx->sym)->rects[PINLAYER];
  s=format;
  dbg(1, "print_verilog_primitive(): name=%s, format=%s netlist_count=%d\n",name,format, netlist_count);

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
   if( state==TOK_BEGIN && (c=='@' || c=='$') && !escape ) state=TOK_TOKEN;
   else if(state==TOK_TOKEN && token_pos > 1 &&
      (
        ( (space  || c == '$' || c == '@') && !escape ) || 
        ( (!space && c != '$' && c != '@') && escape  )
      )
     
     ) { 
     state=TOK_SEP;
   }

   STR_ALLOC(&token, token_pos, &sizetok);
   if(state==TOK_TOKEN) {
      token[token_pos++]=c;
   }
   else if(state==TOK_SEP)                    /* got a token */
   {
    token[token_pos]='\0';
    token_pos=0;

    value = get_tok_value(xctx->inst[inst].prop_ptr, token+1, 2);
    if(value[0] == '\0')
    value=get_tok_value(template, token+1, 0);
    if(!get_tok_size && token[0] =='$') {
      fputs(token + 1, fd);
    } else if(value && value[0]!='\0') {
       /* instance names (name) and node labels (lab) go thru the expandlabel function. */
       /*if something else must be parsed, put an if here! */

     if(!(strcmp(token+1,"name"))) {
       if( (lab=expandlabel(value, &tmp)) != NULL)
          fprintf(fd, "----name(%s)", lab);
       else
          fprintf(fd, "%s", value);
     }
     else if(!(strcmp(token+1,"lab"))) {
       if( (lab=expandlabel(value, &tmp)) != NULL)
          fprintf(fd, "----pin(%s)", lab);
       else
          fprintf(fd, "%s", value);
     }
     else  fprintf(fd, "%s", value);
    }
    else if(strcmp(token,"@symname")==0) /* of course symname must not be present  */
                                         /* in hash table */
    {
     fprintf( fd, "%s",skip_dir(xctx->inst[inst]. name) );
    }
    else if (strcmp(token,"@symname_ext")==0) 
    {
      fputs(get_cell_w_ext(xctx->inst[inst].name, 0), fd);
    }
    else if(strcmp(token,"@schname")==0) /* of course schname must not be present  */
                                         /* in hash table */
    {
      /* fputs(xctx->sch[xctx->currsch],fd); */
      fputs(xctx->current_name, fd);
    }
    else if(strcmp(token,"@pinlist")==0) /* of course pinlist must not be present  */
                                         /* in hash table. print multiplicity */
    {                                    /* and node number: m1 n1 m2 n2 .... */
     for(i=0;i<no_of_pins;i++)
     {
       char *prop = (xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][i].prop_ptr;
       if(strcmp(get_tok_value(prop, "verilog_ignore",0), "true")) {
         str_ptr =  net_name(inst,i, &mult, 0);
         fprintf(fd, "----pin(%s) ", str_ptr);
       }
     }
    }
    else if(token[0]=='@' && token[1]=='@') {    /* recognize single pins 15112003 */
     for(i=0;i<no_of_pins;i++) {
      char *prop = (xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][i].prop_ptr;
      if(!strcmp( get_tok_value(prop,"name",0), token+2)) {
        if(strcmp(get_tok_value(prop, "verilog_ignore",0), "true")) {
          str_ptr =  net_name(inst,i, &mult, 0);
          fprintf(fd, "----pin(%s) ", str_ptr);
        }
        break;
      }
     }
    }
    /* reference by pin number instead of pin name, allows faster lookup of the attached net name 20180911 */
    else if(token[0]=='@' && token[1]=='#') {
      int pin_number = atoi(token+2);
      if(pin_number < no_of_pins) {
        const char *vi;
        char *prop = (xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][pin_number].prop_ptr;
        vi = get_tok_value(prop,"verilog_ignore",0);
        if(strcmp(vi, "true")) {
          str_ptr =  net_name(inst,pin_number, &mult, 0);
          fprintf(fd, "----pin(%s) ", str_ptr);
        }
      }
    }
    else if(!strncmp(token,"@tcleval", 8)) {
      /* char tclcmd[strlen(token)+100] ; */
      size_t s;
      char *tclcmd=NULL;
      s = token_pos + strlen(name) + strlen(xctx->inst[inst].name) + 100;
      tclcmd = my_malloc(524, s);
      Tcl_ResetResult(interp);
      my_snprintf(tclcmd, s, "tclpropeval {%s} {%s} {%s}", token, name, xctx->inst[inst].name);
      tcleval(tclcmd);
      fprintf(fd, "%s", tclresult());
      my_free(1057, &tclcmd);
    }
    if(c!='$' && c!='@' && c!='\0') fputc(c,fd);
    if(c == '@' || c == '$') s--;
    state=TOK_BEGIN;
   }
   else if(state==TOK_BEGIN && c!='\0')  fputc(c,fd);
   if(c=='\0')
   {
    fputc('\n',fd);
    fprintf(fd, "---- end primitive\n");
    break ;
   }
  }
  my_free(1058, &template);
  my_free(1059, &format);
  my_free(1060, &name);
  my_free(1061, &token);
}

int isonlydigit(const char *s)
{
    char c;
    if(s == NULL || *s == '\0') return 0;
    while( (c = *s) ) {
     if(c < '0' || c > '9') return 0;
     s++;
    }
    return 1;
}


const char *find_nth(const char *str, char sep, int n)
{
  static char *result=NULL;
  static char empty[]="";
  int i;
  char *ptr;
  int count;

  if(!str) {
    my_free(1062, &result);
    return empty;
  }
  my_strdup(525, &result, str);
  if(!result) return empty;
  for(i=0, count=1, ptr=result; result[i] != 0; i++) {
    if(result[i]==sep) {
      result[i]=0;
      if(count==n) {
        return ptr;
      }
      ptr=result+i+1;
      count++;
    }
  }
  if(count==n) return ptr;
  else return empty;
}

/* substitute given tokens in a string with their corresponding values */
/* ex.: name=@name w=@w l=@l ---> name=m112 w=3e-6 l=0.8e-6 */
/* if s==NULL return emty string */
const char *translate(int inst, const char* s)
{
 static char empty[]="";
 static char *result=NULL;
 int size=0, tmp;
 register int c, state=TOK_BEGIN, space;
 char *token=NULL;
 const char *tmp_sym_name;
 int sizetok=0;
 int result_pos=0, token_pos=0;
 /* struct inst_hashentry *ptr; */
 struct stat time_buf;
 struct tm *tm;
 char file_name[PATH_MAX];
 const char *value;
 int escape=0;
 char date[200];

 if(!s) {
   my_free(1063, &result);
   return empty;
 }

 size=CADCHUNKALLOC;
 my_realloc(527, &result,size);
 result[0]='\0';

 dbg(1, "translate(): substituting props in <%s>, instance <%s>\n",
        s?s:"NULL",xctx->inst[inst].instname?xctx->inst[inst].instname:"NULL");

 while(1)
 {
  c=*s++;
  if(c=='\\') {
    escape=1;
    c=*s++;
  }
  else escape=0;
  space=SPACE(c);
  if( state==TOK_BEGIN && (c=='@' || c=='$' ) && !escape  ) state=TOK_TOKEN; /* 20161210 escape */
  else if( state==TOK_TOKEN && (
                              (space && !escape)  ||
                               (c =='@' || c == '$') ||
                              (!space && escape)
                            )
                         && token_pos > 1 ) state=TOK_SEP;


  STR_ALLOC(&result, result_pos, &size);
  STR_ALLOC(&token, token_pos, &sizetok);
  if(state==TOK_TOKEN) token[token_pos++]=c;
  else if(state==TOK_SEP)
  {
   token[token_pos]='\0';
   dbg(2, "translate(): token=%s\n", token);

   if(!spiceprefix && !strcmp(token, "@spiceprefix")) {
     value = NULL;
     get_tok_size = 0;
   } else {
     value = get_tok_value(xctx->inst[inst].prop_ptr, token+1, 2);
     if(!get_tok_size) value=get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->templ, token+1, 2);
   }

   if(!get_tok_size && token[0] =='$') {
    char *env = getenv(token + 1);
    if(env) { /* do environment var substitution if no xschem definition for $token */
      tmp = strlen(env);
      STR_ALLOC(&result, tmp + result_pos, &size);
      memcpy(result+result_pos, env, tmp+1);
    } else { /* no definition found -> subst with token without leading $ */
      tmp=token_pos -1 ; /* we need token_pos -1 chars, ( strlen(token+1) ) , excluding leading '$' */
      STR_ALLOC(&result, tmp + result_pos, &size);
      dbg(2, "translate(): token=%s, token_pos = %d\n", token, token_pos);
      memcpy(result+result_pos, token + 1, tmp+1);
    }
    result_pos+=tmp;
   }
   token_pos = 0;
   if(get_tok_size) {
    tmp=get_tok_value_size;  /* strlen(value); */
    STR_ALLOC(&result, tmp + result_pos, &size);
    memcpy(result+result_pos, value, tmp+1);
    result_pos+=tmp;
   } else if(strcmp(token,"@symname")==0) {
    tmp_sym_name=xctx->inst[inst].name ? get_cell(xctx->inst[inst].name, 0) : "";
    tmp=strlen(tmp_sym_name);
    STR_ALLOC(&result, tmp + result_pos, &size);
    memcpy(result+result_pos,tmp_sym_name, tmp+1);
    result_pos+=tmp;
   } else if(strcmp(token,"@symname_ext")==0) {
    tmp_sym_name=xctx->inst[inst].name ? get_cell_w_ext(xctx->inst[inst].name, 0) : "";
    tmp=strlen(tmp_sym_name);
    STR_ALLOC(&result, tmp + result_pos, &size);
    memcpy(result+result_pos,tmp_sym_name, tmp+1);
    result_pos+=tmp;
   } else if(token[0]=='@' && token[1]=='@') {    /* recognize single pins 15112003 */
     int i, mult;
     int no_of_pins= (xctx->inst[inst].ptr+ xctx->sym)->rects[PINLAYER];
     prepare_netlist_structs(0);
     for(i=0;i<no_of_pins;i++) {
       char *prop = (xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][i].prop_ptr;
       if (!strcmp( get_tok_value(prop,"name",0), token+2)) {
         if(strcmp(get_tok_value(prop,"spice_ignore",0), "true")) {
           const char *str_ptr =  net_name(inst,i, &mult, 0);
           tmp = strlen(str_ptr) +100 ;
           STR_ALLOC(&result, tmp + result_pos, &size);
           result_pos += my_snprintf(result + result_pos, tmp, "%s", str_ptr);
         }
         break;
       }
     }
   } else if(token[0]=='@' && token[1]=='#') {
     int n;
     char *pin_attr = my_malloc(532, sizetok * sizeof(char));
     char *pin_num_or_name = my_malloc(55, sizetok * sizeof(char));

     pin_num_or_name[0]='\0';
     pin_attr[0]='\0';
     n=-1;
     sscanf(token+2, "%[^:]:%[^:]", pin_num_or_name, pin_attr);
     if(isonlydigit(pin_num_or_name)) {
       n = atoi(pin_num_or_name);
     }
     else if(pin_num_or_name[0]) {
       for(n = 0 ; n < (xctx->inst[inst].ptr+ xctx->sym)->rects[PINLAYER]; n++) {
         char *prop = (xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][n].prop_ptr;
         if(!strcmp(get_tok_value(prop,"name",0), pin_num_or_name)) break;
       }
     }
     if(n>=0  && pin_attr[0] && n < (xctx->inst[inst].ptr+ xctx->sym)->rects[PINLAYER]) {
       char *pin_attr_value = NULL;

       /* get pin_attr value from instance: "pinnumber(ENABLE)=5" --> return 5, attr "pinnumber" of pin "ENABLE"
        *                                   "pinnumber(3)=6       --> return 6, attr "pinnumber" of 4th pin */
       if(!pin_attr_value) pin_attr_value = get_pin_attr_from_inst(inst, n, pin_attr);
       /* get pin_attr from instance pin attribute string */
       if(!pin_attr_value) {
        my_strdup(499, &pin_attr_value,
           get_tok_value((xctx->inst[inst].ptr+ xctx->sym)->rect[PINLAYER][n].prop_ptr, pin_attr, 2));
       }

       /* @#n:net_name attribute (n = pin number or name) will translate to net name attached  to pin
        * if 'net_name=true' attribute is set in instance or symbol */
       if(  !pin_attr_value && !strcmp(pin_attr, "net_name")) {
         char *instprop = xctx->inst[inst].prop_ptr;
         char *symprop = (xctx->inst[inst].ptr + xctx->sym)->prop_ptr;
         if( show_pin_net_names && (!strcmp(get_tok_value(instprop, "net_name", 0), "true") ||
            !strcmp(get_tok_value(symprop, "net_name", 0), "true"))) {
            prepare_netlist_structs(0);
            my_strdup2(1175, &pin_attr_value,
                 xctx->inst[inst].node && xctx->inst[inst].node[n] ? xctx->inst[inst].node[n] : "?");
         /* do not show net_name: set to empty string */
         } else {
            my_strdup2(1178, &pin_attr_value, "");
         }
       }
       if(!pin_attr_value ) my_strdup(379, &pin_attr_value, "--UNDEF--");
       value = pin_attr_value;
       /* recognize slotted devices: instname = "U3:3", value = "a:b:c:d" --> value = "c" */
       if(value && value[0] != 0 && !strcmp(pin_attr, "pinnumber") ) {
         char *ss;
         int slot;
         char *tmpstr = NULL;
         tmpstr = my_malloc(1176, sizeof(xctx->inst[inst].instname));
         if( (ss=strchr(xctx->inst[inst].instname, ':')) ) {
           sscanf(ss+1, "%s", tmpstr);
           if(isonlydigit(tmpstr)) {
             slot = atoi(tmpstr);
             if(strstr(value,":")) value = find_nth(value, ':', slot);
           }
         }
         my_free(1177, &tmpstr);
       }
       tmp=strlen(value);
       STR_ALLOC(&result, tmp + result_pos, &size);
       memcpy(result+result_pos, value, tmp+1);
       result_pos+=tmp;
       my_free(1064, &pin_attr_value);
     }
     my_free(1065, &pin_attr);
     my_free(1066, &pin_num_or_name);
   } else if(strcmp(token,"@sch_last_modified")==0) {

    my_strncpy(file_name, abs_sym_path(get_tok_value(
      (xctx->inst[inst].ptr+ xctx->sym)->prop_ptr, "schematic",0 ), "")
      , S(file_name));
    if(!file_name[0]) {
      my_strncpy(file_name, add_ext(abs_sym_path(xctx->inst[inst].name, ""), ".sch"), S(file_name));
    }
    if(!stat(file_name , &time_buf)) {
      tm=localtime(&(time_buf.st_mtime) );
      tmp=strftime(date, sizeof(date), "%Y-%m-%d  %H:%M:%S", tm);
      STR_ALLOC(&result, tmp + result_pos, &size);
      memcpy(result+result_pos, date, tmp+1);
      result_pos+=tmp;
    }
   } else if(strcmp(token,"@sym_last_modified")==0) {
    my_strncpy(file_name, abs_sym_path(xctx->inst[inst].name, ""), S(file_name));
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
   } else if(strcmp(token,"@schname")==0) {
     /* tmp=strlen(xctx->sch[xctx->currsch]);*/
     tmp = strlen(xctx->current_name);
     STR_ALLOC(&result, tmp + result_pos, &size);
     /* memcpy(result+result_pos,xctx->sch[xctx->currsch], tmp+1); */
     memcpy(result+result_pos, xctx->current_name, tmp+1);
     result_pos+=tmp;
   } else if(strcmp(token,"@prop_ptr")==0 && xctx->inst[inst].prop_ptr) {
     tmp=strlen(xctx->inst[inst].prop_ptr);
     STR_ALLOC(&result, tmp + result_pos, &size);
     memcpy(result+result_pos,xctx->inst[inst].prop_ptr, tmp+1);
     result_pos+=tmp;
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
   }

   if(c == '@' || c == '$') s--;
   else result[result_pos++]=c;
   state=TOK_BEGIN;
  }
  else if(state==TOK_BEGIN) result[result_pos++]=c;
  if(c=='\0')
  {
   result[result_pos]='\0';
   break;
  }
 }
 dbg(2, "translate(): returning %s\n", result);
 my_free(1067, &token);

 /* if result is like: 'tcleval(some_string)' pass it thru tcl evaluation so expressions
  * can be calculated */
 tcl_hook(&result);
 return result;
}

const char *translate2(struct Lcc *lcc, int level, char* s)
{
  static char empty[]="";
  static char *result = NULL;
  int i, size = 0, tmp, save_tok_size, save_value_size;
  register int c, state = TOK_BEGIN, space;
  char *token = NULL;
  const char *tmp_sym_name;
  int sizetok = 0;
  int result_pos = 0, token_pos = 0;
  char *value1 = NULL;
  char *value2 = NULL;
  char *value = NULL;
  int escape = 0;

  if(!s) {
    my_free(1068, &result);
    return empty;
  }
  size = CADCHUNKALLOC;
  my_realloc(661, &result, size);
  result[0] = '\0';

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
    if (state == TOK_BEGIN && c == '@' && !escape) state = TOK_TOKEN;
    else if (state == TOK_TOKEN && ( (space && !escape) || c == '@' || (!space && escape)) && token_pos > 1) {
      state = TOK_SEP;
    }
    STR_ALLOC(&result, result_pos, &size);
    STR_ALLOC(&token, token_pos, &sizetok);
    if (state == TOK_TOKEN) token[token_pos++] = c;
    else if (state == TOK_SEP) {
      token[token_pos] = '\0';
      token_pos = 0;

      if(!spiceprefix && !strcmp(token, "@spiceprefix")) {
        my_free(1069, &value1);
        get_tok_size = 0;
      } else {
        my_strdup2(332, &value1, get_tok_value(lcc[level].prop_ptr, token + 1, 2));
      }

      value = "";
      if(get_tok_size) {
        value = value1;
        i = level;
        /* recursive substitution of value using parent level prop_str attributes */
        while(i > 1) {
          save_tok_size = get_tok_size;
          save_value_size = get_tok_value_size;
          my_strdup2(440, &value2, get_tok_value(lcc[i-1].prop_ptr, value, 2));
          if(get_tok_size && value2[0]) {
            value = value2;
          } else {
            /* restore last successful get_tok_value() size parameters */
            get_tok_size = save_tok_size;
            get_tok_value_size = save_value_size;
            break;
          }
          i--;
        }
        tmp = get_tok_value_size;  /* strlen(value); */
        STR_ALLOC(&result, tmp + 1 + result_pos, &size); /* +1 to add leading '$' */
        /* prefix substituted token with a '$' so it will be recognized by translate() for last level translation with
           instance placement prop_ptr attributes  at drawing/netlisting time. */
        memcpy(result + result_pos , "$", 1);
        memcpy(result + result_pos + 1 , value, tmp + 1);
        result_pos += tmp + 1;
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

      if (c == '@') s--;
      else result[result_pos++] = c;
      state = TOK_BEGIN;
    }
    else if (state == TOK_BEGIN) result[result_pos++] = c;
    if (c == '\0') {
      result[result_pos] = '\0';
      break;
    }
  }
  if(result && strstr(result, "tcleval(")== result) {
    tcl_hook(&result);
  }
  my_free(1070, &token);
  my_free(1071, &value1);
  my_free(1072, &value2);
  return result;
}
