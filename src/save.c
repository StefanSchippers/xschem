/* File: save.c
 *
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
 * simulation.
 * Copyright (C) 1998-2021 Stefan Frederik Schippers
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
#ifdef __unix__
#include <sys/wait.h>  /* waitpid */
#endif

/*
read an unknown xschem record usually like:
text {string} text {string}....
until a '\n' (outside the '{' '}' brackets)  or EOF is found.
within the brackets use load_ascii_string so escapes and string
newlines are correctly handled
*/
void read_record(int firstchar, FILE *fp, int dbg_level)
{
  int c;
  char *str = NULL;
  dbg(dbg_level, "SKIP RECORD\n");
  if(firstchar != '{') {
    dbg(dbg_level, "%c", firstchar);
  }
  while((c = fgetc(fp)) != EOF) {
    if (c=='\r') continue;
    if(c == '\n') {
      dbg(dbg_level, "\n");
      ungetc(c, fp); /* so following read_line does not skip next line */
      break;
    }
    if(c == '{') {
      ungetc(c, fp);
      load_ascii_string(&str, fp);
      dbg(dbg_level, "{%s}", str);
    } else {
      dbg(dbg_level, "%c", c);
    }
  }
  dbg(dbg_level,   "END SKIP RECORD\n");
  my_free(881, &str);
}

/* skip line of text from file, stopping before '\n' or EOF */
/* return first portion of line if found or NULL if EOF */
char *read_line(FILE *fp, int dbg_level)
{
  char s[300];
  static char ret[300]; /* safe to keep even with multiple schematics */
  int first = 0, items;

  ret[0] = '\0';
  while((items = fscanf(fp, "%298[^\r\n]", s)) > 0) {
    if(!first) {
      dbg(dbg_level, "SKIPPING |");
      my_strncpy(ret, s, S(ret)); /* store beginning of line for return */
      first = 1;
    }
    dbg(dbg_level, "%s", s);
  }
  if(first) dbg(dbg_level, "|\n");
  return !first && items == EOF ? NULL : ret;
}

/* */

/* return "/<prefix><random string of random_size characters>"
 * example: "/xschem_undo_dj5hcG38T2"
 */
const char *random_string(const char *prefix)
{
  static const char *charset="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  static const int random_size=10;
  static char str[PATH_MAX]; /* safe even with multiple schematics, if immediately copied */
  int prefix_size;
  static unsigned short once=1; /* safe even with multiple schematics, set once and never changed */
  int i;
  int idx;
  if(once) {
    srand((unsigned short) time(NULL));
    once=0;
  }
  prefix_size = strlen(prefix);
  str[0]='/';
  memcpy(str+1, prefix, prefix_size);
  for(i=prefix_size+1; i < prefix_size + random_size+1; i++) {
    idx = rand()%(sizeof(charset)-1);
    str[i] = charset[idx];
  }
  str[i] ='\0';
  return str;
}


/* */

/* try to create a tmp directory in XSCHEM_TMP_DIR */
/* XSCHEM_TMP_DIR/<prefix><trailing random chars> */
/* after 5 unsuccessfull attemps give up */
/* and return NULL */
/* */
const char *create_tmpdir(char *prefix)
{
  static char str[PATH_MAX]; /* safe even with multiple schematics if immediately copied */
  int i;
  struct stat buf;
  for(i=0; i<5;i++) {
    my_snprintf(str, S(str), "%s%s", tclgetvar("XSCHEM_TMP_DIR"), random_string(prefix));
    if(stat(str, &buf) && !mkdir(str, 0700) ) { /* dir must not exist */
      dbg(1, "create_tmpdir(): created dir: %s\n", str);
      return str;
      break;
    }
    dbg(1, "create_tmpdir(): failed to create %s\n", str);
  }
  fprintf(errfp, "create_tmpdir(): failed to create %s, aborting\n", str);
  return NULL; /* failed to create random dir 5 times */
}

/* */

/* try to create a tmp file in $XSCHEM_TMP_DIR */
/* ${XSCHEM_TMP_DIR}/<prefix><trailing random chars> */
/* after 5 unsuccessfull attemps give up */
/* and return NULL */
/* */
FILE *open_tmpfile(char *prefix, char **filename)
{
  static char str[PATH_MAX]; /* safe even with multiple schematics, if immediately copied */
  int i;
  FILE *fd;
  struct stat buf;
  for(i=0; i<5;i++) {
    my_snprintf(str, S(str), "%s%s", tclgetvar("XSCHEM_TMP_DIR"), random_string(prefix));
    *filename = str;
    if(stat(str, &buf) && (fd = fopen(str, "w")) ) { /* file must not exist */
      dbg(1, "open_tmpfile(): created file: %s\n", str);
      return fd;
      break;
    }
    dbg(1, "open_tmpfile(): failed to create %s\n", str);
  }
  fprintf(errfp, "open_tmpfile(): failed to create %s, aborting\n", str);
  return NULL; /* failed to create random filename 5 times */
}

void updatebbox(int count, xRect *boundbox, xRect *tmp)
{
 RECTORDER(tmp->x1, tmp->y1, tmp->x2, tmp->y2);
 /* dbg(1, "updatebbox(): count=%d, tmp = %g %g %g %g\n",
         count, tmp->x1, tmp->y1, tmp->x2, tmp->y2); */
 if(count==1)  *boundbox = *tmp;
 else
 {
  if(tmp->x1<boundbox->x1) boundbox->x1 = tmp->x1;
  if(tmp->x2>boundbox->x2) boundbox->x2 = tmp->x2;
  if(tmp->y1<boundbox->y1) boundbox->y1 = tmp->y1;
  if(tmp->y2>boundbox->y2) boundbox->y2 = tmp->y2;
 }
}

void save_ascii_string(const char *ptr, FILE *fd, int newline)
{
  int c, len, strbuf_pos = 0;
  static char *strbuf = NULL; /* safe even with multiple schematics */
  static int strbuf_size=0; /* safe even with multiple schematics */

  if(ptr == NULL) {
    if( fd == NULL) { /* used to clear static data */
       my_free(139, &strbuf);
       strbuf_size = 0;
       return;
    }
    if(newline) fputs("{}\n", fd);
    else fputs("{}", fd);
    return;
  }
  len = strlen(ptr) + CADCHUNKALLOC;
  if(strbuf_size < len ) my_realloc(140, &strbuf, (strbuf_size = len));

  strbuf[strbuf_pos++] = '{';
  while( (c = *ptr++) ) {
    if(strbuf_pos > strbuf_size - 6) my_realloc(525, &strbuf, (strbuf_size += CADCHUNKALLOC));
    if( c=='\\' || c=='{' || c=='}') strbuf[strbuf_pos++] = '\\';
    strbuf[strbuf_pos++] = c;
  }
  strbuf[strbuf_pos++] = '}';
  if(newline) strbuf[strbuf_pos++] = '\n';
  strbuf[strbuf_pos] = '\0';
  fwrite(strbuf, 1, strbuf_pos, fd);
}

void save_embedded_symbol(xSymbol *s, FILE *fd)
{
  int c, i, j;

  fprintf(fd, "v {xschem version=%s file_version=%s}\n", XSCHEM_VERSION, XSCHEM_FILE_VERSION);
  fprintf(fd, "G ");
  save_ascii_string(s->prop_ptr,fd, 1);
  fprintf(fd, "V {}\n");
  fprintf(fd, "S {}\n");
  fprintf(fd, "E {}\n");
  for(c=0;c<cadlayers;c++)
  {
   xLine *ptr;
   ptr=s->line[c];
   for(i=0;i<s->lines[c];i++)
   {
    fprintf(fd, "L %d %.16g %.16g %.16g %.16g ", c,ptr[i].x1, ptr[i].y1,ptr[i].x2,
     ptr[i].y2 );
    save_ascii_string(ptr[i].prop_ptr,fd, 1);
   }
  }
  for(c=0;c<cadlayers;c++)
  {
   xRect *ptr;
   ptr=s->rect[c];
   for(i=0;i<s->rects[c];i++)
   {
    fprintf(fd, "B %d %.16g %.16g %.16g %.16g ", c,ptr[i].x1, ptr[i].y1,ptr[i].x2,
     ptr[i].y2);
    save_ascii_string(ptr[i].prop_ptr,fd, 1);
   }
  }
  for(c=0;c<cadlayers;c++)
  {
   xArc *ptr;
   ptr=s->arc[c];
   for(i=0;i<s->arcs[c];i++)
   {
    fprintf(fd, "A %d %.16g %.16g %.16g %.16g %.16g ", c,ptr[i].x, ptr[i].y,ptr[i].r,
     ptr[i].a, ptr[i].b);
    save_ascii_string(ptr[i].prop_ptr,fd, 1);
   }
  }
  for(i=0;i<s->texts;i++)
  {
   xText *ptr;
   ptr = s->text;
   fprintf(fd, "T ");
   save_ascii_string(ptr[i].txt_ptr,fd, 0);
   fprintf(fd, " %.16g %.16g %hd %hd %.16g %.16g ",
    ptr[i].x0, ptr[i].y0, ptr[i].rot, ptr[i].flip, ptr[i].xscale,
     ptr[i].yscale);
   save_ascii_string(ptr[i].prop_ptr,fd, 1);
  }
  for(c=0;c<cadlayers;c++)
  {
   xPoly *ptr;
   ptr=s->poly[c];
   for(i=0;i<s->polygons[c];i++)
   {
    fprintf(fd, "P %d %d ", c,ptr[i].points);
    for(j=0;j<ptr[i].points;j++) {
      fprintf(fd, "%.16g %.16g ", ptr[i].x[j], ptr[i].y[j]);
    }
    save_ascii_string(ptr[i].prop_ptr,fd, 1);
   }
  }
}

void save_inst(FILE *fd, int select_only)
{
 int i, oldversion;
 xInstance *ptr;
 char *tmp = NULL;

 ptr=xctx->inst;
 oldversion = !strcmp(xctx->file_version, "1.0");
 for(i=0;i<xctx->symbols;i++) xctx->sym[i].flags &=~EMBEDDED;
 for(i=0;i<xctx->instances;i++)
 {
   if (select_only && ptr[i].sel != SELECTED) continue;
  fputs("C ", fd);
  if(oldversion) {
    my_strdup2(57, &tmp, add_ext(ptr[i].name, ".sym"));
    save_ascii_string(tmp, fd, 0);
    my_free(882, &tmp);
  } else {
    save_ascii_string(ptr[i].name, fd, 0);
  }
  fprintf(fd, " %.16g %.16g %hd %hd ",ptr[i].x0, ptr[i].y0, ptr[i].rot, ptr[i].flip );
  save_ascii_string(ptr[i].prop_ptr,fd, 1);
  if( !strcmp(get_tok_value(ptr[i].prop_ptr, "embed", 0), "true") ) {
      /* && !(xctx->sym[ptr[i].ptr].flags & EMBEDDED)) {  */
    fprintf(fd, "[\n");
    save_embedded_symbol( xctx->sym+ptr[i].ptr, fd);
    fprintf(fd, "]\n");
    xctx->sym[ptr[i].ptr].flags |= EMBEDDED;
  }
 }
}

void save_wire(FILE *fd, int select_only)
{
 int i;
 xWire *ptr;

 ptr=xctx->wire;
 for(i=0;i<xctx->wires;i++)
 {
   if (select_only && ptr[i].sel != SELECTED) continue;
  fprintf(fd, "N %.16g %.16g %.16g %.16g ",ptr[i].x1, ptr[i].y1, ptr[i].x2,
     ptr[i].y2);
  save_ascii_string(ptr[i].prop_ptr,fd, 1);
 }
}

void save_text(FILE *fd, int select_only)
{
 int i;
 xText *ptr;
 ptr=xctx->text;
 for(i=0;i<xctx->texts;i++)
 {
   if (select_only && ptr[i].sel != SELECTED) continue;
  fprintf(fd, "T ");
  save_ascii_string(ptr[i].txt_ptr,fd, 0);
  fprintf(fd, " %.16g %.16g %hd %hd %.16g %.16g ",
   ptr[i].x0, ptr[i].y0, ptr[i].rot, ptr[i].flip, ptr[i].xscale,
    ptr[i].yscale);
  save_ascii_string(ptr[i].prop_ptr,fd, 1);
 }
}

void save_polygon(FILE *fd, int select_only)
{
    int c, i, j;
    xPoly *ptr;
    for(c=0;c<cadlayers;c++)
    {
     ptr=xctx->poly[c];
     for(i=0;i<xctx->polygons[c];i++)
     {
       if (select_only && ptr[i].sel != SELECTED) continue;
      fprintf(fd, "P %d %d ", c,ptr[i].points);
      for(j=0;j<ptr[i].points;j++) {
        fprintf(fd, "%.16g %.16g ", ptr[i].x[j], ptr[i].y[j]);
      }
      save_ascii_string(ptr[i].prop_ptr,fd, 1);
     }
    }
}

void save_arc(FILE *fd, int select_only)
{
    int c, i;
    xArc *ptr;
    for(c=0;c<cadlayers;c++)
    {
     ptr=xctx->arc[c];
     for(i=0;i<xctx->arcs[c];i++)
     {
       if (select_only && ptr[i].sel != SELECTED) continue;
      fprintf(fd, "A %d %.16g %.16g %.16g %.16g %.16g ", c,ptr[i].x, ptr[i].y,ptr[i].r,
       ptr[i].a, ptr[i].b);
      save_ascii_string(ptr[i].prop_ptr,fd, 1);
     }
    }
}

void save_box(FILE *fd, int select_only)
{
    int c, i;
    xRect *ptr;
    for(c=0;c<cadlayers;c++)
    {
     ptr=xctx->rect[c];
     for(i=0;i<xctx->rects[c];i++)
     {
       if (select_only && ptr[i].sel != SELECTED) continue;
      fprintf(fd, "B %d %.16g %.16g %.16g %.16g ", c,ptr[i].x1, ptr[i].y1,ptr[i].x2,
       ptr[i].y2);
      save_ascii_string(ptr[i].prop_ptr,fd, 1);
     }
    }
}

void save_line(FILE *fd, int select_only)
{
    int c, i;
    xLine *ptr;
    for(c=0;c<cadlayers;c++)
    {
     ptr=xctx->line[c];
     for(i=0;i<xctx->lines[c];i++)
     {
       if (select_only && ptr[i].sel != SELECTED) continue;
      fprintf(fd, "L %d %.16g %.16g %.16g %.16g ", c,ptr[i].x1, ptr[i].y1,ptr[i].x2,
       ptr[i].y2 );
      save_ascii_string(ptr[i].prop_ptr,fd, 1);
     }
    }
}

void write_xschem_file(FILE *fd)
{
  int ty=0;
  char *ptr;

  if(xctx->version_string && (ptr = strstr(xctx->version_string, "xschem")) &&
    (ptr - xctx->version_string < 50)) {
    my_strdup2(59, &xctx->version_string, subst_token(xctx->version_string, "xschem", NULL));
  }
  my_strdup2(1183, &xctx->version_string, subst_token(xctx->version_string, "version", NULL));
  my_strdup2(1184, &xctx->version_string, subst_token(xctx->version_string, "file_version", NULL));
  ptr = xctx->version_string;
  while(*ptr == ' ' || *ptr == '\t') ptr++; /* strip leading spaces */
  fprintf(fd, "v {xschem version=%s file_version=%s %s}\n", XSCHEM_VERSION, XSCHEM_FILE_VERSION, ptr);


  if(xctx->schvhdlprop && !xctx->schsymbolprop) {
    get_tok_value(xctx->schvhdlprop,"type",0);
    ty = xctx->get_tok_size;
    if(ty && !strcmp(xctx->sch[xctx->currsch] + strlen(xctx->sch[xctx->currsch]) - 4,".sym") ) {
      fprintf(fd, "G {}\nK ");
      save_ascii_string(xctx->schvhdlprop,fd, 1);
    } else {
      fprintf(fd, "G ");
      save_ascii_string(xctx->schvhdlprop,fd, 1);
      fprintf(fd, "K ");
      save_ascii_string(xctx->schsymbolprop,fd, 1);
    }
  } else {
    fprintf(fd, "G ");
    save_ascii_string(xctx->schvhdlprop,fd, 1);
    fprintf(fd, "K ");
    save_ascii_string(xctx->schsymbolprop,fd, 1);
  }

  fprintf(fd, "V ");
  save_ascii_string(xctx->schverilogprop,fd, 1);

  fprintf(fd, "S ");
  save_ascii_string(xctx->schprop,fd, 1);

  fprintf(fd, "E ");
  save_ascii_string(xctx->schtedaxprop,fd, 1);

  save_line(fd, 0);
  save_box(fd, 0);
  save_arc(fd, 0);
  save_polygon(fd, 0);
  save_text(fd, 0);
  save_wire(fd, 0);
  save_inst(fd, 0);
}

static void load_text(FILE *fd)
{
  int i;
  const char *str;
   dbg(3, "load_text(): start\n");
   check_text_storage();
   i=xctx->texts;
   xctx->text[i].txt_ptr=NULL;
   load_ascii_string(&xctx->text[i].txt_ptr,fd);
   if(fscanf(fd, "%lf %lf %hd %hd %lf %lf ",
     &xctx->text[i].x0, &xctx->text[i].y0, &xctx->text[i].rot,
     &xctx->text[i].flip, &xctx->text[i].xscale,
     &xctx->text[i].yscale)<6) {
     fprintf(errfp,"WARNING:  missing fields for TEXT object, ignoring\n");
     read_line(fd, 0);
     return;
   }
   xctx->text[i].prop_ptr=NULL;
   xctx->text[i].font=NULL;
   xctx->text[i].sel=0;
   load_ascii_string(&xctx->text[i].prop_ptr,fd);
   if( xctx->text[i].prop_ptr)
     my_strdup(318, &xctx->text[i].font, get_tok_value(xctx->text[i].prop_ptr, "font", 0));

   str = get_tok_value(xctx->text[i].prop_ptr, "hcenter", 0);
   xctx->text[i].hcenter = strcmp(str, "true")  ? 0 : 1;
   str = get_tok_value(xctx->text[i].prop_ptr, "vcenter", 0);
   xctx->text[i].vcenter = strcmp(str, "true")  ? 0 : 1;

   str = get_tok_value(xctx->text[i].prop_ptr, "layer", 0);
   if(str[0]) xctx->text[i].layer = atoi(str);
   else xctx->text[i].layer = -1;
   xctx->text[i].flags = 0;
   str = get_tok_value(xctx->text[i].prop_ptr, "slant", 0);
   xctx->text[i].flags |= strcmp(str, "oblique") ? 0 : TEXT_OBLIQUE;
   xctx->text[i].flags |= strcmp(str, "italic") ? 0 : TEXT_ITALIC;
   str = get_tok_value(xctx->text[i].prop_ptr, "weight", 0);
   xctx->text[i].flags |= strcmp(str, "bold") ? 0 : TEXT_BOLD;
   str = get_tok_value(xctx->text[i].prop_ptr, "hide", 0);
   xctx->text[i].flags |= strcmp(str, "true") ? 0 : SYM_HIDE_TEXT;

   xctx->texts++;
}

static void load_wire(FILE *fd)
{
    xWire *ptr;
    int i;

    check_wire_storage();
    ptr = xctx->wire;
    i = xctx->wires;
    dbg(3, "load_wire(): start\n");
    if(fscanf(fd, "%lf %lf %lf %lf",&ptr[i].x1, &ptr[i].y1, &ptr[i].x2, &ptr[i].y2 )<4) {
      fprintf(errfp,"WARNING:  missing fields for WIRE object, ignoring\n");
      read_line(fd, 0);
      return;
    }
    ptr[i].prop_ptr = NULL;
    ptr[i].end1 = ptr[i].end2 = ptr[i].bus = ptr[i].sel = 0;
    load_ascii_string( &ptr[i].prop_ptr, fd);
    ORDER(ptr[i].x1, ptr[i].y1, ptr[i].x2, ptr[i].y2);
    if(!strcmp(get_tok_value(ptr[i].prop_ptr, "bus", 0), "true") ) ptr[i].bus = 1;
    ptr[i].node = NULL;
    xctx->wires++;
}

static void load_inst(int k, FILE *fd)
{
    int i;
    char *prop_ptr=NULL;
    char name[PATH_MAX];
    char *tmp = NULL;

    i=xctx->instances;
    check_inst_storage();
    load_ascii_string(&tmp, fd);
    if(!tmp) return;
    my_strncpy(name, tmp, S(name));
    dbg(1, "load_inst(): 1: name=%s\n", name);
    if(!strcmp(xctx->file_version,"1.0") ) {
      my_strncpy(name, add_ext(name, ".sym"), S(name));
    }
    xctx->inst[i].name=NULL;
    /* avoid as much as possible calls to rel_sym_path (slow) */
    #ifdef __unix__
    if(name[0] == '/') my_strdup2(56, &xctx->inst[i].name, rel_sym_path(name));
    else my_strdup2(762, &xctx->inst[i].name, name);
    #else 
    my_strdup2(777, &xctx->inst[i].name, rel_sym_path(name));
    #endif
    if(fscanf(fd, "%lf %lf %hd %hd", &xctx->inst[i].x0, &xctx->inst[i].y0,
       &xctx->inst[i].rot, &xctx->inst[i].flip) < 4) {
      fprintf(errfp,"WARNING: missing fields for INSTANCE object, ignoring.\n");
      read_line(fd, 0);
    } else {
      xctx->inst[i].color=-10000;
      xctx->inst[i].flags=0;
      xctx->inst[i].sel=0;
      xctx->inst[i].ptr=-1; /*04112003 was 0 */
      xctx->inst[i].prop_ptr=NULL;
      xctx->inst[i].instname=NULL;
      xctx->inst[i].lab=NULL; /* assigned in link_symbols_to_instances */
      xctx->inst[i].node=NULL;
      load_ascii_string(&prop_ptr,fd);
      my_strdup(319, &xctx->inst[i].prop_ptr, prop_ptr);
      my_strdup2(320, &xctx->inst[i].instname, get_tok_value(xctx->inst[i].prop_ptr, "name", 0));
      if(!strcmp(get_tok_value(xctx->inst[i].prop_ptr,"highlight",0), "true")) xctx->inst[i].flags |= 4;

      dbg(2, "load_inst(): n=%d name=%s prop=%s\n", i, xctx->inst[i].name? xctx->inst[i].name:"<NULL>",
               xctx->inst[i].prop_ptr? xctx->inst[i].prop_ptr:"<NULL>");
      xctx->instances++;
    }
    my_free(884, &tmp);
    my_free(885, &prop_ptr);
}

static void load_polygon(FILE *fd)
{
    int i,c, j, points;
    xPoly *ptr;
    const char *dash;

    dbg(3, "load_polygon(): start\n");
    if(fscanf(fd, "%d %d",&c, &points)<2) {
      fprintf(errfp,"WARNING: missing fields for POLYGON object, ignoring.\n");
      read_line(fd, 0);
      return;
    }
    if(c<0 || c>=cadlayers) {
      fprintf(errfp,"WARNING: wrong layer number for POLYGON object, ignoring.\n");
      read_line(fd, 0);
      return;
    }
    check_polygon_storage(c);
    i=xctx->polygons[c];
    ptr=xctx->poly[c];
    ptr[i].x=NULL;
    ptr[i].y=NULL;
    ptr[i].selected_point=NULL;
    ptr[i].prop_ptr=NULL;
    ptr[i].x = my_calloc(321, points, sizeof(double));
    ptr[i].y = my_calloc(322, points, sizeof(double));
    ptr[i].selected_point= my_calloc(323, points, sizeof(unsigned short));
    ptr[i].points=points;
    ptr[i].sel=0;
    for(j=0;j<points;j++) {
      if(fscanf(fd, "%lf %lf ",&(ptr[i].x[j]), &(ptr[i].y[j]))<2) {
        fprintf(errfp,"WARNING: missing fields for POLYGON points, ignoring.\n");
        my_free(886, &ptr[i].x);
        my_free(887, &ptr[i].y);
        my_free(888, &ptr[i].selected_point);
        read_line(fd, 0);
        return;
      }
    }
    load_ascii_string( &ptr[i].prop_ptr, fd);
    if( !strcmp(get_tok_value(ptr[i].prop_ptr,"fill",0),"true") )
      ptr[i].fill =1;
    else
      ptr[i].fill =0;
    dash = get_tok_value(ptr[i].prop_ptr,"dash",0);
    if(strcmp(dash, "")) {
      int d = atoi(dash);
      ptr[i].dash = d >= 0 ? d : 0;
    } else {
      ptr[i].dash = 0;
    }

    xctx->polygons[c]++;
}

static void load_arc(FILE *fd)
{
    int n,i,c;
    xArc *ptr;
    const char *dash;

    dbg(3, "load_arc(): start\n");
    n = fscanf(fd, "%d",&c);
    if(n != 1 || c < 0 || c >= cadlayers) {
      fprintf(errfp,"WARNING: wrong or missing layer number for ARC object, ignoring.\n");
      read_line(fd, 0);
      return;
    }
    check_arc_storage(c);
    i=xctx->arcs[c];
    ptr=xctx->arc[c];
    if(fscanf(fd, "%lf %lf %lf %lf %lf ",&ptr[i].x, &ptr[i].y,
           &ptr[i].r, &ptr[i].a, &ptr[i].b) < 5) {
      fprintf(errfp,"WARNING:  missing fields for ARC object, ignoring\n");
      read_line(fd, 0);
      return;
    }
    ptr[i].prop_ptr=NULL;
    ptr[i].sel=0;
    load_ascii_string(&ptr[i].prop_ptr, fd);
    if( !strcmp(get_tok_value(ptr[i].prop_ptr,"fill",0),"true") )
      ptr[i].fill =1;
    else
      ptr[i].fill =0;
    dash = get_tok_value(ptr[i].prop_ptr,"dash",0);
    if(strcmp(dash, "")) {
      int d = atoi(dash);
      ptr[i].dash = d >= 0 ? d : 0;
    } else {
      ptr[i].dash = 0;
    }
    xctx->arcs[c]++;
}

static void load_box(FILE *fd)
{
    int i,n,c;
    xRect *ptr;
    const char *dash, *flags;

    dbg(3, "load_box(): start\n");
    n = fscanf(fd, "%d",&c);
    if(n != 1 || c < 0 || c >= cadlayers) {
      fprintf(errfp,"WARNING: wrong or missing layer number for xRECT object, ignoring.\n");
      read_line(fd, 0);
      return;
    }
    check_box_storage(c);
    i=xctx->rects[c];
    ptr=xctx->rect[c];
    if(fscanf(fd, "%lf %lf %lf %lf ",&ptr[i].x1, &ptr[i].y1,
       &ptr[i].x2, &ptr[i].y2) < 4) {
      fprintf(errfp,"WARNING:  missing fields for xRECT object, ignoring\n");
      read_line(fd, 0);
      return;
    }
    RECTORDER(ptr[i].x1, ptr[i].y1, ptr[i].x2, ptr[i].y2);
    ptr[i].prop_ptr=NULL;
    ptr[i].sel=0;
    load_ascii_string( &ptr[i].prop_ptr, fd);
    dash = get_tok_value(ptr[i].prop_ptr,"dash",0);
    if(strcmp(dash, "")) {
      int d = atoi(dash);
      ptr[i].dash = d >= 0 ? d : 0;
    } else {
      ptr[i].dash = 0;
    }
    flags = get_tok_value(ptr[i].prop_ptr,"flags",0);
    if(strcmp(flags, "")) {
      int d = atoi(flags);
      ptr[i].flags = d >= 0 ? d : 0;
    } else {
      ptr[i].flags = 0;
    }
    xctx->rects[c]++;
}

static void load_line(FILE *fd)
{
    int i,n, c;
    xLine *ptr;
    const char *dash;

    dbg(3, "load_line(): start\n");
    n = fscanf(fd, "%d",&c);
    if(n != 1 || c < 0 || c >= cadlayers) {
      fprintf(errfp,"WARNING: Wrong or missing layer number for LINE object, ignoring\n");
      read_line(fd, 0);
      return;
    }
    check_line_storage(c);
    i=xctx->lines[c];
    ptr=xctx->line[c];
    if(fscanf(fd, "%lf %lf %lf %lf ",&ptr[i].x1, &ptr[i].y1, &ptr[i].x2, &ptr[i].y2) < 4) {
      fprintf(errfp,"WARNING:  missing fields for LINE object, ignoring\n");
      read_line(fd, 0);
      return;
    }
    ORDER(ptr[i].x1, ptr[i].y1, ptr[i].x2, ptr[i].y2);
    ptr[i].prop_ptr=NULL;
    ptr[i].sel=0;
    load_ascii_string( &ptr[i].prop_ptr, fd);

    if(!strcmp(get_tok_value(ptr[i].prop_ptr, "bus", 0), "true") )
      ptr[i].bus = 1;
    else
      ptr[i].bus = 0;

    dash = get_tok_value(ptr[i].prop_ptr,"dash",0);
    if(strcmp(dash, "")) {
      int d = atoi(dash);
      ptr[i].dash = d >= 0 ? d : 0;
    } else {
      ptr[i].dash = 0;
    }
    xctx->lines[c]++;
}

void read_xschem_file(FILE *fd)
{
  int i, found, endfile;
  char name_embedded[PATH_MAX];
  char tag[1];
  int inst_cnt;
  int ty=0;

  dbg(2, "read_xschem_file(): start\n");
  inst_cnt = endfile = 0;
  xctx->file_version[0] = '\0';
  while(!endfile)
  {
    if(fscanf(fd," %c",tag)==EOF) break; /* space before %c --> eat white space */
    switch(tag[0])
    {
     case 'v':
      load_ascii_string(&xctx->version_string, fd);
      if(xctx->version_string) {
        my_snprintf(xctx->file_version, S(xctx->file_version), "%s",
                    get_tok_value(xctx->version_string, "file_version", 0));
      }
      dbg(1, "read_xschem_file(): file_version=%s\n", xctx->file_version);
      break;
     case '#':
      read_line(fd, 1);
      break;
     case 'E':
      load_ascii_string(&xctx->schtedaxprop,fd);
      break;
     case 'S':
      load_ascii_string(&xctx->schprop,fd);
      break;
     case 'V':
      load_ascii_string(&xctx->schverilogprop,fd);
      break;
     case 'K':
      load_ascii_string(&xctx->schsymbolprop,fd);
      break;
     case 'G':
      load_ascii_string(&xctx->schvhdlprop,fd);
      break;
     case 'L':
      load_line(fd);
      break;
     case 'P':
      load_polygon(fd);
      break;
     case 'A':
      load_arc(fd);
      break;
     case 'B':
      load_box(fd);
      break;
     case 'T':
      load_text(fd);
      break;
     case 'N':
      load_wire(fd);
      break;
     case 'C':
      load_inst(inst_cnt++, fd);
      break;
     case '[':
      my_strdup(324, &xctx->inst[xctx->instances-1].prop_ptr,
                subst_token(xctx->inst[xctx->instances-1].prop_ptr, "embed", "true"));

      if(xctx->inst[xctx->instances-1].name) {
        char *str;
        my_snprintf(name_embedded, S(name_embedded), "%s/.xschem_embedded_%d_%s",
                    tclgetvar("XSCHEM_TMP_DIR"), getpid(), get_cell_w_ext(xctx->inst[xctx->instances-1].name, 0));
        found=0;
        for(i=0;i<xctx->symbols;i++)
        {
         dbg(1, "read_xschem_file(): sym[i].name=%s, name_embedded=%s\n", xctx->sym[i].name, name_embedded);
         dbg(1, "read_xschem_file(): inst[instances-1].name=%s\n", xctx->inst[xctx->instances-1].name);
         /* symbol has already been loaded: skip [..] */
         if(!strcmp(xctx->sym[i].name, xctx->inst[xctx->instances-1].name)) {
           found=1; break;
         }
         /* if loading file coming back from embedded symbol delete temporary file */
         if(!strcmp(name_embedded, xctx->sym[i].name)) {
           my_strdup2(325, &xctx->sym[i].name, xctx->inst[xctx->instances-1].name);
           xunlink(name_embedded);
           found=1;break;
         }
        }
        read_line(fd, 0); /* skip garbage after '[' */
        if(!found) load_sym_def(xctx->inst[xctx->instances-1].name, fd);
        else {
          while(1) { /* skip embedded [ ... ] */
            int n;
            str = read_line(fd, 1);
            if(!str || !strncmp(str, "]", 1)) break;
            n = fscanf(fd, " ");
            (void)n; /* avoid compiler warnings if n unused. can not remove n since ignoring 
                      * fscanf return value yields another warning */
          }
        }
      }
      break;
     default:
      if( tag[0] == '{' ) ungetc(tag[0], fd);
      read_record(tag[0], fd, 0);
      break;
    }
    read_line(fd, 0); /* discard any remaining characters till (but not including) newline */

    if(xctx->schvhdlprop) {
      char *str = xctx->sch[xctx->currsch];
      get_tok_value(xctx->schvhdlprop, "type",0);
      ty = xctx->get_tok_size;
      if(!xctx->schsymbolprop && ty && !strcmp(str + strlen(str) - 4,".sym")) {
        str = xctx->schsymbolprop;
        xctx->schsymbolprop = xctx->schvhdlprop;
        xctx->schvhdlprop = str;
      }
    }
    if(!xctx->file_version[0]) {
      my_snprintf(xctx->file_version, S(xctx->file_version), "1.0");
      dbg(1, "read_xschem_file(): no file_version, assuming file_version=%s\n", xctx->file_version);
    }
  }
}

void load_ascii_string(char **ptr, FILE *fd)
{
 int c, escape=0;
 int i=0, begin=0;
 char *str=NULL;
 int strlength=0;

 for(;;)
 {
  if(i+5>strlength) my_realloc(326, &str,(strlength+=CADCHUNKALLOC));
  c=fgetc(fd);
  if (c=='\r') continue;
  if(c==EOF) {
    fprintf(errfp, "EOF reached, malformed {...} string input, missing close brace\n");
    my_free(1149, ptr);
    my_free(889, &str);
    return;
  }
  if(begin) {
    if(!escape) {
      if(c=='}') {
        str[i]='\0';
        break;
      }
      if(c=='\\') {
        escape=1;
        continue;
      }
    }
    str[i]=c;
    escape = 0;
    i++;
  } else if(c=='{') begin=1;
 }
 dbg(2, "load_ascii_string(): string read=%s\n",str? str:"<NULL>");
 my_strdup(329, ptr, str);
 dbg(2, "load_ascii_string(): loaded %s\n",*ptr? *ptr:"<NULL>");
 my_free(891, &str);
}

void make_symbol(void)
{
 char name[1024]; /* overflow safe 20161122 */

 if( strcmp(xctx->sch[xctx->currsch],"") )
 {
  my_snprintf(name, S(name), "make_symbol {%s}", xctx->sch[xctx->currsch] );
  dbg(1, "make_symbol(): making symbol: name=%s\n", name);
  tcleval(name);
 }

}

void make_schematic(const char *schname)
{
  FILE *fd=NULL;

  rebuild_selected_array();
  if(!xctx->lastsel)  return;
  if (!(fd = fopen(schname, "w")))
  {
    fprintf(errfp, "make_schematic(): problems opening file %s \n", schname);
    tcleval("alert_ {file opening for write failed!} {}");
    return;
  }
  fprintf(fd, "v {xschem version=%s file_version=%s}\n", XSCHEM_VERSION, XSCHEM_FILE_VERSION);
  fprintf(fd, "G {}");
  fputc('\n', fd);
  fprintf(fd, "V {}");
  fputc('\n', fd);
  fprintf(fd, "E {}");
  fputc('\n', fd);
  fprintf(fd, "S {}");
  fputc('\n', fd);
  fprintf(fd, "K {type=subcircuit\nformat=\"@name @pinlist @symname\"\n");
  fprintf(fd, "%s\n", "template=\"name=x1\"");
  fprintf(fd, "%s", "}\n");
  fputc('\n', fd);
  save_line(fd, 1);
  save_box(fd, 1);
  save_arc(fd, 1);
  save_polygon(fd, 1);
  save_text(fd, 1);
  save_wire(fd, 1);
  save_inst(fd, 1);
  fclose(fd);
}

/* ALWAYS call with absolute path in schname!!! */
/* return value:
 *   0 : did not save
 *   1 : schematic saved
 */
int save_schematic(const char *schname) /* 20171020 added return value */
{
  FILE *fd;
  char name[PATH_MAX]; /* overflow safe 20161122 */
  struct stat buf;

  if( strcmp(schname,"") ) my_strncpy(xctx->sch[xctx->currsch], schname, S(xctx->sch[xctx->currsch]));
  else return 0;
  dbg(1, "save_schematic(): currsch=%d name=%s\n",xctx->currsch, schname);
  dbg(1, "save_schematic(): sch[currsch]=%s\n", xctx->sch[xctx->currsch]);
  dbg(1, "save_schematic(): abs_sym_path=%s\n", abs_sym_path(xctx->sch[xctx->currsch], ""));
  my_strncpy(name, xctx->sch[xctx->currsch], S(name));
  set_modify(-1);
  if(!stat(name, &buf)) {
    if(xctx->time_last_modify && xctx->time_last_modify != buf.st_mtime) {
      tclvareval("ask_save \"Schematic file: ", name,
          "\nHas been changed since opening.\nSave anyway?\" 0", NULL);
      if(strcmp(tclresult(), "yes") ) return 0;
    }
  }
  if(!(fd=fopen(name,"w")))
  {
    fprintf(errfp, "save_schematic(): problems opening file %s \n",name);
    tcleval("alert_ {file opening for write failed!} {}");
    return 0;
  }
  unselect_all();
  write_xschem_file(fd);
  fclose(fd);
  /* update time stamp */
  if(!stat(name, &buf)) {
    xctx->time_last_modify =  buf.st_mtime;
  }
  my_strncpy(xctx->current_name, rel_sym_path(name), S(xctx->current_name));
  /* why clear all these? */
  /* 
   * xctx->prep_hi_structs=0;
   * xctx->prep_net_structs=0;
   * xctx->prep_hash_inst=0;
   * xctx->prep_hash_wires=0;
   */
  if(!strstr(xctx->sch[xctx->currsch], ".xschem_embedded_")) {
     set_modify(0);
  }
  return 1;
}

/* from == -1 --> link symbols to all instances, from 0 to instances-1 */
void link_symbols_to_instances(int from) /* from >= 0 : linking symbols from pasted schematic / clipboard */
{
  int cond, i, merge = 1;
  char *type=NULL;

  if(from < 0 ) {
    from = 0;
    merge = 0;
  }
  for(i = from; i < xctx->instances; i++) {
    dbg(2, "link_symbols_to_instances(): inst=%d\n", i);
    dbg(2, "link_symbols_to_instances(): matching inst %d name=%s \n",i, xctx->inst[i].name);
    dbg(2, "link_symbols_to_instances(): -------\n");
    xctx->inst[i].ptr = match_symbol(xctx->inst[i].name);
  }
  for(i = from; i < xctx->instances; i++) {
    if(merge) select_element(i,SELECTED,1, 0); /* leave elements selected if a paste/copy from windows is done */
    type=xctx->sym[xctx->inst[i].ptr].type;
    cond= !type || !IS_LABEL_SH_OR_PIN(type);
    if(cond) xctx->inst[i].flags|=2; /* ordinary symbol */
    else {
      xctx->inst[i].flags &=~2; /* label or pin */
      my_strdup(1216, &xctx->inst[i].lab, get_tok_value(xctx->inst[i].prop_ptr,"lab",0));
    }
  }
  /* symbol_bbox() might call translate() that might call prepare_netlist_structs() that 
   * needs .lab field set above, so this must be done last */
  for(i = from; i < xctx->instances; i++) {
    symbol_bbox(i, &xctx->inst[i].x1, &xctx->inst[i].y1, &xctx->inst[i].x2, &xctx->inst[i].y2);
  }
}

/* ALWAYS use absolute pathname for filename!!! */
void load_schematic(int load_symbols, const char *filename, int reset_undo) /* 20150327 added reset_undo */
{
  FILE *fd;
  char name[PATH_MAX];
  char msg[PATH_MAX+100];
  struct stat buf;
  int i;
  xctx->prep_hi_structs=0;
  xctx->prep_net_structs=0;
  xctx->prep_hash_inst=0;
  xctx->prep_hash_wires=0;
  if(reset_undo) xctx->clear_undo();
  if(reset_undo) xctx->prev_set_modify = -1; /* will force set_modify(0) to set window title */
  else  xctx->prev_set_modify = 0;           /* will prevent set_modify(0) from setting window title */
  if(filename && filename[0]) {
    my_strncpy(name, filename, S(name));
    my_strncpy(xctx->sch[xctx->currsch], name, S(xctx->sch[xctx->currsch]));
    /* if name is /some/path/.  remove /. at end */
    my_snprintf(msg, S(msg), "regsub {/\\.$} [file dirname {%s}] {}", name);
    my_strncpy(xctx->current_dirname,  tcleval(msg), S(xctx->current_dirname));
    my_strncpy(xctx->current_name, rel_sym_path(name), S(xctx->current_name));
    dbg(1, "load_schematic(): opening file for loading:%s, filename=%s\n", name, filename);
    dbg(1, "load_schematic(): sch[currsch]=%s\n", xctx->sch[xctx->currsch]);
    if(!name[0]) return;

    if(reset_undo) {
      if(!stat(name, &buf)) { /* file exists */
        xctx->time_last_modify =  buf.st_mtime;
      } else {
        xctx->time_last_modify = time(NULL); /* file does not exist, set mtime to current time */
      }
    }
    if( (fd=fopen(name,fopen_read_mode))== NULL) {
      fprintf(errfp, "load_schematic(): unable to open file: %s, filename=%s\n",
          name, filename ? filename : "<NULL>");
      my_snprintf(msg, S(msg), "update; alert_ {Unable to open file: %s}", filename ? filename: "(null)");
      tcleval(msg);
      clear_drawing();
    } else {
      clear_drawing();
      dbg(1, "load_schematic(): reading file: %s\n", name);
      read_xschem_file(fd);
      fclose(fd); /* 20150326 moved before load symbols */
      if(reset_undo) set_modify(0);
      dbg(2, "load_schematic(): loaded file:wire=%d inst=%d\n",xctx->wires , xctx->instances);
      if(load_symbols) link_symbols_to_instances(-1);
      if(reset_undo) {
        tclvareval("is_xschem_file {", xctx->sch[xctx->currsch], "}", NULL);
        if(!strcmp(tclresult(), "SYMBOL")) {
          xctx->save_netlist_type = xctx->netlist_type;
          xctx->netlist_type = CAD_SYMBOL_ATTRS;
          set_tcl_netlist_type();
          xctx->loaded_symbol = 1;
        } else {
          if(xctx->loaded_symbol) {
            xctx->netlist_type = xctx->save_netlist_type;
            set_tcl_netlist_type();
          }
          xctx->loaded_symbol = 0;
        }
      }
    }
    dbg(1, "load_schematic(): %s, returning\n", xctx->sch[xctx->currsch]);
  } else {
    if(reset_undo) xctx->time_last_modify = time(NULL); /* no file given, set mtime to current time */
    clear_drawing();
    for(i=0;;i++) {
      if(xctx->netlist_type == CAD_SYMBOL_ATTRS) {
        if(i == 0) my_snprintf(name, S(name), "%s.sym", "untitled");
        else my_snprintf(name, S(name), "%s-%d.sym", "untitled", i);
      } else {
        if(i == 0) my_snprintf(name, S(name), "%s.sch", "untitled");
        else my_snprintf(name, S(name), "%s-%d.sch", "untitled", i);
      }
      if(stat(name, &buf)) break;
    }
    my_snprintf(xctx->sch[xctx->currsch], S(xctx->sch[xctx->currsch]), "%s/%s", pwd_dir, name);
    my_strncpy(xctx->current_name, name, S(xctx->current_name));
    if(reset_undo) set_modify(0);
  }
  check_collapsing_objects();
  if(tclgetboolvar("autotrim_wires")) trim_wires();
  update_conn_cues(0, 0);
  if(xctx->hilight_nets && load_symbols) {
    propagate_hilights(1, 1, XINSERT_NOREPLACE);
  }
}

void clear_undo(void)
{
  xctx->cur_undo_ptr = 0;
  xctx->tail_undo_ptr = 0;
  xctx->head_undo_ptr = 0;
}

void delete_undo(void)
{
  int i;
  char diff_name[PATH_MAX]; /* overflow safe 20161122 */

  dbg(1, "delete_undo(): undo_initialized = %d\n", xctx->undo_initialized);
  if(!xctx->undo_initialized) return;
  clear_undo();
  for(i=0; i<MAX_UNDO; i++) {
    my_snprintf(diff_name, S(diff_name), "%s/undo%d",xctx->undo_dirname, i);
    xunlink(diff_name);
  }
  rmdir(xctx->undo_dirname);
  my_free(895, &xctx->undo_dirname);
  xctx->undo_initialized = 0;
}

/* create undo directory in XSCHEM_TEMP_DIR */
static void init_undo(void)
{
  if(!xctx->undo_initialized) {
    /* create undo directory */
    if( !my_strdup(644, &xctx->undo_dirname, create_tmpdir("xschem_undo_") )) {
      dbg(0, "xinit(): problems creating tmp undo dir, Undo will be disabled\n");
      dbg(0, "xinit(): Check permissions in %s\n", tclgetvar("XSCHEM_TMP_DIR"));
      xctx->no_undo = 1; /* disable undo */
    }
    xctx->undo_initialized = 1;
  }
}

void push_undo(void)
{
    #if HAS_PIPE==1
    int pd[2];
    pid_t pid;
    FILE *diff_fd;
    #endif
    FILE *fd;
    char diff_name[PATH_MAX+100]; /* overflow safe 20161122 */

    if(xctx->no_undo)return;
    dbg(1, "push_undo(): cur_undo_ptr=%d tail_undo_ptr=%d head_undo_ptr=%d\n",
       xctx->cur_undo_ptr, xctx->tail_undo_ptr, xctx->head_undo_ptr);
    init_undo();
    #if HAS_POPEN==1
    my_snprintf(diff_name, S(diff_name), "gzip --fast -c > %s/undo%d",
         xctx->undo_dirname, xctx->cur_undo_ptr%MAX_UNDO);
    fd = popen(diff_name,"w");
    if(!fd) {
      fprintf(errfp, "push_undo(): failed to open write pipe %s\n", diff_name);
      xctx->no_undo=1;
      return;
    }
    #elif HAS_PIPE==1
    my_snprintf(diff_name, S(diff_name), "%s/undo%d", xctx->undo_dirname, xctx->cur_undo_ptr%MAX_UNDO);
    pipe(pd);
    if((pid = fork()) ==0) {                                    /* child process */
      close(pd[1]);                                     /* close write side of pipe */
      if(!(diff_fd=freopen(diff_name,"w", stdout)))     /* redirect stdout to file diff_name */
      {
        dbg(1, "push_undo(): problems opening file %s \n",diff_name);
        tcleval("exit");
      }

      /* the following 2 statements are a replacement for dup2() which is not c89
       * however these are not atomic, if another thread takes stdin
       * in between we are in trouble */
      #if(HAS_DUP2) 
      dup2(pd[0], 0);
      #else
      close(0); /* close stdin */
      dup(pd[0]); /* duplicate read side of pipe to stdin */
      #endif
      execlp("gzip", "gzip", "--fast", "-c", NULL);       /* replace current process with comand */
      /* never gets here */
      fprintf(errfp, "push_undo(): problems with execlp\n");
      tcleval("exit");
    }
    close(pd[0]);                                       /* close read side of pipe */
    fd=fdopen(pd[1],"w");
    #else /* uncompressed undo */
    my_snprintf(diff_name, S(diff_name), "%s/undo%d", xctx->undo_dirname, xctx->cur_undo_ptr%MAX_UNDO);
    fd = fopen(diff_name,"w");
    if(!fd) {
      fprintf(errfp, "push_undo(): failed to open undo file %s\n", diff_name);
      xctx->no_undo=1;
      return;
    }
    #endif
    write_xschem_file(fd);
    xctx->cur_undo_ptr++;
    xctx->head_undo_ptr = xctx->cur_undo_ptr;
    xctx->tail_undo_ptr = xctx->head_undo_ptr <= MAX_UNDO? 0: xctx->head_undo_ptr-MAX_UNDO;
    #if HAS_POPEN==1
    pclose(fd);
    #elif HAS_PIPE==1
    fclose(fd);
    waitpid(pid, NULL,0);
    #else
    fclose(fd);
    #endif
}

/* redo:
 * 0: undo (with push current state for allowing following redo) 
 * 1: redo
 * 2: read top data from undo stack without changing undo stack
 */
void pop_undo(int redo, int set_modify_status)
{
  FILE *fd;
  char diff_name[PATH_MAX+12];
  #if HAS_PIPE==1
  int pd[2];
  pid_t pid;
  FILE *diff_fd;
  #endif

  if(xctx->no_undo) return;
  if(redo == 1) {
    if(xctx->cur_undo_ptr < xctx->head_undo_ptr) {
      dbg(1, "pop_undo(): redo; cur_undo_ptr=%d tail_undo_ptr=%d head_undo_ptr=%d\n",
         xctx->cur_undo_ptr, xctx->tail_undo_ptr, xctx->head_undo_ptr);
      xctx->cur_undo_ptr++;
    } else {
      return;
    }
  } else if(redo == 0) {  /* undo */
    if(xctx->cur_undo_ptr == xctx->tail_undo_ptr) return;
    dbg(1, "pop_undo(): undo; cur_undo_ptr=%d tail_undo_ptr=%d head_undo_ptr=%d\n",
       xctx->cur_undo_ptr, xctx->tail_undo_ptr, xctx->head_undo_ptr);
    if(xctx->head_undo_ptr == xctx->cur_undo_ptr) {
      xctx->push_undo();
      xctx->head_undo_ptr--;
      xctx->cur_undo_ptr--;
    }
    if(xctx->cur_undo_ptr<=0) return; /* check undo tail */
    xctx->cur_undo_ptr--;
  } else { /* redo == 2, get data without changing undo stack */
    if(xctx->cur_undo_ptr<=0) return; /* check undo tail */
    xctx->cur_undo_ptr--; /* will be restored after building file name */
  }
  clear_drawing();
  unselect_all();

  #if HAS_POPEN==1
  my_snprintf(diff_name, S(diff_name), "gzip -d -c %s/undo%d", xctx->undo_dirname, xctx->cur_undo_ptr%MAX_UNDO);
  fd=popen(diff_name, "r");
  if(!fd) {
    fprintf(errfp, "pop_undo(): failed to open read pipe %s\n", diff_name);
    xctx->no_undo=1;
    return;
  }
  #elif HAS_PIPE==1
  my_snprintf(diff_name, S(diff_name), "%s/undo%d", xctx->undo_dirname, xctx->cur_undo_ptr%MAX_UNDO);
  pipe(pd);
  if((pid = fork())==0) {                                     /* child process */
    close(pd[0]);                                    /* close read side of pipe */
    if(!(diff_fd=freopen(diff_name,"r", stdin)))     /* redirect stdin from file name */
    {
      dbg(1, "pop_undo(): problems opening file %s \n",diff_name);
      tcleval("exit");
    }
    /* connect write side of pipe to stdout */
    #if HAS_DUP2
    dup2(pd[1], 1);
    #else
    close(1);    /* close stdout */
    dup(pd[1]);  /* write side of pipe --> stdout */
    #endif
    execlp("gzip", "gzip", "-d", "-c", NULL);       /* replace current process with command */
    /* never gets here */
    dbg(1, "pop_undo(): problems with execlp\n");
    tcleval("exit");
  }
  close(pd[1]);                                       /* close write side of pipe */
  fd=fdopen(pd[0],"r");
  #else /* uncompressed undo */
  my_snprintf(diff_name, S(diff_name), "%s/undo%d", xctx->undo_dirname, xctx->cur_undo_ptr%MAX_UNDO);
  fd=fopen(diff_name, "r");
  if(!fd) {
    fprintf(errfp, "pop_undo(): failed to open read pipe %s\n", diff_name);
    xctx->no_undo=1;
    return;
  }
  #endif
  read_xschem_file(fd);
  if(redo == 2) xctx->cur_undo_ptr++; /* restore undo stack pointer */

  #if HAS_POPEN==1
  pclose(fd); /* 20150326 moved before load symbols */
  #elif HAS_PIPE==1
  fclose(fd);
  waitpid(pid, NULL, 0);
  #else
  fclose(fd);
  #endif
  dbg(2, "pop_undo(): loaded file:wire=%d inst=%d\n",xctx->wires , xctx->instances);
  if(set_modify_status) set_modify(1);
  xctx->prep_hash_inst=0;
  xctx->prep_hash_wires=0;
  xctx->prep_net_structs=0;
  xctx->prep_hi_structs=0;
  link_symbols_to_instances(-1);
  update_conn_cues(0, 0);
  if(xctx->hilight_nets) {
    propagate_hilights(1, 1, XINSERT_NOREPLACE);
  }
  dbg(2, "pop_undo(): returning\n");
}

/* given a 'symname' component instantiation in a LCC schematic instance
 * get the type attribute from symbol global properties.
 * first look in already loaded symbols else inspect symbol file
 * do not load all symname data, just get the type
 * return symbol type in type pointer or "" if no type or no symbol found
 * if pintable given (!=NULL) hash all symbol pins
 * if embed_fd is not NULL read symbol from embedded '[...]' tags using embed_fd file pointer */
static void get_sym_type(const char *symname, char **type, 
                         Int_hashentry **pintable, FILE *embed_fd, int *sym_n_pins)
{
  int i, c, n = 0;
  char name[PATH_MAX];
  FILE *fd;
  char tag[1];
  int found = 0;
  if(!strcmp(xctx->file_version,"1.0")) {
    my_strncpy(name, abs_sym_path(symname, ".sym"), S(name));
  } else {
    my_strncpy(name, abs_sym_path(symname, ""), S(name));
  }
  found=0;
  /* first look in already loaded symbols in xctx->sym[] array... */
  for(i=0;i<xctx->symbols;i++) {
    if(strcmp(symname, xctx->sym[i].name) == 0) {
      my_strdup2(316, type, xctx->sym[i].type);
      found = 1;
      break;
    }
  }
  /* hash pins to get LCC schematic have same order as corresponding symbol */
  if(found && pintable) {
    *sym_n_pins = xctx->sym[i].rects[PINLAYER];
    for (c = 0; c < xctx->sym[i].rects[PINLAYER]; c++) {
      int_hash_lookup(pintable, get_tok_value(xctx->sym[i].rect[PINLAYER][c].prop_ptr, "name", 0), c, XINSERT);
    }
  }
  if( !found ) {
    dbg(1, "get_sym_type(): open file %s, pintable %s\n",name, pintable ? "set" : "null");
    /* ... if not found open file and look for 'type' into the global attributes. */

    if(embed_fd) fd = embed_fd;
    else fd=fopen(name,fopen_read_mode);

    if(fd==NULL) {
      dbg(1, "get_sym_type(): Symbol not found: %s\n",name);
      my_strdup2(1162, type, "");
    } else {
      char *globalprop=NULL;
      int fscan_ret;
      xRect rect;

      rect.prop_ptr = NULL;
      while(1) {
        if(fscanf(fd," %c",tag)==EOF) break;
        if(embed_fd && tag[0] == ']') break;
        switch(tag[0]) {
          case 'G':
            load_ascii_string(&globalprop,fd);
            if(!found) {
              my_strdup2(1164, type, get_tok_value(globalprop, "type", 0));
            }
            break;
          case 'K':
            load_ascii_string(&globalprop,fd);
            my_strdup2(1165, type, get_tok_value(globalprop, "type", 0));
            if(type[0]) found = 1;
            break;
          case 'B':
           fscan_ret = fscanf(fd, "%d",&c);
           if(fscan_ret != 1 || c <0 || c>=cadlayers) {
             fprintf(errfp,"get_sym_type(): box layer wrong or missing or > defined cadlayers, "
                           "ignoring, increase cadlayers\n");
             ungetc(tag[0], fd);
             read_record(tag[0], fd, 1);
           }
           fscan_ret = fscanf(fd, "%lf %lf %lf %lf ",&rect.x1, &rect.y1, &rect.x2, &rect.y2);
           if(fscan_ret < 4) dbg(0, "Warning: missing fields in 'B' line\n");
           load_ascii_string( &rect.prop_ptr, fd);
           dbg(1, "get_sym_type(): %s rect.prop_ptr=%s\n", symname, rect.prop_ptr);
           if (pintable && c == PINLAYER) {
             /* hash pins to get LCC schematic have same order as corresponding symbol */
             int_hash_lookup(pintable, get_tok_value(rect.prop_ptr, "name", 0), n++, XINSERT);
             dbg(1, "get_sym_type() : hashing %s\n", get_tok_value(rect.prop_ptr, "name", 0));
             ++(*sym_n_pins);
           }
           break;
          default:
            if( tag[0] == '{' ) ungetc(tag[0], fd);
            read_record(tag[0], fd, 1);
            break;
        }
        read_line(fd, 0); /* discard any remaining characters till (but not including) newline */
      }
      my_free(1166, &globalprop);
      my_free(1167, &rect.prop_ptr);
      if(!embed_fd) fclose(fd);
    }
  }
  dbg(1, "get_sym_type(): symbol=%s --> type=%s\n", symname, *type);
}


/* given a .sch file used as instance in LCC schematics, order its pin
 * as in corresponding .sym file if it exists */
static void align_sch_pins_with_sym(const char *name, int pos)
{
  char *ptr;
  char symname[PATH_MAX];
  char *symtype = NULL;
  const char *pinname;
  int i, fail = 0, sym_n_pins=0;
  Int_hashentry *pintable[HASHSIZE];

  if ((ptr = strrchr(name, '.')) && !strcmp(ptr, ".sch")) {
    my_strncpy(symname, add_ext(name, ".sym"), S(symname));
    memset(pintable, 0, HASHSIZE * sizeof(Int_hashentry *));
    /* hash all symbol pins with their position into pintable hash*/
    get_sym_type(symname, &symtype, pintable, NULL, &sym_n_pins);
    if(symtype[0]) { /* found a .sym for current .sch LCC instance */
      xRect *rect = NULL;
      if (sym_n_pins!=xctx->sym[pos].rects[PINLAYER]) {
        dbg(0, " align_sch_pins_with_sym(): warning: number of pins mismatch between %s and %s\n",
          name, symname);
        fail = 1;
      }
      rect = (xRect *) my_malloc(1168, sizeof(xRect) * sym_n_pins);
      dbg(1, "align_sch_pins_with_sym(): symbol: %s\n", symname);
      for(i=0; i < xctx->sym[pos].rects[PINLAYER]; i++) {
        Int_hashentry *entry;
        pinname = get_tok_value(xctx->sym[pos].rect[PINLAYER][i].prop_ptr, "name", 0);
        entry = int_hash_lookup(pintable, pinname, 0 , XLOOKUP);
        if(!entry) {
          dbg(0, " align_sch_pins_with_sym(): warning: pin mismatch between %s and %s : %s\n",
            name, symname, pinname);
          fail = 1;
          break;
        }
        rect[entry->value] = xctx->sym[pos].rect[PINLAYER][i]; /* rect[] is the pin array ordered as in symbol */
        dbg(1, "align_sch_pins_with_sym(): i=%d, pin name=%s entry->value=%d\n", i, pinname, entry->value);
      }
      if(!fail) {
        /* copy rect[] ordererd array to LCC schematic instance */
        for(i=0; i < xctx->sym[pos].rects[PINLAYER]; i++) {
          xctx->sym[pos].rect[PINLAYER][i] = rect[i];
        }
      }
      int_hash_free(pintable);
      my_free(1169, &rect);
    }
    my_free(1170, &symtype);
  }
}

/* replace i/o/iopin instances of LCC schematics with symbol pins (boxes on PINLAYER layer) */
static void add_pinlayer_boxes(int *lastr, xRect **bb,
                 const char *symtype, char *prop_ptr, double i_x0, double i_y0)
{
  int i, save;
  const char *label;
  char *pin_label = NULL;

  i = lastr[PINLAYER];
  my_realloc(652, &bb[PINLAYER], (i + 1) * sizeof(xRect));
  bb[PINLAYER][i].x1 = i_x0 - 2.5; bb[PINLAYER][i].x2 = i_x0 + 2.5;
  bb[PINLAYER][i].y1 = i_y0 - 2.5; bb[PINLAYER][i].y2 = i_y0 + 2.5;
  RECTORDER(bb[PINLAYER][i].x1, bb[PINLAYER][i].y1, bb[PINLAYER][i].x2, bb[PINLAYER][i].y2);
  bb[PINLAYER][i].prop_ptr = NULL;
  label = get_tok_value(prop_ptr, "lab", 0);
  save = strlen(label)+30;
  pin_label = my_malloc(315, save);
  pin_label[0] = '\0';
  if (!strcmp(symtype, "ipin")) {
    my_snprintf(pin_label, save, "name=%s dir=in ", label);
  } else if (!strcmp(symtype, "opin")) {
    my_snprintf(pin_label, save, "name=%s dir=out ", label);
  } else if (!strcmp(symtype, "iopin")) {
    my_snprintf(pin_label, save, "name=%s dir=inout ", label);
  }
  my_strdup(463, &bb[PINLAYER][i].prop_ptr, pin_label);
  bb[PINLAYER][i].dash = 0;
  bb[PINLAYER][i].sel = 0;
  /* add to symbol pins remaining attributes from schematic pins, except name= and lab= */
  my_strdup(157, &pin_label, get_sym_template(prop_ptr, "lab"));   /* remove name=...  and lab=... */
  my_strcat(159, &bb[PINLAYER][i].prop_ptr, pin_label);
  my_free(900, &pin_label);
  lastr[PINLAYER]++;
}

static void use_lcc_pins(int level, char *symtype, char (*filename)[PATH_MAX])
{
  if(level == 0) {
    if (!strcmp(symtype, "ipin")) {
       my_snprintf(*filename, S(*filename), "%s/%s", tclgetvar("XSCHEM_SHAREDIR"), "systemlib/ipin_lcc_top.sym");
    } else if (!strcmp(symtype, "opin")) {
       my_snprintf(*filename, S(*filename), "%s/%s", tclgetvar("XSCHEM_SHAREDIR"), "systemlib/opin_lcc_top.sym");
    } else if (!strcmp(symtype, "iopin")) {
       my_snprintf(*filename, S(*filename), "%s/%s", tclgetvar("XSCHEM_SHAREDIR"), "systemlib/iopin_lcc_top.sym");
    }
  } else {
    if (!strcmp(symtype, "ipin")) {
       my_snprintf(*filename, S(*filename), "%s/%s", tclgetvar("XSCHEM_SHAREDIR"), "systemlib/ipin_lcc.sym");
    } else if (!strcmp(symtype, "opin")) {
       my_snprintf(*filename, S(*filename), "%s/%s", tclgetvar("XSCHEM_SHAREDIR"), "systemlib/opin_lcc.sym");
    } else if (!strcmp(symtype, "iopin")) {
       my_snprintf(*filename, S(*filename), "%s/%s", tclgetvar("XSCHEM_SHAREDIR"), "systemlib/iopin_lcc.sym");
    }
  }
}

void calc_symbol_bbox(int pos)
{
  int c, i, count = 0;
  xRect boundbox, tmp;

  boundbox.x1 = boundbox.x2 = boundbox.y1 = boundbox.y2 = 0;
  for(c=0;c<cadlayers;c++)
  {
   for(i=0;i<xctx->sym[pos].lines[c];i++)
   {
    count++;
    tmp.x1=xctx->sym[pos].line[c][i].x1;tmp.y1=xctx->sym[pos].line[c][i].y1;
    tmp.x2=xctx->sym[pos].line[c][i].x2;tmp.y2=xctx->sym[pos].line[c][i].y2;
    updatebbox(count,&boundbox,&tmp);
    dbg(2, "calc_symbol_bbox(): line[%d][%d]: %g %g %g %g\n",
			c, i, tmp.x1,tmp.y1,tmp.x2,tmp.y2);
   }
   for(i=0;i<xctx->sym[pos].arcs[c];i++)
   {
    count++;
    arc_bbox(xctx->sym[pos].arc[c][i].x, xctx->sym[pos].arc[c][i].y, xctx->sym[pos].arc[c][i].r,
             xctx->sym[pos].arc[c][i].a, xctx->sym[pos].arc[c][i].b,
             &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
    /* printf("arc bbox: %g %g %g %g\n", tmp.x1, tmp.y1, tmp.x2, tmp.y2); */
    updatebbox(count,&boundbox,&tmp);
   }
   for(i=0;i<xctx->sym[pos].rects[c];i++)
   {
    count++;
    tmp.x1=xctx->sym[pos].rect[c][i].x1;tmp.y1=xctx->sym[pos].rect[c][i].y1;
    tmp.x2=xctx->sym[pos].rect[c][i].x2;tmp.y2=xctx->sym[pos].rect[c][i].y2;
    updatebbox(count,&boundbox,&tmp);
   }
   for(i=0;i<xctx->sym[pos].polygons[c];i++)
   {
     double x1=0., y1=0., x2=0., y2=0.;
     int k;
     count++;
     for(k=0; k<xctx->sym[pos].poly[c][i].points; k++) {
       /*fprintf(errfp, "  poly: point %d: %.16g %.16g\n", k, pp[c][i].x[k], pp[c][i].y[k]); */
       if(k==0 || xctx->sym[pos].poly[c][i].x[k] < x1) x1 = xctx->sym[pos].poly[c][i].x[k];
       if(k==0 || xctx->sym[pos].poly[c][i].y[k] < y1) y1 = xctx->sym[pos].poly[c][i].y[k];
       if(k==0 || xctx->sym[pos].poly[c][i].x[k] > x2) x2 = xctx->sym[pos].poly[c][i].x[k];
       if(k==0 || xctx->sym[pos].poly[c][i].y[k] > y2) y2 = xctx->sym[pos].poly[c][i].y[k];
     }
     tmp.x1=x1;tmp.y1=y1;tmp.x2=x2;tmp.y2=y2;
     updatebbox(count,&boundbox,&tmp);
   }
  }
/*
*   do not include symbol text in bounding box, since text length
*   is variable from one instance to another due to '@' variable expansions
*
*   for(i=0;i<lastt;i++)
*   {
     int tmp;
*    count++;
*    rot=tt[i].rot;flip=tt[i].flip;
*    text_bbox(tt[i].txt_ptr, tt[i].xscale, tt[i].yscale, rot, flip,
*    tt[i].x0, tt[i].y0, &rx1,&ry1,&rx2,&ry2, &tmp);
*    tmp.x1=rx1;tmp.y1=ry1;tmp.x2=rx2;tmp.y2=ry2;
*    updatebbox(count,&boundbox,&tmp);
*  }
*/
  xctx->sym[pos].minx = boundbox.x1;
  xctx->sym[pos].maxx = boundbox.x2;
  xctx->sym[pos].miny = boundbox.y1;
  xctx->sym[pos].maxy = boundbox.y2;
}

/* Global (or static global) variables used:
 * cadlayers
 * errfp
 * xctx->file_version
 * xctx->sym
 * xctx->symbols
 * has_x
 */
int load_sym_def(const char *name, FILE *embed_fd)
{
  static int recursion_counter=0; /* safe to keep even with multiple schematics, operation not interruptable */
  Lcc *lcc; /* size = level */
  FILE *fd_tmp;
  short rot,flip;
  double angle;
  double rx1,ry1,rx2,ry2;
  int incremented_level=0;
  int level = 0;
  int max_level, fscan_ret;
  long filepos;
  char sympath[PATH_MAX];
  int i,c, k, poly_points;
  char *aux_ptr=NULL;
  char *prop_ptr=NULL, *symtype=NULL;
  double inst_x0, inst_y0;
  short inst_rot, inst_flip;
  char *symname = NULL;
  char tag[1];
  int *lastl = my_malloc(333, cadlayers * sizeof(lastl));
  int *lastr = my_malloc(334, cadlayers * sizeof(int));
  int *lastp = my_malloc(335, cadlayers * sizeof(int));
  int *lasta = my_malloc(336, cadlayers * sizeof(int));
  xLine **ll = my_malloc(337, cadlayers * sizeof(xLine *));
  xRect **bb = my_malloc(338, cadlayers * sizeof(xRect *));
  xArc **aa = my_malloc(339, cadlayers * sizeof(xArc *));
  xPoly **pp = my_malloc(340, cadlayers * sizeof(xPoly *));
  int lastt;
  xText *tt;
  int endfile;
  const char *str;
  char *skip_line;
  const char *dash;
  xSymbol * symbol;
  int symbols, sym_n_pins=0;

  check_symbol_storage();
  symbol = xctx->sym;
  symbols = xctx->symbols;
  dbg(1, "l_s_d(): recursion_counter=%d, name=%s\n", recursion_counter, name);
  recursion_counter++;
  dbg(1, "l_s_d(): name=%s\n", name);
  lcc=NULL;
  my_realloc(647, &lcc, (level + 1) * sizeof(Lcc));
  max_level = level + 1;
  if(!strcmp(xctx->file_version,"1.0")) {
    my_strncpy(sympath, abs_sym_path(name, ".sym"), S(sympath));
  } else {
    my_strncpy(sympath, abs_sym_path(name, ""), S(sympath));
  }
  if(!embed_fd) {
    if((lcc[level].fd=fopen(sympath,fopen_read_mode))==NULL)
    {
      if(recursion_counter == 1) dbg(0, "l_s_d(): Symbol not found: %s\n",sympath);
      my_snprintf(sympath, S(sympath), "%s/%s", tclgetvar("XSCHEM_SHAREDIR"), "systemlib/missing.sym");
      if((lcc[level].fd=fopen(sympath, fopen_read_mode))==NULL)
      {
       fprintf(errfp, "l_s_d(): systemlib/missing.sym missing, I give up\n");
       tcleval("exit");
      }
    }
    dbg(1, "l_s_d(): fopen1(%s), level=%d, fd=%p\n",sympath, level, lcc[level].fd);
  } else {
    dbg(1, "l_s_d(): getting embed_fd, level=%d\n", level);
    lcc[level].fd = embed_fd;
  }
  endfile=0;
  for(c=0;c<cadlayers;c++)
  {
   lasta[c] = lastl[c] = lastr[c] = lastp[c] = 0;
   ll[c] = NULL; bb[c] = NULL; pp[c] = NULL; aa[c] = NULL;
  }
  lastt=0;
  tt=NULL;
  symbol[symbols].prop_ptr = NULL;
  symbol[symbols].type = NULL;
  symbol[symbols].templ = NULL;
  symbol[symbols].name=NULL;
  my_strdup2(352, &symbol[symbols].name,name);
  while(1)
  {
   if(endfile && embed_fd && level == 0) break; /* ']' line encountered --> exit */
   if(fscanf(lcc[level].fd," %c",tag)==EOF) {
     if (level) {
         dbg(1, "l_s_d(): fclose1, level=%d, fd=%p\n", level, lcc[level].fd);
         fclose(lcc[level].fd);
         my_free(898, &lcc[level].prop_ptr);
         my_free(899, &lcc[level].symname);
         --level;
         continue;
     } else break;
   }
   if(endfile) { /* endfile due to max hierarchy: throw away rest of file and do the above '--level' cleanups */
     read_record(tag[0], lcc[level].fd, 0);
     continue;
   }
   incremented_level = 0;
   switch(tag[0])
   {
    case 'v':
     load_ascii_string(&aux_ptr, lcc[level].fd);
     break;
    case '#':
     read_line(lcc[level].fd, 1);
     break;
    case 'E':
     load_ascii_string(&aux_ptr, lcc[level].fd);
     break;
    case 'V':
     load_ascii_string(&aux_ptr, lcc[level].fd);
     break;
    case 'S':
     load_ascii_string(&aux_ptr, lcc[level].fd);
     break;
    case 'K': /* 1.2 file format: symbol attributes for schematics placed as symbols */
     if (level==0) {
       load_ascii_string(&symbol[symbols].prop_ptr, lcc[level].fd);
       if(!symbol[symbols].prop_ptr) break;
       my_strdup2(424, &symbol[symbols].templ,
                  get_tok_value(symbol[symbols].prop_ptr, "template", 0));
       my_strdup2(515, &symbol[symbols].type,
                  get_tok_value(symbol[symbols].prop_ptr, "type",0));
       if(!strcmp(get_tok_value(symbol[symbols].prop_ptr,"highlight",0), "true")) symbol[symbols].flags |= 4;
       else symbol[symbols].flags &= ~4;

     }
     else {
       load_ascii_string(&aux_ptr, lcc[level].fd);
     }
     break;
    case 'G': /* .sym files or pre-1.2 symbol attributes for schematics placed as symbols */
     if (level==0 && !symbol[symbols].prop_ptr) {
       load_ascii_string(&symbol[symbols].prop_ptr, lcc[level].fd);
       if(!symbol[symbols].prop_ptr) break;
       my_strdup2(341, &symbol[symbols].templ,
                  get_tok_value(symbol[symbols].prop_ptr, "template", 0));
       my_strdup2(342, &symbol[symbols].type,
                  get_tok_value(symbol[symbols].prop_ptr, "type",0));
       if(!strcmp(get_tok_value(symbol[symbols].prop_ptr,"highlight",0), "true")) symbol[symbols].flags |= 4;
       else symbol[symbols].flags &= ~4;
     }
     else {
       load_ascii_string(&aux_ptr, lcc[level].fd);
     }
     break;
    case 'L':
     fscan_ret = fscanf(lcc[level].fd, "%d",&c);
     if(fscan_ret != 1 || c < 0 || c>=cadlayers) {
       fprintf(errfp,"l_s_d(): WARNING: wrong or missing line layer\n");
       read_line(lcc[level].fd, 0);
       continue;
     }
     i=lastl[c];
     my_realloc(343, &ll[c],(i+1)*sizeof(xLine));
     if(fscanf(lcc[level].fd, "%lf %lf %lf %lf ",&ll[c][i].x1, &ll[c][i].y1,
        &ll[c][i].x2, &ll[c][i].y2) < 4 ) {
       fprintf(errfp,"l_s_d(): WARNING:  missing fields for LINE object, ignoring\n");
       read_line(lcc[level].fd, 0);
       continue;
     }
     if (level>0) {
       rot = lcc[level].rot; flip = lcc[level].flip;
       ROTATION(rot, flip, 0.0, 0.0, ll[c][i].x1, ll[c][i].y1, rx1, ry1);
       ROTATION(rot, flip, 0.0, 0.0, ll[c][i].x2, ll[c][i].y2, rx2, ry2);
       ll[c][i].x1 = lcc[level].x0 + rx1;  ll[c][i].y1 = lcc[level].y0 + ry1;
       ll[c][i].x2 = lcc[level].x0 + rx2;  ll[c][i].y2 = lcc[level].y0 + ry2;
     }
     ORDER(ll[c][i].x1, ll[c][i].y1, ll[c][i].x2, ll[c][i].y2);
     ll[c][i].prop_ptr=NULL;
     load_ascii_string( &ll[c][i].prop_ptr, lcc[level].fd);
     dbg(2, "l_s_d(): loaded line: ptr=%lx\n", (unsigned long)ll[c]);
     if(!strcmp(get_tok_value(ll[c][i].prop_ptr,"bus", 0), "true") )
       ll[c][i].bus = 1;
     else
       ll[c][i].bus = 0;
     dash = get_tok_value(ll[c][i].prop_ptr,"dash", 0);
     if( strcmp(dash, "") ) {
       int d = atoi(dash);
       ll[c][i].dash = d >= 0 ? d : 0;
     } else
       ll[c][i].dash = 0;
     ll[c][i].sel = 0;
     lastl[c]++;
     break;
    case 'P':
     if(fscanf(lcc[level].fd, "%d %d",&c, &poly_points) < 2 ) {
       fprintf(errfp,"l_s_d(): WARNING: missing fields for POLYGON object, ignoring\n");
       read_line(lcc[level].fd, 0);
       continue;
     }
     if(c < 0 || c>=cadlayers) {
       fprintf(errfp,"l_s_d(): WARNING: wrong polygon layer\n");
       read_line(lcc[level].fd, 0);
       continue;
     }
     i=lastp[c];
     my_realloc(344, &pp[c],(i+1)*sizeof(xPoly));
     pp[c][i].x = my_calloc(345, poly_points, sizeof(double));
     pp[c][i].y = my_calloc(346, poly_points, sizeof(double));
     pp[c][i].selected_point = my_calloc(347, poly_points, sizeof(unsigned short));
     pp[c][i].points = poly_points;
     for(k=0;k<poly_points;k++) {
       if(fscanf(lcc[level].fd, "%lf %lf ",&(pp[c][i].x[k]), &(pp[c][i].y[k]) ) < 2 ) {
         fprintf(errfp,"l_s_d(): WARNING: missing fields for POLYGON object\n");
       }
       if (level>0) {
         rot = lcc[level].rot; flip = lcc[level].flip;
         ROTATION(rot, flip, 0.0, 0.0, pp[c][i].x[k], pp[c][i].y[k], rx1, ry1);
         pp[c][i].x[k] = lcc[level].x0 + rx1;  pp[c][i].y[k] = lcc[level].y0 + ry1;
       }
     }
     pp[c][i].prop_ptr=NULL;
     load_ascii_string( &pp[c][i].prop_ptr, lcc[level].fd);
     if( !strcmp(get_tok_value(pp[c][i].prop_ptr,"fill",0),"true") )
       pp[c][i].fill =1;
     else
       pp[c][i].fill =0;

     dash = get_tok_value(pp[c][i].prop_ptr,"dash", 0);
     if( strcmp(dash, "") ) {
       int d = atoi(dash);
       pp[c][i].dash = d >= 0 ? d : 0;
     } else
       pp[c][i].dash = 0;
     pp[c][i].sel = 0;

     dbg(2, "l_s_d(): loaded polygon: ptr=%lx\n", (unsigned long)pp[c]);
     lastp[c]++;
     break;
    case 'A':
     fscan_ret = fscanf(lcc[level].fd, "%d",&c);
     if(fscan_ret != 1 || c < 0 || c>=cadlayers) {
       fprintf(errfp,"l_s_d(): Wrong or missing arc layer\n");
       read_line(lcc[level].fd, 0);
       continue;
     }
     i=lasta[c];
     my_realloc(348, &aa[c],(i+1)*sizeof(xArc));
     if( fscanf(lcc[level].fd, "%lf %lf %lf %lf %lf ",&aa[c][i].x, &aa[c][i].y,
        &aa[c][i].r, &aa[c][i].a, &aa[c][i].b) < 5 ) {
       fprintf(errfp,"l_s_d(): WARNING: missing fields for ARC object, ignoring\n");
       read_line(lcc[level].fd, 0);
       continue;
     }
     if (level>0) {
       rot = lcc[level].rot; flip = lcc[level].flip;
       if (flip) {
         angle = 270. * rot + 180. - aa[c][i].b - aa[c][i].a;
       }
       else {
         angle = aa[c][i].a + rot * 270.;
       }
       angle = fmod(angle, 360.);
       if (angle < 0.) angle += 360.;
       ROTATION(rot, flip, 0.0, 0.0, aa[c][i].x, aa[c][i].y, rx1, ry1);
       aa[c][i].x = lcc[level].x0 + rx1;  aa[c][i].y = lcc[level].y0 + ry1;
       aa[c][i].a = angle;
     }
     aa[c][i].prop_ptr=NULL;
     load_ascii_string( &aa[c][i].prop_ptr, lcc[level].fd);
     if( !strcmp(get_tok_value(aa[c][i].prop_ptr,"fill",0),"true") )
       aa[c][i].fill =1;
     else
       aa[c][i].fill =0;
     dash = get_tok_value(aa[c][i].prop_ptr,"dash", 0);
     if( strcmp(dash, "") ) {
       int d = atoi(dash);
       aa[c][i].dash = d >= 0 ? d : 0;
     } else
       aa[c][i].dash = 0;
     aa[c][i].sel = 0;
     dbg(2, "l_s_d(): loaded arc: ptr=%lx\n", (unsigned long)aa[c]);
     lasta[c]++;
     break;
    case 'B':
     fscan_ret = fscanf(lcc[level].fd, "%d",&c);
     if(fscan_ret != 1 || c < 0 || c>=cadlayers) {
       fprintf(errfp,"l_s_d(): WARNING: wrong or missing box layer\n");
       read_line(lcc[level].fd, 0);
       continue;
     }
     if (level>0 && c == PINLAYER)  /* Don't care about pins inside SYM */
       c = 7;
     i=lastr[c];
     my_realloc(349, &bb[c],(i+1)*sizeof(xRect));
     if(fscanf(lcc[level].fd, "%lf %lf %lf %lf ",&bb[c][i].x1, &bb[c][i].y1,
        &bb[c][i].x2, &bb[c][i].y2) < 4 ) {
       fprintf(errfp,"l_s_d(): WARNING:  missing fields for Box object, ignoring\n");
       read_line(lcc[level].fd, 0);
       continue;
     }
     if (level>0) {
       rot = lcc[level].rot; flip = lcc[level].flip;
       ROTATION(rot, flip, 0.0, 0.0, bb[c][i].x1, bb[c][i].y1, rx1, ry1);
       ROTATION(rot, flip, 0.0, 0.0, bb[c][i].x2, bb[c][i].y2, rx2, ry2);
       bb[c][i].x1 = lcc[level].x0 + rx1;  bb[c][i].y1 = lcc[level].y0 + ry1;
       bb[c][i].x2 = lcc[level].x0 + rx2;  bb[c][i].y2 = lcc[level].y0 + ry2;
     }
     RECTORDER(bb[c][i].x1, bb[c][i].y1, bb[c][i].x2, bb[c][i].y2);
     bb[c][i].prop_ptr=NULL;
     load_ascii_string( &bb[c][i].prop_ptr, lcc[level].fd);
     dbg(2, "l_s_d(): loaded rect: ptr=%lx\n", (unsigned long)bb[c]);

     dash = get_tok_value(bb[c][i].prop_ptr,"dash", 0);
     if( strcmp(dash, "") ) {
       int d = atoi(dash);
       bb[c][i].dash = d >= 0 ? d : 0;
     } else
       bb[c][i].dash = 0;
     bb[c][i].sel = 0;
     lastr[c]++;
     break;
    case 'T':
     i=lastt;
     my_realloc(350, &tt,(i+1)*sizeof(xText));
     tt[i].txt_ptr=NULL;
     tt[i].font=NULL;
     load_ascii_string(&tt[i].txt_ptr, lcc[level].fd);
     if(fscanf(lcc[level].fd, "%lf %lf %hd %hd %lf %lf ",&tt[i].x0, &tt[i].y0, &tt[i].rot,
        &tt[i].flip, &tt[i].xscale, &tt[i].yscale) < 6 ) {
       fprintf(errfp,"l_s_d(): WARNING:  missing fields for Text object, ignoring\n");
       read_line(lcc[level].fd, 0);
       continue;
     }
     if (level>0) {
       const char* tmp = translate2(lcc, level, tt[i].txt_ptr);
       rot = lcc[level].rot; flip = lcc[level].flip;
       if (tmp) my_strdup(651, &tt[i].txt_ptr, tmp);
       ROTATION(rot, flip, 0.0, 0.0, tt[i].x0, tt[i].y0, rx1, ry1);
       tt[i].x0 = lcc[level].x0 + rx1;  tt[i].y0 = lcc[level].y0 + ry1;
       tt[i].rot = (tt[i].rot + ((lcc[level].flip && (tt[i].rot & 1)) ?
                   lcc[level].rot + 2 : lcc[level].rot)) & 0x3;
       tt[i].flip = lcc[level].flip ^ tt[i].flip;
     }
     tt[i].prop_ptr=NULL;
     load_ascii_string(&tt[i].prop_ptr, lcc[level].fd);
     if(level > 0 && symtype && !strcmp(symtype, "label")) {
       char lay[30];
       my_snprintf(lay, S(lay), " layer=%d", WIRELAYER);
       my_strcat(1163, &tt[i].prop_ptr, lay);
     }
     dbg(1, "l_s_d(): loaded text : t=%s p=%s\n", tt[i].txt_ptr, tt[i].prop_ptr ? tt[i].prop_ptr : "NULL");
     my_strdup(351, &tt[i].font, get_tok_value(tt[i].prop_ptr, "font", 0));
     str = get_tok_value(tt[i].prop_ptr, "hcenter", 0);
     tt[i].hcenter = strcmp(str, "true")  ? 0 : 1;
     str = get_tok_value(tt[i].prop_ptr, "vcenter", 0);
     tt[i].vcenter = strcmp(str, "true")  ? 0 : 1;
     str = get_tok_value(tt[i].prop_ptr, "layer", 0);
     if(str[0]) tt[i].layer = atoi(str);
     else tt[i].layer = -1;
     tt[i].flags = 0;
     str = get_tok_value(tt[i].prop_ptr, "slant", 0);
     tt[i].flags |= strcmp(str, "oblique")  ? 0 : TEXT_OBLIQUE;
     tt[i].flags |= strcmp(str, "italic")  ? 0 : TEXT_ITALIC;
     str = get_tok_value(tt[i].prop_ptr, "weight", 0);
     tt[i].flags |= strcmp(str, "bold")  ? 0 : TEXT_BOLD;
     str = get_tok_value(tt[i].prop_ptr, "hide", 0);
     tt[i].flags |= strcmp(str, "true")  ? 0 : SYM_HIDE_TEXT;
     lastt++;
     break;
    case 'N': /* store wires as lines on layer WIRELAYER. */
     i = lastl[WIRELAYER];
     my_realloc(314, &ll[WIRELAYER],(i+1)*sizeof(xLine));
     if(fscanf(lcc[level].fd, "%lf %lf %lf %lf ",&ll[WIRELAYER][i].x1, &ll[WIRELAYER][i].y1,
        &ll[WIRELAYER][i].x2, &ll[WIRELAYER][i].y2) < 4 ) {
       fprintf(errfp,"l_s_d(): WARNING:  missing fields for LINE object, ignoring\n");
       read_line(lcc[level].fd, 0);
       continue;
     }
     if (level>0) {
       rot = lcc[level].rot; flip = lcc[level].flip;
       ROTATION(rot, flip, 0.0, 0.0, ll[WIRELAYER][i].x1, ll[WIRELAYER][i].y1, rx1, ry1);
       ROTATION(rot, flip, 0.0, 0.0, ll[WIRELAYER][i].x2, ll[WIRELAYER][i].y2, rx2, ry2);
       ll[WIRELAYER][i].x1 = lcc[level].x0 + rx1;  ll[WIRELAYER][i].y1 = lcc[level].y0 + ry1;
       ll[WIRELAYER][i].x2 = lcc[level].x0 + rx2;  ll[WIRELAYER][i].y2 = lcc[level].y0 + ry2;
     }
     ORDER(ll[WIRELAYER][i].x1, ll[WIRELAYER][i].y1, ll[WIRELAYER][i].x2, ll[WIRELAYER][i].y2);
     ll[WIRELAYER][i].prop_ptr=NULL;
     load_ascii_string( &ll[WIRELAYER][i].prop_ptr, lcc[level].fd);
     dbg(2, "l_s_d(): loaded line: ptr=%lx\n", (unsigned long)ll[WIRELAYER]);
     ll[WIRELAYER][i].dash = 0;
     if(!strcmp(get_tok_value(ll[WIRELAYER][i].prop_ptr, "bus", 0), "true"))
       ll[WIRELAYER][i].bus = 1;
     else
       ll[WIRELAYER][i].bus = 0;
     ll[WIRELAYER][i].sel = 0;
     lastl[WIRELAYER]++;
     break;
    case 'C':
      load_ascii_string(&symname, lcc[level].fd);
      dbg(1, "l_s_d(): C line: symname=%s\n", symname);
      if (fscanf(lcc[level].fd, "%lf %lf %hd %hd", &inst_x0, &inst_y0, &inst_rot, &inst_flip) < 4) {
        fprintf(errfp, "l_s_d(): WARNING: missing fields for COMPONENT object, ignoring\n");
        read_line(lcc[level].fd, 0);
        continue;
      }
      load_ascii_string(&prop_ptr, lcc[level].fd);
      if(level + 1 >=CADMAXHIER) {
        fprintf(errfp, "l_s_d(): Symbol recursively instantiating symbol: max depth reached, skipping\n");
        if(has_x) tcleval("alert_ {xSymbol recursively instantiating symbol: max depth reached, skipping} {} 1");
        endfile = 1;
        continue;
      }

      {
        char c;
        filepos = xftell(lcc[level].fd); /* store file pointer position to inspect next line */
        fd_tmp = NULL;
        read_line(lcc[level].fd, 1);
        fscan_ret = fscanf(lcc[level].fd, " ");
        if(fscanf(lcc[level].fd," %c",&c)!=EOF) {
          if( c == '[') {
            fd_tmp = lcc[level].fd;
          }
        }
        /* get symbol type by looking into list of loaded symbols or (if not found) by
         * opening/closing the symbol file and getting the 'type' attribute from global symbol attributes
         * if fd_tmp set read symbol from embedded tags '[...]' */
        get_sym_type(symname, &symtype, NULL, fd_tmp, &sym_n_pins);
        xfseek(lcc[level].fd, filepos, SEEK_SET); /* rewind file pointer */
      }

      dbg(1, "l_s_d(): level=%d, symname=%s symtype=%s\n", level, symname, symtype);
      if(  /* add here symbol types not to consider when loading schematic-as-symbol instances */
          !strcmp(symtype, "logo") ||
          !strcmp(symtype, "netlist_commands") ||
          !strcmp(symtype, "arch_declarations") ||
          !strcmp(symtype, "architecture") ||
          !strcmp(symtype, "attributes") ||
          !strcmp(symtype, "package") ||
          !strcmp(symtype, "port_attributes") ||
          !strcmp(symtype, "use") ||
          !strcmp(symtype, "launcher") ||
          !strcmp(symtype, "verilog_preprocessor") ||
          !strcmp(symtype, "timescale")
        ) break;
      /* add PINLAYER boxes (symbol pins) at schematic i/o/iopin coordinates. */
      if( level==0 && IS_PIN(symtype) ) {
        add_pinlayer_boxes(lastr, bb, symtype, prop_ptr, inst_x0, inst_y0);
      }
      /* build symbol filename to be loaded */
      if (!strcmp(xctx->file_version, "1.0")) {
        my_strncpy(sympath, abs_sym_path(symname, ".sym"), S(sympath));
      }
      else {
        my_strncpy(sympath, abs_sym_path(symname, ""), S(sympath));
      }
      /* replace i/o/iopin.sym filename with better looking (for LCC symbol) pins */
      use_lcc_pins(level, symtype, &sympath);

      /* find out if symbol is in an external file or embedded, set fd_tmp accordingly */
      if ((fd_tmp = fopen(sympath, fopen_read_mode)) == NULL) {
        char c;
        fprintf(errfp, "l_s_d(): unable to open file to read schematic: %s\n", sympath);
        filepos = xftell(lcc[level].fd); /* store file pointer position to inspect next char */
        read_line(lcc[level].fd, 1);
        fscan_ret = fscanf(lcc[level].fd, " ");
        if(fscanf(lcc[level].fd," %c",&c)!=EOF) {
          if( c == '[') {
            fd_tmp = lcc[level].fd;
          } else {
            xfseek(lcc[level].fd, filepos, SEEK_SET); /* rewind file pointer */
          }
        }
      }
      if(fd_tmp) {
        if (level+1 >= max_level) {
          my_realloc(653, &lcc, (max_level + 1) * sizeof(Lcc));
          max_level++;
        }
        ++level;
        incremented_level = 1;
        lcc[level].fd = fd_tmp;
        lcc[level].prop_ptr = NULL;
        lcc[level].symname = NULL;
        lcc[level].x0 = inst_x0;
        lcc[level].y0 = inst_y0;
        lcc[level].rot = inst_rot;
        lcc[level].flip = inst_flip;
        /* calculate LCC sub-schematic x0, y0, rotation and flip */
        if (level > 1) {
          short rot, flip;
          static const int map[4]={0,3,2,1};

          flip = lcc[level-1].flip;
          rot = lcc[level-1].rot;
          ROTATION(rot, flip, 0.0, 0.0, lcc[level].x0, lcc[level].y0,lcc[level].x0, lcc[level].y0);
          lcc[level].rot = (lcc[(level-1)].flip ? map[lcc[level].rot] : lcc[level].rot) + lcc[(level-1)].rot;
          lcc[level].rot &= 0x3;
          lcc[level].flip = lcc[level].flip ^ lcc[level-1].flip;
          lcc[level].x0 += lcc[(level-1)].x0;
          lcc[level].y0 += lcc[(level-1)].y0;
        }
        my_strdup(654, &lcc[level].prop_ptr, prop_ptr);
        my_strdup(657, &lcc[level].symname, symname);
        dbg(1, "level incremented: level=%d, symname=%s, prop_ptr=%s sympath=%s\n", 
          level, symname, prop_ptr, sympath);
      }
      break;
    case '[':
     while(1) { /* skip embedded [ ... ] */
       skip_line = read_line(lcc[level].fd, 1);
       if(!skip_line || !strncmp(skip_line, "]", 1)) break;
       fscan_ret = fscanf(lcc[level].fd, " ");
     }
     break;
    case ']':
     if(level) {
       my_free(1173, &lcc[level].prop_ptr);
       my_free(1174, &lcc[level].symname);
       --level;
     } else {
       endfile=1;
     }
     break;
    default:
     if( tag[0] == '{' ) ungetc(tag[0], lcc[level].fd);
     read_record(tag[0], lcc[level].fd, 0);
     break;
   }
   /* if a 'C' line was encountered and level was incremented, rest of line must be read
      with lcc[level-1].fd file pointer */
   if(incremented_level)
     read_line(lcc[level-1].fd, 0); /* discard any remaining characters till (but not including) newline */
   else
     read_line(lcc[level].fd, 0); /* discard any remaining characters till (but not including) newline */
  }
  if(!embed_fd) {
    dbg(1, "l_s_d(): fclose2, level=%d, fd=%p\n", level, lcc[0].fd);
    fclose(lcc[0].fd);
  }
  if(embed_fd || strstr(name, ".xschem_embedded_")) {
    symbol[symbols].flags |= EMBEDDED;
  } else {
    symbol[symbols].flags &= ~EMBEDDED;
  }
  dbg(2, "l_s_d(): finished parsing file\n");
  for(c=0;c<cadlayers;c++)
  {
   symbol[symbols].arcs[c] = lasta[c];
   symbol[symbols].lines[c] = lastl[c];
   symbol[symbols].rects[c] = lastr[c];
   symbol[symbols].polygons[c] = lastp[c];
   symbol[symbols].arc[c] = aa[c];
   symbol[symbols].line[c] = ll[c];
   symbol[symbols].poly[c] = pp[c];
   symbol[symbols].rect[c] = bb[c];
  }
  symbol[symbols].texts = lastt;
  symbol[symbols].text = tt;
  calc_symbol_bbox(symbols);
  /* given a .sch file used as instance in LCC schematics, order its pin
   * as in corresponding .sym file if it exists */
  align_sch_pins_with_sym(name, symbols);
  xctx->symbols++;
  my_free(910, &prop_ptr);
  my_free(901, &lastl);
  my_free(902, &lastr);
  my_free(903, &lastp);
  my_free(904, &lasta);
  my_free(905, &ll);
  my_free(906, &bb);
  my_free(907, &aa);
  my_free(908, &pp);
  my_free(909, &lcc);
  my_free(911, &aux_ptr);
  my_free(912, &symname);
  my_free(913, &symtype);
  recursion_counter--;
  return 1;
}

void make_schematic_symbol_from_sel(void)
{
  char filename[PATH_MAX] = "";
  char name[1024]; 

  my_snprintf(name, S(name), "save_file_dialog {Save file} .sch.sym INITIALLOADDIR");
  tcleval(name);
  my_strncpy(filename, tclresult(), S(filename));
  if (!strcmp(filename, xctx->sch[xctx->currsch])) {
    if (has_x)
      tcleval("tk_messageBox -type ok -parent [xschem get topwindow] "
              "-message {Cannot overwrite current schematic}");
  }
  else if (strlen(filename)) {
    if (xctx->lastsel) xctx->push_undo();
    make_schematic(filename);
    delete(0/*to_push_undo*/);
    place_symbol(-1, filename, 0, 0, 0, 0, NULL, 4, 1, 0/*to_push_undo*/);
    if (has_x)
    {
      my_snprintf(name, S(name), 
        "tk_messageBox -type okcancel -parent [xschem get topwindow] "
        "-message {do you want to make symbol view for %s ?}", filename);
      tcleval(name);
    }
    if (!has_x || !strcmp(tclresult(), "ok")) {
      my_snprintf(name, S(name), "make_symbol_lcc {%s}", filename);
      dbg(1, "make_symbol_lcc(): making symbol: name=%s\n", filename);
      tcleval(name);
    }
    draw();
  }
}

void create_sch_from_sym(void)
{
  xSymbol *ptr;
  int i, j, npin, ypos;
  double x;
  int p=0;
  xRect *rct;
  FILE *fd;
  char *pindir[3] = {"in", "out", "inout"};
  char *pinname[3] = {"devices/ipin.sym", "devices/opin.sym", "devices/iopin.sym"};
  char *generic_pin = {"devices/generic_pin.sym"};
  char *pinname2[3] = {"ipin.sym", "opin.sym", "iopin.sym"};
  char *generic_pin2 = {"generic_pin.sym"};
  int indirect;

  char *dir = NULL;
  char *prop = NULL;
  char schname[PATH_MAX];
  char *sub_prop;
  char *sub2_prop=NULL;
  char *str=NULL;
  struct stat buf;
  char *sch = NULL;
  int ln;

  if(!stat(abs_sym_path(pinname[0], ""), &buf)) {
    indirect=1;
  } else {
    indirect=0;
  }
  /* printf("indirect=%d\n", indirect); */

  rebuild_selected_array();
  if(xctx->lastsel > 1)  return;
  if(xctx->lastsel==1 && xctx->sel_array[0].type==ELEMENT)
  {
    my_strdup2(1250, &sch,
      get_tok_value((xctx->inst[xctx->sel_array[0].n].ptr+ xctx->sym)->prop_ptr, "schematic",0 ));
    my_strncpy(schname, abs_sym_path(sch, ""), S(schname));
    my_free(1251, &sch);
    if(!schname[0]) {
      my_strncpy(schname, add_ext(abs_sym_path(xctx->inst[xctx->sel_array[0].n].name, ""), ".sch"), S(schname));
    }
    if( !stat(schname, &buf) ) {
      tclvareval("ask_save \"Create schematic file: ", schname,
          "?\nWARNING: This schematic file already exists, it will be overwritten\"", NULL);
      if(strcmp(tclresult(), "yes") ) {
        return;
      }
    }
    if(!(fd=fopen(schname,"w")))
    {
      fprintf(errfp, "create_sch_from_sym(): problems opening file %s \n",schname);
      tcleval("alert_ {file opening for write failed!} {}");
      return;
    }
    fprintf(fd, "v {xschem version=%s file_version=%s}\n", XSCHEM_VERSION, XSCHEM_FILE_VERSION);
    fprintf(fd, "G {}");
    fputc('\n', fd);
    fprintf(fd, "V {}");
    fputc('\n', fd);
    fprintf(fd, "E {}");
    fputc('\n', fd);
    fprintf(fd, "S {}");
    fputc('\n', fd);
    ptr = xctx->inst[xctx->sel_array[0].n].ptr+xctx->sym;
    npin = ptr->rects[GENERICLAYER];
    rct = ptr->rect[GENERICLAYER];
    ypos=0;
    for(i=0;i<npin;i++) {
      my_strdup(356, &prop, rct[i].prop_ptr);
      if(!prop) continue;
      sub_prop=strstr(prop,"name=")+5;
      if(!sub_prop) continue;
      x=-120.0;
      ln = 100+strlen(sub_prop);
      my_realloc(357, &str, ln);
      my_snprintf(str, ln, "name=g%d lab=%s", p++, sub_prop);
      if(indirect)
        fprintf(fd, "C {%s} %.16g %.16g %.16g %.16g ", generic_pin, x, 20.0*(ypos++), 0.0, 0.0 );
      else
        fprintf(fd, "C {%s} %.16g %.16g %.16g %.16g ", generic_pin2, x, 20.0*(ypos++), 0.0, 0.0 );
      save_ascii_string(str, fd, 1);
    } /* for(i) */
    npin = ptr->rects[PINLAYER];
    rct = ptr->rect[PINLAYER];
    for(j=0;j<3;j++) {
      if(j==1) ypos=0;
      for(i=0;i<npin;i++) {
        my_strdup(358, &prop, rct[i].prop_ptr);
        if(!prop) continue;
        sub_prop=strstr(prop,"name=")+5;
        if(!sub_prop) continue;
        /* remove dir=... from prop string 20171004 */
        my_strdup(360, &sub2_prop, subst_token(sub_prop, "dir", NULL));

        my_strdup(361, &dir, get_tok_value(rct[i].prop_ptr,"dir",0));
        if(!sub2_prop) continue;
        if(!dir) continue;
        if(j==0) x=-120.0; else x=120.0;
        if(!strcmp(dir, pindir[j])) {
          ln = 100+strlen(sub2_prop);
          my_realloc(362, &str, ln);
          my_snprintf(str, ln, "name=g%d lab=%s", p++, sub2_prop);
          if(indirect)
            fprintf(fd, "C {%s} %.16g %.16g %.16g %.16g ", pinname[j], x, 20.0*(ypos++), 0.0, 0.0);
          else
            fprintf(fd, "C {%s} %.16g %.16g %.16g %.16g ", pinname2[j], x, 20.0*(ypos++), 0.0, 0.0);
          save_ascii_string(str, fd, 1);
        } /* if() */
      } /* for(i) */
    }  /* for(j) */
    fclose(fd);
  } /* if(xctx->lastsel...) */
  my_free(916, &dir);
  my_free(917, &prop);
  my_free(919, &sub2_prop);
  my_free(920, &str);
}

void descend_symbol(void)
{
  char *str=NULL;
  FILE *fd;
  char name[PATH_MAX];
  char name_embedded[PATH_MAX];
  rebuild_selected_array();
  if(xctx->lastsel > 1)  return;
  if(xctx->lastsel==1 && xctx->sel_array[0].type==ELEMENT) {
    if(xctx->modified)
    {
      int ret;
  
      ret = save(1);
      /* if circuit is changed but not saved before descending
       * state will be inconsistent when returning, can not propagare hilights
       * save() return value:
       *  1 : file saved 
       * -1 : user cancel
       *  0 : file not saved due to errors or per user request
       */
      if(ret == 0) clear_all_hilights();
      if(ret == -1) return; /* user cancel */
    }
    my_snprintf(name, S(name), "%s", xctx->inst[xctx->sel_array[0].n].name);
    /* dont allow descend in the default missing symbol */
    if((xctx->inst[xctx->sel_array[0].n].ptr+ xctx->sym)->type &&
       !strcmp( (xctx->inst[xctx->sel_array[0].n].ptr+ xctx->sym)->type,"missing")) return;
  }
  else return;

  /* build up current hierarchy path */
  my_strdup(363,  &str, xctx->inst[xctx->sel_array[0].n].instname);
  my_strdup(364, &xctx->sch_path[xctx->currsch+1], xctx->sch_path[xctx->currsch]);
  my_strcat(365, &xctx->sch_path[xctx->currsch+1], str);
  my_strcat(366, &xctx->sch_path[xctx->currsch+1], ".");
  xctx->sch_path_hash[xctx->currsch+1] = 0;
  xctx->sch_inst_number[xctx->currsch+1] = 1;
  my_free(921, &str);
  xctx->previous_instance[xctx->currsch]=xctx->sel_array[0].n;
  xctx->zoom_array[xctx->currsch].x=xctx->xorigin;
  xctx->zoom_array[xctx->currsch].y=xctx->yorigin;
  xctx->zoom_array[xctx->currsch].zoom=xctx->zoom;
  ++xctx->currsch;
  if((xctx->inst[xctx->sel_array[0].n].ptr+ xctx->sym)->flags & EMBEDDED ||
    !strcmp(get_tok_value(xctx->inst[xctx->sel_array[0].n].prop_ptr,"embed", 0), "true")) {
    /* save embedded symbol into a temporary file */
    my_snprintf(name_embedded, S(name_embedded),
      "%s/.xschem_embedded_%d_%s", tclgetvar("XSCHEM_TMP_DIR"), getpid(), get_cell_w_ext(name, 0));
    if(!(fd = fopen(name_embedded, "w")) ) {
      fprintf(errfp, "descend_symbol(): problems opening file %s \n", name_embedded);
    }
    save_embedded_symbol(xctx->inst[xctx->sel_array[0].n].ptr+xctx->sym, fd);
    fclose(fd);
    unselect_all();
    remove_symbols(); /* must follow save (if) embedded */
    /* load_symbol(name_embedded); */
    load_schematic(1, name_embedded, 1);
  } else {
    /* load_symbol(abs_sym_path(name, "")); */
    unselect_all();
    remove_symbols(); /* must follow save (if) embedded */
    load_schematic(1, abs_sym_path(name, ""), 1);
  }
  zoom_full(1, 0, 1, 0.97);
}

/* 20111023 align selected object to current grid setting */
#define SNAP_TO_GRID(a)  (a=my_round(( a)/c_snap)*c_snap )
void round_schematic_to_grid(double c_snap)
{
 int i, c, n, p;
 rebuild_selected_array();
 for(i=0;i<xctx->lastsel;i++)
 {
   c = xctx->sel_array[i].col; n = xctx->sel_array[i].n;
   switch(xctx->sel_array[i].type)
   {
     case xTEXT:
       SNAP_TO_GRID(xctx->text[n].x0);
       SNAP_TO_GRID(xctx->text[n].y0);
     break;

     case xRECT:
       if(c == PINLAYER) {
         double midx, midx_round, deltax;
         double midy, midy_round, deltay;
         midx_round = midx = (xctx->rect[c][n].x1 + xctx->rect[c][n].x2) / 2;
         midy_round = midy = (xctx->rect[c][n].y1 + xctx->rect[c][n].y2) / 2;
         SNAP_TO_GRID(midx_round);
         SNAP_TO_GRID(midy_round);
         deltax = midx_round - midx;
         deltay = midy_round - midy;
         xctx->rect[c][n].x1 += deltax;
         xctx->rect[c][n].x2 += deltax;
         xctx->rect[c][n].y1 += deltay;
         xctx->rect[c][n].y2 += deltay;
       } else {
         SNAP_TO_GRID(xctx->rect[c][n].x1);
         SNAP_TO_GRID(xctx->rect[c][n].y1);
         SNAP_TO_GRID(xctx->rect[c][n].x2);
         SNAP_TO_GRID(xctx->rect[c][n].y2);
       }
     break;

     case WIRE:
       SNAP_TO_GRID(xctx->wire[n].x1);
       SNAP_TO_GRID(xctx->wire[n].y1);
       SNAP_TO_GRID(xctx->wire[n].x2);
       SNAP_TO_GRID(xctx->wire[n].y2);
     break;

     case LINE:
       SNAP_TO_GRID(xctx->line[c][n].x1);
       SNAP_TO_GRID(xctx->line[c][n].y1);
       SNAP_TO_GRID(xctx->line[c][n].x2);
       SNAP_TO_GRID(xctx->line[c][n].y2);
     break;

     case ARC:
       SNAP_TO_GRID(xctx->arc[c][n].x);
       SNAP_TO_GRID(xctx->arc[c][n].y);
     break;

     case POLYGON:
       for(p=0;p<xctx->poly[c][n].points; p++) {
         SNAP_TO_GRID(xctx->poly[c][n].x[p]);
         SNAP_TO_GRID(xctx->poly[c][n].y[p]);
       }
     break;

     case ELEMENT:
       SNAP_TO_GRID(xctx->inst[n].x0);
       SNAP_TO_GRID(xctx->inst[n].y0);

       symbol_bbox(n, &xctx->inst[n].x1, &xctx->inst[n].y1, &xctx->inst[n].x2, &xctx->inst[n].y2);
     break;

     default:
     break;
   }
 }
}

/* what: */
/*                      1: save selection */
/*                      2: save clipboard */
void save_selection(int what)
{
 FILE *fd;
 int i, c, n, k;
 char name[PATH_MAX];

 dbg(3, "save_selection():\n");
 if(what==1)
   my_snprintf(name, S(name), "%s/%s.sch",user_conf_dir , ".selection");
 else /* what=2 */
   my_snprintf(name, S(name), "%s/%s.sch",user_conf_dir , ".clipboard");

 if(!(fd=fopen(name,"w")))
 {
    fprintf(errfp, "save_selection(): problems opening file %s \n", name);
    tcleval("alert_ {file opening for write failed!} {}");
    return;
 }
 fprintf(fd, "v {xschem version=%s file_version=%s}\n", XSCHEM_VERSION, XSCHEM_FILE_VERSION);
 fprintf(fd, "G { %.16g %.16g }\n", xctx->mousex_snap, xctx->mousey_snap);
 for(i=0;i<xctx->lastsel;i++)
 {
   c = xctx->sel_array[i].col;n = xctx->sel_array[i].n;
   switch(xctx->sel_array[i].type)
   {
     case xTEXT:
      fprintf(fd, "T ");
      save_ascii_string(xctx->text[n].txt_ptr,fd, 0);
      fprintf(fd, " %.16g %.16g %hd %hd %.16g %.16g ",
       xctx->text[n].x0, xctx->text[n].y0, xctx->text[n].rot, xctx->text[n].flip,
       xctx->text[n].xscale, xctx->text[n].yscale);
      save_ascii_string(xctx->text[n].prop_ptr,fd, 1);
     break;

     case ARC:
      fprintf(fd, "A %d %.16g %.16g %.16g %.16g %.16g ",
        c, xctx->arc[c][n].x, xctx->arc[c][n].y, xctx->arc[c][n].r,
       xctx->arc[c][n].a, xctx->arc[c][n].b);
      save_ascii_string(xctx->arc[c][n].prop_ptr,fd, 1);
     break;

     case xRECT:
      fprintf(fd, "B %d %.16g %.16g %.16g %.16g ", c,xctx->rect[c][n].x1, xctx->rect[c][n].y1,xctx->rect[c][n].x2,
       xctx->rect[c][n].y2);
      save_ascii_string(xctx->rect[c][n].prop_ptr,fd, 1);
     break;

     case POLYGON:
      fprintf(fd, "P %d %d ", c, xctx->poly[c][n].points);
      for(k=0; k<xctx->poly[c][n].points; k++) {
        fprintf(fd, "%.16g %.16g ", xctx->poly[c][n].x[k], xctx->poly[c][n].y[k]);
      }
      save_ascii_string(xctx->poly[c][n].prop_ptr,fd, 1);
     break;

     case WIRE:
      fprintf(fd, "N %.16g %.16g %.16g %.16g ",xctx->wire[n].x1, xctx->wire[n].y1,
        xctx->wire[n].x2, xctx->wire[n].y2);
      save_ascii_string(xctx->wire[n].prop_ptr,fd, 1);
     break;

     case LINE:
      fprintf(fd, "L %d %.16g %.16g %.16g %.16g ", c,xctx->line[c][n].x1, xctx->line[c][n].y1,
       xctx->line[c][n].x2, xctx->line[c][n].y2 );
      save_ascii_string(xctx->line[c][n].prop_ptr,fd, 1);
     break;

     case ELEMENT:
      fprintf(fd, "C ");
      save_ascii_string(xctx->inst[n].name,fd, 0);
      fprintf(fd, " %.16g %.16g %hd %hd ",xctx->inst[n].x0, xctx->inst[n].y0,
        xctx->inst[n].rot, xctx->inst[n].flip );
      save_ascii_string(xctx->inst[n].prop_ptr,fd, 1);
     break;

     default:
     break;
   }
 }
 fclose(fd);

}

