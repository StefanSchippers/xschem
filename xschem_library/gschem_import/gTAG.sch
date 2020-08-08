v {xschem version=2.9.7 file_version=1.1}
G {}
V {}
S {}
E {}
T {JTAG (IEEE1149.1) consist of tdi, tdo, tck, tms, (trst)} 5110 -5740 2 1 0.333333 0.333333 {}
T {5} 6220 -5160 2 1 0.333333 0.333333 {}
T {1} 6070 -5160 2 1 0.333333 0.333333 {}
T {gTAG - usb to jtag interface} 4850 -6520 2 1 1.66667 1.66667 {}
T {gTAG - top level} 6150 -5240 2 1 0.666667 0.666667 {}
T {gTAG.sch} 6070 -5190 2 1 0.333333 0.333333 {}
T {Stefan Petersen (spe@stacken.kth.se)} 6450 -5160 2 1 0.333333 0.333333 {}
T {$Revision$} 6450 -5190 2 1 0.333333 0.333333 {}






C {title-A2.sym} 4430 -5150 0 0 {}
C {gTAG-ucont.sym} 5410 -5800 0 0 {name=S1
source=gTAG-ucont.sch
device=none
}
C {gTAG-jtagio.sym} 4990 -5800 0 0 {name=S2
source=gTAG-jtagio.sch
device=none
}
C {gTAG-consio.sym} 5860 -5800 0 0 {name=S3
source=gTAG-consio.sch
device=none
}
C {gTAG-psu.sym} 5390 -6170 0 0 {name=S4
source=gTAG-psu.sch
device=none
}
C {copyleft.sym} 4460 -5170 0 0 {name=S4
device=none
}

N 5280 -5980 5410 -5980 {lab=tdi_ttl }
C {lab_wire.sym} 5345 -5980 0 0 {lab=tdi_ttl }
N 5280 -5940 5410 -5940 {lab=tdo_ttl }
C {lab_wire.sym} 5345 -5940 0 0 {lab=tdo_ttl }
N 5280 -5900 5410 -5900 {lab=tms_ttl }
C {lab_wire.sym} 5345 -5900 0 0 {lab=tms_ttl }
N 5280 -5860 5410 -5860 {lab=tck_ttl }
C {lab_wire.sym} 5345 -5860 0 0 {lab=tck_ttl }
N 5280 -5820 5410 -5820 {lab=trst_ttl }
C {lab_wire.sym} 5345 -5820 0 0 {lab=trst_ttl }
N 5390 -6230 5120 -6230 {}
N 5120 -6230 5120 -6030 {lab=jtag_power }
C {lab_wire.sym} 5120 -6130 0 0 {lab=jtag_power }
N 5710 -6230 6010 -6230 {}
N 6010 -6230 6010 -6030 {lab=usb_power }
C {lab_wire.sym} 6010 -6130 0 0 {lab=usb_power }
N 5730 -5910 5860 -5910 {lab=usb_io+ }
C {lab_wire.sym} 5795 -5910 0 0 {lab=usb_io+ }
N 5730 -5880 5860 -5880 {lab=usb_io- }
C {lab_wire.sym} 5795 -5880 0 0 {lab=usb_io- }
N 5730 -5980 5740 -5980 {}
N 5740 -5980 5740 -6190 {lab=pon_reset# }
C {lab_wire.sym} 5740 -6085 0 0 {lab=pon_reset# }
N 5740 -6190 5710 -6190 {}

