#
#  File: create_graph.tcl
#  
#  This file is part of XSCHEM,
#  a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
#  simulation.
#  Copyright (C) 1998-2024 Stefan Frederik Schippers
# 
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


# procedure to create a graph in an empty xschem window and display waveforms
proc create_graph {title rawfile node {analysis tran} {color {4 5 6 7 8 9 10 11 12 13 14}}} {
  # clear window if not already empty
  xschem clear force

  # add title text
  xschem text 30 -350 0 0 $title {} 0.5 1
  # clear loaded raw file if any
  xschem raw_clear
  # set current layer to graph layer (grey, layer 2)
  xschem set rectcolor 2 
  # create a 300x400 rectangle
  xschem rect 0 -300 400 0
  # make it a graph
  xschem setprop rect 2 0 flags graph
  # read a simulation raw file
  xschem raw_read $rawfile $analysis
  # add nodes to display
  xschem setprop rect 2 0 node $node
  # set node colors
  xschem setprop rect 2 0 color $color
  # set thinner graph line widths
  xschem setprop rect 2 0 linewidth_mult 0.5
  # make xschem display full the graph rectangle
  xschem zoom_full
  # full zoom graph data on x axis
  xschem setprop rect 2 0 fullxzoom
  # full zoom graph data on y axis
  xschem setprop rect 2 0 fullyzoom
  # clear modified flag so we can quit without xschem asking to save changed file.
  xschem set_modify 0
}

