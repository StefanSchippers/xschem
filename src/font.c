/* File: font.c
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

void compile_font(void)
{
 int code, i;
 char name[PATH_MAX];

 currentsch = 0;
 my_snprintf(name, S(name), "%s/systemlib/font.sch", tclgetvar("XSCHEM_SHAREDIR"));
 unselect_all();
 remove_symbols();
 load_schematic(1,name,0);
 for(code=0;code<127;code++)
 {
  unselect_all();
  select_inside(code*FONTOFFSET-1,-FONTHEIGHT-1,
                 code*FONTOFFSET+FONTWIDTH+1,FONTWHITESPACE + FONTDESCENT+1, 1);
  rebuild_selected_array();
  character[code] = my_calloc(134, lastselected*4+1, sizeof(double));
  character[code][0] = (double)lastselected;
  dbg(2, "compile_font(): character[%d][]={%.16g",code,character[code][0]);
  for(i=0;i<lastselected;i++)
  {
   character[code][i*4+1] = 
      line[selectedgroup[i].col][selectedgroup[i].n].x1-code*FONTOFFSET;
   character[code][i*4+2] = 
      line[selectedgroup[i].col][selectedgroup[i].n].y1+FONTHEIGHT;
   character[code][i*4+3] = 
      line[selectedgroup[i].col][selectedgroup[i].n].x2-code*FONTOFFSET;
   character[code][i*4+4] = 
      line[selectedgroup[i].col][selectedgroup[i].n].y2+FONTHEIGHT;
   dbg(2, ",\n%.16g,%.16g,%.16g,%.16g",
    character[code][i*4+1],character[code][i*4+2],
    character[code][i*4+3],character[code][i*4+4]);
  }
  dbg(2, "};\n");
 }
 clear_drawing();
 unselect_all();
 currentsch = 0;
 my_strncpy(schematic[currentsch], "", S(schematic[currentsch]));
 if(event_reporting) {
   printf("xschem clear_drawing\n");
   printf("xschem unselect_all\n");
   fflush(stdout);
 }
}

