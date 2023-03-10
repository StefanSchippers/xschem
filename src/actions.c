/* File: actions.c
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

#include "xschem.h"
#ifdef __unix__
#include <sys/wait.h>  /* waitpid */
#endif

void here(double i)
{
  dbg(0, "here %g\n", i);
}

/* super simple 32 bit hashing function for files
 * It is suppoded to be used on text files.
 * Calculates the same hash on windows (crlf) and unix (lf) text files.
 * If you want high collision resistance and 
 * avoid 'birthday problem' collisions use a better hash function, like md5sum
 * or sha256sum 
 */
unsigned int hash_file(const char *f, int skip_path_lines)
{
  FILE *fd;
  int i;
  size_t n;
  int cr = 0;
  unsigned int h=5381;
  char *line = NULL;
  fd = fopen(f, "r"); /* windows won't return \r in the lines and we chop them out anyway in the code */
  if(fd) {
    while((line = my_fgets(fd, &n))) {
      /* skip lines of type: '** sch_path: ...' or '-- sch_path: ...' or '// sym_path: ...' */
      if(skip_path_lines && n > 14) {
        if(!strncmp(line+2, " sch_path: ", 11) || !strncmp(line+2, " sym_path: ", 11) ) {
          my_free(_ALLOC_ID_, &line);
          continue;
        }
      }
      for(i = 0; i < n; ++i) {
        /* skip CRs so hashes will match on unix / windows */
        if(line[i] == '\r') {
          cr = 1;
          continue;
        } else if(line[i] == '\n' && cr) {
          cr = 0;
        } else if(cr) { /* no skip \r if not followed by \n */
          cr = 0;
          h += (h << 5) + '\r';
        }
        h += (h << 5) + (unsigned char)line[i];
      }
      my_free(_ALLOC_ID_, &line);
    } /* while(line ....) */
    if(cr) h += (h << 5) + '\r'; /* file ends with \r not followed by \n: keep it */
    fclose(fd);
    return h;
  } else {
    dbg(0, "Can not open file %s\n", f);
  }
  return 0;
}

/* mod=-1 used to force set title */
void set_modify(int mod)
{
  char *top_path;

  top_path =  xctx->top_path[0] ? xctx->top_path : ".";
  if(mod != -1) xctx->modified = mod;
  dbg(1, "set_modify(): %d\n", mod);
  if(mod == -1 || mod != xctx->prev_set_modify) { /* mod=-1 used to force set title */
    if(mod != -1) xctx->prev_set_modify = mod;
    else mod = xctx->modified;
    if(has_x && strcmp(get_cell(xctx->sch[xctx->currsch],1), "systemlib/font")) {
      if(mod == 1) {
        tclvareval("wm title ", top_path, " \"xschem - [file tail [xschem get schname]]*\"", NULL);
        tclvareval("wm iconname ", top_path, " \"xschem - [file tail [xschem get schname]]*\"", NULL);
      } else {
        tclvareval("wm title ", top_path, " \"xschem - [file tail [xschem get schname]]\"", NULL);
        tclvareval("wm iconname ", top_path, " \"xschem - [file tail [xschem get schname]]\"", NULL);
      }
    }
  }
  if(xctx->modified) tcleval("set_tab_names *");
  else tcleval("set_tab_names");
}

void print_version()
{
  printf("XSCHEM V%s\n", XSCHEM_VERSION);
  printf("Copyright 1998-2022 Stefan Schippers\n");
  printf("\n");
  printf("This is free software; see the source for copying conditions.  There is NO\n");
  printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
  exit(EXIT_SUCCESS);
}

#if 0
*   char *escape_chars(char *dest, const char *source, int size)
*   {
*     int s=0;
*     int d=0;
*     size--; /* reserve space for \0 */
*     while(source && source[s]) {
*       switch(source[s]) {
*         case '\n':
*           if(d < size-1) {
*             dest[d++] = '\\';
*             dest[d++] = 'n';
*           }
*           break;
*         case '\t':
*           if(d < size-1) {
*             dest[d++] = '\\';
*             dest[d++] = 't';
*           }
*           break;
*         case '\\':
*         case '\'':
*         case ' ':
*         case ';':
*         case '$':
*         case '!':
*         case '#':
*         case '{':
*         case '}':
*         case '[':
*         case ']':
*         case '"':
*           if(d < size-1) {
*              dest[d++] = '\\';
*              dest[d++] = source[s];
*           }
*           break;
*         default:
*           if(d < size) dest[d++] = source[s];
*       }
*       s++;
*     }
*     dest[d] = '\0';
*     return dest;
*   }
#endif

void set_snap(double newsnap) /*  20161212 set new snap factor and just notify new value */
{
    static double default_snap = -1.0; /* safe to keep even with multiple schematics, set at program start */
    double cs;

    cs = tclgetdoublevar("cadsnap");
    if(default_snap == -1.0) {
      default_snap = cs;
      if(default_snap==0.0) default_snap = CADSNAP;
    }
    cs = newsnap ? newsnap : default_snap;
    if(cs == default_snap) {
      tclvareval(xctx->top_path, ".statusbar.3 configure -background PaleGreen", NULL);
    } else {
      tclvareval(xctx->top_path, ".statusbar.3 configure -background OrangeRed", NULL);
    }
    tclsetdoublevar("cadsnap", cs);
}

void set_grid(double newgrid)
{
    static double default_grid = -1.0; /* safe to keep even with multiple schematics, set at program start */
    double cg;

    cg = tclgetdoublevar("cadgrid");
    if(default_grid == -1.0) {
      default_grid = cg;
      if(default_grid==0.0) default_grid = CADGRID;
    }
    cg = newgrid ? newgrid : default_grid;
    dbg(1, "set_grid(): default_grid = %.16g, cadgrid=%.16g\n", default_grid, cg);
    if(cg == default_grid) {
      tclvareval(xctx->top_path, ".statusbar.5 configure -background PaleGreen", NULL);
    } else {
      tclvareval(xctx->top_path, ".statusbar.5 configure -background OrangeRed", NULL);
    }
    tclsetdoublevar("cadgrid", cg);
}

int set_netlist_dir(int force, char *dir)
{
  char cmd[PATH_MAX+200];
  if(dir) my_snprintf(cmd, S(cmd), "select_netlist_dir %d {%s}", force, dir);
  else    my_snprintf(cmd, S(cmd), "select_netlist_dir %d", force);
  tcleval(cmd);
  if(!strcmp("", tclresult()) ) {
    return 0;
  }
  return 1;
}

/* wrapper to TCL function */
const char *abs_sym_path(const char *s, const char *ext)
{
  char c[PATH_MAX+1000];
  my_snprintf(c, S(c), "abs_sym_path {%s} {%s}", s, ext);
  tcleval(c);
  return tclresult();
}

/* Wrapper to Tcl function */
const char *rel_sym_path(const char *s)
{
  char c[PATH_MAX+1000];
  my_snprintf(c, S(c), "rel_sym_path {%s}", s);
  tcleval(c);
  return tclresult();
}

const char *add_ext(const char *f, const char *ext)
{
  static char ff[PATH_MAX]; /* safe to keep even with multiple schematics */
  char *p;
  int i;

  dbg(1, "add_ext(): f=%s ext=%s\n", f, ext);
  if((p=strrchr(f,'.'))) {
    my_strncpy(ff, f, (p-f) + 1);
    p = ff + (p-f);
    dbg(1, "add_ext(): 1: ff=%s\n", ff);
  } else {
    i = my_strncpy(ff, f, S(ff));
    p = ff+i;
    dbg(1, "add_ext(): 2: ff=%s\n", ff);
  }
  my_strncpy(p, ext, S(ff)-(p-ff));
  dbg(1, "add_ext(): 3: ff=%s\n", ff);
  return ff;
}

void toggle_only_probes()
{
  xctx->only_probes =  tclgetboolvar("only_probes");
  draw();
}

#ifdef __unix__
void new_xschem_process(const char *cell, int symbol)
{
  char f[PATH_MAX]; /*  overflow safe 20161122 */
  struct stat buf;
  pid_t pid1;
  pid_t pid2;
  int status;

  dbg(1, "new_xschem_process(): executable: %s, cell=%s, symbol=%d\n", xschem_executable, cell, symbol);
  if(stat(xschem_executable,&buf)) {
    fprintf(errfp, "new_xschem_process(): executable not found\n");
    return;
  }
  fflush(NULL); /* flush all stdio streams before process forking */
  /* double fork method to avoid zombies 20180925*/
  if ( (pid1 = fork()) > 0 ) {
    /* parent process */
    waitpid(pid1, &status, 0);
  } else if (pid1 == 0) {
    /* child process  */
    if ( (pid2 = fork()) > 0 ) {
      _exit(0); /* --> child of child will be reparented to init */
    } else if (pid2 == 0) {
      /* child of child */
      if(!cell || !cell[0]) {
        if(!symbol)
          execl(xschem_executable,xschem_executable, "-b", "-s", "--tcl",
                "set XSCHEM_START_WINDOW {}", NULL);
        else
          execl(xschem_executable,xschem_executable, "-b", "-y", "--tcl",
                "set XSCHEM_START_WINDOW {}", NULL);
      }
      else if(!symbol) {
        my_strncpy(f, cell, S(f));
        execl(xschem_executable,xschem_executable, "-b", "-s", f, NULL);
      }
      else {
        my_strncpy(f, cell, S(f));
        execl(xschem_executable,xschem_executable, "-b", "-y", f, NULL);
      }
    } else {
      /* error */
      fprintf(errfp, "new_xschem_process(): fork error 1\n");
      _exit(1);
    }
  } else {
    /* error */
    fprintf(errfp, "new_xschem_process(): fork error 2\n");
    tcleval("exit");
  }
}
#else

void new_xschem_process(const char* cell, int symbol)
{
  char cmd_line[2 * PATH_MAX + 100];
  struct stat buf;
  dbg(1, "new_xschem_process(): executable: %s, cell=%s, symbol=%d\n", xschem_executable, cell, symbol);
  if (stat(xschem_executable, &buf)) {
    fprintf(errfp, "new_xschem_process(): executable not found\n");
    return;
  }
  /* According to Stackoverflow, system should be avoided because it's resource heavy
  *  and not secure.
  *  Furthermore, system doesn't spawn a TCL shell with XSchem
  */
  /* int result = system(xschem_executable); */
  STARTUPINFOA si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));
  /* "detach" (-b) is not processed for Windows, so 
     use DETACHED_PROCESS in CreateProcessA to not create
     a TCL shell
  */
  if (!cell || !cell[0]) { 
    if (!symbol) 
      my_snprintf(cmd_line, S(cmd_line), "%s -b -s --tcl \"set XSCHEM_START_WINDOW {}\"", xschem_executable);
    else
      my_snprintf(cmd_line, S(cmd_line), "%s -b -y --tcl \"set XSCHEM_START_WINDOW {}\"", xschem_executable);
  }
  else if (!symbol) {
    my_snprintf(cmd_line, S(cmd_line), "%s -b -s \"%s\"", xschem_executable, cell);
  }
  else {
    my_snprintf(cmd_line, S(cmd_line), "%s -b -y \"%s\"", xschem_executable, cell);
  }

  CreateProcessA
  (
    NULL,               /* the path */
    cmd_line,           /* Command line */
    NULL,               /* Process handle not inheritable */
    NULL,               /* Thread handle not inheritable */
    FALSE,              /* Set handle inheritance to FALSE */
    DETACHED_PROCESS,   /* Opens file in a separate console */
    NULL,               /* Use parent's environment block */
    NULL,               /* Use parent's starting directory */
    &si,                /* Pointer to STARTUPINFO structure */
    &pi                 /* Pointer to PROCESS_INFORMATION structure */
  );
}
#endif
const char *get_file_path(char *f)
{
  char tmp[2*PATH_MAX+100];
  my_snprintf(tmp, S(tmp),"get_file_path {%s}", f);
  tcleval(tmp);
  return tclresult();
}

/* return value:
 *  1 : file saved or not needed to save since no change
 * -1 : user cancel
 *  0 : file not saved due to errors or per user request
 */
int save(int confirm)
{
     if(xctx->modified)
     {
       if(confirm) {
         tcleval("ask_save");
         if(!strcmp(tclresult(), "") ) return -1; /* user clicks "Cancel" */
         else if(!strcmp(tclresult(), "yes") ) return save_schematic(xctx->sch[xctx->currsch]);
         else return 0; /* user clicks "no" */
       } else {
         return save_schematic(xctx->sch[xctx->currsch]);
       }
     }
     return 1; /* circuit not changed: always succeeed */
}

void saveas(const char *f, int type) /*  changed name from ask_save_file to saveas 20121201 */
{
    char name[PATH_MAX+1000];
    char filename[PATH_MAX];
    char res[PATH_MAX];
    char *p;
    if(!f && has_x) {
      my_strncpy(filename , xctx->sch[xctx->currsch], S(filename));
      if(type == SYMBOL) {
        if( (p = strrchr(filename, '.')) && !strcmp(p, ".sch") ) {
          my_strncpy(filename, add_ext(filename, ".sym"), S(filename));
        }
        my_snprintf(name, S(name), "save_file_dialog {Save file} *.\\{sch,sym\\} INITIALLOADDIR {%s}", filename);
      } else {
        my_snprintf(name, S(name), "save_file_dialog {Save file} *.\\{sch,sym\\} INITIALLOADDIR {%s}", filename);
      }

      tcleval(name);
      my_strncpy(res, tclresult(), S(res));
    }
    else if(f) {
      my_strncpy(res, f, S(res));
    }
    else res[0]='\0';

    if(!res[0]) return;
    dbg(1, "saveas(): res = %s\n", res);
    save_schematic(res);
    tclvareval("update_recent_file {", res,"}",  NULL);

    my_strncpy(xctx->current_name, rel_sym_path(res), S(xctx->current_name));
    return;
}

void ask_new_file(void)
{
    char f[PATH_MAX]; /*  overflow safe 20161125 */

    if(!has_x) return;
    if(xctx->modified) {
      if(save(1) == -1 ) return; /*  user cancels save, so do nothing. */
    }
    tcleval("load_file_dialog {Load file} *.\\{sch,sym\\} INITIALLOADDIR");
    my_snprintf(f, S(f),"%s", tclresult());
    if(f[0]) {
      char win_path[WINDOW_PATH_SIZE];
      int skip = 0;
      dbg(1, "ask_new_file(): load: f=%s\n", f);
 
      if(check_loaded(f, win_path)) {
        char msg[PATH_MAX + 100];
        my_snprintf(msg, S(msg),
           "tk_messageBox -type okcancel -icon warning -parent [xschem get topwindow] "
           "-message {Warning: %s already open.}", f);
        if(has_x) {
          tcleval(msg);
          if(strcmp(tclresult(), "ok")) skip = 1;
        }
        else dbg(0, "ask_new_file: %s already open: %s\n", f, win_path);
      }
      if(!skip) {
        dbg(1, "ask_new_file(): load file: %s\n", f);
        clear_all_hilights();
        xctx->currsch = 0;
        unselect_all(1);
        remove_symbols();
        load_schematic(1, f,1); /* 20180925.1 */
        tclvareval("update_recent_file {", f, "}", NULL);
        my_strdup(_ALLOC_ID_, &xctx->sch_path[xctx->currsch],".");
        xctx->sch_path_hash[xctx->currsch] = 0;
        xctx->sch_inst_number[xctx->currsch] = 1;
        zoom_full(1, 0, 1, 0.97);
      }
    }
}

/* remove symbol and decrement symbols */
/* Warning: removing a symbol with a loaded schematic will make all symbol references corrupt */
/* you should clear_drawing() first or load_schematic() or link_symbols_to_instances()
   immediately afterwards */
void remove_symbol(int j)
{
  int i,c;
  xSymbol save;

  dbg(1,"clearing symbol %s\n", xctx->sym[j].name);
  my_free(_ALLOC_ID_, &xctx->sym[j].prop_ptr);
  my_free(_ALLOC_ID_, &xctx->sym[j].templ);
  my_free(_ALLOC_ID_, &xctx->sym[j].type);
  my_free(_ALLOC_ID_, &xctx->sym[j].name);
  /*  /20150409 */
  for(c=0;c<cadlayers; ++c) {
    for(i=0;i<xctx->sym[j].polygons[c]; ++i) {
      if(xctx->sym[j].poly[c][i].prop_ptr != NULL) {
        my_free(_ALLOC_ID_, &xctx->sym[j].poly[c][i].prop_ptr);
      }
      my_free(_ALLOC_ID_, &xctx->sym[j].poly[c][i].x);
      my_free(_ALLOC_ID_, &xctx->sym[j].poly[c][i].y);
      my_free(_ALLOC_ID_, &xctx->sym[j].poly[c][i].selected_point);
    }
    my_free(_ALLOC_ID_, &xctx->sym[j].poly[c]);
    xctx->sym[j].polygons[c] = 0;
 
    for(i=0;i<xctx->sym[j].lines[c]; ++i) {
      if(xctx->sym[j].line[c][i].prop_ptr != NULL) {
        my_free(_ALLOC_ID_, &xctx->sym[j].line[c][i].prop_ptr);
      }
    }
    my_free(_ALLOC_ID_, &xctx->sym[j].line[c]);
    xctx->sym[j].lines[c] = 0;
 
    for(i=0;i<xctx->sym[j].arcs[c]; ++i) {
      if(xctx->sym[j].arc[c][i].prop_ptr != NULL) {
        my_free(_ALLOC_ID_, &xctx->sym[j].arc[c][i].prop_ptr);
      }
    }
    my_free(_ALLOC_ID_, &xctx->sym[j].arc[c]);
    xctx->sym[j].arcs[c] = 0;
 
    for(i=0;i<xctx->sym[j].rects[c]; ++i) {
      if(xctx->sym[j].rect[c][i].prop_ptr != NULL) {
        my_free(_ALLOC_ID_, &xctx->sym[j].rect[c][i].prop_ptr);
      }
      set_rect_extraptr(0, &xctx->sym[j].rect[c][i]);
    }
    my_free(_ALLOC_ID_, &xctx->sym[j].rect[c]);
    xctx->sym[j].rects[c] = 0;
  }
  for(i=0;i<xctx->sym[j].texts; ++i) {
    if(xctx->sym[j].text[i].prop_ptr != NULL) {
      my_free(_ALLOC_ID_, &xctx->sym[j].text[i].prop_ptr);
    }
    if(xctx->sym[j].text[i].txt_ptr != NULL) {
      my_free(_ALLOC_ID_, &xctx->sym[j].text[i].txt_ptr);
    }
    if(xctx->sym[j].text[i].font != NULL) {
      my_free(_ALLOC_ID_, &xctx->sym[j].text[i].font);
    }
  }
  my_free(_ALLOC_ID_, &xctx->sym[j].text);
  xctx->sym[j].texts = 0;

  save = xctx->sym[j]; /* save cleared symbol slot */
  for(i = j + 1; i < xctx->symbols; ++i) {
    xctx->sym[i-1] = xctx->sym[i];
  }
  xctx->sym[xctx->symbols-1] = save; /* fill end with cleared symbol slot */
  xctx->symbols--;
}

void remove_symbols(void)
{
  int j;

  for(j = 0; j < xctx->instances; ++j) {
    delete_inst_node(j); /* must be deleted before symbols are deleted */
    xctx->inst[j].ptr = -1; /* clear symbol reference on instanecs */
  }
  for(j=xctx->symbols-1;j>=0;j--) {
    dbg(2, "remove_symbols(): removing symbol %d\n",j);
    remove_symbol(j);
  }
  dbg(1, "remove_symbols(): done\n");
}


/* set cached rect .flags bitmask based on attributes, currently:
 * graph              1
 * graph_unlocked     1 + 2
 * image           1024
 * image_unscaled  1024 + 2048
 */
int set_rect_flags(xRect *r)
{
  const char *flags;
  unsigned short f = 0;
  if(r->prop_ptr && r->prop_ptr[0]) {
    flags = get_tok_value(r->prop_ptr,"flags",0);
    if(strstr(flags, "unscaled")) f |= 3072;
    else if(strstr(flags, "image")) f |= 1024;
    else if(strstr(flags, "unlocked")) f |= 3;
    else if(strstr(flags, "graph")) f |= 1;
  }
  r->flags = f;
  dbg(1, "set_rect_flags(): flags=%d\n", f);
  return f;
}

int set_text_flags(xText *t)
{
  const char *str;
  t->flags = 0;
  t->hcenter = 0;
  t->vcenter = 0;
  t->layer = -1;
  if(t->prop_ptr) {
    my_strdup(_ALLOC_ID_, &t->font, get_tok_value(t->prop_ptr, "font", 0));
    str = get_tok_value(t->prop_ptr, "hcenter", 0);
    t->hcenter = strcmp(str, "true")  ? 0 : 1;
    str = get_tok_value(t->prop_ptr, "vcenter", 0);
    t->vcenter = strcmp(str, "true")  ? 0 : 1;
    str = get_tok_value(t->prop_ptr, "layer", 0);
    if(str[0]) t->layer = atoi(str);
    str = get_tok_value(t->prop_ptr, "slant", 0);
    t->flags |= strcmp(str, "oblique")  ? 0 : TEXT_OBLIQUE;
    t->flags |= strcmp(str, "italic")  ? 0 : TEXT_ITALIC;
    str = get_tok_value(t->prop_ptr, "weight", 0);
    t->flags |= strcmp(str, "bold")  ? 0 : TEXT_BOLD;
    str = get_tok_value(t->prop_ptr, "hide", 0);
    t->flags |= strcmp(str, "true")  ? 0 : HIDE_TEXT;
  }
  return 0;
}

/* what: 
 * 1: create
 * 0: clear
 */
int set_rect_extraptr(int what, xRect *drptr)
{
  #if HAS_CAIRO==1
  if(what==1) { /* create */
    if(drptr->flags & 1024) { /* embedded image */
      if(!drptr->extraptr) {
        xEmb_image *d;
        d = my_malloc(_ALLOC_ID_, sizeof(xEmb_image));
        d->image = NULL;
        drptr->extraptr = d;
      }
    }
  } else {   /* clear */
    if(drptr->flags & 1024) { /* embedded image */
      if(drptr->extraptr) {
        xEmb_image *d = drptr->extraptr;
        if(d->image) cairo_surface_destroy(d->image);
        my_free(_ALLOC_ID_, &drptr->extraptr);
      }
    }
  }
  #endif
  return 0;
}

void clear_drawing(void)
{
 int i,j;
 xctx->graph_lastsel = -1;
 del_inst_table();
 del_wire_table();
 my_free(_ALLOC_ID_, &xctx->schtedaxprop);
 my_free(_ALLOC_ID_, &xctx->schsymbolprop);
 my_free(_ALLOC_ID_, &xctx->schprop);
 my_free(_ALLOC_ID_, &xctx->schvhdlprop);
 my_free(_ALLOC_ID_, &xctx->version_string);
 if(xctx->header_text) my_free(_ALLOC_ID_, &xctx->header_text);
 my_free(_ALLOC_ID_, &xctx->schverilogprop);
 for(i=0;i<xctx->wires; ++i)
 {
  my_free(_ALLOC_ID_, &xctx->wire[i].prop_ptr);
  my_free(_ALLOC_ID_, &xctx->wire[i].node);
 }
 xctx->wires = 0;
 for(i=0;i<xctx->instances; ++i)
 {
  my_free(_ALLOC_ID_, &xctx->inst[i].prop_ptr);
  my_free(_ALLOC_ID_, &xctx->inst[i].name);
  my_free(_ALLOC_ID_, &xctx->inst[i].instname);
  my_free(_ALLOC_ID_, &xctx->inst[i].lab);
  delete_inst_node(i);
 }
 xctx->instances = 0;
 for(i=0;i<xctx->texts; ++i)
 {
  my_free(_ALLOC_ID_, &xctx->text[i].font);
  my_free(_ALLOC_ID_, &xctx->text[i].prop_ptr);
  my_free(_ALLOC_ID_, &xctx->text[i].txt_ptr);
 }
 xctx->texts = 0;
 for(i=0;i<cadlayers; ++i)
 {
  for(j=0;j<xctx->lines[i]; ++j)
  {
   my_free(_ALLOC_ID_, &xctx->line[i][j].prop_ptr);
  }
  for(j=0;j<xctx->rects[i]; ++j)
  {
   my_free(_ALLOC_ID_, &xctx->rect[i][j].prop_ptr);
   set_rect_extraptr(0, &xctx->rect[i][j]);
  }
  for(j=0;j<xctx->arcs[i]; ++j)
  {
   my_free(_ALLOC_ID_, &xctx->arc[i][j].prop_ptr);
  }
  for(j=0;j<xctx->polygons[i]; ++j) {
    my_free(_ALLOC_ID_, &xctx->poly[i][j].x);
    my_free(_ALLOC_ID_, &xctx->poly[i][j].y);
    my_free(_ALLOC_ID_, &xctx->poly[i][j].prop_ptr);
    my_free(_ALLOC_ID_, &xctx->poly[i][j].selected_point);
  }
  xctx->lines[i] = 0;
  xctx->arcs[i] = 0;
  xctx->rects[i] = 0;
  xctx->polygons[i] = 0;
 }
 dbg(1, "clear drawing(): deleted data structures, now deleting hash\n");
 int_hash_free(&xctx->inst_table);
}

/*  xctx->n_active_layers is the total number of layers for hilights. */
void enable_layers(void)
{
  int i;
  char tmp[50];
  const char *en;
  xctx->n_active_layers = 0;
  for(i = 0; i< cadlayers; ++i) {
    my_snprintf(tmp, S(tmp), "enable_layer(%d)",i);
    en = tclgetvar(tmp);
    if(!en || en[0] == '0') xctx->enable_layer[i] = 0;
    else {
      xctx->enable_layer[i] = 1;
      if(i>=7) {
        xctx->active_layer[xctx->n_active_layers] = i;
        xctx->n_active_layers++;
      }
    }
  }
}

short connect_by_kissing(void)
{
  xSymbol *symbol;
  int npin, i, j;
  double x0,y0, pinx0, piny0;
  short flip, rot;
  xRect *rct;
  short kissing, changed = 0;
  int k, ii, done_undo = 0;
  Wireentry *wptr;
  Instpinentry *iptr;
  int sqx, sqy;

  rebuild_selected_array();
  k = xctx->lastsel;
  prepare_netlist_structs(0);
  for(j=0;j<k; ++j) if(xctx->sel_array[j].type==ELEMENT) {
    x0 = xctx->inst[xctx->sel_array[j].n].x0;
    y0 = xctx->inst[xctx->sel_array[j].n].y0;
    rot = xctx->inst[xctx->sel_array[j].n].rot;
    flip = xctx->inst[xctx->sel_array[j].n].flip;
    symbol = xctx->sym + xctx->inst[xctx->sel_array[j].n].ptr;
    npin = symbol->rects[PINLAYER];
    rct=symbol->rect[PINLAYER];
    for(i=0;i<npin; ++i) {
      pinx0 = (rct[i].x1+rct[i].x2)/2;
      piny0 = (rct[i].y1+rct[i].y2)/2;
      ROTATION(rot, flip, 0.0, 0.0, pinx0, piny0, pinx0, piny0);
      pinx0 += x0;
      piny0 += y0;
      get_square(pinx0, piny0, &sqx, &sqy);
      iptr=xctx->instpin_spatial_table[sqx][sqy];
      wptr=xctx->wire_spatial_table[sqx][sqy];
      kissing=0;
      while(iptr) {
        ii = iptr->n;
        if(ii == xctx->sel_array[j].n) {
          iptr = iptr->next;
          continue;
        }
        if( iptr->x0 == pinx0 && iptr->y0 == piny0  &&  xctx->inst[ii].sel == 0) {
          kissing=1;
          break;
        }
        iptr = iptr->next;
      }
      while(wptr) {
        xWire *w = &xctx->wire[wptr->n];
        if( touch(w->x1, w->y1, w->x2, w->y2, pinx0, piny0)) {
          if( w->sel) {
            kissing=0;
            break;
          }
          else if( (pinx0 != w->x1 || piny0 != w->y1) && (pinx0 != w->x2 || piny0 != w->y2)) {
            kissing = 1;
            break;
          }
        }
        wptr = wptr->next;
      }
      if(kissing) {
     
        dbg(1, "connect_by_kissing(): adding wire in %g %g, wires before = %d\n", pinx0, piny0, xctx->wires);
        if(!done_undo) {
          xctx->push_undo();
          done_undo = 1;
        }
        storeobject(-1, pinx0, piny0,  pinx0, piny0, WIRE, 0, SELECTED1, NULL);
        /* storeobject sets modify, but we clear it here, will be set if move operation completes */
        set_modify(0);
        changed = 1;
        xctx->need_reb_sel_arr = 1;
      }
    }
  }
  rebuild_selected_array();
  return changed;
}

void attach_labels_to_inst(int interactive) /*  offloaded from callback.c 20171005 */
{
  xSymbol *symbol;
  int npin, i, j;
  double x0,y0, pinx0, piny0;
  short flip, rot, rot1 ;
  xRect *rct;
  char *labname=NULL;
  char *prop=NULL; /*  20161122 overflow safe */
  char *symname_pin = NULL;
  char *symname_wire = NULL;
  char *type=NULL;
  short dir;
  int k,ii, skip;
  int do_all_inst=0;
  const char *rot_txt;
  int rotated_text=-1;

  Wireentry *wptr;
  Instpinentry *iptr;
  int sqx, sqy;
  int first_call;
  int use_label_prefix;
  int found=0;

  my_strdup(_ALLOC_ID_, &symname_pin, tcleval("rel_sym_path [find_file lab_pin.sym]"));
  my_strdup(_ALLOC_ID_, &symname_wire, tcleval("rel_sym_path [find_file lab_wire.sym]"));
  if(symname_pin && symname_wire) {
    rebuild_selected_array();
    k = xctx->lastsel;
    first_call=1; /*  20171214 for place_symbol--> new_prop_string */
    prepare_netlist_structs(0);
    for(j=0;j<k; ++j) if(xctx->sel_array[j].type==ELEMENT) {
      found=1;
      my_strdup(_ALLOC_ID_, &prop, xctx->inst[xctx->sel_array[j].n].instname);
      my_strcat(_ALLOC_ID_, &prop, "_");
      tclsetvar("custom_label_prefix",prop);
  
      if(interactive && !do_all_inst) {
        dbg(1,"attach_labels_to_inst(): invoking tcl attach_labels_to_inst\n");
        tcleval("attach_labels_to_inst");
        if(!strcmp(tclgetvar("rcode"),"") ) {
          bbox(END, 0., 0., 0., 0.);
          my_free(_ALLOC_ID_, &prop);
          return;
        }
      }
      if(interactive == 0 ) {
        tclsetvar("rcode", "yes");
        tclsetvar("use_lab_wire", "0");
        tclsetvar("use_label_prefix", "0");
        tclsetvar("do_all_inst", "1");
        tclsetvar("rotated_text", "0");
      }
      use_label_prefix = atoi(tclgetvar("use_label_prefix"));
      rot_txt = tclgetvar("rotated_text");
      if(strcmp(rot_txt,"")) rotated_text=atoi(rot_txt);
      my_strdup(_ALLOC_ID_, &type,(xctx->inst[xctx->sel_array[j].n].ptr+ xctx->sym)->type);
      if( type && IS_LABEL_OR_PIN(type) ) {
        continue;
      }
      if(!do_all_inst && !strcmp(tclgetvar("do_all_inst"),"1")) do_all_inst=1;
      dbg(1, "attach_labels_to_inst(): 1--> %s %.16g %.16g   %s\n",
          xctx->inst[xctx->sel_array[j].n].name,
          xctx->inst[xctx->sel_array[j].n].x0,
          xctx->inst[xctx->sel_array[j].n].y0,
          xctx->sym[xctx->inst[xctx->sel_array[j].n].ptr].name);
  
      x0 = xctx->inst[xctx->sel_array[j].n].x0;
      y0 = xctx->inst[xctx->sel_array[j].n].y0;
      rot = xctx->inst[xctx->sel_array[j].n].rot;
      flip = xctx->inst[xctx->sel_array[j].n].flip;
      symbol = xctx->sym + xctx->inst[xctx->sel_array[j].n].ptr;
      npin = symbol->rects[PINLAYER];
      rct=symbol->rect[PINLAYER];
  
      for(i=0;i<npin; ++i) {
         my_strdup(_ALLOC_ID_, &labname,get_tok_value(rct[i].prop_ptr,"name",1));
         dbg(1,"attach_labels_to_inst(): 2 --> labname=%s\n", labname);
  
         pinx0 = (rct[i].x1+rct[i].x2)/2;
         piny0 = (rct[i].y1+rct[i].y2)/2;
  
         if(strcmp(get_tok_value(rct[i].prop_ptr,"dir",0),"in")) dir=1; /*  out or inout pin */
         else dir=0; /*  input pin */
  
         /*  opin or iopin on left of symbol--> reverse orientation 20171205 */
         if(rotated_text ==-1 && dir==1 && pinx0<0) dir=0;
  
         ROTATION(rot, flip, 0.0, 0.0, pinx0, piny0, pinx0, piny0);
  
         pinx0 += x0;
         piny0 += y0;
  
         get_square(pinx0, piny0, &sqx, &sqy);
         iptr=xctx->instpin_spatial_table[sqx][sqy];
         wptr=xctx->wire_spatial_table[sqx][sqy];
  
         skip=0;
         while(iptr) {
           ii = iptr->n;
           if(ii == xctx->sel_array[j].n) {
             iptr = iptr->next;
             continue;
           }
  
           if( iptr->x0 == pinx0 && iptr->y0 == piny0 ) {
             skip=1;
             break;
           }
           iptr = iptr->next;
         }
         while(wptr) {
           if( touch(xctx->wire[wptr->n].x1, xctx->wire[wptr->n].y1,
               xctx->wire[wptr->n].x2, xctx->wire[wptr->n].y2, pinx0, piny0) ) {
             skip=1;
             break;
           }
           wptr = wptr->next;
         }
         if(!skip) {
           my_strdup(_ALLOC_ID_, &prop, "name=p1 lab=");
           if(use_label_prefix) {
             my_strcat(_ALLOC_ID_, &prop, (char *)tclgetvar("custom_label_prefix"));
           }
           /*  /20171005 */
  
           my_strcat(_ALLOC_ID_, &prop, labname);
           dir ^= flip; /*  20101129  20111030 */
           if(rotated_text ==-1) {
             rot1=rot;
             if(rot1==1 || rot1==2) { dir=!dir;rot1 = (short)((rot1+2) %4);}
           } else {
             rot1=(short)((rot+rotated_text)%4); /*  20111103 20171208 text_rotation */
           }
           if(!strcmp(tclgetvar("use_lab_wire"),"0")) {
             place_symbol(-1,symname_pin, pinx0, piny0, rot1, dir, prop, 2, first_call, 1/*to_push_undo*/);
             first_call=0;
           } else {
             place_symbol(-1,symname_wire, pinx0, piny0, rot1, dir, prop, 2, first_call, 1/*to_push_undo*/);
             first_call=0;
           }
         }
         dbg(1, "attach_labels_to_inst(): %d   %.16g %.16g %s\n", i, pinx0, piny0,labname);
      }
    }
    my_free(_ALLOC_ID_, &prop);
    my_free(_ALLOC_ID_, &labname);
    my_free(_ALLOC_ID_, &type);
    if(!found) return;
    /*  draw things  */
    bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
    draw();
    bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  } else {
    fprintf(errfp, "attach_labels_to_inst(): location of schematic labels not found\n");
    tcleval("alert_ {attach_labels_to_inst(): location of schematic labels not found} {}");
  }
  my_free(_ALLOC_ID_, &symname_pin);
  my_free(_ALLOC_ID_, &symname_wire);
}

void delete_files(void)
{
  char str[PATH_MAX + 100];
  rebuild_selected_array();
  if(xctx->lastsel && xctx->sel_array[0].type==ELEMENT) {
    my_snprintf(str, S(str), "delete_files {%s}",
         abs_sym_path(xctx->inst[xctx->sel_array[0].n].name, ""));
  } else {
    my_snprintf(str, S(str), "delete_files {%s}",
         abs_sym_path(xctx->sch[xctx->currsch], ""));
  }
  tcleval(str);
}

void place_net_label(int type)
{
  if(type == 1) {
      const char *lab = tcleval("rel_sym_path [find_file lab_pin.sym]");
      place_symbol(-1, lab, xctx->mousex_snap, xctx->mousey_snap, 0, 0, NULL, 4, 1, 1/*to_push_undo*/);
  } else {
      const char *lab = tcleval("rel_sym_path [find_file lab_wire.sym]");
      place_symbol(-1, lab, xctx->mousex_snap, xctx->mousey_snap, 0, 0, NULL, 4, 1, 1/*to_push_undo*/);
  }
  move_objects(START,0,0,0);
  xctx->ui_state |= START_SYMPIN;
}

/*  draw_sym==4 select element after placing */
/*  draw_sym==2 begin bbox if(first_call), add bbox */
/*  draw_sym==1 begin bbox if(first_call), add bbox, end bbox, draw placed symbols  */
/*  */
/*  first_call: set to 1 on first invocation for a given set of symbols (same prefix) */
/*  set to 0 on next calls, this speeds up searching for unique names in prop string */
/*  returns 1 if symbol successfully placed, 0 otherwise */
int place_symbol(int pos, const char *symbol_name, double x, double y, short rot, short flip,
                   const char *inst_props, int draw_sym, int first_call, int to_push_undo)
/*  if symbol_name is a valid string load specified cell and */
/*  use the given params, otherwise query user */
{
 int i,j,n;
 char name[PATH_MAX];
 char *type;
 int cond;
 if(symbol_name==NULL) {
   tcleval("load_file_dialog {Choose symbol} *.sym INITIALINSTDIR");
   my_strncpy(name, tclresult(), S(name));
 } else {
   my_strncpy(name, symbol_name, S(name));
 }
 dbg(1, "place_symbol(): load_file_dialog returns:  name=%s\n",name);
 my_strncpy(name, rel_sym_path(name), S(name));
 if(name[0]) {
   if(first_call && to_push_undo) xctx->push_undo();
 } else  return 0;
 i=match_symbol(name);

 if(i!=-1)
 {
  check_inst_storage();
  if(pos==-1 || pos > xctx->instances) n=xctx->instances;
  else
  {
   xctx->prep_hash_inst = 0; /* instances moved so need to rebuild hash */
   for(j=xctx->instances;j>pos;j--)
   {
    xctx->inst[j]=xctx->inst[j-1];
   }
   n=pos;
  }
  /*  03-02-2000 */
  dbg(1, "place_symbol(): checked inst_ptr storage, sym number i=%d\n", i);
  xctx->inst[n].ptr = i;
  xctx->inst[n].name=NULL;
  xctx->inst[n].instname=NULL;
  xctx->inst[n].lab=NULL;
  dbg(1, "place_symbol(): entering my_strdup: name=%s\n",name);  /*  03-02-2000 */
  my_strdup2(_ALLOC_ID_, &xctx->inst[n].name ,name);
  dbg(1, "place_symbol(): done my_strdup: name=%s\n",name);  /*  03-02-2000 */
  /*  xctx->inst[n].x0=symbol_name ? x : xctx->mousex_snap; */
  /*  xctx->inst[n].y0=symbol_name ? y : xctx->mousey_snap; */
  xctx->inst[n].x0= x ; /*  20070228 x and y given in callback */
  xctx->inst[n].y0= y ;
  xctx->inst[n].rot=symbol_name ? rot : 0;
  xctx->inst[n].flip=symbol_name ? flip : 0;

  xctx->inst[n].flags=0;
  xctx->inst[n].color=-10000; /* small negative values used for simulation */
  xctx->inst[n].sel=0;
  xctx->inst[n].node=NULL;
  xctx->inst[n].prop_ptr=NULL;
  dbg(1, "place_symbol() :all inst_ptr members set\n");  /*  03-02-2000 */
  if(first_call) hash_all_names();
  if(inst_props) {
    new_prop_string(n, inst_props,!first_call, tclgetboolvar("disable_unique_names")); /*  20171214 first_call */
  }
  else {
    set_inst_prop(n); /* no props, get from sym template, also calls new_prop_string() */
  }
  dbg(1, "place_symbol(): done set_inst_prop()\n");  /*  03-02-2000 */

  if(!strcmp(get_tok_value(xctx->inst[n].prop_ptr,"hide",0), "true"))
    xctx->inst[n].flags |= HIDE_INST;
  if(!strcmp(get_tok_value(xctx->inst[n].prop_ptr,"highlight",0), "true"))
    xctx->inst[n].flags |= HILIGHT_CONN;
  type = xctx->sym[xctx->inst[n].ptr].type;
  cond= !type || !IS_LABEL_SH_OR_PIN(type);
  if(cond) xctx->inst[n].flags|=2;
  else my_strdup(_ALLOC_ID_, &xctx->inst[n].lab, get_tok_value(xctx->inst[n].prop_ptr,"lab",0));
  xctx->inst[n].embed = !strcmp(get_tok_value(xctx->inst[n].prop_ptr, "embed", 2), "true");
  if(first_call && (draw_sym & 3) ) bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
  xctx->instances++; /* must be updated before calling symbol_bbox() */
  /* force these vars to 0 to trigger a prepare_netlist_structs(0) needed by symbol_bbox->translate
   * to translate @#n:net_name texts */
  xctx->prep_net_structs=0;
  xctx->prep_hi_structs=0;
  symbol_bbox(n, &xctx->inst[n].x1, &xctx->inst[n].y1,
                    &xctx->inst[n].x2, &xctx->inst[n].y2);
  if(xctx->prep_hash_inst) hash_inst(XINSERT, n); /* no need to rehash, add item */
  /* xctx->prep_hash_inst=0; */

  if(draw_sym & 3) {
    bbox(ADD, xctx->inst[n].x1, xctx->inst[n].y1, xctx->inst[n].x2, xctx->inst[n].y2);
  }
  /* set_modify(1); */
  if(draw_sym&1) {
    bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
    draw();
    bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  }
  /*   hilight new element 24122002 */

  if(draw_sym & 4 ) {
    select_element(n, SELECTED,0, 1);
    drawtemparc(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0, 0.0);
    drawtemprect(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
    drawtempline(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
    xctx->need_reb_sel_arr = 1;
    rebuild_selected_array(); /* sets  xctx->ui_state |= SELECTION; */
  }

 }
 return 1;
}

void symbol_in_new_window(int new_process)
{
  char filename[PATH_MAX];
  char win_path[WINDOW_PATH_SIZE];
  rebuild_selected_array();
  
  if(xctx->lastsel !=1 || xctx->sel_array[0].type!=ELEMENT) {
    my_strncpy(filename,  xctx->sch[xctx->currsch], S(filename));
    if(new_process) new_xschem_process(filename, 1);
    else new_schematic("create", NULL, filename);
  }
  else {
    my_strncpy(filename, abs_sym_path(xctx->inst[xctx->sel_array[0].n].name, ""), S(filename));
    if(!check_loaded(filename, win_path)) {
      if(new_process) new_xschem_process(filename, 1);
      else new_schematic("create", NULL, filename);
    }
  }
}

 /*  20111007 duplicate current schematic if no inst selected */
void schematic_in_new_window(int new_process)
{
  char filename[PATH_MAX];
  char win_path[WINDOW_PATH_SIZE];
  rebuild_selected_array();
  if(xctx->lastsel !=1 || xctx->sel_array[0].type!=ELEMENT) {
    if(new_process) new_xschem_process(xctx->sch[xctx->currsch], 0);
    else new_schematic("create", NULL, xctx->sch[xctx->currsch]);
  }
  else {
    if(                   /*  do not descend if not subcircuit */
       (xctx->inst[xctx->sel_array[0].n].ptr+ xctx->sym)->type &&
       strcmp(
          (xctx->inst[xctx->sel_array[0].n].ptr+ xctx->sym)->type,
           "subcircuit"
       ) &&
       strcmp(
          (xctx->inst[xctx->sel_array[0].n].ptr+ xctx->sym)->type,
           "primitive"
       )
    ) return;
    get_sch_from_sym(filename, xctx->inst[xctx->sel_array[0].n].ptr+ xctx->sym);
    if(!check_loaded(filename, win_path)) {
      if(new_process) new_xschem_process(filename, 0);
      else new_schematic("create", NULL, filename);
    }
  }
}

void launcher(void)
{
  const char *url;
  char program[PATH_MAX];
  int n;
  rebuild_selected_array();
  if(xctx->lastsel ==1 && xctx->sel_array[0].type==ELEMENT)
  {
    double mx=xctx->mousex, my=xctx->mousey;
    select_object(mx,my,SELECTED, 0);
    tcleval("update; after 300");
    select_object(mx,my,0, 0);
    n=xctx->sel_array[0].n;
    my_strncpy(program, get_tok_value(xctx->inst[n].prop_ptr,"program",0), S(program)); /* handle backslashes */
    url = get_tok_value(xctx->inst[n].prop_ptr,"url",0); /* handle backslashes */
    dbg(1, "launcher(): url=%s\n", url);
    if(url[0] || (program[0])) { /* open url with appropriate program */
      tclvareval("launcher {", url, "} {", program, "}", NULL);
    } else {
      my_strncpy(program, get_tok_value(xctx->inst[n].prop_ptr,"tclcommand",0), S(program));
      if(program[0]) { /* execute tcl command */
        tcleval(program);
      }
    }
  }
}

void get_sch_from_sym(char *filename, xSymbol *sym)
{
  char *sch = NULL;
  char *str_tmp = NULL;
  char *ptr;
  int web_url = 0;

  /* get sch/sym name from parent schematic downloaded from web */
  if( strstr(xctx->current_dirname, "http://") == xctx->current_dirname ||
      strstr(xctx->current_dirname, "https://") == xctx->current_dirname) {
    web_url = 1;
  }
  my_strdup2(_ALLOC_ID_, &str_tmp,  get_tok_value(sym->prop_ptr, "schematic", 2));
  if(str_tmp[0]) {
    /* @symname in schematic attribute will be replaced with symbol name */
    if( (ptr = strstr(str_tmp, "@symname"))) {
      *ptr = '\0';
      my_strdup2(_ALLOC_ID_, &sch, str_tmp);
      my_strcat(_ALLOC_ID_, &sch, sym->name);
      ptr += 8;
      my_strcat(_ALLOC_ID_, &sch, ptr);
    } else {
      my_strdup2(_ALLOC_ID_, &sch, str_tmp);
    }
    dbg(1, "get_sch_from_sym(): sch=%s\n", sch);
    my_strdup2(_ALLOC_ID_, &sch, tcl_hook2(&sch));
    dbg(1, "get_sch_from_sym(): after tcl_hook2 sch=%s\n", sch);
    /* for schematics referenced from web symbols do not build absolute path */
    if(web_url) my_strncpy(filename, sch, PATH_MAX);
    else my_strncpy(filename, abs_sym_path(sch, ""), PATH_MAX);
    my_free(_ALLOC_ID_, &sch);
  } else {
    if(tclgetboolvar("search_schematic")) {
      /* for schematics referenced from web symbols do not build absolute path */
      if(web_url) my_strncpy(filename, add_ext(sym->name, ".sch"), PATH_MAX);
      else my_strncpy(filename, abs_sym_path(sym->name, ".sch"), PATH_MAX);
    } else {
      /* for schematics referenced from web symbols do not build absolute path */
      if(web_url) my_strncpy(filename, add_ext(sym->name, ".sch"), PATH_MAX);
      else my_strncpy(filename, add_ext(abs_sym_path(sym->name, ""), ".sch"), PATH_MAX);
    }
  }
  my_free(_ALLOC_ID_, &str_tmp);
  dbg(1, "get_sch_from_sym(): sym->name=%s, filename=%s\n", sym->name, filename);
}

int descend_schematic(int instnumber)
{
 const char *str;
 char filename[PATH_MAX];
 int inst_mult, inst_number;
 int save_ok = 0;
 int n = 0;

 rebuild_selected_array();
 if(xctx->lastsel !=1 || xctx->sel_array[0].type!=ELEMENT)
 {
  dbg(1, "descend_schematic(): wrong selection\n");
  return 0;
 }
 else
 {
  n = xctx->sel_array[0].n;
  dbg(1, "descend_schematic(): selected:%s\n", xctx->inst[n].name);
  /* no name set for current schematic: save it before descending*/
  if(!strcmp(xctx->sch[xctx->currsch],""))
  {
    char cmd[PATH_MAX+1000];
    char filename[PATH_MAX];
    char res[PATH_MAX];

    my_strncpy(filename, xctx->sch[xctx->currsch], S(filename));
    my_snprintf(cmd, S(cmd), "save_file_dialog {Save file} *.\\{sch,sym\\} INITIALLOADDIR {%s}", filename);
    tcleval(cmd);
    my_strncpy(res, tclresult(), S(res));
    if(!res[0]) return 0;
    dbg(1, "descend_schematic(): saving: %s\n",res);
    save_ok = save_schematic(res);
    if(save_ok==0) return 0;
  }

  dbg(1, "descend_schematic(): inst type: %s\n", (xctx->inst[n].ptr+ xctx->sym)->type);

  if(                   /*  do not descend if not subcircuit */
     (xctx->inst[n].ptr+ xctx->sym)->type &&
     strcmp( (xctx->inst[n].ptr+ xctx->sym)->type, "subcircuit") &&
     strcmp( (xctx->inst[n].ptr+ xctx->sym)->type, "primitive")
  ) return 0;

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
    if(ret == -1) return 0; /* user cancel */
  }

  /*  build up current hierarchy path */
  dbg(1, "descend_schematic(): selected instname=%s\n", xctx->inst[n].instname);


  if(xctx->inst[n].instname && xctx->inst[n].instname[0]) {
    str=expandlabel(xctx->inst[n].instname, &inst_mult);
  } else {
    str = "";
    inst_mult = 1;
  }
  my_strdup(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], xctx->sch_path[xctx->currsch]);
  xctx->sch_path_hash[xctx->currsch+1] =0;
  my_strdup(_ALLOC_ID_, &xctx->hier_attr[xctx->currsch].prop_ptr, 
            xctx->inst[n].prop_ptr);
  my_strdup(_ALLOC_ID_, &xctx->hier_attr[xctx->currsch].templ,
            get_tok_value((xctx->inst[n].ptr+ xctx->sym)->prop_ptr, "template", 0));
  inst_number = 1;
  if(inst_mult > 1) { /* on multiple instances ask where to descend, to correctly evaluate
                         the hierarchy path you descend to */

    if(instnumber == 0 ) {
      const char *inum;
      tclvareval("input_line ", "{input instance number (leftmost = 1) to descend into:\n"
        "negative numbers select instance starting\nfrom the right (rightmost = -1)}"
        " {} 1 6", NULL);
      inum = tclresult();
      dbg(1, "descend_schematic(): inum=%s\n", inum);
      if(!inum[0]) {
        my_free(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1]);
        xctx->sch_path_hash[xctx->currsch+1] =0;
        return 0;
      }
      inst_number=atoi(inum);
    } else {
      inst_number = instnumber;
    }
    if(inst_number < 0 ) inst_number += inst_mult+1;
    /* any invalid number->descend to leftmost inst */
    if(inst_number <1 || inst_number > inst_mult) inst_number = 1;
  }
  dbg(1,"descend_schematic(): inst_number=%d\n", inst_number);
  my_strcat(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], find_nth(str, ",", inst_number));
  dbg(1,"descend_schematic(): inst_number=%d\n", inst_number);
  my_strcat(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], ".");
  xctx->sch_inst_number[xctx->currsch+1] = inst_number;
  dbg(1, "descend_schematic(): current path: %s\n", xctx->sch_path[xctx->currsch+1]);
  dbg(1, "descend_schematic(): inst_number=%d\n", inst_number);

  xctx->previous_instance[xctx->currsch]=n;
  xctx->zoom_array[xctx->currsch].x=xctx->xorigin;
  xctx->zoom_array[xctx->currsch].y=xctx->yorigin;
  xctx->zoom_array[xctx->currsch].zoom=xctx->zoom;
  xctx->currsch++;
  hilight_child_pins();

  get_sch_from_sym(filename, xctx->inst[n].ptr+ xctx->sym);
  dbg(1, "descend_schematic(): filename=%s\n", filename);
  unselect_all(1);
  remove_symbols();
  /* we are descending from a parent schematic downloaded from the web */
  if( strstr(xctx->current_dirname, "http://") == xctx->current_dirname ||
      strstr(xctx->current_dirname, "https://") == xctx->current_dirname) {
    char sympath[PATH_MAX];
    /* download item into ${XSCHEM_TMP_DIR}/xschem_web */
    tclvareval("try_download_url {", xctx->current_dirname, "} {", filename, "}", NULL);
    /* build local file name of downloaded object and load it */
    my_snprintf(sympath, S(sympath), "%s/xschem_web/%s",  tclgetvar("XSCHEM_TMP_DIR"), get_cell_w_ext(filename, 0));
    load_schematic(1, sympath, 1);
  } else {
    load_schematic(1, filename, 1);
  }
  if(xctx->hilight_nets)
  {
    prepare_netlist_structs(0);
    propagate_hilights(1, 0, XINSERT_NOREPLACE);
  }
  dbg(1, "descend_schematic(): before zoom(): prep_hash_inst=%d\n", xctx->prep_hash_inst);
  zoom_full(1, 0, 1, 0.97);
 }
 return 1;
}

void go_back(int confirm) /*  20171006 add confirm */
{
 int save_ok;
 int from_embedded_sym;
 int save_modified;
 char filename[PATH_MAX];
 int prev_sch_type;

 save_ok=1;
 prev_sch_type = xctx->netlist_type; /* if CAD_SYMBOL_ATTRS do not hilight_parent_pins */
 if(xctx->currsch>0)
 {
  /* if current sym/schematic is changed ask save before going up */
  if(xctx->modified)
  {
    if(confirm) {
      tcleval("ask_save");
      if(!strcmp(tclresult(), "yes") ) save_ok = save_schematic(xctx->sch[xctx->currsch]);
      else if(!strcmp(tclresult(), "") ) return;
    } else {
      save_ok = save_schematic(xctx->sch[xctx->currsch]);
    }
  }
  if(save_ok==0) return;
  unselect_all(1);
  remove_symbols();
  from_embedded_sym=0;
  if(strstr(xctx->sch[xctx->currsch], ".xschem_embedded_")) {
    /* when returning after editing an embedded symbol
     * load immediately symbol definition before going back (.xschem_embedded... file will be lost)
     */
    load_sym_def(xctx->sch[xctx->currsch], NULL);
    from_embedded_sym=1;
  }
  my_strncpy(xctx->sch[xctx->currsch] , "", S(xctx->sch[xctx->currsch]));
  xctx->currsch--;
  save_modified = xctx->modified; /* we propagate modified flag (cleared by load_schematic */
                            /* by default) to parent schematic if going back from embedded symbol */

  my_strncpy(filename, xctx->sch[xctx->currsch], S(filename));
  load_schematic(1, filename, 1);
  if(from_embedded_sym) xctx->modified=save_modified; /* to force ask save embedded sym in parent schematic */

  if(xctx->hilight_nets) {
    if(prev_sch_type != CAD_SYMBOL_ATTRS) hilight_parent_pins(); 
    propagate_hilights(1, 1, XINSERT_NOREPLACE);
  }
  xctx->xorigin=xctx->zoom_array[xctx->currsch].x;
  xctx->yorigin=xctx->zoom_array[xctx->currsch].y;
  xctx->zoom=xctx->zoom_array[xctx->currsch].zoom;
  xctx->mooz=1/xctx->zoom;

  change_linewidth(-1.);
  draw();

  dbg(1, "go_back(): current path: %s\n", xctx->sch_path[xctx->currsch]);
 }
}

#ifndef __unix__
/* Source: https://www.tcl.tk/man/tcl8.7/TclCmd/glob.htm */
/* backslash character has a special meaning to glob command,
so glob patterns containing Windows style path separators need special care.*/
void change_to_unix_fn(char* fn)
{
  size_t len, i, ii;
  len = strlen(fn);
  ii = 0;
  for (i = 0; i < len; ++i) {
    if (fn[i]!='\\') fn[ii++] = fn[i];
    else { fn[ii++] = '/'; if (fn[i + 1] == '\\') ++i; }
  }
}
#endif

/* selected: 0 -> all, 1 -> selected, 2 -> hilighted */
void calc_drawing_bbox(xRect *boundbox, int selected)
{
 xRect rect;
 int c, i;
 int count=0;
 #if HAS_CAIRO==1
 int customfont;
 #endif

 boundbox->x1=-100;
 boundbox->x2=100;
 boundbox->y1=-100;
 boundbox->y2=100;
 if(selected != 2) for(c=0;c<cadlayers; ++c)
 {
  const char *tmp = tclgetvar("hide_empty_graphs");
  int hide_graphs =  (tmp && tmp[0] == '1') ? 1 : 0;
  int waves = (sch_waves_loaded() >= 0);

  for(i=0;i<xctx->lines[c]; ++i)
  {
   if(selected == 1 && !xctx->line[c][i].sel) continue;
   rect.x1=xctx->line[c][i].x1;
   rect.x2=xctx->line[c][i].x2;
   rect.y1=xctx->line[c][i].y1;
   rect.y2=xctx->line[c][i].y2;
   ++count;
   updatebbox(count,boundbox,&rect);
  }

  for(i=0;i<xctx->polygons[c]; ++i)
  {
    double x1=0., y1=0., x2=0., y2=0.;
    int k;
    if(selected == 1 && !xctx->poly[c][i].sel) continue;
    ++count;
    for(k=0; k<xctx->poly[c][i].points; ++k) {
      /* fprintf(errfp, "  poly: point %d: %.16g %.16g\n", k, pp[c][i].x[k], pp[c][i].y[k]); */
      if(k==0 || xctx->poly[c][i].x[k] < x1) x1 = xctx->poly[c][i].x[k];
      if(k==0 || xctx->poly[c][i].y[k] < y1) y1 = xctx->poly[c][i].y[k];
      if(k==0 || xctx->poly[c][i].x[k] > x2) x2 = xctx->poly[c][i].x[k];
      if(k==0 || xctx->poly[c][i].y[k] > y2) y2 = xctx->poly[c][i].y[k];
    }
    rect.x1=x1;rect.y1=y1;rect.x2=x2;rect.y2=y2;
    updatebbox(count,boundbox,&rect);
  }

  for(i=0;i<xctx->arcs[c]; ++i)
  {
    if(selected == 1 && !xctx->arc[c][i].sel) continue;
    arc_bbox(xctx->arc[c][i].x, xctx->arc[c][i].y, xctx->arc[c][i].r, xctx->arc[c][i].a, xctx->arc[c][i].b,
             &rect.x1, &rect.y1, &rect.x2, &rect.y2);
    ++count;
    updatebbox(count,boundbox,&rect);
  }

  for(i=0;i<xctx->rects[c]; ++i)
  {
   if(selected == 1 && !xctx->rect[c][i].sel) continue;
   /* skip graph objects if no datafile loaded */
   if(c == GRIDLAYER && xctx->rect[c][i].flags) {  
     if(hide_graphs && !waves) continue;
   }
   rect.x1=xctx->rect[c][i].x1;
   rect.x2=xctx->rect[c][i].x2;
   rect.y1=xctx->rect[c][i].y1;
   rect.y2=xctx->rect[c][i].y2;
   ++count;
   updatebbox(count,boundbox,&rect);
  }
 }
 if(selected == 2 && xctx->hilight_nets) prepare_netlist_structs(0);
 for(i=0;i<xctx->wires; ++i)
 {
   double ov, y1, y2;
   if(selected == 1 && !xctx->wire[i].sel) continue;
   if(selected == 2) {
   /* const char *str;
    * str = get_tok_value(xctx->wire[i].prop_ptr, "lab",0);
    * if(!str[0] || !bus_hilight_hash_lookup(str, 0,XLOOKUP)) continue;
    */
     if(!xctx->hilight_nets || !xctx->wire[i].node || 
       !xctx->wire[i].node[0] || !bus_hilight_hash_lookup(xctx->wire[i].node, 0,XLOOKUP)) continue;
   }
   if(xctx->wire[i].bus){
     ov = INT_BUS_WIDTH(xctx->lw)> cadhalfdotsize ? INT_BUS_WIDTH(xctx->lw) : CADHALFDOTSIZE;
     if(xctx->wire[i].y1 < xctx->wire[i].y2) { y1 = xctx->wire[i].y1-ov; y2 = xctx->wire[i].y2+ov; }
     else                        { y1 = xctx->wire[i].y1+ov; y2 = xctx->wire[i].y2-ov; }
   } else {
     ov = cadhalfdotsize;
     if(xctx->wire[i].y1 < xctx->wire[i].y2) { y1 = xctx->wire[i].y1-ov; y2 = xctx->wire[i].y2+ov; }
     else                        { y1 = xctx->wire[i].y1+ov; y2 = xctx->wire[i].y2-ov; }
   }
   rect.x1 = xctx->wire[i].x1-ov;
   rect.x2 = xctx->wire[i].x2+ov;
   rect.y1 = y1;
   rect.y2 = y2;
   ++count;
   updatebbox(count,boundbox,&rect);
 }
 if(has_x && selected != 2) for(i=0;i<xctx->texts; ++i)
 { 
   int no_of_lines; 
   double longest_line;
   if(selected == 1 && !xctx->text[i].sel) continue;
   #if HAS_CAIRO==1
   customfont = set_text_custom_font(&xctx->text[i]);
   #endif
   if(text_bbox(xctx->text[i].txt_ptr, xctx->text[i].xscale,
         xctx->text[i].yscale,xctx->text[i].rot, xctx->text[i].flip,
         xctx->text[i].hcenter, xctx->text[i].vcenter,
         xctx->text[i].x0, xctx->text[i].y0,
         &rect.x1,&rect.y1, &rect.x2,&rect.y2, &no_of_lines, &longest_line) ) {
     ++count;
     updatebbox(count,boundbox,&rect);
   }
   #if HAS_CAIRO==1
   if(customfont) {
     cairo_restore(xctx->cairo_ctx);
   }
   #endif
 }
 for(i=0;i<xctx->instances; ++i)
 {
  char *type;
  Hilight_hashentry *entry;

  if(selected == 1 && !xctx->inst[i].sel) continue;

  if(selected == 2) {
    int found;
    type = (xctx->inst[i].ptr+ xctx->sym)->type;
    found = 0;
    if( type && IS_LABEL_OR_PIN(type)) {
      entry=bus_hilight_hash_lookup(xctx->inst[i].lab, 0, XLOOKUP );
      if(entry) found = 1;
    }
    if(!found &&  xctx->inst[i].color != -10000 ) {
      found = 1;
    }
    if(!found) continue;
  }

  /* cpu hog 20171206 */
  /*  symbol_bbox(i, &xctx->inst[i].x1, &xctx->inst[i].y1, &xctx->inst[i].x2, &xctx->inst[i].y2); */
  rect.x1=xctx->inst[i].x1;
  rect.y1=xctx->inst[i].y1;
  rect.x2=xctx->inst[i].x2;
  rect.y2=xctx->inst[i].y2;
  ++count;
  updatebbox(count,boundbox,&rect);
 }
}

/* flags: bit0: invoke change_linewidth()/xsetLineattributes, bit1: centered zoom */
void zoom_full(int dr, int sel, int flags, double shrink)
{
  xRect boundbox;
  double yzoom;
  double bboxw, bboxh, schw, schh;

  if(flags & 1) {
    if(tclgetboolvar("change_lw")) {
      xctx->lw = 1.;
    }
    xctx->areax1 = -2*INT_WIDTH(xctx->lw);
    xctx->areay1 = -2*INT_WIDTH(xctx->lw);
    xctx->areax2 = xctx->xrect[0].width+2*INT_WIDTH(xctx->lw);
    xctx->areay2 = xctx->xrect[0].height+2*INT_WIDTH(xctx->lw);
    xctx->areaw = xctx->areax2-xctx->areax1;
    xctx->areah = xctx->areay2 - xctx->areay1;
  }
  calc_drawing_bbox(&boundbox, sel);
  dbg(1, "zoom_full: %s, %g %g  %g %g\n",
      xctx->current_win_path, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
  schw = xctx->areaw-4*INT_WIDTH(xctx->lw);
  schh = xctx->areah-4*INT_WIDTH(xctx->lw);
  bboxw = boundbox.x2-boundbox.x1;
  bboxh = boundbox.y2-boundbox.y1;
  xctx->zoom = bboxw / schw;
  yzoom = bboxh / schh;
  if(yzoom > xctx->zoom) xctx->zoom = yzoom;
  xctx->zoom /= shrink;
  /* we do this here since change_linewidth may not be called  if flags & 1 == 0*/
  cadhalfdotsize = CADHALFDOTSIZE +  0.04 * (tclgetdoublevar("cadsnap")-10);

  xctx->mooz = 1 / xctx->zoom;
  if(flags & 2) {
    xctx->xorigin = -boundbox.x1 + (xctx->zoom * schw - bboxw) / 2; /* centered */
    xctx->yorigin = -boundbox.y1 + (xctx->zoom * schh - bboxh) / 2; /* centered */
  } else {
    xctx->xorigin = -boundbox.x1 + (1 - shrink) / 2 * xctx->zoom * schw;
    xctx->yorigin = -boundbox.y1 + xctx->zoom * schh - bboxh - (1 - shrink) / 2 * xctx->zoom * schh;
  }
  dbg(1, "zoom_full(): dr=%d sel=%d flags=%d areaw=%d, areah=%d\n", sel, dr, flags, xctx->areaw, xctx->areah);
  if(flags & 1) change_linewidth(-1.);
  if(dr && has_x) {
    draw();
    redraw_w_a_l_r_p_rubbers();
  }
}

void view_zoom(double z)
{
  double factor;
  /*  int i; */
  factor = z!=0.0 ? z : CADZOOMSTEP;
  if(xctx->zoom<CADMINZOOM) return;
  xctx->zoom/= factor;
  xctx->mooz=1/xctx->zoom;
  xctx->xorigin=-xctx->mousex_snap+(xctx->mousex_snap+xctx->xorigin)/factor;
  xctx->yorigin=-xctx->mousey_snap+(xctx->mousey_snap+xctx->yorigin)/factor;
  change_linewidth(-1.);
  draw();
  redraw_w_a_l_r_p_rubbers();
}

void view_unzoom(double z)
{
  double factor;
  /*  int i; */
  factor = z!=0.0 ? z : CADZOOMSTEP;
  if(xctx->zoom>CADMAXZOOM) return;
  xctx->zoom*= factor;
  xctx->mooz=1/xctx->zoom;
  /* 20181022 make unzoom and zoom symmetric  */
  /* keeping the mouse pointer as the origin */
  if(tclgetboolvar("unzoom_nodrift")) {
    xctx->xorigin=-xctx->mousex_snap+(xctx->mousex_snap+xctx->xorigin)*factor;
    xctx->yorigin=-xctx->mousey_snap+(xctx->mousey_snap+xctx->yorigin)*factor;
  } else {
    xctx->xorigin=xctx->xorigin+xctx->areaw*xctx->zoom*(1-1/factor)/2;
    xctx->yorigin=xctx->yorigin+xctx->areah*xctx->zoom*(1-1/factor)/2;
  }
  change_linewidth(-1.);
  draw();
  redraw_w_a_l_r_p_rubbers();
}

void set_viewport_size(int w, int h, double lw)
{
    xctx->xrect[0].x = 0;
    xctx->xrect[0].y = 0;
    xctx->xrect[0].width = (unsigned short)w;
    xctx->xrect[0].height = (unsigned short)h;
    xctx->areax2 = w+2*INT_WIDTH(lw);
    xctx->areay2 = h+2*INT_WIDTH(lw);
    xctx->areax1 = -2*INT_WIDTH(lw);
    xctx->areay1 = -2*INT_WIDTH(lw);
    xctx->lw = lw;
    xctx->areaw = xctx->areax2-xctx->areax1;
    xctx->areah = xctx->areay2-xctx->areay1;
}

void save_restore_zoom(int save)
{
  static int savew, saveh; /* safe to keep even with multiple schematics */
  static double savexor, saveyor, savezoom, savelw; /* safe to keep even with multiple schematics */

  if(save) {
    savew = xctx->xrect[0].width;
    saveh = xctx->xrect[0].height;
    savelw = xctx->lw;
    savexor = xctx->xorigin;
    saveyor = xctx->yorigin;
    savezoom = xctx->zoom;
  } else {
    xctx->xrect[0].x = 0;
    xctx->xrect[0].y = 0;
    xctx->xrect[0].width = (unsigned short)savew;
    xctx->xrect[0].height = (unsigned short)saveh;
    xctx->areax2 = savew+2*INT_WIDTH(savelw);
    xctx->areay2 = saveh+2*INT_WIDTH(savelw);
    xctx->areax1 = -2*INT_WIDTH(savelw);
    xctx->areay1 = -2*INT_WIDTH(savelw);
    xctx->lw = savelw;
    xctx->areaw = xctx->areax2-xctx->areax1;
    xctx->areah = xctx->areay2-xctx->areay1;
    xctx->xorigin = savexor;
    xctx->yorigin = saveyor;
    xctx->zoom = savezoom;
    xctx->mooz = 1 / savezoom;
  }
}

void zoom_box(double x1, double y1, double x2, double y2, double factor)
{
  double yy1;
  if(factor == 0.) factor = 1.;
  RECTORDER(x1,y1,x2,y2);
  xctx->xorigin=-x1;xctx->yorigin=-y1;
  xctx->zoom=(x2-x1)/(xctx->areaw-4*INT_WIDTH(xctx->lw));
  yy1=(y2-y1)/(xctx->areah-4*INT_WIDTH(xctx->lw));
  if(yy1>xctx->zoom) xctx->zoom=yy1;
  xctx->zoom*= factor;
  xctx->mooz=1/xctx->zoom;
  xctx->xorigin=xctx->xorigin+xctx->areaw*xctx->zoom*(1-1/factor)/2;
  xctx->yorigin=xctx->yorigin+xctx->areah*xctx->zoom*(1-1/factor)/2;
}

void zoom_rectangle(int what)
{
  if( (what & START) )
  {
    xctx->nl_x1=xctx->nl_x2=xctx->mousex_snap;xctx->nl_y1=xctx->nl_y2=xctx->mousey_snap;
    xctx->ui_state |= STARTZOOM;
  }
  if( what & END)
  {
    xctx->ui_state &= ~STARTZOOM;
    RECTORDER(xctx->nl_x1,xctx->nl_y1,xctx->nl_x2,xctx->nl_y2);
    drawtemprect(xctx->gctiled, NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
    xctx->xorigin=-xctx->nl_x1;xctx->yorigin=-xctx->nl_y1;
    xctx->zoom=(xctx->nl_x2-xctx->nl_x1)/(xctx->areaw-4*INT_WIDTH(xctx->lw));
    xctx->nl_yy1=(xctx->nl_y2-xctx->nl_y1)/(xctx->areah-4*INT_WIDTH(xctx->lw));
    if(xctx->nl_yy1>xctx->zoom) xctx->zoom=xctx->nl_yy1;
    xctx->mooz=1/xctx->zoom;
    change_linewidth(-1.);
    draw();
    redraw_w_a_l_r_p_rubbers();
    dbg(1, "zoom_rectangle(): coord: %.16g %.16g %.16g %.16g zoom=%.16g\n",
      xctx->nl_x1,xctx->nl_y1,xctx->mousex_snap, xctx->mousey_snap,xctx->zoom);
  }
  if(what & RUBBER)
  {
    xctx->nl_xx1=xctx->nl_x1;xctx->nl_yy1=xctx->nl_y1;xctx->nl_xx2=xctx->nl_x2;xctx->nl_yy2=xctx->nl_y2;
    RECTORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
    drawtemprect(xctx->gctiled,NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
    xctx->nl_x2=xctx->mousex_snap;xctx->nl_y2=xctx->mousey_snap;


    /*  20171211 update selected objects while dragging */
    rebuild_selected_array();
    bbox(START,0.0, 0.0, 0.0, 0.0);
    bbox(ADD, xctx->nl_xx1, xctx->nl_yy1, xctx->nl_xx2, xctx->nl_yy2);
    bbox(SET,0.0, 0.0, 0.0, 0.0);
    draw_selection(xctx->gc[SELLAYER], 0);
    bbox(END,0.0, 0.0, 0.0, 0.0);

    xctx->nl_xx1=xctx->nl_x1;xctx->nl_yy1=xctx->nl_y1;xctx->nl_xx2=xctx->nl_x2;xctx->nl_yy2=xctx->nl_y2;
    RECTORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
    drawtemprect(xctx->gc[SELLAYER], NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
  }
}

#define STORE
void draw_stuff(void)
{
   double x1,y1,w,h, x2, y2;
   int i;
   int n = 200000;
   clear_drawing();
   view_unzoom(40);
   #ifndef STORE
   n /= (cadlayers - 4);
   for(xctx->rectcolor = 4; xctx->rectcolor < cadlayers; xctx->rectcolor++) {
   #else
   #endif
     for(i = 0; i < n; ++i)
      {
       w=(xctx->areaw*xctx->zoom/800) * rand() / (RAND_MAX+1.0);
       h=(xctx->areah*xctx->zoom/80) * rand() / (RAND_MAX+1.0);
       x1=(xctx->areaw*xctx->zoom) * rand() / (RAND_MAX+1.0)-xctx->xorigin;
       y1=(xctx->areah*xctx->zoom) * rand() / (RAND_MAX+1.0)-xctx->yorigin;
       x2=x1+w;
       y2=y1+h;
       ORDER(x1,y1,x2,y2);
       #ifdef STORE
       xctx->rectcolor = (int) (16.0*rand()/(RAND_MAX+1.0))+4;
       storeobject(-1, x1, y1, x2, y2, xRECT,xctx->rectcolor, 0, NULL);
       #else 
       drawtemprect(xctx->gc[xctx->rectcolor], ADD, x1, y1, x2, y2);
       #endif
     }
  
     for(i = 0; i < n; ++i)
      {
       w=(xctx->areaw*xctx->zoom/80) * rand() / (RAND_MAX+1.0);
       h=(xctx->areah*xctx->zoom/800) * rand() / (RAND_MAX+1.0);
       x1=(xctx->areaw*xctx->zoom) * rand() / (RAND_MAX+1.0)-xctx->xorigin;
       y1=(xctx->areah*xctx->zoom) * rand() / (RAND_MAX+1.0)-xctx->yorigin;
       x2=x1+w;
       y2=y1+h;
       ORDER(x1,y1,x2,y2);
       #ifdef STORE
       xctx->rectcolor = (int) (16.0*rand()/(RAND_MAX+1.0))+4;
       storeobject(-1, x1, y1, x2, y2,xRECT,xctx->rectcolor, 0, NULL);
       #else 
       drawtemprect(xctx->gc[xctx->rectcolor], ADD, x1, y1, x2, y2);
       #endif
     }
  
     for(i = 0; i < n; ++i)
     {
       w=xctx->zoom * rand() / (RAND_MAX+1.0);
       h=w;
       x1=(xctx->areaw*xctx->zoom) * rand() / (RAND_MAX+1.0)-xctx->xorigin;
       y1=(xctx->areah*xctx->zoom) * rand() / (RAND_MAX+1.0)-xctx->yorigin;
       x2=x1+w;
       y2=y1+h;
       RECTORDER(x1,y1,x2,y2);
       #ifdef STORE
       xctx->rectcolor = (int) (16.0*rand()/(RAND_MAX+1.0))+4;
       storeobject(-1, x1, y1, x2, y2,xRECT,xctx->rectcolor, 0, NULL);
       #else 
       drawtemprect(xctx->gc[xctx->rectcolor], ADD, x1, y1, x2, y2);
       #endif
     }
   #ifndef STORE
     drawtemprect(xctx->gc[xctx->rectcolor], END, 0.0, 0.0, 0.0, 0.0);
   }
   #else
   draw();
   #endif
}

static void restore_selection(double x1, double y1, double x2, double y2)
{
  double xx1,yy1,xx2,yy2;
  xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
  RECTORDER(xx1,yy1,xx2,yy2);
  rebuild_selected_array();
  if(!xctx->lastsel) return;
  bbox(START,0.0, 0.0, 0.0, 0.0);
  bbox(ADD, xx1, yy1, xx2, yy2);
  bbox(SET,0.0, 0.0, 0.0, 0.0);
  draw_selection(xctx->gc[SELLAYER], 0);
  bbox(END,0.0, 0.0, 0.0, 0.0);
}

void new_wire(int what, double mx_snap, double my_snap)
{
  int big =  xctx->wires> 2000 || xctx->instances > 2000 ;
  int s_pnetname;
  if( (what & PLACE) ) {
    s_pnetname = tclgetboolvar("show_pin_net_names");
    if( (xctx->ui_state & STARTWIRE) && (xctx->nl_x1!=xctx->nl_x2 || xctx->nl_y1!=xctx->nl_y2) ) {
      xctx->push_undo();
      if(xctx->manhattan_lines==1) {
        if(xctx->nl_xx2!=xctx->nl_xx1) {
          xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
          xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
          ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy1);
          storeobject(-1, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy1,WIRE,0,0,NULL);
          hash_wire(XINSERT, xctx->wires-1, 1);
          drawline(WIRELAYER,NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy1, 0, NULL);
        }
        if(xctx->nl_yy2!=xctx->nl_yy1) {
          xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1; 
          xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
          ORDER(xctx->nl_xx2,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
          storeobject(-1, xctx->nl_xx2,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2,WIRE,0,0,NULL);
          hash_wire(XINSERT, xctx->wires-1, 1);
          drawline(WIRELAYER,NOW, xctx->nl_xx2,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2, 0, NULL);
        }
      } else if(xctx->manhattan_lines==2) {
        if(xctx->nl_yy2!=xctx->nl_yy1) {
          xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
          xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
          ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx1,xctx->nl_yy2);
          storeobject(-1, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx1,xctx->nl_yy2,WIRE,0,0,NULL);
          hash_wire(XINSERT, xctx->wires-1, 1);
          drawline(WIRELAYER,NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx1,xctx->nl_yy2, 0, NULL);
        }
        if(xctx->nl_xx2!=xctx->nl_xx1) {
          xctx->nl_xx1=xctx->nl_x1;xctx->nl_yy1=xctx->nl_y1;
          xctx->nl_xx2=xctx->nl_x2;xctx->nl_yy2=xctx->nl_y2;
          ORDER(xctx->nl_xx1,xctx->nl_yy2,xctx->nl_xx2,xctx->nl_yy2);
          storeobject(-1, xctx->nl_xx1,xctx->nl_yy2,xctx->nl_xx2,xctx->nl_yy2,WIRE,0,0,NULL);
          hash_wire(XINSERT, xctx->wires-1, 1);
          drawline(WIRELAYER,NOW, xctx->nl_xx1,xctx->nl_yy2,xctx->nl_xx2,xctx->nl_yy2, 0, NULL);
        }
      } else {
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
        xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
        storeobject(-1, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2,WIRE,0,0,NULL);
        hash_wire(XINSERT, xctx->wires-1, 1);
        drawline(WIRELAYER,NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2, 0, NULL);
      }
      xctx->prep_hi_structs = 0;
      if(tclgetboolvar("autotrim_wires")) trim_wires();
      if(s_pnetname || xctx->hilight_nets) {
        prepare_netlist_structs(0); /* since xctx->prep_hi_structs==0, do a delete_netlist_structs() first,
                                     * this clears both xctx->prep_hi_structs and xctx->prep_net_structs. */
        if(!big) {
          bbox(START , 0.0 , 0.0 , 0.0 , 0.0);
          if(xctx->node_redraw_table.table == NULL)  int_hash_init(&xctx->node_redraw_table, HASHSIZE);
          int_hash_lookup(&(xctx->node_redraw_table),  xctx->wire[xctx->wires-1].node, 0, XINSERT_NOREPLACE);
        } 
        if(!big) {
          find_inst_to_be_redrawn(1 + 4 + 8); /* add bboxes before and after symbol_bbox, don't use selection */
          find_inst_to_be_redrawn(16); /* delete hash and arrays */
          bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
        }
        if(xctx->hilight_nets) {
          propagate_hilights(1, 1, XINSERT_NOREPLACE);
        }
        draw();
        if(!big) bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
      } else update_conn_cues(WIRELAYER, 1,1);
      /* draw_hilight_net(1);*/  /* for updating connection bubbles on hilight nets */
    }
    if(! (what &END)) {
      xctx->nl_x1=mx_snap;
      xctx->nl_y1=my_snap;
      xctx->nl_x2=xctx->mousex_snap;
      xctx->nl_y2=xctx->mousey_snap;
      xctx->nl_xx1=xctx->nl_x1;
      xctx->nl_yy1=xctx->nl_y1;
      xctx->nl_xx2=xctx->mousex_snap;
      xctx->nl_yy2=xctx->mousey_snap;
      if(xctx->manhattan_lines==1) {
        xctx->nl_x2 = mx_snap; xctx->nl_y2 = my_snap;
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
        xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy1);
        drawtempline(xctx->gc[WIRELAYER], NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy1);
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
        xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx2,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
        drawtempline(xctx->gc[WIRELAYER], NOW, xctx->nl_xx2,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      } else if(xctx->manhattan_lines==2) {
        xctx->nl_x2 = mx_snap; xctx->nl_y2 = my_snap;
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
        xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx1,xctx->nl_yy2);
        drawtempline(xctx->gc[WIRELAYER], NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx1,xctx->nl_yy2);
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
        xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx1,xctx->nl_yy2,xctx->nl_xx2,xctx->nl_yy2);
        drawtempline(xctx->gc[WIRELAYER], NOW, xctx->nl_xx1,xctx->nl_yy2,xctx->nl_xx2,xctx->nl_yy2);
      } else {
        xctx->nl_x2 = mx_snap; xctx->nl_y2 = my_snap;
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
        xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
        drawtempline(xctx->gc[WIRELAYER], NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      }
    }
    xctx->ui_state |= STARTWIRE;
  }
  if( what & END) {
    xctx->ui_state &= ~STARTWIRE;
  }
  if( (what & RUBBER)  ) {
    if(xctx->manhattan_lines==1) {
      xctx->nl_xx1=xctx->nl_x1;xctx->nl_yy1=xctx->nl_y1;
      xctx->nl_xx2=xctx->nl_x2;xctx->nl_yy2=xctx->nl_y2;
      ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy1);
      drawtempline(xctx->gctiled, NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy1);
      xctx->nl_xx1=xctx->nl_x1;xctx->nl_yy1=xctx->nl_y1;
      xctx->nl_xx2=xctx->nl_x2;xctx->nl_yy2=xctx->nl_y2;
      ORDER(xctx->nl_xx2,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      drawtempline(xctx->gctiled, NOW, xctx->nl_xx2,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      restore_selection(xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2);
      xctx->nl_x2 = mx_snap; xctx->nl_y2 = my_snap;
      if(!(what & CLEAR)) {
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
        xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy1);
        drawtempline(xctx->gc[WIRELAYER], NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy1);
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
         xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx2,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
        drawtempline(xctx->gc[WIRELAYER], NOW, xctx->nl_xx2,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      }
    } else if(xctx->manhattan_lines==2) {
      xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
      xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
      ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx1,xctx->nl_yy2);
      drawtempline(xctx->gctiled, NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx1,xctx->nl_yy2);
      xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
      xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
      ORDER(xctx->nl_xx1,xctx->nl_yy2,xctx->nl_xx2,xctx->nl_yy2);
      drawtempline(xctx->gctiled, NOW, xctx->nl_xx1,xctx->nl_yy2,xctx->nl_xx2,xctx->nl_yy2);
      restore_selection(xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2);
      xctx->nl_x2 = mx_snap; xctx->nl_y2 = my_snap;
      if(!(what & CLEAR)) {
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
        xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx1,xctx->nl_yy2);
        drawtempline(xctx->gc[WIRELAYER], NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx1,xctx->nl_yy2);
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
        xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx1,xctx->nl_yy2,xctx->nl_xx2,xctx->nl_yy2);
        drawtempline(xctx->gc[WIRELAYER], NOW, xctx->nl_xx1,xctx->nl_yy2,xctx->nl_xx2,xctx->nl_yy2);
      }
    } else {
      xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
      xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
      ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      drawtempline(xctx->gctiled, NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      restore_selection(xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2);
      xctx->nl_x2 = mx_snap; xctx->nl_y2 = my_snap;
      if(!(what & CLEAR)) {
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
        xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
        drawtempline(xctx->gc[WIRELAYER], NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      }
    }
  }
}

void change_layer()
{
  int k, n, type, c;
  double x1,y1,x2,y2, a, b, r;


   if(xctx->lastsel) xctx->push_undo();
   for(k=0;k<xctx->lastsel; ++k)
   {
     n=xctx->sel_array[k].n;
     type=xctx->sel_array[k].type;
     c=xctx->sel_array[k].col;
     if(type==LINE && xctx->line[c][n].sel==SELECTED) {
       x1 = xctx->line[c][n].x1;
       y1 = xctx->line[c][n].y1;
       x2 = xctx->line[c][n].x2;
       y2 = xctx->line[c][n].y2;
       storeobject(-1, x1,y1,x2,y2,LINE,xctx->rectcolor, 0, xctx->line[c][n].prop_ptr);
     }
     if(type==ARC && xctx->arc[c][n].sel==SELECTED) {
       x1 = xctx->arc[c][n].x;
       y1 = xctx->arc[c][n].y;
       r = xctx->arc[c][n].r;
       a = xctx->arc[c][n].a;
       b = xctx->arc[c][n].b;
       store_arc(-1, x1, y1, r, a, b, xctx->rectcolor, 0, xctx->arc[c][n].prop_ptr);
     }
     if(type==POLYGON && xctx->poly[c][n].sel==SELECTED) {
        store_poly(-1, xctx->poly[c][n].x, xctx->poly[c][n].y, 
                       xctx->poly[c][n].points, xctx->rectcolor, 0, xctx->poly[c][n].prop_ptr);
     }
     else if(type==xRECT && xctx->rect[c][n].sel==SELECTED) {
       x1 = xctx->rect[c][n].x1;
       y1 = xctx->rect[c][n].y1;
       x2 = xctx->rect[c][n].x2;
       y2 = xctx->rect[c][n].y2;
       storeobject(-1, x1,y1,x2,y2,xRECT,xctx->rectcolor, 0, xctx->rect[c][n].prop_ptr);
     }
     else if(type==xTEXT && xctx->text[n].sel==SELECTED) {
       if(xctx->rectcolor != xctx->text[n].layer) {
         char *p;
         set_modify(1);
         my_strdup(_ALLOC_ID_, &xctx->text[n].prop_ptr, 
           subst_token(xctx->text[n].prop_ptr, "layer", dtoa(xctx->rectcolor) ));
         xctx->text[n].layer = xctx->rectcolor;
         p = xctx->text[n].prop_ptr;
         while(*p) {
           if(*p == '\n') *p = ' ';
           ++p;
         }
       }
     }
   }
   if(xctx->lastsel) delete_only_rect_line_arc_poly();
   unselect_all(1);
}

void new_arc(int what, double sweep)
{
  if(what & PLACE) {
    xctx->nl_state=0;
    xctx->nl_r = -1.;
    xctx->nl_sweep_angle=sweep;
    xctx->nl_xx1 = xctx->nl_xx2 = xctx->nl_x1 = xctx->nl_x2 = xctx->nl_x3 = xctx->mousex_snap;
    xctx->nl_yy1 = xctx->nl_yy2 = xctx->nl_y1 = xctx->nl_y2 = xctx->nl_y3 = xctx->mousey_snap;
    xctx->ui_state |= STARTARC;
  }
  if(what & SET) {
    if(xctx->nl_state==0) {
      xctx->nl_x2 = xctx->mousex_snap;
      xctx->nl_y2 = xctx->mousey_snap;
      drawtempline(xctx->gctiled, NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      xctx->nl_state=1;
    } else if(xctx->nl_state==1) {
      xctx->nl_x3 = xctx->mousex_snap;
      xctx->nl_y3 = xctx->mousey_snap;
      arc_3_points(xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2,
          xctx->nl_x3, xctx->nl_y3, &xctx->nl_x, &xctx->nl_y, &xctx->nl_r, &xctx->nl_a, &xctx->nl_b);
      if(xctx->nl_sweep_angle==360.) xctx->nl_b=360.;
      if(xctx->nl_r>0.) {
        xctx->push_undo();
        drawarc(xctx->rectcolor, NOW, xctx->nl_x, xctx->nl_y, xctx->nl_r, xctx->nl_a, xctx->nl_b, 0, 0);
        store_arc(-1, xctx->nl_x, xctx->nl_y, xctx->nl_r, xctx->nl_a, xctx->nl_b, xctx->rectcolor, 0, NULL);
      }
      xctx->ui_state &= ~STARTARC;
      xctx->nl_state=0;
    }
  }
  if(what & RUBBER) {
    if(xctx->nl_state==0) {
      drawtempline(xctx->gctiled, NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      xctx->nl_xx2 = xctx->mousex_snap;
      xctx->nl_yy2 = xctx->mousey_snap;
      xctx->nl_xx1 = xctx->nl_x1;xctx->nl_yy1 = xctx->nl_y1;
      ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      drawtempline(xctx->gc[SELLAYER], NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
    }
    else if(xctx->nl_state==1) {
      xctx->nl_x3 = xctx->mousex_snap;
      xctx->nl_y3 = xctx->mousey_snap;
      if(xctx->nl_r>0.) drawtemparc(xctx->gctiled, NOW, xctx->nl_x, xctx->nl_y, xctx->nl_r, xctx->nl_a, xctx->nl_b);
      arc_3_points(xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2,
          xctx->nl_x3, xctx->nl_y3, &xctx->nl_x, &xctx->nl_y, &xctx->nl_r, &xctx->nl_a, &xctx->nl_b);
      if(xctx->nl_sweep_angle==360.) xctx->nl_b=360.;
      if(xctx->nl_r>0.) drawtemparc(xctx->gc[xctx->rectcolor], NOW, xctx->nl_x, xctx->nl_y, xctx->nl_r, xctx->nl_a, xctx->nl_b);
    }
  }
}

void new_line(int what)
{
  if( (what & PLACE) )
  {
    if( (xctx->nl_x1!=xctx->nl_x2 || xctx->nl_y1!=xctx->nl_y2) && (xctx->ui_state & STARTLINE) )
    {
      xctx->push_undo();
      if(xctx->manhattan_lines==1) {
        if(xctx->nl_xx2!=xctx->nl_xx1) {
          xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
          xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
          ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy1);
          storeobject(-1, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy1,LINE,xctx->rectcolor,0,NULL);
          drawline(xctx->rectcolor,NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy1, 0, NULL);
        }
        if(xctx->nl_yy2!=xctx->nl_yy1) {
          xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
          xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
          ORDER(xctx->nl_xx2,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
          storeobject(-1, xctx->nl_xx2,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2,LINE,xctx->rectcolor,0,NULL);
          drawline(xctx->rectcolor,NOW, xctx->nl_xx2,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2, 0, NULL);
        }
      } else if(xctx->manhattan_lines==2) {
        if(xctx->nl_yy2!=xctx->nl_yy1) {
          xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
          xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
          ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx1,xctx->nl_yy2);
          storeobject(-1, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx1,xctx->nl_yy2,LINE,xctx->rectcolor,0,NULL);
          drawline(xctx->rectcolor,NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx1,xctx->nl_yy2, 0, NULL);
        }
        if(xctx->nl_xx2!=xctx->nl_xx1) {
          xctx->nl_xx1=xctx->nl_x1;xctx->nl_yy1=xctx->nl_y1;
          xctx->nl_xx2=xctx->nl_x2;xctx->nl_yy2=xctx->nl_y2;
          ORDER(xctx->nl_xx1,xctx->nl_yy2,xctx->nl_xx2,xctx->nl_yy2);
          storeobject(-1, xctx->nl_xx1,xctx->nl_yy2,xctx->nl_xx2,xctx->nl_yy2,LINE,xctx->rectcolor,0,NULL);
          drawline(xctx->rectcolor,NOW, xctx->nl_xx1,xctx->nl_yy2,xctx->nl_xx2,xctx->nl_yy2, 0, NULL);
        }
      } else {
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
        xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
        storeobject(-1, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2,LINE,xctx->rectcolor,0,NULL);
        drawline(xctx->rectcolor,NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2, 0, NULL);
      }
    }
    xctx->nl_x1=xctx->nl_x2=xctx->mousex_snap;xctx->nl_y1=xctx->nl_y2=xctx->mousey_snap;
    xctx->ui_state |= STARTLINE;
  }
  if( what & END)
  {
    xctx->ui_state &= ~STARTLINE;
  }

  if(what & RUBBER)
  {
    if(xctx->manhattan_lines==1) {
      xctx->nl_xx1=xctx->nl_x1;xctx->nl_yy1=xctx->nl_y1;
      xctx->nl_xx2=xctx->nl_x2;xctx->nl_yy2=xctx->nl_y2;
      ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy1);
      drawtempline(xctx->gctiled, NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy1);
      xctx->nl_xx1=xctx->nl_x1;xctx->nl_yy1=xctx->nl_y1;
      xctx->nl_xx2=xctx->nl_x2;xctx->nl_yy2=xctx->nl_y2;
      ORDER(xctx->nl_xx2,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      drawtempline(xctx->gctiled, NOW, xctx->nl_xx2,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      restore_selection(xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2);
      xctx->nl_x2 = xctx->mousex_snap; xctx->nl_y2 = xctx->mousey_snap;
      if(!(what & CLEAR)) {
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
        xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy1);
        drawtempline(xctx->gc[xctx->rectcolor], NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy1);
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
        xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx2,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
        drawtempline(xctx->gc[xctx->rectcolor], NOW, xctx->nl_xx2,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      }
    } else if(xctx->manhattan_lines==2) {
      xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
      xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
      ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx1,xctx->nl_yy2);
      drawtempline(xctx->gctiled, NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx1,xctx->nl_yy2);
      xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
      xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
      ORDER(xctx->nl_xx1,xctx->nl_yy2,xctx->nl_xx2,xctx->nl_yy2);
      drawtempline(xctx->gctiled, NOW, xctx->nl_xx1,xctx->nl_yy2,xctx->nl_xx2,xctx->nl_yy2);
      restore_selection(xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2);
      xctx->nl_x2 = xctx->mousex_snap; xctx->nl_y2 = xctx->mousey_snap;
      if(!(what & CLEAR)) {
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
        xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx1,xctx->nl_yy2);
        drawtempline(xctx->gc[xctx->rectcolor], NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx1,xctx->nl_yy2);
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
        xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx1,xctx->nl_yy2,xctx->nl_xx2,xctx->nl_yy2);
        drawtempline(xctx->gc[xctx->rectcolor], NOW, xctx->nl_xx1,xctx->nl_yy2,xctx->nl_xx2,xctx->nl_yy2);
      }
    } else {
      xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
      xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
      ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      drawtempline(xctx->gctiled, NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      restore_selection(xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2);
      xctx->nl_x2 = xctx->mousex_snap; xctx->nl_y2 = xctx->mousey_snap;
      if(!(what & CLEAR)) {
        xctx->nl_xx1 = xctx->nl_x1; xctx->nl_yy1 = xctx->nl_y1;
        xctx->nl_xx2 = xctx->nl_x2; xctx->nl_yy2 = xctx->nl_y2;
        ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
        drawtempline(xctx->gc[xctx->rectcolor], NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      }
    }
  }
}

void new_rect(int what)
{
  if( (what & PLACE) )
  {
   if( (xctx->nl_x1!=xctx->nl_x2 || xctx->nl_y1!=xctx->nl_y2) && (xctx->ui_state & STARTRECT) )
   {
    int save_draw;
    RECTORDER(xctx->nl_x1,xctx->nl_y1,xctx->nl_x2,xctx->nl_y2);
    xctx->push_undo();
    drawrect(xctx->rectcolor, NOW, xctx->nl_x1,xctx->nl_y1,xctx->nl_x2,xctx->nl_y2, 0);
    save_draw = xctx->draw_window;
    xctx->draw_window = 1;
    /* draw fill pattern even in xcopyarea mode */
    filledrect(xctx->rectcolor, NOW, xctx->nl_x1,xctx->nl_y1,xctx->nl_x2,xctx->nl_y2);
    xctx->draw_window = save_draw;
    storeobject(-1, xctx->nl_x1,xctx->nl_y1,xctx->nl_x2,xctx->nl_y2,xRECT,xctx->rectcolor, 0, NULL);
   }
   xctx->nl_x1=xctx->nl_x2=xctx->mousex_snap;xctx->nl_y1=xctx->nl_y2=xctx->mousey_snap;
   xctx->ui_state |= STARTRECT;
  }
  if( what & END)
  {
   xctx->ui_state &= ~STARTRECT;
  }
  if(what & RUBBER)
  {
   xctx->nl_xx1=xctx->nl_x1;xctx->nl_yy1=xctx->nl_y1;xctx->nl_xx2=xctx->nl_x2;xctx->nl_yy2=xctx->nl_y2;
   RECTORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
   drawtemprect(xctx->gctiled,NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
   xctx->nl_x2=xctx->mousex_snap;xctx->nl_y2=xctx->mousey_snap;
   xctx->nl_xx1=xctx->nl_x1;xctx->nl_yy1=xctx->nl_y1;xctx->nl_xx2=xctx->nl_x2;xctx->nl_yy2=xctx->nl_y2;
   RECTORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
   drawtemprect(xctx->gc[xctx->rectcolor], NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
  }
}


void new_polygon(int what)
{
   if( what & PLACE ) xctx->nl_points=0; /*  start new polygon placement */

   if(xctx->nl_points >= xctx->nl_maxpoints-1) {  /*  check storage for 2 xctx->nl_points */
     xctx->nl_maxpoints = (1+xctx->nl_points / CADCHUNKALLOC) * CADCHUNKALLOC;
     my_realloc(_ALLOC_ID_, &xctx->nl_polyx, sizeof(double)*xctx->nl_maxpoints);
     my_realloc(_ALLOC_ID_, &xctx->nl_polyy, sizeof(double)*xctx->nl_maxpoints);
   }
   if( what & PLACE )
   {
     /* fprintf(errfp, "new_poly: PLACE, nl_points=%d\n", xctx->nl_points); */
     xctx->nl_polyy[xctx->nl_points]=xctx->mousey_snap;
     xctx->nl_polyx[xctx->nl_points]=xctx->mousex_snap;
     xctx->nl_points++;
     xctx->nl_polyx[xctx->nl_points]=xctx->nl_polyx[xctx->nl_points-1]; /* prepare next point for rubber */
     xctx->nl_polyy[xctx->nl_points] = xctx->nl_polyy[xctx->nl_points-1];
     /* fprintf(errfp, "added point: %.16g %.16g\n", xctx->nl_polyx[xctx->nl_points-1],
         xctx->nl_polyy[xctx->nl_points-1]); */
     xctx->ui_state |= STARTPOLYGON;
   }
   if( what & ADD)
   {
     /* closed poly */
     if(what & END) {
       /* delete last rubber */
       drawtemppolygon(xctx->gctiled, NOW, xctx->nl_polyx, xctx->nl_polyy, xctx->nl_points+1);
       xctx->nl_polyx[xctx->nl_points] = xctx->nl_polyx[0];
       xctx->nl_polyy[xctx->nl_points] = xctx->nl_polyy[0];
     /* add point */
     } else if(xctx->nl_polyx[xctx->nl_points] != xctx->nl_polyx[xctx->nl_points-1] ||
          xctx->nl_polyy[xctx->nl_points] != xctx->nl_polyy[xctx->nl_points-1]) {
       xctx->nl_polyx[xctx->nl_points] = xctx->mousex_snap;
       xctx->nl_polyy[xctx->nl_points] = xctx->mousey_snap;
     } else {
       return;
     }
     xctx->nl_points++;
     /* prepare next point for rubber */
     xctx->nl_polyx[xctx->nl_points]=xctx->nl_polyx[xctx->nl_points-1];
     xctx->nl_polyy[xctx->nl_points]=xctx->nl_polyy[xctx->nl_points-1];
   }
   /* end open or closed poly  by user request */
   if((what & SET || (what & END)) ||
        /* closed poly end by clicking on first point */
        ((what & ADD) && xctx->nl_polyx[xctx->nl_points-1] == xctx->nl_polyx[0] &&
         xctx->nl_polyy[xctx->nl_points-1] == xctx->nl_polyy[0]) ) {
     xctx->push_undo();
     drawtemppolygon(xctx->gctiled, NOW, xctx->nl_polyx, xctx->nl_polyy, xctx->nl_points+1);
     store_poly(-1, xctx->nl_polyx, xctx->nl_polyy, xctx->nl_points, xctx->rectcolor, 0, NULL);
     /* fprintf(errfp, "new_poly: finish: nl_points=%d\n", xctx->nl_points); */
     drawtemppolygon(xctx->gc[xctx->rectcolor], NOW, xctx->nl_polyx, xctx->nl_polyy, xctx->nl_points);
     xctx->ui_state &= ~STARTPOLYGON;
     drawpolygon(xctx->rectcolor, NOW, xctx->nl_polyx, xctx->nl_polyy, xctx->nl_points, 0, 0);
     my_free(_ALLOC_ID_, &xctx->nl_polyx);
     my_free(_ALLOC_ID_, &xctx->nl_polyy);
     xctx->nl_maxpoints = xctx->nl_points = 0;
   }
   if(what & RUBBER)
   {
     /* fprintf(errfp, "new_poly: RUBBER\n"); */
     drawtemppolygon(xctx->gctiled, NOW, xctx->nl_polyx, xctx->nl_polyy, xctx->nl_points+1);
     xctx->nl_polyy[xctx->nl_points] = xctx->mousey_snap;
     xctx->nl_polyx[xctx->nl_points] = xctx->mousex_snap;
     drawtemppolygon(xctx->gc[xctx->rectcolor], NOW, xctx->nl_polyx, xctx->nl_polyy, xctx->nl_points+1);
   }
}

#if HAS_CAIRO==1
int text_bbox(const char *str, double xscale, double yscale,
    short rot, short flip, int hcenter, int vcenter, double x1,double y1, double *rx1, double *ry1,
    double *rx2, double *ry2, int *cairo_lines, double *cairo_longest_line)
{
  int c=0;
  char *str_ptr, *s = NULL;
  double size;
  cairo_text_extents_t ext;
  cairo_font_extents_t fext;
  double ww, hh, maxw;
  
  /*                will not match exactly font metrics when doing ps/svg output , but better than nothing */
  if(!has_x) return text_bbox_nocairo(str, xscale, yscale, rot, flip, hcenter, vcenter, x1, y1,
                                      rx1, ry1, rx2, ry2, cairo_lines, cairo_longest_line);
  size = xscale*52.*cairo_font_scale;

  /*  if(size*xctx->mooz>800.) { */
  /*    return 0; */
  /*  } */

  cairo_set_font_size (xctx->cairo_ctx, size*xctx->mooz);
  cairo_font_extents(xctx->cairo_ctx, &fext);
  ww=0.; hh=1.;
  c=0;
  *cairo_lines=1;
  my_strdup2(_ALLOC_ID_, &s, str);
  str_ptr = s;
  while( s && s[c] ) {
    if(s[c] == '\n') {
      s[c]='\0';
      ++hh;
      (*cairo_lines)++;
      if(str_ptr[0]!='\0') {
        cairo_text_extents(xctx->cairo_ctx, str_ptr, &ext);
        maxw = ext.x_advance > ext.width ? ext.x_advance : ext.width;
        if(maxw > ww) ww= maxw;
      }
      s[c]='\n';
      str_ptr = s+c+1;
    } else {
    }
    ++c;
  }
  if(str_ptr && str_ptr[0]!='\0') {
    cairo_text_extents(xctx->cairo_ctx, str_ptr, &ext);
    maxw = ext.x_advance > ext.width ? ext.x_advance : ext.width;
    if(maxw > ww) ww= maxw;
  }
  my_free(_ALLOC_ID_, &s);
  hh = hh*fext.height * cairo_font_line_spacing;
  *cairo_longest_line = ww;

  *rx1=x1;*ry1=y1;
  if(hcenter) {
    if     (rot==0 && flip == 0) { *rx1-= ww*xctx->zoom/2;}
    else if(rot==1 && flip == 0) { *ry1-= ww*xctx->zoom/2;}
    else if(rot==2 && flip == 0) { *rx1+= ww*xctx->zoom/2;}
    else if(rot==3 && flip == 0) { *ry1+= ww*xctx->zoom/2;}
    else if(rot==0 && flip == 1) { *rx1+= ww*xctx->zoom/2;}
    else if(rot==1 && flip == 1) { *ry1+= ww*xctx->zoom/2;}
    else if(rot==2 && flip == 1) { *rx1-= ww*xctx->zoom/2;}
    else if(rot==3 && flip == 1) { *ry1-= ww*xctx->zoom/2;}
  }

  if(vcenter) {
    if     (rot==0 && flip == 0) { *ry1-= hh*xctx->zoom/2;}
    else if(rot==1 && flip == 0) { *rx1+= hh*xctx->zoom/2;}
    else if(rot==2 && flip == 0) { *ry1+= hh*xctx->zoom/2;}
    else if(rot==3 && flip == 0) { *rx1-= hh*xctx->zoom/2;}
    else if(rot==0 && flip == 1) { *ry1-= hh*xctx->zoom/2;}
    else if(rot==1 && flip == 1) { *rx1+= hh*xctx->zoom/2;}
    else if(rot==2 && flip == 1) { *ry1+= hh*xctx->zoom/2;}
    else if(rot==3 && flip == 1) { *rx1-= hh*xctx->zoom/2;}
  }


  ROTATION(rot, flip, 0.0,0.0, ww*xctx->zoom,hh*xctx->zoom,(*rx2),(*ry2));
  *rx2+=*rx1;*ry2+=*ry1;
  if     (rot==0) {*ry1-=cairo_vert_correct; *ry2-=cairo_vert_correct;}
  else if(rot==1) {*rx1+=cairo_vert_correct; *rx2+=cairo_vert_correct;}
  else if(rot==2) {*ry1+=cairo_vert_correct; *ry2+=cairo_vert_correct;}
  else if(rot==3) {*rx1-=cairo_vert_correct; *rx2-=cairo_vert_correct;}
  RECTORDER((*rx1),(*ry1),(*rx2),(*ry2));
  return 1;
}
int text_bbox_nocairo(const char *str,double xscale, double yscale,
    short rot, short flip, int hcenter, int vcenter, double x1,double y1, double *rx1, double *ry1,
    double *rx2, double *ry2, int *cairo_lines, double *cairo_longest_line)
#else
int text_bbox(const char *str,double xscale, double yscale,
    short rot, short flip, int hcenter, int vcenter, double x1,double y1, double *rx1, double *ry1,
    double *rx2, double *ry2, int *cairo_lines, double *cairo_longest_line)
#endif
{
 register int c=0, length =0;
 double w, h;
  
  w=0;h=1;
  *cairo_lines = 1;
  if(str!=NULL) while( str[c] )
  {
   if((str)[c++]=='\n') {(*cairo_lines)++; h++; length=0;}
   else length++;
   if(length > w)
     w = length;
  }
  w *= (FONTWIDTH+FONTWHITESPACE)*xscale* tclgetdoublevar("nocairo_font_xscale");
  *cairo_longest_line = w;
  h *= (FONTHEIGHT+FONTDESCENT+FONTWHITESPACE)*yscale* tclgetdoublevar("nocairo_font_yscale");
  *rx1=x1;*ry1=y1;
  if(     rot==0) *ry1-=nocairo_vert_correct;
  else if(rot==1) *rx1+=nocairo_vert_correct;
  else if(rot==2) *ry1+=nocairo_vert_correct;
  else            *rx1-=nocairo_vert_correct;

  if(hcenter) {
    if     (rot==0 && flip == 0) { *rx1-= w/2;}
    else if(rot==1 && flip == 0) { *ry1-= w/2;}
    else if(rot==2 && flip == 0) { *rx1+= w/2;}
    else if(rot==3 && flip == 0) { *ry1+= w/2;}
    else if(rot==0 && flip == 1) { *rx1+= w/2;}
    else if(rot==1 && flip == 1) { *ry1+= w/2;}
    else if(rot==2 && flip == 1) { *rx1-= w/2;}
    else if(rot==3 && flip == 1) { *ry1-= w/2;}
  }

  if(vcenter) {
    if     (rot==0 && flip == 0) { *ry1-= h/2;}
    else if(rot==1 && flip == 0) { *rx1+= h/2;}
    else if(rot==2 && flip == 0) { *ry1+= h/2;}
    else if(rot==3 && flip == 0) { *rx1-= h/2;}
    else if(rot==0 && flip == 1) { *ry1-= h/2;}
    else if(rot==1 && flip == 1) { *rx1+= h/2;}
    else if(rot==2 && flip == 1) { *ry1+= h/2;}
    else if(rot==3 && flip == 1) { *rx1-= h/2;}
  }

  ROTATION(rot, flip, 0.0,0.0,w,h,(*rx2),(*ry2));
  *rx2+=*rx1;*ry2+=*ry1;
  RECTORDER((*rx1),(*ry1),(*rx2),(*ry2));
  return 1;
}

/* round() does not exist in C89 */
double my_round(double a)
{
  /* return 0.0 or -0.0 if a == 0.0 or -0.0 */
  return (a > 0.0) ? floor(a + 0.5) : (a < 0.0) ? ceil(a - 0.5) : a;
}

double round_to_n_digits(double x, int n)
{
  double scale;
  if(x == 0.0) return x;
  scale = pow(10.0, ceil(log10(fabs(x))) - n);
  return my_round(x / scale) * scale;
}

double floor_to_n_digits(double x, int n)
{
  double scale;
  if(x == 0.0) return x;
  scale = pow(10.0, ceil(log10(fabs(x))) - n);
  return floor(x / scale) * scale;
}

double ceil_to_n_digits(double x, int n)
{
  double scale;
  if(x == 0.0) return x;
  scale = pow(10.0, ceil(log10(fabs(x))) - n);
  return ceil(x / scale) * scale;
}

int place_text(int draw_text, double mx, double my)
{
  char *txt;
  int textlayer;
  /* const char *str; */
  int save_draw;
  xText *t = &xctx->text[xctx->texts];
  #if HAS_CAIRO==1
  const char  *textfont;
  #endif

  tclsetvar("props","");
  tclsetvar("retval","");

  if(!tclgetvar("hsize"))
   tclsetvar("hsize","0.4");
  if(!tclgetvar("vsize"))
   tclsetvar("vsize","0.4");
  xctx->semaphore++;
  tcleval("enter_text {text:} normal");
  xctx->semaphore--;

  dbg(1, "place_text(): hsize=%s vsize=%s\n",tclgetvar("hsize"), tclgetvar("vsize") );

  txt =  (char *)tclgetvar("retval");
  if(!txt || !strcmp(txt,"")) return 0;   /*  dont allocate text object if empty string given */
  xctx->push_undo();
  check_text_storage();
  t->txt_ptr=NULL;
  t->prop_ptr=NULL;  /*  20111006 added missing initialization of pointer */
  t->font=NULL;
  my_strdup(_ALLOC_ID_, &t->txt_ptr, txt);
  t->x0=mx;
  t->y0=my;
  t->rot=0;
  t->flip=0;
  t->sel=0;
  t->xscale= atof(tclgetvar("hsize"));
  t->yscale= atof(tclgetvar("vsize"));
  my_strdup(_ALLOC_ID_, &t->prop_ptr, (char *)tclgetvar("props"));
  /*  debug ... */
  /*  t->prop_ptr=NULL; */
  dbg(1, "place_text(): done text input\n");
  set_text_flags(t);
  textlayer = t->layer;
  if(textlayer < 0 || textlayer >= cadlayers) textlayer = TEXTLAYER;
  #if HAS_CAIRO==1
  textfont = t->font;
  if((textfont && textfont[0]) || (t->flags & (TEXT_BOLD | TEXT_OBLIQUE | TEXT_ITALIC))) {
    cairo_font_slant_t slant;
    cairo_font_weight_t weight;
    textfont = (t->font && t->font[0]) ? t->font : tclgetvar("cairo_font_name");
    weight = ( t->flags & TEXT_BOLD) ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL;
    slant = CAIRO_FONT_SLANT_NORMAL;
    if(t->flags & TEXT_ITALIC) slant = CAIRO_FONT_SLANT_ITALIC;
    if(t->flags & TEXT_OBLIQUE) slant = CAIRO_FONT_SLANT_OBLIQUE;
    cairo_save(xctx->cairo_ctx);
    cairo_save(xctx->cairo_save_ctx);
    xctx->cairo_font =
          cairo_toy_font_face_create(textfont, slant, weight);
    cairo_set_font_face(xctx->cairo_ctx, xctx->cairo_font);
    cairo_set_font_face(xctx->cairo_save_ctx, xctx->cairo_font);
    cairo_font_face_destroy(xctx->cairo_font);
  }
  #endif
  save_draw=xctx->draw_window;
  xctx->draw_window=1;
  if(draw_text) {
    draw_string(textlayer, NOW, t->txt_ptr, 0, 0, t->hcenter, t->vcenter, t->x0,t->y0, t->xscale, t->yscale);
  }
  xctx->draw_window = save_draw;
  #if HAS_CAIRO==1
  if((textfont && textfont[0]) || (t->flags & (TEXT_BOLD | TEXT_OBLIQUE | TEXT_ITALIC))) {
    cairo_restore(xctx->cairo_ctx);
    cairo_restore(xctx->cairo_save_ctx);
  }
  #endif
  xctx->texts++;
  select_text(xctx->texts - 1, SELECTED, 0);
  rebuild_selected_array(); /* sets xctx->ui_state |= SELECTION */
  drawtemprect(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
  drawtempline(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
  set_modify(1);
  return 1;
}

void pan(int what, int mx, int my)
{
  int dx, dy, ddx, ddy;
  if(what & START) {
    xctx->mmx_s = xctx->mx_s = mx;
    xctx->mmy_s = xctx->my_s = my;
    xctx->xorig_save = xctx->xorigin;
    xctx->yorig_save = xctx->yorigin;
  }
  else if(what == RUBBER) {
    dx = mx - xctx->mx_s;
    dy = my - xctx->my_s;
    ddx = abs(mx -xctx->mmx_s);
    ddy = abs(my -xctx->mmy_s);
    if(ddx>5 || ddy>5) {
      xctx->xorigin = xctx->xorig_save + dx*xctx->zoom;
      xctx->yorigin = xctx->yorig_save + dy*xctx->zoom;
      draw();
      xctx->mmx_s = mx;
      xctx->mmy_s = my;
    }
  }
}

/*  20150927 select=1: select objects, select=0: unselect objects */
void select_rect(int what, int select)
{
 if(what & RUBBER)
 {
    if(xctx->nl_sem==0) {
      fprintf(errfp, "ERROR: select_rect() RUBBER called before START\n");
      tcleval("alert_ {ERROR: select_rect() RUBBER called before START} {}");
    }
    xctx->nl_xx1=xctx->nl_xr;xctx->nl_xx2=xctx->nl_xr2;xctx->nl_yy1=xctx->nl_yr;xctx->nl_yy2=xctx->nl_yr2;
    RECTORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
    drawtemprect(xctx->gctiled,NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
    xctx->nl_xr2=xctx->mousex_snap;xctx->nl_yr2=xctx->mousey_snap;

    /*  20171026 update unselected objects while dragging */
    rebuild_selected_array();
    bbox(START,0.0, 0.0, 0.0, 0.0);
    bbox(ADD, xctx->nl_xx1, xctx->nl_yy1, xctx->nl_xx2, xctx->nl_yy2);
    bbox(SET,0.0, 0.0, 0.0, 0.0);
    draw_selection(xctx->gc[SELLAYER], 0);
    if(!xctx->nl_sel) select_inside(xctx->nl_xx1, xctx->nl_yy1, xctx->nl_xx2, xctx->nl_yy2, xctx->nl_sel);
    bbox(END,0.0, 0.0, 0.0, 0.0);
    xctx->nl_xx1=xctx->nl_xr;xctx->nl_xx2=xctx->nl_xr2;xctx->nl_yy1=xctx->nl_yr;xctx->nl_yy2=xctx->nl_yr2;
    RECTORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
    drawtemprect(xctx->gc[SELLAYER],NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
 }
 else if(what & START)
 {
    /*
     * if(xctx->nl_sem==1) {
     *  fprintf(errfp, "ERROR: reentrant call of select_rect()\n");
     *  tcleval("alert_ {ERROR: reentrant call of select_rect()} {}");
     * }
     */
    xctx->nl_sel = select;
    xctx->ui_state |= STARTSELECT;

    /*  use m[xy]_double_save instead of mouse[xy]_snap */
    /*  to avoid delays in setting the start point of a */
    /*  selection rectangle, this is noticeable and annoying on */
    /*  networked / slow X servers. 20171218 */
    /* xctx->nl_xr=xctx->nl_xr2=xctx->mousex_snap; */
    /* xctx->nl_yr=xctx->nl_yr2=xctx->mousey_snap; */
    xctx->nl_xr=xctx->nl_xr2=xctx->mx_double_save;
    xctx->nl_yr=xctx->nl_yr2=xctx->my_double_save;
    xctx->nl_sem=1;
 }
 else if(what & END)
 {
    RECTORDER(xctx->nl_xr,xctx->nl_yr,xctx->nl_xr2,xctx->nl_yr2);
    drawtemprect(xctx->gctiled, NOW, xctx->nl_xr,xctx->nl_yr,xctx->nl_xr2,xctx->nl_yr2);
    /*  draw_selection(xctx->gc[SELLAYER], 0); */
    select_inside(xctx->nl_xr,xctx->nl_yr,xctx->nl_xr2,xctx->nl_yr2, xctx->nl_sel);


    bbox(START,0.0, 0.0, 0.0, 0.0);
    bbox(ADD, xctx->nl_xr, xctx->nl_yr, xctx->nl_xr2, xctx->nl_yr2);
    bbox(SET,0.0, 0.0, 0.0, 0.0);
    draw_selection(xctx->gc[SELLAYER], 0);
    bbox(END,0.0, 0.0, 0.0, 0.0);
    /*  /20171219 */

    xctx->ui_state &= ~STARTSELECT;
    xctx->nl_sem=0;
 }
}

