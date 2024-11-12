v {xschem version=3.4.5 file_version=1.2
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
}
G {}
K {}
V {}
S {}
E {}
B 2 1240 -590 2040 -80 {flags=graph
y1=0
y2=2.1
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0
x2=5e-08
divx=5
subdivx=1
node="out1
out2
out3
out4
out5
out6
out7"
color="7 8 9 10 11 12 13"
dataset=-1
unitx=1
logx=0
logy=0
hilight_wave=-1}
B 2 1240 -1010 2040 -600 {flags=graph
y1=0
y2=2
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0
x2=5e-08
divx=5
subdivx=1


dataset=-1
unitx=1
logx=0
logy=0
hilight_wave=-1
color="4 7"
node="plus minus"}
T {Default instance:
Uses comp3.sch} 320 -960 0 0 0.4 0.4 { layer=7}
T {Alternate instance:
Uses comp3_parax.sch} 10 -960 0 0 0.4 0.4 { layer=8}
T {Alternate instance:
Uses comp3_pex
contained in attribute
spice_sym_def
No schematic used} 10 -760 0 0 0.4 0.4 { layer=9}
T {Alternate instance:
Uses comp3_empty.sch
netlist embedded in global
spice schematic attribute} 320 -750 0 0 0.4 0.4 { layer=11}
T {Alternate instance:
Uses spice_sym_def to read in 
file comp3_file.cir
no schematic used} 330 -460 0 0 0.4 0.4 { layer=12}
T {The same symbol is simulated with 5 different implementations
using instance 'schematic' and 'spice_sym_def ' attributes} 30 -1040 0 0 0.4 0.4 { layer=4 slant=oblique}
T {Instance based
implementation selection.} 100 -1140 0 0 0.8 0.8 {}
T {comp3_read.sym:
symbol has "spice_sym_def"
attribute and reads in a file} 740 -410 0 0 0.4 0.4 { layer=13}
T {Alternate instance:
Uses comp3_pex2
referenced by a 
.include comp3_pex2.cir
in spice_sym_def
No schematic used} 10 -500 0 0 0.4 0.4 { layer=10}
T {Symbol based
implementation selection.} 650 -530 2 1 0.8 0.8 {}
T {The 'spice_sym_def ' attribute is defined inside the
symbol, this applies to all placements of this symbol} 640 -510 0 0 0.4 0.4 { layer=4 slant=oblique}
T {In all cases where an external netlist file
is provided either directly or by a .include
line xschem will adapt port order of instances
to match the port order of the provided
netlists.} 620 -940 0 0 0.5 0.5 {}
C {comp3.sym} 480 -850 0 0 {name=x1}
C {comp3.sym} 180 -850 0 0 {name=x2
schematic=comp3_parax.sch}
C {comp3.sym} 180 -580 0 0 {name=x3
schematic=comp3_pex
spice_sym_def=".subckt comp3_pex MINUS PLUS OUT
** parasitic netlist
cparax1 net1 0 20f
cparax2 net2 0 20f
cparax3 net3 0 20f
cparax4 net4 0 20f
cparax5 net5 0 20f
cparaxout out 0 40f
M1 net1 GN1 0 0 nmos w=4u l=0.4u m=1
M2 GN1 GN1 0 0 nmos w=4u l=0.4u m=1
I0 VCC GN1 30u
M3 net2 MINUS net1 0 nmos w=1.5u l=0.2u m=1
M4 net3 PLUS net1 0 nmos w=1.5u l=0.2u m=1
M5 net2 net2 VCC VCC pmos w=6u l=0.3u m=1
M6 net3 net2 VCC VCC pmos w=6u l=0.3u m=1
M14 net4 net3 VCC VCC pmos w=6u l=0.3u m=1
.save v(gn1)
.save v(net2)
M7 net4 GN1 0 0 nmos w=4u l=0.4u m=1
M8 net5 net4 0 0 nmos w=1u l=0.4u m=1
M9 net5 net4 VCC VCC pmos w=2u l=0.4u m=1
M10 OUT net5 0 0 nmos w=1u l=0.4u m=1
M11 OUT net5 VCC VCC pmos w=2u l=0.4u m=1
M13 net4 net4 net5 0 nmos w=2u l=0.1u m=1
M12 net5 net5 net4 0 nmos w=2u l=0.1u m=1
C2 GN1 0 200f m=1
.ends"

verilog_sym_def="verilog stuff"
vhdl_sym_def="vhdl stuff"}
C {comp3.sym} 180 -290 0 0 {name=x4
schematic=comp3_pex2
spice_sym_def="tcleval(.include [abs_sym_path comp3_pex2.cir])"

verilog_sym_def="verilog stuff"
vhdl_sym_def="vhdl stuff"}
C {comp3.sym} 480 -580 0 0 {name=x5
schematic=comp3_empty.sch}
C {comp3.sym} 480 -290 0 0 {name=x6
schematic=comp3_file
spice_sym_def="tcleval(
  [read_data_nonewline [abs_sym_path comp3_file.cir]]
)"

vhdl_sym_def="tcleval(
  [read_data_nonewline [abs_sym_path comp3_file.cir]]
)"
tclcommand="textwindow [abs_sym_path comp3_file.cir]"}
C {comp3_read.sym} 890 -280 0 0 {name=x7

tclcommand="textwindow [abs_sym_path comp3_read.cir]"}
C {lab_pin.sym} 540 -850 0 1 {name=p2 lab=OUT1}
C {lab_pin.sym} 240 -850 0 1 {name=p5 lab=OUT2}
C {lab_pin.sym} 240 -580 0 1 {name=p8 lab=OUT3}
C {lab_pin.sym} 240 -290 0 1 {name=p35 lab=OUT4}
C {lab_pin.sym} 540 -580 0 1 {name=p16 lab=OUT5}
C {lab_pin.sym} 540 -290 0 1 {name=p19 lab=OUT6}
C {lab_pin.sym} 950 -280 0 1 {name=p23 lab=OUT7}
C {lab_pin.sym} 120 -820 0 0 {name=p6 lab=MINUS}
C {lab_pin.sym} 700 -90 0 0 {name=p10 lab=0}
C {vsource.sym} 820 -120 0 0 {name=V2 value=1
savecurrent=true}
C {lab_pin.sym} 820 -150 0 0 {name=p11 lab=MINUS}
C {lab_pin.sym} 820 -90 0 0 {name=p12 lab=0}
C {vsource.sym} 970 -120 0 0 {name=V3 value="pwl 0 0 10n 0 20n 2 30n 2 40n 0"
savecurrent=true}
C {lab_pin.sym} 970 -150 0 0 {name=p13 lab=PLUS}
C {lab_pin.sym} 970 -90 0 0 {name=p14 lab=0}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {launcher.sym} 1380 -40 0 0 {name=h5
descr="load waves" 
tclcommand="xschem raw_read $netlist_dir/test_instance_schematic_selection.raw tran"
}
C {code_shown.sym} 140 -170 0 0 {name=COMMANDS only_toplevel=false value=".control
  save all
  tran 0.05n 50n
  write test_instance_schematic_selection.raw
.endc
"}
C {lab_pin.sym} 420 -880 0 0 {name=p1 lab=PLUS}
C {lab_pin.sym} 420 -610 0 0 {name=p15 lab=PLUS}
C {code.sym} 0 -200 0 0 {name=MODELS only_toplevel=false value="* Beta Version released on 2/22/06

* PTM 65nm NMOS 
 
.model nmos nmos level = 54
+version = 4.0 binunit = 1 paramchk= 1 mobmod = 0 
+capmod = 2 igcmod = 1 igbmod = 1 geomod = 1 
+diomod = 1 rdsmod = 0 rbodymod= 1 rgatemod= 1 
+permod = 1 acnqsmod= 0 trnqsmod= 0 
+tnom = 27 toxe = 1.85e-9 toxp = 1.2e-9 toxm = 1.85e-9 
+dtox = 0.65e-9 epsrox = 3.9 wint = 5e-009 lint = 5.25e-009 
+ll = 0 wl = 0 lln = 1 wln = 1 
+lw = 0 ww = 0 lwn = 1 wwn = 1 
+lwl = 0 wwl = 0 xpart = 0 toxref = 1.85e-9 
+xl = -30e-9
+vth0 = 0.423 k1 = 0.4 k2 = 0.01 k3 = 0 
+k3b = 0 w0 = 2.5e-006 dvt0 = 1 dvt1 = 2 
+dvt2 = -0.032 dvt0w = 0 dvt1w = 0 dvt2w = 0 
+dsub = 0.1 minv = 0.05 voffl = 0 dvtp0 = 1.0e-009 
+dvtp1 = 0.1 lpe0 = 0 lpeb = 0 xj = 1.96e-008 
+ngate = 2e+020 ndep = 2.54e+018 nsd = 2e+020 phin = 0 
+cdsc = 0.000 cdscb = 0 cdscd = 0 cit = 0 
+voff = -0.13 nfactor = 1.9 eta0 = 0.0058 etab = 0 
+vfb = -0.55 u0 = 0.0491 ua = 6e-010 ub = 1.2e-018 
+uc = 0 vsat = 124340 a0 = 1.0 ags = 1e-020 
+a1 = 0 a2 = 1.0 b0 = 0 b1 = 0 
+keta = 0.04 dwg = 0 dwb = 0 pclm = 0.04 
+pdiblc1 = 0.001 pdiblc2 = 0.001 pdiblcb = -0.005 drout = 0.5 
+pvag = 1e-020 delta = 0.01 pscbe1 = 8.14e+008 pscbe2 = 1e-007 
+fprout = 0.2 pdits = 0.08 pditsd = 0.23 pditsl = 2.3e+006 
+rsh = 5 rdsw = 165 rsw = 85 rdw = 85 
+rdswmin = 0 rdwmin = 0 rswmin = 0 prwg = 0 
+prwb = 6.8e-011 wr = 1 alpha0 = 0.074 alpha1 = 0.005 
+beta0 = 30 agidl = 0.0002 bgidl = 2.1e+009 cgidl = 0.0002 
+egidl = 0.8 
+aigbacc = 0.012 bigbacc = 0.0028 cigbacc = 0.002 
+nigbacc = 1 aigbinv = 0.014 bigbinv = 0.004 cigbinv = 0.004 
+eigbinv = 1.1 nigbinv = 3 aigc = 0.012 bigc = 0.0028 
+cigc = 0.002 aigsd = 0.012 bigsd = 0.0028 cigsd = 0.002 
+nigc = 1 poxedge = 1 pigcd = 1 ntox = 1 
+xrcrg1 = 12 xrcrg2 = 5 
+cgso = 1.5e-010 cgdo = 1.5e-010 cgbo = 2.56e-011 cgdl = 2.653e-10 
+cgsl = 2.653e-10 ckappas = 0.03 ckappad = 0.03 acde = 1 
+moin = 15 noff = 0.9 voffcv = 0.02 
+kt1 = -0.11 kt1l = 0 kt2 = 0.022 ute = -1.5 
+ua1 = 4.31e-009 ub1 = 7.61e-018 uc1 = -5.6e-011 prt = 0 
+at = 33000 
+fnoimod = 1 tnoimod = 0 
+jss = 0.0001 jsws = 1e-011 jswgs = 1e-010 njs = 1 
+ijthsfwd= 0.01 ijthsrev= 0.001 bvs = 10 xjbvs = 1 
+jsd = 0.0001 jswd = 1e-011 jswgd = 1e-010 njd = 1 
+ijthdfwd= 0.01 ijthdrev= 0.001 bvd = 10 xjbvd = 1 
+pbs = 1 cjs = 0.0005 mjs = 0.5 pbsws = 1 
+cjsws = 5e-010 mjsws = 0.33 pbswgs = 1 cjswgs = 3e-010 
+mjswgs = 0.33 pbd = 1 cjd = 0.0005 mjd = 0.5 
+pbswd = 1 cjswd = 5e-010 mjswd = 0.33 pbswgd = 1 
+cjswgd = 5e-010 mjswgd = 0.33 tpb = 0.005 tcj = 0.001 
+tpbsw = 0.005 tcjsw = 0.001 tpbswg = 0.005 tcjswg = 0.001 
+xtis = 3 xtid = 3 
+dmcg = 0e-006 dmci = 0e-006 dmdg = 0e-006 dmcgt = 0e-007 
+dwj = 0.0e-008 xgw = 0e-007 xgl = 0e-008 
+rshg = 0.4 gbmin = 1e-010 rbpb = 5 rbpd = 15 
+rbps = 15 rbdb = 15 rbsb = 15 ngcon = 1 

* PTM 65nm PMOS
 
.model pmos pmos level = 54
+version = 4.0 binunit = 1 paramchk= 1 mobmod = 0 
+capmod = 2 igcmod = 1 igbmod = 1 geomod = 1 
+diomod = 1 rdsmod = 0 rbodymod= 1 rgatemod= 1 
+permod = 1 acnqsmod= 0 trnqsmod= 0 
+tnom = 27 toxe = 1.95e-009 toxp = 1.2e-009 toxm = 1.95e-009 
+dtox = 0.75e-9 epsrox = 3.9 wint = 5e-009 lint = 5.25e-009 
+ll = 0 wl = 0 lln = 1 wln = 1 
+lw = 0 ww = 0 lwn = 1 wwn = 1 
+lwl = 0 wwl = 0 xpart = 0 toxref = 1.95e-009 
+xl = -30e-9
+vth0 = -0.365 k1 = 0.4 k2 = -0.01 k3 = 0 
+k3b = 0 w0 = 2.5e-006 dvt0 = 1 dvt1 = 2 
+dvt2 = -0.032 dvt0w = 0 dvt1w = 0 dvt2w = 0 
+dsub = 0.1 minv = 0.05 voffl = 0 dvtp0 = 1e-009 
+dvtp1 = 0.05 lpe0 = 0 lpeb = 0 xj = 1.96e-008 
+ngate = 2e+020 ndep = 1.87e+018 nsd = 2e+020 phin = 0 
+cdsc = 0.000 cdscb = 0 cdscd = 0 cit = 0 
+voff = -0.126 nfactor = 1.9 eta0 = 0.0058 etab = 0 
+vfb = 0.55 u0 = 0.00574 ua = 2.0e-009 ub = 0.5e-018 
+uc = 0 vsat = 70000 a0 = 1.0 ags = 1e-020 
+a1 = 0 a2 = 1 b0 = -1e-020 b1 = 0 
+keta = -0.047 dwg = 0 dwb = 0 pclm = 0.12 
+pdiblc1 = 0.001 pdiblc2 = 0.001 pdiblcb = 3.4e-008 drout = 0.56 
+pvag = 1e-020 delta = 0.01 pscbe1 = 8.14e+008 pscbe2 = 9.58e-007 
+fprout = 0.2 pdits = 0.08 pditsd = 0.23 pditsl = 2.3e+006 
+rsh = 5 rdsw = 165 rsw = 85 rdw = 85 
+rdswmin = 0 rdwmin = 0 rswmin = 0 prwg = 3.22e-008 
+prwb = 6.8e-011 wr = 1 alpha0 = 0.074 alpha1 = 0.005 
+beta0 = 30 agidl = 0.0002 bgidl = 2.1e+009 cgidl = 0.0002 
+egidl = 0.8 
+aigbacc = 0.012 bigbacc = 0.0028 cigbacc = 0.002 
+nigbacc = 1 aigbinv = 0.014 bigbinv = 0.004 cigbinv = 0.004 
+eigbinv = 1.1 nigbinv = 3 aigc = 0.69 bigc = 0.0012 
+cigc = 0.0008 aigsd = 0.0087 bigsd = 0.0012 cigsd = 0.0008 
+nigc = 1 poxedge = 1 pigcd = 1 ntox = 1 
+xrcrg1 = 12 xrcrg2 = 5 
+cgso = 1.5e-010 cgdo = 1.5e-010 cgbo = 2.56e-011 cgdl = 2.653e-10
+cgsl = 2.653e-10 ckappas = 0.03 ckappad = 0.03 acde = 1
+moin = 15 noff = 0.9 voffcv = 0.02
+kt1 = -0.11 kt1l = 0 kt2 = 0.022 ute = -1.5 
+ua1 = 4.31e-009 ub1 = 7.61e-018 uc1 = -5.6e-011 prt = 0 
+at = 33000 
+fnoimod = 1 tnoimod = 0 
+jss = 0.0001 jsws = 1e-011 jswgs = 1e-010 njs = 1 
+ijthsfwd= 0.01 ijthsrev= 0.001 bvs = 10 xjbvs = 1 
+jsd = 0.0001 jswd = 1e-011 jswgd = 1e-010 njd = 1 
+ijthdfwd= 0.01 ijthdrev= 0.001 bvd = 10 xjbvd = 1 
+pbs = 1 cjs = 0.0005 mjs = 0.5 pbsws = 1 
+cjsws = 5e-010 mjsws = 0.33 pbswgs = 1 cjswgs = 3e-010 
+mjswgs = 0.33 pbd = 1 cjd = 0.0005 mjd = 0.5 
+pbswd = 1 cjswd = 5e-010 mjswd = 0.33 pbswgd = 1 
+cjswgd = 5e-010 mjswgd = 0.33 tpb = 0.005 tcj = 0.001 
+tpbsw = 0.005 tcjsw = 0.001 tpbswg = 0.005 tcjswg = 0.001 
+xtis = 3 xtid = 3 
+dmcg = 0e-006 dmci = 0e-006 dmdg = 0e-006 dmcgt = 0e-007 
+dwj = 0.0e-008 xgw = 0e-007 xgl = 0e-008 
+rshg = 0.4 gbmin = 1e-010 rbpb = 5 rbpd = 15 
+rbps = 15 rbdb = 15 rbsb = 15 ngcon = 1 
"}
C {lab_pin.sym} 420 -550 0 0 {name=p17 lab=MINUS}
C {lab_pin.sym} 120 -880 0 0 {name=p4 lab=PLUS}
C {lab_pin.sym} 420 -320 0 0 {name=p18 lab=PLUS}
C {vsource.sym} 700 -120 0 0 {name=V1 value=2
savecurrent=true}
C {lab_pin.sym} 420 -260 0 0 {name=p20 lab=MINUS}
C {lab_pin.sym} 120 -610 0 0 {name=p7 lab=PLUS}
C {lab_pin.sym} 830 -310 0 0 {name=p22 lab=PLUS}
C {lab_pin.sym} 700 -150 0 0 {name=p21 lab=VCC}
C {lab_pin.sym} 830 -250 0 0 {name=p24 lab=MINUS}
C {lab_pin.sym} 420 -820 0 0 {name=p3 lab=MINUS}
C {lab_pin.sym} 120 -320 0 0 {name=p34 lab=PLUS}
C {lab_pin.sym} 120 -550 0 0 {name=p9 lab=MINUS}
C {lab_pin.sym} 120 -260 0 0 {name=p36 lab=MINUS}
