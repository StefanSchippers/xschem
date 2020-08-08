v {xschem version=2.9.5_RC6 file_version=1.1}
G {}
V {}
S {}
E {}
T {Copyright (C) 2008 DJ Delorie (dj delorie com)
Distributed under the terms of the GNU General Public License,
either verion 2 or (at your choice) any later version.} 40 -150 0 0 0.4 0.4 {}
C {ipin.sym} 240 -330 0 0 {name=p1 lab=ibin[7:0]}
C {opin.sym} 400 -350 0 0 {name=p2 
lab=bcd2[1:0]
verilog_type=reg}
C {title.sym} 160 -30 0 0 {name=l1 author="Stefan Schippers"}
C {opin.sym} 400 -320 0 0 {name=p3
lab=bcd1[3:0]
verilog_type=reg}
C {opin.sym} 400 -290 0 0 {name=p4 lab=bcd0[3:0] verilog_type=reg}
C {code.sym} 260 -380 0 0 {name=CODE value="// Copyright (C) 2008 DJ Delorie <dj delorie com>
// Distributed under the terms of the GNU General Public License,
// either verion 2 or (at your choice) any later version.

always @ (ibin)
begin

  case (ibin)
    0 : begin bcd2 <= 2'b00; bcd1 <= 4'b0000; bcd0 <= 4'b0000; end
    1 : begin bcd2 <= 2'b00; bcd1 <= 4'b0000; bcd0 <= 4'b0001; end
    2 : begin bcd2 <= 2'b00; bcd1 <= 4'b0000; bcd0 <= 4'b0010; end
    3 : begin bcd2 <= 2'b00; bcd1 <= 4'b0000; bcd0 <= 4'b0011; end
    4 : begin bcd2 <= 2'b00; bcd1 <= 4'b0000; bcd0 <= 4'b0100; end
    5 : begin bcd2 <= 2'b00; bcd1 <= 4'b0000; bcd0 <= 4'b0101; end
    6 : begin bcd2 <= 2'b00; bcd1 <= 4'b0000; bcd0 <= 4'b0110; end
    7 : begin bcd2 <= 2'b00; bcd1 <= 4'b0000; bcd0 <= 4'b0111; end
    8 : begin bcd2 <= 2'b00; bcd1 <= 4'b0000; bcd0 <= 4'b1000; end
    9 : begin bcd2 <= 2'b00; bcd1 <= 4'b0000; bcd0 <= 4'b1001; end
    10 : begin bcd2 <= 2'b00; bcd1 <= 4'b0001; bcd0 <= 4'b0000; end
    11 : begin bcd2 <= 2'b00; bcd1 <= 4'b0001; bcd0 <= 4'b0001; end
    12 : begin bcd2 <= 2'b00; bcd1 <= 4'b0001; bcd0 <= 4'b0010; end
    13 : begin bcd2 <= 2'b00; bcd1 <= 4'b0001; bcd0 <= 4'b0011; end
    14 : begin bcd2 <= 2'b00; bcd1 <= 4'b0001; bcd0 <= 4'b0100; end
    15 : begin bcd2 <= 2'b00; bcd1 <= 4'b0001; bcd0 <= 4'b0101; end
    16 : begin bcd2 <= 2'b00; bcd1 <= 4'b0001; bcd0 <= 4'b0110; end
    17 : begin bcd2 <= 2'b00; bcd1 <= 4'b0001; bcd0 <= 4'b0111; end
    18 : begin bcd2 <= 2'b00; bcd1 <= 4'b0001; bcd0 <= 4'b1000; end
    19 : begin bcd2 <= 2'b00; bcd1 <= 4'b0001; bcd0 <= 4'b1001; end
    20 : begin bcd2 <= 2'b00; bcd1 <= 4'b0010; bcd0 <= 4'b0000; end
    21 : begin bcd2 <= 2'b00; bcd1 <= 4'b0010; bcd0 <= 4'b0001; end
    22 : begin bcd2 <= 2'b00; bcd1 <= 4'b0010; bcd0 <= 4'b0010; end
    23 : begin bcd2 <= 2'b00; bcd1 <= 4'b0010; bcd0 <= 4'b0011; end
    24 : begin bcd2 <= 2'b00; bcd1 <= 4'b0010; bcd0 <= 4'b0100; end
    25 : begin bcd2 <= 2'b00; bcd1 <= 4'b0010; bcd0 <= 4'b0101; end
    26 : begin bcd2 <= 2'b00; bcd1 <= 4'b0010; bcd0 <= 4'b0110; end
    27 : begin bcd2 <= 2'b00; bcd1 <= 4'b0010; bcd0 <= 4'b0111; end
    28 : begin bcd2 <= 2'b00; bcd1 <= 4'b0010; bcd0 <= 4'b1000; end
    29 : begin bcd2 <= 2'b00; bcd1 <= 4'b0010; bcd0 <= 4'b1001; end
    30 : begin bcd2 <= 2'b00; bcd1 <= 4'b0011; bcd0 <= 4'b0000; end
    31 : begin bcd2 <= 2'b00; bcd1 <= 4'b0011; bcd0 <= 4'b0001; end
    32 : begin bcd2 <= 2'b00; bcd1 <= 4'b0011; bcd0 <= 4'b0010; end
    33 : begin bcd2 <= 2'b00; bcd1 <= 4'b0011; bcd0 <= 4'b0011; end
    34 : begin bcd2 <= 2'b00; bcd1 <= 4'b0011; bcd0 <= 4'b0100; end
    35 : begin bcd2 <= 2'b00; bcd1 <= 4'b0011; bcd0 <= 4'b0101; end
    36 : begin bcd2 <= 2'b00; bcd1 <= 4'b0011; bcd0 <= 4'b0110; end
    37 : begin bcd2 <= 2'b00; bcd1 <= 4'b0011; bcd0 <= 4'b0111; end
    38 : begin bcd2 <= 2'b00; bcd1 <= 4'b0011; bcd0 <= 4'b1000; end
    39 : begin bcd2 <= 2'b00; bcd1 <= 4'b0011; bcd0 <= 4'b1001; end
    40 : begin bcd2 <= 2'b00; bcd1 <= 4'b0100; bcd0 <= 4'b0000; end
    41 : begin bcd2 <= 2'b00; bcd1 <= 4'b0100; bcd0 <= 4'b0001; end
    42 : begin bcd2 <= 2'b00; bcd1 <= 4'b0100; bcd0 <= 4'b0010; end
    43 : begin bcd2 <= 2'b00; bcd1 <= 4'b0100; bcd0 <= 4'b0011; end
    44 : begin bcd2 <= 2'b00; bcd1 <= 4'b0100; bcd0 <= 4'b0100; end
    45 : begin bcd2 <= 2'b00; bcd1 <= 4'b0100; bcd0 <= 4'b0101; end
    46 : begin bcd2 <= 2'b00; bcd1 <= 4'b0100; bcd0 <= 4'b0110; end
    47 : begin bcd2 <= 2'b00; bcd1 <= 4'b0100; bcd0 <= 4'b0111; end
    48 : begin bcd2 <= 2'b00; bcd1 <= 4'b0100; bcd0 <= 4'b1000; end
    49 : begin bcd2 <= 2'b00; bcd1 <= 4'b0100; bcd0 <= 4'b1001; end
    50 : begin bcd2 <= 2'b00; bcd1 <= 4'b0101; bcd0 <= 4'b0000; end
    51 : begin bcd2 <= 2'b00; bcd1 <= 4'b0101; bcd0 <= 4'b0001; end
    52 : begin bcd2 <= 2'b00; bcd1 <= 4'b0101; bcd0 <= 4'b0010; end
    53 : begin bcd2 <= 2'b00; bcd1 <= 4'b0101; bcd0 <= 4'b0011; end
    54 : begin bcd2 <= 2'b00; bcd1 <= 4'b0101; bcd0 <= 4'b0100; end
    55 : begin bcd2 <= 2'b00; bcd1 <= 4'b0101; bcd0 <= 4'b0101; end
    56 : begin bcd2 <= 2'b00; bcd1 <= 4'b0101; bcd0 <= 4'b0110; end
    57 : begin bcd2 <= 2'b00; bcd1 <= 4'b0101; bcd0 <= 4'b0111; end
    58 : begin bcd2 <= 2'b00; bcd1 <= 4'b0101; bcd0 <= 4'b1000; end
    59 : begin bcd2 <= 2'b00; bcd1 <= 4'b0101; bcd0 <= 4'b1001; end
    60 : begin bcd2 <= 2'b00; bcd1 <= 4'b0110; bcd0 <= 4'b0000; end
    61 : begin bcd2 <= 2'b00; bcd1 <= 4'b0110; bcd0 <= 4'b0001; end
    62 : begin bcd2 <= 2'b00; bcd1 <= 4'b0110; bcd0 <= 4'b0010; end
    63 : begin bcd2 <= 2'b00; bcd1 <= 4'b0110; bcd0 <= 4'b0011; end
    64 : begin bcd2 <= 2'b00; bcd1 <= 4'b0110; bcd0 <= 4'b0100; end
    65 : begin bcd2 <= 2'b00; bcd1 <= 4'b0110; bcd0 <= 4'b0101; end
    66 : begin bcd2 <= 2'b00; bcd1 <= 4'b0110; bcd0 <= 4'b0110; end
    67 : begin bcd2 <= 2'b00; bcd1 <= 4'b0110; bcd0 <= 4'b0111; end
    68 : begin bcd2 <= 2'b00; bcd1 <= 4'b0110; bcd0 <= 4'b1000; end
    69 : begin bcd2 <= 2'b00; bcd1 <= 4'b0110; bcd0 <= 4'b1001; end
    70 : begin bcd2 <= 2'b00; bcd1 <= 4'b0111; bcd0 <= 4'b0000; end
    71 : begin bcd2 <= 2'b00; bcd1 <= 4'b0111; bcd0 <= 4'b0001; end
    72 : begin bcd2 <= 2'b00; bcd1 <= 4'b0111; bcd0 <= 4'b0010; end
    73 : begin bcd2 <= 2'b00; bcd1 <= 4'b0111; bcd0 <= 4'b0011; end
    74 : begin bcd2 <= 2'b00; bcd1 <= 4'b0111; bcd0 <= 4'b0100; end
    75 : begin bcd2 <= 2'b00; bcd1 <= 4'b0111; bcd0 <= 4'b0101; end
    76 : begin bcd2 <= 2'b00; bcd1 <= 4'b0111; bcd0 <= 4'b0110; end
    77 : begin bcd2 <= 2'b00; bcd1 <= 4'b0111; bcd0 <= 4'b0111; end
    78 : begin bcd2 <= 2'b00; bcd1 <= 4'b0111; bcd0 <= 4'b1000; end
    79 : begin bcd2 <= 2'b00; bcd1 <= 4'b0111; bcd0 <= 4'b1001; end
    80 : begin bcd2 <= 2'b00; bcd1 <= 4'b1000; bcd0 <= 4'b0000; end
    81 : begin bcd2 <= 2'b00; bcd1 <= 4'b1000; bcd0 <= 4'b0001; end
    82 : begin bcd2 <= 2'b00; bcd1 <= 4'b1000; bcd0 <= 4'b0010; end
    83 : begin bcd2 <= 2'b00; bcd1 <= 4'b1000; bcd0 <= 4'b0011; end
    84 : begin bcd2 <= 2'b00; bcd1 <= 4'b1000; bcd0 <= 4'b0100; end
    85 : begin bcd2 <= 2'b00; bcd1 <= 4'b1000; bcd0 <= 4'b0101; end
    86 : begin bcd2 <= 2'b00; bcd1 <= 4'b1000; bcd0 <= 4'b0110; end
    87 : begin bcd2 <= 2'b00; bcd1 <= 4'b1000; bcd0 <= 4'b0111; end
    88 : begin bcd2 <= 2'b00; bcd1 <= 4'b1000; bcd0 <= 4'b1000; end
    89 : begin bcd2 <= 2'b00; bcd1 <= 4'b1000; bcd0 <= 4'b1001; end
    90 : begin bcd2 <= 2'b00; bcd1 <= 4'b1001; bcd0 <= 4'b0000; end
    91 : begin bcd2 <= 2'b00; bcd1 <= 4'b1001; bcd0 <= 4'b0001; end
    92 : begin bcd2 <= 2'b00; bcd1 <= 4'b1001; bcd0 <= 4'b0010; end
    93 : begin bcd2 <= 2'b00; bcd1 <= 4'b1001; bcd0 <= 4'b0011; end
    94 : begin bcd2 <= 2'b00; bcd1 <= 4'b1001; bcd0 <= 4'b0100; end
    95 : begin bcd2 <= 2'b00; bcd1 <= 4'b1001; bcd0 <= 4'b0101; end
    96 : begin bcd2 <= 2'b00; bcd1 <= 4'b1001; bcd0 <= 4'b0110; end
    97 : begin bcd2 <= 2'b00; bcd1 <= 4'b1001; bcd0 <= 4'b0111; end
    98 : begin bcd2 <= 2'b00; bcd1 <= 4'b1001; bcd0 <= 4'b1000; end
    99 : begin bcd2 <= 2'b00; bcd1 <= 4'b1001; bcd0 <= 4'b1001; end
    100 : begin bcd2 <= 2'b01; bcd1 <= 4'b0000; bcd0 <= 4'b0000; end
    101 : begin bcd2 <= 2'b01; bcd1 <= 4'b0000; bcd0 <= 4'b0001; end
    102 : begin bcd2 <= 2'b01; bcd1 <= 4'b0000; bcd0 <= 4'b0010; end
    103 : begin bcd2 <= 2'b01; bcd1 <= 4'b0000; bcd0 <= 4'b0011; end
    104 : begin bcd2 <= 2'b01; bcd1 <= 4'b0000; bcd0 <= 4'b0100; end
    105 : begin bcd2 <= 2'b01; bcd1 <= 4'b0000; bcd0 <= 4'b0101; end
    106 : begin bcd2 <= 2'b01; bcd1 <= 4'b0000; bcd0 <= 4'b0110; end
    107 : begin bcd2 <= 2'b01; bcd1 <= 4'b0000; bcd0 <= 4'b0111; end
    108 : begin bcd2 <= 2'b01; bcd1 <= 4'b0000; bcd0 <= 4'b1000; end
    109 : begin bcd2 <= 2'b01; bcd1 <= 4'b0000; bcd0 <= 4'b1001; end
    110 : begin bcd2 <= 2'b01; bcd1 <= 4'b0001; bcd0 <= 4'b0000; end
    111 : begin bcd2 <= 2'b01; bcd1 <= 4'b0001; bcd0 <= 4'b0001; end
    112 : begin bcd2 <= 2'b01; bcd1 <= 4'b0001; bcd0 <= 4'b0010; end
    113 : begin bcd2 <= 2'b01; bcd1 <= 4'b0001; bcd0 <= 4'b0011; end
    114 : begin bcd2 <= 2'b01; bcd1 <= 4'b0001; bcd0 <= 4'b0100; end
    115 : begin bcd2 <= 2'b01; bcd1 <= 4'b0001; bcd0 <= 4'b0101; end
    116 : begin bcd2 <= 2'b01; bcd1 <= 4'b0001; bcd0 <= 4'b0110; end
    117 : begin bcd2 <= 2'b01; bcd1 <= 4'b0001; bcd0 <= 4'b0111; end
    118 : begin bcd2 <= 2'b01; bcd1 <= 4'b0001; bcd0 <= 4'b1000; end
    119 : begin bcd2 <= 2'b01; bcd1 <= 4'b0001; bcd0 <= 4'b1001; end
    120 : begin bcd2 <= 2'b01; bcd1 <= 4'b0010; bcd0 <= 4'b0000; end
    121 : begin bcd2 <= 2'b01; bcd1 <= 4'b0010; bcd0 <= 4'b0001; end
    122 : begin bcd2 <= 2'b01; bcd1 <= 4'b0010; bcd0 <= 4'b0010; end
    123 : begin bcd2 <= 2'b01; bcd1 <= 4'b0010; bcd0 <= 4'b0011; end
    124 : begin bcd2 <= 2'b01; bcd1 <= 4'b0010; bcd0 <= 4'b0100; end
    125 : begin bcd2 <= 2'b01; bcd1 <= 4'b0010; bcd0 <= 4'b0101; end
    126 : begin bcd2 <= 2'b01; bcd1 <= 4'b0010; bcd0 <= 4'b0110; end
    127 : begin bcd2 <= 2'b01; bcd1 <= 4'b0010; bcd0 <= 4'b0111; end
    128 : begin bcd2 <= 2'b01; bcd1 <= 4'b0010; bcd0 <= 4'b1000; end
    129 : begin bcd2 <= 2'b01; bcd1 <= 4'b0010; bcd0 <= 4'b1001; end
    130 : begin bcd2 <= 2'b01; bcd1 <= 4'b0011; bcd0 <= 4'b0000; end
    131 : begin bcd2 <= 2'b01; bcd1 <= 4'b0011; bcd0 <= 4'b0001; end
    132 : begin bcd2 <= 2'b01; bcd1 <= 4'b0011; bcd0 <= 4'b0010; end
    133 : begin bcd2 <= 2'b01; bcd1 <= 4'b0011; bcd0 <= 4'b0011; end
    134 : begin bcd2 <= 2'b01; bcd1 <= 4'b0011; bcd0 <= 4'b0100; end
    135 : begin bcd2 <= 2'b01; bcd1 <= 4'b0011; bcd0 <= 4'b0101; end
    136 : begin bcd2 <= 2'b01; bcd1 <= 4'b0011; bcd0 <= 4'b0110; end
    137 : begin bcd2 <= 2'b01; bcd1 <= 4'b0011; bcd0 <= 4'b0111; end
    138 : begin bcd2 <= 2'b01; bcd1 <= 4'b0011; bcd0 <= 4'b1000; end
    139 : begin bcd2 <= 2'b01; bcd1 <= 4'b0011; bcd0 <= 4'b1001; end
    140 : begin bcd2 <= 2'b01; bcd1 <= 4'b0100; bcd0 <= 4'b0000; end
    141 : begin bcd2 <= 2'b01; bcd1 <= 4'b0100; bcd0 <= 4'b0001; end
    142 : begin bcd2 <= 2'b01; bcd1 <= 4'b0100; bcd0 <= 4'b0010; end
    143 : begin bcd2 <= 2'b01; bcd1 <= 4'b0100; bcd0 <= 4'b0011; end
    144 : begin bcd2 <= 2'b01; bcd1 <= 4'b0100; bcd0 <= 4'b0100; end
    145 : begin bcd2 <= 2'b01; bcd1 <= 4'b0100; bcd0 <= 4'b0101; end
    146 : begin bcd2 <= 2'b01; bcd1 <= 4'b0100; bcd0 <= 4'b0110; end
    147 : begin bcd2 <= 2'b01; bcd1 <= 4'b0100; bcd0 <= 4'b0111; end
    148 : begin bcd2 <= 2'b01; bcd1 <= 4'b0100; bcd0 <= 4'b1000; end
    149 : begin bcd2 <= 2'b01; bcd1 <= 4'b0100; bcd0 <= 4'b1001; end
    150 : begin bcd2 <= 2'b01; bcd1 <= 4'b0101; bcd0 <= 4'b0000; end
    151 : begin bcd2 <= 2'b01; bcd1 <= 4'b0101; bcd0 <= 4'b0001; end
    152 : begin bcd2 <= 2'b01; bcd1 <= 4'b0101; bcd0 <= 4'b0010; end
    153 : begin bcd2 <= 2'b01; bcd1 <= 4'b0101; bcd0 <= 4'b0011; end
    154 : begin bcd2 <= 2'b01; bcd1 <= 4'b0101; bcd0 <= 4'b0100; end
    155 : begin bcd2 <= 2'b01; bcd1 <= 4'b0101; bcd0 <= 4'b0101; end
    156 : begin bcd2 <= 2'b01; bcd1 <= 4'b0101; bcd0 <= 4'b0110; end
    157 : begin bcd2 <= 2'b01; bcd1 <= 4'b0101; bcd0 <= 4'b0111; end
    158 : begin bcd2 <= 2'b01; bcd1 <= 4'b0101; bcd0 <= 4'b1000; end
    159 : begin bcd2 <= 2'b01; bcd1 <= 4'b0101; bcd0 <= 4'b1001; end
    160 : begin bcd2 <= 2'b01; bcd1 <= 4'b0110; bcd0 <= 4'b0000; end
    161 : begin bcd2 <= 2'b01; bcd1 <= 4'b0110; bcd0 <= 4'b0001; end
    162 : begin bcd2 <= 2'b01; bcd1 <= 4'b0110; bcd0 <= 4'b0010; end
    163 : begin bcd2 <= 2'b01; bcd1 <= 4'b0110; bcd0 <= 4'b0011; end
    164 : begin bcd2 <= 2'b01; bcd1 <= 4'b0110; bcd0 <= 4'b0100; end
    165 : begin bcd2 <= 2'b01; bcd1 <= 4'b0110; bcd0 <= 4'b0101; end
    166 : begin bcd2 <= 2'b01; bcd1 <= 4'b0110; bcd0 <= 4'b0110; end
    167 : begin bcd2 <= 2'b01; bcd1 <= 4'b0110; bcd0 <= 4'b0111; end
    168 : begin bcd2 <= 2'b01; bcd1 <= 4'b0110; bcd0 <= 4'b1000; end
    169 : begin bcd2 <= 2'b01; bcd1 <= 4'b0110; bcd0 <= 4'b1001; end
    170 : begin bcd2 <= 2'b01; bcd1 <= 4'b0111; bcd0 <= 4'b0000; end
    171 : begin bcd2 <= 2'b01; bcd1 <= 4'b0111; bcd0 <= 4'b0001; end
    172 : begin bcd2 <= 2'b01; bcd1 <= 4'b0111; bcd0 <= 4'b0010; end
    173 : begin bcd2 <= 2'b01; bcd1 <= 4'b0111; bcd0 <= 4'b0011; end
    174 : begin bcd2 <= 2'b01; bcd1 <= 4'b0111; bcd0 <= 4'b0100; end
    175 : begin bcd2 <= 2'b01; bcd1 <= 4'b0111; bcd0 <= 4'b0101; end
    176 : begin bcd2 <= 2'b01; bcd1 <= 4'b0111; bcd0 <= 4'b0110; end
    177 : begin bcd2 <= 2'b01; bcd1 <= 4'b0111; bcd0 <= 4'b0111; end
    178 : begin bcd2 <= 2'b01; bcd1 <= 4'b0111; bcd0 <= 4'b1000; end
    179 : begin bcd2 <= 2'b01; bcd1 <= 4'b0111; bcd0 <= 4'b1001; end
    180 : begin bcd2 <= 2'b01; bcd1 <= 4'b1000; bcd0 <= 4'b0000; end
    181 : begin bcd2 <= 2'b01; bcd1 <= 4'b1000; bcd0 <= 4'b0001; end
    182 : begin bcd2 <= 2'b01; bcd1 <= 4'b1000; bcd0 <= 4'b0010; end
    183 : begin bcd2 <= 2'b01; bcd1 <= 4'b1000; bcd0 <= 4'b0011; end
    184 : begin bcd2 <= 2'b01; bcd1 <= 4'b1000; bcd0 <= 4'b0100; end
    185 : begin bcd2 <= 2'b01; bcd1 <= 4'b1000; bcd0 <= 4'b0101; end
    186 : begin bcd2 <= 2'b01; bcd1 <= 4'b1000; bcd0 <= 4'b0110; end
    187 : begin bcd2 <= 2'b01; bcd1 <= 4'b1000; bcd0 <= 4'b0111; end
    188 : begin bcd2 <= 2'b01; bcd1 <= 4'b1000; bcd0 <= 4'b1000; end
    189 : begin bcd2 <= 2'b01; bcd1 <= 4'b1000; bcd0 <= 4'b1001; end
    190 : begin bcd2 <= 2'b01; bcd1 <= 4'b1001; bcd0 <= 4'b0000; end
    191 : begin bcd2 <= 2'b01; bcd1 <= 4'b1001; bcd0 <= 4'b0001; end
    192 : begin bcd2 <= 2'b01; bcd1 <= 4'b1001; bcd0 <= 4'b0010; end
    193 : begin bcd2 <= 2'b01; bcd1 <= 4'b1001; bcd0 <= 4'b0011; end
    194 : begin bcd2 <= 2'b01; bcd1 <= 4'b1001; bcd0 <= 4'b0100; end
    195 : begin bcd2 <= 2'b01; bcd1 <= 4'b1001; bcd0 <= 4'b0101; end
    196 : begin bcd2 <= 2'b01; bcd1 <= 4'b1001; bcd0 <= 4'b0110; end
    197 : begin bcd2 <= 2'b01; bcd1 <= 4'b1001; bcd0 <= 4'b0111; end
    198 : begin bcd2 <= 2'b01; bcd1 <= 4'b1001; bcd0 <= 4'b1000; end
    199 : begin bcd2 <= 2'b01; bcd1 <= 4'b1001; bcd0 <= 4'b1001; end
    200 : begin bcd2 <= 2'b10; bcd1 <= 4'b0000; bcd0 <= 4'b0000; end
    201 : begin bcd2 <= 2'b10; bcd1 <= 4'b0000; bcd0 <= 4'b0001; end
    202 : begin bcd2 <= 2'b10; bcd1 <= 4'b0000; bcd0 <= 4'b0010; end
    203 : begin bcd2 <= 2'b10; bcd1 <= 4'b0000; bcd0 <= 4'b0011; end
    204 : begin bcd2 <= 2'b10; bcd1 <= 4'b0000; bcd0 <= 4'b0100; end
    205 : begin bcd2 <= 2'b10; bcd1 <= 4'b0000; bcd0 <= 4'b0101; end
    206 : begin bcd2 <= 2'b10; bcd1 <= 4'b0000; bcd0 <= 4'b0110; end
    207 : begin bcd2 <= 2'b10; bcd1 <= 4'b0000; bcd0 <= 4'b0111; end
    208 : begin bcd2 <= 2'b10; bcd1 <= 4'b0000; bcd0 <= 4'b1000; end
    209 : begin bcd2 <= 2'b10; bcd1 <= 4'b0000; bcd0 <= 4'b1001; end
    210 : begin bcd2 <= 2'b10; bcd1 <= 4'b0001; bcd0 <= 4'b0000; end
    211 : begin bcd2 <= 2'b10; bcd1 <= 4'b0001; bcd0 <= 4'b0001; end
    212 : begin bcd2 <= 2'b10; bcd1 <= 4'b0001; bcd0 <= 4'b0010; end
    213 : begin bcd2 <= 2'b10; bcd1 <= 4'b0001; bcd0 <= 4'b0011; end
    214 : begin bcd2 <= 2'b10; bcd1 <= 4'b0001; bcd0 <= 4'b0100; end
    215 : begin bcd2 <= 2'b10; bcd1 <= 4'b0001; bcd0 <= 4'b0101; end
    216 : begin bcd2 <= 2'b10; bcd1 <= 4'b0001; bcd0 <= 4'b0110; end
    217 : begin bcd2 <= 2'b10; bcd1 <= 4'b0001; bcd0 <= 4'b0111; end
    218 : begin bcd2 <= 2'b10; bcd1 <= 4'b0001; bcd0 <= 4'b1000; end
    219 : begin bcd2 <= 2'b10; bcd1 <= 4'b0001; bcd0 <= 4'b1001; end
    220 : begin bcd2 <= 2'b10; bcd1 <= 4'b0010; bcd0 <= 4'b0000; end
    221 : begin bcd2 <= 2'b10; bcd1 <= 4'b0010; bcd0 <= 4'b0001; end
    222 : begin bcd2 <= 2'b10; bcd1 <= 4'b0010; bcd0 <= 4'b0010; end
    223 : begin bcd2 <= 2'b10; bcd1 <= 4'b0010; bcd0 <= 4'b0011; end
    224 : begin bcd2 <= 2'b10; bcd1 <= 4'b0010; bcd0 <= 4'b0100; end
    225 : begin bcd2 <= 2'b10; bcd1 <= 4'b0010; bcd0 <= 4'b0101; end
    226 : begin bcd2 <= 2'b10; bcd1 <= 4'b0010; bcd0 <= 4'b0110; end
    227 : begin bcd2 <= 2'b10; bcd1 <= 4'b0010; bcd0 <= 4'b0111; end
    228 : begin bcd2 <= 2'b10; bcd1 <= 4'b0010; bcd0 <= 4'b1000; end
    229 : begin bcd2 <= 2'b10; bcd1 <= 4'b0010; bcd0 <= 4'b1001; end
    230 : begin bcd2 <= 2'b10; bcd1 <= 4'b0011; bcd0 <= 4'b0000; end
    231 : begin bcd2 <= 2'b10; bcd1 <= 4'b0011; bcd0 <= 4'b0001; end
    232 : begin bcd2 <= 2'b10; bcd1 <= 4'b0011; bcd0 <= 4'b0010; end
    233 : begin bcd2 <= 2'b10; bcd1 <= 4'b0011; bcd0 <= 4'b0011; end
    234 : begin bcd2 <= 2'b10; bcd1 <= 4'b0011; bcd0 <= 4'b0100; end
    235 : begin bcd2 <= 2'b10; bcd1 <= 4'b0011; bcd0 <= 4'b0101; end
    236 : begin bcd2 <= 2'b10; bcd1 <= 4'b0011; bcd0 <= 4'b0110; end
    237 : begin bcd2 <= 2'b10; bcd1 <= 4'b0011; bcd0 <= 4'b0111; end
    238 : begin bcd2 <= 2'b10; bcd1 <= 4'b0011; bcd0 <= 4'b1000; end
    239 : begin bcd2 <= 2'b10; bcd1 <= 4'b0011; bcd0 <= 4'b1001; end
    240 : begin bcd2 <= 2'b10; bcd1 <= 4'b0100; bcd0 <= 4'b0000; end
    241 : begin bcd2 <= 2'b10; bcd1 <= 4'b0100; bcd0 <= 4'b0001; end
    242 : begin bcd2 <= 2'b10; bcd1 <= 4'b0100; bcd0 <= 4'b0010; end
    243 : begin bcd2 <= 2'b10; bcd1 <= 4'b0100; bcd0 <= 4'b0011; end
    244 : begin bcd2 <= 2'b10; bcd1 <= 4'b0100; bcd0 <= 4'b0100; end
    245 : begin bcd2 <= 2'b10; bcd1 <= 4'b0100; bcd0 <= 4'b0101; end
    246 : begin bcd2 <= 2'b10; bcd1 <= 4'b0100; bcd0 <= 4'b0110; end
    247 : begin bcd2 <= 2'b10; bcd1 <= 4'b0100; bcd0 <= 4'b0111; end
    248 : begin bcd2 <= 2'b10; bcd1 <= 4'b0100; bcd0 <= 4'b1000; end
    249 : begin bcd2 <= 2'b10; bcd1 <= 4'b0100; bcd0 <= 4'b1001; end
    250 : begin bcd2 <= 2'b10; bcd1 <= 4'b0101; bcd0 <= 4'b0000; end
    251 : begin bcd2 <= 2'b10; bcd1 <= 4'b0101; bcd0 <= 4'b0001; end
    252 : begin bcd2 <= 2'b10; bcd1 <= 4'b0101; bcd0 <= 4'b0010; end
    253 : begin bcd2 <= 2'b10; bcd1 <= 4'b0101; bcd0 <= 4'b0011; end
    254 : begin bcd2 <= 2'b10; bcd1 <= 4'b0101; bcd0 <= 4'b0100; end
    255 : begin bcd2 <= 2'b10; bcd1 <= 4'b0101; bcd0 <= 4'b0101; end
  endcase
end
"}
