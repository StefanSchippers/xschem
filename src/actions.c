/* File: actions.c
 *
 * This file is part of XSCHEM,
 * a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
 * simulation.
 * Copyright (C) 1998-2024 Stefan Frederik Schippers
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
  fd = my_fopen(f, "r"); /* windows won't return \r in the lines and we chop them out anyway in the code */
  if(fd) {
    while((line = my_fgets(fd, &n))) {
      /* skip lines of type: '** sch_path: ...' or '-- sch_path: ...' or '// sym_path: ...'
       * skip also .include /path/to/some/file */
      if(skip_path_lines && n > 14) {
        if(!strncmp(line+2, " sch_path: ", 11) || !strncmp(line+2, " sym_path: ", 11) ) {
          my_free(_ALLOC_ID_, &line);
          continue;
        }
        if(!strncmp(line, ".include ", 9) || !strncmp(line, ".INCLUDE ", 9) ) {
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

int there_are_floaters(void)
{
  int floaters = 0, k;
  for(k = 0; k < xctx->texts; k++) {
    if(xctx->text[k].flags & TEXT_FLOATER) {
      floaters = 1;
      dbg(1, "text %d is a floater\n", k);
      break;
    }
  }
  return floaters;
}

const char *get_text_floater(int i)
{
  const char *txt_ptr =  xctx->text[i].txt_ptr;
  if(xctx->text[i].flags & TEXT_FLOATER) {
    int inst = -1;
    const char *instname;

    if(!xctx->floater_inst_table.table) {
      floater_hash_all_names();
    }
      
    if(xctx->text[i].floater_instname) 
      instname = xctx->text[i].floater_instname;
    else {
      instname = get_tok_value(xctx->text[i].prop_ptr, "name", 0);
      if(!xctx->tok_size) {
        instname = get_tok_value(xctx->text[i].prop_ptr, "floater", 0);
      }
    }
    inst = get_instance(instname);
    if(inst >= 0) {
      if(xctx->text[i].floater_ptr) {
        txt_ptr = xctx->text[i].floater_ptr;
      } else {
        /* cache floater translated text to avoid re-evaluating every time schematic is drawn */
        my_strdup2(_ALLOC_ID_, &xctx->text[i].floater_ptr, translate(inst, xctx->text[i].txt_ptr));
        txt_ptr = xctx->text[i].floater_ptr;
      }
      dbg(1, "floater: %s\n",txt_ptr);
    } else {
      /* do just a tcl substitution if floater does not reference an existing instance 
       * (but name=something or floater=something attribute must be present) and text
       * matches tcleval(...) or contains '@' */
      if(strstr(txt_ptr, "tcleval(") == txt_ptr || strchr(txt_ptr, '@')) {
        /* my_strdup2(_ALLOC_ID_, &xctx->text[i].floater_ptr, tcl_hook2(xctx->text[i].txt_ptr)); */
        my_strdup2(_ALLOC_ID_, &xctx->text[i].floater_ptr, translate(-1, xctx->text[i].txt_ptr));
        txt_ptr = xctx->text[i].floater_ptr;
      }
    }
  }
  return txt_ptr;
} 

/* mod:
 *   0 : clear modified flag, update title and tab names, upd. simulation button colors.
 *   1 : set modified flag, update title and tab names, upd. simulation button colors, rst floater caches.
 *   2 : clear modified flag, do nothing else.
 *   3 : set modified flag, do nothing else.
 *  -1 : set title, rst floater caches.
 *  -2 : rst floater caches, update simulation button colors (Simulate, Waves, Netlist).
 * If floaters are present set_modify(1) (after a modify operation) must be done before draw()
 * to invalidate cached floater string values  before redrawing
 * return 1 if floaters are found (mod==-2 or mod == 1 or mod == -1) */
int set_modify(int mod)
{
  int i, floaters = 0;
  
  dbg(1, "set_modify(): %d, prev_set_modify=%d\n", mod, xctx->prev_set_modify);

  /* set modify state */
  if(mod == 0 || mod == 1 || mod == 2 || mod == 3) {
    xctx->modified = (mod & 1);
  }
  if(mod == 1 || (mod == 0  && xctx->prev_set_modify) || mod == -2) {
    /*                Do not configure buttons if displaying in preview window */
    if(has_x && (xctx->top_path[0] == '\0' || strstr(xctx->top_path, ".x") == xctx->top_path)) {
      char s[256];
      tclvareval("catch {", xctx->top_path, ".menubar entryconfigure Netlist -background $simulate_bg}", NULL);
      tclvareval("set tctx::", xctx->current_win_path, "_netlist $simulate_bg", NULL);
      my_snprintf(s, S(s), "tctx::%s_simulate_id", xctx->current_win_path);
      if(tclgetvar(s)) {
        tclvareval("catch {", xctx->top_path, ".menubar entryconfigure Simulate -background ", tclresult(), "}", NULL);
        tclvareval("set tctx::", xctx->current_win_path, "_simulate ", tclresult(), NULL);
      } else {
        tclvareval("catch {", xctx->top_path, ".menubar entryconfigure Simulate -background $simulate_bg}", NULL);
        tclvareval("set tctx::", xctx->current_win_path, "_simulate $simulate_bg", NULL);
      }
    }
    if(sch_waves_loaded() >= 0) {
      if(has_x && (xctx->top_path[0] == '\0' || strstr(xctx->top_path, ".x") == xctx->top_path)) {
        tclvareval("set tctx::", xctx->current_win_path, "_waves Green", NULL);
        tclvareval("catch {", xctx->top_path, ".menubar entryconfigure Waves -background Green}", NULL);
      }
    } else {
      if(has_x && (xctx->top_path[0] == '\0' || strstr(xctx->top_path, ".x") == xctx->top_path)) {
        tclvareval("set tctx::", xctx->current_win_path, "_waves $simulate_bg", NULL);
        tclvareval("catch {", xctx->top_path, ".menubar entryconfigure Waves -background $simulate_bg}", NULL);
      }
    }
  }  
  
  /* clear floater caches */
  if(mod == 1 || mod == -2 || mod == -1) {
    for(i = 0; i < xctx->texts; i++)
    if(xctx->text[i].flags & TEXT_FLOATER) {
      floaters++;
      my_free(_ALLOC_ID_, &xctx->text[i].floater_ptr); /* clear floater cached value */
    }
    int_hash_free(&xctx->floater_inst_table);
  }

  /* force title   no mod      mod */
  if(mod == -1 || mod == 0 || mod == 1) {
    if(has_x &&
       strcmp(get_cell(xctx->sch[xctx->currsch],1), "systemlib/font") &&
       (xctx->prev_set_modify != xctx->modified || mod == -1)
      ) {
      char *top_path =  xctx->top_path[0] ? xctx->top_path : ".";
      if(xctx->modified == 1) {
        tclvareval("wm title ", top_path, " \"xschem - [file tail [xschem get schname]]*\"", NULL);
        tclvareval("wm iconname ", top_path, " \"xschem - [file tail [xschem get schname]]*\"", NULL);
      } else {
        tclvareval("wm title ", top_path, " \"xschem - [file tail [xschem get schname]]\"", NULL);
        tclvareval("wm iconname ", top_path, " \"xschem - [file tail [xschem get schname]]\"", NULL);
      }
      dbg(1, "modified=%d, schname=%s\n", xctx->modified, xctx->current_name);
      if(xctx->modified) tcleval("set_tab_names *");
      else tcleval("set_tab_names");
    }
  }
  xctx->prev_set_modify = xctx->modified;
  return floaters;
}

void print_version()
{
  printf("XSCHEM V%s\n", XSCHEM_VERSION);
  printf("Copyright (C) 1998-2024 Stefan Schippers\n");
  printf("\n");
  printf("This is free software; see the source for copying conditions.  There is NO\n");
  printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
}

char *escape_chars(const char *source, const char *charset)
{
  int s=0;
  int d=0;
  static char *dest = NULL;
  size_t slen, size;

  if(!source) {
    if(dest)  my_free(_ALLOC_ID_, &dest);
    return NULL;
  }
  slen = strlen(source);
  size = slen + 1;
  my_realloc(_ALLOC_ID_, &dest, size);
  while(source && source[s]) {
    if(d >= size - 2) {
      size += 2;
      my_realloc(_ALLOC_ID_, &dest, size);
    }
    if(!strcmp(charset, "")) {
      switch(source[s]) {
        case '\n':
          dest[d++] = '\\';
          dest[d++] = 'n';
          break;
        case '\t':
          dest[d++] = '\\';
          dest[d++] = 't';
          break;
        case '\\':
        case '\'':
        case ' ':
        case ';':
        case '$':
        case '!':
        case '#':
        case '{':
        case '}':
        case '[':
        case ']':
        case '"':
          dest[d++] = '\\';
          dest[d++] = source[s];
          break;
        default:
          dest[d++] = source[s];
      }
    } else {
      if(strchr(charset, source[s])) { 
        dest[d++] = '\\';
        dest[d++] = source[s];
      } else {
        dest[d++] = source[s];
      }
    }
    s++;
  }
  dest[d] = '\0';
  return dest;
}

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
    if(has_x) {
      if(cs == default_snap) {
        tclvareval(xctx->top_path, ".statusbar.3 configure -background PaleGreen", NULL);
      } else {
        tclvareval(xctx->top_path, ".statusbar.3 configure -background OrangeRed", NULL);
      }
    }
    xctx->cadhalfdotsize = CADHALFDOTSIZE * (cs < 20. ? cs : 20.) / 10.;
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
    if(has_x) {
      if(cg == default_grid) {
        tclvareval(xctx->top_path, ".statusbar.5 configure -background PaleGreen", NULL);
      } else {
        tclvareval(xctx->top_path, ".statusbar.5 configure -background OrangeRed", NULL);
      }
    }
    tclsetdoublevar("cadgrid", cg);
}


/* 
 *
 *   
 * what==0: force creation of $netlist_dir (if netlist_dir variable not empty)
 *           and return current setting.
 * 
 * what==1: if no dir given prompt user
 *           else set netlist_dir to dir  
 *   
 * what==2: just set netlist_dir according to local_netlist_dir setting
 */
int set_netlist_dir(int what, const char *dir)
{
  char cmd[PATH_MAX+200];
  if(dir) my_snprintf(cmd, S(cmd), "set_netlist_dir %d {%s}", what, dir);
  else    my_snprintf(cmd, S(cmd), "set_netlist_dir %d", what);
  tcleval(cmd);
  if(!strcmp("", tclresult()) ) {
    return 0;
  }
  return 1;
}

/* wrapper to TCL function */
/* remove parameter section of symbol generator before calculating abs path : xxx(a,b) -> xxx */
const char *sanitized_abs_sym_path(const char *s, const char *ext)
{   
  char c[PATH_MAX+1000];
      
  my_snprintf(c, S(c), "abs_sym_path [regsub {\\(.*} {%s} {}] {%s}", s, ext);
  tcleval(c);
  return tclresult();
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
  if(strchr(f,'(')) my_strncpy(ff, f, S(ff)); /* generator: return as is */
  else {
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
  }
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
    tcleval("exit 1");
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
 *  confirm: 
 *    0 : do not ask user to save
 *    1 : ask user to save
 *  fast:
 *    passed to save_schematic
 */
int save(int confirm, int fast)
{
  struct stat buf;
  char *name = xctx->sch[xctx->currsch];
  int force = 0;

  /* current schematic exists on disk ... */
  if(!stat(name, &buf)) {
    /* ... and modification time on disk has changed since file loaded ... */
    if(xctx->time_last_modify && xctx->time_last_modify != buf.st_mtime) {
      /* ... so force a save */
      force = 1;
      confirm = 0;
    }
  } 

  if(force || xctx->modified)
  {
    dbg(1, "save(): force=%d modified=%d\n", force, xctx->modified);
    if(confirm) {
      tcleval("ask_save_optional");
      if(!strcmp(tclresult(), "") ) return -1; /* user clicks "Cancel" */
      else if(!strcmp(tclresult(), "yes") ) return save_schematic(xctx->sch[xctx->currsch], fast);
      else return 0; /* user clicks "no" */
    } else {
      return save_schematic(xctx->sch[xctx->currsch], fast);
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
        my_snprintf(name, S(name), "save_file_dialog {Save file} * INITIALLOADDIR {%s}", filename);
      } else {
        my_snprintf(name, S(name), "save_file_dialog {Save file} * INITIALLOADDIR {%s}", filename);
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
    save_schematic(res, 0);
    tclvareval("update_recent_file {", res,"}",  NULL);
    return;
}

void ask_new_file(void)
{
    char f[PATH_MAX]; /*  overflow safe 20161125 */

    if(!has_x) return;
    if(xctx->modified) {
      if(save(1, 0) == -1 ) return; /*  user cancels save, so do nothing. */
    }
    tcleval("load_file_dialog {Load file} *.\\{sch,sym,tcl\\} INITIALLOADDIR");
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
        tcleval(msg);
        if(strcmp(tclresult(), "ok")) skip = 1;
      }
      if(!skip) {
        dbg(1, "ask_new_file(): load file: %s\n", f);
        clear_all_hilights();
        xctx->currsch = 0;
        unselect_all(1);
        remove_symbols();
        xctx->zoom=CADINITIALZOOM;
        xctx->mooz=1/CADINITIALZOOM;
        xctx->xorigin=CADINITIALX;
        xctx->yorigin=CADINITIALY;
        load_schematic(1, f, 1, 1);
        tclvareval("update_recent_file {", f, "}", NULL);
        if(xctx->portmap[xctx->currsch].table) str_hash_free(&xctx->portmap[xctx->currsch]);
        my_strdup(_ALLOC_ID_, &xctx->sch_path[xctx->currsch],".");
        xctx->sch_path_hash[xctx->currsch] = 0;
        xctx->sch_inst_number[xctx->currsch] = 1;
        zoom_full(1, 0, 1 + 2 * tclgetboolvar("zoom_full_center"), 0.97);
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

  dbg(1,"clearing symbol %d: %s\n", j, xctx->sym[j].name);
  my_free(_ALLOC_ID_, &xctx->sym[j].prop_ptr);
  my_free(_ALLOC_ID_, &xctx->sym[j].templ);
  my_free(_ALLOC_ID_, &xctx->sym[j].parent_prop_ptr);
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
      dbg(1, "remove_symbol(): freeing symbol %d text_ptr %d\n", j, i);
    }
    if(xctx->sym[j].text[i].font != NULL) {
      my_free(_ALLOC_ID_, &xctx->sym[j].text[i].font);
    }
    if(xctx->sym[j].text[i].floater_instname != NULL) {
      my_free(_ALLOC_ID_, &xctx->sym[j].text[i].floater_instname);
    }
    if(xctx->sym[j].text[i].floater_ptr != NULL) {
      my_free(_ALLOC_ID_, &xctx->sym[j].text[i].floater_ptr);
    }
  }
  my_free(_ALLOC_ID_, &xctx->sym[j].text);

  my_free(_ALLOC_ID_, &xctx->sym[j].line);
  my_free(_ALLOC_ID_, &xctx->sym[j].rect);
  my_free(_ALLOC_ID_, &xctx->sym[j].arc);
  my_free(_ALLOC_ID_, &xctx->sym[j].poly);
  my_free(_ALLOC_ID_, &xctx->sym[j].lines);
  my_free(_ALLOC_ID_, &xctx->sym[j].polygons);
  my_free(_ALLOC_ID_, &xctx->sym[j].arcs);
  my_free(_ALLOC_ID_, &xctx->sym[j].rects);

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
 * graph                1
 *   unlocked           2
 *   private_cursor     4
 * image             1024
 *   unscaled        2048
 */
int set_rect_flags(xRect *r)
{
  const char *flags;
  unsigned short f = 0;
  if(r->prop_ptr && r->prop_ptr[0]) {
    flags = get_tok_value(r->prop_ptr,"flags",0);
    if(strstr(flags, "graph")) {
      f |= 1;
      if(strstr(flags, "unlocked")) f |= 2;
      if(strstr(flags, "private_cursor")) f |= 4;
    }
    if(strstr(flags, "image")) {
      f |= 1024;
      if(strstr(flags, "unscaled")) f |= 2048;
    }
  }
  r->flags = f;
  dbg(1, "set_rect_flags(): flags=%d\n", f);
  return f;
}

int set_sym_flags(xSymbol *sym)
{
  const char *ptr;
  sym->flags = 0;
  my_strdup2(_ALLOC_ID_, &sym->templ,
             get_tok_value(sym->prop_ptr, "template", 0));

  my_strdup2(_ALLOC_ID_, &sym->type,
             get_tok_value(sym->prop_ptr, "type",0));

  if(!strboolcmp(get_tok_value(sym->prop_ptr,"highlight",0), "true"))
    sym->flags |= HILIGHT_CONN;

  if(!strboolcmp(get_tok_value(sym->prop_ptr,"hide",0), "true"))
    sym->flags |= HIDE_INST;

  ptr = get_tok_value(sym->prop_ptr,"spice_ignore",0);
  if(!strcmp(ptr, "short"))
       sym->flags |= SPICE_SHORT;
  else if(!strboolcmp(ptr, "true") || !strcmp(ptr, "open"))
       sym->flags |= SPICE_IGNORE;

  ptr = get_tok_value(sym->prop_ptr,"spectre_ignore",0);
  if(!strcmp(ptr, "short"))
       sym->flags |= SPECTRE_SHORT;
  else if(!strboolcmp(ptr, "true") || !strcmp(ptr, "open"))
       sym->flags |= SPECTRE_IGNORE;
  
  ptr = get_tok_value(sym->prop_ptr,"verilog_ignore",0);
  if(!strcmp(ptr, "short"))
       sym->flags |= VERILOG_SHORT;
  else if(!strboolcmp(ptr, "true") || !strcmp(ptr, "open"))
       sym->flags |= VERILOG_IGNORE;

  ptr = get_tok_value(sym->prop_ptr,"vhdl_ignore",0);
  if(!strcmp(ptr, "short"))
       sym->flags |= VHDL_SHORT;
  else if(!strboolcmp(ptr, "true") || !strcmp(ptr, "open"))
       sym->flags |= VHDL_IGNORE;

  ptr = get_tok_value(sym->prop_ptr,"tedax_ignore",0);
  if(!strcmp(ptr, "short"))
       sym->flags |= TEDAX_SHORT;
  else if(!strboolcmp(ptr, "true") || !strcmp(ptr, "open"))
       sym->flags |= TEDAX_IGNORE;

  ptr = get_tok_value(sym->prop_ptr,"lvs_ignore",0);
  if(!strcmp(ptr, "short"))
       sym->flags |= LVS_IGNORE_SHORT;
  else if(!strboolcmp(ptr, "true") || !strcmp(ptr, "open"))
       sym->flags |= LVS_IGNORE_OPEN;
  dbg(1, "set_sym_flags: inst %s flags=%d\n", sym->name, sym->flags);
  return 0;
}

int set_inst_flags(xInstance *inst)
{
  const char *ptr;
  inst->flags &= IGNORE_INST; /* do not clear IGNORE_INST bit, used in draw_symbol() */
  my_strdup2(_ALLOC_ID_, &inst->instname, get_tok_value(inst->prop_ptr, "name", 0));
  dbg(1, "set_inst_flags(): instname=%s\n", inst->instname);
  if(inst->ptr >=0) {
    char *type = xctx->sym[inst->ptr].type;
    int cond= type && IS_LABEL_SH_OR_PIN(type);
    if(cond) {
      inst->flags |= PIN_OR_LABEL;
      my_strdup2(_ALLOC_ID_, &(inst->lab), get_tok_value(inst->prop_ptr,"lab",0));
    }
  }

  if(!strboolcmp(get_tok_value(inst->prop_ptr,"hide",0), "true"))
    inst->flags |= HIDE_INST;
  if(!strboolcmp(get_tok_value(inst->prop_ptr,"hide_texts",0), "true"))
    inst->flags |= HIDE_SYMBOL_TEXTS;
              
  ptr = get_tok_value(inst->prop_ptr,"spice_ignore",0);
  if(!strcmp(ptr, "short"))
    inst->flags |= SPICE_SHORT;
  else if(!strboolcmp(ptr, "true") || !strcmp(ptr, "open"))
    inst->flags |= SPICE_IGNORE;

  ptr = get_tok_value(inst->prop_ptr,"spectre_ignore",0);
  if(!strcmp(ptr, "short"))
    inst->flags |= SPECTRE_SHORT;
  else if(!strboolcmp(ptr, "true") || !strcmp(ptr, "open"))
    inst->flags |= SPECTRE_IGNORE;
  
  ptr = get_tok_value(inst->prop_ptr,"verilog_ignore",0);
  if(!strcmp(ptr, "short"))
    inst->flags |= VERILOG_SHORT;
  else if(!strboolcmp(ptr, "true") || !strcmp(ptr, "open"))
    inst->flags |= VERILOG_IGNORE;
  
  ptr = get_tok_value(inst->prop_ptr,"vhdl_ignore",0);
  if(!strcmp(ptr, "short"))
    inst->flags |= VHDL_SHORT;
  else if(!strboolcmp(ptr, "true") || !strcmp(ptr, "open"))
    inst->flags |= VHDL_IGNORE;
  
  ptr = get_tok_value(inst->prop_ptr,"tedax_ignore",0);
  if(!strcmp(ptr, "short"))
    inst->flags |= TEDAX_SHORT;
  else if(!strboolcmp(ptr, "true") || !strcmp(ptr, "open"))
    inst->flags |= TEDAX_IGNORE;
  
  ptr = get_tok_value(inst->prop_ptr,"lvs_ignore",0);
  if(!strcmp(ptr, "short"))
    inst->flags |= LVS_IGNORE_SHORT;
  else if(!strboolcmp(ptr, "true") || !strcmp(ptr, "open"))
    inst->flags |= LVS_IGNORE_OPEN;

  if(!strboolcmp(get_tok_value(inst->prop_ptr,"highlight",0), "true"))
    inst->flags |= HILIGHT_CONN;

  inst->embed = !strboolcmp(get_tok_value(inst->prop_ptr, "embed", 2), "true");

  dbg(1, "set_inst_flags: inst %s flags=%d\n", inst->instname, inst->flags);
  return 0;
}

int set_text_flags(xText *t)
{
  const char *str;
  t->flags = 0;
  t->hcenter = 0;
  t->vcenter = 0;
  t->layer = -1; /* -1 means default TEXTLAYER is to be used */
  if(t->prop_ptr) {
    my_strdup(_ALLOC_ID_, &t->font, get_tok_value(t->prop_ptr, "font", 0));
    str = get_tok_value(t->prop_ptr, "hcenter", 0);
    t->hcenter = strboolcmp(str, "true")  ? 0 : 1;
    str = get_tok_value(t->prop_ptr, "vcenter", 0);
    t->vcenter = strboolcmp(str, "true")  ? 0 : 1;
    str = get_tok_value(t->prop_ptr, "layer", 0);
    if(str[0]) t->layer = atoi(str);
    str = get_tok_value(t->prop_ptr, "slant", 0);
    t->flags |= strcmp(str, "oblique")  ? 0 : TEXT_OBLIQUE;
    t->flags |= strcmp(str, "italic")  ? 0 : TEXT_ITALIC;
    str = get_tok_value(t->prop_ptr, "weight", 0);
    t->flags |= strcmp(str, "bold")  ? 0 : TEXT_BOLD;
    str = get_tok_value(t->prop_ptr, "hide", 0);
    if(!strcmp(str, "instance")) t->flags |= HIDE_TEXT_INSTANTIATED;
    else {
      t->flags |= strboolcmp(str, "true")  ? 0 : HIDE_TEXT;
    }
    str = get_tok_value(t->prop_ptr, "name", 0);
    if(!xctx->tok_size) str = get_tok_value(t->prop_ptr, "floater", 0);
    t->flags |= xctx->tok_size ? TEXT_FLOATER : 0;
    my_strdup2(_ALLOC_ID_, &t->floater_instname, str);
  }
  return 0;
}


void reset_caches(void)
{
  int i;
  dbg(1, "reset_caches()\n");
  for(i = 0; i < xctx->instances; i++) {
    set_inst_flags(&xctx->inst[i]);
  }     
  for(i = 0; i < xctx->symbols; i++) { 
    set_sym_flags(&xctx->sym[i]);
  }     
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
        if(d->image) {
           cairo_surface_destroy(d->image);
        }
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
 my_free(_ALLOC_ID_, &xctx->schverilogprop);
 my_free(_ALLOC_ID_, &xctx->schspectreprop);
 my_free(_ALLOC_ID_, &xctx->version_string);
 if(xctx->header_text) my_free(_ALLOC_ID_, &xctx->header_text);
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
  my_free(_ALLOC_ID_, &xctx->text[i].floater_instname);
  my_free(_ALLOC_ID_, &xctx->text[i].floater_ptr);
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
 int_hash_free(&xctx->inst_name_table);
 int_hash_free(&xctx->floater_inst_table);
}

/*  xctx->n_active_layers is the total number of layers for hilights. */
void enable_layers(void)
{
  int i;
  char tmp[50];
  int en;
  xctx->n_active_layers = 0;
  for(i = 0; i< cadlayers; ++i) {
    my_snprintf(tmp, S(tmp), "enable_layer(%d)",i);
    en = tclgetboolvar(tmp);
    if(!en) xctx->enable_layer[i] = 0;
    else {
      xctx->enable_layer[i] = 1;
      if(i>=7) {
        xctx->active_layer[xctx->n_active_layers] = i;
        xctx->n_active_layers++;
      }
    }
  }
}


/* not used... */
void clear_partial_selected_wires(void)
{
  int j;
  rebuild_selected_array();
  for(j=0; j < xctx->lastsel; ++j) if(xctx->sel_array[j].type == WIRE) {
    int wire = xctx->sel_array[j].n;
    select_wire(wire, 0, 1, 1);
  }
  xctx->need_reb_sel_arr = 1;
  rebuild_selected_array();
}


/* Add wires when moving instances or wires */
int connect_by_kissing(void)
{
  xSymbol *symbol;
  int npin, i, j;
  double x0,y0, pinx0, piny0;
  int kissing, changed = 0;
  int k, ii, done_undo = 0;
  Wireentry *wptr;
  Instpinentry *iptr;
  int sqx, sqy;
  Str_hashtable coord_table = {NULL, 0}; /* hash table to add new wires at a given position only once */
  char coord[200]; /* string representation of 'x0 y0' or 'pinx0 piny0' */

  str_hash_init(&coord_table, HASHSIZE);
  rebuild_selected_array();
  k = xctx->lastsel;
  prepare_netlist_structs(0); /* rebuild spatial hashes */

  /* add wires to moving instance pins */
  for(j=0;j<k; ++j) if(xctx->sel_array[j].type==ELEMENT) {
    int inst = xctx->sel_array[j].n;
    symbol = xctx->sym + xctx->inst[inst].ptr;
    npin = symbol->rects[PINLAYER];
    for(i=0;i<npin; ++i) {
      get_inst_pin_coord(inst, i, &pinx0, &piny0);
      get_square(pinx0, piny0, &sqx, &sqy);
      iptr=xctx->instpin_spatial_table[sqx][sqy];
      wptr=xctx->wire_spatial_table[sqx][sqy];
      kissing=0;
      while(iptr) {
        ii = iptr->n;
        if(ii == inst) {
          iptr = iptr->next;
          continue;
        }
        if( iptr->x0 == pinx0 && iptr->y0 == piny0  && xctx->inst[ii].sel == 0) {
          kissing = 1;
          break;
        }
        iptr = iptr->next;
      }
      while(wptr) {
        xWire *w = &xctx->wire[wptr->n];
        if( touch(w->x1, w->y1, w->x2, w->y2, pinx0, piny0) && w->sel == 0) {
          kissing = 1;
          break;
        }
        wptr = wptr->next;
      }
      if(kissing) {
     
        if(!done_undo) {
          xctx->push_undo();
          done_undo = 1;
        }
        my_snprintf(coord, S(coord), "%.16g %.16g", pinx0, piny0);
        if (str_hash_lookup(&coord_table, coord, "", XLOOKUP)==NULL) {
          dbg(1, "connect_by_kissing(): adding wire in %g %g, wires before = %d\n", pinx0, piny0, xctx->wires);
          str_hash_lookup(&coord_table, coord, "", XINSERT);
          storeobject(-1, pinx0, piny0,  pinx0, piny0, WIRE, 0, SELECTED1, NULL);
          changed = 1;
          xctx->prep_hash_wires=0;
          xctx->need_reb_sel_arr = 1;
        }
      }
    }
  }

  /* add wires to moving wire endpoints */
  for(j=0; j < k; ++j) if(xctx->sel_array[j].type == WIRE) {
    int wire = xctx->sel_array[j].n;
    if(xctx->wire[wire].sel != SELECTED) continue; /* skip partially selected wires */
    for(i=0;i<2; ++i) {
      if(i == 0) {
        x0 = xctx->wire[wire].x1;
        y0 = xctx->wire[wire].y1;
      } else {
        x0 = xctx->wire[wire].x2;
        y0 = xctx->wire[wire].y2;
      }

      get_square(x0, y0, &sqx, &sqy);
      iptr=xctx->instpin_spatial_table[sqx][sqy];
      wptr=xctx->wire_spatial_table[sqx][sqy];
      kissing=0;
      while(iptr) {
        ii = iptr->n;
        dbg(1, "connect_by_kissing(): ii=%d, x0=%g, y0=%g,  iptr->x0=%g, iptr->y0=%g\n",
               ii, x0, y0, iptr->x0, iptr->y0);
        if( iptr->x0 == x0 && iptr->y0 == y0  &&  xctx->inst[ii].sel == 0) {
          kissing = 1;
          break;
        }
        iptr = iptr->next;
      }
      while(wptr) {
        xWire *w = &xctx->wire[wptr->n];
        if(wire == wptr->n) {
          wptr = wptr->next;
          continue;
        }
        if( touch(w->x1, w->y1, w->x2, w->y2, x0, y0) && w->sel == 0) {
          kissing = 1;
          break;
        }
        wptr = wptr->next;
      }
      if(kissing) {
        if(!done_undo) {
          xctx->push_undo();
          done_undo = 1;
        }
        my_snprintf(coord, S(coord), "%.16g %.16g", x0, y0);
        if (str_hash_lookup(&coord_table, coord, "", XLOOKUP)==NULL) {
          dbg(1, "connect_by_kissing(): adding wire in %g %g, wires before = %d\n", x0, y0, xctx->wires);
          str_hash_lookup(&coord_table, coord, "", XINSERT);
          storeobject(-1, x0, y0,  x0, y0, WIRE, 0, SELECTED1, NULL);
          changed = 1;
          xctx->prep_hash_wires=0;
          xctx->need_reb_sel_arr = 1;
        }
      }
    }
  }
  str_hash_free(&coord_table);
  rebuild_selected_array();
  return changed;
}

int unselect_partial_sel_wires(void)
{
  xSymbol *symbol;
  int npin, i, j;
  double x0,y0, pinx0, piny0;
  int changed = 0;
  int k;
  Wireentry *wptr;
  int sqx, sqy;

  if(!tclgetboolvar("unselect_partial_sel_wires")) return 0;
  rebuild_selected_array();
  k = xctx->lastsel;
  prepare_netlist_structs(0); /* rebuild spatial hashes */
  /* unselect wires attached to moved instance pins */
  for(j=0;j<k; ++j) if(xctx->sel_array[j].type==ELEMENT) {
    int inst = xctx->sel_array[j].n;
    symbol = xctx->sym + xctx->inst[inst].ptr;
    npin = symbol->rects[PINLAYER];
    for(i=0;i<npin; ++i) {
      get_inst_pin_coord(inst, i, &pinx0, &piny0);
      get_square(pinx0, piny0, &sqx, &sqy);
      wptr=xctx->wire_spatial_table[sqx][sqy];
      while(wptr) {
        xWire *w = &xctx->wire[wptr->n];
        if(touch(w->x1, w->y1, w->x2, w->y2, pinx0, piny0) && w->sel && w->sel != SELECTED) {
          select_wire(wptr->n, 0, 1, 1);
          changed = 1;
        }
        wptr = wptr->next;
      }
    }
  }
  /* unselect wires attached to moved wire endpoints */
  for(j=0; j < k; ++j) if(xctx->sel_array[j].type == WIRE) {
    int wire = xctx->sel_array[j].n;
    if(xctx->wire[wire].sel != SELECTED) continue; /* skip partially selected wires */
    for(i=0;i<2; ++i) {
      if(i == 0) {
        x0 = xctx->wire[wire].x1;
        y0 = xctx->wire[wire].y1;
      } else {
        x0 = xctx->wire[wire].x2;
        y0 = xctx->wire[wire].y2;
      }
      get_square(x0, y0, &sqx, &sqy);
      wptr=xctx->wire_spatial_table[sqx][sqy];
      while(wptr) {
        xWire *w = &xctx->wire[wptr->n];
        if(wire == wptr->n) {
          wptr = wptr->next;
          continue;
        }
        if(touch(w->x1, w->y1, w->x2, w->y2, x0, y0) && w->sel && w->sel != SELECTED) {
          xctx->wire[wptr->n].sel = 0;
          select_wire(wptr->n, 0, 1, 1);
          changed = 1;
        }
        wptr = wptr->next;
      }
    }
  }
  return changed;
}


/* interactive = 0: do not present dialog box
 * interactive = 1: present dialog box
 * interactive = 2: attach lab_show to unconnected pins, no dialog box
 */
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
  char *symname_show = NULL;
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

  my_strdup(_ALLOC_ID_, &symname_pin, tcleval("find_file_first lab_pin.sym"));
  my_strdup(_ALLOC_ID_, &symname_wire, tcleval("find_file_first lab_wire.sym"));
  my_strdup(_ALLOC_ID_, &symname_show, tcleval("find_file_first lab_show.sym"));
  if(symname_pin && symname_wire && symname_show) {
    rebuild_selected_array();
    k = xctx->lastsel;
    first_call=1;
    prepare_netlist_structs(0);
    for(j=0;j<k; ++j) if(xctx->sel_array[j].type==ELEMENT) {
      found=1;
      my_strdup(_ALLOC_ID_, &prop, xctx->inst[xctx->sel_array[j].n].instname);
      my_strcat(_ALLOC_ID_, &prop, "_");
      tclsetvar("custom_label_prefix",prop);
  
      if(interactive == 1 && !do_all_inst) {
        dbg(1,"attach_labels_to_inst(): invoking tcl attach_labels_to_inst\n");
        tcleval("attach_labels_to_inst");
        if(!strcmp(tclgetvar("tctx::rcode"),"") ) {
          bbox(END, 0., 0., 0., 0.);
          my_free(_ALLOC_ID_, &prop);
          return;
        }
      }
      if(interactive != 1 ) {
        tclsetvar("tctx::rcode", "yes");
        tclsetvar("use_lab_wire", "0");
        tclsetvar("use_label_prefix", "0");
        tclsetvar("do_all_inst", "1");
        tclsetvar("rotated_text", "0");
      }
      use_label_prefix = tclgetboolvar("use_label_prefix");
      rot_txt = tclgetvar("rotated_text");
      if(strcmp(rot_txt,"")) rotated_text=atoi(rot_txt);
      my_strdup(_ALLOC_ID_, &type,(xctx->inst[xctx->sel_array[j].n].ptr+ xctx->sym)->type);
      if( type && IS_LABEL_OR_PIN(type) ) {
        continue;
      }
      if(!do_all_inst && tclgetboolvar("do_all_inst")) do_all_inst=1;
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
           if(interactive == 2) {
             place_symbol(-1,symname_show, pinx0, piny0, rot1, dir, prop, 2, first_call, 1/*to_push_undo*/);
           } else if(!tclgetboolvar("use_lab_wire")) {
             place_symbol(-1,symname_pin, pinx0, piny0, rot1, dir, prop, 2, first_call, 1/*to_push_undo*/);
           } else {
             place_symbol(-1,symname_wire, pinx0, piny0, rot1, dir, prop, 2, first_call, 1/*to_push_undo*/);
           }
           first_call=0;
         }
         dbg(1, "attach_labels_to_inst(): %d   %.16g %.16g %s\n", i, pinx0, piny0,labname);
      }
    }
    if(first_call == 0) set_modify(1);
    my_free(_ALLOC_ID_, &prop);
    my_free(_ALLOC_ID_, &labname);
    my_free(_ALLOC_ID_, &type);
    if(!found) return;
    /*  draw things  */
    if(!first_call) {
      bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
      draw();
      bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
    }
  } else {
    fprintf(errfp, "attach_labels_to_inst(): location of schematic labels not found\n");
    tcleval("alert_ {attach_labels_to_inst(): location of schematic labels not found} {}");
  }
  /* if hilights are present in schematic propagate to new added labels */
  if(xctx->hilight_nets) {
    propagate_hilights(1, 0, XINSERT_NOREPLACE); 
    redraw_hilights(0);
  }
  my_free(_ALLOC_ID_, &symname_pin);
  my_free(_ALLOC_ID_, &symname_wire);
  my_free(_ALLOC_ID_, &symname_show);
}

void delete_files(void)
{
  char str[PATH_MAX + 100];
  rebuild_selected_array();
  if(xctx->lastsel && xctx->sel_array[0].type==ELEMENT) {
    my_snprintf(str, S(str), "delete_files {%s}",
         abs_sym_path(tcl_hook2(xctx->inst[xctx->sel_array[0].n].name), ""));
  } else {
    my_snprintf(str, S(str), "delete_files {%s}",
         abs_sym_path(xctx->sch[xctx->currsch], ""));
  }
  tcleval(str);
}

void place_net_label(int type)
{
  if(type == 1) {
      const char *lab = tcleval("find_file_first lab_pin.sym");
      place_symbol(-1, lab, xctx->mousex_snap, xctx->mousey_snap, 0, 0, NULL, 4, 1, 1/*to_push_undo*/);
  } else if(type == 0) {
      const char *lab = tcleval("find_file_first lab_wire.sym");
      place_symbol(-1, lab, xctx->mousex_snap, xctx->mousey_snap, 0, 0, NULL, 4, 1, 1/*to_push_undo*/);
  } else if(type == 2) {
      const char *lab = tcleval("find_file_first ipin.sym");
      place_symbol(-1, lab, xctx->mousex_snap, xctx->mousey_snap, 0, 0, NULL, 4, 1, 1/*to_push_undo*/);
  } else if(type == 3) {
      const char *lab = tcleval("find_file_first opin.sym");
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
 char name1[PATH_MAX];
 char tclev = 0;

 if(symbol_name==NULL) {
   tcleval("load_file_dialog {Choose symbol} *.\\{sym,tcl\\} INITIALINSTDIR");
   my_strncpy(name1, tclresult(), S(name1));
 } else {
   my_strncpy(name1, abs_sym_path(trim_chars(symbol_name, " \t\n"), ""), S(name1));
 }
 if(!name1[0]) return 0;
 dbg(1, "place_symbol(): 1: name1=%s first_call=%d\n",name1, first_call);
 /* remove tcleval( given in file selector, if any ... */
 if(strstr(name1, "tcleval(")) {
   tclev = 1;
   my_snprintf(name1, S(name1), "%s", str_replace(name1, "tcleval(", "", 0, -1));
 }
 dbg(1, "place_symbol(): 2: name1=%s\n",name1);

 tclvareval("is_xschem_file {", name1, "}", NULL);
 if(!strcmp(tclresult(), "GENERATOR")) {
   size_t len = strlen(name1);
   if( name1[len - 1] != ')') my_snprintf(name, S(name), "%s()", name1);
   else my_strncpy(name, name1, S(name));
 } else {
   my_strncpy(name, name1, S(name));
 }
 my_strncpy(name1, rel_sym_path(name), S(name1));
 /* ... and re-add tcleval( around relative path symbol name */
 if(tclev) {
   my_snprintf(name, S(name), "tcleval(%s", name1);
 } else {
   my_strncpy(name, name1, S(name));
 }
 if(name[0]) {
   if(first_call && to_push_undo) xctx->push_undo();
 } else  return 0;
 i=match_symbol(name);

 if(i!=-1)
 {
  if(first_call) hash_names(-1, XINSERT);
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
  xctx->inst[n].instname=NULL;
  dbg(1, "place_symbol() :all inst_ptr members set\n");  /*  03-02-2000 */
  if(inst_props) {
    new_prop_string(n, inst_props, tclgetboolvar("disable_unique_names")); /*  20171214 first_call */
  }
  else {
    set_inst_prop(n); /* no props, get from sym template, also calls new_prop_string() */
  }
  dbg(1, "place_symbol(): done set_inst_prop()\n");  /*  03-02-2000 */

  
  xctx->instances++;/* translate expects the correct balue of xctx->instances */
  /* After having assigned prop_ptr to new instance translate symbol reference
   * to resolve @params  --> res.tcl(@value\) --> res.tcl(100) */
  my_strncpy(name, translate(n, name), S(name));
  /* parameters like res.tcl(@value\) have been resolved, so reload symbol removing previous */
  if(strcmp(name, name1)) {
    remove_symbol(i);
    i = match_symbol(name);
  }
  xctx->inst[n].ptr = i;
  set_inst_flags(&xctx->inst[n]);
  hash_names(n, XINSERT);
  if(first_call && (draw_sym & 3) ) bbox(START, 0.0 , 0.0 , 0.0 , 0.0);
  /* force these vars to 0 to trigger a prepare_netlist_structs(0) needed by symbol_bbox->translate
   * to translate @#n:net_name texts */
  xctx->prep_net_structs=0;
  xctx->prep_hi_structs=0;
  symbol_bbox(n, &xctx->inst[n].x1, &xctx->inst[n].y1,
                    &xctx->inst[n].x2, &xctx->inst[n].y2);
  if(xctx->prep_hash_inst) hash_inst(XINSERT, n); /* no need to rehash, add item */
  /* xctx->prep_hash_inst=0; */

  /* embed a (locked) graph object floater inside the symbol */
  if(xctx->sym[i].type && !strcmp(xctx->sym[i].type, "scope")) {
    char *prop = NULL;

    my_strdup(_ALLOC_ID_, &xctx->inst[n].prop_ptr,
          subst_token(xctx->inst[n].prop_ptr, "attach", xctx->inst[n].instname));
    my_mstrcat(_ALLOC_ID_, &prop, "name=", xctx->inst[n].instname, "\n", NULL);
    my_mstrcat(_ALLOC_ID_, &prop, "flags=graph,unlocked\n", NULL);
    my_mstrcat(_ALLOC_ID_, &prop, "lock=1\n", NULL);
    my_mstrcat(_ALLOC_ID_, &prop, "color=8\n", NULL);
    if(xctx->sym[i].rects[PINLAYER] == 0) {
      if(xctx->lastsel == 1 && xctx->sel_array[0].type==ELEMENT) {
        my_mstrcat(_ALLOC_ID_, &prop, "node=\"tcleval([xschem get_fqdevice [xschem translate ",
                                       xctx->inst[n].instname, " @device]])\"\n", NULL);
        my_strdup(_ALLOC_ID_, &xctx->inst[n].prop_ptr,
            subst_token(xctx->inst[n].prop_ptr, "device", xctx->inst[xctx->sel_array[0].n].instname));
      } else {
        const char msg[]="scope_ammeter is being inserted but no selected ammeter device/vsource to link to\n";
        dbg(0, "%s", msg);
        if(has_x) tclvareval("alert_ {", msg, "} {} 1", NULL);
        #if 1
        if(xctx->inst[n].instname) my_free(_ALLOC_ID_, &xctx->inst[n].instname);       
        if(xctx->inst[n].name) my_free(_ALLOC_ID_, &xctx->inst[n].name);       
        if(xctx->inst[n].prop_ptr) my_free(_ALLOC_ID_, &xctx->inst[n].prop_ptr);       
        if(xctx->inst[n].lab) my_free(_ALLOC_ID_, &xctx->inst[n].lab);       
        if(prop) my_free(_ALLOC_ID_, &prop);
        xctx->instances--;
        return 0;
        #endif
      }
    } else if(xctx->sym[i].rects[PINLAYER] == 1) {
      my_mstrcat(_ALLOC_ID_, &prop,
        "node=\"tcleval(",
        "[xschem translate ", xctx->inst[n].instname, " @#0:net_name]",
        ")\"\n", NULL);
    } else {
      my_mstrcat(_ALLOC_ID_, &prop,
        "node=\"tcleval(",
        "[xschem translate ", xctx->inst[n].instname, " @#0:net_name] ",
        "[xschem translate ", xctx->inst[n].instname, " @#1:net_name] -",
        ")\"\n", NULL);
    }
    storeobject(-1, x + 20, y-125, x + 130 , y - 25, xRECT, 2, SELECTED, prop);
    my_free(_ALLOC_ID_, &prop);
  }

  if(draw_sym & 3) {
    bbox(ADD, xctx->inst[n].x1, xctx->inst[n].y1, xctx->inst[n].x2, xctx->inst[n].y2);
  }
  if(draw_sym&1) {
    bbox(SET , 0.0 , 0.0 , 0.0 , 0.0);
    draw();
    bbox(END , 0.0 , 0.0 , 0.0 , 0.0);
  }
  /*   hilight new element 24122002 */

  if(draw_sym & 4 ) {
    unselect_all(1);
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
    if(tclgetboolvar("search_schematic")) {
      my_strncpy(filename, abs_sym_path(xctx->current_name, ".sym"), S(filename));
    } else {
      my_strncpy(filename, add_ext(xctx->sch[xctx->currsch], ".sym"), S(filename));
    }
    if(new_process) new_xschem_process(filename, 1);
    else new_schematic("create", NULL, filename, 1);
  }
  else {
    my_strncpy(filename, abs_sym_path(tcl_hook2(xctx->inst[xctx->sel_array[0].n].name), ""), S(filename));
    if(!check_loaded(filename, win_path)) {
      if(new_process) new_xschem_process(filename, 1);
      else new_schematic("create", NULL, filename, 1);
    }
  }
}

int copy_hierarchy_data(const char *from_win_path, const char *to_win_path)
{
  int n;
  Xschem_ctx **save_xctx;
  Xschem_ctx *from, *to;
  char **sch;
  char **sch_path;
  int *sch_path_hash;
  int *sch_inst_number;
  int *previous_instance;
  Zoom *zoom_array;
  Lcc *hier_attr;
  int i, j;
  Str_hashentry **fromnext;
  Str_hashentry **tonext;

  
  if(!get_window_count()) { return 0; }
  save_xctx = get_save_xctx();
  n = get_tab_or_window_number(from_win_path);
  if(n >= 0) {
    from = save_xctx[n];
  } else return 0;
  n = get_tab_or_window_number(to_win_path);
  if(n >= 0) {
    to = save_xctx[n];
  } else return 0;
  sch = from->sch;
  sch_path = from->sch_path;
  sch_path_hash = from->sch_path_hash;
  sch_inst_number = from->sch_inst_number;
  previous_instance = from->previous_instance;
  zoom_array = from->zoom_array;
  hier_attr = from->hier_attr;
  to->currsch = from->currsch;
  for(i = 0; i <= from->currsch; i++) {
    my_strdup2(_ALLOC_ID_, &to->sch[i], sch[i]);
    my_strdup2(_ALLOC_ID_, &to->sch_path[i], sch_path[i]);
    to->sch_path_hash[i] = sch_path_hash[i];
    to->sch_inst_number[i] = sch_inst_number[i];
    to->previous_instance[i] = previous_instance[i];
    to->zoom_array[i].x = zoom_array[i].x;
    to->zoom_array[i].y = zoom_array[i].y;
    to->zoom_array[i].zoom = zoom_array[i].zoom;
    to->hier_attr[i].x0 = hier_attr[i].x0;
    to->hier_attr[i].y0 = hier_attr[i].y0;
    to->hier_attr[i].rot = hier_attr[i].rot;
    to->hier_attr[i].flip = hier_attr[i].flip;
    to->hier_attr[i].fd = NULL; /* Never used outside load_sym_def() */
    my_strdup2(_ALLOC_ID_, &to->hier_attr[i].prop_ptr, hier_attr[i].prop_ptr);
    my_strdup2(_ALLOC_ID_, &to->hier_attr[i].templ, hier_attr[i].templ);
    my_strdup2(_ALLOC_ID_, &to->hier_attr[i].symname, hier_attr[i].symname);
    my_strdup2(_ALLOC_ID_, &to->hier_attr[i].sym_extra, hier_attr[i].sym_extra);
    if(to->portmap[i].table) str_hash_free(&to->portmap[i]);
    str_hash_init(&to->portmap[i], HASHSIZE);
    for(j = 0; j < HASHSIZE; j++) {
      if(!from->portmap[i].table || !from->portmap[i].table[j]) continue;
      fromnext = &(from->portmap[i].table[j]);
      tonext =  &(to->portmap[i].table[j]);
      while(*fromnext) {
        Str_hashentry *e;
        e = my_calloc(_ALLOC_ID_, 1, sizeof(Str_hashentry));
        e->hash = (*fromnext)->hash;
        my_strdup2(_ALLOC_ID_, &e->token, (*fromnext)->token);
        my_strdup2(_ALLOC_ID_, &e->value, (*fromnext)->value);
        *tonext = e;
        fromnext = &( (*fromnext)->next );
        tonext = &( (*tonext)->next );
      }
    }
  }
  return 1;
}

/*  20111007 duplicate current schematic if no inst selected */
/* if force set to 1 force opening another new schematic even if already open */
int schematic_in_new_window(int new_process, int dr, int force)
{
  char filename[PATH_MAX];
  char win_path[WINDOW_PATH_SIZE];
  rebuild_selected_array();
  if(xctx->lastsel == 0) {
    if(new_process) new_xschem_process(xctx->sch[xctx->currsch], 0);
    else {
      int gf = xctx->graph_flags;
      double c1 = xctx->graph_cursor1_x;
      double c2 = xctx->graph_cursor2_x;
      new_schematic("create", force ? "noalert" : "", xctx->sch[xctx->currsch], dr);

      /* propagte raw cursor info to new window */
      xctx->graph_flags = gf;
      xctx->graph_cursor1_x = c1;
      xctx->graph_cursor2_x = c2;
      dbg(1, "path=%s\n", xctx->current_win_path);
    }
    return 1;
  }
  else if(xctx->lastsel > 1) {
    return 0;
  }
  else { /* xctx->lastsel == 1 */
    if(xctx->inst[xctx->sel_array[0].n].ptr < 0 ) return 0;
    if(!(xctx->inst[xctx->sel_array[0].n].ptr+ xctx->sym)->type) return 0;
    if(xctx->sel_array[0].type != ELEMENT) return 0;
    if(                   /*  do not descend if not subcircuit */
       strcmp(
          (xctx->inst[xctx->sel_array[0].n].ptr+ xctx->sym)->type,
           "subcircuit"
       ) &&
       strcmp(
          (xctx->inst[xctx->sel_array[0].n].ptr+ xctx->sym)->type,
           "primitive"
       )
    ) return 0;
    get_sch_from_sym(filename, xctx->inst[xctx->sel_array[0].n].ptr+ xctx->sym, xctx->sel_array[0].n, 0);
    if(force || !check_loaded(filename, win_path)) {
      if(new_process) new_xschem_process(filename, 0);
      else new_schematic("create", "noalert", filename, dr);
    }
  }
  return 1;
}

void launcher(void)
{
  const char *url;
  char program[PATH_MAX];
  char *command = NULL;
  int n, c;
  char *prop_ptr=NULL;
  rebuild_selected_array();
  tcleval("update");
  if(xctx->lastsel ==1)
  {
    double mx=xctx->mousex, my=xctx->mousey;
    n=xctx->sel_array[0].n;
    c=xctx->sel_array[0].col;
    if     (xctx->sel_array[0].type==ELEMENT) prop_ptr = xctx->inst[n].prop_ptr;
    else if(xctx->sel_array[0].type==xRECT)   prop_ptr = xctx->rect[c][n].prop_ptr;
    else if(xctx->sel_array[0].type==POLYGON) prop_ptr = xctx->poly[c][n].prop_ptr;
    else if(xctx->sel_array[0].type==ARC)     prop_ptr = xctx->arc[c][n].prop_ptr;
    else if(xctx->sel_array[0].type==LINE)    prop_ptr = xctx->line[c][n].prop_ptr;
    else if(xctx->sel_array[0].type==WIRE)    prop_ptr = xctx->wire[n].prop_ptr;
    else if(xctx->sel_array[0].type==xTEXT)   prop_ptr = xctx->text[n].prop_ptr;
    my_strdup2(_ALLOC_ID_, &command, get_tok_value(prop_ptr,"tclcommand",0));
    my_strncpy(program, get_tok_value(prop_ptr,"program",0), S(program)); /* handle backslashes */
    url = get_tok_value(prop_ptr,"url",0); /* handle backslashes */
    dbg(1, "launcher(): url=%s\n", url);
    if(url[0] || (program[0])) { /* open url with appropriate program */
      tclvareval("launcher {", url, "} {", program, "}", NULL);
    } else if(command && command[0]){
      if(Tcl_GlobalEval(interp, command) != TCL_OK) {
        dbg(0, "%s\n", tclresult());
        if(has_x) tclvareval("alert_ {", tclresult(), "} {}", NULL);
        Tcl_ResetResult(interp);
      }
    } else { /* no action defined --> warning */
      const char *msg = "No action on launcher is defined (url or tclcommand)";
      dbg(0, "%s\n", msg);
      /* if(has_x) tclvareval("alert_ {", msg, "} {}", NULL); */ /* commented, annoying */
    }
    my_free(_ALLOC_ID_, &command);
    tcleval("after 300");
    select_object(mx,my,0, 0, NULL);
  }
}


/* get symbol reference of instance 'inst', looking into 
 * instance 'schematic' attribute (and appending '.sym') if set
 * or get it from inst[inst].name.
 * perform tcl substitution of the result and
 * return the last 'ndir' directory components of symbol reference. */
const char *get_sym_name(int inst, int ndir, int ext, int abs_path)
{
  const char *sym, *sch;

  /* instance based symbol selection */
  sch = tcl_hook2(str_replace(get_tok_value(xctx->inst[inst].prop_ptr,"schematic", 6), "@symname",
        get_cell(xctx->inst[inst].name, 0), '\\', -1));

  dbg(1, "get_sym_name(): sch=%s\n", sch);
  if(xctx->tok_size) { /* token exists */ 
    if(abs_path)
      sym = abs_sym_path(sch, ".sym");
    else
      sym = add_ext(rel_sym_path(sch), ".sym");
  } 
  else {
    if(abs_path)
      sym = abs_sym_path(tcl_hook2(xctx->inst[inst].name), "");
    else
      sym = tcl_hook2(xctx->inst[inst].name);
  }

  if(ext) return get_cell_w_ext(sym, ndir);
  else return get_cell(sym, ndir);
}

void copy_symbol(xSymbol *dest_sym, xSymbol *src_sym)
{
  int c, j;
  
  dest_sym->minx = src_sym->minx;
  dest_sym->maxx = src_sym->maxx;
  dest_sym->miny = src_sym->miny;
  dest_sym->maxy = src_sym->maxy;
  dest_sym->flags = src_sym->flags;
  dest_sym->texts = src_sym->texts;

  dest_sym->name = NULL;
  dest_sym->base_name = NULL; /* this is not allocated and points to the base symbol */
  dest_sym->prop_ptr = NULL;
  dest_sym->type = NULL;
  dest_sym->templ = NULL;
  dest_sym->parent_prop_ptr = NULL;
  my_strdup2(_ALLOC_ID_, &dest_sym->name, src_sym->name);
  my_strdup2(_ALLOC_ID_, &dest_sym->type, src_sym->type);
  my_strdup2(_ALLOC_ID_, &dest_sym->templ, src_sym->templ);
  my_strdup(_ALLOC_ID_, &dest_sym->parent_prop_ptr, src_sym->parent_prop_ptr);
  my_strdup2(_ALLOC_ID_, &dest_sym->prop_ptr, src_sym->prop_ptr);

  dest_sym->line = my_calloc(_ALLOC_ID_, cadlayers, sizeof(xLine *));
  dest_sym->poly = my_calloc(_ALLOC_ID_, cadlayers, sizeof(xPoly *));
  dest_sym->arc = my_calloc(_ALLOC_ID_, cadlayers, sizeof(xArc *));
  dest_sym->rect = my_calloc(_ALLOC_ID_, cadlayers, sizeof(xRect *));
  dest_sym->lines = my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  dest_sym->rects = my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  dest_sym->arcs = my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  dest_sym->polygons = my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));

  dest_sym->text = my_calloc(_ALLOC_ID_, src_sym->texts, sizeof(xText));
  memcpy(dest_sym->lines, src_sym->lines, sizeof(dest_sym->lines[0]) * cadlayers);
  memcpy(dest_sym->rects, src_sym->rects, sizeof(dest_sym->rects[0]) * cadlayers);
  memcpy(dest_sym->arcs, src_sym->arcs, sizeof(dest_sym->arcs[0]) * cadlayers);
  memcpy(dest_sym->polygons, src_sym->polygons, sizeof(dest_sym->polygons[0]) * cadlayers);
  for(c = 0;c<cadlayers; ++c) {
    /* symbol lines */
    dest_sym->line[c] = my_calloc(_ALLOC_ID_, src_sym->lines[c], sizeof(xLine));
    for(j = 0; j < src_sym->lines[c]; ++j) { 
      dest_sym->line[c][j] = src_sym->line[c][j];
      dest_sym->line[c][j].prop_ptr = NULL;
      my_strdup(_ALLOC_ID_, &dest_sym->line[c][j].prop_ptr, src_sym->line[c][j].prop_ptr);
    }
    /* symbol rects */
    dest_sym->rect[c] = my_calloc(_ALLOC_ID_, src_sym->rects[c], sizeof(xRect));
    for(j = 0; j < src_sym->rects[c]; ++j) { 
      dest_sym->rect[c][j] = src_sym->rect[c][j];
      dest_sym->rect[c][j].prop_ptr = NULL;
      dest_sym->rect[c][j].extraptr = NULL;
      my_strdup(_ALLOC_ID_, &dest_sym->rect[c][j].prop_ptr, src_sym->rect[c][j].prop_ptr);
    }
    /* symbol arcs */
    dest_sym->arc[c] = my_calloc(_ALLOC_ID_, src_sym->arcs[c], sizeof(xArc));
    for(j = 0; j < src_sym->arcs[c]; ++j) { 
      dest_sym->arc[c][j] = src_sym->arc[c][j];
      dest_sym->arc[c][j].prop_ptr = NULL;
      my_strdup(_ALLOC_ID_, &dest_sym->arc[c][j].prop_ptr, src_sym->arc[c][j].prop_ptr);
    }
    /* symbol polygons */
    dest_sym->poly[c] = my_calloc(_ALLOC_ID_, src_sym->polygons[c], sizeof(xPoly));
    for(j = 0; j < src_sym->polygons[c]; ++j) { 
      int points = src_sym->poly[c][j].points;
      dest_sym->poly[c][j] = src_sym->poly[c][j];
      dest_sym->poly[c][j].prop_ptr = NULL;
      dest_sym->poly[c][j].x = my_malloc(_ALLOC_ID_, points * sizeof(double));
      dest_sym->poly[c][j].y = my_malloc(_ALLOC_ID_, points * sizeof(double));
      dest_sym->poly[c][j].selected_point = my_malloc(_ALLOC_ID_, points * sizeof(unsigned short));
      my_strdup(_ALLOC_ID_, &dest_sym->poly[c][j].prop_ptr, src_sym->poly[c][j].prop_ptr);
      memcpy(dest_sym->poly[c][j].x, src_sym->poly[c][j].x, points * sizeof(double));
      memcpy(dest_sym->poly[c][j].y, src_sym->poly[c][j].y, points * sizeof(double));
      memcpy(dest_sym->poly[c][j].selected_point, src_sym->poly[c][j].selected_point,
           points * sizeof(unsigned short));
    }
  }
  /* symbol texts */
  for(j = 0; j < src_sym->texts; ++j) {
    dest_sym->text[j] = src_sym->text[j];
    dest_sym->text[j].prop_ptr = NULL;
    dest_sym->text[j].txt_ptr = NULL;
    dest_sym->text[j].font = NULL;
    dest_sym->text[j].floater_instname = NULL;
    dest_sym->text[j].floater_ptr = NULL;
    my_strdup2(_ALLOC_ID_, &dest_sym->text[j].prop_ptr, src_sym->text[j].prop_ptr);
    my_strdup2(_ALLOC_ID_, &dest_sym->text[j].floater_ptr, src_sym->text[j].floater_ptr);
    dbg(1, "copy_symbol1(): allocating sym %d text %d\n", dest_sym - xctx->sym, j);
    my_strdup2(_ALLOC_ID_, &dest_sym->text[j].txt_ptr, src_sym->text[j].txt_ptr);
    my_strdup2(_ALLOC_ID_, &dest_sym->text[j].font, src_sym->text[j].font);
    my_strdup2(_ALLOC_ID_, &dest_sym->text[j].floater_instname, src_sym->text[j].floater_instname);
  }
}   

void toggle_ignore(void)
{
  int i, n, first = 1;
  char *attr;
  int flag = 0; /* 1: spice_ignore=true, 2: spice_ignore=short */
  const char *ignore_str;
  if(xctx->netlist_type == CAD_VERILOG_NETLIST) attr="verilog_ignore";
  else if(xctx->netlist_type == CAD_VHDL_NETLIST) attr="vhdl_ignore";
  else if(xctx->netlist_type == CAD_TEDAX_NETLIST) attr="tedax_ignore";
  else if(xctx->netlist_type == CAD_SPECTRE_NETLIST) attr="spectre_ignore";
  else if(xctx->netlist_type == CAD_SPICE_NETLIST) attr="spice_ignore";
  else attr = NULL;
  if(attr) {
    rebuild_selected_array();
    for(n=0; n < xctx->lastsel; ++n) {
      if(xctx->sel_array[n].type == ELEMENT) {
        i = xctx->sel_array[n].n;
        if(first) {
          xctx->push_undo();
          first = 0;
        }
        flag = 0;
        ignore_str = get_tok_value(xctx->inst[i].prop_ptr, attr, 0);
        if(!strcmp(ignore_str, "short")) flag = 2;
        else if(!strboolcmp(ignore_str, "true")) flag = 1;

        if(flag == 0) flag = 1;
        else if(flag == 1) flag = 2;
        else flag = 0;

      
        if(flag == 1) {
          my_strdup(_ALLOC_ID_, &xctx->inst[i].prop_ptr, subst_token(xctx->inst[i].prop_ptr, attr, "true"));
        } else if(flag == 2) {
          my_strdup(_ALLOC_ID_, &xctx->inst[i].prop_ptr, subst_token(xctx->inst[i].prop_ptr, attr, "short"));
        } else {
          my_strdup(_ALLOC_ID_, &xctx->inst[i].prop_ptr, subst_token(xctx->inst[i].prop_ptr, attr, NULL));
        }
        set_inst_flags(&xctx->inst[i]);
        set_modify(1);
        xctx->prep_hash_inst=0;
        xctx->prep_net_structs=0;
        xctx->prep_hi_structs=0;
      }
    }
    draw();
  }
}

/* what = 1: start
 * what = 0 : end : should NOT be called if match_symbol() has been executed between start & end
 */
void get_additional_symbols(int what)
{
  int i;
  static int num_syms; /* no context switch between start and end so it is safe */
  Int_hashentry *found;
  Int_hashtable sym_table = {NULL, 0};
  struct stat buf;
  int is_gen = 0;

  if(what == 1) { /* start */
    int_hash_init(&sym_table, HASHSIZE);
    num_syms = xctx->symbols;
    for(i = 0; i < xctx->symbols; ++i) {
      int_hash_lookup(&sym_table, xctx->sym[i].name, i, XINSERT);
    }
    /* handle instances with "schematic=..." attribute (polymorphic symbols) */
    for(i=0;i<xctx->instances; ++i) {
      char *spice_sym_def = NULL;
      char *vhdl_sym_def = NULL;
      char *verilog_sym_def = NULL;
      char *spectre_sym_def = NULL;
      char *default_schematic = NULL;
      char *sch = NULL;
      char symbol_base_sch[PATH_MAX] = "";
      size_t schematic_token_found = 0;
      
      if(xctx->inst[i].ptr < 0) continue;
      dbg(1, "get_additional_symbols(): inst=%d (%s) sch=%s\n",i, xctx->inst[i].name,  sch);
      /* copy instance based *_sym_def attributes to symbol */
      my_strdup(_ALLOC_ID_, &spice_sym_def, get_tok_value(xctx->inst[i].prop_ptr,"spice_sym_def",6));
      my_strdup(_ALLOC_ID_, &spectre_sym_def, get_tok_value(xctx->inst[i].prop_ptr,"spectre_sym_def",6));
      my_strdup(_ALLOC_ID_, &verilog_sym_def, get_tok_value(xctx->inst[i].prop_ptr,"verilog_sym_def",4));
      my_strdup(_ALLOC_ID_, &vhdl_sym_def, get_tok_value(xctx->inst[i].prop_ptr,"vhdl_sym_def",4));

      /* resolve schematic=generator.tcl( @n ) where n=11 is defined in instance attrs */
      my_strdup2(_ALLOC_ID_, &sch, get_tok_value(xctx->inst[i].prop_ptr,"schematic", 6));
      dbg(1, "get_additional_symbols(): schematic=%s\n", sch);
      schematic_token_found = xctx->tok_size;
      my_strdup2(_ALLOC_ID_, &sch, translate3(sch, 1, xctx->inst[i].prop_ptr, NULL, NULL, NULL));
      dbg(1, "get_additional_symbols(): sch=%s tok_size= %ld\n", sch, xctx->tok_size);
    
      my_strdup2(_ALLOC_ID_, &sch, tcl_hook2(
         str_replace(sch, "@symname", get_cell(xctx->inst[i].name, 0), '\\', -1)));

      /* schematic does not exist */
      if(sch[0] && stat(abs_sym_path(sch, ""), &buf)) {
        my_snprintf(symbol_base_sch, PATH_MAX, "%s.sch", get_cell(xctx->sym[xctx->inst[i].ptr].name, 9999));
        dbg(1, "get_additional_symbols(): schematic not existing\n");
        dbg(1, "using: %s\n", symbol_base_sch);
      }
      if(schematic_token_found && sch[0]) { /* `schematic` token exists  and a schematic is specified */
        int j;
        char *sym = NULL;
        char *symname_attr = NULL;
        int ignore_schematic = 0;
        xSymbol *symptr = xctx->inst[i].ptr + xctx->sym;

        my_strdup2(_ALLOC_ID_, &default_schematic, get_tok_value(symptr->prop_ptr,"default_schematic",0));
        ignore_schematic = !strcmp(default_schematic, "ignore");

        dbg(1, "get_additional_symbols(): inst=%d, sch=%s instname=%s\n", i, sch, xctx->inst[i].instname);
        dbg(1, "get_additional_symbols(): current_name=%s\n", xctx->current_name);

        is_gen = is_generator(sch);

        if(is_gen) {
          my_strdup2(_ALLOC_ID_, &sym, sch);
          dbg(1, "get_additional_symbols(): generator\n");
        } else {
          my_strdup2(_ALLOC_ID_, &sym, add_ext(rel_sym_path(sch), ".sym"));
        }

        my_mstrcat(_ALLOC_ID_, &symname_attr, "symname=", get_cell(sym, 0), NULL);
        my_mstrcat(_ALLOC_ID_, &symname_attr, " symref=", get_sym_name(i, 9999, 1, 1), NULL);
        my_strdup(_ALLOC_ID_, &spice_sym_def, 
            translate3(spice_sym_def, 1, xctx->inst[i].prop_ptr,
                                         symptr->templ, 
                                         symname_attr, NULL));
        my_strdup(_ALLOC_ID_, &spectre_sym_def, 
            translate3(spectre_sym_def, 1, xctx->inst[i].prop_ptr,
                                         symptr->templ, 
                                         symname_attr, NULL));
        my_free(_ALLOC_ID_, &symname_attr);
        /* if instance symbol has default_schematic set to ignore copy the symbol anyway, since
         * the base symbol will not be netlisted by *_block_netlist() */
        found = ignore_schematic ? NULL : int_hash_lookup(&sym_table, sym, 0, XLOOKUP);
        if(!found) {
          j = xctx->symbols;
          int_hash_lookup(&sym_table, sym, j, XINSERT);
          dbg(1, "get_additional_symbols(): adding symbol %s\n", sym);
          check_symbol_storage();
          copy_symbol(&xctx->sym[j], symptr);
          xctx->sym[j].base_name = symptr->name;
          my_strdup(_ALLOC_ID_, &xctx->sym[j].name, sym);

          my_strdup(_ALLOC_ID_, &xctx->sym[j].parent_prop_ptr, xctx->inst[i].prop_ptr);
          /* the copied symbol will not inherit the default_schematic attribute otherwise it will also
           * be skipped */
          if(default_schematic) {
            my_strdup(_ALLOC_ID_, &xctx->sym[j].prop_ptr, 
              subst_token(xctx->sym[j].prop_ptr, "default_schematic", NULL)); /* delete attribute */
          }
          /* if symbol has no corresponding schematic file use symbol base schematic */
          if(!is_gen && symbol_base_sch[0]) {
            my_strdup(_ALLOC_ID_, &xctx->sym[j].prop_ptr,
              subst_token(xctx->sym[j].prop_ptr, "schematic", symbol_base_sch));
          }
          if(spice_sym_def) {
             my_strdup(_ALLOC_ID_, &xctx->sym[j].prop_ptr, 
               subst_token(xctx->sym[j].prop_ptr, "spice_sym_def", spice_sym_def));
          }
          if(spectre_sym_def) {
             my_strdup(_ALLOC_ID_, &xctx->sym[j].prop_ptr, 
               subst_token(xctx->sym[j].prop_ptr, "spectre_sym_def", spectre_sym_def));
          }
          if(verilog_sym_def) {
             my_strdup(_ALLOC_ID_, &xctx->sym[j].prop_ptr,
               subst_token(xctx->sym[j].prop_ptr, "verilog_sym_def", verilog_sym_def));
          }
          if(vhdl_sym_def) {
             my_strdup(_ALLOC_ID_, &xctx->sym[j].prop_ptr,
               subst_token(xctx->sym[j].prop_ptr, "vhdl_sym_def", vhdl_sym_def));
          }
          xctx->symbols++;
        } else {
         j = found->value;
        }
        my_free(_ALLOC_ID_, &sym);
        my_free(_ALLOC_ID_, &default_schematic);
      } /* if(xctx->tok_size && sch[0]) */
      my_free(_ALLOC_ID_, &sch);
      my_free(_ALLOC_ID_, &spice_sym_def);
      my_free(_ALLOC_ID_, &spectre_sym_def);
      my_free(_ALLOC_ID_, &vhdl_sym_def);
      my_free(_ALLOC_ID_, &verilog_sym_def);
    } /* for(i=0;i<xctx->instances; ++i) */
    int_hash_free(&sym_table);
  } else { /* end */
    for(i = xctx->symbols - 1; i >= num_syms; --i) {
      remove_symbol(i);
    }
    xctx->symbols = num_syms;
  }
}
/* fallback = 1: if schematic attribute is set but file not existing fallback
 * to defaut symbol schematic (symname.sym -> symname.sch)
 * if inst == -1 use only symbol reference */
void get_sch_from_sym(char *filename, xSymbol *sym, int inst, int fallback)
{
  char *sch = NULL;
  char *str_tmp = NULL;
  int web_url = 0;
  struct stat buf;
  int file_exists=0;
  int cancel = 0;
  int is_gen = 0;

  my_strncpy(filename, "", PATH_MAX);

  if(inst >= xctx->instances) {
    dbg(0, "get_sch_from_sym() error: called with invalid inst=%d\n", inst);
    return;
  }

  if(!sym) {
    dbg(0, "get_sch_from_sym() error: called with NULL sym", inst);
    return;
  }

  /* get sch/sym name from parent schematic downloaded from web */
  if(is_from_web(xctx->current_dirname)) {
    web_url = 1;
  }

  dbg(1, "get_sch_from_sym(): current_dirname= %s\n", xctx->current_dirname);
  dbg(1, "get_sch_from_sym(): symbol %s inst=%d web_url=%d\n", sym->name, inst, web_url);
  /* resolve schematic=generator.tcl( @n ) where n=11 is defined in instance attrs */
  if(inst >=0 ) {
    my_strdup(_ALLOC_ID_, &str_tmp, translate3(get_tok_value(xctx->inst[inst].prop_ptr,"schematic", 6),
              1, xctx->inst[inst].prop_ptr, NULL, NULL, NULL));
  }
  if(!str_tmp) my_strdup2(_ALLOC_ID_, &str_tmp,  get_tok_value(sym->prop_ptr, "schematic", 6));
  if(str_tmp[0]) { /* schematic attribute in symbol or instance was given */
    /* @symname in schematic attribute will be replaced with symbol name */
    my_strdup2(_ALLOC_ID_, &sch, tcl_hook2(str_replace(str_tmp, "@symname",
       get_cell(sym->name, 0), '\\', -1)));
    if(is_generator(sch)) { /* generator: return as is */
      my_strncpy(filename, sch, PATH_MAX);
      is_gen = 1;
      dbg(1, "get_sch_from_sym(): filename=%s\n", filename);
    } else { /* not generator */
      dbg(1, "get_sch_from_sym(): after tcl_hook2 sch=%s\n", sch);
      /* for schematics referenced from web symbols do not build absolute path */
      if(web_url) my_strncpy(filename, sch, PATH_MAX);
      else my_strncpy(filename, abs_sym_path(sch, ""), PATH_MAX);
    }
  }
  
  if(has_x && fallback && !is_gen && filename[0]) {
    file_exists = !stat(filename, &buf);
    if(!file_exists) {
      tclvareval("ask_save {Schematic ", filename, "\ndoes not exist.\nDescend into base schematic?}", NULL);
      if(strcmp(tclresult(), "yes") ) fallback = 0; /* 'no' or 'cancel' */
       if(!strcmp(tclresult(), "") ) { /* 'cancel' */
         cancel = 1;
       }
    }
  }

  /* no schematic attr from instance or symbol */
  if(!cancel && (!str_tmp[0] || (fallback && !is_gen && filename[0] && !file_exists ))) {
    const char *symname_tcl = tcl_hook2(sym->name);
    if(is_generator(symname_tcl))  my_strncpy(filename, symname_tcl, PATH_MAX);
    else if(tclgetboolvar("search_schematic")) {
      /* for schematics referenced from web symbols do not build absolute path */
      if(web_url) my_strncpy(filename, add_ext(sym->name, ".sch"), PATH_MAX);
      else my_strncpy(filename, abs_sym_path(sym->name, ".sch"), PATH_MAX);
    } else {
      /* for schematics referenced from web symbols do not build absolute path */
      if(web_url) my_strncpy(filename, add_ext(sym->name, ".sch"), PATH_MAX);
      else {
        if(!stat(abs_sym_path(sym->name, ""), &buf)) /* symbol exists. pretend schematic exists too ... */
          my_strncpy(filename, add_ext(abs_sym_path(sym->name, ""), ".sch"), PATH_MAX);
        else /* ... symbol does not exist (instances with schematic=... attr) so can not pretend that */
          my_strncpy(filename, abs_sym_path(sym->name, ".sch"), PATH_MAX);
      }
    }
  }
  if(sch) my_free(_ALLOC_ID_, &sch);

  if(web_url && filename[0] && xschem_web_dirname[0]) {
    char sympath[PATH_MAX];

    /* build local cached filename of web_url */
    my_snprintf(sympath, S(sympath), "%s/%s",  xschem_web_dirname, get_cell_w_ext(filename, 0));
    if(stat(sympath, &buf)) { /* not found, download */
      /* download item into ${XSCHEM_TMP_DIR}/xschem_web_xxxxx */
      tclvareval("try_download_url {", xctx->current_dirname, "} {", filename, "}", NULL);
    }
    if(stat(sympath, &buf)) { /* not found !!! build abs_sym_path to look into local fs and hope fror the best */
      my_strncpy(filename, abs_sym_path(sym->name, ".sch"), PATH_MAX);
    } else {
      my_strncpy(filename, sympath, PATH_MAX);
    }
  }
  my_free(_ALLOC_ID_, &str_tmp);
  dbg(1, "get_sch_from_sym(): sym->name=%s, filename=%s\n", sym->name, filename);
}

/* When descended into an i-th instance of a vector instance this function allows 
 * to change the path to the j-th instance. the instnumber parameters follows the same rules
 * as descend_schematic() */
int change_sch_path(int instnumber, int dr)
{
  int level = xctx->currsch - 1;
  char *instname = NULL;
  char *expanded_instname = NULL;
  int inst_mult;
  char *path = NULL;
  char *ptr;
  size_t pathlen;
  int res = 0;
  if(level < 0 ) return 0;
  my_strdup2(_ALLOC_ID_, &instname, get_tok_value(xctx->hier_attr[level].prop_ptr, "name", 0));
  my_strdup2(_ALLOC_ID_, &expanded_instname, expandlabel(instname, &inst_mult));
  my_strdup2(_ALLOC_ID_, &path, xctx->sch_path[xctx->currsch]);
  if(instnumber < 0 ) instnumber += inst_mult+1;
  /* any invalid number->descend to leftmost inst */
  if(instnumber <1 || instnumber > inst_mult) instnumber = 1;
  pathlen = strlen(path);
  if(pathlen == 0) goto end;
  path[pathlen - 1] = '\0';
  ptr = strrchr(path, '.');
  if(!ptr) goto end;
  *(ptr+1) = '\0';
  my_free(_ALLOC_ID_, &xctx->sch_path[xctx->currsch]);
  my_strcat(_ALLOC_ID_, &xctx->sch_path[xctx->currsch], path);
  my_strcat(_ALLOC_ID_, &xctx->sch_path[xctx->currsch], find_nth(expanded_instname, ",", "", 0, instnumber));
  my_strcat(_ALLOC_ID_, &xctx->sch_path[xctx->currsch], ".");
  xctx->sch_path_hash[xctx->currsch] = 0;
  xctx->sch_inst_number[level] = instnumber;
  dbg(1, "instname=%s, path=%s\n", instname, path);
  path[pathlen - 1] = '.';
  res = 1;
  if(dr && has_x) {
    draw();
  }
  end:
  my_free(_ALLOC_ID_, &instname);
  my_free(_ALLOC_ID_, &path);
  my_free(_ALLOC_ID_, &expanded_instname);
  return res;
}

/* fallback = 1: if schematic=.. attr is set but file not existing descend into symbol base schematic
 * instnumber: instance to descend into in case of vector instances (1 = leftmost, -1=rightmost)
 * if set_title == 0 do not set window title (faster)
 *              == 1 do set_title
 *              == 2 do not process instance pins/nets
 *              == 4 do not descend into i-th instance of vecrtor instance. just 
 *                 concatenate instance name as is to path and descend.
 *              above flags can be ORed together */
int descend_schematic(int instnumber, int fallback, int alert, int set_title)
{
 char *str = NULL;
 char filename[PATH_MAX];
 int inst_mult, inst_number;
 int save_ok = 0;
 int i, n = 0;
 int descend_ok = 1;

 if(xctx->currsch + 1 >= CADMAXHIER) {
   dbg(0, "descend_schematic(): max hierarchy depth reached: %d", CADMAXHIER);
   return 0;
 }
 rebuild_selected_array();
 if(/* xctx->lastsel !=1 || */ xctx->sel_array[0].type!=ELEMENT) {
   dbg(1, "descend_schematic(): wrong selection\n");
   return 0;
 }
 else {
   /* no name set for current schematic: save it before descending*/
   if(!strcmp(xctx->sch[xctx->currsch],""))
   {
     char cmd[PATH_MAX+1000];
     char res[PATH_MAX];
 
     my_strncpy(filename, xctx->sch[xctx->currsch], S(filename));
     my_snprintf(cmd, S(cmd), "save_file_dialog {Save file} * INITIALLOADDIR {%s}", filename);
     tcleval(cmd);
     my_strncpy(res, tclresult(), S(res));
     if(!res[0]) return 0;
     dbg(1, "descend_schematic(): saving: %s\n",res);
     save_ok = save_schematic(res, 0);
     if(save_ok==0) return 0;
   }
   n = xctx->sel_array[0].n;
   get_sch_from_sym(filename, xctx->inst[n].ptr+ xctx->sym, n, fallback);

   if(!filename[0]) return 0; /* no filename returned from get_sch_from_sym() --> abort */
   dbg(1, "descend_schematic(): selected:%s\n", xctx->inst[n].name);
   dbg(1, "descend_schematic(): inst type: %s\n", (xctx->inst[n].ptr+ xctx->sym)->type);
   if(                   /*  do not descend if not subcircuit */
      (xctx->inst[n].ptr+ xctx->sym)->type &&
      strcmp( (xctx->inst[n].ptr+ xctx->sym)->type, "subcircuit") &&
      strcmp( (xctx->inst[n].ptr+ xctx->sym)->type, "primitive")
   ) return 0;
   if(xctx->modified) {
     int ret;
 
     ret = save(1, 0);
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
     if(set_title & 4)  {
       my_strdup2(_ALLOC_ID_, &str, xctx->inst[n].instname);
       inst_mult = 1;
       instnumber = 1;
     } else {
       my_strdup2(_ALLOC_ID_, &str, expandlabel(xctx->inst[n].instname, &inst_mult));
     }
   } else {
     my_strdup2(_ALLOC_ID_, &str, "");
     inst_mult = 1;
   }
   prepare_netlist_structs(0); /* for portmap feature (mapping subcircuit nodes connected to
                                * ports to upper level) */

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
         my_free(_ALLOC_ID_, &str);
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

   my_strdup(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], xctx->sch_path[xctx->currsch]);
   xctx->sch_path_hash[xctx->currsch+1] =0;
   if(xctx->portmap[xctx->currsch + 1].table) str_hash_free(&xctx->portmap[xctx->currsch + 1]);
   str_hash_init(&xctx->portmap[xctx->currsch + 1], HASHSIZE);

   if(!(set_title & 2)) for(i = 0; i < xctx->sym[xctx->inst[n].ptr].rects[PINLAYER]; i++) {
     const char *pin_name = get_tok_value(xctx->sym[xctx->inst[n].ptr].rect[PINLAYER][i].prop_ptr,"name",0);
     char *pin_node = NULL, *net_node = NULL;
     int k, mult, net_mult;
     char *single_p, *single_n = NULL, *single_n_ptr = NULL;
     char *p_n_s1 = NULL;
     char *p_n_s2 = NULL;

     if(!pin_name[0]) continue;
     if(!xctx->inst[n].node[i]) continue;

     my_strdup2(_ALLOC_ID_, &pin_node, expandlabel(pin_name, &mult));
     my_strdup2(_ALLOC_ID_, &net_node, expandlabel(xctx->inst[n].node[i], &net_mult));

     p_n_s1 = pin_node;
     for(k = 1; k<=mult; ++k) {
         single_p = my_strtok_r(p_n_s1, ",", "", 0, &p_n_s2);
         p_n_s1 = NULL;
         my_strdup2(_ALLOC_ID_, &single_n,
             find_nth(net_node, ",", "", 0, ((inst_number - 1) * mult + k - 1) % net_mult + 1));
         single_n_ptr = single_n;
         if(single_n_ptr[0] == '#') {
           if(mult > 1) {
             my_mstrcat(_ALLOC_ID_, &single_n, "[", my_itoa((inst_mult - inst_number + 1) * mult - k), "]", NULL);
           }
           single_n_ptr = single_n + 1;
         }
         str_hash_lookup(&xctx->portmap[xctx->currsch + 1], single_p, single_n_ptr, XINSERT);
         dbg(1, "descend_schematic(): %s: %s ->%s\n", xctx->inst[n].instname, single_p, single_n_ptr);
     }
     if(single_n) my_free(_ALLOC_ID_, &single_n);
     my_free(_ALLOC_ID_, &net_node);
     my_free(_ALLOC_ID_, &pin_node);
   }

   my_strdup(_ALLOC_ID_, &xctx->hier_attr[xctx->currsch].prop_ptr, 
             xctx->inst[n].prop_ptr);
   my_strdup(_ALLOC_ID_, &xctx->hier_attr[xctx->currsch].templ, xctx->sym[xctx->inst[n].ptr].templ);
   my_strdup(_ALLOC_ID_, &xctx->hier_attr[xctx->currsch].sym_extra, 
     get_tok_value(xctx->sym[xctx->inst[n].ptr].prop_ptr, "extra", 0));

   dbg(1,"descend_schematic(): inst_number=%d\n", inst_number);
   my_strcat(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], find_nth(str, ",", "", 0, inst_number));
   my_free(_ALLOC_ID_, &str);
   dbg(1,"descend_schematic(): inst_number=%d\n", inst_number);
   my_strcat(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], ".");
   xctx->sch_inst_number[xctx->currsch] = inst_number;
   dbg(1, "descend_schematic(): current path: %s\n", xctx->sch_path[xctx->currsch+1]);
   dbg(1, "descend_schematic(): inst_number=%d\n", inst_number);
 
   xctx->previous_instance[xctx->currsch]=n;
   xctx->zoom_array[xctx->currsch].x=xctx->xorigin;
   xctx->zoom_array[xctx->currsch].y=xctx->yorigin;
   xctx->zoom_array[xctx->currsch].zoom=xctx->zoom;
   xctx->currsch++;
   hilight_child_pins();
   unselect_all(1);
   dbg(1, "descend_schematic(): filename=%s\n", filename);
   /* we are descending from a parent schematic downloaded from the web */
   if(!tclgetboolvar("keep_symbols")) remove_symbols();
   descend_ok = load_schematic(1, filename, (set_title & 1), alert);
   if(descend_ok) {
     if(xctx->hilight_nets) {
       prepare_netlist_structs(0);
       propagate_hilights(1, 0, XINSERT_NOREPLACE);
     }
     dbg(1, "descend_schematic(): before zoom(): prep_hash_inst=%d\n", xctx->prep_hash_inst);
  
     if(xctx->rects[GRIDLAYER] > 0 && tcleval("info exists ngspice::ngspice_data")[0] == '0') {
       Graph_ctx *gr = &xctx->graph_struct;
       xRect *r = &xctx->rect[GRIDLAYER][0];
       if(r->flags & 1) {
         if(xctx->graph_flags & 4) {
           backannotate_at_cursor_b_pos(r, gr);
         }
       }
     }
   }
   zoom_full(1, 0, 1 + 2 * tclgetboolvar("zoom_full_center"), 0.97);
 }
 return descend_ok;
}

/* 
 * what: 
 * 1: ask gui user confirm if schematic modified
 * 2: do *NOT* reset window title
 */
void go_back(int what)
{
 int save_ok;
 int from_embedded_sym;
 int save_modified;
 char filename[PATH_MAX];
 int prev_sch_type;
 int confirm = what & 1;
 int set_title = !(what & 2);

 save_ok=1;
 dbg(1,"go_back(): sch[xctx->currsch]=%s\n", xctx->sch[xctx->currsch]);
 prev_sch_type = xctx->netlist_type; /* if CAD_SYMBOL_ATTRS do not hilight_parent_pins */
 if(xctx->currsch>0)
 {
  /* if current sym/schematic is changed ask save before going up */
  if(xctx->modified)
  {
    if(confirm) {
      tcleval("ask_save_optional");
      if(!strcmp(tclresult(), "yes") ) save_ok = save_schematic(xctx->sch[xctx->currsch], 0);
      else if(!strcmp(tclresult(), "") ) return;
    }
    /* do not automatically save if confirm==0. Script developers should take care of this */
    /* 
     * else {
     *   save_ok = save_schematic(xctx->sch[xctx->currsch], 0);
     * }
     */
  }
  if(save_ok==0) {
    fprintf(errfp, "go_back(): file opening for write failed! %s \n", xctx->current_name);
    tclvareval("alert_ {file opening for write failed! ", xctx->current_name, "} {}", NULL);
  }
  unselect_all(1);
  if(!tclgetboolvar("keep_symbols")) remove_symbols();
  from_embedded_sym=0;
  if(strstr(xctx->sch[xctx->currsch], ".xschem_embedded_")) {
    /* when returning after editing an embedded symbol
     * load immediately symbol definition before going back (.xschem_embedded... file will be lost)
     */
    load_sym_def(xctx->sch[xctx->currsch], NULL);
    from_embedded_sym=1;
  }
  my_free(_ALLOC_ID_, &xctx->sch[xctx->currsch]);
  if(xctx->portmap[xctx->currsch].table) str_hash_free(&xctx->portmap[xctx->currsch]);

  xctx->sch_path_hash[xctx->currsch] = 0;
  xctx->currsch--;
  my_free(_ALLOC_ID_, &xctx->hier_attr[xctx->currsch].prop_ptr);
  my_free(_ALLOC_ID_, &xctx->hier_attr[xctx->currsch].templ);
  my_free(_ALLOC_ID_, &xctx->hier_attr[xctx->currsch].sym_extra);
  save_modified = xctx->modified; /* we propagate modified flag (cleared by load_schematic */
                            /* by default) to parent schematic if going back from embedded symbol */

  my_strncpy(filename, xctx->sch[xctx->currsch], S(filename));
  load_schematic(1, filename, set_title, 1);
  /* if we are returning from a symbol created from a generator don't set modified flag on parent
   * as these symbols can not be edited / saved as embedded
   * xctx->sch_inst_number[xctx->currsch + 1] == -1 --> we came from an inst with no embed flag set */
  if(from_embedded_sym && xctx->sch_inst_number[xctx->currsch] != -1)
    xctx->modified=save_modified; /* to force ask save embedded sym in parent schematic */

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

void clear_schematic(int cancel, int symbol)
{
      if(cancel == 1) cancel=save(1, 0);
      if(cancel != -1) { /* -1 means user cancel save request */
        char name[PATH_MAX];
        struct stat buf;
        int i;
        xctx->currsch = 0;
        unselect_all(1);
        remove_symbols();
        clear_drawing();
        if(symbol == 1) {
          xctx->netlist_type = CAD_SYMBOL_ATTRS;
          set_tcl_netlist_type();
          for(i=0;; ++i) { /* find a non-existent untitled[-n].sym */
            if(i == 0) my_snprintf(name, S(name), "%s.sym", "untitled");
            else my_snprintf(name, S(name), "%s-%d.sym", "untitled", i);
            if(stat(name, &buf)) break;
          }
          my_free(_ALLOC_ID_, &xctx->sch[xctx->currsch]);
          my_mstrcat(_ALLOC_ID_, &xctx->sch[xctx->currsch], pwd_dir, "/", name, NULL);
          my_strncpy(xctx->current_name, name, S(xctx->current_name));
        } else {
          xctx->netlist_type = CAD_SPICE_NETLIST;
          set_tcl_netlist_type();
          for(i=0;; ++i) {
            if(i == 0) my_snprintf(name, S(name), "%s.sch", "untitled");
            else my_snprintf(name, S(name), "%s-%d.sch", "untitled", i);
            if(stat(name, &buf)) break;
          }
          my_free(_ALLOC_ID_, &xctx->sch[xctx->currsch]);
          my_mstrcat(_ALLOC_ID_, &xctx->sch[xctx->currsch], pwd_dir, "/", name, NULL);
          my_strncpy(xctx->current_name, name, S(xctx->current_name));
        }
        draw();
        set_modify(0);
        xctx->prep_hash_inst=0;
        xctx->prep_hash_wires=0;
        xctx->prep_net_structs=0;
        xctx->prep_hi_structs=0;
        if(has_x) {
          set_modify(-1);
        }
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
 char *estr = NULL;

 xctx->show_hidden_texts = tclgetboolvar("show_hidden_texts");
 boundbox->x1=-100;
 boundbox->x2=100;
 boundbox->y1=-100;
 boundbox->y2=100;
 if(selected != 2) for(c=0;c<cadlayers; ++c)
 {
  int hide_graphs = tclgetboolvar("hide_empty_graphs");
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
     ov = INT_BUS_WIDTH(xctx->lw)> xctx->cadhalfdotsize ? INT_BUS_WIDTH(xctx->lw) : CADHALFDOTSIZE;
     if(xctx->wire[i].y1 < xctx->wire[i].y2) { y1 = xctx->wire[i].y1-ov; y2 = xctx->wire[i].y2+ov; }
     else                        { y1 = xctx->wire[i].y1+ov; y2 = xctx->wire[i].y2-ov; }
   } else {
     ov = xctx->cadhalfdotsize;
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
 if(has_x && selected != 2) {
   for(i=0;i<xctx->texts; ++i)
   { 
     int no_of_lines; 
     double longest_line;
     if(selected == 1 && !xctx->text[i].sel) continue;

     if(!xctx->show_hidden_texts && xctx->text[i].flags & (HIDE_TEXT /* | HIDE_TEXT_INSTANTIATED */)) continue;
     #if HAS_CAIRO==1
     customfont = set_text_custom_font(&xctx->text[i]);
     #endif
     estr = my_expand(get_text_floater(i), tclgetintvar("tabstop"));
     if(text_bbox(estr, xctx->text[i].xscale,
           xctx->text[i].yscale,xctx->text[i].rot, xctx->text[i].flip,
           xctx->text[i].hcenter, xctx->text[i].vcenter,
           xctx->text[i].x0, xctx->text[i].y0,
           &rect.x1,&rect.y1, &rect.x2,&rect.y2, &no_of_lines, &longest_line) ) {
       ++count;
       updatebbox(count,boundbox,&rect);
     }
     my_free(_ALLOC_ID_, &estr);
     #if HAS_CAIRO==1
     if(customfont) {
       cairo_restore(xctx->cairo_ctx);
     }
     #endif
   }
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
  double cs = tclgetdoublevar("cadsnap");
  if(flags & 1) {
    if(tclgetboolvar("change_lw")) {
      xctx->lw = 1.;
    }
    xctx->areax1 = -2*INT_LINE_W(xctx->lw);
    xctx->areay1 = -2*INT_LINE_W(xctx->lw);
    xctx->areax2 = xctx->xrect[0].width+2*INT_LINE_W(xctx->lw);
    xctx->areay2 = xctx->xrect[0].height+2*INT_LINE_W(xctx->lw);
    xctx->areaw = xctx->areax2-xctx->areax1;
    xctx->areah = xctx->areay2 - xctx->areay1;
  }
  calc_drawing_bbox(&boundbox, sel);
  dbg(1, "zoom_full: %s, %g %g  %g %g\n",
      xctx->current_win_path, boundbox.x1, boundbox.y1, boundbox.x2, boundbox.y2);
  schw = xctx->areaw-4*INT_LINE_W(xctx->lw);
  schh = xctx->areah-4*INT_LINE_W(xctx->lw);
  bboxw = boundbox.x2-boundbox.x1;
  bboxh = boundbox.y2-boundbox.y1;
  xctx->zoom = bboxw / schw;
  yzoom = bboxh / schh;
  if(yzoom > xctx->zoom) xctx->zoom = yzoom;
  xctx->zoom /= shrink;

  xctx->mooz = 1 / xctx->zoom;
  if(flags & 2) {
    xctx->xorigin = -boundbox.x1 + (xctx->zoom * schw - bboxw) / 2; /* centered */
    xctx->yorigin = -boundbox.y1 + (xctx->zoom * schh - bboxh) / 2; /* centered */
  } else {
    xctx->xorigin = -boundbox.x1 + (1 - shrink) / 2 * xctx->zoom * schw;
    xctx->yorigin = -boundbox.y1 + xctx->zoom * schh - bboxh - (1 - shrink) / 2 * xctx->zoom * schh;
  }
  dbg(1, "zoom_full(): dr=%d sel=%d flags=%d areaw=%d, areah=%d\n", sel, dr, flags, xctx->areaw, xctx->areah);
  dbg(1, "zoom_full(): zoom=%g, xor=%g, yor=%g\n", xctx->zoom, xctx->xorigin, xctx->yorigin);
  dbg(1, "zoom_full(): current_name=%s\n", xctx->current_name);
  if(flags & 1) change_linewidth(-1.);
  /* we do this here since change_linewidth may not be called  if flags & 1 == 0*/
  xctx->cadhalfdotsize = CADHALFDOTSIZE * (cs < 20. ? cs : 20.) / 10.;
  if(dr && has_x) {
    draw();
    redraw_w_a_l_r_p_z_rubbers(1);
  }
}

void view_zoom(double z)
{
  double factor;
  factor = z!=0.0 ? z : CADZOOMSTEP;
  if(xctx->zoom<CADMINZOOM) return;
  xctx->zoom/= factor;
  xctx->mooz=1/xctx->zoom;
  xctx->xorigin=-xctx->mousex_snap+(xctx->mousex_snap+xctx->xorigin)/factor;
  xctx->yorigin=-xctx->mousey_snap+(xctx->mousey_snap+xctx->yorigin)/factor;
  change_linewidth(-1.);

  draw();
  redraw_w_a_l_r_p_z_rubbers(1);
}

void view_unzoom(double z)
{
  double factor;
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
  redraw_w_a_l_r_p_z_rubbers(1);
}

void set_viewport_size(int w, int h, double lw)
{
    xctx->xrect[0].x = 0;
    xctx->xrect[0].y = 0;
    xctx->xrect[0].width = (unsigned short)w;
    xctx->xrect[0].height = (unsigned short)h;
    xctx->areax2 = w+2*INT_LINE_W(lw);
    xctx->areay2 = h+2*INT_LINE_W(lw);
    xctx->areax1 = -2*INT_LINE_W(lw);
    xctx->areay1 = -2*INT_LINE_W(lw);
    xctx->lw = lw;
    xctx->areaw = xctx->areax2-xctx->areax1;
    xctx->areah = xctx->areay2-xctx->areay1;
}

void save_restore_zoom(int save, Zoom_info *zi)
{
  if(save) {
    dbg(1, "save_restore_zoom: save width= %d, height=%d\n", xctx->xrect[0].width, xctx->xrect[0].height);
    dbg(1, "                   zoom=%g\n", xctx->zoom);
    zi->savew = xctx->xrect[0].width;
    zi->saveh = xctx->xrect[0].height;
    zi->savelw = xctx->lw;
    zi->savexor = xctx->xorigin;
    zi->saveyor = xctx->yorigin;
    zi->savezoom = xctx->zoom;
  } else {
    xctx->xrect[0].x = 0;
    xctx->xrect[0].y = 0;
    xctx->xrect[0].width = (unsigned short)zi->savew;
    xctx->xrect[0].height = (unsigned short)zi->saveh;
    dbg(1, "save_restore_zoom: restore width= %d, height=%d\n", xctx->xrect[0].width, xctx->xrect[0].height);
    xctx->areax2 = zi->savew+2*INT_LINE_W(zi->savelw);
    xctx->areay2 = zi->saveh+2*INT_LINE_W(zi->savelw);
    xctx->areax1 = -2*INT_LINE_W(zi->savelw);
    xctx->areay1 = -2*INT_LINE_W(zi->savelw);
    xctx->lw = zi->savelw;
    xctx->areaw = xctx->areax2-xctx->areax1;
    xctx->areah = xctx->areay2-xctx->areay1;
    xctx->xorigin = zi->savexor;
    xctx->yorigin = zi->saveyor;
    xctx->zoom = zi->savezoom;
    xctx->mooz = 1 / zi->savezoom;
    dbg(1, "                   zoom=%g\n", xctx->zoom);
  }
}

void zoom_box(double x1, double y1, double x2, double y2, double factor)
{
  double yy1;
  if(factor == 0.) factor = 1.;
  RECTORDER(x1,y1,x2,y2);
  xctx->xorigin=-x1;xctx->yorigin=-y1;
  xctx->zoom=(x2-x1)/(xctx->areaw-4*INT_LINE_W(xctx->lw));
  yy1=(y2-y1)/(xctx->areah-4*INT_LINE_W(xctx->lw));
  if(yy1>xctx->zoom) xctx->zoom=yy1;
  xctx->zoom*= factor;
  xctx->mooz=1/xctx->zoom;
  xctx->xorigin=xctx->xorigin+xctx->areaw*xctx->zoom*(1-1/factor)/2;
  xctx->yorigin=xctx->yorigin+xctx->areah*xctx->zoom*(1-1/factor)/2;
  dbg(1, "zoom_box(): zoom=%g\n", xctx->zoom);
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
    drawtemprect(xctx->gctiled, NOW, xctx->nl_xx1, xctx->nl_yy1, xctx->nl_xx2, xctx->nl_yy2);
    if( xctx->nl_x1 != xctx->nl_x2 || xctx->nl_y1 != xctx->nl_y2) {
      xctx->xorigin = -xctx->nl_x1; xctx->yorigin = -xctx->nl_y1;
      xctx->zoom = (xctx->nl_x2 - xctx->nl_x1) / (xctx->areaw - 4 * INT_LINE_W(xctx->lw));
      xctx->nl_yy1=(xctx->nl_y2 - xctx->nl_y1) / (xctx->areah - 4 * INT_LINE_W(xctx->lw));
      if(xctx->nl_yy1 > xctx->zoom) xctx->zoom = xctx->nl_yy1;
      xctx->mooz = 1 / xctx->zoom;
      change_linewidth(-1.);
      draw();
      redraw_w_a_l_r_p_z_rubbers(1);
      dbg(1, "zoom_rectangle(): coord: %.16g %.16g %.16g %.16g zoom=%.16g\n",
        xctx->nl_x1, xctx->nl_y1, xctx->mousex_snap, xctx->mousey_snap, xctx->zoom);
    }
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
  int intlw = 2 * INT_LINE_W(xctx->lw) + (int)xctx->cadhalfdotsize;
  xx1 = x1; yy1 = y1; xx2 = x2; yy2 = y2;
  RECTORDER(xx1,yy1,xx2,yy2);
  rebuild_selected_array();
  if(!xctx->lastsel) return;
  bbox(START,0.0, 0.0, 0.0, 0.0);
  bbox(ADD, xx1 - intlw, yy1 - intlw, xx2 + intlw, yy2 + intlw);
  bbox(SET,0.0, 0.0, 0.0, 0.0);
  draw_selection(xctx->gc[SELLAYER], 0);
  bbox(END,0.0, 0.0, 0.0, 0.0);
}

void new_wire(int what, double mx_snap, double my_snap)
{
  int modified = 0;
  double nl_xx1, nl_yy1, nl_xx2, nl_yy2;
  if( (what & PLACE) ) {
    if( (xctx->ui_state & STARTWIRE) && (xctx->nl_x1!=xctx->nl_x2 || xctx->nl_y1!=xctx->nl_y2) ) {
      xctx->push_undo();
      if(xctx->manhattan_lines & 1) {
        if(xctx->nl_x2!=xctx->nl_x1) {
          nl_xx1 = xctx->nl_x1; nl_yy1 = xctx->nl_y1;
          nl_xx2 = xctx->nl_x2; nl_yy2 = xctx->nl_y2;
          ORDER(nl_xx1,nl_yy1,nl_xx2,nl_yy1);
          storeobject(-1, nl_xx1,nl_yy1,nl_xx2,nl_yy1,WIRE,0,0,NULL);
          modified = 1;
          hash_wire(XINSERT, xctx->wires-1, 1);
          drawline(WIRELAYER,NOW, nl_xx1,nl_yy1,nl_xx2,nl_yy1, 0, NULL);
        }
        if(xctx->nl_y2!=xctx->nl_y1) {
          nl_xx1 = xctx->nl_x1; nl_yy1 = xctx->nl_y1; 
          nl_xx2 = xctx->nl_x2; nl_yy2 = xctx->nl_y2;
          ORDER(nl_xx2,nl_yy1,nl_xx2,nl_yy2);
          storeobject(-1, nl_xx2,nl_yy1,nl_xx2,nl_yy2,WIRE,0,0,NULL);
          modified = 1;
          hash_wire(XINSERT, xctx->wires-1, 1);
          drawline(WIRELAYER,NOW, nl_xx2,nl_yy1,nl_xx2,nl_yy2, 0, NULL);
        }
      } else if(xctx->manhattan_lines & 2) {
        if(xctx->nl_y2!=xctx->nl_y1) {
          nl_xx1 = xctx->nl_x1; nl_yy1 = xctx->nl_y1;
          nl_xx2 = xctx->nl_x2; nl_yy2 = xctx->nl_y2;
          ORDER(nl_xx1,nl_yy1,nl_xx1,nl_yy2);
          storeobject(-1, nl_xx1,nl_yy1,nl_xx1,nl_yy2,WIRE,0,0,NULL);
          modified = 1;
          hash_wire(XINSERT, xctx->wires-1, 1);
          drawline(WIRELAYER,NOW, nl_xx1,nl_yy1,nl_xx1,nl_yy2, 0, NULL);
        }
        if(xctx->nl_x2!=xctx->nl_x1) {
          nl_xx1=xctx->nl_x1;nl_yy1=xctx->nl_y1;
          nl_xx2=xctx->nl_x2;nl_yy2=xctx->nl_y2;
          ORDER(nl_xx1,nl_yy2,nl_xx2,nl_yy2);
          storeobject(-1, nl_xx1,nl_yy2,nl_xx2,nl_yy2,WIRE,0,0,NULL);
          modified = 1;
          hash_wire(XINSERT, xctx->wires-1, 1);
          drawline(WIRELAYER,NOW, nl_xx1,nl_yy2,nl_xx2,nl_yy2, 0, NULL);
        }
      } else {
        nl_xx1 = xctx->nl_x1; nl_yy1 = xctx->nl_y1;
        nl_xx2 = xctx->nl_x2; nl_yy2 = xctx->nl_y2;
        ORDER(nl_xx1,nl_yy1,nl_xx2,nl_yy2);
        storeobject(-1, nl_xx1,nl_yy1,nl_xx2,nl_yy2,WIRE,0,0,NULL);
        modified = 1;
        hash_wire(XINSERT, xctx->wires-1, 1);
        drawline(WIRELAYER,NOW, nl_xx1,nl_yy1,nl_xx2,nl_yy2, 0, NULL);
      }
      xctx->prep_hi_structs = 0;
      if(tclgetboolvar("autotrim_wires")) trim_wires();
      prepare_netlist_structs(0); /* since xctx->prep_hi_structs==0, do a delete_netlist_structs() first,
                                   * this clears both xctx->prep_hi_structs and xctx->prep_net_structs. */
      if(xctx->hilight_nets) {
        propagate_hilights(1, 1, XINSERT_NOREPLACE);
      }
      draw();
      /* draw_hilight_net(1);*/  /* for updating connection bubbles on hilight nets */
    }
    xctx->nl_x1 = xctx->nl_x2=mx_snap; xctx->nl_y1 = xctx->nl_y2=my_snap;
    xctx->ui_state |= STARTWIRE;
    if(modified) set_modify(1);
  }
  if( what & END) {
    xctx->ui_state &= ~STARTWIRE;
  }
  if( (what & RUBBER)  ) {
    drawtemp_manhattanline(xctx->gctiled, NOW, xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2, 0);
    restore_selection(xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2);
    xctx->nl_x2 = mx_snap; xctx->nl_y2 = my_snap;
    if(!(what & CLEAR)) {
      drawtemp_manhattanline(xctx->gc[WIRELAYER], NOW, xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2, 0);
    }
  }
}

void change_layer()
{
  int k, n, type, c;
  double x1,y1,x2,y2, a, b, r;
  int modified = 0;

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
       modified = 1;
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
       modified = 1;
     }
     else if(type==xTEXT && xctx->text[n].sel==SELECTED) {
       if(xctx->rectcolor != xctx->text[n].layer) {
         char *p;
         my_strdup2(_ALLOC_ID_, &xctx->text[n].prop_ptr, 
           subst_token(xctx->text[n].prop_ptr, "layer", dtoa(xctx->rectcolor) ));
         xctx->text[n].layer = xctx->rectcolor;
         p = xctx->text[n].prop_ptr;
         while(*p) {
           if(*p == '\n') *p = ' ';
           ++p;
         }
         modified = 1;
       }
     }
   }
   if(xctx->lastsel) delete_only_rect_line_arc_poly();
   unselect_all(1);
   if(modified) set_modify(1);
}

void new_arc(int what, double sweep, double mousex_snap, double mousey_snap)
{
  if(what & PLACE) {
    xctx->nl_state=0;
    xctx->nl_r = -1.;
    xctx->nl_sweep_angle=sweep;
    xctx->nl_xx1 = xctx->nl_xx2 = xctx->nl_x1 = xctx->nl_x2 = xctx->nl_x3 = mousex_snap;
    xctx->nl_yy1 = xctx->nl_yy2 = xctx->nl_y1 = xctx->nl_y2 = xctx->nl_y3 = mousey_snap;
    xctx->ui_state |= STARTARC;
  }
  if(what & SET) {
    if(xctx->nl_state==0) {
      xctx->nl_x2 = xctx->mousex_snap;
      xctx->nl_y2 = xctx->mousey_snap;
      drawtempline(xctx->gctiled, NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      restore_selection(xctx->nl_xx1, xctx->nl_yy1, xctx->nl_xx2, xctx->nl_yy2);
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
        set_modify(1);
      }
      xctx->ui_state &= ~STARTARC;
      xctx->nl_state=0;
    }
  }
  if(what & RUBBER) {
    if(xctx->nl_state==0) {
      drawtempline(xctx->gctiled, NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      restore_selection(xctx->nl_xx1, xctx->nl_yy1, xctx->nl_xx2, xctx->nl_yy2);
      xctx->nl_x2 = xctx->mousex_snap;xctx->nl_y2 = xctx->mousey_snap;
      xctx->nl_xx1 = xctx->nl_x1; /* This **is** needed. Don't remove! */
      xctx->nl_yy1 = xctx->nl_y1; /* This **is** needed. Don't remove! */
      xctx->nl_xx2 = xctx->mousex_snap;
      xctx->nl_yy2 = xctx->mousey_snap;
      ORDER(xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
      drawtempline(xctx->gc[SELLAYER], NOW, xctx->nl_xx1,xctx->nl_yy1,xctx->nl_xx2,xctx->nl_yy2);
    }
    else if(xctx->nl_state==1) {
      xctx->nl_x3 = xctx->mousex_snap;
      xctx->nl_y3 = xctx->mousey_snap;
      if(xctx->nl_r>0.)
          drawtemparc(xctx->gctiled, NOW, xctx->nl_x, xctx->nl_y, xctx->nl_r, xctx->nl_a, xctx->nl_b);
      arc_3_points(xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2,
          xctx->nl_x3, xctx->nl_y3, &xctx->nl_x, &xctx->nl_y, &xctx->nl_r, &xctx->nl_a, &xctx->nl_b);
      restore_selection(xctx->nl_xx1, xctx->nl_yy1, xctx->nl_xx2, xctx->nl_yy2);
      arc_bbox(xctx->nl_x, xctx->nl_y, xctx->nl_r, xctx->nl_a, xctx->nl_b,
                &xctx->nl_xx1, &xctx->nl_yy1, &xctx->nl_xx2, &xctx->nl_yy2);
      if(xctx->nl_sweep_angle==360.) xctx->nl_b=360.;
      if(xctx->nl_r>0.) drawtemparc(xctx->gc[xctx->rectcolor], NOW,
           xctx->nl_x, xctx->nl_y, xctx->nl_r, xctx->nl_a, xctx->nl_b);
    }
  }
}

void new_line(int what, double mx_snap, double my_snap)
{
  int modified = 0;
  double nl_xx1, nl_yy1, nl_xx2, nl_yy2;

  if( (what & PLACE) )
  {
    if( (xctx->nl_x1!=xctx->nl_x2 || xctx->nl_y1!=xctx->nl_y2) && (xctx->ui_state & STARTLINE) )
    {
      xctx->push_undo();
      if(xctx->manhattan_lines & 1) {
        if(xctx->nl_x2!=xctx->nl_x1) {
          nl_xx1 = xctx->nl_x1; nl_yy1 = xctx->nl_y1;
          nl_xx2 = xctx->nl_x2; nl_yy2 = xctx->nl_y2;
          ORDER(nl_xx1,nl_yy1,nl_xx2,nl_yy1);
          storeobject(-1, nl_xx1,nl_yy1,nl_xx2,nl_yy1,LINE,xctx->rectcolor,0,NULL);
          modified = 1;
          drawline(xctx->rectcolor,NOW, nl_xx1,nl_yy1,nl_xx2,nl_yy1, 0, NULL);
        }
        if(xctx->nl_y2!=xctx->nl_y1) {
          nl_xx1 = xctx->nl_x1; nl_yy1 = xctx->nl_y1;
          nl_xx2 = xctx->nl_x2; nl_yy2 = xctx->nl_y2;
          ORDER(nl_xx2,nl_yy1,nl_xx2,nl_yy2);
          storeobject(-1, nl_xx2,nl_yy1,nl_xx2,nl_yy2,LINE,xctx->rectcolor,0,NULL);
          modified = 1;
          drawline(xctx->rectcolor,NOW, nl_xx2,nl_yy1,nl_xx2,nl_yy2, 0, NULL);
        }
      } else if(xctx->manhattan_lines & 2) {
        if(xctx->nl_y2!=xctx->nl_y1) {
          nl_xx1 = xctx->nl_x1; nl_yy1 = xctx->nl_y1;
          nl_xx2 = xctx->nl_x2; nl_yy2 = xctx->nl_y2;
          ORDER(nl_xx1,nl_yy1,nl_xx1,nl_yy2);
          storeobject(-1, nl_xx1,nl_yy1,nl_xx1,nl_yy2,LINE,xctx->rectcolor,0,NULL);
          modified = 1;
          drawline(xctx->rectcolor,NOW, nl_xx1,nl_yy1,nl_xx1,nl_yy2, 0, NULL);
        }
        if(xctx->nl_x2!=xctx->nl_x1) {
          nl_xx1=xctx->nl_x1;nl_yy1=xctx->nl_y1;
          nl_xx2=xctx->nl_x2;nl_yy2=xctx->nl_y2;
          ORDER(nl_xx1,nl_yy2,nl_xx2,nl_yy2);
          storeobject(-1, nl_xx1,nl_yy2,nl_xx2,nl_yy2,LINE,xctx->rectcolor,0,NULL);
          modified = 1;
          drawline(xctx->rectcolor,NOW, nl_xx1,nl_yy2,nl_xx2,nl_yy2, 0, NULL);
        }
      } else {
        nl_xx1 = xctx->nl_x1; nl_yy1 = xctx->nl_y1;
        nl_xx2 = xctx->nl_x2; nl_yy2 = xctx->nl_y2;
        ORDER(nl_xx1,nl_yy1,nl_xx2,nl_yy2);
        storeobject(-1, nl_xx1,nl_yy1,nl_xx2,nl_yy2,LINE,xctx->rectcolor,0,NULL);
        modified = 1;
        drawline(xctx->rectcolor,NOW, nl_xx1,nl_yy1,nl_xx2,nl_yy2, 0, NULL);
      }
      if(modified) set_modify(1);
    }
    xctx->nl_x1=xctx->nl_x2=mx_snap;xctx->nl_y1=xctx->nl_y2=my_snap;
    xctx->ui_state |= STARTLINE;
  }
  if( what & END)
  {
    xctx->ui_state &= ~STARTLINE;
  }
  if( (what & RUBBER)  ) {
    drawtemp_manhattanline(xctx->gctiled, NOW, xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2, 0);
    restore_selection(xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2);
    xctx->nl_x2 = mx_snap; xctx->nl_y2 = my_snap;
    if(!(what & CLEAR)) { 
      drawtemp_manhattanline(xctx->gc[xctx->rectcolor], NOW, xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2, 0);
    }    
  }        
}

void new_rect(int what, double mousex_snap, double mousey_snap)
{
  int modified = 0;
  double nl_xx1, nl_yy1, nl_xx2, nl_yy2;
  if( (what & PLACE) )
  {
   if( (xctx->nl_x1!=xctx->nl_x2 || xctx->nl_y1!=xctx->nl_y2) && (xctx->ui_state & STARTRECT) )
   {
    int save_draw;
    RECTORDER(xctx->nl_x1,xctx->nl_y1,xctx->nl_x2,xctx->nl_y2);
    xctx->push_undo();
    drawrect(xctx->rectcolor, NOW, xctx->nl_x1,xctx->nl_y1,xctx->nl_x2,xctx->nl_y2, 0, -1, -1);
    save_draw = xctx->draw_window;
    xctx->draw_window = 1;
    /* draw fill pattern even in xcopyarea mode */
    filledrect(xctx->rectcolor, NOW, xctx->nl_x1,xctx->nl_y1,xctx->nl_x2,xctx->nl_y2, 1, -1, -1);
    xctx->draw_window = save_draw;
    storeobject(-1, xctx->nl_x1,xctx->nl_y1,xctx->nl_x2,xctx->nl_y2,xRECT,xctx->rectcolor, 0, NULL);
    modified = 1;
   }
   xctx->nl_x1 = xctx->nl_x2 = mousex_snap;xctx->nl_y1 = xctx->nl_y2 = mousey_snap;
   xctx->ui_state |= STARTRECT;
   if(modified) set_modify(1);
  }
  if( what & END)
  {
   xctx->ui_state &= ~STARTRECT;
  }
  if(what & RUBBER)
  {
   nl_xx1 = xctx->nl_x1;nl_yy1 = xctx->nl_y1;nl_xx2 = xctx->nl_x2;nl_yy2 = xctx->nl_y2;
   RECTORDER(nl_xx1,nl_yy1,nl_xx2,nl_yy2);
   drawtemprect(xctx->gctiled,NOW, nl_xx1,nl_yy1,nl_xx2,nl_yy2);
   restore_selection(xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2);
   xctx->nl_x2 = xctx->mousex_snap;xctx->nl_y2 = xctx->mousey_snap;
   nl_xx1 = xctx->nl_x1;nl_yy1 = xctx->nl_y1;nl_xx2 = xctx->nl_x2;nl_yy2 = xctx->nl_y2;
   RECTORDER(nl_xx1,nl_yy1,nl_xx2,nl_yy2);
   drawtemprect(xctx->gc[xctx->rectcolor], NOW, nl_xx1,nl_yy1,nl_xx2,nl_yy2);
  }
}


void new_polygon(int what, double mousex_snap, double mousey_snap)
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
     xctx->nl_polyy[xctx->nl_points]=mousey_snap;
     xctx->nl_polyx[xctx->nl_points]=mousex_snap;
     xctx->nl_points++;
     xctx->nl_polyx[xctx->nl_points]=xctx->nl_polyx[xctx->nl_points-1]; /* prepare next point for rubber */
     xctx->nl_polyy[xctx->nl_points] = xctx->nl_polyy[xctx->nl_points-1];
     /* fprintf(errfp, "added point: %.16g %.16g\n", xctx->nl_polyx[xctx->nl_points-1],
         xctx->nl_polyy[xctx->nl_points-1]); */
     xctx->nl_x1=xctx->nl_x2=mousex_snap;xctx->nl_y1=xctx->nl_y2=mousey_snap;
     xctx->ui_state |= STARTPOLYGON;
     set_modify(1);
   }
   if( what & ADD)
   {
     if(mousex_snap < xctx->nl_x1) xctx->nl_x1 = mousex_snap;
     if(mousex_snap > xctx->nl_x2) xctx->nl_x2 = mousex_snap;
     if(mousey_snap < xctx->nl_y1) xctx->nl_y1 = mousey_snap;
     if(mousey_snap > xctx->nl_y2) xctx->nl_y2 = mousey_snap;
     /* closed poly */
     if(what & END) {
       /* delete last rubber */
       drawtemppolygon(xctx->gctiled, NOW, xctx->nl_polyx, xctx->nl_polyy, xctx->nl_points+1, 0);
       xctx->nl_polyx[xctx->nl_points] = xctx->nl_polyx[0];
       xctx->nl_polyy[xctx->nl_points] = xctx->nl_polyy[0];
     /* add point */
     } else if(xctx->nl_polyx[xctx->nl_points] != xctx->nl_polyx[xctx->nl_points-1] ||
          xctx->nl_polyy[xctx->nl_points] != xctx->nl_polyy[xctx->nl_points-1]) {
       xctx->nl_polyx[xctx->nl_points] = mousex_snap;
       xctx->nl_polyy[xctx->nl_points] = mousey_snap;
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
     drawtemppolygon(xctx->gctiled, NOW, xctx->nl_polyx, xctx->nl_polyy, xctx->nl_points+1, 0);
     store_poly(-1, xctx->nl_polyx, xctx->nl_polyy, xctx->nl_points, xctx->rectcolor, 0, NULL);
     /* fprintf(errfp, "new_poly: finish: nl_points=%d\n", xctx->nl_points); */
     drawtemppolygon(xctx->gc[xctx->rectcolor], NOW, xctx->nl_polyx, xctx->nl_polyy, xctx->nl_points, 0);
     xctx->ui_state &= ~STARTPOLYGON;
     drawpolygon(xctx->rectcolor, NOW, xctx->nl_polyx, xctx->nl_polyy, xctx->nl_points, 0, 0, 0);
     my_free(_ALLOC_ID_, &xctx->nl_polyx);
     my_free(_ALLOC_ID_, &xctx->nl_polyy);
     xctx->nl_maxpoints = xctx->nl_points = 0;
   }
   if(what & RUBBER)
   {
     if(mousex_snap < xctx->nl_x1) xctx->nl_x1 = mousex_snap;
     if(mousex_snap > xctx->nl_x2) xctx->nl_x2 = mousex_snap;
     if(mousey_snap < xctx->nl_y1) xctx->nl_y1 = mousey_snap;
     if(mousey_snap > xctx->nl_y2) xctx->nl_y2 = mousey_snap;
     /* fprintf(errfp, "new_poly: RUBBER\n"); */
     drawtemppolygon(xctx->gctiled, NOW, xctx->nl_polyx, xctx->nl_polyy, xctx->nl_points+1, 0);
     xctx->nl_polyy[xctx->nl_points] = mousey_snap;
     xctx->nl_polyx[xctx->nl_points] = mousex_snap;
     restore_selection(xctx->nl_x1, xctx->nl_y1, xctx->nl_x2, xctx->nl_y2);
     /* xctx->nl_x2 = mousex_snap; xctx->nl_y2 = mousey_snap; */
     drawtemppolygon(xctx->gc[xctx->rectcolor], NOW, xctx->nl_polyx, xctx->nl_polyy, xctx->nl_points+1, 0);
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

  /* if no cairo_ctx is available use text_bbox_nocairo().
  * will not match exactly font metrics when doing ps/svg output, but better than nothing */
  if(!has_x && !xctx->cairo_ctx) return text_bbox_nocairo(str, xscale, yscale, rot, flip, hcenter, vcenter, x1, y1,
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
  w *= (FONTWIDTH+FONTWHITESPACE)*xscale* tclgetdoublevar("nocairo_font_xscale") * cairo_font_scale;
  *cairo_longest_line = w;
  h *= (FONTHEIGHT+FONTDESCENT+FONTWHITESPACE)*yscale* tclgetdoublevar("nocairo_font_yscale") * cairo_font_scale;
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



int create_text(int draw_text, double x, double y, int rot, int flip, const char *txt,
    const char *props, double hsize, double vsize)
{
  int textlayer;
  xText *t;
  int save_draw;
  #if HAS_CAIRO==1
  const char  *textfont;
  #endif

  check_text_storage();
  t = &xctx->text[xctx->texts];
  t->txt_ptr=NULL;
  t->prop_ptr=NULL;  /*  20111006 added missing initialization of pointer */
  t->floater_ptr = NULL;
  t->font=NULL;
  t->floater_instname=NULL;
  my_strdup2(_ALLOC_ID_, &t->txt_ptr, txt);
  t->x0=x;
  t->y0=y;
  t->rot=(short int) rot;
  t->flip=(short int) flip;
  t->sel=0;
  t->xscale= hsize;
  t->yscale= vsize;
  my_strdup(_ALLOC_ID_, &t->prop_ptr, props);
  /*  debug ... */
  /*  t->prop_ptr=NULL; */
  dbg(1, "create_text(): done text input\n");
  set_text_flags(t);
  textlayer = t->layer;
  if(textlayer < 0 || textlayer >= cadlayers) textlayer = TEXTLAYER;

  if(draw_text) {
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
    draw_string(textlayer, NOW, get_text_floater(xctx->texts), t->rot, t->flip,
        t->hcenter, t->vcenter, t->x0,t->y0, t->xscale, t->yscale);
    xctx->draw_window = save_draw;
    #if HAS_CAIRO==1
    if((textfont && textfont[0]) || (t->flags & (TEXT_BOLD | TEXT_OBLIQUE | TEXT_ITALIC))) {
      cairo_restore(xctx->cairo_ctx);
      cairo_restore(xctx->cairo_save_ctx);
    }
    #endif
  }
  xctx->texts++;
  return 1;
}

int place_text(int draw_text, double mx, double my)
{
  char *txt, *props, *hsize, *vsize;

  tclsetvar("props","");
  tclsetvar("retval","");

  if(!tclgetvar("tctx::hsize"))
   tclsetvar("tctx::hsize","0.4");
  if(!tclgetvar("tctx::vsize"))
   tclsetvar("tctx::vsize","0.4");
  xctx->semaphore++;
  tcleval("enter_text {text:} normal");
  xctx->semaphore--;

  dbg(1, "place_text(): hsize=%s vsize=%s\n",tclgetvar("tctx::hsize"), tclgetvar("tctx::vsize") );
  /* get: retval, hsize, vsize, props,  */
  txt =  (char *)tclgetvar("retval");
  props =  (char *)tclgetvar("props");
  hsize =  (char *)tclgetvar("tctx::hsize");
  vsize =  (char *)tclgetvar("tctx::vsize");
  if(!txt || !strcmp(txt,"")) return 0;   /*  dont allocate text object if empty string given */
  xctx->push_undo();
  dbg(1,"props=%s, txt=%s\n", props, txt);

  create_text(draw_text, mx, my, 0, 0, txt, props, atof(hsize), atof(vsize));
  select_text(xctx->texts - 1, SELECTED, 0, 1);
  rebuild_selected_array(); /* sets xctx->ui_state |= SELECTION */
  drawtemprect(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
  drawtempline(xctx->gc[SELLAYER], END, 0.0, 0.0, 0.0, 0.0);
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

/* instead of doing a drawtemprect(xctx->gctiled, NOW, ....) do 4 
 * XCopy Area operations */
void fix_restore_rect(double x1, double y1, double x2, double y2)
{
  dbg(1, "%g %g %g %g\n", x1, y1, x2, y2);
  /* horizontal lines */
  MyXCopyAreaDouble(display, xctx->save_pixmap, xctx->window, xctx->gc[0],
      x1, y1, x2, y1, x1, y1,
      xctx->lw);

  MyXCopyAreaDouble(display, xctx->save_pixmap, xctx->window, xctx->gc[0],
      x1, y2, x2, y2, x1, y2,
      xctx->lw);

  /* vertical lines */
  MyXCopyAreaDouble(display, xctx->save_pixmap, xctx->window, xctx->gc[0],
      x1, y1, x1, y2, x1, y1,
      xctx->lw);

  MyXCopyAreaDouble(display, xctx->save_pixmap, xctx->window, xctx->gc[0],
      x2, y1, x2, y2, x2, y1,
      xctx->lw);
}


/*  20150927 select=1: select objects, select=0: unselect objects
 * uses static variables:
 *   xctx->nl_xr, xctx->nl_yr, xctx->nl_xr2, xctx->nl_yr2: The selection box
 *   xctx->nl_sel: selection mode (1=select, 0=unselect) as set in what == START
 *   xctx->nl_dir: drag direction: 1=right, 1=left
 *   xctx->nl_sem: semaphore used internally to detect misuse of the function.
 */
void select_rect(int stretch, int what, int select)
{
 double nl_xx1, nl_yy1, nl_xx2, nl_yy2;
 int incremental_select = tclgetboolvar("incremental_select");
 int sel_touch = tclgetboolvar("select_touch");
 dbg(1, "select_rect(): what=%d, mousex_save=%g mousey_save=%g, mousex=%g mousey=%g\n", 
        what, xctx->mx_double_save, xctx->my_double_save, xctx->mousex, xctx->mousey);
 if(what & RUBBER)
 {
    if(xctx->nl_sem==0) {
      fprintf(errfp, "ERROR: select_rect() RUBBER called before START\n");
      tcleval("alert_ {ERROR: select_rect() RUBBER called before START} {}");
    }
    nl_xx1=xctx->nl_xr;nl_xx2=xctx->nl_xr2;nl_yy1=xctx->nl_yr;nl_yy2=xctx->nl_yr2;
    RECTORDER(nl_xx1,nl_yy1,nl_xx2,nl_yy2);
    drawtemprect(xctx->gctiled,NOW, nl_xx1,nl_yy1,nl_xx2,nl_yy2);
    xctx->nl_xr2=xctx->mousex;xctx->nl_yr2=xctx->mousey;

    if(!xctx->nl_sel || (incremental_select && xctx->nl_dir == 0))
       select_inside(stretch, nl_xx1, nl_yy1, nl_xx2, nl_yy2, xctx->nl_sel);
    else if(incremental_select && xctx->nl_dir == 1 && sel_touch)
       select_touch(nl_xx1, nl_yy1, nl_xx2, nl_yy2, xctx->nl_sel);
    nl_xx1=xctx->nl_xr;nl_xx2=xctx->nl_xr2;nl_yy1=xctx->nl_yr;nl_yy2=xctx->nl_yr2;
    RECTORDER(nl_xx1,nl_yy1,nl_xx2,nl_yy2);
    drawtemprect(xctx->gc[SELLAYER],NOW, nl_xx1,nl_yy1,nl_xx2,nl_yy2);

    rebuild_selected_array();
    draw_selection(xctx->gc[SELLAYER], 0);
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

    if(!sel_touch || xctx->nl_dir == 0)
      select_inside(stretch, xctx->nl_xr,xctx->nl_yr,xctx->nl_xr2,xctx->nl_yr2, xctx->nl_sel);
    else 
      select_touch(xctx->nl_xr,xctx->nl_yr,xctx->nl_xr2,xctx->nl_yr2, xctx->nl_sel);

    draw_selection(xctx->gc[SELLAYER], 0);
    xctx->ui_state &= ~STARTSELECT;
    xctx->nl_sem=0;
 }
}

/* needed to dynamically reassign the `manhattan_lines` value for wire-drawing */
void recompute_orthogonal_manhattanline(double linex1, double liney1, double linex2, double liney2) {
  double origin_shifted_x2, origin_shifted_y2;
  /* Origin shift the cartesian coordinate p2(x2,y2) w.r.t. p1(x1,y1) */
  origin_shifted_x2 = linex2 - linex1;
  origin_shifted_y2 = liney2 - liney1;
  /* Draw whichever component of the resulting orthogonal-wire is bigger (either horizontal or vertical), first */
  if(origin_shifted_x2*origin_shifted_x2 > origin_shifted_y2*origin_shifted_y2)
    xctx->manhattan_lines = 1;
  else
    xctx->manhattan_lines = 2;

  return;
}

