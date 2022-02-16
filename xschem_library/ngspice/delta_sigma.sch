v {xschem version=3.0.0 file_version=1.2 }
G {}
K {}
V {}
S {}
E {}
B 2 900 -620 1700 -330 {flags=graph
y1=0.294023
y2=1.25402
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=-8.91647e-08
x2=8.98315e-06
divx=5
subdivx=1
node="sig_in
vref
vcc"
color="8 9 11"
dataset=0
unitx=u
}
B 2 900 -330 1700 -130 {flags=graph
y1=0
y2=1.2
ypos1=0.0499032
ypos2=0.633885
divy=5
subdivy=1
unity=1
x1=-8.91647e-08
x2=8.98315e-06
divx=5
subdivx=1
node="CODE,code[5],code[4],code[3],code[2],code[1],code[0]
ck
rst
x1.comp"
color="7 4 10 4"
dataset=0
unitx=u
digital=1}
B 2 900 -890 1700 -630 {flags=graph
y1=17
y2=51
ypos1=0
ypos2=2
divy=5
subdivy=1
unity=1
x1=-8.91647e-08
x2=8.98315e-06
divx=5
subdivx=1
node="\\"sig_in 1.2 / 63 *\\""
color=8
dataset=0
unitx=u
}
T {Value of this signal
is equal to adc conversion
CODE[5:0]} 602.5 -792.5 0 0 0.4 0.4 {layer=8}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {lab_pin.sym} 270 -580 0 0 {name=p33 lab=VREF}
C {vsource.sym} 270 -550 0 0 {name=v2 value="'VCC/2'"}
C {lab_pin.sym} 270 -520 0 0 {name=p34 lab=0}
C {code_shown.sym} 10 -310 0 0 {name=CONTROL 
tclcommand="xschem edit_vi_prop"
place=end
value=".model switch sw vt='VCC/2' vh=0.2 ron=1000 roff=1G
.param VCC=1.2
.option method=GEAR
.measure tran avg1 AVG v(x1.qn) from=1u to=3u 
.measure tran avg2 AVG v(x1.qn) from=4u to=6u
.measure tran avg3 AVG v(x1.qn) from=7u to=9u
.control
  * save all
  tran 0.6n 9u uic
  write delta_sigma.raw
.endc
"}
C {vsource.sym} 270 -450 0 1 {name=v3 
value="pwl
+ 0.001u 0.34 3u 0.34 
+ 3.001u 0.88 6u 0.88
+ 6.001u 0.97 9u 0.97"}
C {lab_pin.sym} 270 -420 0 0 {name=p1 lab=0}
C {lab_pin.sym} 270 -480 0 0 {name=p2 lab=SIG_IN}
C {vsource.sym} 520 -460 0 0 {name=v1 
value="pulse 0 VCC 100n 100p 100p 9.9n 20n" 
xvalue="sin 0.2 1.8 1u 0"
}
C {lab_pin.sym} 520 -430 0 0 {name=p6 lab=0}
C {lab_pin.sym} 520 -490 0 0 {name=p7 lab=CK}
C {vsource.sym} 390 -550 0 0 {name=v5 value=VCC}
C {lab_pin.sym} 390 -520 0 0 {name=p17 lab=0}
C {lab_pin.sym} 390 -480 0 0 {name=p18 lab=VSS}
C {vsource.sym} 390 -450 0 0 {name=v6 value=0}
C {lab_pin.sym} 390 -420 0 0 {name=p19 lab=0}
C {lab_pin.sym} 520 -590 0 0 {name=p55 lab=RST}
C {vsource.sym} 520 -560 0 0 {name=v7 value="pwl 0 VCC 
+ 1u VCC 1.001u 0 3u 0 3.001u VCC
+ 4u VCC 4.001u 0 6u 0 6.001u VCC
+ 7u VCC 7.001u 0 9u 0 9.001u VCC"

}
C {lab_pin.sym} 520 -530 0 0 {name=p56 lab=0}
C {vdd.sym} 390 -580 0 0 {name=l2 lab=VCC}
C {adc.sym} 620 -190 0 0 {name=x1}
C {lab_pin.sym} 700 -220 0 1 {name=p38 lab=CODE[5:0]}
C {lab_pin.sym} 540 -220 0 0 {name=p39 lab=SIG_IN}
C {lab_pin.sym} 540 -200 0 0 {name=p40 lab=VREF}
C {lab_pin.sym} 540 -180 0 0 {name=p41 lab=CK}
C {lab_pin.sym} 540 -160 0 0 {name=p42 lab=RST}
C {code.sym} 0 -580 0 0 {name=MODELS_65nm only_toplevel=false value="* Beta Version released on 2/22/06

* PTM 65nm NMOS 
 
.model  nmos  nmos  level = 54
+version = 4.0          binunit = 1            paramchk= 1            mobmod  = 0          
+capmod  = 2            igcmod  = 1            igbmod  = 1            geomod  = 1          
+diomod  = 1            rdsmod  = 0            rbodymod= 1            rgatemod= 1          
+permod  = 1            acnqsmod= 0            trnqsmod= 0          
+tnom    = 27           toxe    = 1.85e-9      toxp    = 1.2e-9       toxm    = 1.85e-9   
+dtox    = 0.65e-9      epsrox  = 3.9          wint    = 5e-009       lint    = 5.25e-009   
+ll      = 0            wl      = 0            lln     = 1            wln     = 1          
+lw      = 0            ww      = 0            lwn     = 1            wwn     = 1          
+lwl     = 0            wwl     = 0            xpart   = 0            toxref  = 1.85e-9   
+xl      = -30e-9
+vth0    = 0.423        k1      = 0.4          k2      = 0.01         k3      = 0          
+k3b     = 0            w0      = 2.5e-006     dvt0    = 1            dvt1    = 2       
+dvt2    = -0.032       dvt0w   = 0            dvt1w   = 0            dvt2w   = 0          
+dsub    = 0.1          minv    = 0.05         voffl   = 0            dvtp0   = 1.0e-009     
+dvtp1   = 0.1          lpe0    = 0            lpeb    = 0            xj      = 1.96e-008   
+ngate   = 2e+020       ndep    = 2.54e+018    nsd     = 2e+020       phin    = 0          
+cdsc    = 0.000        cdscb   = 0            cdscd   = 0            cit     = 0          
+voff    = -0.13        nfactor = 1.9          eta0    = 0.0058       etab    = 0          
+vfb     = -0.55        u0      = 0.0491       ua      = 6e-010       ub      = 1.2e-018     
+uc      = 0            vsat    = 124340       a0      = 1.0          ags     = 1e-020     
+a1      = 0            a2      = 1.0          b0      = 0            b1      = 0          
+keta    = 0.04         dwg     = 0            dwb     = 0            pclm    = 0.04       
+pdiblc1 = 0.001        pdiblc2 = 0.001        pdiblcb = -0.005       drout   = 0.5        
+pvag    = 1e-020       delta   = 0.01         pscbe1  = 8.14e+008    pscbe2  = 1e-007     
+fprout  = 0.2          pdits   = 0.08         pditsd  = 0.23         pditsl  = 2.3e+006   
+rsh     = 5            rdsw    = 165          rsw     = 85           rdw     = 85        
+rdswmin = 0            rdwmin  = 0            rswmin  = 0            prwg    = 0          
+prwb    = 6.8e-011     wr      = 1            alpha0  = 0.074        alpha1  = 0.005      
+beta0   = 30           agidl   = 0.0002       bgidl   = 2.1e+009     cgidl   = 0.0002     
+egidl   = 0.8          
+aigbacc = 0.012        bigbacc = 0.0028       cigbacc = 0.002     
+nigbacc = 1            aigbinv = 0.014        bigbinv = 0.004        cigbinv = 0.004      
+eigbinv = 1.1          nigbinv = 3            aigc    = 0.012        bigc    = 0.0028     
+cigc    = 0.002        aigsd   = 0.012        bigsd   = 0.0028       cigsd   = 0.002     
+nigc    = 1            poxedge = 1            pigcd   = 1            ntox    = 1          
+xrcrg1  = 12           xrcrg2  = 5          
+cgso    = 1.5e-010     cgdo    = 1.5e-010     cgbo    = 2.56e-011    cgdl    = 2.653e-10     
+cgsl    = 2.653e-10    ckappas = 0.03         ckappad = 0.03         acde    = 1          
+moin    = 15           noff    = 0.9          voffcv  = 0.02       
+kt1     = -0.11        kt1l    = 0            kt2     = 0.022        ute     = -1.5       
+ua1     = 4.31e-009    ub1     = 7.61e-018    uc1     = -5.6e-011    prt     = 0          
+at      = 33000      
+fnoimod = 1            tnoimod = 0          
+jss     = 0.0001       jsws    = 1e-011       jswgs   = 1e-010       njs     = 1          
+ijthsfwd= 0.01         ijthsrev= 0.001        bvs     = 10           xjbvs   = 1          
+jsd     = 0.0001       jswd    = 1e-011       jswgd   = 1e-010       njd     = 1          
+ijthdfwd= 0.01         ijthdrev= 0.001        bvd     = 10           xjbvd   = 1          
+pbs     = 1            cjs     = 0.0005       mjs     = 0.5          pbsws   = 1          
+cjsws   = 5e-010       mjsws   = 0.33         pbswgs  = 1            cjswgs  = 3e-010     
+mjswgs  = 0.33         pbd     = 1            cjd     = 0.0005       mjd     = 0.5        
+pbswd   = 1            cjswd   = 5e-010       mjswd   = 0.33         pbswgd  = 1          
+cjswgd  = 5e-010       mjswgd  = 0.33         tpb     = 0.005        tcj     = 0.001      
+tpbsw   = 0.005        tcjsw   = 0.001        tpbswg  = 0.005        tcjswg  = 0.001      
+xtis    = 3            xtid    = 3          
+dmcg    = 0e-006       dmci    = 0e-006       dmdg    = 0e-006       dmcgt   = 0e-007     
+dwj     = 0.0e-008     xgw     = 0e-007       xgl     = 0e-008     
+rshg    = 0.4          gbmin   = 1e-010       rbpb    = 5            rbpd    = 15         
+rbps    = 15           rbdb    = 15           rbsb    = 15           ngcon   = 1          

* PTM 65nm PMOS
 
.model  pmos  pmos  level = 54
+version = 4.0          binunit = 1            paramchk= 1            mobmod  = 0          
+capmod  = 2            igcmod  = 1            igbmod  = 1            geomod  = 1          
+diomod  = 1            rdsmod  = 0            rbodymod= 1            rgatemod= 1          
+permod  = 1            acnqsmod= 0            trnqsmod= 0          
+tnom    = 27           toxe    = 1.95e-009    toxp    = 1.2e-009     toxm    = 1.95e-009   
+dtox    = 0.75e-9      epsrox  = 3.9          wint    = 5e-009       lint    = 5.25e-009   
+ll      = 0            wl      = 0            lln     = 1            wln     = 1          
+lw      = 0            ww      = 0            lwn     = 1            wwn     = 1          
+lwl     = 0            wwl     = 0            xpart   = 0            toxref  = 1.95e-009   
+xl      = -30e-9
+vth0    = -0.365       k1      = 0.4          k2      = -0.01        k3      = 0          
+k3b     = 0            w0      = 2.5e-006     dvt0    = 1            dvt1    = 2       
+dvt2    = -0.032       dvt0w   = 0            dvt1w   = 0            dvt2w   = 0          
+dsub    = 0.1          minv    = 0.05         voffl   = 0            dvtp0   = 1e-009     
+dvtp1   = 0.05         lpe0    = 0            lpeb    = 0            xj      = 1.96e-008   
+ngate   = 2e+020       ndep    = 1.87e+018    nsd     = 2e+020       phin    = 0          
+cdsc    = 0.000        cdscb   = 0            cdscd   = 0            cit     = 0          
+voff    = -0.126       nfactor = 1.9          eta0    = 0.0058       etab    = 0          
+vfb     = 0.55         u0      = 0.00574      ua      = 2.0e-009     ub      = 0.5e-018     
+uc      = 0            vsat    = 70000        a0      = 1.0          ags     = 1e-020     
+a1      = 0            a2      = 1            b0      = -1e-020      b1      = 0          
+keta    = -0.047       dwg     = 0            dwb     = 0            pclm    = 0.12       
+pdiblc1 = 0.001        pdiblc2 = 0.001        pdiblcb = 3.4e-008     drout   = 0.56       
+pvag    = 1e-020       delta   = 0.01         pscbe1  = 8.14e+008    pscbe2  = 9.58e-007  
+fprout  = 0.2          pdits   = 0.08         pditsd  = 0.23         pditsl  = 2.3e+006   
+rsh     = 5            rdsw    = 165          rsw     = 85           rdw     = 85        
+rdswmin = 0            rdwmin  = 0            rswmin  = 0            prwg    = 3.22e-008  
+prwb    = 6.8e-011     wr      = 1            alpha0  = 0.074        alpha1  = 0.005      
+beta0   = 30           agidl   = 0.0002       bgidl   = 2.1e+009     cgidl   = 0.0002     
+egidl   = 0.8          
+aigbacc = 0.012        bigbacc = 0.0028       cigbacc = 0.002     
+nigbacc = 1            aigbinv = 0.014        bigbinv = 0.004        cigbinv = 0.004      
+eigbinv = 1.1          nigbinv = 3            aigc    = 0.69         bigc    = 0.0012     
+cigc    = 0.0008       aigsd   = 0.0087       bigsd   = 0.0012       cigsd   = 0.0008     
+nigc    = 1            poxedge = 1            pigcd   = 1            ntox    = 1 
+xrcrg1  = 12           xrcrg2  = 5          
+cgso    = 1.5e-010     cgdo    = 1.5e-010     cgbo    = 2.56e-011    cgdl    = 2.653e-10
+cgsl    = 2.653e-10    ckappas = 0.03         ckappad = 0.03         acde    = 1
+moin    = 15           noff    = 0.9          voffcv  = 0.02
+kt1     = -0.11        kt1l    = 0            kt2     = 0.022        ute     = -1.5       
+ua1     = 4.31e-009    ub1     = 7.61e-018    uc1     = -5.6e-011    prt     = 0          
+at      = 33000      
+fnoimod = 1            tnoimod = 0          
+jss     = 0.0001       jsws    = 1e-011       jswgs   = 1e-010       njs     = 1          
+ijthsfwd= 0.01         ijthsrev= 0.001        bvs     = 10           xjbvs   = 1          
+jsd     = 0.0001       jswd    = 1e-011       jswgd   = 1e-010       njd     = 1          
+ijthdfwd= 0.01         ijthdrev= 0.001        bvd     = 10           xjbvd   = 1          
+pbs     = 1            cjs     = 0.0005       mjs     = 0.5          pbsws   = 1          
+cjsws   = 5e-010       mjsws   = 0.33         pbswgs  = 1            cjswgs  = 3e-010     
+mjswgs  = 0.33         pbd     = 1            cjd     = 0.0005       mjd     = 0.5        
+pbswd   = 1            cjswd   = 5e-010       mjswd   = 0.33         pbswgd  = 1          
+cjswgd  = 5e-010       mjswgd  = 0.33         tpb     = 0.005        tcj     = 0.001      
+tpbsw   = 0.005        tcjsw   = 0.001        tpbswg  = 0.005        tcjswg  = 0.001      
+xtis    = 3            xtid    = 3          
+dmcg    = 0e-006       dmci    = 0e-006       dmdg    = 0e-006       dmcgt   = 0e-007     
+dwj     = 0.0e-008     xgw     = 0e-007       xgl     = 0e-008     
+rshg    = 0.4          gbmin   = 1e-010       rbpb    = 5            rbpd    = 15         
+rbps    = 15           rbdb    = 15           rbsb    = 15           ngcon   = 1          
"}
C {spice_probe.sym} 270 -580 0 0 {name=p3 attrs=""}
C {spice_probe.sym} 390 -580 0 0 {name=p4 attrs=""}
C {spice_probe.sym} 270 -480 0 0 {name=p5 attrs=""}
C {spice_probe.sym} 520 -490 0 0 {name=p8 attrs=""}
C {spice_probe.sym} 520 -590 0 0 {name=p9 attrs=""}
C {spice_probe.sym} 700 -220 0 0 {name=p10 attrs=""}
C {launcher.sym} 1270 -90 0 0 {name=h5 
descr="Select arrow and 
Ctrl-Left-Click to load/unload waveforms" 
tclcommand="
xschem raw_read $netlist_dir/[file tail [file rootname [xschem get current_name]]].raw
"
}
C {launcher.sym} 870 -90 0 0 {name=h1 
descr="Netlist + Simulate
Ctrl-Left-Click" 
tclcommand="xschem netlist; xschem simulate"
}
