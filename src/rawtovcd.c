/* File: rawtovcd.c
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

/* create a vcd file from a ngspice raw file containing a transient time simulation*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define BUFSIZE 4095
int binary_waves=0;
double voltage = 3;
double vth=2.5;
double vtl=0.5;
int debug = 1;
FILE *fd;
int nvars = 0 , npoints = 0;
double **values = NULL;
char **names = NULL, **vcd_ids = NULL;
double timescale=1e11; /* spice times will be multiplied by this number to get an integer */
double rel_timestep_precision = 5e-3;
double abs_timestep_precision = 1e-10;

void replace_bracket(char *s)
{
  while(*s) {
   /* if(*s =='[' || *s == ']') *s='_'; */
   if(*s ==':') *s='.';
   ++s;
  }
}

/* get a short unique ascii identifier to identify node */
const char *get_vcd_id(int idx)
{
  static const char syms[] =
    "0123456789abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ=+-_)(*&^%$#@!~`:;',\"<.>/?|";
  static const int n = sizeof(syms)-1;
  int q, r, pos;
  static char res[32];

  q = idx;
  pos = 31;
  res[pos] = '\0';
  do {
    pos--;
    r = q % n;
    q /= n;
    res[pos] = syms[r];
  } while(q && pos > 0);
  return res + pos;
}

/* binary block is just a blob of npoints * nvars doubles */
void read_binary_block()
{
  int p;

  /* allocate storage for binary block */
  values = calloc(npoints, sizeof(double *));
  for(p = 0 ; p < npoints; p++) {
    values[p] = calloc(nvars, sizeof(double));
  }
  /* read binary block */
  for(p = 0; p < npoints; p++) {
    if(fread(values[p], sizeof(double), nvars, fd) != nvars) {
       fprintf(stderr, "Warning: binary block is not of correct size\n");
    }
  }
  if(debug) fprintf(stderr, "done reading binary block\n");
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
int read_dataset(void)
{
  int variables = 0, i, done_points = 0;
  char line[BUFSIZE + 1], varname[BUFSIZE + 1];
  const char *id;
  char *ptr;
  int transient = 0;
  npoints = 0;
  nvars = 0;
  while((ptr = fgets(line, sizeof(line), fd)) ) {
    if(!strncmp(line, "Binary:", 7)) break; /* start of binary block */
    if(transient) {
      if(variables) {
        sscanf(line, "%d %s", &i, varname); /* read index and name of saved waveform */
        names[i] = malloc(strlen(varname) + 1);
        strcpy(names[i], varname);
        replace_bracket(names[i]);
        id = get_vcd_id(i);
        vcd_ids[i] = malloc(strlen(id) + 1) ;
        strcpy(vcd_ids[i], id);
      }
      if(!strncmp(line, "Variables:", 10)) {
        variables = 1;
        names = calloc(nvars, sizeof(char *));
        vcd_ids = calloc(nvars, sizeof(char *));
      }
    }
    if(!strncmp(line, "No. of Data Rows :", 18)) {
      sscanf(line, "No. of Data Rows : %d", &npoints);
      done_points = 1;
    }
    if(!strncmp(line, "No. Variables:", 14)) {
      sscanf(line, "No. Variables: %d", &nvars);
    }
    if(!strncmp(line, "Plotname: Transient Analysis", 28)) {
      transient = 1;
    }
    if(!done_points && !strncmp(line, "No. Points:", 11)) {
      sscanf(line, "No. Points: %d", &npoints);
    }
  }
  if(!ptr) {
    if(debug) fprintf(stderr, "EOF found\n");
    variables = -1; /* EOF */
  }
  if(debug) fprintf(stderr, "npoints=%d, nvars=%d\n", npoints, nvars);
  if(variables == 1) {
    read_binary_block();
  } else if(variables == 0) {
    if(debug) fprintf(stderr, "seeking past binary block\n");
    fseek(fd, nvars * npoints * sizeof(double), SEEK_CUR); /* skip binary block */
  }
  return variables;
}

unsigned char tobin(double v)
{
  if(v > vth) return '1';
  else if(v < vtl) return '0';
  else return 'x';
}

void write_vcd_header()
{
  char t[20];
  int v;
  printf("$timescale\n");
  strcpy(t,
      timescale == 1e12 ? "1ps"   :
      timescale == 1e11 ? "10ps"  :
      timescale == 1e10 ? "100ps" :
      timescale == 1e9  ? "1ns"   :
      timescale == 1e8  ? "10ns"  :
      timescale == 1e7  ? "100ns" :
      timescale == 1e6  ? "1us"   :
      timescale == 1e5  ? "10us"  :
      timescale == 1e4  ? "100us" :
      timescale == 1e3  ? "1ms"   :
      timescale == 1e2  ? "10ms"  :
      timescale == 1e1  ? "100ms" :
      timescale == 1    ? "1s"    :
      timescale == 1e-1 ? "10s"   :
      timescale == 1e-2 ? "100s"  :
                          "1000s");
  printf("   %s\n", t);
  printf("$end\n");
  for(v = 1; v < nvars; v++) {
    if(binary_waves) printf("$var reg 1 %s %s $end\n", vcd_ids[v], names[v]);
    else             printf("$var real 1 %s %s $end\n", vcd_ids[v], names[v]);
  }
  printf("$enddefinitions $end\n");
}

void dump_vcd_waves()
{
  int p, v;
  double *lastvalue, val;

  lastvalue = malloc(nvars * sizeof(double));
  for(p = 0; p < npoints; p++) {
    if(p == 0) {
      printf("#0\n");
      printf("$dumpvars\n");
      for(v = 1 ; val = values[p][v], v < nvars; v++) {
        if(binary_waves) printf("%c%s\n", tobin(val), vcd_ids[v]);
        else             printf("r%.3g %s\n", val, vcd_ids[v]);
        if(binary_waves) lastvalue[v] = tobin(val);
        else             lastvalue[v] = val;
      }
      printf("$end\n");
    } else {
      printf("#%d\n", (int) (values[p][0] * timescale));
      for(v = 1 ; val = values[p][v], v < nvars; v++) {
        if(binary_waves) {
          if( tobin(val) != lastvalue[v] ) {
            printf("%c%s\n", tobin(val), vcd_ids[v]);
            lastvalue[v] = tobin(val);
          }
        } else {
          if(
             (val != 0.0 &&  fabs((val - lastvalue[v]) / val) > rel_timestep_precision) ||
             (val == 0.0 && fabs(val - lastvalue[v]) > abs_timestep_precision)
            ) {
            printf("r%.3g %s\n", val, vcd_ids[v]);
            lastvalue[v] = val;
          }
        }
      }
    }
  }
  free(lastvalue);
}

void free_storage()
{
  int i;

  for(i = 0 ; i < nvars; ++i) {
    free(names[i]);
    free(vcd_ids[i]);
  }
  for(i = 0 ; i < npoints; ++i) {
    free(values[i]);
  }
  free(values);
  free(names);
  free(vcd_ids);
}

int main(int argc, char *argv[])
{
  int res;
  int i = 1;

  for(i = 1; i < argc; ++i) {
    if(!strcmp(argv[i], "-v")) {
      ++i;
      if(i + 1 >= argc) continue;
      binary_waves = 1;
      voltage = atof(argv[i]);
      vth = voltage * 0.75;
      vtl = voltage * 0.25;
    } else if(argv[i][0] == '-') {
      ++i;
    } else {
      break;
    }
  }

  if(i >= argc) {
    fprintf(stderr, "Rawtovcd: convert a spice RAW file to VCD\n");
    fprintf(stderr, "If '-v voltage' is given transform waves to digital (binary values)\n");
    fprintf(stderr, "usage: rawtovcd [-v voltage] rawfile > vcdfile\n");
    exit(EXIT_FAILURE);
  }
  if(!strcmp(argv[i], "-")) fd = stdin;
  else fd = fopen(argv[i], "r");
  if(fd) for(;;) {
    if((res = read_dataset()) == 1) {
      write_vcd_header();
      dump_vcd_waves();
      free_storage();
      break;
    } else if(res == -1) {  /* EOF */
      fprintf(stderr, "rawtovcd: dataset not found in raw file\n");
      return EXIT_FAILURE;
    }
  } else {
    fprintf(stderr, "rawtovcd: failed to open file for reading\n");
    return EXIT_FAILURE;
  }
  if(fd && fd != stdin) fclose(fd);
  return EXIT_SUCCESS;
}
