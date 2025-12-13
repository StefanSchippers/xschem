#!/bin/sh
#
#  File: change_ref.awk
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

awk '
BEGIN{
  lib["inv"]="cmoslib"
  lib["invh"]="cmoslib"
  lib["nand2"]="cmoslib"
  lib["nand3"]="cmoslib"
  lib["nand4"]="cmoslib"
  lib["nand5"]="cmoslib"
  lib["nd2"]="cmoslib"
  lib["nr2"]="cmoslib"
  lib["iv"]="cmoslib"
  lib["iv2"]="cmoslib"
  lib["iv3"]="cmoslib"
  lib["mux21t"]="cmoslib"
  lib["ao21"]="cmoslib"
  lib["ld1tq"]="cmoslib"
  lib["nor2"]="cmoslib"
  lib["nor3"]="cmoslib"
  lib["nor4"]="cmoslib"
  lib["oa21"]="cmoslib"
  lib["eo"]="cmoslib"
  lib["tg3"]="cmoslib"
  lib["passgate"]="cmoslib"
  lib["ipin"]="devices"
  lib["opin"]="devices"
  lib["lab"]="devices"
  lib["lab2"]="devices"
  lib["label"]="devices"
  lib["capa"]="devices"
  lib["cnwlva"]="devices"
  lib["cnwlvi"]="devices"
  lib["dnwpsu"]="devices"
  lib["res"]="devices"
  lib["rnx"]="devices"
  lib["rnx3"]="devices"
  lib["npn"]="devices"
  lib["pnp"]="devices"
  lib["nmos"]="devices"
  lib["nmos4"]="devices"
  lib["nmoshv"]="devices"
  lib["nmosnat"]="devices"
  lib["nmosnat4"]="devices"
  lib["pmos"]="devices"
  lib["pmos4"]="devices"
  lib["pmoshv"]="devices"
  lib["pmosnat"]="devices"
  lib["pmosnat4"]="devices"
  lib["rnwhv"]="devices"
  lib["vdd"]="devices"
  lib["vcvs"]="devices"
  lib["gnd"]="devices"
  lib["vsource"]="devices"


}

/^C {.*} /{
 a=$2
 gsub(/[{}]/,"",a)
 if(a in lib) $2="{" lib[a] "/" a "}"
}

{
 print $0 > "____ZZZ____"
}

' $@

mv ____ZZZ____ $1
