v {xschem version=3.4.8RC file_version=1.3}
G {}
K {}
V {}
S {}
F {}
E {}
B 2 600 -390 1250 -100 {flags=graph
y1=0
y2=5.1
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=8.2718061e-25
x2=1e-07
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
node="a
z1
z2"
color="4 7 9"
dataset=-1
unitx=1
logx=0
logy=0
hilight_wave=-1}
T {4 stage buffer where all transistor geometries are calculated  as a function of input parameters:
Wn_1x, mos_ratio, stage_ratio, nl, pl
All equations are resolved by xschem and netlist contains only calculated numbers.} 20 -780 0 0 0.5 0.5 {layer=7}
T {each instance specifies a unique schematic=.. attribute
this is needed since this subcircuit is not parametric.
Parameter equations are resolved by xschem.} 190 -600 0 0 0.3 0.3 {layer=7}
T {each instance specifies a unique schematic=.. attribute
this is needed since this subcircuit is not parametric.
Parameter equations are resolved by xschem.} 190 -320 0 0 0.3 0.3 {layer=7}
N 100 -470 160 -470 {lab=A}
N 360 -470 430 -470 {lab=Z1}
N 40 -160 40 -120 {lab=VCC}
N 40 -70 40 -60 {lab=0}
N 100 -470 100 -420 {lab=A}
N 370 -470 370 -420 {lab=Z1}
N 360 -190 430 -190 {lab=Z2}
N 370 -190 370 -140 {lab=Z2}
N 100 -190 160 -190 {lab=A}
C {buffer.sym} 260 -470 0 0 {name=x1 Wn_1x=1.0 mos_ratio=3 stage_ratio=2 nl=1 pl=1.2
schematic=buffer1}
C {lab_pin.sym} 100 -190 0 0 {name=p5 sig_type=std_logic lab=A}
C {vsource.sym} 40 -90 0 0 {name=V1 value=5 savecurrent=false}
C {gnd.sym} 40 -60 0 0 {name=l2 lab=0}
C {lab_pin.sym} 40 -160 0 0 {name=p1 sig_type=std_logic lab=VCC}
C {lab_pin.sym} 100 -470 0 0 {name=p2 sig_type=std_logic lab=A}
C {lab_pin.sym} 430 -470 0 1 {name=p3 sig_type=std_logic lab=Z1}
C {vsource.sym} 100 -390 0 0 {name=V2 value="pulse 0 5 1n 0.1n 0.1n 7.9n 16n" savecurrent=false}
C {gnd.sym} 100 -360 0 0 {name=l1 lab=0}
C {code_shown.sym} 950 -620 0 0 {name=COMMANDS only_toplevel=false
value="
.options SCALE=1e-6
.control
  save all
  tran 0.05n 100n
  remzerovec
  write tb_buffer.raw
.endc
"}
C {launcher.sym} 640 -60 0 0 {name=h5
descr="load waves"
tclcommand="xschem raw_read $netlist_dir/tb_buffer.raw tran"
}
C {capa.sym} 370 -390 0 0 {name=C1
m=1
value=1p
footprint=1206
device="ceramic capacitor"}
C {gnd.sym} 370 -360 0 0 {name=l3 lab=0}
C {buffer.sym} 260 -190 0 0 {name=x2 Wn_1x=2.0 mos_ratio=2 stage_ratio=3 nl=0.8 pl=0.9
schematic=buffer2}
C {lab_pin.sym} 430 -190 0 1 {name=p4 sig_type=std_logic lab=Z2}
C {capa.sym} 370 -110 0 0 {name=C2
m=1
value=1p
footprint=1206
device="ceramic capacitor"}
C {gnd.sym} 370 -80 0 0 {name=l4 lab=0}
C {code.sym} 670 -600 0 0 {name="MODELS"
spice_ignore=0 place=end
only_toplevel=false value="
** From the ngspice distribution:
** https://sourceforge.net/p/ngspice/ngspice/ci/master/tree/examples/mos/modelcard.nmos
** https://sourceforge.net/p/ngspice/ngspice/ci/master/tree/examples/mos/modelcard.pmos

.model cmosn NMOS
+Level=        49 version=3.3.0
+Tnom=27.0
+Nch= 2.498E+17  Tox=9E-09 Xj=1.00000E-07
+Lint=9.36e-8 Wint=1.47e-7
+Vth0= .6322    K1= .756  K2= -3.83e-2  K3= -2.612
+Dvt0= 2.812  Dvt1= 0.462  Dvt2=-9.17e-2
+Nlx= 3.52291E-08  W0= 1.163e-6
+K3b= 2.233
+Vsat= 86301.58  Ua= 6.47e-9  Ub= 4.23e-18  Uc=-4.706281E-11
+Rdsw= 650  U0= 388.3203 wr=1
+A0= .3496967 Ags=.1    B0=0.546    B1= 1
+Dwg = -6.0E-09 Dwb = -3.56E-09 Prwb = -.213
+Keta=-3.605872E-02  A1= 2.778747E-02  A2= .9
+Voff=-6.735529E-02  NFactor= 1.139926  Cit= 1.622527E-04
+Cdsc=-2.147181E-05
+Cdscb= 0  Dvt0w =  0 Dvt1w =  0 Dvt2w =  0
+Cdscd =  0 Prwg =  0
+Eta0= 1.0281729E-02  Etab=-5.042203E-03
+Dsub= .31871233
+Pclm= 1.114846  Pdiblc1= 2.45357E-03  Pdiblc2= 6.406289E-03
+Drout= .31871233  Pscbe1= 5000000  Pscbe2= 5E-09 Pdiblcb = -.234
+Pvag= 0 delta=0.01
+Wl =  0 Ww = -1.420242E-09 Wwl =  0
+Wln =  0 Wwn =  .2613948 Ll =  1.300902E-10
+Lw =  0 Lwl =  0 Lln =  .316394
+Lwn =  0
+kt1=-.3  kt2=-.051
+At= 22400
+Ute=-1.48
+Ua1= 3.31E-10  Ub1= 2.61E-19 Uc1= -3.42e-10
+Kt1l=0 Prt=764.3
+vgs_max=4 vds_max=4 vbs_max=4

.model cmosp PMOS
+Level=        49 version=3.3.0
+Tnom=27.0
+Nch= 3.533024E+17  Tox=9E-09 Xj=1.00000E-07
+Lint=6.23e-8 Wint=1.22e-7
+Vth0=-.6732829 K1= .8362093  K2=-8.606622E-02  K3= 1.82
+Dvt0= 1.903801  Dvt1= .5333922  Dvt2=-.1862677
+Nlx= 1.28e-8  W0= 2.1e-6
+K3b= -0.24 Prwg=-0.001 Prwb=-0.323
+Vsat= 103503.2  Ua= 1.39995E-09  Ub= 1.e-19  Uc=-2.73e-11
+Rdsw= 460  U0= 138.7609
+A0= .4716551 Ags=0.12
+Keta=-1.871516E-03  A1= .3417965  A2= 0.83
+Voff=-.074182  NFactor= 1.54389  Cit=-1.015667E-03
+Cdsc= 8.937517E-04
+Cdscb= 1.45e-4  Cdscd=1.04e-4
+Dvt0w=0.232 Dvt1w=4.5e6 Dvt2w=-0.0023
+Eta0= 6.024776E-02  Etab=-4.64593E-03
+Dsub= .23222404
+Pclm= .989  Pdiblc1= 2.07418E-02  Pdiblc2= 1.33813E-3
+Drout= .3222404  Pscbe1= 118000  Pscbe2= 1E-09
+Pvag= 0
+kt1= -0.25  kt2= -0.032 prt=64.5
+At= 33000
+Ute= -1.5
+Ua1= 4.312e-9 Ub1= 6.65e-19  Uc1= 0
+Kt1l=0
+vgs_max=4 vds_max=4 vbs_max=4
"}
C {title.sym} 160 -30 0 0 {name=l5 author="Stefan Schippers"}
