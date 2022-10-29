/* File: editprop.c
 *
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
 * simulation.
 * Copyright (C) 1998-2022 Stefan Frederik Schippers
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

#include <stdarg.h>
#include "xschem.h"

double mylog10(double x)
{
  if(x > 0) return log10(x);
  else return -35;
}

double mylog(double x)
{
  if(x > 0) return log(x);
  else return -35;
}

int my_strcasecmp(const char *s1, const char *s2)
{
  while(tolower(*s1) == tolower(*s2)) {
    if (*s1 == '\0') return 0;
    s1++;
    s2++;
  }
  return tolower(*s1) - tolower(*s2);
}

int my_strncasecmp(const char *s1, const char *s2, size_t n)
{
  if (n == 0) return 0;
  while(tolower(*s1) == tolower(*s2)) {
    if (--n == 0) return 0;
    if (*s1 == '\0') return 0;
    s1++;
    s2++;
  }
  return tolower(*s1) - tolower(*s2);
}

/* caller should free allocated storage for s */
char *my_fgets(FILE *fd)
{
  enum { SIZE = 1024 };
  char buf[SIZE];
  char *s = NULL;
  size_t len;

  while(fgets(buf, SIZE, fd)) {
    my_strcat(425, &s, buf);
    len = strlen(buf);
    if(buf[len - 1] == '\n') break;
  }
  return s;
}

/* split a string into tokens like standard strtok_r,
 * if quote string is not empty any character matching quote is considered a quoting
 * character, removed from input and all characters before next quote are considered
 * as part of the token. backslash can be used to enter literal quoting characters and
 * literal backslashes.
 * if quote is empty no backslash is removed from input and behavior is identical
 * to strtok_r
 */
char *my_strtok_r(char *str, const char *delim, const char *quote, char **saveptr)
{
  char *tok;
  int q = 0; /* quote */
  int e = 0; /* escape */
  int ne = 0; /* number of escapes / quoting chars to remove */
  if(str) { /* 1st call */
    *saveptr = str;
  }
  while(**saveptr && strchr(delim, **saveptr) ) { /* skip separators */
    ++(*saveptr);
  }
  tok = *saveptr; /* start of token */
  while(**saveptr && (e || q || !strchr(delim, **saveptr)) ) { /* look for sep. marking end of current token */
    if(ne) *(*saveptr - ne) = **saveptr; /* shift back eating escapes / quotes */
    if(!e && strchr(quote, **saveptr)) {
      q = !q;
      ne++;
    }
    if(quote[0] && !e && **saveptr == '\\') { /* if quote is empty string do not skip backslashes either */
      e = 1;
      ne++;
    } else e = 0;
    ++(*saveptr);
  }
  if(**saveptr) {
    **saveptr = '\0'; /* mark end of token */
    if(ne) *(*saveptr - ne) = **saveptr; /* shift back eating escapes / quotes */
    ++(*saveptr);     /* if not at end of string advance one char for next iteration */
  } else if(ne) *(*saveptr - ne ) = **saveptr; /* shift back eating escapes / quotes */
  if(tok[0]) return tok; /* return token */
  else return NULL; /* no more tokens */
}

size_t my_strdup(int id, char **dest, const char *src) /* empty source string --> dest=NULL */
{
 size_t len;

 if(src!=NULL && src[0]!='\0')  {
   len = strlen(src)+1;
   my_realloc(id, dest, len);
   memcpy(*dest, src, len);
   dbg(3,"my_strdup(%d,): duplicated string %s\n", id, src);
   return len-1;
 } else if(*dest) {
   my_free(1146, dest);
   dbg(3,"my_strdup(%d,): freed destination ptr\n", id);
 }

 return 0;
}

/* 20171004 copy at most n chars, adding a null char at end */
void my_strndup(int id, char **dest, const char *src, size_t n) /* empty source string --> dest=NULL */

{
 if(*dest!=NULL) {
   dbg(3,"  my_strndup:  calling my_free\n");
   my_free(1147, dest);
 }
 if(src!=NULL && src[0]!='\0')
 {
  /* 20180924 replace strndup() */
  char *p = memchr(src, '\0', n);
  if(p) n = p - src;
  *dest = my_malloc(id, n+1);
  if(*dest) {
    memcpy(*dest, src, n);
    (*dest)[n] = '\0';
  }
  /* *dest=strndup(src, n); */

  dbg(3,"my_strndup(%d,): duplicated string %s\n", id, src);
 }
}

void dbg(int level, char *fmt, ...)
{
  if(debug_var>=level) {
    va_list args;
    va_start(args, fmt);
    vfprintf(errfp, fmt, args);
    va_end(args);
  }
}

#ifdef HAS_SNPRINTF
size_t my_snprintf(char *str, size_t size, const char *fmt, ...)
{
  int  size_of_print;
  char s[200];

  va_list args;
  va_start(args, fmt);
  size_of_print = vsnprintf(str, size, fmt, args);

  if(has_x && size_of_print >=size) { /* output was truncated  */
    snprintf(s, S(s), "alert_ { Warning: overflow in my_snprintf print size=%d, buffer size=%d} {}",
             size_of_print, size);
    tcleval(s);
  }
  va_end(args);
  return size_of_print;
}
#else

/*
   this is a replacement for snprintf(), **however** it implements only
   the bare minimum set of formatting used by XSCHEM
*/
size_t my_snprintf(char *string, size_t size, const char *format, ...)
{
  va_list args;
  const char *f, *fmt = NULL, *prev;
  int overflow, format_spec;
  size_t l, n = 0;

  va_start(args, format);

  /* fprintf(errfp, "my_snprintf(): size=%d, format=%s\n", size, format); */
  prev = format;
  format_spec = 0;
  overflow = 0;
  for(f = format; *f; f++) {
    if(*f == '%') {
      format_spec = 1;
      fmt = f;
    }
    if(*f == 's' && format_spec) {
      char *sptr;
      sptr = va_arg(args, char *);
      l = fmt - prev;
      if(n+l > size) {
        overflow = 1;
        break;
      }
      memcpy(string + n, prev, l);
      string[n+l] = '\0';
      n += l;
      l = strlen(sptr);
      if(n+l+1 > size) {
        overflow = 1;
        break;
      }
      memcpy(string + n, sptr, l+1);
      n += l;
      format_spec = 0;
      prev = f + 1;
    }
    else if(format_spec && (*f == 'd' || *f == 'x' || *f == 'c' || *f == 'u') ) {
      char nfmt[50], nstr[50];
      int i, nlen;
      i = va_arg(args, int);
      l = f - fmt+1;
      strncpy(nfmt, fmt, l);
      nfmt[l] = '\0';
      l = fmt - prev;
      if(n+l > size) break;
      memcpy(string + n, prev, l);
      string[n+l] = '\0';
      n += l;
      nlen = sprintf(nstr, nfmt, i);
      if(n + nlen + 1 > size) {
        overflow = 1;
        break;
      }
      memcpy(string +n, nstr, nlen+1);
      n += nlen;
      format_spec = 0;
      prev = f + 1;
    }
    else if(format_spec && (*f == 'g' || *f == 'e' || *f == 'f')) {
      char nfmt[50], nstr[50];
      double i;
      int nlen;
      i = va_arg(args, double);
      l = f - fmt+1;
      strncpy(nfmt, fmt, l);
      nfmt[l] = '\0';
      l = fmt - prev;
      if(n+l > size) {
        overflow = 1;
        break;
      }
      memcpy(string + n, prev, l);
      string[n+l] = '\0';
      n += l;
      nlen = sprintf(nstr, nfmt, i);
      if(n + nlen + 1 > size) {
        overflow = 1;
        break;
      }
      memcpy(string +n, nstr, nlen+1);
      n += nlen;
      format_spec = 0;
      prev = f + 1;
    }
  }
  l = f - prev;
  if(!overflow && n+l+1 <= size) {
    memcpy(string + n, prev, l+1);
    n += l;
  } else {
    dbg(1, "my_snprintf(): overflow, target size=%d, format=%s\n", size, format);
  }

  va_end(args);
  /* fprintf(errfp, "my_snprintf(): returning: |%s|\n", string); */
  return n;
}
#endif /* HAS_SNPRINTF */

size_t my_strdup2(int id, char **dest, const char *src) /* 20150409 duplicates also empty string  */
{
 size_t len;
 if(src!=NULL) {
   len = strlen(src)+1;
   my_realloc(id, dest, len);
   memcpy(*dest, src, len);
   dbg(3,"my_strdup2(%d,): duplicated string %s\n", id, src);
   return len-1;
 } else if(*dest) {
   my_free(1148, dest);
   dbg(3,"my_strdup2(%d,): freed destination ptr\n", id);
 }
 return 0;
}

/* recognizes spice suffixes: 12p --> 1.2e11, 3MEG --> 3e6 */
double atof_spice(const char *s)
{
  int n;
  double a = 0.0, mul=1.0;
  char lower_s[100];
  char suffix[100];
  const char *p;

  if(!s) return 0.0;
  my_strncpy(lower_s, s, S(lower_s));
  strtolower(lower_s);
  n = sscanf(lower_s, "%lf%s", &a, suffix);
  if(n == 0) {
    return 0.0;
  } else if(n == 1) {
    mul = 1.0;
  } else {
    p = strpbrk(suffix, "tgmkunpfa");
    if(p != suffix ) mul = 1.0;
    else if(*p == 't') mul=1e12;
    else if(*p == 'g') mul=1e9;
    else if(*p == 'm') {
      if(strstr(p, "meg") == p) mul=1e6;
      else if(strstr(p, "mil") == p) mul=25.4e-6;
      else mul=1e-3;
    }
    else if(*p == 'k') mul=1e3;
    else if(*p == 'u') mul=1e-6;
    else if(*p == 'n') mul=1e-9;
    else if(*p == 'p') mul=1e-12;
    else if(*p == 'f') mul=1e-15;
    else if(*p == 'a') mul=1e-18;
    else mul = 1.0;
    a *= mul;
  }
  return a;
}

char *my_itoa(int i)
{
  static char s[30];
  size_t n;
  n = my_snprintf(s, S(s), "%d", i);
  if(xctx) xctx->tok_size = n;
  return s;
}

char *dtoa(double i)
{
  static char s[70];
  size_t n;

  n = my_snprintf(s, S(s), "%g", i);
  if(xctx) xctx->tok_size = n;
  return s;
}

char *dtoa_eng(double i)
{
  static char s[70];
  size_t n;
  int suffix = 0;
  double absi = fabs(i);

  if     (absi == 0.0)  { suffix = 0;}
  else if(absi < 1e-23) { i = 0; suffix = 0;}
  else if(absi >=1e12)  { i /= 1e12; suffix = 'T';}
  else if(absi >=1e9)   { i /= 1e9 ; suffix = 'G';}
  else if(absi >=1e6)   { i /= 1e6 ; suffix = 'M';}
  else if(absi >=1e3)   { i /= 1e3 ; suffix = 'k';}
  else if(absi >=0.1)   { suffix = 0;}
  else if(absi >=1e-3)  { i *= 1e3 ; suffix = 'm';}
  else if(absi >=1e-6)  { i *= 1e6 ; suffix = 'u';}
  else if(absi >=1e-9)  { i *= 1e9 ; suffix = 'n';}
  else if(absi >=1e-12) { i *= 1e12; suffix = 'p';}
  else if(absi >=1e-15) { i *= 1e15; suffix = 'f';}
  else                  { i *= 1e18; suffix = 'a';}
  if(suffix) {
    n = my_snprintf(s, S(s), "%.5g%c", i, suffix);
  } else {
    n = my_snprintf(s, S(s), "%.5g", i);
  }
  if(xctx) xctx->tok_size = n;
  return s;
}

char *dtoa_prec(double i)
{
  static char s[70];
  size_t n;
  n = my_snprintf(s, S(s), "%.10e", i);
  if(xctx) xctx->tok_size = n;
  return s;
}

size_t my_mstrcat(int id, char **str, const char *add, ...)
{
  va_list args;
  const char *append_str;
  size_t s, a;

  if(add == NULL) return 0;
  s = 0;
  if(*str != NULL) s = strlen(*str);
  va_start(args, add);
  append_str = add;
  do {
    if( *str != NULL) {
      if(append_str[0]=='\0') continue;
      a = strlen(append_str) + 1;
      my_realloc(id, str, s + a );
      memcpy(*str + s, append_str, a);
      s += a - 1;
      dbg(3,"my_mstrcat(%d,): reallocated string %s\n", id, *str);
    } else {
      if(append_str[0] == '\0') continue;
      a = strlen(append_str) + 1;
      *str = my_malloc(id, a);
      memcpy(*str, append_str, a);
      dbg(3,"my_mstrcat(%d,): allocated string %s\n", id, *str);
      s = a - 1;
    }
    append_str = va_arg(args, const char *);
  } while(append_str);
  va_end(args);
  return s;
}

size_t my_strcat(int id, char **str, const char *append_str)
{
  size_t s, a;
  dbg(3,"my_strcat(%d,): str=%s  append_str=%s\n", id, *str, append_str);
  if( *str != NULL)
  {
    s = strlen(*str);
    if(append_str == NULL || append_str[0]=='\0') return s;
    a = strlen(append_str) + 1;
    my_realloc(id, str, s + a );
    memcpy(*str + s, append_str, a);
    dbg(3,"my_strcat(%d,): reallocated string %s\n", id, *str);
    return s + a - 1;
  } else {
    if(append_str == NULL || append_str[0] == '\0') return 0;
    a = strlen(append_str) + 1;
    *str = my_malloc(id, a);
    memcpy(*str, append_str, a);
    dbg(3,"my_strcat(%d,): allocated string %s\n", id, *str);
    return a - 1;
  }
}

size_t my_strncat(int id, char **str, size_t n, const char *append_str)
{
 size_t s, a;
 dbg(3,"my_strncat(%d,): str=%s  append_str=%s\n", id, *str, append_str);
 a = strlen(append_str) + 1;
 if(a > n + 1) a = n + 1;
 if( *str != NULL)
 {
  s = strlen(*str);
  if(append_str == NULL || append_str[0] == '\0') return s;
  my_realloc(id, str, s + a );
  memcpy(*str + s, append_str, a);
  *(*str + s + a - 1) = '\0';
  dbg(3,"my_strncat(%d,): reallocated string %s\n", id, *str);
  return s + a - 1;
 }
 else
 {
  if(append_str == NULL || append_str[0] == '\0') return 0;
  *str = my_malloc(id,  a );
  memcpy(*str, append_str, a);
  *(*str + a - 1) = '\0';
  dbg(3,"my_strncat(%d,): allocated string %s\n", id, *str);
  return a - 1;
 }
}


void *my_calloc(int id, size_t nmemb, size_t size)
{
   void *ptr;
   if(size*nmemb > 0) {
     ptr=calloc(nmemb, size);
     if(ptr == NULL)
        fprintf(errfp,"my_calloc(%d,): allocation failure %ld * %ld bytes\n", id, nmemb, size);
     dbg(3, "\nmy_calloc(%d,): allocating %p , %lu bytes\n",
               id, ptr, (unsigned long) (size*nmemb));
   }
   else ptr = NULL;
   return ptr;
}

void *my_malloc(int id, size_t size)
{
 void *ptr;
 if(size>0) {
   ptr=malloc(size);
   if(ptr == NULL) fprintf(errfp,"my_malloc(%d,): allocation failure for %ld bytes\n", id, size);
   dbg(3, "\nmy_malloc(%d,): allocating %p , %lu bytes\n", id, ptr, (unsigned long) size);
 }
 else ptr=NULL;
 return ptr;
}

void my_realloc(int id, void *ptr,size_t size)
{
 void *a;
 a = *(void **)ptr;
 if(size == 0) {
   free(*(void **)ptr);
   dbg(3, "\nmy_free(%d,):  my_realloc_freeing %p\n",id, *(void **)ptr);
   *(void **)ptr=NULL;
 } else {
   *(void **)ptr=realloc(*(void **)ptr,size);
    if(*(void **)ptr == NULL) fprintf(errfp,"my_realloc(%d,): allocation failure for %ld bytes\n", id, size);
   dbg(3, "\nmy_realloc(%d,): reallocating %p --> %p to %lu bytes\n",
           id, a, *(void **)ptr,(unsigned long) size);
 }
}

char *my_free(int id, void *ptr)
{
 if(*(void **)ptr) {
   free(*(void **)ptr);
   dbg(3, "\nmy_free(%d,):  freeing %p\n", id, *(void **)ptr);
   *(void **)ptr=NULL;
 } else {
   dbg(3, "\n--> my_free(%d,): trying to free NULL pointer\n", id);
 }
 return NULL;
}

/* n characters at most are copied, *d will be always NUL terminated if *s does
 *   not fit(d[n-1]='\0')
 * return # of copied characters
 */
int my_strncpy(char *d, const char *s, size_t n)
{
  int i = 0;
  n -= 1;
  dbg(3, "my_strncpy():  copying %s to %lu\n", s, (unsigned long)d);
  while( (d[i] = s[i]) )
  {
    if(i == n) {
      if(s[i] != '\0') dbg(1, "my_strncpy(): overflow, n=%d, s=%s\n", n+1, s);
      d[i] = '\0';
      return i;
    }
    i++;
  }
  return i;
}

char *strtolower(char* s) {
  char *p;
  if(s) for(p=s; *p; p++) *p=(char)tolower(*p);
  return s;
}
char *strtoupper(char* s) {
  char *p;
  if(s) for(p=s; *p; p++) *p=(char)toupper(*p);
  return s;
}

/* call hash_all_names() (once) before (repeatedly) using this function */
void set_inst_prop(int i)
{
  char *ptr;
  char *tmp = NULL;

  ptr = (xctx->inst[i].ptr+ xctx->sym)->templ;
  dbg(1, "set_inst_prop(): i=%d, name=%s, prop_ptr = %s, template=%s\n",
     i, xctx->inst[i].name, xctx->inst[i].prop_ptr, ptr);
  my_strdup(69, &xctx->inst[i].prop_ptr, ptr);
  my_strdup2(70, &xctx->inst[i].instname, get_tok_value(ptr, "name",0));
  if(xctx->inst[i].instname[0]) {
    my_strdup(101, &tmp, xctx->inst[i].prop_ptr);
    new_prop_string(i, tmp, 0, tclgetboolvar("disable_unique_names"));
    my_free(724, &tmp);
  }
}

static void edit_rect_property(int x)
{
  int i, c, n;
  int drw = 0;
  const char *dash, *fill;
  int preserve;
  char *oldprop=NULL;
  my_strdup(67, &oldprop, xctx->rect[xctx->sel_array[0].col][xctx->sel_array[0].n].prop_ptr);
  if(oldprop && oldprop[0]) {
    tclsetvar("retval",oldprop);
  } else {
    tclsetvar("retval","");
  }
  if(x==0) {
    xctx->semaphore++;
    tcleval("text_line {Input property:} 0 normal");
    xctx->semaphore--;
  }
  else if(x==2) tcleval("viewdata $::retval");
  else if(x==1) tcleval("edit_vi_prop {Text:}");
  else {
    fprintf(errfp, "edit_rect_property() : unknown parameter x=%d\n",x); exit(EXIT_FAILURE);
  }
  preserve = atoi(tclgetvar("preserve_unchanged_attrs"));
  if(strcmp(tclgetvar("rcode"),"") )
  {
    xctx->push_undo();
    set_modify(1);
    for(i=0; i<xctx->lastsel; i++) {
      if(xctx->sel_array[i].type != xRECT) continue;
      c = xctx->sel_array[i].col;
      n = xctx->sel_array[i].n;
      if(oldprop && preserve == 1) {
        set_different_token(&xctx->rect[c][n].prop_ptr,
               (char *) tclgetvar("retval"), oldprop, 0, 0);
      } else {
        my_strdup(99, &xctx->rect[c][n].prop_ptr,
               (char *) tclgetvar("retval"));
      }
      set_rect_flags(&xctx->rect[c][n]); /* set cached .flags bitmask from on attributes */

      set_rect_extraptr(0, &xctx->rect[c][n]);
      dash = get_tok_value(xctx->rect[c][n].prop_ptr,"dash",0);
      if( strcmp(dash, "") ) {
        int d = atoi(dash);
        xctx->rect[c][n].dash = (short)(d >= 0? d : 0);
      } else
        xctx->rect[c][n].dash = 0;

      fill = get_tok_value(xctx->rect[c][n].prop_ptr,"fill", 0);
      if(!strcmp(fill, "false")) xctx->rect[c][n].fill = 0;
      else xctx->rect[c][n].fill = 1;

      if( (oldprop &&  xctx->rect[c][n].prop_ptr && strcmp(oldprop, xctx->rect[c][n].prop_ptr)) ||
          (!oldprop && xctx->rect[c][n].prop_ptr) || (oldprop && !xctx->rect[c][n].prop_ptr)) {
         if(!drw) {
           bbox(START,0.0,0.0,0.0,0.0);
           drw = 1;
         }
         if( xctx->rect[c][n].flags & 1024) {
           draw_image(0, &xctx->rect[c][n], &xctx->rect[c][n].x1, &xctx->rect[c][n].y1,
                         &xctx->rect[c][n].x2, &xctx->rect[c][n].y2, 0, 0);
         }
         bbox(ADD, xctx->rect[c][n].x1, xctx->rect[c][n].y1, xctx->rect[c][n].x2, xctx->rect[c][n].y2);
      }
    }
    if(drw) {
      bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
      draw();
      bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
    }
  }
  my_free(725, &oldprop);
}

static void edit_line_property(void)
{
  int i, c, n;
  const char *dash;
  int preserve;
  char *oldprop=NULL;
  my_strdup(46, &oldprop, xctx->line[xctx->sel_array[0].col][xctx->sel_array[0].n].prop_ptr);
  if(oldprop && oldprop[0]) {
    tclsetvar("retval", oldprop);
  } else {
    tclsetvar("retval","");
  }
  xctx->semaphore++;
  tcleval("text_line {Input property:} 0 normal");
  xctx->semaphore--;
  preserve = atoi(tclgetvar("preserve_unchanged_attrs"));
  if(strcmp(tclgetvar("rcode"),"") )
  {
    double y1, y2;
    xctx->push_undo();
    set_modify(1);
    bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
    for(i=0; i<xctx->lastsel; i++) {
      if(xctx->sel_array[i].type != LINE) continue;
      c = xctx->sel_array[i].col;
      n = xctx->sel_array[i].n;
      if(oldprop && preserve == 1) {
        set_different_token(&xctx->line[c][n].prop_ptr,
               (char *) tclgetvar("retval"), oldprop, 0, 0);
      } else {
        my_strdup(102, &xctx->line[c][n].prop_ptr,
               (char *) tclgetvar("retval"));
      }
      xctx->line[c][n].bus = !strcmp(get_tok_value(xctx->line[c][n].prop_ptr,"bus",0), "true");
      dash = get_tok_value(xctx->line[c][n].prop_ptr,"dash",0);
      if( strcmp(dash, "") ) {
        int d = atoi(dash);
        xctx->line[c][n].dash = (short)(d >= 0? d : 0);
      } else
        xctx->line[c][n].dash = 0;
      if(xctx->line[c][n].y1 < xctx->line[c][n].y2) {
        y1 = xctx->line[c][n].y1-INT_BUS_WIDTH(xctx->lw); y2 = xctx->line[c][n].y2+INT_BUS_WIDTH(xctx->lw);
      } else {
        y1 = xctx->line[c][n].y1+INT_BUS_WIDTH(xctx->lw); y2 = xctx->line[c][n].y2-INT_BUS_WIDTH(xctx->lw);
      }
      bbox(ADD, xctx->line[c][n].x1-INT_BUS_WIDTH(xctx->lw), y1, xctx->line[c][n].x2+INT_BUS_WIDTH(xctx->lw), y2);
    }
    bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
    draw();
    bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  }
  my_free(726, &oldprop);
}


static void edit_wire_property(void)
{
  int i;
  int preserve;
  char *oldprop=NULL;
  const char *bus_ptr;

  my_strdup(47, &oldprop, xctx->wire[xctx->sel_array[0].n].prop_ptr);
  if(oldprop && oldprop[0]) {
    tclsetvar("retval", oldprop);
  } else {
    tclsetvar("retval","");
  }
  xctx->semaphore++;
  tcleval("text_line {Input property:} 0 normal");
  xctx->semaphore--;
  preserve = atoi(tclgetvar("preserve_unchanged_attrs"));
  if(strcmp(tclgetvar("rcode"),"") )
  {
    xctx->push_undo();
    set_modify(1);
    bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
    for(i=0; i<xctx->lastsel; i++) {
      int oldbus=0;
      int k = xctx->sel_array[i].n;
      if(xctx->sel_array[i].type != WIRE) continue;
      /* does not seem to be necessary */
     /*  xctx->prep_hash_wires=0;
      *  xctx->prep_net_structs=0;
      *  xctx->prep_hi_structs=0; */
      oldbus = xctx->wire[k].bus;
      if(oldprop && preserve == 1) {
        set_different_token(&xctx->wire[k].prop_ptr,
               (char *) tclgetvar("retval"), oldprop, 0, 0);
      } else {
        my_strdup(100, &xctx->wire[k].prop_ptr,(char *) tclgetvar("retval"));
      }
      bus_ptr = get_tok_value(xctx->wire[k].prop_ptr,"bus",0);
      if(!strcmp(bus_ptr, "true")) {
        double ov, y1, y2;
        ov = INT_BUS_WIDTH(xctx->lw) > cadhalfdotsize ? INT_BUS_WIDTH(xctx->lw) : CADHALFDOTSIZE;
        if(xctx->wire[k].y1 < xctx->wire[k].y2) { y1 = xctx->wire[k].y1-ov; y2 = xctx->wire[k].y2+ov; }
        else { y1 = xctx->wire[k].y1+ov; y2 = xctx->wire[k].y2-ov; }
        bbox(ADD, xctx->wire[k].x1-ov, y1 , xctx->wire[k].x2+ov , y2 );
        xctx->wire[k].bus=1;
      } else {
        if(oldbus){
          double ov, y1, y2;
          ov = INT_BUS_WIDTH(xctx->lw)> cadhalfdotsize ? INT_BUS_WIDTH(xctx->lw) : CADHALFDOTSIZE;
          if(xctx->wire[k].y1 < xctx->wire[k].y2) { y1 = xctx->wire[k].y1-ov; y2 = xctx->wire[k].y2+ov; }
          else                        { y1 = xctx->wire[k].y1+ov; y2 = xctx->wire[k].y2-ov; }
          bbox(ADD, xctx->wire[k].x1-ov, y1 , xctx->wire[k].x2+ov , y2 );
          xctx->wire[k].bus=0;
        }
      }
    }
    bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
    draw();
    bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  }
  my_free(727, &oldprop);
}

static void edit_arc_property(void)
{
  int old_fill;
  double x1, y1, x2, y2;
  int c, i, ii, old_dash, drw = 0;
  char *oldprop = NULL;
  const char *dash;
  int preserve;

  my_strdup(98, &oldprop, xctx->arc[xctx->sel_array[0].col][xctx->sel_array[0].n].prop_ptr);
  if(oldprop && oldprop[0]) {
    tclsetvar("retval", oldprop);
  } else {
    tclsetvar("retval","");
  }
  xctx->semaphore++;
  tcleval("text_line {Input property:} 0 normal");
  xctx->semaphore--;
  preserve = atoi(tclgetvar("preserve_unchanged_attrs"));
  if(strcmp(tclgetvar("rcode"),"") )
  {

   set_modify(1); xctx->push_undo();
   for(ii=0; ii<xctx->lastsel; ii++) {
     if(xctx->sel_array[ii].type != ARC) continue;

     i = xctx->sel_array[ii].n;
     c = xctx->sel_array[ii].col;

     if(oldprop && preserve == 1) {
        set_different_token(&xctx->arc[c][i].prop_ptr, (char *) tclgetvar("retval"), oldprop, 0, 0);

     } else {
        my_strdup(156, &xctx->arc[c][i].prop_ptr, (char *) tclgetvar("retval"));
     }
     old_fill = xctx->arc[c][i].fill;
     if( !strcmp(get_tok_value(xctx->arc[c][i].prop_ptr,"fill",0),"true") )
       xctx->arc[c][i].fill =1;
     else
       xctx->arc[c][i].fill =0;
     old_dash = xctx->arc[c][i].dash;
     dash = get_tok_value(xctx->arc[c][i].prop_ptr,"dash",0);
     if( strcmp(dash, "") ) {
       int d = atoi(dash);
       xctx->arc[c][i].dash = (short)(d >= 0 ? d : 0);
     } else
       xctx->arc[c][i].dash = 0;


     if(old_fill != xctx->arc[c][i].fill || old_dash != xctx->arc[c][i].dash) {
       if(!drw) {
         bbox(START,0.0,0.0,0.0,0.0);
         drw = 1;
       }
       arc_bbox(xctx->arc[c][i].x, xctx->arc[c][i].y, xctx->arc[c][i].r, 0, 360, &x1,&y1,&x2,&y2);
       bbox(ADD, x1, y1, x2, y2);
     }
   }
   if(drw) {
     bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
     draw();
     bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
   }
  }
}

static void edit_polygon_property(void)
{
  int old_fill;
  int k;
  double x1=0., y1=0., x2=0., y2=0.;
  int c, i, ii, old_dash;
  int drw = 0;
  char *oldprop = NULL;
  const char *dash;
  int preserve;

  dbg(1, "edit_property(): input property:\n");
  my_strdup(112, &oldprop, xctx->poly[xctx->sel_array[0].col][xctx->sel_array[0].n].prop_ptr);
  if(oldprop && oldprop[0]) {
    tclsetvar("retval", oldprop);
  } else {
    tclsetvar("retval","");
  }
  xctx->semaphore++;
  tcleval("text_line {Input property:} 0 normal");
  xctx->semaphore--;
  preserve = atoi(tclgetvar("preserve_unchanged_attrs"));
  if(strcmp(tclgetvar("rcode"),"") )
  {

   set_modify(1); xctx->push_undo();
   for(ii=0; ii<xctx->lastsel; ii++) {
     if(xctx->sel_array[ii].type != POLYGON) continue;

     i = xctx->sel_array[ii].n;
     c = xctx->sel_array[ii].col;

     if(oldprop && preserve == 1) {
        set_different_token(&xctx->poly[c][i].prop_ptr, (char *) tclgetvar("retval"), oldprop, 0, 0);
     } else {
        my_strdup(113, &xctx->poly[c][i].prop_ptr, (char *) tclgetvar("retval"));
     }
     old_fill = xctx->poly[c][i].fill;
     old_dash = xctx->poly[c][i].dash;
     if( !strcmp(get_tok_value(xctx->poly[c][i].prop_ptr,"fill",0),"true") )
       xctx->poly[c][i].fill =1;
     else
       xctx->poly[c][i].fill =0;
     dash = get_tok_value(xctx->poly[c][i].prop_ptr,"dash",0);
     if( strcmp(dash, "") ) {
       int d = atoi(dash);
       xctx->poly[c][i].dash = (short)(d >= 0 ? d : 0);
     } else
       xctx->poly[c][i].dash = 0;
     if(old_fill != xctx->poly[c][i].fill || old_dash != xctx->poly[c][i].dash) {
       if(!drw) {
         bbox(START,0.0,0.0,0.0,0.0);
         drw = 1;
       }
       for(k=0; k<xctx->poly[c][i].points; k++) {
         if(k==0 || xctx->poly[c][i].x[k] < x1) x1 = xctx->poly[c][i].x[k];
         if(k==0 || xctx->poly[c][i].y[k] < y1) y1 = xctx->poly[c][i].y[k];
         if(k==0 || xctx->poly[c][i].x[k] > x2) x2 = xctx->poly[c][i].x[k];
         if(k==0 || xctx->poly[c][i].y[k] > y2) y2 = xctx->poly[c][i].y[k];
       }
       bbox(ADD, x1, y1, x2, y2);
     }
   }
   if(drw) {
     bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
     draw();
     bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
   }
  }
}


/* x=0 use text widget   x=1 use vim editor */
static void edit_text_property(int x)
{
   int rot, flip;
   #if HAS_CAIRO==1
   int customfont;
   #endif
   int sel, k, text_changed = 0, props_changed = 0, size_changed = 0, tmp;
   int c,l, preserve;
   double hsize, vsize, dtmp;
   double xx1,yy1,xx2,yy2;
   double pcx,pcy;      /* pin center 20070317 */
   char property[100];/* used for float 2 string conv (xscale  and yscale) overflow safe */
   const char *str;
   char *oldprop = NULL;

   dbg(1, "edit_text_property(): entering\n");
   sel = xctx->sel_array[0].n;
   my_strdup(656, &oldprop, xctx->text[sel].prop_ptr);
   if(oldprop && oldprop[0])
      tclsetvar("props", oldprop);
   else
      tclsetvar("props","");
   tclsetvar("retval",xctx->text[sel].txt_ptr);
   my_snprintf(property, S(property), "%.16g",xctx->text[sel].yscale);
   tclsetvar("vsize",property);
   my_snprintf(property, S(property), "%.16g",xctx->text[sel].xscale);
   tclsetvar("hsize",property);
   if(x==0) {
     const char *props;
     xctx->semaphore++;
     tcleval("enter_text {text:} normal");
     xctx->semaphore--;
     hsize =atof(tclgetvar("hsize"));
     vsize =atof(tclgetvar("vsize"));
     props = tclgetvar("props");
     if(xctx->text[sel].xscale != hsize || xctx->text[sel].yscale != vsize) {
       size_changed = 1;
     }
     if( (oldprop && strcmp(oldprop, tclgetvar("props"))) || (!oldprop && props[0]) ) props_changed = 1;
   }
   else if(x==2) tcleval("viewdata $::retval");
   else if(x==1) tcleval("edit_vi_prop {Text:}");
   else {
     fprintf(errfp, "edit_text_property() : unknown parameter x=%d\n",x); exit(EXIT_FAILURE);
   }
   preserve = atoi(tclgetvar("preserve_unchanged_attrs"));
   if(x == 0 || x == 1) {
     if(strcmp(xctx->text[sel].txt_ptr, tclgetvar("retval") ) ) {
       dbg(1, "edit_text_property(): x=%d, text_changed=1\n", x);
       text_changed=1;
     }
   }
   if(strcmp(tclgetvar("rcode"),"") )
   {
     dbg(1, "edit_text_property(): rcode !=\"\"\n");
     if(text_changed || size_changed || props_changed) {
       set_modify(1);
       xctx->push_undo();
     }
     bbox(START,0.0,0.0,0.0,0.0);
     for(k=0;k<xctx->lastsel;k++)
     {
       if(xctx->sel_array[k].type!=xTEXT) continue;
       sel=xctx->sel_array[k].n;
       rot = xctx->text[sel].rot; /* calculate bbox, some cleanup needed here */
       flip = xctx->text[sel].flip;
       #if HAS_CAIRO==1
       customfont = set_text_custom_font(&xctx->text[sel]);
       #endif
       text_bbox(xctx->text[sel].txt_ptr, xctx->text[sel].xscale,
                 xctx->text[sel].yscale, (short)rot, (short)flip, xctx->text[sel].hcenter,
                 xctx->text[sel].vcenter, xctx->text[sel].x0, xctx->text[sel].y0,
                 &xx1,&yy1,&xx2,&yy2, &tmp, &dtmp);
       #if HAS_CAIRO==1
       if(customfont) cairo_restore(xctx->cairo_ctx);
       #endif
       bbox(ADD, xx1, yy1, xx2, yy2 );
       /* dbg(1, "edit_property(): text props=%s text=%s\n", tclgetvar("props"), tclgetvar("retval")); */
       if(text_changed) {
         double cg;
         cg = tclgetdoublevar("cadgrid");
         c = xctx->rects[PINLAYER];
         for(l=0;l<c;l++) {
           if(!strcmp( (get_tok_value(xctx->rect[PINLAYER][l].prop_ptr, "name",0)),
                        xctx->text[sel].txt_ptr) ) {
             #if HAS_CAIRO==1
             customfont = set_text_custom_font(&xctx->text[sel]);
             #endif
             text_bbox(xctx->text[sel].txt_ptr, xctx->text[sel].xscale,
             xctx->text[sel].yscale, (short)rot, (short)flip, xctx->text[sel].hcenter,
             xctx->text[sel].vcenter, xctx->text[sel].x0, xctx->text[sel].y0,
             &xx1,&yy1,&xx2,&yy2, &tmp, &dtmp);
             #if HAS_CAIRO==1
             if(customfont) cairo_restore(xctx->cairo_ctx);
             #endif
             pcx = (xctx->rect[PINLAYER][l].x1+xctx->rect[PINLAYER][l].x2)/2.0;
             pcy = (xctx->rect[PINLAYER][l].y1+xctx->rect[PINLAYER][l].y2)/2.0;
             if(
                 /* 20171206 20171221 */
                 (fabs( (yy1+yy2)/2 - pcy) < cg/2 &&
                 (fabs(xx1 - pcx) < cg*3 || fabs(xx2 - pcx) < cg*3) )
                 ||
                 (fabs( (xx1+xx2)/2 - pcx) < cg/2 &&
                 (fabs(yy1 - pcy) < cg*3 || fabs(yy2 - pcy) < cg*3) )
             ) {
               if(x==0)
                 my_strdup(71, &xctx->rect[PINLAYER][l].prop_ptr,
                   subst_token(xctx->rect[PINLAYER][l].prop_ptr, "name",
                   (char *) tclgetvar("retval")) );
               else
                 my_strdup(72, &xctx->rect[PINLAYER][l].prop_ptr,
                   subst_token(xctx->rect[PINLAYER][l].prop_ptr, "name",
                   (char *) tclgetvar("retval")) );
             }
           }
         }
         my_strdup(74, &xctx->text[sel].txt_ptr, (char *) tclgetvar("retval"));
       }
       if(x==0 && props_changed) {
         if(oldprop && preserve)
           set_different_token(&xctx->text[sel].prop_ptr, (char *) tclgetvar("props"), oldprop, 0, 0);
         else
           my_strdup(75, &xctx->text[sel].prop_ptr,(char *) tclgetvar("props"));
         my_strdup(76, &xctx->text[sel].font, get_tok_value(xctx->text[sel].prop_ptr, "font", 0));
         str = get_tok_value(xctx->text[sel].prop_ptr, "hcenter", 0);
         xctx->text[sel].hcenter = strcmp(str, "true")  ? 0 : 1;
         str = get_tok_value(xctx->text[sel].prop_ptr, "vcenter", 0);
         xctx->text[sel].vcenter = strcmp(str, "true")  ? 0 : 1;
         str = get_tok_value(xctx->text[sel].prop_ptr, "layer", 0);
         if(str[0]) xctx->text[sel].layer = atoi(str);
         else xctx->text[sel].layer=-1;
         xctx->text[sel].flags = 0;
         str = get_tok_value(xctx->text[sel].prop_ptr, "slant", 0);
         xctx->text[sel].flags |= strcmp(str, "oblique")  ? 0 : TEXT_OBLIQUE;
         xctx->text[sel].flags |= strcmp(str, "italic")  ? 0 : TEXT_ITALIC;
         str = get_tok_value(xctx->text[sel].prop_ptr, "weight", 0);
         xctx->text[sel].flags |= strcmp(str, "bold")  ? 0 : TEXT_BOLD;
         str = get_tok_value(xctx->text[sel].prop_ptr, "hide", 0);
         xctx->text[sel].flags |= strcmp(str, "true")  ? 0 : HIDE_TEXT;
         if(size_changed) {
           xctx->text[sel].xscale=hsize;
           xctx->text[sel].yscale=vsize;
         }
       }
       /* calculate bbox, some cleanup needed here */
       #if HAS_CAIRO==1
       customfont = set_text_custom_font(&xctx->text[sel]);
       #endif
       text_bbox(xctx->text[sel].txt_ptr, xctx->text[sel].xscale,
                 xctx->text[sel].yscale, (short)rot, (short)flip, xctx->text[sel].hcenter,
                  xctx->text[sel].vcenter, xctx->text[sel].x0, xctx->text[sel].y0,
                 &xx1,&yy1,&xx2,&yy2, &tmp, &dtmp);
       #if HAS_CAIRO==1
       if(customfont) cairo_restore(xctx->cairo_ctx);
       #endif

       bbox(ADD, xx1, yy1, xx2, yy2 );
     }
     bbox(SET,0.0,0.0,0.0,0.0);
     draw();
     bbox(END,0.0,0.0,0.0,0.0);
   }
   my_free(890, &oldprop);
}

/* x=0 use text widget   x=1 use vim editor */
static void update_symbol(const char *result, int x)
{
  int k, sym_number;
  int no_change_props=0;
  int only_different=0;
  int copy_cell=0;
  int prefix=0;
  char *name = NULL, *ptr = NULL, *new_prop = NULL;
  char symbol[PATH_MAX];
  char *type;
  int cond;
  int pushed=0;
  int *ii = &xctx->edit_sym_i; /* static var */
  int *netl_com = &xctx->netlist_commands; /* static var */

  dbg(1, "update_symbol(): entering\n");
  *ii=xctx->sel_array[0].n;
  if(!result) {
   dbg(1, "update_symbol(): edit symbol prop aborted\n");
   my_free(1289, &xctx->old_prop);
   return;
  }
  /* create new_prop updated attribute string */
  if(*netl_com && x==1) {
    my_strdup(79,  &new_prop,
      subst_token(xctx->old_prop, "value", (char *) tclgetvar("retval") )
    );
    dbg(1, "update_symbol(): new_prop=%s\n", new_prop);
    dbg(1, "update_symbol(): tcl retval==%s\n", tclgetvar("retval"));
  }
  else {
    my_strdup(80, &new_prop, (char *) tclgetvar("retval"));
    dbg(1, "update_symbol(): new_prop=%s\n", new_prop);
  }
  my_strncpy(symbol, (char *) tclgetvar("symbol") , S(symbol));
  dbg(1, "update_symbol(): symbol=%s\n", symbol);
  no_change_props=atoi(tclgetvar("no_change_attrs") );
  only_different=atoi(tclgetvar("preserve_unchanged_attrs") );
  copy_cell=atoi(tclgetvar("user_wants_copy_cell") );
  bbox(START,0.0,0.0,0.0,0.0);
  /* 20191227 necessary? --> Yes since a symbol copy has already been done
     in edit_symbol_property() -> tcl edit_prop, this ensures new symbol is loaded from disk.
     if for some reason a symbol with matching name is loaded in xschem this
     may be out of sync wrt disk version */
  if(copy_cell) {
   remove_symbols();
   link_symbols_to_instances(-1);
  }
  /* symbol reference changed? --> sym_number >=0, set prefix to 1st char
     to use for inst name (from symbol template) */
  prefix=0;
  sym_number = -1;
  if(strcmp(symbol, xctx->inst[*ii].name)) {
    set_modify(1);
    sym_number=match_symbol(symbol); /* check if exist */
    if(sym_number>=0) {
      prefix=(get_tok_value((xctx->sym+sym_number)->templ, "name",0))[0]; /* get new symbol prefix  */
    }
  }
  for(k=0;k<xctx->lastsel;k++) {
    dbg(1, "update_symbol(): for k loop: k=%d\n", k);
    if(xctx->sel_array[k].type!=ELEMENT) continue;
    *ii=xctx->sel_array[k].n;
    /* 20171220 calculate bbox before changes to correctly redraw areas */
    /* must be recalculated as cairo text extents vary with zoom factor. */
    symbol_bbox(*ii, &xctx->inst[*ii].x1, &xctx->inst[*ii].y1, &xctx->inst[*ii].x2, &xctx->inst[*ii].y2);
    if(sym_number>=0) /* changing symbol ! */
    {
      if(!pushed) { xctx->push_undo(); pushed=1;}
      delete_inst_node(*ii); /* 20180208 fix crashing bug: delete node info if changing symbol */
                        /* if number of pins is different we must delete these data *before* */
                        /* changing ysmbol, otherwise *ii might end up deleting non allocated data. */
      my_strdup2(82, &xctx->inst[*ii].name, rel_sym_path(symbol));
      xctx->inst[*ii].ptr=sym_number; /* update instance to point to new symbol */
    }
    bbox(ADD, xctx->inst[*ii].x1, xctx->inst[*ii].y1,
              xctx->inst[*ii].x2, xctx->inst[*ii].y2);

    /* update property string from tcl dialog */
    if(!no_change_props)
    {
      dbg(1, "update_symbol(): no_change_props=%d\n", no_change_props);
      if(only_different) {
        char * ss=NULL;
        my_strdup(119, &ss, xctx->inst[*ii].prop_ptr);
        if( set_different_token(&ss, new_prop, xctx->old_prop, 0, 0) ) {
          if(!pushed) { xctx->push_undo(); pushed=1;}
          my_strdup(111, &xctx->inst[*ii].prop_ptr, ss);
          set_modify(1);
        }
        my_free(729, &ss);
      }
      else {
        if(new_prop) {
          if(!xctx->inst[*ii].prop_ptr || strcmp(xctx->inst[*ii].prop_ptr, new_prop)) {
            dbg(1, "update_symbol(): changing prop: |%s| -> |%s|\n",
                xctx->inst[*ii].prop_ptr, new_prop);
            if(!pushed) { xctx->push_undo(); pushed=1;}
            my_strdup(84, &xctx->inst[*ii].prop_ptr, new_prop);
            set_modify(1);
          }
        }  else {
          if(!pushed) { xctx->push_undo(); pushed=1;}
          my_strdup(86, &xctx->inst[*ii].prop_ptr, "");
          set_modify(1);
        }
      }
    }
    /* if symbol changed ensure instance name (with new prefix char) is unique */
    /* preserve backslashes in name ----------------------------------->. */
    my_strdup(152, &name, get_tok_value(xctx->inst[*ii].prop_ptr, "name", 1));
    if(name && name[0] ) {
      dbg(1, "update_symbol(): prefix!='\\0', name=%s\n", name);
      /* 20110325 only modify prefix if prefix not NUL */
      if(prefix) name[0]=(char)prefix; /* change prefix if changing symbol type; */
      dbg(1, "update_symbol(): name=%s, inst[*ii].prop_ptr=%s\n",
          name, xctx->inst[*ii].prop_ptr);
      my_strdup(89, &ptr,subst_token(xctx->inst[*ii].prop_ptr, "name", name) );
                     /* set name of current inst */
      if(!pushed) { xctx->push_undo(); pushed=1;}
      if(!k) hash_all_names();
      new_prop_string(*ii, ptr, k, tclgetboolvar("disable_unique_names")); /* set new prop_ptr */
    } else {
      my_strdup2(367, &xctx->inst[*ii].instname, "");
    }
    type=xctx->sym[xctx->inst[*ii].ptr].type;
    cond= !type || !IS_LABEL_SH_OR_PIN(type);
    if(cond) xctx->inst[*ii].flags |= 2; /* bit 1: flag for different textlayer for pin/labels */
    else {
      xctx->inst[*ii].flags &= ~2;
      my_strdup(880, &xctx->inst[*ii].lab,
                get_tok_value(xctx->inst[*ii].prop_ptr, "lab",0));
    }
    if(!strcmp(get_tok_value(xctx->inst[*ii].prop_ptr,"highlight",0), "true"))
          xctx->inst[*ii].flags |= HILIGHT_CONN;
    else  xctx->inst[*ii].flags &= ~HILIGHT_CONN;
    xctx->inst[*ii].embed = !strcmp(get_tok_value(xctx->inst[*ii].prop_ptr, "embed", 2), "true");
  }  /* end for(k=0;k<xctx->lastsel;k++) */
  /* new symbol bbox after prop changes (may change due to text length) */
  if(xctx->modified) {
    xctx->prep_hash_inst=0;
    xctx->prep_net_structs=0;
    xctx->prep_hi_structs=0;
    find_inst_to_be_redrawn(1 + 4 + 32);  /* 32: call prepare_netlist_structs(0) */
    find_inst_to_be_redrawn(16); /* clear data */
    if(xctx->hilight_nets) {
      propagate_hilights(1, 1, XINSERT_NOREPLACE);
    }
  }
  /* redraw symbol with new props */
  bbox(SET,0.0,0.0,0.0,0.0);
  dbg(1, "update_symbol(): redrawing inst_ptr.txtprop string\n");
  draw();
  bbox(END,0.0,0.0,0.0,0.0);
  my_free(731, &name);
  my_free(732, &ptr);
  my_free(733, &new_prop);
  my_free(734, &xctx->old_prop);
}

/* x=0 use text widget   x=1 use vim editor */
static void edit_symbol_property(int x)
{
   char *result=NULL;
   int *ii = &xctx->edit_sym_i; /* static var */
   int *netl_com = &xctx->netlist_commands; /* static var */

   *ii=xctx->sel_array[0].n;
   *netl_com = 0;
   if ((xctx->inst[*ii].ptr + xctx->sym)->type!=NULL)
     *netl_com = 
       !strcmp( (xctx->inst[*ii].ptr+ xctx->sym)->type, "netlist_commands");
   if(xctx->inst[*ii].prop_ptr!=NULL) {
     if(*netl_com && x==1) {
       tclsetvar("retval",get_tok_value( xctx->inst[*ii].prop_ptr,"value",0));
     } else {
       tclsetvar("retval",xctx->inst[*ii].prop_ptr);
     }
   }
   else {
     tclsetvar("retval","");
   }
   my_strdup(91, &xctx->old_prop, xctx->inst[*ii].prop_ptr);
   tclsetvar("symbol",xctx->inst[*ii].name);

   if(x==0) {
     tcleval("edit_prop {Input property:}");
     my_strdup(77, &result, tclresult());
   }
   else {
     /* edit_vi_netlist_prop will replace \" with " before editing,
        replace back " with \" when done and wrap the resulting text with quotes
        ("text") when done */
     if(*netl_com && x==1)    tcleval("edit_vi_netlist_prop {Input property:}");
     else if(x==1)    tcleval("edit_vi_prop {Input property:}");
     else if(x==2)    tcleval("viewdata $::retval");
     my_strdup(78, &result, tclresult());
   }
   dbg(1, "edit_symbol_property(): before update_symbol, modified=%d\n", xctx->modified);
   update_symbol(result, x);
   my_free(728, &result);
   dbg(1, "edit_symbol_property(): done update_symbol, modified=%d\n", xctx->modified);
   *ii=-1;
}

void change_elem_order(void)
{
  xInstance tmpinst;
  xRect tmpbox;
  xWire tmpwire;
  char tmp_txt[50]; /* overflow safe */
  int c, new_n;

  rebuild_selected_array();
  if(xctx->lastsel==1)
  {
    my_snprintf(tmp_txt, S(tmp_txt), "%d",xctx->sel_array[0].n);
    tclsetvar("retval",tmp_txt);
    xctx->semaphore++;
    tcleval("text_line {Object Sequence number} 0");
    xctx->semaphore--;
    if(strcmp(tclgetvar("rcode"),"") )
    {
      xctx->push_undo();
      set_modify(1);
      xctx->prep_hash_inst=0;
      xctx->prep_net_structs=0;
      xctx->prep_hi_structs=0;
    }
    sscanf(tclgetvar("retval"), "%d",&new_n);

    if(xctx->sel_array[0].type==ELEMENT)
    {
      if(new_n>=xctx->instances) new_n=xctx->instances-1;
      tmpinst=xctx->inst[new_n];
      xctx->inst[new_n]=xctx->inst[xctx->sel_array[0].n];
      xctx->inst[xctx->sel_array[0].n]=tmpinst;
      dbg(1, "change_elem_order(): selected element %d\n", xctx->sel_array[0].n);
    }
    else if(xctx->sel_array[0].type==xRECT)
    {
      c=xctx->sel_array[0].col;
      if(new_n>=xctx->rects[c]) new_n=xctx->rects[c]-1;
      tmpbox=xctx->rect[c][new_n];
      xctx->rect[c][new_n]=xctx->rect[c][xctx->sel_array[0].n];
      xctx->rect[c][xctx->sel_array[0].n]=tmpbox;
      dbg(1, "change_elem_order(): selected element %d\n", xctx->sel_array[0].n);
      if(c == GRIDLAYER) {
        if(xctx->graph_lastsel == new_n) xctx->graph_lastsel = xctx->sel_array[0].n;
        else if(xctx->graph_lastsel ==  xctx->sel_array[0].n) xctx->graph_lastsel = new_n;
      }
    }
    else if(xctx->sel_array[0].type==WIRE)
    {
      if(new_n>=xctx->wires) new_n=xctx->wires-1;
      tmpwire=xctx->wire[new_n];
      xctx->wire[new_n]=xctx->wire[xctx->sel_array[0].n];
      xctx->wire[xctx->sel_array[0].n]=tmpwire;
      dbg(1, "change_elem_order(): selected element %d\n", xctx->sel_array[0].n);
    }
    xctx->need_reb_sel_arr = 1;
  }
}

char *str_replace(const char *s, const char *rep, const char *with)
{
  static char *result = NULL;
  static size_t size=0;
  size_t result_pos = 0;
  size_t rep_len;
  size_t with_len;

  if(s==NULL || rep == NULL || with == NULL || rep[0] == '\0') {
    my_free(1244, &result);
    size = 0;
    return NULL;
  }
  rep_len = strlen(rep);
  with_len = strlen(with);
  dbg(1, "str_replace(): %s, %s, %s\n", s, rep, with);
  if( size == 0 ) {
    size = CADCHUNKALLOC;
    my_realloc(1245, &result, size);
  }
  while(*s) {
    STR_ALLOC(&result, result_pos + with_len + 1, &size);
    if(!strncmp(s, rep, rep_len)) {
      my_strncpy(result + result_pos, with, with_len + 1);
      result_pos += with_len;
      s += rep_len;
    } else {
      result[result_pos++] = *s++;
    }
  }
  result[result_pos] = '\0';
  dbg(1, "str_replace(): returning %s\n", result);
  return result;
}

/* x=0 use tcl text widget  x=1 use vim editor  x=2 only view data */
void edit_property(int x)
{
 int j;

 if(!has_x) return;
 rebuild_selected_array(); /* from the .sel field in objects build */
 if(xctx->lastsel==0 )      /* the array of selected objs */
 {
   char *new_prop = NULL;

   if(xctx->netlist_type==CAD_SYMBOL_ATTRS) {
    if(xctx->schsymbolprop!=NULL)
      tclsetvar("retval",xctx->schsymbolprop);
    else
      tclsetvar("retval","");
   }
   else if(xctx->netlist_type==CAD_VHDL_NETLIST) {
    if(xctx->schvhdlprop!=NULL)
      tclsetvar("retval",xctx->schvhdlprop);
    else
      tclsetvar("retval","");
   }
   else if(xctx->netlist_type==CAD_VERILOG_NETLIST) {
    if(xctx->schverilogprop!=NULL)
      tclsetvar("retval",xctx->schverilogprop);
    else
      tclsetvar("retval","");
   }
   else if(xctx->netlist_type==CAD_SPICE_NETLIST) {
    if(xctx->schprop!=NULL)
      tclsetvar("retval",xctx->schprop);
    else
      tclsetvar("retval","");
   }
   else if(xctx->netlist_type==CAD_TEDAX_NETLIST) {
    if(xctx->schtedaxprop!=NULL)
      tclsetvar("retval",xctx->schtedaxprop);
    else
      tclsetvar("retval","");
   }

   if(x==0) {
     xctx->semaphore++;
     tcleval("text_line {Global schematic property:} 0");
     xctx->semaphore--;
   }
   else if(x==1) {
      dbg(1, "edit_property(): executing edit_vi_prop\n");
      tcleval("edit_vi_prop {Global schematic property:}");
   }
   else if(x==2)    tcleval("viewdata $::retval");
   dbg(1, "edit_property(): done executing edit_vi_prop, result=%s\n",tclresult());
   dbg(1, "edit_property(): rcode=%s\n",tclgetvar("rcode") );

   my_strdup(650, &new_prop, (char *) tclgetvar("retval"));
   tclsetvar("retval", new_prop);
   my_free(893, &new_prop);


   if(strcmp(tclgetvar("rcode"),"") )
   {
     if(xctx->netlist_type==CAD_SYMBOL_ATTRS && 
        (!xctx->schsymbolprop || strcmp(xctx->schsymbolprop, tclgetvar("retval") ) ) ) {
        set_modify(1); xctx->push_undo();
        my_strdup(422, &xctx->schsymbolprop, (char *) tclgetvar("retval"));

     } else if(xctx->netlist_type==CAD_VERILOG_NETLIST &&
        (!xctx->schverilogprop || strcmp(xctx->schverilogprop, tclgetvar("retval") ) ) ) {
        set_modify(1); xctx->push_undo();
        my_strdup(94, &xctx->schverilogprop, (char *) tclgetvar("retval"));

     } else if(xctx->netlist_type==CAD_SPICE_NETLIST && 
        (!xctx->schprop || strcmp(xctx->schprop, tclgetvar("retval") ) ) ) {
        set_modify(1); xctx->push_undo();
        my_strdup(95, &xctx->schprop, (char *) tclgetvar("retval"));

     } else if(xctx->netlist_type==CAD_TEDAX_NETLIST &&
        (!xctx->schtedaxprop || strcmp(xctx->schtedaxprop, tclgetvar("retval") ) ) ) {
        set_modify(1); xctx->push_undo();
        my_strdup(96, &xctx->schtedaxprop, (char *) tclgetvar("retval"));

     } else if(xctx->netlist_type==CAD_VHDL_NETLIST &&
        (!xctx->schvhdlprop || strcmp(xctx->schvhdlprop, tclgetvar("retval") ) ) ) {
        set_modify(1); xctx->push_undo();
        my_strdup(97, &xctx->schvhdlprop, (char *) tclgetvar("retval"));
     }
   }

   /* update the bounding box of vhdl "architecture" instances that embed */
   /* the xctx->schvhdlprop string. 04102001 */
   for(j=0;j<xctx->instances;j++)
   {
    if( xctx->inst[j].ptr !=-1 &&
        (xctx->inst[j].ptr+ xctx->sym)->type &&
        !strcmp( (xctx->inst[j].ptr+ xctx->sym)->type, "architecture") )
    {
      dbg(1, "edit_property(): updating vhdl architecture\n");
      symbol_bbox(j, &xctx->inst[j].x1, &xctx->inst[j].y1,
                        &xctx->inst[j].x2, &xctx->inst[j].y2);
    }
   } /* end for(j...) */
   return;
 } /* if((xctx->lastsel==0 ) */

 switch(xctx->sel_array[0].type)
 {
  case ELEMENT:
   edit_symbol_property(x);
   while( x == 0 && tclgetvar("edit_symbol_prop_new_sel")[0] == '1' ) {
     unselect_all(1);
     select_object(xctx->mousex, xctx->mousey, SELECTED, 0);
     rebuild_selected_array();
     if(xctx->lastsel && xctx->sel_array[0].type ==ELEMENT) {
       edit_symbol_property(0);
     } else {
       break;
     }
   }
   tclsetvar("edit_symbol_prop_new_sel", "");
   break;
  case ARC:
   edit_arc_property();
   break;
  case xRECT:
   edit_rect_property(x);
   break;
  case WIRE:
   edit_wire_property();
   break;
  case POLYGON:
   edit_polygon_property();
   break;
  case LINE:
   edit_line_property();
   break;
  case xTEXT:
   edit_text_property(x);
   break;
 }

}


