v {xschem version=2.9.5 file_version=1.1}
G {}
V {}
S {}
E {}
N 520 -440 520 -330 {lab=VCC}
N 620 -440 620 -380 {lab=VCC}
N 520 -200 520 -190 {lab=OUT}
N 420 -340 420 -300 {lab=B}
N 420 -300 480 -300 {lab=B}
N 520 -440 620 -440 {lab=VCC}
N 420 -440 420 -400 {lab=C}
N 250 -440 420 -440 {lab=C}
N 420 -300 420 -270 {lab=B}
N 520 -210 610 -210 {lab=OUT}
N 500 -440 520 -440 {lab=VCC}
N 420 -440 440 -440 {lab=C}
N 520 -270 520 -210 {lab=OUT}
N 420 -200 420 -190 {lab=OUT}
N 420 -200 520 -200 {lab=OUT}
N 520 -210 520 -200 {lab=OUT}
N 420 -210 420 -200 {lab=OUT}
C {ind.sym} 420 -370 0 0 {name=l0 value=\{l0\}}
C {capa.sym} 250 -410 0 0 {name=C3 m=1 value=1nF footprint=1206 device="ceramic capacitor"}
C {res.sym} 470 -440 1 0 {name=Rb0 value=100k footprint=1206 device=resistor m=1}
C {opin.sym} 610 -210 0 0 {name=p1 lab=OUT}
C {vsource.sym} 620 -350 0 0 {name=v0 value="pwl 0 0 1u 15"}
C {gnd.sym} 620 -320 0 0 {name=l2 lab=0}
C {res.sym} 520 -160 0 0 {name=Re value=1k footprint=1206 device=resistor m=1}
C {gnd.sym} 520 -130 0 0 {name=l3 lab=0}
C {res.sym} 340 -410 0 0 {name=Rb1 value=100k footprint=1206 device=resistor m=1}
C {gnd.sym} 340 -380 0 0 {name=l4 lab=0}
C {gnd.sym} 250 -380 0 0 {name=l5 lab=0}
C {capa.sym} 420 -160 0 1 {name=C2 m=1 value=\{cr\} footprint=1206 device="ceramic capacitor"}
C {capa.sym} 420 -240 0 1 {name=C1 m=1 value=\{cr\} footprint=1206 device="ceramic capacitor"}
C {gnd.sym} 420 -130 0 0 {name=l6 lab=0}
C {title.sym} 160 -30 0 0 {name=l7 author="Stefan Schippers"}
C {lab_pin.sym} 620 -440 0 1 {name=l8 sig_type=std_logic lab=VCC}
C {code_shown.sym} 50 -210 0 0 {name=CONTROL place=end value=".option savecurrents
.params l0=20u cr=89p 
.options method=gear reltol=1m
.tran 1ns 20us 10us 1ns
"}
C {code.sym} 740 -200 0 0 {name=MODELS value=".MODEL Q2N5179  NPN (IS=1.55467e-17 BF=296.182 NF=0.850014 VAF=10
+IKF=0.00544635 ISE=2.01913e-14 NE=1.54276 BR=19.550
+NR=0.825166 VAR=73.1109 IKR=0.0544635 ISC=1e-160
+NC=2.9688 RB=21.0221 IRB=0.478136 RBM=0.1384250
+RE=0.000646335 RC=3.0552 XTB=0.582018 XTI=1
+EG=1.06135 CJE=1.08982e-12 VJE=0.99 MJE=0.230
+TF=2.15066e-11 XTF=1000 VTF=1.33967 ITF=0.0010
+CJC=1.89423e-12 VJC=0.95 MJC=0.23 XCJC=0.4084790
+FC=0.1 CJS=0 VJS=0.75 MJS=0.50
+TR=1e-07 PTF=0 KF=0 AF=1)
"}
C {lab_pin.sym} 420 -320 0 0 {name=l9 sig_type=std_logic lab=B}
C {lab_wire.sym} 390 -440 0 0 {name=l10 sig_type=std_logic lab=C}
C {npn.sym} 500 -300 0 0 {name=Q1 model=Q2N5179 device=Q2N5179 footprint=SOT23 area=1}
