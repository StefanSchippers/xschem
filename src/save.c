/* File: save.c
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


/* splits a command string into argv-like arguments
 * return # of args in *argc
 * argv[*argc] is always set to NULL
 * parse_cmd_string(NULL, NULL) to clear static allocated data */
#define PARSE_SIZE 128
char **parse_cmd_string(const char *cmd, int *argc)
{
  static char *cmd_copy = NULL;
  static char *argv[PARSE_SIZE];
  char *cmd_ptr, *cmd_save;
  dbg(1, "parse_cmd_string(): cmd=|%s|\n", cmd ? cmd : "<NULL>");
  if(!cmd || !cmd[0]) {
    if(cmd_copy) my_free(_ALLOC_ID_, &cmd_copy);
    return NULL;
  }
  *argc = 0;
  my_strdup2(_ALLOC_ID_, &cmd_copy, cmd);
  cmd_ptr = cmd_copy;
  while( (argv[*argc] = my_strtok_r(cmd_ptr, " \t", "'\"", 0, &cmd_save)) ) {
    cmd_ptr = NULL;
    dbg(1, "--> %s\n", argv[*argc]);
    (*argc)++;
    if(*argc + 1 >= PARSE_SIZE) break; /* leave one element for the last NULL pointer */
  }
  argv[*argc] = NULL; /*terminating pointer needed by execvp() */
  return argv;
}

/* get an input databuffer (din[ilen]), and a shell command (cmd) that reads stdin
 * and writes stdout, return the result in dout[olen].
 * Caller must free the returned buffer.
 */
#ifdef __unix__
int filter_data(const char *din,  const size_t ilen,
           char **dout, size_t *olen,
           const char *cmd)
{
  int p1[2]; /* parent -> child, 0: read, 1: write */
  int p2[2]; /* child -> parent, 0: read, 1: write */
  int ret = 0, wstatus;
  pid_t pid;
  size_t bufsize = 32768, oalloc = 0, n = 0;

  if(!cmd) { /* basic check */
    *dout = NULL;
    *olen = 0;
    return 1;
  }

  dbg(1, "filter_data(): ilen=%ld, cmd=%s\n", ilen, cmd);
  if(pipe(p1) == -1) dbg(0, "filter_data(): pipe creation failed\n");
  if(pipe(p2) == -1) dbg(0, "filter_data(): pipe creation failed\n");

  dbg(1, "p1[0] = %d\n", p1[0]);
  dbg(1, "p1[1] = %d\n", p1[1]);
  dbg(1, "p2[0] = %d\n", p2[0]);
  dbg(1, "p2[1] = %d\n", p2[1]);

   
  signal(SIGPIPE, SIG_IGN); /* so attempting write/read a broken pipe won't kill program */
/* 
 *                                  p2
 *  -------------------   p2[0] <--------- p2[1]   -------------------  
 * |   Parent program  |                          |   Child filter    |    
 *  -------------------   p1[1] ---------> p1[0]   -------------------  
 *                                  p1
 */
  fflush(NULL); /* flush all stdio streams before process forking */
  if( (pid = fork()) == 0) { /* child process */
    #if 1
    char **av;
    int ac;
    #endif
    /* child */
    debug_var = 0; /* do not log child allocations, see below */
    close(p1[1]); /* only read from p1 */
    close(p2[0]); /* only write to p2 */
    close(0); /* dup2(p1[0],0); */  /* connect read side of read pipe to stdin */
    if(dup(p1[0]) == -1) dbg(0, "filter_data(): dup() call failed\n");
    close(p1[0]);
    close(1); /* dup2(p2[1],1); */ /* connect write side of write pipe to stdout */
    if(dup(p2[1]) == -1) dbg(0, "filter_data(): dup() call failed\n");
    close(p2[1]);

    #if 1
    av = parse_cmd_string(cmd, &ac);
    /* ATTENTION: above parse_cmd_string() 'cmd_copy' allocated string is not (can not be) freed
     * since av[] points into it, * so it may appear as leaked memory. This is the reason I set
     * debug_var=0 in child. (avoid false warnings).
     * Following execvp() clears all process data, nothing is leaked. */
    if(execvp(av[0], av) == -1) {
    #endif

    #if 0
    if(execl("/bin/sh", "sh", "-c", cmd, (char *) NULL) == -1) {
    #endif

    #if 0
    if(system(cmd) == -1) {
    #endif

      fprintf(stderr, "error: conversion failed\n");
      ret = 1;
    }
    _exit(ret); /* childs should always use _exit() to avoid 
                 * flushing open stdio streams and other unwanted side effects */
  }
  /* parent */
  close(p1[0]); /*only write to p1 */
  close(p2[1]); /* only read from p2 */
  if(din && ilen) {
    if(write(p1[1], din, ilen) != ilen) { /* write input data to pipe */
      fprintf(stderr, "filter_data() write to pipe failed or not completed\n");
      ret = 1;
    }
  }
  fsync(p1[1]);
  close(p1[1]);
  if(!ret) {
    oalloc = bufsize + 1; /* add extra space for final '\0' */
    *dout = my_malloc(_ALLOC_ID_, oalloc);
    *olen = 0;
    while( (n = read(p2[0], *dout + *olen, bufsize)) > 0) {
      *olen += n;
      dbg(1, "filter_data(): olen=%d, oalloc=%d\n", *olen, oalloc);
      if(*olen + bufsize + 1 >= oalloc) { /* allocate for next read */
        oalloc = *olen + bufsize + 1; /* add extra space for final '\0' */
        oalloc = ((oalloc << 2) + oalloc) >> 2; /* size up 1.25x */
        dbg(1, "filter_data() read %ld bytes, reallocate dout to %ld bytes, bufsize=%ld\n", n, oalloc, bufsize);
        my_realloc(_ALLOC_ID_, dout, oalloc);
      }
    }
    if(*olen) (*dout)[*olen] = '\0'; /* so (if ascii) it can be used as a string */
  }
  if(n < 0 || !*olen) {
    if(oalloc) {
      my_free(_ALLOC_ID_, dout);
      *olen = 0;
    }
    fprintf(stderr, "no data read\n");
    ret = 1;
  }
  waitpid(pid, &wstatus, 0); /* write for child process to finish and unzombie it */
  close(p2[0]);
  signal(SIGPIPE, SIG_DFL); /* restore default SIGPIPE signal action */

  if(WIFEXITED(wstatus)) dbg(1, "Child exited normally\n");
  dbg(1, "Child exit status=%d\n", WEXITSTATUS(wstatus));
  if(WIFSIGNALED(wstatus))dbg(1, "Child was terminated by signal\n");
  return ret;
}
#else /* anyone wanting to write a similar function for windows Welcome! */
int filter_data(const char* din, const size_t ilen,
  char** dout, size_t* olen,
  const char* cmd)
{
  *dout = NULL;
  *olen = 0;
  return 1;
}
#endif

/* Caller should free returned buffer */
/* set brk to 1 if you want newlines added */
char *base64_encode(const unsigned char *data, const size_t input_length, size_t *output_length, int brk) {
  static const char b64_enc[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
  };
  static int mod_table[] = {0, 2, 1};
  int i, j, cnt;
  size_t alloc_length;
  char *encoded_data;
  int octet_a, octet_b, octet_c, triple;


  *output_length = 4 * ((input_length + 2) / 3);
  alloc_length = (1 + (*output_length / 4096)) * 4096;
  encoded_data = my_malloc(_ALLOC_ID_, alloc_length);
  if (encoded_data == NULL) return NULL;
  cnt = 0;
  
  for (i = 0, j = 0; i < input_length;) {
    octet_a = i < input_length ? (unsigned char)data[i++] : 0;
    octet_b = i < input_length ? (unsigned char)data[i++] : 0;
    octet_c = i < input_length ? (unsigned char)data[i++] : 0;
    triple = (octet_a << 16) + (octet_b << 8) + octet_c;
    if(j + 10  >= alloc_length) {
       dbg(1, "alloc-length=%ld, j=%d, output_length=%ld\n", alloc_length, j, *output_length);
       alloc_length += 4096;
       my_realloc(_ALLOC_ID_, &encoded_data, alloc_length);
    }
    if(brk && ( (cnt & 31) == 0) ) {
      *output_length += 1;
      encoded_data[j++] = '\n';
    }
    encoded_data[j++] = b64_enc[(triple >> 18) & 0x3F];
    encoded_data[j++] = b64_enc[(triple >> 12) & 0x3F];
    encoded_data[j++] = b64_enc[(triple >> 6) & 0x3F];
    encoded_data[j++] = b64_enc[(triple) & 0x3F];
    ++cnt;
  }
  for (i = 0; i < mod_table[input_length % 3]; ++i)
    encoded_data[*output_length - 1 - i] = '=';
  encoded_data[*output_length] = '\0'; /* add \0 at end so it can be used as a regular char string */
  return encoded_data;
}

/* Caller should free returned buffer */
unsigned char *base64_decode(const char *data, const size_t input_length, size_t *output_length) {
  static const unsigned char b64_dec[256] = {
    0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
    0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
    0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3e, 0x3f, 0x3f, 0x3f, 0x3f,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
    0x3f, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
    0x3f, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
    0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
    0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
    0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
    0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
    0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
    0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
    0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
    0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f
  };
  unsigned char *decoded_data;
  int i, j, sextet[4], triple, cnt, padding;
  size_t actual_length;
  
  actual_length = input_length;
  *output_length = input_length / 4 * 3 + 4; /* add 4 more just in case... */
  padding = 0;
  if (data[input_length - 1] == '=') padding++;
  if (data[input_length - 2] == '=') padding++;
  decoded_data = my_malloc(_ALLOC_ID_, *output_length);
  if (decoded_data == NULL) return NULL;
  cnt = 0;
  for (i = 0, j = 0; i < input_length;) {
    if(data[i] == '\n' || data[i] == ' '  || data[i] == '\r' || data[i] == '\t') {
      dbg(1, "base64_decode(): white space: i=%d, cnt=%d, j=%d\n", i, cnt, j);
      actual_length--;
      ++i;
      continue;
    }
    sextet[cnt & 3] = data[i] == '=' ? 0 : b64_dec[(int)data[i]];
    if((cnt & 3) == 3) {
      triple = (sextet[0] << 18) + (sextet[1] << 12) + (sextet[2] << 6) + (sextet[3]);
      decoded_data[j++] = (unsigned char)((triple >> 16) & 0xFF);
      decoded_data[j++] = (unsigned char)((triple >> 8) & 0xFF);
      decoded_data[j++] = (unsigned char)((triple) & 0xFF);
    }
    ++cnt;
    ++i;
  }
  *output_length = actual_length / 4 * 3 - padding;
  return decoded_data;
}

/* Caller should free returned buffer */
/* set brk to 1 if you want newlines added */
unsigned char *ascii85_encode(const unsigned char *data, const size_t input_length, size_t *output_length) {
  static const char b85_enc[] = {
    '!', '"', '#', '$', '%', '&', '\'', '(',
    ')', '*', '+', ',', '-', '.', '/', '0',
    '1', '2', '3', '4', '5', '6', '7', '8',
    '9', ':', ';', '<', '=', '>', '?', '@',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', '[', '\\', ']', '^', '_', '`',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
    'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u'
  };
  
  int padding = (4-(input_length % 4))%4;
  static unsigned int pow85[] = {1, 85, 7225, 614125, 52200625};
  unsigned char *paddedData = my_calloc(_ALLOC_ID_, input_length+padding, 1);
  unsigned char *encoded_data;
  int i, idx = 0;
  memcpy( paddedData, data, input_length);
  *output_length = 5*(input_length+padding)/4;
  encoded_data = my_malloc(_ALLOC_ID_, *output_length +1);
  encoded_data[*output_length]=0;
  for (i = 0; i < input_length+padding; i+=4)
  {
    unsigned int val = ((unsigned int)(paddedData[i])<<24) +  ((unsigned int)(paddedData[i+1])<<16) +
                    ((unsigned int)(paddedData[i+2])<<8) + ((unsigned int)(paddedData[i+3]));
    if (val==0)
    {
      encoded_data[idx]='z';
      *output_length-=4;
      ++idx;
      continue;
    }
    encoded_data[idx] = (unsigned char)(val / pow85[4]);
    val = val - encoded_data[idx] * pow85[4];
    encoded_data[idx]=b85_enc[encoded_data[idx]];
    ++idx;
    encoded_data[idx] = (unsigned char)(val / pow85[3]);
    val = val - encoded_data[idx] * pow85[3];
    encoded_data[idx]=b85_enc[encoded_data[idx]];
    ++idx;
    encoded_data[idx] = (unsigned char)(val / pow85[2]);
    val = val - encoded_data[idx] * pow85[2];
    encoded_data[idx]=b85_enc[encoded_data[idx]];
    ++idx;
    encoded_data[idx] = (unsigned char)(val / pow85[1]);
    val = val - encoded_data[idx] * pow85[1];
    encoded_data[idx]=b85_enc[encoded_data[idx]];
    ++idx;
    encoded_data[idx] = (unsigned char)val;
    encoded_data[idx]=b85_enc[encoded_data[idx]];
    ++idx;
  }
  my_free(_ALLOC_ID_, &paddedData);
  *output_length-=padding;
  encoded_data[*output_length]=0;
  return encoded_data;
}

/* Non-square 'r x c' matrix 'a' in-place transpose */
void transpose_matrix(double *a, int r, int c)
{
  double t; /* holds element to be replaced, eventually becomes next element to move */
  int size = r * c - 1;
  int next; /* location of 't' to be moved */
  int begin; /* holds start of cycle */
  int i;
  double tmp;
  char *done = my_calloc(_ALLOC_ID_, r, c); /* hash to mark moved elements */

  done[0] = done[size] = 1; /* first and last matrix elements are not moved. */
  i = 1;
  while (i < size) {
    begin = i;
    t = a[i];
    do {
      next = (i * r) % size;
      SWAP(a[next], t, tmp);
      dbg(1, "swap %g <--> %g\n", a[next], t);
      done[i] = 1;
      i = next;
    } while (i != begin);
    /* Get Next Move */
    for (i = 1; i < size && done[i]; i++) ;
  }
  my_free(_ALLOC_ID_, &done);
}

static void skip_raw_ascii_points(int npoints, FILE *fd)
{
  char line[1024];
  int i;
  for(i = 0; i < npoints; i++) {
    while(1) {
      if(!fgets(line, 1024, fd)) {
        dbg(1, "premature end of ascii block\n");
        return;
      }
      if(line[0] == '\n') {
        dbg(1, "found empty line --> break\n");
        break;
      }
    }   
  }
}     

static int read_raw_ascii_point(int ac, double *tmp, FILE *fd)
{
  char line[1024];
  int lines = 0;
  double d, id; /* id = imaginary part for AC */
  int p;
  while(1) {
    if(!fgets(line, 1024, fd)) {
      dbg(1, "premature end of ascii block\n");
      return lines;
    }
    if(line[0] == '\n' || (line[0] == '\r' && line[1] == '\n')) {
      dbg(1, "found empty line --> return\n");
      break;
    }
    if(lines == 0) {
      if(ac) {
        if(sscanf(line,"%d %lf,%lg", &p, &d, &id) != 3) {
          dbg(1, "missing field on first line of ascii data block\n");
          return lines;
        }
        tmp[lines] = d;
        lines++;
        tmp[lines] = id;
      } else {
        if(sscanf(line,"%d %lf", &p, &d) != 2) {
          dbg(1, "missing field on first line of ascii data block\n");
          return lines;
        }
        tmp[lines] = d;
      }
    } else {
      if(ac) {
        if(sscanf(line,"%lf,%lf", &d, &id) != 2) {
          dbg(1, "missing field of ascii data block\n");
          return lines;
        }
        tmp[lines] = d;
        lines++;
        tmp[lines] = id;
      } else {
        #if 0
        if(sscanf(line,"%lf", &d) != 1) {
          dbg(1, "missing field of ascii data block\n");
          return lines;
        }
        tmp[lines] = d;
        #else /* faster */
        tmp[lines] = my_atof(line);
        #endif
      }
    }
    lines++;
  }
  dbg(1, "read_raw_ascii_point() return %d\n", lines);
  return lines;
}

/* SPICE RAWFILE ROUTINES */
/* read the ascii / binary portion of a ngspice raw simulation file
 * data layout in memory arranged to maximize cache locality 
 * when looking up data 
 */
static void read_raw_data_block(int binary, FILE *fd, Raw *raw, int ac)
{
  int i, p, v;
  double *tmp;
  int offset = 0;
  #ifdef __unix__
    long filepos;
  #else
    __int3264 filepos;
  #endif 
  int npoints;
  int rawvars = raw->nvars;

  if(!raw || !raw->npoints) {
    dbg(0, "read_raw_data_block() no raw struct allocated\n");
    return;
  }

  /* we store 4 variables (mag, phase, real and imag) but raw file has only real and imag */
  if(ac) rawvars >>= 1;
  /* read buffer */
  tmp = my_calloc(_ALLOC_ID_, rawvars, (sizeof(double *) ));

  /* if sweep1 and sweep2 are given calculate actual npoints that will be loaded */
  npoints = raw->npoints[raw->datasets];
  if(!(raw->sweep1 == raw->sweep2 && raw->sweep1 == -1.0)) {
    double sweepvar;
    npoints = 0;
    filepos = xftell(fd); /* store file pointer position */
    for(p = 0; p < raw->npoints[raw->datasets]; p++) {
      if(binary) {
        if(fread(tmp, sizeof(double), rawvars, fd) != rawvars) {
           dbg(0, "Warning: binary block is not of correct size\n");
        }
      } else {
        if(read_raw_ascii_point(ac, tmp, fd) != rawvars) {
           dbg(0, "Warning: ascii block is not of correct size\n");
        }
      }
      sweepvar = tmp[0];
      if(sweepvar < raw->sweep1 || sweepvar >= raw->sweep2) continue;
      else npoints++;
    }
    xfseek(fd, filepos, SEEK_SET); /* rewind file pointer */
  }
  for(p = 0 ; p < raw->datasets; p++) {
    offset += raw->npoints[p];
  }
  /* allocate storage for binary block, add one data column for custom data plots */
  if(!raw->values) raw->values = my_calloc(_ALLOC_ID_, raw->nvars + 1, sizeof(SPICE_DATA *));
  for(p = 0 ; p <= raw->nvars; p++) {
    my_realloc(_ALLOC_ID_,
       &raw->values[p], (offset + npoints) * sizeof(SPICE_DATA));
  }
  /* read binary block */
  p = 0;
  for(i = 0; i < raw->npoints[raw->datasets]; i++) {
    if(binary) {
      if(fread(tmp, sizeof(double), rawvars, fd) != rawvars) {
        dbg(0, "Warning: binary block is not of correct size\n");
      }
    } else {
      if(read_raw_ascii_point(ac, tmp, fd) != rawvars) {
         dbg(0, "Warning: ascii block is not of correct size\n");
      }
    }
    if(!(raw->sweep1 == raw->sweep2 && raw->sweep1 == -1.0)) {
      double sweepvar = tmp[0];
      if(sweepvar < raw->sweep1 || sweepvar >= raw->sweep2) continue;
    }

    /* assign to xschem struct, memory aligned per variable, for cache locality */
    if(ac) {
      int vv = 0;
      for(v = 0; v < raw->nvars; v += 4) { /*AC analysis: calculate magnitude */
        vv = v >> 1;
        if( v == 0 )  /* sweep var */
          raw->values[v][offset + p] = (SPICE_DATA)sqrt( tmp[vv] * tmp[vv] + tmp[vv + 1] * tmp[vv + 1]);
        else /* magnitude */
          /* avoid 0 for dB calculations */
          if(tmp[vv] == 0.0 && tmp[vv + 1] == 0.0) raw->values[v][offset + p] = 1e-35f;
          else raw->values[v][offset + p] = 
                  (SPICE_DATA)sqrt(tmp[vv] * tmp[vv] + tmp[vv + 1] * tmp[vv + 1]);
        /* AC analysis: calculate phase */
        if(tmp[vv] == 0.0 && tmp[vv + 1] == 0.0) raw->values[v + 1] [offset + p] = 0.0; 
        else raw->values[v + 1] [offset + p] =
                (SPICE_DATA)(atan2(tmp[vv + 1], tmp[vv]) * 180.0 / XSCH_PI);

        raw->values[v + 2] [offset + p] = (SPICE_DATA)tmp[vv];     /* real part */
        raw->values[v + 3] [offset + p] = (SPICE_DATA)tmp[vv + 1]; /* imaginary part */
      }
    } 
    else for(v = 0; v < raw->nvars; v++) {
      raw->values[v][offset + p] = (SPICE_DATA)tmp[v];
    }
    p++;
  }
  raw->npoints[raw->datasets] = npoints; /* if sweeep1 and sweep2 are given less points are read */
  my_free(_ALLOC_ID_, &tmp);
}

/* parse ascii raw header section:
 * returns: 1 if dataset and variables were read.
 *          0 if transient sim dataset not found
 *         -1 on EOF
 * Typical ascii header of raw file looks like:
 *
 * Title: **.subckt poweramp
 * Date: Thu Nov 21 18:36:25  2019
 * Plotname: Transient Analysis
 * Flags: real
 * No. Variables: 158
 * No. Points: 90267
 * Variables:
 *         0       time    time
 *         1       v(net1) voltage
 *         2       v(vss)  voltage
 *         ...
 *         ...
 *         155     i(v.x1.vd)      current
 *         156     i(v0)   current
 *         157     i(v1)   current
 * Binary:
 */
static int read_dataset(FILE *fd, Raw **rawptr, const char *type)
{ 
  int variables = 0, i, done_points = 0;
  char *line = NULL, *varname = NULL, *lowerline = NULL;
  int n = 0, done_header = 0, ac = 0;
  int exit_status = 0, npoints, nvars;
  int dbglev=1;
  const char *sim_type = NULL;
  Raw *raw;
 
  if(!rawptr) {
    dbg(0, "read_dataset(): NULL rawptr given\n");
    return 0;
  }
  raw = *rawptr;
  if(!raw) {
    dbg(0, "read_dataset(): no raw struct allocated\n");
    return 0;
  }
  dbg(1, "read_dataset(): type=%s\n", type ? type : "<NULL>");
  if(type) {
    if(!my_strcasecmp(type, "spectrum")) type = "ac";
    else if(!my_strcasecmp(type, "sp")) type = "ac";
  }
  while((line = my_fgets(fd, NULL))) {
    my_strdup2(_ALLOC_ID_, &lowerline, line);
    strtolower(lowerline);

    /* after this line comes the ascii or binary blob made of nvars * npoints * sizeof(double) bytes */
    if(!strcmp(line, "Values:\n") || !strcmp(line, "Values:\r\n")) {
      if(sim_type) {
        my_strdup(_ALLOC_ID_, &raw->sim_type, sim_type);
        done_header = 1;
        dbg(dbglev, "read_dataset(): read binary block, nvars=%d npoints=%d\n", nvars, npoints);
        read_raw_data_block(0, fd, raw, ac); 
        raw->datasets++;
        exit_status = 1;
      } else {
        dbg(dbglev, "read_dataset(): skip ascii block, nvars=%d npoints=%d\n", nvars, npoints);
        /* skip ascii block */
        skip_raw_ascii_points(npoints, fd);
      }
      sim_type = NULL; /* ready for next header */
      done_points = 0;
      ac = 0;
    } 

    /* after this line comes the binary blob made of nvars * npoints * sizeof(double) bytes */
    else if(!strcmp(line, "Binary:\n") || !strcmp(line, "Binary:\r\n")) {
      if(sim_type) {
        my_strdup(_ALLOC_ID_, &raw->sim_type, sim_type);
        done_header = 1;
        dbg(dbglev, "read_dataset(): read binary block, nvars=%d npoints=%d\n", nvars, npoints);
        read_raw_data_block(1, fd, raw, ac); 
        raw->datasets++;
        exit_status = 1;
      } else { 
        dbg(dbglev, "read_dataset(): skip binary block, nvars=%d npoints=%d\n", nvars, npoints);
        xfseek(fd, nvars * npoints * sizeof(double), SEEK_CUR); /* skip binary block */
      }
      sim_type = NULL; /* ready for next header */
      done_points = 0;
      ac = 0;
    }
    else if(!strncmp(line, "Flags:", 6) && strstr(lowerline, "complex")) {
      ac = 1;
    }
    /* if type is given (not NULL) choose the simulation that matches type, else take the first one */
    /* if sim_type is set skip all datasets that do not match */
    else if(!strncmp(line, "Plotname:", 9) && strstr(lowerline, "transient analysis")) {
      if(!type) type = "tran";
      if(!strcmp(type, "tran")) sim_type = "tran";
      dbg(dbglev, "read_dataset(): tran sim_type=%s\n", sim_type ? sim_type : "<NULL>");
    }
    else if(!strncmp(line, "Plotname:", 9) && strstr(lowerline, "dc transfer characteristic")) {
      if(!type) type = "dc";
      if(!strcmp(type, "dc")) sim_type = "dc";
      dbg(dbglev, "read_dataset(): dc sim_type=%s\n", sim_type ? sim_type : "<NULL>");
    }
    else if(!strncmp(line, "Plotname:", 9) && strstr(lowerline, "noise spectral density curves")) {
      if(!type) type = "noise";
      if(!strcmp(type, "noise")) sim_type = "noise";
      dbg(dbglev, "read_dataset(): noise sim_type=%s\n", sim_type ? sim_type : "<NULL>");
    }
    else if(!strncmp(line, "Plotname:", 9) && strstr(lowerline, "operating point")) {
      if(!type) type = "op";
      if(!strcmp(type, "op")) sim_type = "op";
      dbg(dbglev, "read_dataset(): op sim_type=%s\n", sim_type ? sim_type : "<NULL>");
    } 
    else if(!strncmp(line, "Plotname:", 9) && strstr(lowerline, "integrated noise")) {
      if(!type) type = "op";
      else if(!strcmp(type, "noise")) {
        sim_type = "noise";
      }
      if(!strcmp(type, "op")) sim_type = "op";
      dbg(dbglev, "read_dataset(): op sim_type=%s\n", sim_type ? sim_type : "<NULL>");
    }
    else if(!strncmp(line, "Plotname:", 9) &&
            ( strstr(lowerline, "ac analysis") ||
              strstr(lowerline, "spectrum") ||
              strstr(lowerline, "sp analysis")) ) {
      ac = 1;
      if(!type) type = "ac";
      if(!strcmp(type, "ac")) sim_type = "ac";
      dbg(dbglev, "read_dataset(): ac sim_type=%s\n", sim_type ? sim_type : "<NULL>");
    }
    else if(!strncmp(line, "Plotname:", 9)) {
      char name[PATH_MAX];
      char *ptr;
      my_strncpy(name, line + 10, S(name));
      ptr = strchr(name ,'\n');
      if(ptr) *ptr = '\0';
      if(name[0]) {
        if(!type) type = name;
        if(!strcmp(type, name)) sim_type = name;
        dbg(dbglev, "read_dataset(): sim_type=%s\n", sim_type ? sim_type : "<NULL>");
      }
    }
    /* points and vars are needed for all sections (also ones we are not interested in)
     * to skip binary blobs */
    else if(!strncmp(line, "No. of Data Rows :", 18)) {
      /* array of number of points of datasets (they are of varialbe length) */
      n = sscanf(line, "No. of Data Rows : %d", &npoints);
      if(n < 1) {
        dbg(0, "read_dataset(): WAARNING: malformed raw file, aborting\n");
        extra_rawfile(3, NULL, NULL, -1.0, -1.0);
        /* free_rawfile(rawptr, 0); */
        exit_status = 0;
        goto read_dataset_done;
      }
      if(sim_type) {
        my_realloc(_ALLOC_ID_, &raw->npoints, (raw->datasets+1) * sizeof(int));
        raw->npoints[raw->datasets] = npoints;
        /* multi-point OP is equivalent to a DC sweep. Change  sim_type */
        if(raw->npoints[raw->datasets] > 1 && !strcmp(sim_type, "op") ) {
          sim_type = "dc";
        }
      }
      done_points = 1;
    }
    else if(!strncmp(line, "No. Variables:", 14)) {
      int multiplier = 1;
      n = sscanf(line, "No. Variables: %d", &nvars);
      dbg(dbglev, "read_dataset(): nvars=%d\n", nvars);

      if(ac) {
        nvars <<= 1;
        multiplier = 2; /* we store 4 vars (mag, ph, re, im) for each raw file var (re, im) */
      }
      if(raw->datasets > 0  && raw->nvars != nvars * multiplier && sim_type) {
        dbg(0, "Xschem requires all datasets to be saved with identical and same number of variables\n");
        dbg(0, "There is a mismatch, so this and following datasets will not be read\n");
        /* exit_status = 1; */ /* do not set, if something useful has been read keep exit status as is */
        goto read_dataset_done;
      }

      if(n < 1) {
        dbg(0, "read_dataset(): WAARNING: malformed raw file, aborting\n");
        extra_rawfile(3, NULL, NULL, -1.0, -1.0);
        /* free_rawfile(rawptr, 0); */
        exit_status = 0;
        goto read_dataset_done;
      }
      if(sim_type) {
        raw->nvars = nvars;
        if(ac) raw->nvars <<= 1; /* we store mag, phase, real and imag from raw real and imag */
      }
    }
    else if(!done_points && !strncmp(line, "No. Points:", 11)) {
      n = sscanf(line, "No. Points: %d", &npoints);
      if(n < 1) {
        dbg(0, "read_dataset(): WAARNING: malformed raw file, aborting\n");
        extra_rawfile(3, NULL, NULL, -1.0, -1.0);
        /* free_rawfile(rawptr, 0); */
        exit_status = 0;
        goto read_dataset_done;
      }
      if(sim_type) {
        my_realloc(_ALLOC_ID_, &raw->npoints, (raw->datasets+1) * sizeof(int));
        raw->npoints[raw->datasets] = npoints;
        /* multi-point OP is equivalent to a DC sweep. Change  sim_type */
        if(raw->npoints[raw->datasets] > 1 && !strcmp(sim_type, "op") ) {
          sim_type = "dc";
        }
      }
    }
    if(sim_type && !done_header && variables) {
      char *ptr;
      /* get the list of lines with index and node name */
      if(!raw->names) raw->names = my_calloc(_ALLOC_ID_, raw->nvars, sizeof(char *));
      if(!raw->cursor_b_val) raw->cursor_b_val = my_calloc(_ALLOC_ID_, raw->nvars, sizeof(double));
      my_realloc(_ALLOC_ID_, &varname, strlen(line) + 1) ;
      n = sscanf(line, "%d %s", &i, varname); /* read index and name of saved waveform */
      if(n < 2) {
        dbg(0, "read_dataset(): WAARNING: malformed raw file, aborting\n");
        extra_rawfile(3, NULL, NULL, -1.0, -1.0);
        /* free_rawfile(rawptr, 0); */
        exit_status = 0;
        goto read_dataset_done;
      }
      strtolower(varname);
      /* transform ':' hierarchy separators (Xyce) to '.' */
      ptr = varname;
      while(*ptr) {
        if(*ptr == ':') *ptr = '.';
        ++ptr;
      }
      if(ac || (sim_type && !strcmp(sim_type, "ac")) ) { /* AC */
        my_strcat(_ALLOC_ID_, &raw->names[i << 2], varname);
        int_hash_lookup(&raw->table, raw->names[i << 2], (i << 2), XINSERT_NOREPLACE);
        if(strstr(varname, "v(") == varname /* || strstr(varname, "i(") == varname */) {
          my_mstrcat(_ALLOC_ID_, &raw->names[(i << 2) + 1], "ph(", varname + 2, NULL);
        } else {
          my_mstrcat(_ALLOC_ID_, &raw->names[(i << 2) + 1], "ph(", varname, ")", NULL);
        }
        int_hash_lookup(&raw->table, raw->names[(i << 2) + 1], (i << 2) + 1, XINSERT_NOREPLACE);

        if(strstr(varname, "v(") == varname /* || strstr(varname, "i(") == varname */) {
          my_mstrcat(_ALLOC_ID_, &raw->names[(i << 2) + 2], "re(", varname + 2, NULL);
        } else {
          my_mstrcat(_ALLOC_ID_, &raw->names[(i << 2) + 2], "re(", varname, ")", NULL);
        }
        int_hash_lookup(&raw->table, raw->names[(i << 2) + 2], (i << 2) + 2, XINSERT_NOREPLACE);

        if(strstr(varname, "v(") == varname /* || strstr(varname, "i(") == varname */) {
          my_mstrcat(_ALLOC_ID_, &raw->names[(i << 2) + 3], "im(", varname + 2, NULL);
        } else {
          my_mstrcat(_ALLOC_ID_, &raw->names[(i << 2) + 3], "im(", varname, ")", NULL);
        }
        int_hash_lookup(&raw->table, raw->names[(i << 2) + 3], (i << 2) + 3, XINSERT_NOREPLACE);

      } else {
        my_strcat(_ALLOC_ID_, &raw->names[i], varname);
        int_hash_lookup(&raw->table, raw->names[i], i, XINSERT_NOREPLACE);
      }
      /* use hash table to store index number of variables */
      dbg(dbglev, "read_dataset(): get node list -> names[%d] = %s\n", i, raw->names[i]);
    }
    /* after this line comes the list of indexes and associated nodes */
    if(sim_type && !strncmp(line, "Variables:", 10)) {
      variables = 1 ;
    }
    my_free(_ALLOC_ID_, &line);
  } /*  while((line = my_fgets(fd, NULL))  */


  /* no analysis was found: delete */
  if(exit_status != 1) {
    free_rawfile(rawptr, 0);
  }
  read_dataset_done:
  if(line) my_free(_ALLOC_ID_, &line);
  if(lowerline) my_free(_ALLOC_ID_, &lowerline);
  if(varname) my_free(_ALLOC_ID_, &varname);
  if(exit_status == 1 && raw->datasets && raw->npoints) {
    dbg(dbglev, "raw file read: datasets=%d, last dataset points=%d, nvars=%d\n",
        raw->datasets,  raw->npoints[raw->datasets-1], raw->nvars);
  }
  return exit_status;
}

void free_rawfile(Raw **rawptr, int dr)
{
  int i;

  Raw *raw;
  if(!rawptr || !*rawptr) {
    dbg(0, "free_rawfile(): no raw file to clear\n");
    if(dr) draw();
    return;
  }
  raw = *rawptr;
  dbg(0, "free_rawfile(): clearing data\n");
  if(raw->names) {
    for(i = 0 ; i < raw->nvars; ++i) {
      my_free(_ALLOC_ID_, &raw->names[i]);
    }
    my_free(_ALLOC_ID_, &raw->names);
    my_free(_ALLOC_ID_, &raw->cursor_b_val);
  }
  if(raw->values) {
    /* free also extra column for custom data plots */
    for(i = 0 ; i <= raw->nvars; ++i) {
      my_free(_ALLOC_ID_, &raw->values[i]);
    }
    my_free(_ALLOC_ID_, &raw->values);
  }
  if(raw->sim_type) my_free(_ALLOC_ID_, &raw->sim_type);
  if(raw->npoints) my_free(_ALLOC_ID_, &raw->npoints);
  if(raw->rawfile) my_free(_ALLOC_ID_, &raw->rawfile);
  if(raw->schname) my_free(_ALLOC_ID_, &raw->schname);
  if(raw->table.table) int_hash_free(&raw->table);
  my_free(_ALLOC_ID_, rawptr);

  if(has_x) {
    tclvareval("set tctx::", xctx->current_win_path, "_waves $simulate_bg", NULL);
    tclvareval(xctx->top_path, ".menubar entryconfigure Waves -background $simulate_bg", NULL);
  }   

  if(dr) draw();
}

/* caller must free returned pointer */
char *base64_from_file(const char *f, size_t *length)
{
  FILE *fd;
  struct stat st;
  unsigned char *s = NULL;
  char *b64s = NULL;
  size_t len;
  int stat_res;

  stat_res = stat(f, &st);
  if (stat_res == 0 && ( (st.st_mode & S_IFMT) == S_IFREG) ) {
    len = st.st_size;
    fd = fopen(f, fopen_read_mode);
    if(fd) {
      size_t bytes_read;
      s = my_malloc(_ALLOC_ID_, len);
      if((bytes_read = fread(s, 1, len, fd)) < len) {
        dbg(0, "base64_from_file(): less bytes FROM %S, got %ld bytes\n", f, bytes_read);
      }
      fclose(fd);
      b64s = base64_encode(s, len, length, 1);
      my_free(_ALLOC_ID_, &s);
    }
    else {
      dbg(0, "base64_from_file(): failed to open file %s for reading\n", f);
    }
  }
  return b64s;
}

/* "spice_data" attribute is set on instance by executing 'xschem embed_rawfile'
 * after seletcing the component */
int raw_read_from_attr(Raw **rawptr, const char *type, double sweep1, double sweep2)
{
  int res = 0;
  unsigned char *s;
  size_t decoded_length;
  FILE *fd;
  char *tmp_filename;
  Raw *raw;

  if(!rawptr) {
    dbg(0, "raw_read_from_attr(): NULL rawptr given\n");
    return res;
  }
  raw = *rawptr;
  if(raw) {
    dbg(0, "raw_read_from_attr(): must clear current raw file before loading new\n");
    return res;
  }
  if(xctx->lastsel==1 && xctx->sel_array[0].type==ELEMENT) {
    xInstance *i = &xctx->inst[xctx->sel_array[0].n];
    const char *b64_spice_data;
    size_t length;
    if(i->prop_ptr && (b64_spice_data = get_tok_value(i->prop_ptr, "spice_data", 0))[0]) {
      length = strlen(b64_spice_data);
      if( (fd = open_tmpfile("embedded_rawfile_", ".raw", &tmp_filename)) ) {
        s = base64_decode(b64_spice_data, length, &decoded_length);
        fwrite(s, decoded_length, 1, fd);
        fclose(fd);
        my_free(_ALLOC_ID_, &s);
        res = raw_read(tmp_filename, rawptr, type, sweep1, sweep2);
        unlink(tmp_filename);
      } else {
        dbg(0, "raw_read_from_attr(): failed to open file %s for reading\n", tmp_filename);
      }
    }
  }
  return res;
}

int raw_add_vector(const char *varname, const char *expr, int sweep_idx)
{
  int f;
  int res = 0;
  Raw *raw = xctx->raw;
  if(!raw || !raw->values) return 0;

  if(!int_hash_lookup(&raw->table, varname, 0, XLOOKUP)) {
    raw->nvars++;
    my_realloc(_ALLOC_ID_, &raw->names, raw->nvars * sizeof(char *));
    my_realloc(_ALLOC_ID_, &raw->cursor_b_val, raw->nvars * sizeof(double));
    raw->cursor_b_val[raw->nvars - 1] = 0.0;
    raw->names[raw->nvars - 1] = NULL;
    my_strdup2(_ALLOC_ID_, &raw->names[raw->nvars - 1], varname);
    int_hash_lookup(&raw->table, raw->names[raw->nvars - 1], raw->nvars - 1, XINSERT_NOREPLACE);
    my_realloc(_ALLOC_ID_, &raw->values, (raw->nvars + 1) * sizeof(SPICE_DATA *));
    raw->values[raw->nvars] = NULL;
    my_realloc(_ALLOC_ID_, &raw->values[raw->nvars], raw->allpoints * sizeof(SPICE_DATA));
    res = 1;
  }
  if(expr) {
    plot_raw_custom_data(sweep_idx, 0, raw->allpoints -1, expr, varname);
  } else if(res == 1) {
    for(f = 0; f < raw->allpoints; f++) {
      raw->values[raw->nvars - 1][f] = 0.0;
    }
  }
  return res;
}

/* read a ngspice raw file (with data portion in binary format) */
int raw_read(const char *f, Raw **rawptr, const char *type, double sweep1, double sweep2)
{
  int res = 0;
  FILE *fd;
  Raw *raw;

  if(!rawptr) {
    dbg(0, "NULL rawptr pointer given\n");
    return res;
  }
  if(*rawptr) {
    dbg(0, "raw_read(): must clear current raw file before loading new\n");
    return res;
  }
  dbg(1, "raw_read(): type=%s\n", type ? type : "<NULL>");
  *rawptr = my_calloc(_ALLOC_ID_, 1, sizeof(Raw));
  raw = *rawptr;
  raw->level = -1; 
  raw->annot_p = -1;
  raw->sweep1 = sweep1;
  raw->sweep2 = sweep2;
  raw->annot_sweep_idx = -1;

  int_hash_init(&raw->table, HASHSIZE);
  fd = fopen(f, fopen_read_mode);
  if(fd) {
    if((res = read_dataset(fd, rawptr, type)) == 1) {
      int i;
      set_modify(-2); /* clear text floater caches */
      my_strdup2(_ALLOC_ID_, &raw->rawfile, f);
      my_strdup2(_ALLOC_ID_, &raw->schname, xctx->sch[xctx->currsch]);
      raw->level = xctx->currsch;
      raw->allpoints = 0;
      for(i = 0; i < raw->datasets; ++i) {
        raw->allpoints +=  raw->npoints[i];
      }
      dbg(0, "Raw file data read: %s\n", f);
      dbg(0, "points=%d, vars=%d, datasets=%d sim_type=%s\n", 
             raw->allpoints, raw->nvars, raw->datasets, raw->sim_type ? raw->sim_type : "<NULL>");
    } else {
      /* free_rawfile(rawptr, 0); */ /* do not free: already done in read_dataset()->extra_rawfile() */
      dbg(0, "raw_read(): no useful data found\n");
    }
    fclose(fd);
    if(has_x) {
      if(sch_waves_loaded() >= 0) {
        tclvareval("set tctx::", xctx->current_win_path, "_waves Green", NULL);
        tclvareval(xctx->top_path, ".menubar entryconfigure Waves -background Green", NULL);
      } else {
        tclvareval("set tctx::", xctx->current_win_path, "_waves $simulate_bg", NULL);
        tclvareval(xctx->top_path, ".menubar entryconfigure Waves -background $simulate_bg", NULL);
      }
    }
    return res;
  }
  dbg(0, "raw_read(): failed to open file %s for reading\n", f);
  return 0;
}

int raw_deletevar(const char *name)
{
  int ret = 0;
  int i, n;
  Raw *raw = xctx->raw; 
  Int_hashentry *entry;

  n = get_raw_index(name, &entry);
  if(n < 0) return ret;
  dbg(1, "n=%d, %s \n", n, entry->token);
  int_hash_lookup(&raw->table, entry->token, 0, XDELETE);
  my_free(_ALLOC_ID_, &raw->names[n]);
  for(i = n + 1; i < raw->nvars; i++) {
    int_hash_lookup(&raw->table, raw->names[i], i - 1, XINSERT); /* update hash table */
    raw->names[i - 1] = raw->names[i];
  }
  my_free(_ALLOC_ID_, &raw->values[n]);
  for(i = n + 1; i <= raw->nvars; i++) {
    raw->values[i - 1] = raw->values[i];
  }
  raw->nvars--;
  my_realloc(_ALLOC_ID_, &raw->names, sizeof(char *) * raw->nvars);
  my_realloc(_ALLOC_ID_, &raw->values, sizeof(SPICE_DATA *) * raw->nvars + 1);
  ret = 1;
  return ret;
}

/* create a new raw file with '(max - min) / step' points with only a sweep variable in it. */
int new_rawfile(const char *name, const char *type, const char *sweepvar,
                       double start, double end, double step)
{
  int i;
  int ret = 1;
  Raw *raw;
  int number = (int)floor((end - start) / step) + 1;

  /* if not already done insert base raw file (if there is one) into xctx->extra_raw_arr[0] */
  if(xctx->raw && xctx->extra_raw_n == 0) {
    xctx->extra_raw_arr[xctx->extra_raw_n] = xctx->raw;
    xctx->extra_raw_n++;
  }

  if(xctx->extra_raw_n < MAX_RAW_N && name && type) {
    for(i = 0; i < xctx->extra_raw_n; i++) {
      if(xctx->extra_raw_arr[i]->sim_type &&
         !strcmp(xctx->extra_raw_arr[i]->rawfile, name) &&
         !strcmp(xctx->extra_raw_arr[i]->sim_type, type)
        ) break;
    }

    for(i = 0; i < xctx->extra_raw_n; i++) {
      if( !strcmp(xctx->extra_raw_arr[i]->rawfile, name) &&
          !strcmp(xctx->extra_raw_arr[i]->sim_type, type)
        ) break;
    } 
    if(i >= xctx->extra_raw_n) { /* file not already loaded: read it and switch to it */
      double t;

      xctx->raw = my_calloc(_ALLOC_ID_, 1, sizeof(Raw));
      raw = xctx->raw;
      raw->level = -1; 
      raw->sweep1 = -1.0;
      raw->sweep2 = -1.0;
      raw->annot_p = -1;
      raw->annot_sweep_idx = -1;
  
      int_hash_init(&raw->table, HASHSIZE);
      my_strdup2(_ALLOC_ID_, &raw->rawfile, name);
      my_strdup2(_ALLOC_ID_, &raw->schname, xctx->sch[xctx->currsch]);
      my_strdup(_ALLOC_ID_, &raw->sim_type, type);
      raw->level = xctx->currsch;
      my_realloc(_ALLOC_ID_, &raw->npoints, 1 * sizeof(int)); /* for now assume only one dataset */
      raw->datasets = 1;
      raw->allpoints = number;
      raw->npoints[0] = number;
      raw->nvars = 1;
      raw->values = my_calloc(_ALLOC_ID_, raw->nvars + 1, sizeof(SPICE_DATA *));
      raw->values[0] = my_calloc(_ALLOC_ID_, number,  sizeof(SPICE_DATA));
      raw->values[1] = my_calloc(_ALLOC_ID_, number,  sizeof(SPICE_DATA));
      raw->names = my_calloc(_ALLOC_ID_, raw->nvars, sizeof(char *));
      raw->cursor_b_val = my_calloc(_ALLOC_ID_, raw->nvars, sizeof(double));
      my_strdup2(_ALLOC_ID_, &raw->names[0], sweepvar);
      int_hash_lookup(&raw->table, raw->names[0], 0, XINSERT_NOREPLACE);

      for(i = 0; i < number; i++) {
        t = start + i * step;
        raw->values[0][i] = t;
      }

      xctx->extra_raw_arr[xctx->extra_raw_n] = xctx->raw;
      xctx->extra_prev_idx = xctx->extra_idx;
      xctx->extra_idx = xctx->extra_raw_n;
      xctx->extra_raw_n++;
    } else { /* file found: switch to it */
      dbg(1, "new_rawfile() %d read: found: switch to it\n", i);
      xctx->extra_prev_idx = xctx->extra_idx;
      xctx->extra_idx = i; 
      xctx->raw = xctx->extra_raw_arr[xctx->extra_idx];
    }
  } else {
    ret = 0;
  }
  return ret;
}

/* what == 0: do nothing and return 0
 * what == 1: read another raw file and switch to it (make it the current one)
 *            if type == table use table_read() to read an ascii table
 * what == 2: switch raw file. If filename given switch to that one, 
 * else if filename is an integer switch to that raw file index,
 * else switch to next
 * what == 3: remove a raw file. If no filename given remove all
 * what == 4: print info
 * what == 5: switch back to previous
 * return 1 if sucessfull, 0 otherwise
 */
int extra_rawfile(int what, const char *file, const char *type, double sweep1, double sweep2)
{
  int i;
  int ret = 1;
  char f[PATH_MAX];

  if(type && !type[0]) type = NULL; /* empty string as type will be considered NULL */

  dbg(1, "extra_rawfile(): what=%d, file=%s, type=%s\n",
      what, file ? file : "<NULL>", type ? type : "<NULL>");
  if(what == 0) return 0;
  /* if not already done insert base raw file (if there is one) into xctx->extra_raw_arr[0] */
  if(xctx->raw && xctx->extra_raw_n == 0) {
    xctx->extra_raw_arr[xctx->extra_raw_n] = xctx->raw;
    xctx->extra_raw_n++;
  }
  /* **************** table_read ************* */
  if(what == 1 && xctx->extra_raw_n < MAX_RAW_N && file && (type && !strcmp(type, "table"))) {
    tclvareval("subst {", file, "}", NULL);
    my_strncpy(f, tclresult(), S(f));
    dbg(1, "extra_rawfile: table_read: f=%s\n", f);
    for(i = 0; i < xctx->extra_raw_n; i++) {
      if( !strcmp(xctx->extra_raw_arr[i]->rawfile, f)) break;
    } 
    if(i >= xctx->extra_raw_n) { /* file not already loaded: read it and switch to it */
      int read_ret = 0;
      Raw *save;
      save = xctx->raw; 
      xctx->raw = NULL;
      read_ret = table_read(f);
      my_strdup(_ALLOC_ID_, &xctx->raw->sim_type, type);
      if(read_ret) {
        xctx->extra_raw_arr[xctx->extra_raw_n] = xctx->raw;
        xctx->extra_prev_idx = xctx->extra_idx;
        xctx->extra_idx = xctx->extra_raw_n;
        xctx->extra_raw_n++;
      } else {
        ret = 0; /* not found so did not switch */
        dbg(0, "extra_rawfile() read: %s not found or no \"%s\" analysis\n", f, type);
        if(xctx->extra_raw_n) { /* only restore if raw wiles were not deleted due to a failure in read_raw() */
          xctx->raw = save; /* restore */
          xctx->extra_prev_idx = xctx->extra_idx;
        }
      }
    } else { /* file found: switch to it */
      dbg(1, "extra_rawfile() %d read: found: switch to it\n", i);
      xctx->extra_prev_idx = xctx->extra_idx;
      xctx->extra_idx = i; 
      xctx->raw = xctx->extra_raw_arr[xctx->extra_idx];
    }
  /* **************** read ************* */
  } else if(what == 1 && xctx->extra_raw_n < MAX_RAW_N && file /* && type*/) {
    tclvareval("subst {", file, "}", NULL);
    my_strncpy(f, tclresult(), S(f));
    if(type) {
      if(!my_strcasecmp(type, "spectrum")) type = "ac";
      else if(!my_strcasecmp(type, "sp")) type = "ac";
    }
    for(i = 0; i < xctx->extra_raw_n; i++) {
      if(xctx->extra_raw_arr[i]->sim_type && 
         !strcmp(xctx->extra_raw_arr[i]->rawfile, f) &&
         (!type || !strcmp(xctx->extra_raw_arr[i]->sim_type, type) )
        ) break;
    }
    if(i >= xctx->extra_raw_n) { /* file not already loaded: read it and switch to it */
      int read_ret = 0;
      Raw *save;
      save = xctx->raw;
      xctx->raw = NULL;
      read_ret = raw_read(f, &xctx->raw, type, sweep1, sweep2);
      if(read_ret) {
        dbg(1, "extra_rawfile(): read %s %s, switch to it. raw->sim_type=%s\n", f,
          type ? type : "<NULL>", xctx->raw->sim_type ? xctx->raw->sim_type : "<NULL>");
        xctx->extra_raw_arr[xctx->extra_raw_n] = xctx->raw;
        xctx->extra_prev_idx = xctx->extra_idx;
        xctx->extra_idx = xctx->extra_raw_n;
        xctx->extra_raw_n++;
      } else {
        ret = 0; /* not found so did not switch */
        dbg(0, "extra_rawfile() read: %s not found or no \"%s\" analysis\n", f, type ? type : "<unspecified>");
        if(xctx->extra_raw_n) { /* only restore if raw wiles were not deleted due to a failure in read_raw() */
          xctx->raw = save; /* restore */
          xctx->extra_prev_idx = xctx->extra_idx;
        }
      }
    } else { /* file found: switch to it */
      dbg(1, "extra_rawfile() %d read: found: switch to it\n", i);
      xctx->extra_prev_idx = xctx->extra_idx;
      xctx->extra_idx = i;
      xctx->raw = xctx->extra_raw_arr[xctx->extra_idx];
    }
  /* **************** switch ************* */
  } else if(what == 2 && xctx->extra_raw_n > 0) {
    if(file && type) {
      tclvareval("subst {", file, "}", NULL);
      my_strncpy(f, tclresult(), S(f));
      for(i = 0; i < xctx->extra_raw_n; i++) {
        dbg(1, "      extra_rawfile(): checking with %s\n", xctx->extra_raw_arr[i]->rawfile);
        if(xctx->extra_raw_arr[i]->sim_type &&
           !strcmp(xctx->extra_raw_arr[i]->rawfile, f) &&
           !strcmp(xctx->extra_raw_arr[i]->sim_type, type)
          ) break;
      }
      if(i < xctx->extra_raw_n) { /* if file found switch to it ... */
        dbg(1, "extra_rawfile() switch: found: switch to it\n");
        xctx->extra_prev_idx = xctx->extra_idx;
        xctx->extra_idx = i;
      } else {
        dbg(1, "extra_rawfile() switch: %s not found or no %s analysis\n", f, type ? type : "<NULL>");
        ret = 0;
      }
    } else if(file && isonlydigit(file) ) {
      tclvareval("subst {", file, "}", NULL);
      my_strncpy(f, tclresult(), S(f));
      i = atoi(file);
      if(i >= 0 && i < xctx->extra_raw_n) { /* if file found switch to it ... */
        dbg(1, "extra_rawfile() switch %d: found: switch %d to it\n", xctx->extra_idx, i);
        xctx->extra_prev_idx = xctx->extra_idx; 
        xctx->extra_idx = i;
      } else {
        dbg(0, "extra_rawfile() switch: %s not found or no %s analysis\n", f, type ? type : "<NULL>");
        ret = 0;
      }
    } else { /* switch to next */
      xctx->extra_prev_idx = xctx->extra_idx;
      xctx->extra_idx = (xctx->extra_idx + 1) % xctx->extra_raw_n;
    }
    xctx->raw = xctx->extra_raw_arr[xctx->extra_idx];
  /* **************** switch back ************* */
  } else if(what == 5 && xctx->extra_raw_n > 0) {
    int tmp;
    tmp = xctx->extra_idx;
    xctx->extra_idx = xctx->extra_prev_idx;
    xctx->extra_prev_idx = tmp;
    xctx->raw = xctx->extra_raw_arr[xctx->extra_idx];
  /* **************** clear ************* */
  } else if(what == 3) {
    if(!file) { /* clear all */
      if(xctx->extra_raw_n == 0) ret = 0;
      for(i = 0; i < xctx->extra_raw_n; i++) {
        free_rawfile(&xctx->extra_raw_arr[i], 0);
      }
      tcleval("array unset ngspice::ngspice_data");
      xctx->raw = NULL;
      xctx->extra_prev_idx = 0;
      xctx->extra_idx = 0;
      xctx->extra_raw_n = 0;
    } else { /* clear provided file if found, switch to first in remaining if any */
      int found = 0;
      tclvareval("subst {", file, "}", NULL);
      my_strncpy(f, tclresult(), S(f));
      if(xctx->extra_raw_n > 0 ) {
        for(i = 0; i < xctx->extra_raw_n; i++) {
          if(type && type[0] &&
              !strcmp(xctx->extra_raw_arr[i]->rawfile, f) &&
              !strcmp(xctx->extra_raw_arr[i]->sim_type, type)
              ) {
            free_rawfile(&xctx->extra_raw_arr[i], 0);
            found++;
            continue;
          } else if( !(type && type[0]) && !strcmp(xctx->extra_raw_arr[i]->rawfile, f)) {
            free_rawfile(&xctx->extra_raw_arr[i], 0);
            found++;
            continue;
          }
          if(found) {
            xctx->extra_raw_arr[i - found] = xctx->extra_raw_arr[i];
          }
        }
        if(found != 0) {
          xctx->extra_raw_n -= found;
          xctx->extra_idx = 0;
          xctx->extra_prev_idx = 0;
          if(xctx->extra_raw_n) {
            xctx->raw = xctx->extra_raw_arr[0];
          } else {
            tcleval("array unset ngspice::ngspice_data");
            xctx->raw = NULL;
          }
        } else ret = 0;
      } else ret = 0;
    }
  /* **************** info ************* */
  } else if(what == 4) {
    if(xctx->raw) {
      dbg(1, "extra_raw_n = %d\n", xctx->extra_raw_n);
      Tcl_AppendResult(interp, my_itoa(xctx->extra_idx), " current\n", NULL);
      for(i = 0; i < xctx->extra_raw_n; i++) {
        Tcl_AppendResult(interp, my_itoa(i), " ", xctx->extra_raw_arr[i]->rawfile, " ", 
            xctx->extra_raw_arr[i]->sim_type ? xctx->extra_raw_arr[i]->sim_type : "<NULL>", "\n",  NULL);
      }
    }
  } else {
    ret = 0;
  }
  return ret;
}

int update_op()
{
  int res = 0, p = 0, i;
  tcleval("array unset ngspice::ngspice_data");
  if(xctx->raw && xctx->raw->values) {
    xctx->raw->annot_p = 0;
    dbg(1, "update_op(): nvars=%d\n", xctx->raw->nvars);
    for(i = 0; i < xctx->raw->nvars; ++i) {
      char s[100];
      res = 1;
      xctx->raw->cursor_b_val[i] =  xctx->raw->values[i][p];
      my_snprintf(s, S(s), "%.4g", xctx->raw->values[i][p]);
      dbg(1, "%s = %g\n", xctx->raw->names[i], xctx->raw->values[i][p]);
      tclvareval("array set ngspice::ngspice_data [list {",  xctx->raw->names[i], "} ", s, "]", NULL);
    }
    tclvareval("set ngspice::ngspice_data(n\\ vars) ", my_itoa( xctx->raw->nvars), NULL);
    tclvareval("set ngspice::ngspice_data(n\\ points) 1", NULL);
  } 
  return res;
}

/* Read data organized as a table
 * First line is the header line containing variable names.
 * data is presented in column format after the header line
 * First column is sweep (x-axis) variable
 * Double empty lines start a new dataset
 * Single empty lines are ignored
 * Datasets can have different # of lines.
 * new dataset do not start with a header row.
 * Lines beginning with '#' are comments and ignored
 *
 *    time    var_a   var_b   var_c
 * # this is a comment, ignored
 *     0.0     0.0     1.8    0.3
 *   <single empty line: ignored>
 *     0.1     0.0     1.5    0.6
 *     ...     ...     ...    ...
 *   <empty line>
 *   <Second empty line: start new dataset>
 *     0.0     0.0     1.8    0.3
 *     0.1     0.0     1.5    0.6
 *     ...     ...     ...    ...
 *
 */
int table_read(const char *f)
{
  int res = 0;
  FILE *fd;
  int ufd;
  size_t lines, bytes;
  char *line = NULL, *line_ptr, *line_save;
  const char *line_tok;
  Raw *raw;
  if(xctx->raw) {
    dbg(0, "table_read(): must clear current data file before loading new\n");
    return 0;
  }
  xctx->raw = my_calloc(_ALLOC_ID_, 1, sizeof(Raw));
  raw = xctx->raw;
  raw->level = -1; 
  raw->annot_p = -1;
  raw->annot_sweep_idx = -1;

  /* quick inspect file and get upper bound of number of data lines */
  ufd = open(f, O_RDONLY);
  if(ufd < 0) goto err;
  count_lines_bytes(ufd, &lines, &bytes);
  close(ufd);

  int_hash_init(&raw->table, HASHSIZE);
  fd = fopen(f, fopen_read_mode);
  if(fd) {
    int nline = 0;
    int field;
    int npoints = 0;
    int dataset_points = 0;
    int prev_prev_empty = 0, prev_empty = 0;
    res = 1;
    /* read data line by line */
    while((line = my_fgets(fd, NULL))) {
      int empty = 1;
      if(line[0] == '#') {
        goto clear;
      }
      line_ptr = line;
      while(*line_ptr) { /* non empty line ? */
        if(*line_ptr != ',' && *line_ptr != ' ' && *line_ptr != '\t' && *line_ptr != '\n') {
          empty = 0;
          break;
        }
        line_ptr++;
      }
      if(empty) {
        prev_prev_empty = prev_empty;
        prev_empty = 1;
        goto clear;
      }
      if(!raw->datasets || (prev_prev_empty == 1 && prev_empty == 1) ) {
        raw->datasets++;
        my_realloc(_ALLOC_ID_, &raw->npoints, raw->datasets * sizeof(int));
        dataset_points = 0;
      }
      prev_prev_empty = prev_empty = 0;
      line_ptr = line;
      field = 0;
      #ifdef __unix__
      while( (line_tok = strtok_r(line_ptr, ", \t\n", &line_save)) ) {
      #else
      while( (line_tok = my_strtok_r(line_ptr, ", \t\n", "", 0, &line_save)) ) {
      #endif
        line_ptr = NULL;
        /* dbg(1,"%s ", line_tok); */
        if(nline == 0) { /* header line */
          my_realloc(_ALLOC_ID_, &raw->names, (field + 1) * sizeof(char *));
          raw->names[field] = NULL;
          my_strcat(_ALLOC_ID_, &raw->names[field], line_tok);
          int_hash_lookup(&raw->table, raw->names[field], field, XINSERT_NOREPLACE);
          raw->nvars = field + 1;
        } else { /* data line */
          if(field >= raw->nvars) break;
          #if SPICE_DATA_TYPE == 1 /* float */
          raw->values[field][npoints] = (SPICE_DATA)my_atof(line_tok);
          #else /* double */
          raw->values[field][npoints] = (SPICE_DATA)my_atod(line_tok);
          #endif
        }
        ++field;
      }
      if(nline) { /* skip header line for npoints calculation*/
        ++npoints;
        dataset_points++;
      }
      raw->npoints[raw->datasets - 1] = dataset_points;
      /* dbg(1, "\n"); */
      ++nline;
      if(nline == 1) {
        int f;
        raw->values = my_calloc(_ALLOC_ID_, raw->nvars + 1, sizeof(SPICE_DATA *));
        for(f = 0; f <= raw->nvars; f++) { /* one extra column for wave expressions */
          my_realloc(_ALLOC_ID_, &raw->values[f], lines * sizeof(SPICE_DATA));
        }
      }
      clear:
      my_free(_ALLOC_ID_, &line);
    } /* while(line ....) */
    raw->allpoints = 0;
    if(res == 1) {
      int i;
      my_strdup2(_ALLOC_ID_, &raw->rawfile, f);
      my_strdup2(_ALLOC_ID_, &raw->schname, xctx->sch[xctx->currsch]);
      raw->level = xctx->currsch;
      raw->allpoints = 0;
      for(i = 0; i < raw->datasets; ++i) {
        raw->allpoints +=  raw->npoints[i];
      }
      dbg(0, "Table file data read: %s\n", f);
      dbg(0, "points=%d, vars=%d, datasets=%d\n",
             raw->allpoints, raw->nvars, raw->datasets);
    } else {
      dbg(0, "table_read(): no useful data found\n");
    }
    raw->cursor_b_val = my_calloc(_ALLOC_ID_, raw->nvars, sizeof(double));
    fclose(fd);
    return res;
  }
  err:
  dbg(0, "table_read(): failed to open file %s for reading\n", f);
  return 0;
}

int raw_get_pos(const char *node, double value, int dset, int from_start, int to_end)
{
  int x = -1;
  Raw *raw = xctx->raw;
  int idx = -1;

  if(sch_waves_loaded() >= 0) {
    if(dset >= raw->datasets) dset = raw->datasets - 1;
    if(dset < 0) dset = 0;
    idx = get_raw_index(node, NULL);
    if(idx >= 0) {
      double vx;
      int start, end;
      int sign, lastpoint = raw->npoints[dset] - 1;
      double vstart, vend;

      start = from_start >= 0 ? from_start : 0;
      end = to_end >= 0 ? to_end : lastpoint;
      if(start > lastpoint) start = lastpoint;
      if(end > lastpoint) end = lastpoint;
      vstart = get_raw_value(dset, idx, start);
      vend = get_raw_value(dset, idx, end);
      sign = (vend > vstart) ? 1 : -1;
      if( sign * value >= sign * vstart && sign * value <= sign * vend) {
        while(1) {
          x = (start + end ) / 2;
          vx = get_raw_value(dset, idx, x);
          if(abs(end - start) <= 1) break;
          if( sign * vx > sign * value) end = x;
          else start = x;
        }
      }
    }
  }

  return x;
}
/* given a node XXyy try XXyy , xxyy, XXYY, v(XXyy), v(xxyy), V(XXYY) */
int get_raw_index(const char *node, Int_hashentry **entry_ret)
{
  char inode[512];
  char vnode[512];
  Int_hashentry *entry;


  dbg(1, "get_raw_index(): node=%s\n", node);
  if(sch_waves_loaded() >= 0) {
    my_strncpy(inode, node, S(inode));
    entry = int_hash_lookup(&xctx->raw->table, inode, 0, XLOOKUP);
    if(!entry) {
      strtoupper(inode);
      entry = int_hash_lookup(&xctx->raw->table, inode, 0, XLOOKUP);
    }
    if(!entry) {
      strtolower(inode);
      entry = int_hash_lookup(&xctx->raw->table, inode, 0, XLOOKUP);
    }
    if(!entry) {
      my_snprintf(vnode, S(vnode), "v(%s)", inode);
      entry = int_hash_lookup(&xctx->raw->table, vnode, 0, XLOOKUP);
    }
    if(!entry && strstr(inode, "i(v.x")) {
      char *ptr = inode;
      inode[2] = 'i';
      inode[3] = '(';
      ptr += 2;
      entry = int_hash_lookup(&xctx->raw->table, ptr, 0, XLOOKUP);
    }

    if(entry_ret) *entry_ret = entry;
    if(entry) return entry->value;
  }
  return -1;
}

/* store calculated custom graph data for later retrieval as in running average calculations 
 * what: 
 * 0: clear data
 * 1: store value
 * 2: retrieve value
 */
static double ravg_store(int what , int i, int p, int last, double value)
{
  static int imax = 0;
  static double **arr = NULL;
  int j;

  /* 
  dbg(0, "ravg_store: what= %d i= %d p= %d last= %d value=%g\n",
              what, i, p, last, value);
  */
  if(what == 2) {
    return arr[i][p];
  } else if(what == 1) {
    if(i >= imax) {
      int new_size = i + 4;
      my_realloc(_ALLOC_ID_, &arr, sizeof(double *) * new_size);
      for(j = imax; j < new_size; ++j) {
        arr[j] = my_calloc(_ALLOC_ID_, last + 1, sizeof(double));
      }
      imax = new_size;
    }
    arr[i][p] = value;
  } else if(what == 0 && imax) {
    for(j = 0; j < imax; ++j) {
      my_free(_ALLOC_ID_, &arr[j]);
    }
    my_free(_ALLOC_ID_, &arr);
    imax = 0;
  }
  return 0.0;
}

#define STACKMAX 200
#define SPICE_NODE 1
#define NUMBER 2
#define PLUS 3
#define MINUS 4
#define MULT 5
#define DIVIS 6
#define POW 7
#define SIN 8
#define COS 9
#define EXP 10
#define LN 11
#define LOG10 12
#define ABS 13
#define SGN 14
#define SQRT 15
#define TAN 16
#define TANH 17
#define INTEG 18
#define AVG 19
#define DERIV 20
#define EXCH 21
#define DUP 22
#define RAVG 23 /* running average */
#define DB20 24
#define DERIV0 25 /* derivative to first sweep variable, regardless of specified sweep_idx */
#define PREV 26 /* previous point */
#define DEL 27 /* delay by an anount of sweep axis distance */
#define MAX 28 /* clip data above given argument */
#define MIN 29 /* clip data below given argument */
#define ATAN 30
#define ASIN 31
#define ACOS 32
#define COSH 33
#define SINH 34
#define ATANH 35
#define ACOSH 36
#define ASINH 37
#define IDX 38 /* index of point in raw file (0, 1, 2, ...) */
#define REAL 39
#define IMAG 40
#define GT 41 /* greater than */
#define LT 42 /* greater than */
#define EQ 43
#define NE 44
#define GE 45
#define LE 46
#define COND 47 /* conditional expression: X cond Y ? --> X if conf == 1 else Y */
#define CPH 48 /* continuous phase. Instead of -180..+180 avoid discontinuities */


#define ORDER_DERIV 1 /* 1 or 2: 1st order or 2nd order differentiation. 1st order is faster */

typedef struct {
  int i;
  double d;
  int idx; /* spice index node */
  double prevy;
  double prevprevy;
  double prev;
  int prevp;
} Stack1;

int plot_raw_custom_data(int sweep_idx, int first, int last, const char *expr, const char *yname)
{
  int i, p, idx;
  const char *n;
  char *endptr, *ntok_copy = NULL, *ntok_save, *ntok_ptr;
  Stack1 stack1[STACKMAX];
  double stack2[STACKMAX]={0}, tmp, result, avg;
  int stackptr1 = 0, stackptr2 = 0;
  SPICE_DATA *y;
  SPICE_DATA *x = xctx->raw->values[sweep_idx];
  SPICE_DATA *sweepx = xctx->raw->values[0];

  /* dbg(0, "sweep_idx=%d first=%d last=%d expr=%s, yname=%s\n",
   *    sweep_idx, first, last, expr ? expr : "<NULL>", yname ? yname: "<NULL>");
   */
  y = xctx->raw->values[xctx->raw->nvars]; /* custom plot data column */
  if(yname != NULL) {
    int yidx = get_raw_index(yname, NULL);
    if(yidx >= 0) {
      y = xctx->raw->values[yidx]; /* provided index */
    }
  }
  my_strdup2(_ALLOC_ID_, &ntok_copy, expr);
  ntok_ptr = ntok_copy;
  dbg(1, "plot_raw_custom_data(): expr=%s, first=%d, last=%d\n", expr, first, last);
  while( (n = my_strtok_r(ntok_ptr, " \t\n", "", 0, &ntok_save)) ) {
    if(stackptr1 >= STACKMAX -2) {
      dbg(0, "stack overflow in graph expression parsing. Interrupted\n");
      my_free(_ALLOC_ID_, &ntok_copy);
      return -1;
    }
    ntok_ptr = NULL;
    dbg(1, "  plot_raw_custom_data(): n = %s\n", n);
    if(!strcmp(n, "+")) stack1[stackptr1++].i = PLUS;
    else if(!strcmp(n, "==")) stack1[stackptr1++].i = EQ;
    else if(!strcmp(n, "!=")) stack1[stackptr1++].i = NE;
    else if(!strcmp(n, ">")) stack1[stackptr1++].i = GT;
    else if(!strcmp(n, "<")) stack1[stackptr1++].i = LT;
    else if(!strcmp(n, ">=")) stack1[stackptr1++].i = GE;
    else if(!strcmp(n, "<=")) stack1[stackptr1++].i = LE;
    else if(!strcmp(n, "-")) stack1[stackptr1++].i = MINUS;
    else if(!strcmp(n, "*")) stack1[stackptr1++].i = MULT;
    else if(!strcmp(n, "/")) stack1[stackptr1++].i = DIVIS;
    else if(!strcmp(n, "**")) stack1[stackptr1++].i = POW;
    else if(!strcmp(n, "?")) stack1[stackptr1++].i = COND; /* conditional expression */
    else if(!strcmp(n, "atan()")) stack1[stackptr1++].i = ATAN;
    else if(!strcmp(n, "cph()")) stack1[stackptr1++].i = CPH;
    else if(!strcmp(n, "asin()")) stack1[stackptr1++].i = ASIN;
    else if(!strcmp(n, "acos()")) stack1[stackptr1++].i = ACOS;
    else if(!strcmp(n, "tan()")) stack1[stackptr1++].i = TAN;
    else if(!strcmp(n, "sin()")) stack1[stackptr1++].i = SIN;
    else if(!strcmp(n, "cos()")) stack1[stackptr1++].i = COS;
    else if(!strcmp(n, "abs()")) stack1[stackptr1++].i = ABS;
    else if(!strcmp(n, "sgn()")) stack1[stackptr1++].i = SGN;
    else if(!strcmp(n, "sqrt()")) stack1[stackptr1++].i = SQRT;
    else if(!strcmp(n, "tanh()")) stack1[stackptr1++].i = TANH;
    else if(!strcmp(n, "cosh()")) stack1[stackptr1++].i = COSH;
    else if(!strcmp(n, "sinh()")) stack1[stackptr1++].i = SINH;
    else if(!strcmp(n, "atanh()")) stack1[stackptr1++].i = ATANH;
    else if(!strcmp(n, "acosh()")) stack1[stackptr1++].i = ACOSH;
    else if(!strcmp(n, "asinh()")) stack1[stackptr1++].i = ASINH;
    else if(!strcmp(n, "exp()")) stack1[stackptr1++].i = EXP;
    else if(!strcmp(n, "ln()")) stack1[stackptr1++].i = LN;
    else if(!strcmp(n, "log10()")) stack1[stackptr1++].i = LOG10;
    else if(!strcmp(n, "integ()")) {
      if(first > 0) first--;
      stack1[stackptr1++].i = INTEG;
    }
    else if(!strcmp(n, "avg()")) stack1[stackptr1++].i = AVG;
    else if(!strcmp(n, "ravg()")) stack1[stackptr1++].i = RAVG;
    else if(!strcmp(n, "max()")) stack1[stackptr1++].i = MAX;
    else if(!strcmp(n, "min()")) stack1[stackptr1++].i = MIN;
    else if(!strcmp(n, "im()")) stack1[stackptr1++].i = IMAG;
    else if(!strcmp(n, "re()")) stack1[stackptr1++].i = REAL;
    else if(!strcmp(n, "del()")) {
      int d, t = 0, p = 0;
      /* set 'first' to beginning of dataset containing 'first' */
      for(d = 0; d < xctx->raw->datasets; d++) {
        t += xctx->raw->npoints[d];
        if(t > first) break;
        p = t;
      }
      first = p;
      stack1[stackptr1++].i = DEL;
    }
    else if(!strcmp(n, "db20()")) stack1[stackptr1++].i = DB20;
    else if(!strcmp(n, "deriv()")) {
      stack1[stackptr1++].i = DERIV;
      if(first > 0) first--;
      if(first > 0) first--;
    }
    else if(!strcmp(n, "deriv0()")) {
      stack1[stackptr1++].i = DERIV0;
      if(first > 0) first--;
      if(first > 0) first--;
    }
    else if(!strcmp(n, "prev()")) {
      stack1[stackptr1++].i = PREV;
      if(first > 0) first--;
    }
    else if(!strcmp(n, "exch()")) stack1[stackptr1++].i = EXCH;
    else if(!strcmp(n, "dup()")) stack1[stackptr1++].i = DUP;
    else if(!strcmp(n, "idx()")) stack1[stackptr1++].i = IDX;
    else if( (strtod(n, &endptr)), endptr > n) { /* NUMBER */
      stack1[stackptr1].i = NUMBER;
      stack1[stackptr1++].d = atof_spice(n);
    }
    else { /* SPICE_NODE */
      idx = get_raw_index(n, NULL);
      if(idx == -1) {
        dbg(1, "plot_raw_custom_data(): no data found: %s\n", n);
        my_free(_ALLOC_ID_, &ntok_copy);
        return -1; /* no data found in raw file */
      }
      stack1[stackptr1].i = SPICE_NODE;
      stack1[stackptr1].idx = idx;
      stackptr1++;
    }
  } /* while(n = my_strtok_r(...) */
  my_free(_ALLOC_ID_, &ntok_copy);
  for(p = first ; p <= last; p++) {
    stackptr2 = 0;
    for(i = 0; i < stackptr1; ++i) {
      if(stack1[i].i == NUMBER) { /* number */
        stack2[stackptr2++] = stack1[i].d;
      }
      else if(stack1[i].i == IDX) {
        stack2[stackptr2++] = (double)p;
      }
      else if(stack1[i].i == SPICE_NODE && stack1[i].idx < xctx->raw->nvars) { /* spice node */
        stack2[stackptr2++] =  xctx->raw->values[stack1[i].idx][p];
      }
      if(stackptr2 > 2) { /* 3 argument operators */
        if(stack1[i].i == COND) { /*  X cond Y ? --> X if conf == 1 else Y */
          dbg(0, "%g %g %g\n",  stack2[stackptr2 - 3],  stack2[stackptr2 - 2],  stack2[stackptr2 - 1]);
          stack2[stackptr2 - 3] = stack2[stackptr2 - 2] ? stack2[stackptr2 - 3] : stack2[stackptr2 - 1];
          stackptr2 -= 2;
        }
      }
      if(stackptr2 > 1) { /* 2 argument operators */
        switch(stack1[i].i) {
          case PLUS:
            stack2[stackptr2 - 2] = stack2[stackptr2 - 2] + stack2[stackptr2 - 1];
            stackptr2--;
            break;
          case MINUS:
            stack2[stackptr2 - 2] = stack2[stackptr2 - 2] - stack2[stackptr2 - 1];
            stackptr2--;
            break;
          case EQ:
            stack2[stackptr2 - 2] = (stack2[stackptr2 - 2] == stack2[stackptr2 - 1]);
            stackptr2--;
            break;
          case NE:
            stack2[stackptr2 - 2] = (stack2[stackptr2 - 2] != stack2[stackptr2 - 1]);
            stackptr2--;
            break;
          case GT:
            stack2[stackptr2 - 2] = (stack2[stackptr2 - 2] > stack2[stackptr2 - 1]);
            stackptr2--;
            break;
          case LT:
            stack2[stackptr2 - 2] = (stack2[stackptr2 - 2] < stack2[stackptr2 - 1]);
            stackptr2--;
            break;
          case GE:
            stack2[stackptr2 - 2] = (stack2[stackptr2 - 2] >= stack2[stackptr2 - 1]);
            stackptr2--;
            break;
          case LE:
            stack2[stackptr2 - 2] = (stack2[stackptr2 - 2] <= stack2[stackptr2 - 1]);
            stackptr2--;
            break;
          case MULT:
            stack2[stackptr2 - 2] = stack2[stackptr2 - 2] * stack2[stackptr2 - 1];
            stackptr2--;
            break;
          case DIVIS:
            if(stack2[stackptr2 - 1]) {
              stack2[stackptr2 - 2] = stack2[stackptr2 - 2] / stack2[stackptr2 - 1];
            } else if(stack2[stackptr2 - 2] == 0.0) {
              stack2[stackptr2 - 2] = 0;
            } else {
              stack2[stackptr2 - 2] =  y[p - 1];
            }
            stackptr2--;
            break;
          case DEL:
            tmp = stack2[stackptr2 - 1];
            ravg_store(1, i, p, last, stack2[stackptr2 - 2]);
            if(fabs(x[p] - x[first]) <= tmp) {
              result = stack2[stackptr2 - 2];
              stack1[i].prevp = first;
            } else {
              double delta =  fabs(x[p] - x[stack1[i].prevp]);
              while(stack1[i].prevp <= last && delta > tmp) {
                stack1[i].prevp++;
                delta = fabs(x[p] - x[stack1[i].prevp]);
              }
              /* choose the closest:  stack1[i].prev or stack1[i].prev - 1 */
              if( stack1[i].prevp > 0) {
                double delta1 =  fabs(x[p] - x[stack1[i].prevp-1]);
                if(fabs(delta1 - tmp) < fabs(delta - tmp)) stack1[i].prevp--;
              }
              result =  ravg_store(2, i, stack1[i].prevp, 0, 0);
            }
            stack2[stackptr2 - 2] = result;
            stackptr2--;
            break;
          case RAVG:
            if( p == first ) {
              result = 0;
              stack1[i].prevy = stack2[stackptr2 - 2];
              stack1[i].prev = 0;
              stack1[i].prevp = first;
            } else {
              result = stack1[i].prev + (x[p] - x[p - 1]) * (stack1[i].prevy + stack2[stackptr2 - 2]) * 0.5;
              stack1[i].prevy =  stack2[stackptr2 - 2];
              stack1[i].prev = result;
            }
            ravg_store(1, i, p, last, result);
            
            while(stack1[i].prevp <= last && x[p] - x[stack1[i].prevp] > stack2[stackptr2 - 1]) {
              /* dbg(1, "%g  -->  %g\n", x[stack1[i].prevp], x[p]); */
              stack1[i].prevp++;
            }
            stack2[stackptr2 - 2] = (result - ravg_store(2, i, stack1[i].prevp, 0, 0)) / stack2[stackptr2 - 1];
            /* dbg(1, "result=%g ravg_store=%g\n", result,  ravg_store(2, i, stack1[i].prevp, 0, 0)); */
            stackptr2--;
            break;
          case MAX:
            stack2[stackptr2 - 2] = stack2[stackptr2 - 2] < stack2[stackptr2 - 1] ?
                                    stack2[stackptr2 - 1] :  stack2[stackptr2 - 2];
            stackptr2--;
            break;
          case MIN:
            stack2[stackptr2 - 2] = stack2[stackptr2 - 2] > stack2[stackptr2 - 1] ?
                                    stack2[stackptr2 - 1] :  stack2[stackptr2 - 2];
            stackptr2--;
            break;
          case POW:
            stack2[stackptr2 - 2] =  pow(stack2[stackptr2 - 2], stack2[stackptr2 - 1]);
            stackptr2--;
            break;
          case REAL:
            stack2[stackptr2 - 2] = stack2[stackptr2 - 2] * cos(stack2[stackptr2 - 1] * XSCH_PI / 180.);
            stackptr2--;
            break;
          case IMAG:
            stack2[stackptr2 - 2] = stack2[stackptr2 - 2] * sin(stack2[stackptr2 - 1] * XSCH_PI / 180.);
            stackptr2--;
            break;
          case EXCH:
            tmp = stack2[stackptr2 - 2];
            stack2[stackptr2 - 2] = stack2[stackptr2 - 1];
            stack2[stackptr2 - 1] = tmp;
            break;
          default:
            break;
        } /* switch(...) */
      } /* if(stackptr2 > 1) */
      if(stackptr2 > 0) { /* 1 argument operators */
        switch(stack1[i].i) {
          case AVG:
            if( p == first ) {
              avg = stack2[stackptr2 - 1];
              stack1[i].prevy = stack2[stackptr2 - 1];
              stack1[i].prev = stack2[stackptr2 - 1];
            } else {
              if((x[p] != x[first])) {
                avg = stack1[i].prev * (x[p - 1] - x[first]) + 
                    (x[p] - x[p - 1]) * (stack1[i].prevy + stack2[stackptr2 - 1]) * 0.5;
                avg /= (x[p] - x[first]);
              } else  {
                avg = stack1[i].prev;
              }
              stack1[i].prevy =  stack2[stackptr2 - 1];
              stack1[i].prev = avg;
            }
            stack2[stackptr2 - 1] =  avg;
            break;
          case DUP:
            stack2[stackptr2] =  stack2[stackptr2 - 1];
            stackptr2++;
            break;
          case INTEG: 
            if( p == first ) {
              result = 0;
              stack1[i].prevy = stack2[stackptr2 - 1];
              stack1[i].prev = 0;
            } else {
              result = stack1[i].prev + (x[p] - x[p - 1]) * (stack1[i].prevy + stack2[stackptr2 - 1]) * 0.5;
              stack1[i].prevy =  stack2[stackptr2 - 1];
              stack1[i].prev = result;
            }
            stack2[stackptr2 - 1] =  result;
            break;
          #if ORDER_DERIV==1
          case DERIV: 
            if( p == first ) {
              result = 0;
              stack1[i].prevy = stack2[stackptr2 - 1];
              stack1[i].prev = 0;
            } else {
              if((x[p] != x[p - 1])) 
                result =  (stack2[stackptr2 - 1] - stack1[i].prevy) / (x[p] - x[p - 1]);
              else
                result = stack1[i].prev;
              stack1[i].prevy = stack2[stackptr2 - 1] ;
              stack1[i].prev = result;
            }
            stack2[stackptr2 - 1] =  result;
            break;
          case DERIV0:
            if( p == first ) {
              result = 0;
              stack1[i].prevy = stack2[stackptr2 - 1];
              stack1[i].prev = 0;
            } else {
              if((sweepx[p] != sweepx[p - 1]))
                result =  (stack2[stackptr2 - 1] - stack1[i].prevy) / (sweepx[p] - sweepx[p - 1]);
              else
                result = stack1[i].prev;
              stack1[i].prevy = stack2[stackptr2 - 1] ;
              stack1[i].prev = result;
            }
            stack2[stackptr2 - 1] =  result;
            break;
          #else /* second order backward differentiation formulas */
          case DERIV:
            if( p == first ) {
              result = 0;
              stack1[i].prevy = stack2[stackptr2 - 1];
              stack1[i].prev = 0;
            } else if(p == first + 1) {
              if((x[p] != x[p - 1]))
                result =  (stack2[stackptr2 - 1] - stack1[i].prevy) / (x[p] - x[p - 1]);
              else
                result = stack1[i].prev;
              stack1[i].prevprevy =  stack1[i].prevy;
              stack1[i].prevy = stack2[stackptr2 - 1] ;
              stack1[i].prev = result;
            } else {
              double a = x[p - 2] - x[p];
              double c = x[p - 1] - x[p];
              double b = a * a / 2.0;
              double d = c * c / 2.0;
              double b_on_d = b / d;
              double fa = stack1[i].prevprevy;
              double fb = stack1[i].prevy;
              double fc = stack2[stackptr2 - 1];
              if(a != 0.0) 
                result = (fa - b_on_d * fb - (1 - b_on_d) * fc ) / (a - c * b_on_d);
              else
                result = stack1[i].prev;
              stack1[i].prevprevy =  stack1[i].prevy;
              stack1[i].prevy = stack2[stackptr2 - 1] ;
              stack1[i].prev = result;
            }
            stack2[stackptr2 - 1] =  result;
            break;
          case DERIV0:
            if( p == first ) {
              result = 0;
              stack1[i].prevy = stack2[stackptr2 - 1];
              stack1[i].prev = 0;
            } else if(p == first + 1) {
              if((sweepx[p] != sweepx[p - 1]))
                result =  (stack2[stackptr2 - 1] - stack1[i].prevy) / (sweepx[p] - sweepx[p - 1]);
              else
                result = stack1[i].prev;
              stack1[i].prevprevy =  stack1[i].prevy;
              stack1[i].prevy = stack2[stackptr2 - 1] ;
              stack1[i].prev = result;
            } else {
              double a = sweepx[p - 2] - sweepx[p];
              double c = sweepx[p - 1] - sweepx[p];
              double b = a * a / 2.0;
              double d = c * c / 2.0;
              double b_on_d = b / d;
              double fa = stack1[i].prevprevy;
              double fb = stack1[i].prevy;
              double fc = stack2[stackptr2 - 1];
              if(a != 0.0)
                result = (fa - b_on_d * fb - (1 - b_on_d) * fc ) / (a - c * b_on_d);
              else
                result = stack1[i].prev;
              stack1[i].prevprevy =  stack1[i].prevy;
              stack1[i].prevy = stack2[stackptr2 - 1] ;
              stack1[i].prev = result;
            }
            stack2[stackptr2 - 1] =  result;
            break;
          #endif
          case PREV:
            if(p == first) {
              result = stack2[stackptr2 - 1];
            } else {
              result =  stack1[i].prev;
            }
            stack1[i].prev =  stack2[stackptr2 - 1];
            stack2[stackptr2 - 1] =  result;
            break;
          case CPH:
            if(p == first) {
              result = stack2[stackptr2 - 1];
            } else {
              double ph = stack2[stackptr2 - 1];
              double prev_ph = stack1[i].prev;
              result = ph - (360.) * floor((ph - prev_ph)/(360.) + 0.5);
            }
            stack1[i].prev =  result;
            stack2[stackptr2 - 1] =  result;
            break;
          case SQRT:
            stack2[stackptr2 - 1] =  sqrt(stack2[stackptr2 - 1]);
            break;
          case TANH:
            stack2[stackptr2 - 1] =  tanh(stack2[stackptr2 - 1]);
            break;
          case COSH:
            stack2[stackptr2 - 1] =  cosh(stack2[stackptr2 - 1]);
            break;
          case SINH:
            stack2[stackptr2 - 1] =  sinh(stack2[stackptr2 - 1]);
            break;
          case ATANH:
            tmp = stack2[stackptr2 - 1];
            tmp = 0.5 * log( (1 + tmp) / (1 - tmp) );
            stack2[stackptr2 - 1] =  tmp;
            break;
          case ACOSH:
            tmp = stack2[stackptr2 - 1];
            tmp = log(tmp + sqrt(tmp * tmp - 1));
            stack2[stackptr2 - 1] =  tmp;
            break;
          case ASINH:
            tmp = stack2[stackptr2 - 1];
            tmp = log(tmp + sqrt(tmp * tmp + 1));
            stack2[stackptr2 - 1] =  tmp;
            break;
          case TAN:
            stack2[stackptr2 - 1] =  tan(stack2[stackptr2 - 1]);
            break;
          case SIN:
            stack2[stackptr2 - 1] =  sin(stack2[stackptr2 - 1]);
            break;
          case COS:
            stack2[stackptr2 - 1] =  cos(stack2[stackptr2 - 1]);
            break;
          case ATAN:
            stack2[stackptr2 - 1] =  atan(stack2[stackptr2 - 1]);
            break;
          case ASIN:
            stack2[stackptr2 - 1] =  asin(stack2[stackptr2 - 1]);
            break;
          case ACOS:
            stack2[stackptr2 - 1] =  acos(stack2[stackptr2 - 1]);
            break;
          case ABS:
            stack2[stackptr2 - 1] =  fabs(stack2[stackptr2 - 1]);
            break;
          case EXP:
            stack2[stackptr2 - 1] =  exp(stack2[stackptr2 - 1]);
            break;
          case LN:
            stack2[stackptr2 - 1] =  mylog(stack2[stackptr2 - 1]);
            break;
          case LOG10:
            stack2[stackptr2 - 1] =  mylog10(stack2[stackptr2 - 1]);
            break;
          case DB20:
            stack2[stackptr2 - 1] =  20 * mylog10(stack2[stackptr2 - 1]);
            break;
          case SGN:
            stack2[stackptr2 - 1] = stack2[stackptr2 - 1] > 0.0 ? 1 : 
                                    stack2[stackptr2 - 1] < 0.0 ? -1 : 0; 
            break;
        } /* switch(...) */
      } /* if(stackptr2 > 0) */
    } /* for(i = 0; i < stackptr1; ++i) */
    y[p] = (SPICE_DATA)stack2[0];
  } /* for(p = first ...) */
  ravg_store(0, 0, 0, 0, 0.0); /* clear data */
  return xctx->raw->nvars;
}

double get_raw_value(int dataset, int idx, int point)
{
  int i, ofs;
  ofs = 0;
  if(dataset >= xctx->raw->datasets) { 
    dbg(0, "get_raw_value(): dataset(%d) >= datasets(%d)\n", dataset,  xctx->raw->datasets);
  }
  if(xctx->raw && xctx->raw->values && dataset < xctx->raw->datasets) {
    if(dataset == -1) {
      if(point < xctx->raw->allpoints)
        return xctx->raw->values[idx][point];
    } else {
      for(i = 0; i < dataset; ++i) {
        ofs += xctx->raw->npoints[i];
      }
      if(ofs + point < xctx->raw->allpoints) {
        return xctx->raw->values[idx][ofs + point];
      }
    }
  }
  return 0.0;
}
/* END SPICE RAWFILE ROUTINES */

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
  int unget = 1;

  if(firstchar == -1) {
     firstchar = fgetc(fp);
     unget = 0;
  }
  dbg(dbg_level, "SKIP RECORD\n");
  if(firstchar != '{') {
    dbg(dbg_level, "%c", firstchar);
  }
  while((c = fgetc(fp)) != EOF) {
    if (c=='\r') continue;
    if(c == '\n') {
      dbg(dbg_level, "\n");
      if(unget) ungetc(c, fp); /* so following read_line does not skip next line */
      break;
    }
    if(c == '{') {
      ungetc(c, fp);
      load_ascii_string(&str, fp);
      dbg(dbg_level, "{%s}", str ? str : "");
    } else {
      dbg(dbg_level, "%c", c);
    }
  }
  dbg(dbg_level,   "END SKIP RECORD\n");
  my_free(_ALLOC_ID_, &str);
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
static const char *random_string(const char *prefix)
{
  static const char *charset="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  static const int random_size=10;
  static char str[PATH_MAX]; /* safe even with multiple schematics, if immediately copied */
  size_t prefix_size, i;
  static unsigned short once=1; /* safe even with multiple schematics, set once and never changed */
  int idx;
  if(once) {
    srand((unsigned short) time(NULL));
    once=0;
  }
  prefix_size = strlen(prefix);
  str[0]='/';
  memcpy(str+1, prefix, prefix_size);
  for(i=prefix_size+1; i < prefix_size + random_size+1; ++i) {
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
  for(i=0; i<5; ++i) {
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
FILE *open_tmpfile(char *prefix, char *suffix, char **filename)
{
  static char str[PATH_MAX]; /* safe even with multiple schematics, if immediately copied */
  int i;
  FILE *fd;
  struct stat buf;
  for(i=0; i<5; ++i) {
    my_snprintf(str, S(str), "%s%s%s", tclgetvar("XSCHEM_TMP_DIR"), random_string(prefix), suffix);
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
  *       count, tmp->x1, tmp->y1, tmp->x2, tmp->y2); */
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
  int c;
  size_t len, strbuf_pos = 0;
  static char *strbuf = NULL; /* safe even with multiple schematics */
  static size_t strbuf_size=0; /* safe even with multiple schematics */

  if(ptr == NULL) {
    if( fd == NULL) { /* used to clear static data */
       my_free(_ALLOC_ID_, &strbuf);
       strbuf_size = 0;
       return;
    }
    if(newline) fputs("{}\n", fd);
    else fputs("{}", fd);
    return;
  }
  len = strlen(ptr) + CADCHUNKALLOC;
  if(strbuf_size < len ) my_realloc(_ALLOC_ID_, &strbuf, (strbuf_size = len));

  strbuf[strbuf_pos++] = '{';
  while( (c = *ptr++) ) {
    if(strbuf_pos > strbuf_size - 6) my_realloc(_ALLOC_ID_, &strbuf, (strbuf_size += CADCHUNKALLOC));
    if( c=='\\' || c=='{' || c=='}') strbuf[strbuf_pos++] = '\\';
    strbuf[strbuf_pos++] = (char)c;
  }
  strbuf[strbuf_pos++] = '}';
  if(newline) strbuf[strbuf_pos++] = '\n';
  strbuf[strbuf_pos] = '\0';
  fwrite(strbuf, 1, strbuf_pos, fd);
}

static void save_embedded_symbol(xSymbol *s, FILE *fd)
{
  int c, i, j;

  fprintf(fd, "v {xschem version=%s file_version=%s}\n", XSCHEM_VERSION, XSCHEM_FILE_VERSION);
  fprintf(fd, "K ");
  save_ascii_string(s->prop_ptr,fd, 1);
  fprintf(fd, "G {}\n");
  fprintf(fd, "V {}\n");
  fprintf(fd, "S {}\n");
  fprintf(fd, "E {}\n");
  for(c=0;c<cadlayers; ++c)
  {
   xLine *ptr;
   ptr=s->line[c];
   for(i=0;i<s->lines[c]; ++i)
   {
    fprintf(fd, "L %d %.16g %.16g %.16g %.16g ", c,ptr[i].x1, ptr[i].y1,ptr[i].x2,
     ptr[i].y2 );
    save_ascii_string(ptr[i].prop_ptr,fd, 1);
   }
  }
  for(c=0;c<cadlayers; ++c)
  {
   xRect *ptr;
   ptr=s->rect[c];
   for(i=0;i<s->rects[c]; ++i)
   {
    fprintf(fd, "B %d %.16g %.16g %.16g %.16g ", c,ptr[i].x1, ptr[i].y1,ptr[i].x2,
     ptr[i].y2);
    save_ascii_string(ptr[i].prop_ptr,fd, 1);
   }
  }
  for(c=0;c<cadlayers; ++c)
  {
   xArc *ptr;
   ptr=s->arc[c];
   for(i=0;i<s->arcs[c]; ++i)
   {
    fprintf(fd, "A %d %.16g %.16g %.16g %.16g %.16g ", c,ptr[i].x, ptr[i].y,ptr[i].r,
     ptr[i].a, ptr[i].b);
    save_ascii_string(ptr[i].prop_ptr,fd, 1);
   }
  }
  for(i=0;i<s->texts; ++i)
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
  for(c=0;c<cadlayers; ++c)
  {
   xPoly *ptr;
   ptr=s->poly[c];
   for(i=0;i<s->polygons[c]; ++i)
   {
    fprintf(fd, "P %d %d ", c,ptr[i].points);
    for(j=0;j<ptr[i].points; ++j) {
      fprintf(fd, "%.16g %.16g ", ptr[i].x[j], ptr[i].y[j]);
    }
    save_ascii_string(ptr[i].prop_ptr,fd, 1);
   }
  }
}

static void save_inst(FILE *fd, int select_only)
{
 int i, oldversion;
 xInstance *inst;
 char *tmp = NULL;
 int *embedded_saved = NULL;

 dbg(1, "save_inst(): saving instances\n");
 inst=xctx->inst;
 oldversion = !strcmp(xctx->file_version, "1.0");
 embedded_saved = my_calloc(_ALLOC_ID_, xctx->symbols, sizeof(int));
 for(i=0;i<xctx->instances; ++i)
 {
  int ptr = inst[i].ptr;
  dbg(1, "save_inst() %s: instance %d, name=%s\n", xctx->current_name, i, inst[i].name);
  if(ptr == -1) {
    dbg(0, "save_inst(): WARNING: inst %d .ptr = -1 ... current_name=%s\n", i, xctx->current_name);
  }
  if (select_only && inst[i].sel != SELECTED) continue;
  if(ptr >=0) xctx->sym[ptr].flags &=~EMBEDDED;
  fputs("C ", fd);
  if(oldversion) {
    my_strdup2(_ALLOC_ID_, &tmp, add_ext(inst[i].name, ".sym"));
    save_ascii_string(tmp, fd, 0);
    my_free(_ALLOC_ID_, &tmp);
  } else {
    save_ascii_string(inst[i].name, fd, 0);
  }
  fprintf(fd, " %.16g %.16g %hd %hd ",inst[i].x0, inst[i].y0, inst[i].rot, inst[i].flip );
  save_ascii_string(inst[i].prop_ptr,fd, 1);
  if(ptr >= 0 && embedded_saved && !embedded_saved[ptr] && inst[i].embed) {
    embedded_saved[ptr] = 1;
    fprintf(fd, "[\n");
    save_embedded_symbol( xctx->sym+ptr, fd);
    fprintf(fd, "]\n");
    xctx->sym[ptr].flags |= EMBEDDED;
  }
 }
 my_free(_ALLOC_ID_, &embedded_saved);
}

static void save_wire(FILE *fd, int select_only)
{
 int i;
 xWire *ptr;

 ptr=xctx->wire;
 for(i=0;i<xctx->wires; ++i)
 {
   if (select_only && ptr[i].sel != SELECTED) continue;
  fprintf(fd, "N %.16g %.16g %.16g %.16g ",ptr[i].x1, ptr[i].y1, ptr[i].x2,
     ptr[i].y2);
  save_ascii_string(ptr[i].prop_ptr,fd, 1);
 }
}

static void save_text(FILE *fd, int select_only)
{
 int i;
 xText *ptr;
 ptr=xctx->text;
 for(i=0;i<xctx->texts; ++i)
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

static void save_polygon(FILE *fd, int select_only)
{
    int c, i, j;
    xPoly *ptr;
    for(c=0;c<cadlayers; ++c)
    {
     ptr=xctx->poly[c];
     for(i=0;i<xctx->polygons[c]; ++i)
     {
       if (select_only && ptr[i].sel != SELECTED) continue;
      fprintf(fd, "P %d %d ", c,ptr[i].points);
      for(j=0;j<ptr[i].points; ++j) {
        fprintf(fd, "%.16g %.16g ", ptr[i].x[j], ptr[i].y[j]);
      }
      save_ascii_string(ptr[i].prop_ptr,fd, 1);
     }
    }
}

static void save_arc(FILE *fd, int select_only)
{
    int c, i;
    xArc *ptr;
    for(c=0;c<cadlayers; ++c)
    {
     ptr=xctx->arc[c];
     for(i=0;i<xctx->arcs[c]; ++i)
     {
       if (select_only && ptr[i].sel != SELECTED) continue;
      fprintf(fd, "A %d %.16g %.16g %.16g %.16g %.16g ", c,ptr[i].x, ptr[i].y,ptr[i].r,
       ptr[i].a, ptr[i].b);
      save_ascii_string(ptr[i].prop_ptr,fd, 1);
     }
    }
}

static void save_box(FILE *fd, int select_only)
{
    int c, i;
    xRect *ptr;
    for(c=0;c<cadlayers; ++c)
    {
     ptr=xctx->rect[c];
     for(i=0;i<xctx->rects[c]; ++i)
     {
       if (select_only && ptr[i].sel != SELECTED) continue;
      fprintf(fd, "B %d %.16g %.16g %.16g %.16g ", c,ptr[i].x1, ptr[i].y1,ptr[i].x2,
       ptr[i].y2);
      save_ascii_string(ptr[i].prop_ptr,fd, 1);
     }
    }
}

static void save_line(FILE *fd, int select_only)
{
    int c, i;
    xLine *ptr;
    for(c=0;c<cadlayers; ++c)
    {
     ptr=xctx->line[c];
     for(i=0;i<xctx->lines[c]; ++i)
     {
       if (select_only && ptr[i].sel != SELECTED) continue;
      fprintf(fd, "L %d %.16g %.16g %.16g %.16g ", c,ptr[i].x1, ptr[i].y1,ptr[i].x2,
       ptr[i].y2 );
      save_ascii_string(ptr[i].prop_ptr,fd, 1);
     }
    }
}

static void write_xschem_file(FILE *fd)
{
  size_t ty=0;
  char *tmpstring = NULL;
  size_t tmpstring_size;
  char *header_ptr = xctx->header_text ? xctx->header_text : "";
  tmpstring_size = strlen(header_ptr) + 100;
  tmpstring = my_malloc(_ALLOC_ID_, tmpstring_size);
  if(xctx->header_text && xctx->header_text[0]) {
    my_snprintf(tmpstring, tmpstring_size, "xschem version=%s file_version=%s\n%s",
        XSCHEM_VERSION, XSCHEM_FILE_VERSION, header_ptr);
  } else {
    my_snprintf(tmpstring, tmpstring_size, "xschem version=%s file_version=%s",
        XSCHEM_VERSION, XSCHEM_FILE_VERSION);
  }
  fprintf(fd, "v ");
  save_ascii_string(tmpstring, fd, 1);
  my_free(_ALLOC_ID_, &tmpstring);

  if(xctx->schvhdlprop && !xctx->schsymbolprop) {
    get_tok_value(xctx->schvhdlprop,"type",0);
    ty = xctx->tok_size;
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
  xctx->text[i].floater_instname=NULL;
  xctx->text[i].floater_ptr=NULL;
  xctx->text[i].sel=0;
  load_ascii_string(&xctx->text[i].prop_ptr,fd);
  set_text_flags(&xctx->text[i]);
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
    if(!strboolcmp(get_tok_value(ptr[i].prop_ptr, "bus", 0), "true") ) ptr[i].bus = 1;
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
    if(name[0] == '/') my_strdup2(_ALLOC_ID_, &xctx->inst[i].name, rel_sym_path(name));
    else my_strdup2(_ALLOC_ID_, &xctx->inst[i].name, name);
    #else 
    my_strdup2(_ALLOC_ID_, &xctx->inst[i].name, rel_sym_path(name));
    #endif
    my_free(_ALLOC_ID_, &tmp);
    if(fscanf(fd, "%lf %lf %hd %hd", &xctx->inst[i].x0, &xctx->inst[i].y0,
       &xctx->inst[i].rot, &xctx->inst[i].flip) < 4) {
      fprintf(errfp,"WARNING: missing fields for INSTANCE object, ignoring.\n");
      read_line(fd, 0);
    } else {
      xctx->inst[i].color=-10000;
      xctx->inst[i].sel=0;
      xctx->inst[i].ptr=-1; /*04112003 was 0 */
      xctx->inst[i].instname=NULL;
      xctx->inst[i].prop_ptr=NULL;
      xctx->inst[i].lab=NULL; /* assigned in link_symbols_to_instances */
      xctx->inst[i].node=NULL;
      load_ascii_string(&prop_ptr,fd);
      my_strdup(_ALLOC_ID_, &xctx->inst[i].prop_ptr, prop_ptr);

      set_inst_flags(&xctx->inst[i]);
      dbg(2, "load_inst(): n=%d name=%s prop=%s\n", i, xctx->inst[i].name? xctx->inst[i].name:"<NULL>",
               xctx->inst[i].prop_ptr? xctx->inst[i].prop_ptr:"<NULL>");
      xctx->instances++;
    }
    my_free(_ALLOC_ID_, &prop_ptr);
}

static void load_polygon(FILE *fd)
{
    const char *fill_ptr;
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
    ptr[i].x = my_calloc(_ALLOC_ID_, points, sizeof(double));
    ptr[i].y = my_calloc(_ALLOC_ID_, points, sizeof(double));
    ptr[i].selected_point= my_calloc(_ALLOC_ID_, points, sizeof(unsigned short));
    ptr[i].points=points;
    ptr[i].sel=0;
    for(j=0;j<points; ++j) {
      if(fscanf(fd, "%lf %lf ",&(ptr[i].x[j]), &(ptr[i].y[j]))<2) {
        fprintf(errfp,"WARNING: missing fields for POLYGON points, ignoring.\n");
        my_free(_ALLOC_ID_, &ptr[i].x);
        my_free(_ALLOC_ID_, &ptr[i].y);
        my_free(_ALLOC_ID_, &ptr[i].selected_point);
        read_line(fd, 0);
        return;
      }
    }
    load_ascii_string( &ptr[i].prop_ptr, fd);
    fill_ptr = get_tok_value(ptr[i].prop_ptr,"fill",0);
    if( !strcmp(fill_ptr, "full") )
      ptr[i].fill =3; /* bit 1: solid fill (not stippled) */
    else if( !strboolcmp(fill_ptr, "true") )
      ptr[i].fill =1;
    else
      ptr[i].fill =0;
    dash = get_tok_value(ptr[i].prop_ptr,"dash",0);
    if(strcmp(dash, "")) {
      int d = atoi(dash);
      ptr[i].dash = (short)(d >= 0 ? d : 0);
    } else {
      ptr[i].dash = 0;
    }
    xctx->polygons[c]++;
}

static void load_arc(FILE *fd)
{
    int n,i,c;
    xArc *ptr;
    const char *dash, *fill_ptr;

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

    fill_ptr = get_tok_value(ptr[i].prop_ptr,"fill",0);
    if( !strcmp(fill_ptr, "full") )
      ptr[i].fill =3; /* bit 1: solid fill (not stippled) */
    else if( !strboolcmp(fill_ptr, "true") )
      ptr[i].fill =1;
    else
      ptr[i].fill =0;
    dash = get_tok_value(ptr[i].prop_ptr,"dash",0);
    if(strcmp(dash, "")) {
      int d = atoi(dash);
      ptr[i].dash = (short)(d >= 0 ? d : 0);
    } else {
      ptr[i].dash = 0;
    }
    xctx->arcs[c]++;
}

static void load_box(FILE *fd)
{
    int i,n,c;
    xRect *ptr;
    const char *attr, *fill_ptr;

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
    ptr[i].extraptr=NULL;
    ptr[i].prop_ptr=NULL;
    ptr[i].sel=0;
    load_ascii_string( &ptr[i].prop_ptr, fd);
    fill_ptr = get_tok_value(ptr[i].prop_ptr,"fill",0);
    if( !strcmp(fill_ptr, "full") )
      ptr[i].fill =3;
    else if( !strboolcmp(fill_ptr, "false") )
      ptr[i].fill =0;
    else
      ptr[i].fill =1;
    attr = get_tok_value(ptr[i].prop_ptr,"dash",0);
    if(strcmp(attr, "")) {
      int d = atoi(attr);
      ptr[i].dash = (short)(d >= 0 ? d : 0);
    } else {
      ptr[i].dash = 0;
    }

    attr = get_tok_value(ptr[i].prop_ptr,"ellipse",0);
    if(strcmp(attr, "")) {
      int a;
      int b;
      if(sscanf(attr, "%d%*[ ,]%d", &a, &b) != 2) {
        a = 0;
        b = 360;
      }
      ptr[i].ellipse_a = a;
      ptr[i].ellipse_b = b;
    } else {
      ptr[i].ellipse_a = -1;
      ptr[i].ellipse_b = -1;
    }

    set_rect_flags(&xctx->rect[c][i]); /* set cached .flags bitmask from on attributes */
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

    if(!strboolcmp(get_tok_value(ptr[i].prop_ptr, "bus", 0), "true") )
      ptr[i].bus = 1;
    else
      ptr[i].bus = 0;

    dash = get_tok_value(ptr[i].prop_ptr,"dash",0);
    if(strcmp(dash, "")) {
      int d = atoi(dash);
      ptr[i].dash = (short)(d >= 0 ? d : 0);
    } else {
      ptr[i].dash = 0;
    }
    xctx->lines[c]++;
}

static void read_xschem_file(FILE *fd)
{
  int i, found, endfile;
  char name_embedded[PATH_MAX];
  char tag[1];
  int inst_cnt;
  size_t ty=0;
  char *ptr = NULL, *ptr2;

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


        if((ptr2 = strstr(xctx->version_string, "xschem")) && (ptr2 - xctx->version_string < 50)) {
          my_strdup2(_ALLOC_ID_, &ptr, subst_token(xctx->version_string, "xschem", NULL));
        }                 
        my_strdup2(_ALLOC_ID_, &ptr, subst_token(ptr, "version", NULL));
        my_strdup2(_ALLOC_ID_, &ptr, subst_token(ptr, "file_version", NULL));
        
        ptr2 = ptr;
        while(*ptr2 == ' ' || *ptr2 =='\t') ptr2++; /* strip leading spaces */
        if(*ptr2 == '\n') ptr2++; /* strip leading newline */
        my_strdup2(_ALLOC_ID_, &xctx->header_text, ptr2);
        my_free(_ALLOC_ID_,&ptr);
      }
      dbg(1, "read_xschem_file(): file_version=%s\n", xctx->file_version);
      break;
     case '#':
      read_line(fd, 1);
      break;
     case 'F': /* extension for future symbol floater labels */
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
      found=0;
      my_strdup(_ALLOC_ID_, &xctx->inst[xctx->instances-1].prop_ptr,
                subst_token(xctx->inst[xctx->instances-1].prop_ptr, "embed", "true"));
      if(xctx->inst[xctx->instances-1].name) {
        my_snprintf(name_embedded, S(name_embedded), "%s/.xschem_embedded_%d_%s",
                    tclgetvar("XSCHEM_TMP_DIR"), getpid(), get_cell_w_ext(xctx->inst[xctx->instances-1].name, 0));
        for(i=0;i<xctx->symbols; ++i)
        {
         dbg(1, "read_xschem_file(): sym[i].name=%s, name_embedded=%s\n", xctx->sym[i].name, name_embedded);
         dbg(1, "read_xschem_file(): inst[instances-1].name=%s\n", xctx->inst[xctx->instances-1].name);
         /* symbol has already been loaded: skip [..] */
         if(!xctx->x_strcmp(xctx->sym[i].name, xctx->inst[xctx->instances-1].name)) {
           found=1; break;
         }
         /* if loading file coming back from embedded symbol delete temporary file */
         /* symbol from this temp file has already been loaded in go_back() */
         if(!xctx->x_strcmp(name_embedded, xctx->sym[i].name)) {
           my_strdup2(_ALLOC_ID_, &xctx->sym[i].name, xctx->inst[xctx->instances-1].name);
           xunlink(name_embedded);
           found=1;break;
         }
        }
        read_line(fd, 0); /* skip garbage after '[' */
        if(!found) {
          load_sym_def(xctx->inst[xctx->instances-1].name, fd);
          found = 2;
        }
      }
      if(found != 2) {
        char *str;
        int n;
        while(1) { /* skip embedded [ ... ] */
          str = read_line(fd, 1);
          if(!str || !strncmp(str, "]", 1)) break;
          n = fscanf(fd, " ");
          (void)n; /* avoid compiler warnings if n unused. can not remove n since ignoring 
                    * fscanf return value yields another warning */
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
      ty = xctx->tok_size;
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
  int_hash_free(&xctx->floater_inst_table);
}

void load_ascii_string(char **ptr, FILE *fd)
{
 int c, escape=0;
 int i=0, begin=0;
 char *str=NULL;
 int strlength=0;

 for(;;)
 {
  if(i+5>strlength) my_realloc(_ALLOC_ID_, &str,(strlength+=CADCHUNKALLOC));
  c=fgetc(fd);
  if (c=='\r') continue;
  if(c==EOF) {
    fprintf(errfp, "EOF reached, malformed {...} string input, missing close brace\n");
    my_free(_ALLOC_ID_, ptr);
    my_free(_ALLOC_ID_, &str);
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
    str[i]=(char)c;
    escape = 0;
    ++i;
  } else if(c=='{') begin=1;
 }
 dbg(2, "load_ascii_string(): string read=%s\n",str? str:"<NULL>");
 my_strdup(_ALLOC_ID_, ptr, str);
 dbg(2, "load_ascii_string(): loaded %s\n",*ptr? *ptr:"<NULL>");
 my_free(_ALLOC_ID_, &str);
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

static void make_schematic(const char *schname)
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

static int order_changed;
static int pin_compare(const void *a, const void *b)
{
  int pinnumber_a, pinnumber_b;
  const char *tmp;
  int result;
  xRect *aa = (xRect *)a;
  xRect *bb = (xRect *)b;
  
  tmp = get_tok_value(aa->prop_ptr, "sim_pinnumber", 0);
  pinnumber_a = tmp[0] ?  atoi(tmp) : -1;
  tmp = get_tok_value(bb->prop_ptr, "sim_pinnumber", 0);
  pinnumber_b = tmp[0] ?atoi(tmp) : -1;
  result =  pinnumber_a < pinnumber_b ? -1 : pinnumber_a == pinnumber_b ? 0 : 1;
  if(result >= 0) order_changed = 1;
  return result;
}

static int schpin_compare(const void *a, const void *b)
{
  int pinnumber_a, pinnumber_b;
  int result;

  pinnumber_a = ((Sch_pin_record *) a)->pinnumber;
  pinnumber_b = ((Sch_pin_record *) b)->pinnumber;
  result =  pinnumber_a < pinnumber_b ? -1 : pinnumber_a == pinnumber_b ? 0 : 1;
  if(result >= 0) order_changed = 1;
  return result;
}


static void sort_symbol_pins(xRect *pin_array, int npins, const char *name)
{
  int j, do_sort = 0;
  const char *pinnumber;
  order_changed = 0;

  if(npins > 0) do_sort = 1; /* no pins, no sort... */
  /* do not sort if some pins don't have pinnumber attribute */
  for(j = 0; j < npins; ++j) {
    pinnumber = get_tok_value(pin_array[j].prop_ptr, "sim_pinnumber", 0);
    if(!pinnumber[0]) do_sort = 0;
  }
  if(do_sort) {
    qsort(pin_array, npins, sizeof(xRect), pin_compare);
    if(order_changed) {
      dbg(1, "Symbol %s has pinnumber attributes on pins. Pins will be sorted\n", name);
    }
  }
}

/* Caller must free returned pointer (if not NULL)
 * number of i/o ports found returned into npins */
Sch_pin_record *sort_schematic_pins(int *npins)
{                  
  int i, do_sort = -1;
  const char *pinnumber;
  Sch_pin_record *pinnumber_list = NULL;
  char *type;
  int lvs_ignore = tclgetboolvar("lvs_ignore");

  *npins = 0;
  order_changed = 0;
  for(i=0;i<xctx->instances; ++i) {
    if(skip_instance(i, 1, lvs_ignore)) continue;
    type = (xctx->inst[i].ptr + xctx->sym)->type;
    if( type && IS_PIN(type)) {
      (*npins)++;
    }
  }
  pinnumber_list = my_malloc(_ALLOC_ID_, sizeof(Sch_pin_record) * *npins);
  *npins = 0;
  for(i=0;i<xctx->instances; ++i) {
    if(skip_instance(i, 1, lvs_ignore)) continue;
    type = (xctx->inst[i].ptr + xctx->sym)->type;
    if( type && IS_PIN(type)) {
      int n;
      if(do_sort == -1) do_sort = 1;
      pinnumber = get_tok_value(xctx->inst[i].prop_ptr, "sim_pinnumber", 0);
      if(!pinnumber[0]) {
        do_sort = 0;
        n = 0;
      } else {
        n = atoi(pinnumber);
      }
      pinnumber_list[*npins].pinnumber = n;
      pinnumber_list[*npins].n = i;
      (*npins)++;
    }
  }
  if(do_sort) {
    qsort(pinnumber_list, *npins, sizeof(Sch_pin_record), schpin_compare);
  }   
  return pinnumber_list;
}             

/* ALWAYS call with absolute path in schname!!! */
/* return value:
 *   0 : did not save
 *   1 : schematic saved
 */
int save_schematic(const char *schname, int fast) /* 20171020 added return value */
{
  FILE *fd;
  struct stat buf;
  xRect *rect;
  int rects;
  char msg[PATH_MAX + 100];

  if(!schname || !strcmp(schname, "")) return 0;

  dbg(1, "save_schematic(): currsch=%d schname=%s\n",xctx->currsch, schname);
  dbg(1, "save_schematic(): sch[currsch]=%s\n", xctx->sch[xctx->currsch] ? xctx->sch[xctx->currsch] : "<NULL>");

  if(!xctx->sch[xctx->currsch]) { /* no current schematic name -> assign new name */
    my_strdup2(_ALLOC_ID_, &xctx->sch[xctx->currsch], schname);
    set_modify(-1); /* set title to new filename */
  }
  else if(strcmp(schname, xctx->sch[xctx->currsch])) { /* user asks to save to a different filename */
    my_strdup2(_ALLOC_ID_, &xctx->sch[xctx->currsch], schname);
    set_modify(-1); /* set title to new filename */
  }
  else { /* user asks to save to same filename */
    if(!stat(xctx->sch[xctx->currsch], &buf)) {
      if(xctx->time_last_modify && xctx->time_last_modify != buf.st_mtime) {
        tclvareval("ask_save_optional \"Schematic file: ", xctx->sch[xctx->currsch],
            "\nHas been changed since opening.\nSave anyway?\" 0", NULL);
        if(strcmp(tclresult(), "yes") ) return 0;
      }
    }
  }
  if(!(fd=fopen(schname,"w")))
  {
    fprintf(errfp, "save_schematic(): problems opening file %s \n",schname);
    tcleval("alert_ {file opening for write failed!} {}");
    return 0;
  }
  unselect_all(1);
  rects = xctx->rects[PINLAYER];
  rect = xctx->rect[PINLAYER];
  sort_symbol_pins(rect, rects, schname);
  write_xschem_file(fd);
  fclose(fd);
  /* update time stamp */
  if(!stat(schname, &buf)) {
    xctx->time_last_modify =  buf.st_mtime;
  }
  my_strncpy(xctx->current_name, rel_sym_path(schname), S(xctx->current_name));
  my_snprintf(msg, S(msg), "get_directory {%s}", schname);
  my_strncpy(xctx->current_dirname,  tcleval(msg), S(xctx->current_dirname));
  /* why clear all these? */
  /* 
   * xctx->prep_hi_structs=0;
   * xctx->prep_net_structs=0;
   * xctx->prep_hash_inst=0;
   * xctx->prep_hash_wires=0;
   */
  if(!strstr(xctx->sch[xctx->currsch], ".xschem_embedded_")) {
    if(fast) set_modify(2); /* only clear modified flag, no title/tab/sim buttons update */
    else     set_modify(0);
  }
  tclvareval(xctx->top_path, ".menubar entryconfigure Simulate -background $simulate_bg", NULL);
  tclvareval("set tctx::", xctx->current_win_path, "_simulate $simulate_bg", NULL);
  tclvareval("catch {unset tctx::", xctx->current_win_path, "_simulate_id}", NULL);
  /* set local simulation directory if local_netlist_dir is set*/
  set_netlist_dir(2, NULL);
  return 1;
}

/* from == -1: link symbols to all instances, from 0 to instances-1
 * from >=  0: link symbols from pasted schematic / clipboard */
void link_symbols_to_instances(int from)
{
  int cond, i, merge = 1;
  char *type=NULL;
  char *name = NULL;

  if(from < 0 ) {
    from = 0;
    merge = 0;
  }
  for(i = from; i < xctx->instances; ++i) {
    dbg(2, "link_symbols_to_instances(): inst=%d\n", i);
    dbg(2, "link_symbols_to_instances(): matching inst %d name=%s \n",i, xctx->inst[i].name);
    dbg(2, "link_symbols_to_instances(): -------\n");
    my_strdup2(_ALLOC_ID_, &name, tcl_hook2(translate(i, xctx->inst[i].name)));
    xctx->inst[i].ptr = match_symbol(name);
    my_free(_ALLOC_ID_, &name);
  }
  for(i = from; i < xctx->instances; ++i) {
    type=xctx->sym[xctx->inst[i].ptr].type;
    cond= type && IS_LABEL_SH_OR_PIN(type);
    if(cond) {
      xctx->inst[i].flags |= PIN_OR_LABEL; /* label or pin */
      my_strdup2(_ALLOC_ID_, &xctx->inst[i].lab, get_tok_value(xctx->inst[i].prop_ptr,"lab",0));
    }
    else xctx->inst[i].flags &= ~PIN_OR_LABEL; /* ordinary symbol */
  }
  /* symbol_bbox() might call translate() that might call prepare_netlist_structs() that 
   * needs .lab field set above, so this must be done last */
  for(i = from; i < xctx->instances; ++i) {
    symbol_bbox(i, &xctx->inst[i].x1, &xctx->inst[i].y1, &xctx->inst[i].x2, &xctx->inst[i].y2);
    if(merge) select_element(i,SELECTED,1, 0); /* leave elements selected if a paste/copy from windows is done */
  }
}

/* ALWAYS use absolute pathname for fname!!!
 * alert = 0 --> do not show alert if file not existing */
int load_schematic(int load_symbols, const char *fname, int reset_undo, int alert)
{
  FILE *fd;
  char name[PATH_MAX];
  char msg[PATH_MAX+100];
  struct stat buf;
  int i, ret = 1; /* success */
  
  xctx->prep_hi_structs=0;
  xctx->prep_net_structs=0;
  xctx->prep_hash_inst=0;
  xctx->prep_hash_wires=0;
  if(reset_undo) {
    xctx->clear_undo();
    xctx->prev_set_modify = -1; /* will force set_modify(0) to set window title */
  }
  else  xctx->prev_set_modify = 0;           /* will prevent set_modify(0) from setting window title */
  if(fname && fname[0]) {
    int generator = 0;
    if(is_generator(fname)) generator = 1;
    my_strncpy(name, fname, S(name));
    dbg(1, "load_schematic(): fname=%s\n", fname);
    /* remote web object specified */
    if(is_from_web(fname) && xschem_web_dirname[0]) {
      /* download into ${XSCHEM_TMP_DIR}/xschem_web */
      tclvareval("download_url {", fname, "}", NULL);
      /* build local file name of downloaded object */
      my_snprintf(name, S(name), "%s/%s",  xschem_web_dirname, get_cell_w_ext(fname, 0));
      /* build current_dirname by stripping off last filename from url */
      my_snprintf(msg, S(msg), "get_directory {%s}", fname);
      my_strncpy(xctx->current_dirname,  tcleval(msg), S(xctx->current_dirname));
      /* local file name */
      my_strdup2(_ALLOC_ID_, &xctx->sch[xctx->currsch], name);
      /* local relative reference */
      my_strncpy(xctx->current_name, rel_sym_path(name), S(xctx->current_name));
    /* local filename specified but coming (push, pop) from web object ... */
    } else if(is_from_web(xctx->current_dirname) && xschem_web_dirname[0]) {
      /* ... but not local file from web download --> reset current_dirname */
      char sympath[PATH_MAX];
      my_snprintf(sympath, S(sympath), "%s",  xschem_web_dirname);
      /* fname does not begin with $XSCHEM_TMP_DIR/xschem_web and fname does not exist */

      if(strstr(fname, sympath) != fname /* && stat(fname, &buf)*/) { 
        my_snprintf(msg, S(msg), "get_directory {%s}", fname);
        my_strncpy(xctx->current_dirname,  tcleval(msg), S(xctx->current_dirname));
      } 
      /* local file name */
      my_strdup2(_ALLOC_ID_, &xctx->sch[xctx->currsch], fname);
      /* local relative reference */
      my_strncpy(xctx->current_name, rel_sym_path(fname), S(xctx->current_name));
    } else { /* local file specified and not coming from web url */
      /* build current_dirname by stripping off last filename from url */
      my_snprintf(msg, S(msg), "get_directory {%s}", fname);
      my_strncpy(xctx->current_dirname,  tcleval(msg), S(xctx->current_dirname));
      /* local file name */
      my_strdup2(_ALLOC_ID_, &xctx->sch[xctx->currsch], fname);
      /* local relative reference */
      my_strncpy(xctx->current_name, rel_sym_path(fname), S(xctx->current_name));
    }

    dbg(1, "load_schematic(): opening file for loading:%s, fname=%s\n", name, fname);
    dbg(1, "load_schematic(): sch[currsch]=%s\n", xctx->sch[xctx->currsch]);
    if(!name[0]) return 0; /* empty filename */
    if(reset_undo) {
      if(!stat(name, &buf)) { /* file exists */
        xctx->time_last_modify =  buf.st_mtime;
      } else {
        /* xctx->time_last_modify = time(NULL); */ /* file does not exist, set mtime to current time */
        xctx->time_last_modify = 0; /* file does not exist, set mtime to 0 (undefined)*/
      }
    } else {xctx->time_last_modify = 0;} /* undefined */
    if(generator) {
      char *cmd;
      cmd = get_generator_command(fname);
      if(cmd) {
        fd = popen(cmd, "r");
        my_free(_ALLOC_ID_, &cmd);
      } else fd = NULL;
    }
    else fd=fopen(name,fopen_read_mode);
    if( fd == NULL) {
      ret = 0;
      if(alert) {
        fprintf(errfp, "load_schematic(): unable to open file: %s, fname=%s\n", name, fname );
        if(has_x) {
          my_snprintf(msg, S(msg), "update; alert_ {Unable to open file: %s}", fname);
          tcleval(msg);
        }
      }
      clear_drawing();
      if(reset_undo) set_modify(0);
    } else {
      clear_drawing();
      dbg(1, "load_schematic(): reading file: %s\n", name);
      read_xschem_file(fd);
      if(generator) pclose(fd);
      else fclose(fd); /* 20150326 moved before load symbols */
      if(reset_undo) set_modify(0);
      dbg(2, "load_schematic(): loaded file:wire=%d inst=%d\n",xctx->wires , xctx->instances);
      if(load_symbols) link_symbols_to_instances(-1);
      if(reset_undo) {
        tclvareval("is_xschem_file {", xctx->sch[xctx->currsch], "}", NULL);
        if(!strcmp(tclresult(), "SYMBOL") || xctx->instances == 0) {
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
  } else { /* fname == NULL or empty */
    /* if(reset_undo) xctx->time_last_modify = time(NULL); */ /* no file given, set mtime to current time */
    if(reset_undo) xctx->time_last_modify = 0; /* no file given, set mtime to 0 (undefined) */
    clear_drawing();
    for(i=0;; ++i) {
      if(xctx->netlist_type == CAD_SYMBOL_ATTRS) {
        if(i == 0) my_snprintf(name, S(name), "%s.sym", "untitled");
        else my_snprintf(name, S(name), "%s-%d.sym", "untitled", i);
      } else {
        if(i == 0) my_snprintf(name, S(name), "%s.sch", "untitled");
        else my_snprintf(name, S(name), "%s-%d.sch", "untitled", i);
      }
      if(stat(name, &buf)) break;
    }
    my_strncpy(xctx->current_name, name, S(xctx->current_name));
    if(getenv("PWD")) {
      /* $env(PWD) better than pwd_dir as it does not dereference symlinks */
      my_strncpy(xctx->current_dirname, getenv("PWD"), S(xctx->current_dirname));
    } else {
      my_strncpy(xctx->current_dirname, pwd_dir, S(xctx->current_dirname));
    }
    my_mstrcat(_ALLOC_ID_, &xctx->sch[xctx->currsch],  xctx->current_dirname, "/", name, NULL);
    if(reset_undo) set_modify(0);
  }
  check_collapsing_objects();
  if(reset_undo && tclgetboolvar("autotrim_wires")) trim_wires();
  update_conn_cues(WIRELAYER, 0, 0);
  if(xctx->hilight_nets && load_symbols) {
    propagate_hilights(1, 1, XINSERT_NOREPLACE);
  }
  /* set local simulation directory if local_netlist_dir is set*/
  if(reset_undo) {
    set_netlist_dir(2, NULL);
    drc_check(-1);
  }
  return ret;
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
  for(i=0; i<MAX_UNDO; ++i) {
    my_snprintf(diff_name, S(diff_name), "%s/undo%d",xctx->undo_dirname, i);
    xunlink(diff_name);
  }
  rmdir(xctx->undo_dirname);
  my_free(_ALLOC_ID_, &xctx->undo_dirname);
  xctx->undo_initialized = 0;
}

/* create undo directory in XSCHEM_TEMP_DIR */
static void init_undo(void)
{
  if(xctx->no_undo == 0 && !xctx->undo_initialized) {
    /* create undo directory */
    if( !my_strdup(_ALLOC_ID_, &xctx->undo_dirname, create_tmpdir("xschem_undo_") )) {
      dbg(0, "init_undo(): problems creating tmp undo dir, Undo will be disabled\n");
      dbg(0, "init_undo(): Check permissions in %s\n", tclgetvar("XSCHEM_TMP_DIR"));
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

    dbg(1, "push_undo(): cur_undo_ptr=%d tail_undo_ptr=%d head_undo_ptr=%d\n",
       xctx->cur_undo_ptr, xctx->tail_undo_ptr, xctx->head_undo_ptr);
    init_undo();
    if(xctx->no_undo)return;
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
    fflush(NULL); /* flush all stdio streams before process forking */
    if((pid = fork()) ==0) {                                    /* child process */
      close(pd[1]);                                     /* close write side of pipe */
      if(!(diff_fd=freopen(diff_name,"w", stdout)))     /* redirect stdout to file diff_name */
      {
        dbg(1, "push_undo(): problems opening file %s \n",diff_name);
        _exit(1);
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
      _exit(1);
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
 * 4: undo, do not push state for redo
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
  dbg(1, "pop_undo: redo=%d, set_modify_status=%d\n", redo, set_modify_status);
  if(redo == 1) {
    if(xctx->cur_undo_ptr < xctx->head_undo_ptr) {
      dbg(1, "pop_undo(): redo; cur_undo_ptr=%d tail_undo_ptr=%d head_undo_ptr=%d\n",
         xctx->cur_undo_ptr, xctx->tail_undo_ptr, xctx->head_undo_ptr);
      xctx->cur_undo_ptr++;
    } else {
      return;
    }
  } else if(redo == 0 || redo == 4) {  /* undo */
    if(xctx->cur_undo_ptr == xctx->tail_undo_ptr) return;
    dbg(1, "pop_undo(): undo; cur_undo_ptr=%d tail_undo_ptr=%d head_undo_ptr=%d\n",
       xctx->cur_undo_ptr, xctx->tail_undo_ptr, xctx->head_undo_ptr);
    if(redo == 0 && xctx->head_undo_ptr == xctx->cur_undo_ptr) {
      dbg(1, "pop_undo(): doing push_undo, head=%d  cur=%d\n", xctx->head_undo_ptr, xctx->cur_undo_ptr);
      xctx->push_undo();
      xctx->head_undo_ptr--;
      xctx->cur_undo_ptr--;
    }
    /* was incremented by a previous push_undo() in netlisting code, so restore */
    if(redo == 4 && xctx->head_undo_ptr == xctx->cur_undo_ptr) xctx->head_undo_ptr--;
    if(xctx->cur_undo_ptr<=0) return; /* check undo tail */
    xctx->cur_undo_ptr--;
  } else { /* redo == 2, get data without changing undo stack */
    if(xctx->cur_undo_ptr<=0) return; /* check undo tail */
    xctx->cur_undo_ptr--; /* will be restored after building file name */
  }
  clear_drawing();
  unselect_all(1);

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
  fflush(NULL); /* flush all stdio streams before process forking */
  if((pid = fork())==0) {                                     /* child process */
    close(pd[0]);                                    /* close read side of pipe */
    if(!(diff_fd=freopen(diff_name,"r", stdin)))     /* redirect stdin from file name */
    {
      dbg(1, "pop_undo(): problems opening file %s \n",diff_name);
      _exit(1);
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
    _exit(1);
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
  update_conn_cues(WIRELAYER, 0, 0);
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
                         Int_hashtable *pintable, FILE *embed_fd, int *sym_n_pins)
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
  for(i=0;i<xctx->symbols; ++i) {
    if(xctx->x_strcmp(symname, xctx->sym[i].name) == 0) {
      my_strdup2(_ALLOC_ID_, type, xctx->sym[i].type);
      found = 1;
      break;
    }
  }
  /* hash pins to get LCC schematic have same order as corresponding symbol */
  if(found && pintable) {
    *sym_n_pins = xctx->sym[i].rects[PINLAYER];
    for (c = 0; c < xctx->sym[i].rects[PINLAYER]; ++c) {
      int_hash_lookup(pintable, get_tok_value(xctx->sym[i].rect[PINLAYER][c].prop_ptr, "name", 0), c, XINSERT);
    }
  }
  if( !found ) {
    dbg(1, "get_sym_type(): open file %s, pintable %s\n",name, pintable ? "set" : "<NULL>");
    /* ... if not found open file and look for 'type' into the global attributes. */

    if(embed_fd) fd = embed_fd;
    else fd=fopen(name,fopen_read_mode);

    if(fd==NULL) {
      dbg(1, "get_sym_type(): Symbol not found: %s\n",name);
      my_strdup2(_ALLOC_ID_, type, "");
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
              my_strdup2(_ALLOC_ID_, type, get_tok_value(globalprop, "type", 0));
            }
            break;
          case 'K':
            load_ascii_string(&globalprop,fd);
            my_strdup2(_ALLOC_ID_, type, get_tok_value(globalprop, "type", 0));
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
             /* dbg(1, "get_sym_type() : hashing %s\n", get_tok_value(rect.prop_ptr, "name", 0));*/
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
      my_free(_ALLOC_ID_, &globalprop);
      my_free(_ALLOC_ID_, &rect.prop_ptr);
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
  Int_hashtable pintable = {NULL, 0};

  if ((ptr = strrchr(name, '.')) && !strcmp(ptr, ".sch")) {
    my_strncpy(symname, add_ext(name, ".sym"), S(symname));
    int_hash_init(&pintable, HASHSIZE);
    /* hash all symbol pins with their position into pintable hash*/
    get_sym_type(symname, &symtype, &pintable, NULL, &sym_n_pins);
    if(symtype[0]) { /* found a .sym for current .sch LCC instance */
      xRect *rect = NULL;
      if (sym_n_pins!=xctx->sym[pos].rects[PINLAYER]) {
        dbg(0, " align_sch_pins_with_sym(): warning: number of pins mismatch between %s and %s\n",
          name, symname);
        fail = 1;
      }
      rect = (xRect *) my_malloc(_ALLOC_ID_, sizeof(xRect) * sym_n_pins);
      dbg(1, "align_sch_pins_with_sym(): symbol: %s\n", symname);
      for(i=0; i < xctx->sym[pos].rects[PINLAYER]; ++i) {
        Int_hashentry *entry;
        pinname = get_tok_value(xctx->sym[pos].rect[PINLAYER][i].prop_ptr, "name", 0);
        entry = int_hash_lookup(&pintable, pinname, 0 , XLOOKUP);
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
        for(i=0; i < xctx->sym[pos].rects[PINLAYER]; ++i) {
          xctx->sym[pos].rect[PINLAYER][i] = rect[i];
        }
      }
      my_free(_ALLOC_ID_, &rect);
    }
    int_hash_free(&pintable);
    my_free(_ALLOC_ID_, &symtype);
  }
}

/* replace i/o/iopin instances of LCC schematics with symbol pins (boxes on PINLAYER layer) */
static void add_pinlayer_boxes(int *lastr, xRect **bb,
                 const char *symtype, char *prop_ptr, double i_x0, double i_y0)
{
  int i;
  size_t save;
  const char *label;
  char *pin_label = NULL;

  i = lastr[PINLAYER];
  my_realloc(_ALLOC_ID_, &bb[PINLAYER], (i + 1) * sizeof(xRect));
  bb[PINLAYER][i].x1 = i_x0 - 2.5; bb[PINLAYER][i].x2 = i_x0 + 2.5;
  bb[PINLAYER][i].y1 = i_y0 - 2.5; bb[PINLAYER][i].y2 = i_y0 + 2.5;
  RECTORDER(bb[PINLAYER][i].x1, bb[PINLAYER][i].y1, bb[PINLAYER][i].x2, bb[PINLAYER][i].y2);
  bb[PINLAYER][i].prop_ptr = NULL;
  label = get_tok_value(prop_ptr, "lab", 0);
  save = strlen(label)+30;
  pin_label = my_malloc(_ALLOC_ID_, save);
  pin_label[0] = '\0';
  if (!strcmp(symtype, "ipin")) {
    my_snprintf(pin_label, save, "name=%s dir=in ", label);
  } else if (!strcmp(symtype, "opin")) {
    my_snprintf(pin_label, save, "name=%s dir=out ", label);
  } else if (!strcmp(symtype, "iopin")) {
    my_snprintf(pin_label, save, "name=%s dir=inout ", label);
  }
  my_strdup(_ALLOC_ID_, &bb[PINLAYER][i].prop_ptr, pin_label);
  bb[PINLAYER][i].flags = 0;
  bb[PINLAYER][i].extraptr = 0;
  bb[PINLAYER][i].dash = 0;
  bb[PINLAYER][i].ellipse_a =  bb[PINLAYER][i].ellipse_b = -1;
  bb[PINLAYER][i].sel = 0;
  /* add to symbol pins remaining attributes from schematic pins, except name= and lab= */
  my_strdup(_ALLOC_ID_, &pin_label, get_sym_template(prop_ptr, "lab"));   /* remove name=...  and lab=... */
  my_strcat(_ALLOC_ID_, &bb[PINLAYER][i].prop_ptr, pin_label);
  my_free(_ALLOC_ID_, &pin_label);
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

static void calc_symbol_bbox(int pos)
{
  int c, i, count = 0;
  xRect boundbox, tmp;

  boundbox.x1 = boundbox.x2 = boundbox.y1 = boundbox.y2 = 0;
  for(c=0;c<cadlayers; ++c)
  {
   for(i=0;i<xctx->sym[pos].lines[c]; ++i)
   {
    ++count;
    tmp.x1=xctx->sym[pos].line[c][i].x1;tmp.y1=xctx->sym[pos].line[c][i].y1;
    tmp.x2=xctx->sym[pos].line[c][i].x2;tmp.y2=xctx->sym[pos].line[c][i].y2;
    updatebbox(count,&boundbox,&tmp);
    dbg(2, "calc_symbol_bbox(): line[%d][%d]: %g %g %g %g\n",
			c, i, tmp.x1,tmp.y1,tmp.x2,tmp.y2);
   }
   for(i=0;i<xctx->sym[pos].arcs[c]; ++i)
   {
    ++count;
    arc_bbox(xctx->sym[pos].arc[c][i].x, xctx->sym[pos].arc[c][i].y, xctx->sym[pos].arc[c][i].r,
             xctx->sym[pos].arc[c][i].a, xctx->sym[pos].arc[c][i].b,
             &tmp.x1, &tmp.y1, &tmp.x2, &tmp.y2);
    /* printf("arc bbox: %g %g %g %g\n", tmp.x1, tmp.y1, tmp.x2, tmp.y2); */
    updatebbox(count,&boundbox,&tmp);
   }
   for(i=0;i<xctx->sym[pos].rects[c]; ++i)
   {
    ++count;
    tmp.x1=xctx->sym[pos].rect[c][i].x1;tmp.y1=xctx->sym[pos].rect[c][i].y1;
    tmp.x2=xctx->sym[pos].rect[c][i].x2;tmp.y2=xctx->sym[pos].rect[c][i].y2;
    updatebbox(count,&boundbox,&tmp);
   }
   for(i=0;i<xctx->sym[pos].polygons[c]; ++i)
   {
     double x1=0., y1=0., x2=0., y2=0.;
     int k;
     ++count;
     for(k=0; k<xctx->sym[pos].poly[c][i].points; ++k) {
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
*   for(i=0;i<lastt; ++i)
*   {
*    int tmp;
*    count++;
*    rot=tt[i].rot;flip=tt[i].flip;
*    estr = my_expand(get_text_floater(i), tclgetintvar("tabstop"));
*    text_bbox(estr, tt[i].xscale, tt[i].yscale, rot, flip,
*    tt[i].x0, tt[i].y0, &rx1,&ry1,&rx2,&ry2, &dtmp);
*    my_free(_ALLOC_ID_, &estr);
*    tmp.x1=rx1;tmp.y1=ry1;tmp.x2=rx2;tmp.y2=ry2;
*    updatebbox(count,&boundbox,&tmp);
*  }
*/
  xctx->sym[pos].minx = boundbox.x1;
  xctx->sym[pos].maxx = boundbox.x2;
  xctx->sym[pos].miny = boundbox.y1;
  xctx->sym[pos].maxy = boundbox.y2;
}

/* return 1 if http or https url
 * return 0 otherwise
 */
int is_from_web(const char *f)
{
  int res = 0;
  if(strstr(f, "http://") == f || strstr(f, "https://") == f) res = 1;
  dbg(1, "is_from_web(%s) = %d\n", f, res);
  return res;
}


/* load_sym_def(): load a symbol definition looking up 'name' in the search paths.
 * if 'embed_fd' FILE pointer is given read from there instead of searching 'name'
 * Global (or static global) variables used:
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
#ifdef __unix__
  long filepos;
#else
  __int3264 filepos;
#endif
  char sympath[PATH_MAX];
  int i,c, k, poly_points;
  char ch = 0, *aux_ptr=NULL;
  char *prop_ptr=NULL, *symtype=NULL;
  double inst_x0, inst_y0;
  short inst_rot, inst_flip;
  char *symname = NULL;
  char tag[1];
  int *lastl = my_malloc(_ALLOC_ID_, cadlayers * sizeof(lastl));
  int *lastr = my_malloc(_ALLOC_ID_, cadlayers * sizeof(int));
  int *lastp = my_malloc(_ALLOC_ID_, cadlayers * sizeof(int));
  int *lasta = my_malloc(_ALLOC_ID_, cadlayers * sizeof(int));
  int lastt;
  xLine tmpline, **ll = my_malloc(_ALLOC_ID_, cadlayers * sizeof(xLine *));
  xRect tmprect, **bb = my_malloc(_ALLOC_ID_, cadlayers * sizeof(xRect *));
  xPoly tmppoly, **pp = my_malloc(_ALLOC_ID_, cadlayers * sizeof(xPoly *));
  xArc tmparc, **aa = my_malloc(_ALLOC_ID_, cadlayers * sizeof(xArc *));
  xText tmptext, *tt;
  int endfile;
  char *skip_line;
  const char *attr, *fill_ptr;
  xSymbol * symbol;
  int symbols, sym_n_pins=0, generator;
  char *transl_name = NULL;
  char *translated_cmd = NULL;
  int is_floater = 0;

  if(!name) {
    dbg(0, "l_s_d(): Warning: name parameter set to NULL, returning with no action\n");
    return 0;
  }
  sympath[0] = '\0'; /* set to empty */
  check_symbol_storage();
  symbol = xctx->sym;
  symbols = xctx->symbols;
  dbg(1, "l_s_d(): recursion_counter=%d, name=%s\n", recursion_counter, name);
  recursion_counter++;
  lcc=NULL;
  my_realloc(_ALLOC_ID_, &lcc, (level + 1) * sizeof(Lcc));
  max_level = level + 1;
  my_strdup2(_ALLOC_ID_, &transl_name, tcl_hook2(name));
  dbg(1, "l_s_d(): transl_name=%s\n", transl_name);
  generator = is_generator(transl_name);
  if(generator) {
    translated_cmd = get_generator_command(transl_name);
    dbg(1, "l_s_d(): generator: transl_name=|%s|\n", transl_name);
    dbg(1, "l_s_d(): generator: translated_cmd=|%s|\n", translated_cmd);
    if(translated_cmd) {
      lcc[level].fd = popen(translated_cmd, "r"); /* execute ss="/path/to/xxx par1 par2 ..." and pipe in the stdout */
    } else {
      lcc[level].fd = NULL;
    }
    my_free(_ALLOC_ID_, &translated_cmd);
  } else if(!embed_fd) { /* regular symbol: open file */
    if(!strcmp(xctx->file_version,"1.0")) {
      my_strncpy(sympath, abs_sym_path(transl_name, ".sym"), S(sympath));
    } else {
      my_strncpy(sympath, abs_sym_path(transl_name, ""), S(sympath));
    }
    if((lcc[level].fd=fopen(sympath,fopen_read_mode))==NULL) {
      /* not found: try web URL */
      if(is_from_web(xctx->current_dirname)) {
        my_snprintf(sympath, S(sympath), "%s/%s", xschem_web_dirname, get_cell_w_ext(transl_name, 0));
        if((lcc[level].fd=fopen(sympath,fopen_read_mode))==NULL) {
          /* not already cached in .../xschem_web_xxxxx/ so download */
          tclvareval("try_download_url {", xctx->current_dirname, "} {", transl_name, "}", NULL);
        }
        lcc[level].fd=fopen(sympath,fopen_read_mode);
      }
    }
    dbg(1, "l_s_d(): fopen1(%s), level=%d, fd=%p\n",sympath, level, lcc[level].fd);
  } else { /* embedded symbol (defined after instantiation within [...] ) */
    dbg(1, "l_s_d(): getting embed_fd, level=%d\n", level);
    lcc[level].fd = embed_fd;
  }
  if(lcc[level].fd==NULL) {
    /* issue warning only on top level symbol loading */
    if(recursion_counter == 1) dbg(0, "l_s_d(): Symbol not found: %s\n", transl_name);
    my_snprintf(sympath, S(sympath), "%s/%s", tclgetvar("XSCHEM_SHAREDIR"), "systemlib/missing.sym");
    if((lcc[level].fd=fopen(sympath, fopen_read_mode))==NULL)
    {
     fprintf(errfp, "l_s_d(): systemlib/missing.sym missing, I give up\n");
     tcleval("exit");
    }
  }
  endfile=0;
  /* initialize data for loading a new symbol */
  for(c=0;c<cadlayers; ++c)
  {
   lasta[c] = lastl[c] = lastr[c] = lastp[c] = 0;
   ll[c] = NULL; bb[c] = NULL; pp[c] = NULL; aa[c] = NULL;
  }
  lastt=0;
  tt=NULL;
  symbol[symbols].prop_ptr = NULL;
  symbol[symbols].type = NULL;
  symbol[symbols].templ = NULL;
  symbol[symbols].parent_prop_ptr = NULL;
  symbol[symbols].base_name=NULL;
  symbol[symbols].name=NULL;

  symbol[symbols].line=my_calloc(_ALLOC_ID_, cadlayers, sizeof(xLine *));
  symbol[symbols].poly=my_calloc(_ALLOC_ID_, cadlayers, sizeof(xPoly *));
  symbol[symbols].arc=my_calloc(_ALLOC_ID_, cadlayers, sizeof(xArc *));
  symbol[symbols].rect=my_calloc(_ALLOC_ID_, cadlayers, sizeof(xRect *));
  symbol[symbols].lines=my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  symbol[symbols].rects=my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  symbol[symbols].arcs=my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));
  symbol[symbols].polygons=my_calloc(_ALLOC_ID_, cadlayers, sizeof(int));

  my_strdup2(_ALLOC_ID_, &symbol[symbols].name,transl_name);
  /* read symbol from file */
  while(1)
  {
   if(endfile && embed_fd && level == 0) break; /* ']' line encountered --> exit */
   if(fscanf(lcc[level].fd," %c",tag)==EOF) {
     if (level) {
         dbg(1, "l_s_d(): fclose1, level=%d, fd=%p\n", level, lcc[level].fd);
         if(generator) pclose(lcc[level].fd);
         else fclose(lcc[level].fd);
         my_free(_ALLOC_ID_, &lcc[level].prop_ptr);
         my_free(_ALLOC_ID_, &lcc[level].symname);
         --level;
         continue;
     } else break;
   }
   if(endfile) { /* endfile due to max hierarchy: throw away rest of file and do the above '--level' cleanups */
     read_record(tag[0], lcc[level].fd, 0);
     continue;
   }
   incremented_level = 0;
   switch(tag[0]) /* first character of line defines type of object */
   {
    case 'v':
     load_ascii_string(&aux_ptr, lcc[level].fd);
     break;
    case '#':
     read_line(lcc[level].fd, 1);
     break;
    case 'F': /* extension for future symbol floater labels */
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
       dbg(1, "load_sym_def: K prop=\n%s\n", symbol[symbols].prop_ptr);
       if(!symbol[symbols].prop_ptr) break;
       set_sym_flags(& symbol[symbols]);
     }
     else {
       load_ascii_string(&aux_ptr, lcc[level].fd);
     }
     break;
    case 'G': /* .sym files or pre-1.2 symbol attributes for schematics placed as symbols */
     if (level==0 && !symbol[symbols].prop_ptr) {
       load_ascii_string(&symbol[symbols].prop_ptr, lcc[level].fd);
       if(!symbol[symbols].prop_ptr) break;
       set_sym_flags(& symbol[symbols]);
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

     if(fscanf(lcc[level].fd, "%lf %lf %lf %lf ",&tmpline.x1, &tmpline.y1,
        &tmpline.x2, &tmpline.y2) < 4 ) {
       fprintf(errfp,"l_s_d(): WARNING:  missing fields for LINE object, ignoring\n");
       read_line(lcc[level].fd, 0);
       continue;
     }
     tmpline.prop_ptr = NULL;
     load_ascii_string(&tmpline.prop_ptr, lcc[level].fd);

     if( !strboolcmp(get_tok_value(tmpline.prop_ptr, "symbol_ignore", 0), "true")) {
       my_free(_ALLOC_ID_, &tmpline.prop_ptr);
       continue;
     }

     i=lastl[c];
     my_realloc(_ALLOC_ID_, &ll[c],(i+1)*sizeof(xLine));

     ll[c][i].x1 = tmpline.x1;
     ll[c][i].y1 = tmpline.y1;
     ll[c][i].x2 = tmpline.x2;
     ll[c][i].y2 = tmpline.y2;
     ll[c][i].prop_ptr = tmpline.prop_ptr;

     if (level>0) {
       rot = lcc[level].rot; flip = lcc[level].flip;
       ROTATION(rot, flip, 0.0, 0.0, ll[c][i].x1, ll[c][i].y1, rx1, ry1);
       ROTATION(rot, flip, 0.0, 0.0, ll[c][i].x2, ll[c][i].y2, rx2, ry2);
       ll[c][i].x1 = lcc[level].x0 + rx1;  ll[c][i].y1 = lcc[level].y0 + ry1;
       ll[c][i].x2 = lcc[level].x0 + rx2;  ll[c][i].y2 = lcc[level].y0 + ry2;
     }
     ORDER(ll[c][i].x1, ll[c][i].y1, ll[c][i].x2, ll[c][i].y2);
     dbg(2, "l_s_d(): loaded line: ptr=%lx\n", (unsigned long)ll[c]);
     if(!strboolcmp(get_tok_value(ll[c][i].prop_ptr,"bus", 0), "true") )
       ll[c][i].bus = 1;
     else
       ll[c][i].bus = 0;
     attr = get_tok_value(ll[c][i].prop_ptr,"dash", 0);
     if( strcmp(attr, "") ) {
       int d = atoi(attr);
       ll[c][i].dash = (short)(d >= 0 ? d : 0);
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

     tmppoly.x = my_calloc(_ALLOC_ID_, poly_points, sizeof(double));
     tmppoly.y = my_calloc(_ALLOC_ID_, poly_points, sizeof(double));
     tmppoly.selected_point = my_calloc(_ALLOC_ID_, poly_points, sizeof(unsigned short));
     tmppoly.points = poly_points;
     for(k=0;k<poly_points; ++k) {
       if(fscanf(lcc[level].fd, "%lf %lf ",&(tmppoly.x[k]), &(tmppoly.y[k]) ) < 2 ) {
         fprintf(errfp,"l_s_d(): WARNING: missing fields for POLYGON object\n");
       }
       if (level>0) {
         rot = lcc[level].rot; flip = lcc[level].flip;
         ROTATION(rot, flip, 0.0, 0.0, tmppoly.x[k], tmppoly.y[k], rx1, ry1);
         tmppoly.x[k] = lcc[level].x0 + rx1;  tmppoly.y[k] = lcc[level].y0 + ry1;
       }
     }

     tmppoly.prop_ptr=NULL;
     load_ascii_string( &tmppoly.prop_ptr, lcc[level].fd);

     if( !strboolcmp(get_tok_value(tmppoly.prop_ptr, "symbol_ignore", 0), "true")) {
       my_free(_ALLOC_ID_, &tmppoly.prop_ptr);
       my_free(_ALLOC_ID_, &tmppoly.x);
       my_free(_ALLOC_ID_, &tmppoly.y);
       my_free(_ALLOC_ID_, &tmppoly.selected_point);
       continue; 
     }

     i=lastp[c];
     my_realloc(_ALLOC_ID_, &pp[c],(i+1)*sizeof(xPoly));

     pp[c][i].x = tmppoly.x;
     pp[c][i].y = tmppoly.y;
     pp[c][i].selected_point = tmppoly.selected_point;
     pp[c][i].prop_ptr = tmppoly.prop_ptr;
     pp[c][i].points = poly_points;

     fill_ptr = get_tok_value(pp[c][i].prop_ptr,"fill",0);
     if( !strcmp(fill_ptr, "full") )
       pp[c][i].fill =3; /* bit 1: solid fill (not stippled) */
     else if( !strboolcmp(fill_ptr, "true") )
       pp[c][i].fill =1;
     else
       pp[c][i].fill =0;

     attr = get_tok_value(pp[c][i].prop_ptr,"dash", 0);
     if( strcmp(attr, "") ) {
       int d = atoi(attr);
       pp[c][i].dash = (short)(d >= 0 ? d : 0);
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

     if( fscanf(lcc[level].fd, "%lf %lf %lf %lf %lf ",&tmparc.x, &tmparc.y,
        &tmparc.r, &tmparc.a, &tmparc.b) < 5 ) {
       fprintf(errfp,"l_s_d(): WARNING: missing fields for ARC object, ignoring\n");
       read_line(lcc[level].fd, 0);
       continue;
     }
     tmparc.prop_ptr = NULL;
     load_ascii_string( &tmparc.prop_ptr, lcc[level].fd);

     if( !strboolcmp(get_tok_value(tmparc.prop_ptr, "symbol_ignore", 0), "true")) {
       my_free(_ALLOC_ID_, &tmparc.prop_ptr);
       continue;
     }

     i=lasta[c];
     my_realloc(_ALLOC_ID_, &aa[c],(i+1)*sizeof(xArc));

     aa[c][i].x = tmparc.x;
     aa[c][i].y = tmparc.y;
     aa[c][i].r = tmparc.r;
     aa[c][i].a = tmparc.a;
     aa[c][i].b = tmparc.b;
     aa[c][i].prop_ptr = tmparc.prop_ptr;

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
     fill_ptr = get_tok_value(aa[c][i].prop_ptr,"fill",0);
     if( !strcmp(fill_ptr, "full") )
       aa[c][i].fill =3; /* bit 1: solid fill (not stiaaled) */
     else if( !strboolcmp(fill_ptr, "true") )
       aa[c][i].fill =1;
     else
       aa[c][i].fill =0;
     attr = get_tok_value(aa[c][i].prop_ptr,"dash", 0);
     if( strcmp(attr, "") ) {
       int d = atoi(attr);
       aa[c][i].dash = (short)(d >= 0 ? d : 0);
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

     if(fscanf(lcc[level].fd, "%lf %lf %lf %lf ",&tmprect.x1, &tmprect.y1,
        &tmprect.x2, &tmprect.y2) < 4 ) {
       fprintf(errfp,"l_s_d(): WARNING:  missing fields for LINE object, ignoring\n");
       read_line(lcc[level].fd, 0);
       continue;
     }
     tmprect.prop_ptr = NULL;
     load_ascii_string(&tmprect.prop_ptr, lcc[level].fd);

     if( !strboolcmp(get_tok_value(tmprect.prop_ptr, "symbol_ignore", 0), "true")) {
       my_free(_ALLOC_ID_, &tmprect.prop_ptr);
       continue;
     }

     if (level>0 && c == PINLAYER) c = 7; /* Don't care about pins inside SYM: set on different layer */
     i=lastr[c];
     my_realloc(_ALLOC_ID_, &bb[c],(i+1)*sizeof(xRect));

     bb[c][i].x1 = tmprect.x1;
     bb[c][i].y1 = tmprect.y1;
     bb[c][i].x2 = tmprect.x2;
     bb[c][i].y2 = tmprect.y2;
     bb[c][i].prop_ptr = tmprect.prop_ptr;

     if (level>0) {
       rot = lcc[level].rot; flip = lcc[level].flip;
       ROTATION(rot, flip, 0.0, 0.0, bb[c][i].x1, bb[c][i].y1, rx1, ry1);
       ROTATION(rot, flip, 0.0, 0.0, bb[c][i].x2, bb[c][i].y2, rx2, ry2);
       bb[c][i].x1 = lcc[level].x0 + rx1;  bb[c][i].y1 = lcc[level].y0 + ry1;
       bb[c][i].x2 = lcc[level].x0 + rx2;  bb[c][i].y2 = lcc[level].y0 + ry2;
     }
     RECTORDER(bb[c][i].x1, bb[c][i].y1, bb[c][i].x2, bb[c][i].y2);
     /* don't load graphs of LCC schematic instances */
     if(strstr(get_tok_value(bb[c][i].prop_ptr, "flags", 0), "graph")) {
       my_free(_ALLOC_ID_, &bb[c][i].prop_ptr);
       continue;
     }
     dbg(2, "l_s_d(): loaded rect: ptr=%lx\n", (unsigned long)bb[c]);
     fill_ptr = get_tok_value(bb[c][i].prop_ptr,"fill",0);
     if( !strcmp(fill_ptr, "full") )
       bb[c][i].fill =3;
     else if( !strboolcmp(fill_ptr, "false") )
       bb[c][i].fill =0;
     else
       bb[c][i].fill =1;
     attr = get_tok_value(bb[c][i].prop_ptr,"dash", 0);
     if( strcmp(attr, "") ) {
       int d = atoi(attr);
       bb[c][i].dash = (short)(d >= 0 ? d : 0);
     } else bb[c][i].dash = 0;

     attr = get_tok_value(bb[c][i].prop_ptr,"ellipse", 0);
     if( strcmp(attr, "") ) {
       int a;
       int b;
       if(sscanf(attr, "%d%*[ ,]%d", &a, &b) != 2) {
         a = 0;
         b = 360;
       }
       bb[c][i].ellipse_a = a;
       bb[c][i].ellipse_b = b;
     } else {
       bb[c][i].ellipse_a = -1;
       bb[c][i].ellipse_b = -1;
     }

     bb[c][i].sel = 0;
     bb[c][i].extraptr = NULL;
     set_rect_flags(&bb[c][i]);
     lastr[c]++;
     break;
    case 'T':
     tmptext.floater_instname = tmptext.prop_ptr = tmptext.txt_ptr = tmptext.font = tmptext.floater_ptr = NULL;
     load_ascii_string(&tmptext.txt_ptr, lcc[level].fd);
     if(fscanf(lcc[level].fd, "%lf %lf %hd %hd %lf %lf ",&tmptext.x0, &tmptext.y0, &tmptext.rot,
        &tmptext.flip, &tmptext.xscale, &tmptext.yscale) < 6 ) {
       fprintf(errfp,"l_s_d(): WARNING:  missing fields for Text object, ignoring\n");
       read_line(lcc[level].fd, 0);
       continue;
     }
     load_ascii_string(&tmptext.prop_ptr, lcc[level].fd);

     is_floater = 0;
     get_tok_value(tmptext.prop_ptr, "name", 2);
     if(xctx->tok_size) is_floater = 1; /* get rid of floater texts in LCC symbols */
     else {
       get_tok_value(tmptext.prop_ptr, "floater", 2);
       if(xctx->tok_size) is_floater = 1; /* get rid of floater texts in LCC symbols */
     }
     if( !strboolcmp(get_tok_value(tmptext.prop_ptr, "symbol_ignore", 0), "true") || is_floater) {
       my_free(_ALLOC_ID_, &tmptext.prop_ptr);
       my_free(_ALLOC_ID_, &tmptext.txt_ptr);
       continue;
     }
     i=lastt;
     my_realloc(_ALLOC_ID_, &tt,(i+1)*sizeof(xText));
     tt[i].font=NULL;
     tt[i].txt_ptr = tmptext.txt_ptr;
     tt[i].x0 = tmptext.x0;
     tt[i].y0 = tmptext.y0;
     tt[i].rot = tmptext.rot;
     tt[i].flip = tmptext.flip;
     tt[i].xscale = tmptext.xscale;
     tt[i].yscale = tmptext.yscale;
     tt[i].prop_ptr = tmptext.prop_ptr;
     tt[i].floater_ptr = tmptext.floater_ptr;
     tt[i].floater_instname = tmptext.floater_instname;
     dbg(1, "l_s_d(): txt1: level=%d tt[i].txt_ptr=%s, i=%d\n", level, tt[i].txt_ptr, i);
     if (level>0) {
       const char* tmp = translate2(lcc, level, tt[i].txt_ptr);
       dbg(1, "l_s_d(): txt2: tt[i].txt_ptr=%s, i=%d\n",  tt[i].txt_ptr, i);
       rot = lcc[level].rot; flip = lcc[level].flip;
       my_strdup2(_ALLOC_ID_, &tt[i].txt_ptr, tmp);
       dbg(1, "l_s_d(): txt3: tt[i].txt_ptr=%s, i=%d\n",  tt[i].txt_ptr, i);
       /* allow annotation inside LCC instances. */
       if(!strcmp(tt[i].txt_ptr, "@spice_get_voltage")) {
         /* prop_ptr is the attribute string of last loaded LCC component */
         const char *lab;
         size_t new_size = 0;
         char *path = NULL;
         if(level > 1) { /* add parent LCC instance names (X1, Xinv etc) */
           int i;
           for(i = 1; i <level; ++i) {
             const char *instname = get_tok_value(lcc[i].prop_ptr, "name", 0);
             my_strcat(_ALLOC_ID_, &path, instname);
             my_strcat(_ALLOC_ID_, &path, ".");
           }
         }
         if(path) new_size += strlen(path);
         lab = get_tok_value(prop_ptr, "lab", 0);
         new_size += strlen(lab) + 21; /* @spice_get_voltage(<lab>) */
         my_realloc(_ALLOC_ID_, &tt[i].txt_ptr, new_size);
         my_snprintf(tt[i].txt_ptr, new_size, "@spice_get_voltage(%s%s)", path ? path : "", lab);
         my_free(_ALLOC_ID_, &path);
         dbg(1, " --> tt[i].txt_ptr=%s\n", tt[i].txt_ptr);
       }
       if(!strcmp(tt[i].txt_ptr, "@spice_get_current")) {
         /* prop_ptr is the attribute string of last loaded LCC component */
         const char *dev;
         size_t new_size = 0;
         char *path = NULL;
         if(level > 1) { /* add parent LCC instance names (X1, Xinv etc) */
           int i;
           for(i = 1; i <level; ++i) {
             const char *instname = get_tok_value(lcc[i].prop_ptr, "name", 0);
             my_strcat(_ALLOC_ID_, &path, instname);
             my_strcat(_ALLOC_ID_, &path, "."); 
           }
         } 
         if(path) new_size += strlen(path);
         dev = get_tok_value(prop_ptr, "name", 0);
         new_size += strlen(dev) + 21; /* @spice_get_current(<dev>) */
         my_realloc(_ALLOC_ID_, &tt[i].txt_ptr, new_size);
         my_snprintf(tt[i].txt_ptr, new_size, "@spice_get_current(%s%s)", path ? path : "", dev);
         my_free(_ALLOC_ID_, &path);
         dbg(1, " --> tt[i].txt_ptr=%s\n", tt[i].txt_ptr);
       } 
       ROTATION(rot, flip, 0.0, 0.0, tt[i].x0, tt[i].y0, rx1, ry1);
       tt[i].x0 = lcc[level].x0 + rx1;  tt[i].y0 = lcc[level].y0 + ry1;
       tt[i].rot = (tt[i].rot + ((lcc[level].flip && (tt[i].rot & 1)) ?
                   lcc[level].rot + 2 : lcc[level].rot)) & 0x3;
       tt[i].flip = lcc[level].flip ^ tt[i].flip;
     }
     if(level > 0 && symtype && !strcmp(symtype, "label")) {
       char lay[30];
       my_snprintf(lay, S(lay), " layer=%d", WIRELAYER);
       my_strcat(_ALLOC_ID_, &tt[i].prop_ptr, lay);
     }
     dbg(1, "l_s_d(): loaded text : t=%s p=%s\n", tt[i].txt_ptr, tt[i].prop_ptr ? tt[i].prop_ptr : "<NULL>");
     set_text_flags(&tt[i]);
     ++lastt;
     break;
    case 'N': /* store wires as lines on layer WIRELAYER. */

     tmpline.prop_ptr = NULL;
     if(fscanf(lcc[level].fd, "%lf %lf %lf %lf ",&tmpline.x1, &tmpline.y1,
        &tmpline.x2, &tmpline.y2) < 4 ) {
       fprintf(errfp,"l_s_d(): WARNING:  missing fields for LINE object, ignoring\n");
       read_line(lcc[level].fd, 0);
       continue;
     }    
     load_ascii_string(&tmpline.prop_ptr, lcc[level].fd);

     if( !strboolcmp(get_tok_value(tmpline.prop_ptr, "symbol_ignore", 0), "true")) {
       my_free(_ALLOC_ID_, &tmpline.prop_ptr);
       continue;
     }
    
     i = lastl[WIRELAYER];
     my_realloc(_ALLOC_ID_, &ll[WIRELAYER],(i+1)*sizeof(xLine));
     ll[WIRELAYER][i].x1 = tmpline.x1;
     ll[WIRELAYER][i].y1 = tmpline.y1;
     ll[WIRELAYER][i].x2 = tmpline.x2;
     ll[WIRELAYER][i].y2 = tmpline.y2;
     ll[WIRELAYER][i].prop_ptr = tmpline.prop_ptr;

     if (level>0) {
       rot = lcc[level].rot; flip = lcc[level].flip;
       ROTATION(rot, flip, 0.0, 0.0, ll[WIRELAYER][i].x1, ll[WIRELAYER][i].y1, rx1, ry1);
       ROTATION(rot, flip, 0.0, 0.0, ll[WIRELAYER][i].x2, ll[WIRELAYER][i].y2, rx2, ry2);
       ll[WIRELAYER][i].x1 = lcc[level].x0 + rx1;  ll[WIRELAYER][i].y1 = lcc[level].y0 + ry1;
       ll[WIRELAYER][i].x2 = lcc[level].x0 + rx2;  ll[WIRELAYER][i].y2 = lcc[level].y0 + ry2;
     }
     ORDER(ll[WIRELAYER][i].x1, ll[WIRELAYER][i].y1, ll[WIRELAYER][i].x2, ll[WIRELAYER][i].y2);
     dbg(2, "l_s_d(): loaded line: ptr=%lx\n", (unsigned long)ll[WIRELAYER]);
     ll[WIRELAYER][i].dash = 0;
     if(!strboolcmp(get_tok_value(ll[WIRELAYER][i].prop_ptr, "bus", 0), "true"))
       ll[WIRELAYER][i].bus = 1;
     else
       ll[WIRELAYER][i].bus = 0;
     ll[WIRELAYER][i].sel = 0;
     lastl[WIRELAYER]++;
     break;
    case 'C': /* symbol is LCC: contains components */
      load_ascii_string(&symname, lcc[level].fd);
      if (fscanf(lcc[level].fd, "%lf %lf %hd %hd", &inst_x0, &inst_y0, &inst_rot, &inst_flip) < 4) {
        fprintf(errfp, "l_s_d(): WARNING: missing fields for COMPONENT object, ignoring\n");
        read_line(lcc[level].fd, 0);
        continue;
      }
      load_ascii_string(&prop_ptr, lcc[level].fd);
      dbg(1, "l_s_d() component: level=%d, sym=%s, prop_ptr = %s\n", level, symname, prop_ptr);
      if(level + 1 >=CADMAXHIER) {
        fprintf(errfp, "l_s_d(): Symbol recursively instantiating symbol: max depth reached, skipping\n");
        if(has_x) tcleval("alert_ {Symbol recursively instantiating symbol: max depth reached, skipping} {} 1");
        endfile = 1;
        continue;
      }

      if(generator) {
        /* for generators (data from a pipe) can not inspect next line (fseek/ftell) looking for
         * embedded symbols. Assume no embedded symbol follows */
        fd_tmp = NULL;
        get_sym_type(symname, &symtype, NULL, fd_tmp, &sym_n_pins);
      } else {
        filepos = xftell(lcc[level].fd); /* store file pointer position to inspect next line */
        fd_tmp = NULL;
        read_line(lcc[level].fd, 1);
        fscan_ret = fscanf(lcc[level].fd, " ");
        if(fscanf(lcc[level].fd," %c",&ch)!=EOF) {
          if( ch == '[') {
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
          !strcmp(symtype, "netlist_options") ||
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
      if(!strboolcmp(get_tok_value(prop_ptr, "symbol_ignore", 0), "true")) break;
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
        if(!generator) {
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
      }
      if(fd_tmp) {
        if (level+1 >= max_level) {
          my_realloc(_ALLOC_ID_, &lcc, (max_level + 1) * sizeof(Lcc));
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
          lcc[level].rot = (short)((lcc[(level-1)].flip ? map[lcc[level].rot] :
                           lcc[level].rot) + lcc[(level-1)].rot);
          lcc[level].rot &= 0x3;
          lcc[level].flip = lcc[level].flip ^ lcc[level-1].flip;
          lcc[level].x0 += lcc[(level-1)].x0;
          lcc[level].y0 += lcc[(level-1)].y0;
        }
        my_strdup(_ALLOC_ID_, &lcc[level].prop_ptr, prop_ptr);
        my_strdup(_ALLOC_ID_, &lcc[level].symname, symname);
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
       my_free(_ALLOC_ID_, &lcc[level].prop_ptr);
       my_free(_ALLOC_ID_, &lcc[level].symname);
       --level;
     } else {
       endfile=1;
     }
     break;
    default:
     if( tag[0] == '{' ) ungetc(tag[0], lcc[level].fd);
     read_record(tag[0], lcc[level].fd, 0);
     break;
   } /* switch(tag[0]) */
   /* if a 'C' line was encountered and level was incremented, rest of line must be read
      with lcc[level-1].fd file pointer */
   if(incremented_level)
     read_line(lcc[level-1].fd, 0); /* discard any remaining characters till (but not including) newline */
   else
     read_line(lcc[level].fd, 0); /* discard any remaining characters till (but not including) newline */
  } /* while(1) */
  if(!embed_fd) {
    dbg(1, "l_s_d(): fclose2, level=%d, fd=%p\n", level, lcc[0].fd);
    if(generator) pclose(lcc[0].fd);
    else fclose(lcc[0].fd);
  }
  if(embed_fd || strstr(transl_name, ".xschem_embedded_")) {
    symbol[symbols].flags |= EMBEDDED;
  } else {
    symbol[symbols].flags &= ~EMBEDDED;
  }
  dbg(2, "l_s_d(): finished parsing file\n");
  for(c=0;c<cadlayers; ++c)
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
  align_sch_pins_with_sym(transl_name, symbols);
  my_free(_ALLOC_ID_, &prop_ptr);
  my_free(_ALLOC_ID_, &lastl);
  my_free(_ALLOC_ID_, &lastr);
  my_free(_ALLOC_ID_, &lastp);
  my_free(_ALLOC_ID_, &lasta);
  my_free(_ALLOC_ID_, &ll);
  my_free(_ALLOC_ID_, &bb);
  my_free(_ALLOC_ID_, &aa);
  my_free(_ALLOC_ID_, &pp);
  my_free(_ALLOC_ID_, &lcc);
  my_free(_ALLOC_ID_, &aux_ptr);
  my_free(_ALLOC_ID_, &symname);
  my_free(_ALLOC_ID_, &symtype);
  recursion_counter--;
  sort_symbol_pins(xctx->sym[xctx->symbols].rect[PINLAYER],
                   xctx->sym[xctx->symbols].rects[PINLAYER],
                   xctx->sym[xctx->symbols].name);
  xctx->symbols++;
  my_free(_ALLOC_ID_, &transl_name);
  return 1;
}

void make_schematic_symbol_from_sel(void)
{
  char filename[PATH_MAX] = "";
  char name[1024]; 

  my_snprintf(name, S(name), "save_file_dialog {Save file} * INITIALLOADDIR");
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
  char *pinname[3] = {NULL, NULL, NULL};
  char *generic_pin = NULL;

  char *dir = NULL;
  char *prop = NULL;
  char schname[PATH_MAX];
  char *sub_prop;
  char *sub2_prop=NULL;
  char *str=NULL;
  struct stat buf;
  char *sch = NULL;
  size_t ln;

  my_strdup(_ALLOC_ID_, &pinname[0], tcleval("rel_sym_path [find_file_first ipin.sym]"));
  my_strdup(_ALLOC_ID_, &pinname[1], tcleval("rel_sym_path [find_file_first opin.sym]"));
  my_strdup(_ALLOC_ID_, &pinname[2], tcleval("rel_sym_path [find_file_first iopin.sym]"));
  my_strdup(_ALLOC_ID_, &generic_pin, tcleval("rel_sym_path [find_file_first generic_pin.sym]"));
  
  if(pinname[0] && pinname[1] && pinname[2] && generic_pin) {
    rebuild_selected_array();
    if(xctx->lastsel > 1)  return;
    if(xctx->lastsel==1 && xctx->sel_array[0].type==ELEMENT)
    {
      my_strdup2(_ALLOC_ID_, &sch,
        get_tok_value(xctx->sym[xctx->inst[xctx->sel_array[0].n].ptr].prop_ptr, "schematic", 0));
      my_strncpy(schname, abs_sym_path(sch, ""), S(schname));
      my_free(_ALLOC_ID_, &sch);
      if(!schname[0]) {
        my_strncpy(schname, add_ext(abs_sym_path(tcl_hook2(xctx->inst[xctx->sel_array[0].n].name), ""),
             ".sch"), S(schname));
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
      for(i=0;i<npin; ++i) {
        my_strdup(_ALLOC_ID_, &prop, rct[i].prop_ptr);
        if(!prop) continue;
        sub_prop=strstr(prop,"name=")+5;
        if(!sub_prop) continue;
        x=-120.0;
        ln = 100+strlen(sub_prop);
        my_realloc(_ALLOC_ID_, &str, ln);
        my_snprintf(str, ln, "name=g%d lab=%s", p++, sub_prop);
        fprintf(fd, "C {%s} %.16g %.16g %.16g %.16g ", generic_pin, x, 20.0*(ypos++), 0.0, 0.0 );
        save_ascii_string(str, fd, 1);
      } /* for(i) */
      npin = ptr->rects[PINLAYER];
      rct = ptr->rect[PINLAYER];
      for(j=0;j<3; ++j) {
        if(j==1) ypos=0;
        for(i=0;i<npin; ++i) {
          my_strdup(_ALLOC_ID_, &prop, rct[i].prop_ptr);
          if(!prop) continue;
          sub_prop=strstr(prop,"name=")+5;
          if(!sub_prop) continue;
          /* remove dir=... from prop string 20171004 */
          my_strdup(_ALLOC_ID_, &sub2_prop, subst_token(sub_prop, "dir", NULL));
  
          my_strdup(_ALLOC_ID_, &dir, get_tok_value(rct[i].prop_ptr,"dir",0));
          if(!sub2_prop) continue;
          if(!dir) continue;
          if(j==0) x=-120.0; else x=120.0;
          if(!strcmp(dir, pindir[j])) {
            ln = 100+strlen(sub2_prop);
            my_realloc(_ALLOC_ID_, &str, ln);
            my_snprintf(str, ln, "name=g%d lab=%s", p++, sub2_prop);
            fprintf(fd, "C {%s} %.16g %.16g %.16g %.16g ", pinname[j], x, 20.0*(ypos++), 0.0, 0.0);
            save_ascii_string(str, fd, 1);
          } /* if() */
        } /* for(i) */
      }  /* for(j) */
      fclose(fd);
    } /* if(xctx->lastsel...) */
    my_free(_ALLOC_ID_, &dir);
    my_free(_ALLOC_ID_, &prop);
    my_free(_ALLOC_ID_, &sub2_prop);
    my_free(_ALLOC_ID_, &str);
  } else {
    fprintf(errfp, "create_sch_from_sym(): location of schematic pins not found\n");
    tcleval("alert_ {create_sch_from_sym(): location of schematic pins not found} {}");
  }
  my_free(_ALLOC_ID_, &pinname[0]);
  my_free(_ALLOC_ID_, &pinname[1]);
  my_free(_ALLOC_ID_, &pinname[2]);
  my_free(_ALLOC_ID_, &generic_pin);
}

void descend_symbol(void)
{
  char *str=NULL;
  FILE *fd;
  char name[PATH_MAX];
  char name_embedded[PATH_MAX];
  int n = 0;
  struct stat buf;
  rebuild_selected_array();
  if(xctx->lastsel > 1)  return;
  if(xctx->lastsel==1 && xctx->sel_array[0].type==ELEMENT) {
    n =xctx->sel_array[0].n;
    if(xctx->modified)
    {
      int ret;
      ret = save(1, 1);
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
    my_snprintf(name, S(name), "%s", translate(n, xctx->inst[n].name));
    /* dont allow descend in the default missing symbol */
    if((xctx->inst[n].ptr+ xctx->sym)->type &&
       !strcmp( (xctx->inst[n].ptr+ xctx->sym)->type,"missing")) return;
  }
  else return;

  /* build up current hierarchy path */
  my_strdup(_ALLOC_ID_,  &str, xctx->inst[n].instname);
  my_strdup(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], xctx->sch_path[xctx->currsch]);
  my_strcat(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], str);
  my_strcat(_ALLOC_ID_, &xctx->sch_path[xctx->currsch+1], ".");
  if(xctx->portmap[xctx->currsch + 1].table) str_hash_free(&xctx->portmap[xctx->currsch + 1]);
  str_hash_init(&xctx->portmap[xctx->currsch + 1], HASHSIZE);

  xctx->sch_path_hash[xctx->currsch+1] = 0;
  my_free(_ALLOC_ID_, &str);

  /* store hierarchy of inst attributes and sym templates for hierarchic parameter substitution */
  my_strdup(_ALLOC_ID_, &xctx->hier_attr[xctx->currsch].prop_ptr,
            xctx->inst[n].prop_ptr);
  my_strdup(_ALLOC_ID_, &xctx->hier_attr[xctx->currsch].templ,
            get_tok_value(xctx->sym[xctx->inst[n].ptr].prop_ptr, "template", 0));

  if(!xctx->inst[n].embed)
    /* use -1 to keep track we are descending into symbol from instance with no embed attr
     * we use this info to avoid asking to save parent schematic when returning from a symbol
     * created from a generator */
    xctx->sch_inst_number[xctx->currsch] = -1;
  else
    xctx->sch_inst_number[xctx->currsch] = 1; /* inst number we descend into. For symbol always 1 */
  xctx->previous_instance[xctx->currsch]=n; /* instance we are descending from */

  /* store previous zoom area */
  xctx->zoom_array[xctx->currsch].x=xctx->xorigin;
  xctx->zoom_array[xctx->currsch].y=xctx->yorigin;
  xctx->zoom_array[xctx->currsch].zoom=xctx->zoom;

  if((xctx->inst[n].ptr+ xctx->sym)->flags & EMBEDDED || xctx->inst[n].embed) {
    /* save embedded symbol into a temporary file */
    my_snprintf(name_embedded, S(name_embedded),
      "%s/.xschem_embedded_%d_%s", tclgetvar("XSCHEM_TMP_DIR"), getpid(), get_cell_w_ext(name, 0));
    if(!(fd = fopen(name_embedded, "w")) ) {
      fprintf(errfp, "descend_symbol(): problems opening file %s \n", name_embedded);
    } else {
      save_embedded_symbol(xctx->inst[n].ptr+xctx->sym, fd);
      fclose(fd);
    }
    unselect_all(1);
    remove_symbols(); /* must follow save (if) embedded */
    /* load_symbol(name_embedded); */
    ++xctx->currsch; /* increment level counter */
    load_schematic(1, name_embedded, 1, 1);
  } else {
    char *sympath = NULL;
    char *current_dirname_save = NULL;
    int web_url;
    unselect_all(1);
    remove_symbols(); /* must follow save (if) embedded */

    web_url = is_from_web(xctx->current_dirname);

    /* ... we are in a schematic downloaded from web ... */
    if(web_url) {
      /* symbols have already been downloaded while loading parent schematic: set local file path */
      my_mstrcat(_ALLOC_ID_, &sympath, xschem_web_dirname, "/", get_cell_w_ext(tcl_hook2(name), 0), NULL);
      my_strdup(_ALLOC_ID_, &current_dirname_save, xctx->current_dirname); /* save http url */
    }
    if(!sympath || stat(sympath, &buf)) { /* not found */
      dbg(1, "descend_symbol: not found: %s\n", sympath);
      if(is_generator(name)) {
        my_strdup2(_ALLOC_ID_, &sympath, tcl_hook2(name));
      } else {
        my_strdup2(_ALLOC_ID_, &sympath, abs_sym_path(tcl_hook2(name), ""));
      }
    }
    dbg(1, "descend_symbol(): name=%s, sympath=%s, dirname=%s\n", name, sympath, xctx->current_dirname);
    ++xctx->currsch; /* increment level counter */
    load_schematic(1, sympath, 1, 1);
    if(web_url) {
      /* restore web url current_dirname that is reset by load_schematic with local path */
      my_strncpy(xctx->current_dirname, current_dirname_save, S(xctx->current_dirname));
      my_free(_ALLOC_ID_, &current_dirname_save);
    }
    my_free(_ALLOC_ID_, &sympath);
  }
  zoom_full(1, 0, 1 + 2 * tclgetboolvar("zoom_full_center"), 0.97);
}

/* 20111023 align selected object to current grid setting */
#define SNAP_TO_GRID(a)  (a=my_round(( a)/c_snap)*c_snap )
void round_schematic_to_grid(double c_snap)
{
 int i, c, n, p;
 rebuild_selected_array();
 for(i=0;i<xctx->lastsel; ++i)
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
 char *name;

 dbg(3, "save_selection():\n");
 if(what==1)
   name = sel_file;
 else /* what=2 */
   name = clip_file;

 if(!(fd=fopen(name,"w")))
 {
    fprintf(errfp, "save_selection(): problems opening file %s \n", name);
    tcleval("alert_ {file opening for write failed!} {}");
    return;
 }
 fprintf(fd, "v {xschem version=%s file_version=%s}\n", XSCHEM_VERSION, XSCHEM_FILE_VERSION);
 fprintf(fd, "G { %.16g %.16g }\n", xctx->mousex_snap, xctx->mousey_snap);
 for(i=0;i<xctx->lastsel; ++i)
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
      for(k=0; k<xctx->poly[c][n].points; ++k) {
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

