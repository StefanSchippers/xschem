/* File: check.c
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


void check_touch(int i, int j,
         unsigned short *parallel,unsigned short *breaks,
         unsigned short *broken,unsigned short *touches,
         unsigned short *included, unsigned short *includes,
         double *xt, double *yt)
{
 int touch1=0,touch2=0,touch3=0,touch4=0;
 double delta1x,delta1y,delta2x,delta2y;
 double x1,y1,x2,y2;
 x1=wire[i].x1;
 x2=wire[i].x2;
 y1=wire[i].y1;
 y2=wire[i].y2;
 delta1x = x2-x1;delta1y = y1-y2 ;
 delta2x = wire[j].x2-wire[j].x1;delta2y = wire[j].y1-wire[j].y2 ;

 *included = 0;
 *includes = 0;
 *touches = 0;
 *broken = 0; 
 *breaks = 0;
 *parallel = (delta1x*delta2y == delta2x*delta1y ? 1:0);

 /* the order of the following 4 if(touch...) is not don't care !!! */
 if(touch(wire[j].x1,wire[j].y1,wire[j].x2,wire[j].y2,x1,y1) )
 {
  *touches = 1;touch3 =1;
  *xt = x1; *yt = y1;
  if((*xt > wire[j].x1 && *xt < wire[j].x2)||
   (*yt > wire[j].y1 && *yt < wire[j].y2)) *breaks = 1;
 }
 if(touch(wire[j].x1,wire[j].y1,wire[j].x2,wire[j].y2,x2,y2) )
 {
  *touches = 1;touch4 =1;
  *xt = x2; *yt = y2;
  if((*xt > wire[j].x1 && *xt < wire[j].x2)||
    (*yt > wire[j].y1 && *yt < wire[j].y2)) *breaks = 1;
 }
 if(touch3 && touch4) *included = 1;
 
 if(touch(x1,y1,x2,y2,wire[j].x1,wire[j].y1) )
 {
  *touches = 1;touch1=1;
  *xt = wire[j].x1; *yt = wire[j].y1;
  if((*xt > x1 && *xt < x2)||(*yt > y1 && *yt < y2)) *broken = 1;
 }
 if(touch(x1,y1,x2,y2,wire[j].x2,wire[j].y2) )
 {
  *touches = 1;touch2=1;
  *xt = wire[j].x2; *yt = wire[j].y2;
  if((*xt > x1 && *xt < x2)||(*yt > y1 && *yt < y2)) *broken = 1;
 }
 if(touch1 && touch2) *includes = 1;
  dbg(2, "check_touch(): xt=%.16g, yt=%.16g\n",*xt, *yt);
}

void update_conn_cues(int draw_cues, int dr_win)
{
  int k, i, l, sqx, sqy, save_draw;
  struct wireentry *wptr;
  double x0, y0;
  double x1, y1, x2, y2;
  struct wireentry *wireptr;
  
  if(!lastwire) return;
  if(!draw_dots) return;
  if(cadhalfdotsize*mooz<0.7) return;
  x1 = X_TO_XSCHEM(areax1);
  y1 = Y_TO_XSCHEM(areay1);
  x2 = X_TO_XSCHEM(areax2);
  y2 = Y_TO_XSCHEM(areay2);
  hash_wires();
  for(init_wire_iterator(x1, y1, x2, y2); ( wireptr = wire_iterator_next() ) ;) {
    k=wireptr->n;
    /* optimization when editing small areas (detailed zoom)  of a huge schematic */
    if(LINE_OUTSIDE(wire[k].x1, wire[k].y1, wire[k].x2, wire[k].y2, x1, y1, x2, y2)) continue;
    for(l = 0;l < 2;l++) {
      if(l==0 ) {
        if(wire[k].end1 !=-1) continue; /* 20181103 */
        wire[k].end1=0;
        x0 = wire[k].x1;
        y0 = wire[k].y1;
      } else {
        if(wire[k].end2 !=-1) continue; /* 20181103 */
        wire[k].end2=0;
        x0 = wire[k].x2;
        y0 = wire[k].y2;
      }
      get_square(x0, y0, &sqx, &sqy);
      for(wptr = wiretable[sqx][sqy] ; wptr ; wptr = wptr->next) {
        i = wptr->n;
        if(i == k) {
          continue; /* no check wire against itself */
        }
        if( touch(wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2, x0,y0) ) {
          if( (x0 != wire[i].x1 && x0 != wire[i].x2) ||
              (y0 != wire[i].y1 && y0 != wire[i].y2) ) {
            if(l == 0) wire[k].end1 += 2;
            else     wire[k].end2 += 2;
          } else {
            if(l == 0) wire[k].end1 += 1;
            else     wire[k].end2 += 1;
          }
        }
      }
    }
  }
  dbg(3, "update_conn_cues(): check3\n");
  if(draw_cues) {
    save_draw = draw_window; draw_window = dr_win;
    filledarc(WIRELAYER, BEGIN, 0.0, 0.0, 0.0, 0.0, 0.0);
    for(init_wire_iterator(x1, y1, x2, y2); ( wireptr = wire_iterator_next() ) ;) {
      i = wireptr->n;
      /* optimization when editing small areas (detailed zoom)  of a huge schematic */
      if(LINE_OUTSIDE(wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2, x1, y1, x2, y2)) continue;
      if( wire[i].end1 >1 ) { /* 20150331 draw_dots */
        filledarc(WIRELAYER, ADD, wire[i].x1, wire[i].y1, cadhalfdotsize, 0, 360);
      }
      if( wire[i].end2 >1 ) { /* 20150331 draw_dots */
        filledarc(WIRELAYER, ADD, wire[i].x2, wire[i].y2, cadhalfdotsize, 0, 360);
      }
    }
    filledarc(WIRELAYER, END, 0.0, 0.0, 0.0, 0.0, 0.0);
    draw_window = save_draw;
  }
}

void trim_wires(void)
/* wire coordinates must be ordered. */
{
 int j,i, changed;
 unsigned short parallel,breaks,broken,touches,included,includes;
 double xt=0,yt=0;
 int loops=0;

 do { 
   loops++;
   for(i=0;i<lastwire;i++)  wire[i].end1=wire[i].end2=0;
   changed=0;
   for(i=0;i<lastwire;i++)
   {
    for(j=i+1;j<lastwire;j++)
    {
      check_touch(i,j, &parallel,&breaks,&broken,&touches,&included,&includes, &xt,&yt);
      if(included) 
      {
         freenet_nocheck(i);
         i--;
         changed=1;
         break;
      }
      if(includes)
      {
         freenet_nocheck(j);
         changed=1;
         j--;
         continue;
      }
      if(touches)
      { 
         if(broken)
         {
           check_wire_storage();
           changed=1;
           wire[lastwire].x1=wire[i].x1;
           wire[lastwire].y1=wire[i].y1;
           wire[lastwire].end1=wire[i].end1;
           wire[lastwire].end2=1;
           wire[lastwire].x2=xt;
           wire[lastwire].y2=yt;
           wire[lastwire].sel=0;
           wire[lastwire].prop_ptr=NULL;
           my_strdup(27, &wire[lastwire].prop_ptr, wire[i].prop_ptr);
           if(!strcmp(get_tok_value(wire[lastwire].prop_ptr,"bus",0), "true")) /* 20171201 */
             wire[lastwire].bus=1;
           else
             wire[lastwire].bus=0;
           wire[lastwire].node=NULL;
           my_strdup(28, &wire[lastwire].node, wire[i].node);
           lastwire++;
  
           wire[i].x1 = xt;
           wire[i].y1 = yt;
           wire[i].end1 = 1;
         } /* end if broken */
         else if(breaks) /*wire[i] breaks wire[j] */
         {
           changed=1;
           if(wire[i].x1==xt && wire[i].y1==yt) wire[i].end1+=1;
           else if(wire[i].x2==xt && wire[i].y2==yt) wire[i].end2+=1;
  
           check_wire_storage();
           wire[lastwire].x1=wire[j].x1;
           wire[lastwire].y1=wire[j].y1;
           wire[lastwire].end1=wire[j].end1;
           wire[lastwire].end2=1;
           wire[lastwire].x2=xt;
           wire[lastwire].y2=yt;
           wire[lastwire].sel=0;
           wire[lastwire].prop_ptr=NULL;
           my_strdup(29, &wire[lastwire].prop_ptr, wire[j].prop_ptr);
           if(!strcmp(get_tok_value(wire[lastwire].prop_ptr,"bus",0), "true")) /* 20171201 */
             wire[lastwire].bus=1;
           else
             wire[lastwire].bus=0;
           wire[lastwire].node=NULL;
           my_strdup(30, &wire[lastwire].node, wire[j].node);
           lastwire++;
  
           wire[j].x1 = xt;
           wire[j].y1 = yt;
           wire[j].end1 = 1;
         } /* end else if breaks */
         else  /* wire[i] touches but does not break wire[j] */
         {
          if(wire[i].x1==wire[j].x1 && wire[i].y1==wire[j].y1)
            {wire[i].end1++;wire[j].end1++;}
          else if(wire[i].x1==wire[j].x2 && wire[i].y1==wire[j].y2)
            {wire[i].end1++;wire[j].end2++;}
          else if(wire[i].x2==wire[j].x1 && wire[i].y2==wire[j].y1)
            {wire[i].end2++;wire[j].end1++;}
          else 
            {wire[i].end2++;wire[j].end2++;}
         }
      } /* end if touches */
    } /* end for j */
   } /* end for i  */
   for(i=0;i<lastwire;i++) {
    for(j=i+1;j<lastwire;j++) {
      check_touch(i,j, &parallel,&breaks,&broken,&touches,&included,&includes, &xt,&yt);
      if( touches && parallel)
      {
         if(wire[j].x1 == xt && wire[j].y1 == yt) /* touch in x1, y1 */
         {
            if(wire[i].end2 == 1 && wire[j].end1 == 1)  /* merge wire */
            {
               changed=1;
               wire[i].x2 = wire[j].x2;wire[i].y2 = wire[j].y2;
               wire[i].end2=wire[j].end2;
               freenet_nocheck(j);
               j--;
               break;
            }
         }
         else  /* touch in x2,y2 */
         {
            if(wire[i].end1 == 1 && wire[j].end2 == 1)  /* merge wire */
            {
               changed=1;
               wire[i].x1 = wire[j].x1;wire[i].y1 = wire[j].y1;
               wire[i].end1=wire[j].end1;
               freenet_nocheck(j);
               j--;
               break;
            }
         }
      } /* end if touches && parallel */
    } /* end for j */
   } /* end for i  */
   if(changed) {
     set_modify(1);
     prepared_netlist_structs=0;
     prepared_hilight_structs=0;
     prepared_hash_wires=0;
   }
  } while( changed ) ;
  dbg(1, "trim_wires:loops=%d\n", loops);
  draw_dots=1;
}
 
void break_wires_at_pins(void)
{
  int k, i, j, r, rects, rot, flip, sqx, sqy;
  struct wireentry *wptr;
  Box *rect;
  double x0, y0, rx1, ry1;
  int changed=0;
  hash_wires();

  need_rebuild_selected_array=1;
  rebuild_selected_array();
  
  /* for(k=0;k<lastinst;k++) */
  for(j=0;j<lastselected;j++) if(selectedgroup[j].type==ELEMENT) {
    k = selectedgroup[j].n;
    if( (rects = (inst_ptr[k].ptr+instdef)->rects[PINLAYER]) > 0 )
    {
      for(r=0;r<rects;r++)
      {
        rect=(inst_ptr[k].ptr+instdef)->boxptr[PINLAYER];
        x0=(rect[r].x1+rect[r].x2)/2;
        y0=(rect[r].y1+rect[r].y2)/2;
        rot=inst_ptr[k].rot;
        flip=inst_ptr[k].flip;
        ROTATION(0.0,0.0,x0,y0,rx1,ry1);
        x0=inst_ptr[k].x0+rx1;
        y0=inst_ptr[k].y0+ry1;
        get_square(x0, y0, &sqx, &sqy);
        for(wptr=wiretable[sqx][sqy]; wptr; wptr=wptr->next) {
          i = wptr->n;
          if( touch(wire[i].x1, wire[i].y1,
                    wire[i].x2, wire[i].y2, x0,y0) )
          {
            if( (x0!=wire[i].x1 && x0!=wire[i].x2) ||
                (y0!=wire[i].y1 && y0!=wire[i].y2) ) {
              if(!changed) { push_undo(); changed=1;}
              check_wire_storage();
              wire[lastwire].x1=wire[i].x1;
              wire[lastwire].y1=wire[i].y1;
              wire[lastwire].x2=x0;
              wire[lastwire].y2=y0;
              wire[lastwire].sel=SELECTED;
              wire[lastwire].prop_ptr=NULL;
              my_strdup(31, &wire[lastwire].prop_ptr, wire[i].prop_ptr);
              if(!strcmp(get_tok_value(wire[lastwire].prop_ptr,"bus",0), "true")) /* 20171201 */
                wire[lastwire].bus=1;
              else
                wire[lastwire].bus=0;
              wire[lastwire].node=NULL;
              hash_wire(XINSERT, lastwire);
              my_strdup(32, &wire[lastwire].node, wire[i].node);
              need_rebuild_selected_array=1;
              lastwire++;
              wire[i].x1 = x0;
              wire[i].y1 = y0;
            }
          }
        }
      }
    }
  }
  /* prepared_hash_wires=0; */
  /* hash_wires(); */
  rebuild_selected_array();
  for(j=0;j<lastselected;j++) if(selectedgroup[j].type==WIRE) {
  /* for(k=0; k < lastwire; k++) { */
    int l;

    k = selectedgroup[j].n;
    for(l=0;l<2;l++) {
      if(l==0 ) {
        x0 = wire[k].x1;
        y0 = wire[k].y1;
      } else {
        x0 = wire[k].x2;
        y0 = wire[k].y2;
      }
      get_square(x0, y0, &sqx, &sqy);
      /* printf("  k=%d, x0=%g, y0=%g\n", k, x0, y0); */
      for(wptr=wiretable[sqx][sqy] ; wptr ; wptr = wptr->next) {
        i = wptr->n;
        /* printf("check wire %d to wire %d\n", k, i); */
        if(i==k) {
          continue; /* no check wire against itself */
        }
        /* printf("  i=%d, x1=%g, y1=%g x2=%g, y2=%g\n", i, wire[i].x1, wire[i].y1, wire[i].x2, wire[i].y2); */
        if( touch(wire[i].x1, wire[i].y1,
                  wire[i].x2, wire[i].y2, x0,y0) )
        {
          if( (x0!=wire[i].x1 && x0!=wire[i].x2) ||
              (y0!=wire[i].y1 && y0!=wire[i].y2) ) {
            /* printf("touch in mid point: %d\n", l+1); */
            if(!changed) { push_undo(); changed=1;}
            check_wire_storage();
            wire[lastwire].x1=wire[i].x1;
            wire[lastwire].y1=wire[i].y1;
            wire[lastwire].x2=x0;
            wire[lastwire].y2=y0;
            wire[lastwire].sel=SELECTED;
            wire[lastwire].prop_ptr=NULL;
            my_strdup(33, &wire[lastwire].prop_ptr, wire[i].prop_ptr);
            if(!strcmp(get_tok_value(wire[lastwire].prop_ptr,"bus",0), "true")) /* 20171201 */
              wire[lastwire].bus=1;
            else
              wire[lastwire].bus=0;
            wire[lastwire].node=NULL;
            hash_wire(XINSERT, lastwire);
            need_rebuild_selected_array=1;
            lastwire++;
            wire[i].x1 = x0;
            wire[i].y1 = y0;
          }
        }
      }
    }
  }
  set_modify(1);
  prepared_netlist_structs=0;
  prepared_hilight_structs=0;
  prepared_hash_wires=0;
  /*update_conn_cues(0, 0); */

}
