v {xschem version=3.4.8RC file_version=1.3}
G {}
K {}
V {}
S {}
F {}
E {}
B 2 150 -1070 630 -700 {flags=graph
y1=-0.11
y2=2
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0
x2=1.1e-07
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
legendmag=1.6
node="ina[0]
outa[0]"
color="4 10"
dataset=-1
unitx=1
logx=0
logy=0
}
B 2 650 -1070 1130 -700 {flags=graph
y1=-0.11
y2=2
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0
x2=1.1e-07
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
legendmag=1.6
node="inb[6]
outb[6]"
color="4 10"
dataset=-1
unitx=1
logx=0
logy=0
}
B 2 1150 -1070 1630 -700 {flags=graph
y1=-0.11
y2=2
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0
x2=1.1e-07
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
legendmag=1.6
node="ind[1]
outd[1]"
color="4 10"
dataset=-1
unitx=1
logx=0
logy=0
}
B 2 1650 -1070 2130 -700 {flags=graph
y1=-0.11
y2=2
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=0
x2=1.1e-07
divx=5
subdivx=1
xlabmag=1.0
ylabmag=1.0
legendmag=1.6
node="inc[3]
outc[3]"
color="4 10"
dataset=-1
unitx=1
logx=0
logy=0
}
T {netlists ok in Spice and Verilog} 660 -1170 0 0 0.8 0.8 {}
N 1240 -530 1270 -530 {lab=OUTB[7:0]}
N 740 -530 770 -530 {lab=OUTA[2:0]}
N 570 -530 600 -530 {lab=INA[2:0]}
N 1070 -530 1100 -530 {lab=INB[7:0]}
C {lab_pin.sym} 770 -530 0 1 {name=p1 lab=OUTA[2:0]}
C {lab_pin.sym} 570 -530 0 0 {name=p2 lab=INA[2:0]}
C {test_parametric_ports/inv.sym} 1160 -530 0 0 {name=x2 width=8
schematic=inv2}
C {lab_pin.sym} 1270 -530 0 1 {name=p3 lab=OUTB[7:0]}
C {lab_pin.sym} 1070 -530 0 0 {name=p4 lab=INB[7:0]}
C {test_parametric_ports/inv.sym} 660 -530 0 0 {name=x4 width=3
schematic=inv1}
C {test_parametric_ports/buf.sym} 1610 -530 0 0 {name=x1 width=4 schematic=buf1
inv=inv3}
C {lab_pin.sym} 1690 -530 0 1 {name=p7 lab=OUTD[3:0]}
C {lab_pin.sym} 1550 -530 0 0 {name=p8 lab=IND[3:0]}
C {lab_pin.sym} 1240 -270 0 1 {name=p5 lab=OUTC[4:0]
}
C {lab_pin.sym} 1100 -270 0 0 {name=p6 lab=INC[4:0]
}
C {test_parametric_ports/double_buf.sym} 1160 -270 0 0 {name=x3 width=5
schematic=double_buf1.sch
buf=buf3
inv=inv4}
C {ipin.sym} 600 -340 0 0 { name=p9 lab=IND[3:0] }
C {ipin.sym} 600 -360 0 0 { name=p10 lab=INC[4:0] }
C {ipin.sym} 600 -380 0 0 { name=p11 lab=INB[7:0] }
C {ipin.sym} 600 -400 0 0 { name=p12 lab=INA[2:0] }
C {opin.sym} 1690 -340 0 0 { name=p13 lab=OUTD[3:0] }
C {opin.sym} 1690 -360 0 0 { name=p14 lab=OUTC[4:0] }
C {opin.sym} 1690 -380 0 0 { name=p15 lab=OUTB[7:0] }
C {opin.sym} 1690 -400 0 0 { name=p16 lab=OUTA[2:0] }
C {code.sym} 130 -510 0 0 {name="MODELS"
spice_ignore=0
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
"
verilog_ignore=true}
C {code_shown.sym} 70 -300 0 0 {name=COMMANDS only_toplevel=false value="
.include stimuli_@schname\\\\.cir
.control
  tran 1n 110n
  remzerovec
  write @schname\\\\.raw
.endc
"
verilog_ignore=true}
C {launcher.sym} 150 -670 0 0 {name=h5
descr="load waves"
tclcommand="xschem raw_read $netlist_dir/tb_param_ports.raw tran"
}
