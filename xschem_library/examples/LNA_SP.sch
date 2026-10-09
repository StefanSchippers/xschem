v {xschem version=3.4.8RC file_version=1.3}
G {}
K {}
V {}
S {}
F {}
E {}
B 2 810 -940 1070 -700 {flags=graph
y1=17
y2=21
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=82625879
x2=1.2194748e+08
divx=5
subdivx=4
xlabmag=1.0
ylabmag=1.0
legendmag=1.0
dataset=-1
unitx=1
logx=0
logy=0
hilight_wave=0
rawfile=$netlist_dir/LNA_SP2_AC.raw
autoload=1
sim_type=ac
color=9
node="out in / db20()"}
B 2 810 -700 1070 -460 {flags=graph
y1=4.5
y2=6.8
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=50000000
x2=2e+08
divx=5
subdivx=4
xlabmag=1.0
ylabmag=1.0
legendmag=1.0
dataset=-1
unitx=1
logx=0
logy=0
hilight_wave=-1
rawfile=$netlist_dir/LNA_SP2_noise.raw
autoload=1
sim_type=noise
color=17
node=nf_db}
B 2 550 -940 810 -700 {flags=graph
y1=15
y2=21
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=-1.0227444
x2=1.0772556
divx=5
subdivx=4
xlabmag=1.0
ylabmag=1.0
legendmag=1.0
dataset=-1
unitx=1
logx=0
logy=0
hilight_wave=-1
autoload=1
sim_type=sp
rawfile=$netlist_dir/LNA_SP2.raw
mode=Smith
color="10 17"
node="s_1_1
s_2_2"
smz0=50}
B 2 550 -700 810 -460 {flags=graph
y1=15
y2=21
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=-1.0227444
x2=1.0772556
divx=5
subdivx=4
xlabmag=1.0
ylabmag=1.0
legendmag=1.0
dataset=-1
unitx=1
logx=0
logy=0
hilight_wave=-1
autoload=1
sim_type=sp
rawfile=$netlist_dir/LNA_SP2_AC.raw
mode=Smith
smz0=50
color=21
node="Zin(vx1); vx1 i(vmes) / -1 * imp()"}
T {@name} 190 -621.25 2 1 0.2 0.2 {name=Vmes}
T {@value} 180 -636.25 2 1 0.2 0.2 {name=Vmes}
T {@spice_get_current} 236.25 -630 1 1 0.2 0.2 {layer=17
name=Vmes}
N 290 -750 370 -750 {lab=vx2}
N 290 -490 290 -480 {lab=0}
N 410 -690 410 -640 {lab=VC}
N 350 -1050 350 -1040 {lab=#net1}
N 290 -750 290 -710 {lab=vx2}
N 290 -650 290 -610 {lab=vx1}
N 290 -610 290 -550 {lab=vx1}
N 290 -950 290 -930 {lab=vbat}
N 290 -1050 350 -1050 {lab=#net1}
N 410 -690 440 -690 {lab=VC}
N 410 -720 410 -690 {lab=VC}
N 220 -750 220 -740 {lab=vx2}
N 220 -750 290 -750 {lab=vx2}
N 220 -680 220 -670 {lab=0}
N 220 -950 220 -940 {lab=vbat}
N 220 -950 290 -950 {lab=vbat}
N 220 -880 220 -870 {lab=0}
N 290 -1050 290 -1030 {lab=#net1}
N 290 -970 290 -950 {lab=vbat}
N 260 -610 290 -610 {lab=vx1}
N 290 -870 290 -750 {lab=vx2}
N 410 -820 410 -780 {lab=ZC}
N 410 -430 410 -420 {lab=0}
N 410 -500 410 -490 {lab=#net2}
N 480 -500 480 -490 {lab=#net2}
N 410 -500 480 -500 {lab=#net2}
N 480 -430 480 -420 {lab=0}
N 90 -610 120 -610 {lab=in}
N 30 -540 30 -530 {lab=0}
N 410 -950 410 -930 {lab=vbat}
N 290 -950 410 -950 {lab=vbat}
N 410 -870 410 -820 {lab=ZC}
N 470 -950 470 -930 {lab=vbat}
N 410 -950 470 -950 {lab=vbat}
N 470 -870 470 -820 {lab=ZC}
N 410 -820 470 -820 {lab=ZC}
N 500 -870 500 -820 {lab=ZC}
N 500 -950 500 -930 {lab=vbat}
N 90 -610 90 -600 {lab=in}
N 90 -540 90 -530 {lab=0}
N 470 -820 500 -820 {lab=ZC}
N 500 -820 500 -800 {lab=ZC}
N 500 -740 500 -720 {lab=out}
N 500 -660 500 -650 {lab=0}
N 470 -950 500 -950 {lab=vbat}
N 290 -610 370 -610 {lab=vx1}
N 410 -580 410 -570 {lab=VE}
N 410 -510 410 -500 {lab=#net2}
N 30 -610 30 -600 {lab=in}
N 30 -610 90 -610 {lab=in}
N 180 -610 190 -610 {lab=#net3}
N 190 -520 190 -510 {lab=0}
N 190 -610 190 -580 {lab=#net3}
N 190 -610 200 -610 {lab=#net3}
C {simulator_commands.sym} 80 -790 0 0 {name=COMMANDS
simulator=ngspice
only_toplevel=true 
value=".temp 30
.option savecurrents
.save all
.control
  save all

  sp lin 1000 50meg 200meg

  let S11 = s_1_1
  let S12 = s_1_2
  let S21 = s_2_1
  let S22 = s_2_2

  let Rbase = 50
  wrs2p LNA_SP2.s2p

  remzerovec

  write LNA_SP2.raw

  *** noise figure over 50-200 MHz, 50-ohm source (V2 z0=50)
  noise V(out) V2 lin 1000 50meg 200meg
  setplot noise1

  *** per-frequency NF (NF reference T0 = 290 K, Rs = 50 ohm)
  let T0 = 290.0
  let Rsrc = 50.0
  let den = 4*boltz*T0*Rsrc
  let fvec = 1 + inoise_spectrum*inoise_spectrum/den
  let nf_db = 10*log10(fvec)
  save inoise_spectrum onoise_spectrum fvec nf_db
  write LNA_SP2_noise.raw all

  *** small-signal AC
  save all
  AC lin 1000 50Meg 200Meg
  remzerovec
  write LNA_SP2_AC.raw all
  quit 0
.endc

"}
C {code.sym} 80 -970 0 0 {name="MODELS"
spice_ignore=0
only_toplevel=false value=".model 2N3904   NPN(Is=6.734f Xti=3 Eg=1.11 Vaf=74.03 Bf=416.4 Ne=1.259
+               Ise=6.734f Ikf=66.78m Xtb=1.5 Br=.7371 Nc=2 Isc=0 Ikr=0 Rc=1
+               Cjc=3.638p Mjc=.3085 Vjc=.75 Fc=.5 Cje=4.493p Mje=.2593 Vje=.75
+               Tr=239.5n Tf=301.2p Itf=.4 Vtf=4 Xtf=2 Rb=10)

.model 2n3906   PNP(Is=455.9E-18 Xti=3 Eg=1.11 Vaf=33.6 Bf=204.7 Ise=7.558f
+               Ne=1.536 Ikf=.3287 Nk=.9957 Xtb=1.5 Var=100 Br=3.72
+               Isc=529.3E-18 Nc=15.51 Ikr=11.1 Rc=.8508 Cjc=10.13p Mjc=.6993
+               Vjc=1.006 Fc=.5 Cje=10.39p Mje=.6931 Vje=.9937 Tr=10n Tf=181.2p
+               Itf=4.881m Xtf=.7939 Vtf=10 Rb=10)

.model 1N4148   D(Is=5.84n N=1.94 Rs=.7017 Ikf=44.17m Xti=3 Eg=1.11 Cjo=.95p
+               M=.55 Vj=.75 Fc=.5 Isr=11.07n Nr=2.088 Bv=100 Ibv=100u Tt=11.07n)

.MODEL SS9014 NPN (IS=2.87599e-14 BF=377.5 BR=4.79 VAF=123 VAR=11.29   
+IKF=1.1841 IKR=0.275423 ISE=4.7863e-15 ISC=1.44544e-14 NE=1.5 NC=1.5  
+RB=200 RBM=10 RC=5 RE=0.56 IRB=1e-5 CJE=1.7205e-11 CJC=6.2956e-12     
+VJE=0.6905907 VJC=0.4164212 MJE=0.3193434 MJC=0.2559546 TF=5.89463e-10
+XTB=1.8881 XTI=3 EG=1.2415 FC=0.5 XCJC=0.451391 VCEO=45 ICRATING=0.1) 

.MODEL QBF199 NPN (
+ IS  = 4.031E-16
+ NF  = 0.9847
+ ISE = 9.187E-17
+ NE  = 1.24
+ BF  = 150
+ IKF = 0.0467
+ VAF = 35.8
+ BR  = 5
+ ISC = 1.00E-14
+ NC  = 1.5
+ RE  = 1
+ RC  = 10
+ CJC = 5.00E-13
+ CJE = 5.00E-12
+ TF  = 1.86E-10
+ TR  = 10E-9
+ ITF = 0.045
+ VTF = 3
+ XTF = 50
+ VJC = 0.8
+ MJC = 0.2
+ VJE = 0.6
+ MJE = 0.4
+ FC  = 0.5 )
"}
C {vsource.sym} 30 -570 0 1 {name=V2 value="dc 0 ac 1 0 portnum=1 z0=50" savecurrent=false}
C {gnd.sym} 30 -530 0 0 {name=l4 lab=0}
C {vsource.sym} 500 -690 0 1 {name=V3 value="dc 0 ac 0 portnum=2 z0=50" savecurrent=false}
C {gnd.sym} 500 -650 0 1 {name=l6 lab=0}
C {npn.sym} 390 -610 0 0 {name=Q3
model=2N3904
device=2N3904
footprint=SOT23
area=1
m=1}
C {res.sym} 290 -680 0 0 {name=R5
value=300
footprint=1206
device=resistor
m=1}
C {res.sym} 290 -520 0 0 {name=R6
value=360
footprint=1206
device=resistor
m=1}
C {npn.sym} 390 -750 0 0 {name=Q4
model=2N3904
device=QBF199
footprint=SOT23
area=1
m=1}
C {res.sym} 290 -900 0 0 {name=R7
value=360
footprint=1206
device=resistor
m=1}
C {vsource.sym} 350 -1010 0 0 {name=V4 value=5 savecurrent=false}
C {gnd.sym} 350 -980 0 0 {name=l14 lab=0}
C {lab_wire.sym} 440 -690 0 0 {name=p6 sig_type=std_logic lab=VC}
C {lab_wire.sym} 330 -950 0 0 {name=p8 sig_type=std_logic lab=vbat}
C {lab_wire.sym} 290 -610 0 0 {name=p9 sig_type=std_logic lab=vx1}
C {lab_wire.sym} 290 -750 0 0 {name=p10 sig_type=std_logic lab=vx2}
C {capa.sym} 220 -710 0 0 {name=C7
m=1
value=10n
footprint=1206
device="ceramic capacitor"}
C {gnd.sym} 220 -670 0 0 {name=l15 lab=0}
C {capa.sym} 220 -910 0 0 {name=C8
m=1
value=100n
footprint=1206
device="ceramic capacitor"}
C {gnd.sym} 220 -870 0 0 {name=l16 lab=0}
C {ind.sym} 290 -1000 2 0 {name=L17
m=1
value=10u
footprint=1206
device=inductor}
C {lab_wire.sym} 80 -610 0 0 {name=p11 sig_type=std_logic lab=in}
C {capa.sym} 150 -610 1 0 {name=C9
m=1
value=470p
footprint=1206
device="ceramic capacitor"}
C {lab_wire.sym} 410 -820 2 1 {name=p12 sig_type=std_logic lab=ZC}
C {capa.sym} 500 -770 0 0 {name=C11
m=1
value=13p
footprint=1206
device="ceramic capacitor"}
C {lab_wire.sym} 500 -730 2 1 {name=p13 sig_type=std_logic lab=out}
C {res.sym} 410 -460 0 0 {name=R9
value=120
footprint=1206
device=resistor
m=1}
C {capa.sym} 480 -460 0 0 {name=C12
m=1
value=100n
footprint=1206
device="ceramic capacitor"}
C {res.sym} 410 -900 0 1 {name=R2
value=330
footprint=1206
device=resistor
m=1}
C {ind.sym} 90 -570 2 0 {name=L3
m=1
value=47n
footprint=1206
device=inductor}
C {gnd.sym} 90 -530 0 0 {name=l5 lab=0}
C {gnd.sym} 480 -420 0 1 {name=l7 lab=0}
C {gnd.sym} 410 -420 0 1 {name=l8 lab=0}
C {gnd.sym} 290 -480 0 1 {name=l9 lab=0}
C {ind.sym} 500 -900 2 1 {name=L1
m=1
value=94.1n
footprint=1206
device=inductor}
C {capa.sym} 470 -900 0 1 {name=C5
m=1
value=13.78p
footprint=1206
device="ceramic capacitor"}
C {res.sym} 410 -540 0 0 {name=R1
value=1
footprint=1206
device=resistor
m=1}
C {lab_wire.sym} 410 -570 0 1 {name=p1 sig_type=std_logic lab=VE}
C {res.sym} 190 -550 0 0 {name=R3
value=1g
footprint=1206
device=resistor
m=1}
C {gnd.sym} 190 -510 0 1 {name=l2 lab=0}
C {vsource.sym} 230 -610 1 1 {name=Vmes value="dc 0 ac 0 0" savecurrent=true
hide_texts=true
attach=Vmes}
