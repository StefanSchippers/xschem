#!/usr/bin/awk -f

BEGIN{
  halfpinsize=2.5
  ret = 1
}

FNR==1{
  if(FILENAME ~/\.sym$/) is_symbol = 1
  sch = FILENAME
  sub(/\.[^.]+$/, ".sch", sch)
  if(file_exists(sch)) has_schematic = 1
  pin = 0
  net_assign = 0
  pinseq=0
  max_pinseq = 0
  template_attrs=""
  delete slotdef
  numslots=""
}

{
  while(ret > 0 ) {

    if($0 ~ /^L/){
      x1 = $2 / 10
      y1 = $3 / 10
      x2 = $4 / 10
      y2 = $5 / 10
      lines = lines  "L 4 " order(x1, y1, x2, y2) " {}\n"
    }
    
    # T x y color size visibility show angle alignment num_lines
    # 1 2 3  4     5       6        7    8      9        10
    # show 0:show both, 1:value only 2:name only
    # alignment:
    # 2-------------5--------------8
    # |                            |
    # 1      T    E(4) X    T      7
    # |                            |
    # 0-------------3--------------6
    # num_lines starts at 1
    else if($0 ~ /^T/){
      xt = $2 / 10
      yt = $3 / 10
      size = $5 / 30
      visibility = $6
      show = $7
      angle = $8
      align = $9
      nlines = $10
      text = ""
      len = 0
      for(i = 1; i<= nlines; i++) {
        getline
        if($0 ~/=/) {
          sub(/^refdes=/, "name=")
          if($0 ~/slotdef=/) {
            tmp = $0
            sub(/slotdef=/,"", tmp)
            slotnum=tmp
            sub(/:.*/,"", slotnum)
            sub(/.*:/,"", tmp)
            slotdef[slotnum] = tmp
            # print "slotnum=" slotnum " slotdef=" slotdef[slotnum] > "/dev/stderr"
            continue
          }
          if($0 ~/numslots=/) {
            numslots = $0
            sub(/numslots=/,"", numslots)
            template_attrs = template_attrs $0 "\n"
            continue
          }
          if($0 ~/net=/) {
            tmp = $0
            sub(/net=/,"", tmp)
            net[++net_assign] = tmp
            continue
          }
          if(is_symbol && has_schematic) {
            if($0 ~/name=/ && template_attrs == "") template_attrs = template_attrs escape_chars($0) "\n"
          } else {
            template_attrs = template_attrs escape_chars($0) "\n"
          }
          save = $0
          sub(/^device=/, "type=") 
          if ($0 ~/^value=IO/) { # inconsistency in io-1.sym
            $0 = "type=IO"
          }
          if ($0 ~/^type=/) {
            if($0 ~/=INPUT/) {pin = 1; sub(/=.*/, "=ipin"); template_attrs = template_attrs "lab=xxx\n"}
            if($0 ~/=OUTPUT/) {pin = 1; sub(/=.*/, "=opin"); template_attrs = template_attrs "lab=xxx\n"}
            if($0 ~/=IO/) {pin = 1; sub(/=.*/, "=iopin"); template_attrs = template_attrs "lab=xxx\n"}
            if(is_symbol && has_schematic) global_attrs = "type=subcircuit\n" global_attrs
            else global_attrs = $0 "\n" global_attrs 
          }
          $0 = save
          if(show == 1) {
            sub(/=.*/,"",$0)
            $0 = "@" $0
          }
          if(show == 2) sub(/=.*/,"",$0)
          # print ">>> " $0 ":  template_attrs = " template_attrs > "/dev/stderr"
        }
        if(length($0) > len) len = length($0)
        if(text != "") text = text "\n"
        text = text $0
      }
      correct_align()
      if(visibility) {
        texts = texts  "T {" text "} " xt " " (-yt) " " int(angle/90) " " flip " " size " " size " {}\n"
      }
    } 
       
    else if($0 ~ /^B/){
      x1 = $2 / 10
      y1 = $3 / 10
      x2 = x1 + $4 / 10
      y2 = y1 + $5 / 10
      boxes = boxes "L 4 " x1 " " (-y1) " " x2 " " (-y1) " {}\n"
      boxes = boxes "L 4 " x2 " " (-y2) " " x2 " " (-y1) " {}\n"
      boxes = boxes "L 4 " x1 " " (-y2) " " x2 " " (-y2) " {}\n"
      boxes = boxes "L 4 " x1 " " (-y2) " " x1 " " (-y1) " {}\n"
    }
    
    else if($0 ~ /^V/){ #circle
      circles = circles "A 4 " ($2/10) " " (-$3/10) " " ($4/10) " " 0 " " 360 " {}\n"
    }
    
    # A 1000 1000 100 90 180 3 0 0 0 -1 -1
    else if($0 ~ /^A/){ #arc
      arcs = arcs "A 4 " ($2/10) " " (-$3/10) " " ($4/10) " " ($5) " " ($6) " {}\n"
    }
    
    # H 3 0 0 0 -1 -1 1 -1 -1 -1 -1 -1 5 <--n_lines
    #path object --> simulate with polygon
    else if($0 ~ /^H/){
      numlines =$NF
      polys = polys "P 4 " numlines " " 
      for(i = 0; i < numlines; i++) {
        getline
        sub(/,/, " ", $0)
        if($0 == "z") {
          polyx[i] = polyx[0]
          polyy[i] = polyy[0]
        } else {
          polyx[i] = $2/10
          polyy[i] = -$3/10
        }
        polys = polys polyx[i] " " polyy[i] " " 
      }
      polys = polys "{fill=true}\n"
    }
    
    # N 39000 50400 39000 51000 4
    else if($0 ~/^N/) {
      nx1 = $2/10
      ny1 = -$3/10
      nx2 = $4/10
      ny2 = -$5/10
      propstring = ""
      ret = getline
      if($0 == "{") {
        getline
        while($0 !="}") {
          if($0 ~/^T/) {
            # do nothing for now
          } else {
            if($0 ~/netname=/) sub(/netname=/, "lab=")
            propstring = propstring $0 " "
          }
          getline
        }
      } else {
        wires = wires "N " nx1 " " ny1 " " nx2 " " ny2 " {" propstring  "}\n"
        continue # process again last getlined line
      }
      wires = wires "N " nx1 " " ny1 " " nx2 " " ny2 " {" propstring  "}\n"
      if(propstring!="") {
        wires = wires "C {lab_wire.sym} " (nx1+nx2)/2 " " (ny1+ny2)/2 " 0 0 {" propstring "}\n"
      }
    }
    
    #               selectable angle flip 
    #C 36700 54700    1         90     0   resistor-1.sym
    #component
    else if($0 ~ /^C/){  
      cx = $2/10
      cy=-$3/10
      crot = $5/90
      slot=""
      if(crot == 1) crot = 3
      else if(crot == 3) crot = 1
      cflip = $6
      symbol = $NF
      propstring = ""
      ret = getline
      if($0 == "{") {
        getline
        while($0 !="}") {
          if($0 ~/^T /) {
             # do nothing for now
          } else {
            if($0 ~/net=/) {
              netname = pinnumber = $0
              sub(/net=.*:/,"",pinnumber)
              sub(/net=/,"", netname)
              sub(/:.*/,"", netname)
              $0 = "net:" pinnumber "=" netname
            }
            if($0 ~ /slot=/) {
              slot = $0
              sub(/slot=/,"", slot)
              getline
              continue
            }
            if($0 ~/refdes=/){
              sub(/refdes=/, "name=")
              instname = $0

              if(symbol ~/(in|out|io)-1\.sym/) {
                tmp = $0
                sub(/name=/, "lab=", tmp)
                $0 = tmp
              } else {
                getline
                continue
              }
              
            }
            gsub(/ /, "\\\\ ", $0)
            propstring = propstring $0 "\n"
          }
          getline
        }
        if(slot) {
          propstring = instname ":" slot "\n" propstring
        } else {
          propstring = instname "\n" propstring
        }
      } else {
        components = components  "C {" symbol "} " cx " " cy " " crot " " cflip " {" propstring "}\n"
        continue # process again last getlined line
      }
      components = components  "C {" symbol "} " cx " " cy " " crot " " cflip " {" propstring "}\n"
    }
      
    # P 900 100 750 100 1 0 0
    else if($0 ~ /^P/){
      $2/=10; $3/=10; $4/=10; $5/=10
      pin_idx++
      pin_line[pin_idx] =  "L 3 " order($2, $3, $4, $5) " {}"
      if($8 == 0) {
        pin_box[pin_idx] = "B 5 " $2-halfpinsize " " (-$3-halfpinsize) " " $2+halfpinsize " " (-$3+halfpinsize) 
      } else {
        pin_box[pin_idx] = "B 5 " $4-halfpinsize " " (-$5-halfpinsize) " " $4+halfpinsize " " (-$5+halfpinsize) 
      }
      ret = getline
      if($0 == "{") {
        nattr = 0
        text = ""
        found_name = 0
        found_pintype = 0
        while($0 !="}") {
          getline
          if($1 == "T") {
            xt = $2/10
            yt = $3/10
            color = $4
            size = $5/30
            visible = $6
            show = $7
            angle = $8
            align = $9
            nlines = $10
            for(i=1; i<= nlines; i++) {
              getline
              nattr++
              pin_x[pin_idx, nattr] = xt
              pin_y[pin_idx, nattr] = yt
              pin_size[pin_idx, nattr] = size
              pin_visible[pin_idx, nattr] = visible
              pin_show[pin_idx, nattr] = show
              pin_angle[pin_idx, nattr] = angle
              pin_align[pin_idx, nattr] = align
              pin_lineno[pin_idx, nattr] = i
              attr = value = $0
              sub(/=.*/, "", attr)
              sub(/.*=/, "", value)
              if(attr == "pinnumber") pinnumber = value
              if(attr == "pinlabel") {
                 attr = "name"
                 found_name = 1
              }
              if(attr == "pintype") { 
                found_pintype=1
                attr = "dir"
                if(value!="in" && value !="out") value = "inout"
              }
      
              if(attr == "pinseq") {
                pin_index[value] = pin_idx
                pinseq++
                if(value > max_pinseq) max_pinseq = value
              }
              gsub(/\\/, "\\\\\\\\", value)
              gsub(/ /, "\\\\ ", value)
              gsub(/\\_/, "_", value)
              pin_attr[pin_idx, nattr] = attr
              pin_value[pin_idx, nattr] = value
            }
          }
        }
        if(!found_name) {
          nattr++
          pin_attr[pin_idx, nattr] = "name"
          pin_value[pin_idx, nattr] = pinnumber
        }
        if(!found_pintype) {
          nattr++
          pin_attr[pin_idx, nattr] = "dir"
          pin_value[pin_idx, nattr] = "inout"
        }
        pin_nattr[pin_idx] = nattr
      } else continue # process again last getlined line
    }
    break
  } # while(1)
}

function escape_chars(s,     a, b)
{
  a=b=s
  sub(/=.*/,"",a)
  sub(/.*=/,"",b)
  if(s!~/=/) b = ""

  sub(/"/, "\\\\", b)
  if(b ~ / /) {
    b = "\\\\\"" b "\\\\\""
  }
  s = a "=" b
  return s
}

function print_header()
{
  if(net_assign == 1 && pin_idx == 1) {
    pin = 2
    tmp = net[1]
    sub(/:.*/, "", tmp)
    sub(/net=/, "", tmp)
    template_attrs = template_attrs "lab=" tmp "\n"
  } else if(net_assign >=1) {
    #symbol with attribute-defined connections
    for(i = 1; i <= net_assign; i++) {
      netname = pinnumber = net[i]
      sub(/:.*/,"", netname)
      sub(/.*:/, "", pinnumber)
      template_attrs = template_attrs netname "=" netname "\n"
      if(extra !="") extra = extra " " 
      if(extra_format !="") extra_format = extra_format " " 
      if(extra_pinnumber !="") extra_pinnumber = extra_pinnumber " " 
      extra = extra netname 
      extra_pinnumber = extra_pinnumber pinnumber 
      extra_format = extra_format "@" netname
    }
    if(extra) {
      extra = "extra=\"" extra "\""
      extra_pinnumber = "extra_pinnumber=\"" extra_pinnumber "\""
    }
 
  }

  if(pin == 1) spice_attrs = tedax_attrs="" 
  else if(pin == 2) {
    spice_attrs = tedax_attrs=""
    sub(/type=[^ ]+\n/, "type=label\n", global_attrs)
  } else {
    if(is_symbol && has_schematic) {
      spice_attrs="format=\"@name @pinlist @symname " extra_format "\"\n"
    } else {
      spice_attrs="format=\"@name @pinlist @value " extra_format "\"\n"
    }
    tedax_attrs = "tedax_format=\"footprint @name @footprint\n" \
       "value @name @value\n" \
       "device @name @device\n" \
       "@comptag\"\n"
  }
  print "v {xschem version=2.9.9 file_version=1.2}"
  template_attrs = "template=\"" template_attrs "\"\n"

  if(FILENAME ~/\.sym$/) {
    if(global_attrs !~ /type=/) 
    if(pin == 2) { 
      global_attrs = "type=label\n" global_attrs
    } else {
      global_attrs = "type=symbol\n" global_attrs
    }
    print "K {" global_attrs template_attrs tedax_attrs spice_attrs 
    if(extra) {
      print extra
      print extra_pinnumber
    }
    print "}"
  } else {
    print "K {}"
  }

  print "G {}"
  print "V {}"
  print "S {}"
  print "E {}"
}

function file_exists(f,     r, c)
{
  r = getline c < f
  close(f)
  if(r==-1) return 0
  else return 1
}

function order(x1, y1, x2, y2,       tmp) 
{
  y1 = -y1
  y2 = -y2
  if(x2<x1) { 
    tmp = x1; x1 = x2; x2 = tmp; tmp = y1; y1 = y2; y2 = tmp
  } else if(x2 == x1 && y2 < y1) {
    tmp = y1; y1 = y2; y2 = tmp
  }
  return x1 " " y1 " " x2 " " y2
}

function rectorder(x1, y1, x2, y2,       tmp) 
{
  y1 = -y1
  y2 = -y2
  if(x2 < x1) { tmp=x1;x1=x2;x2=tmp}
  if(y2 < y1) { tmp=y1;y1=y2;y2=tmp}
  return x1 " " y1 " " x2 " " y2
}

function alert(s)
{
  print s > "/dev/stderr"
}

# 2-------------5--------------8
# |                            |
# 1      T    E(4) X    T      7
# |                            |
# 0-------------3--------------6
#
# assumes xt, yt, angle, align, len are set globally
# corrects angle, xt, yt, sets flip
function correct_align(          hcorrect, vcorrect)
{
  hcorrect = 17
  vcorrect = 25
  if     (angle ==   0 && align == 0 ) { angle = 180; flip = 1}
  else if(angle ==  90 && align == 0 ) { angle =  90; flip = 1}
  else if(angle == 180 && align == 0 ) { angle =   0; flip = 1}
  else if(angle == 270 && align == 0 ) { angle = 270; flip = 1}

  if     (angle ==   0 && align == 3 ) { angle = 180; flip = 1; xt-=size*hcorrect*len}
  else if(angle ==  90 && align == 3 ) { angle =  90; flip = 1; yt-=size*hcorrect*len}
  else if(angle == 180 && align == 3 ) { angle =   0; flip = 1; xt+=size*hcorrect*len}
  else if(angle == 270 && align == 3 ) { angle = 270; flip = 1; yt+=size*hcorrect*len}

  else if(angle ==   0 && align == 6 ) { angle = 180; flip = 0}
  else if(angle ==  90 && align == 6 ) { angle =  90; flip = 0}
  else if(angle == 180 && align == 6 ) { angle =   0; flip = 0}
  else if(angle == 270 && align == 6 ) { angle = 270; flip = 0}

  else if(angle ==   0 && align == 8 ) { angle =   0; flip = 1}
  else if(angle ==  90 && align == 8 ) { angle = 270; flip = 1}
  else if(angle == 180 && align == 8 ) { angle = 180; flip = 1}
  else if(angle == 270 && align == 8 ) { angle =  90; flip = 1}

  else if(angle ==   0 && align == 7 ) { angle =   0; flip = 1; yt+=size*vcorrect}
  else if(angle ==  90 && align == 7 ) { angle = 270; flip = 1; xt-=size*vcorrect}
  else if(angle == 180 && align == 7 ) { angle = 180; flip = 1; yt-=size*vcorrect}
  else if(angle == 270 && align == 7 ) { angle =  90; flip = 1; xt+=size*vcorrect}

  else if(angle ==   0 && align == 4 ) { angle =   0; flip = 1; yt+=size*vcorrect; xt+=size*hcorrect*len}
  else if(angle ==  90 && align == 4 ) { angle = 270; flip = 1; xt-=size*vcorrect; yt+=size*hcorrect*len}
  else if(angle == 180 && align == 4 ) { angle = 180; flip = 1; yt-=size*vcorrect; xt-=size*hcorrect*len}
  else if(angle == 270 && align == 4 ) { angle =  90; flip = 1; xt+=size*vcorrect; yt-=size*hcorrect*len}

  else if(angle ==   0 && align == 2 ) { angle =   0; flip = 0}
  else if(angle ==  90 && align == 2 ) { angle = 270; flip = 0}
  else if(angle == 180 && align == 2 ) { angle = 180; flip = 0}
  else if(angle == 270 && align == 2 ) { angle =  90; flip = 0}

  else if(angle ==   0 && align == 1 ) { angle =   0; flip = 0; yt+=size*vcorrect}
  else if(angle ==  90 && align == 1 ) { angle = 270; flip = 0; xt-=size*vcorrect}
  else if(angle == 180 && align == 1 ) { angle = 180; flip = 0; yt-=size*vcorrect}
  else if(angle == 270 && align == 1 ) { angle =  90; flip = 0; xt+=size*vcorrect}

  else if(angle ==   0 && align == 5 ) { angle =   0; flip = 0; xt-=size*hcorrect*len}
  else if(angle ==  90 && align == 5 ) { angle = 270; flip = 0; yt-=size*hcorrect*len}
  else if(angle == 180 && align == 5 ) { angle = 180; flip = 0; xt+=size*hcorrect*len}
  else if(angle == 270 && align == 5 ) { angle =  90; flip = 0; yt+=size*hcorrect*len}
}

END{
  print_header()
  print texts
  print boxes
  print lines
  print circles
  print arcs
  print polys
  print components
  print wires
  # i is the pinseq
  npin = 0 # order of pins in xschem
  for(i = 1; i <= max_pinseq; i++) {

    if( i in pin_index) idx = pin_index[i]
    else continue
    print pin_line[idx]
    nattr = pin_nattr[idx]
    attr_string=""
    for(j = 1; j <= nattr; j++) {
      attr_string = attr_string pin_attr[idx,j] "=" pin_value[idx,j] "\n"
    }


    if(numslots > 1 ) {
      pinseq = attr_string
      sub(/.*pinseq=/,"", pinseq)
      sub(/[\n ].*/,"", pinseq)
      # print "pinseq=" pinseq > "/dev/stderr"
      sub(/pinnumber=[^ \n]+/,"", attr_string)
      # print "---\n" attr_string "\n---\n" > "/dev/stderr"
      slotted_pinnumber = ""
      for(j = 1; j<= numslots; j++) {
        # print "slotdef[" j "]=" slotdef[j] > "/dev/stderr"
        split(slotdef[j], slotdef_arr, ",")
        if(slotted_pinnumber !="") slotted_pinnumber = slotted_pinnumber ":"
        slotted_pinnumber = slotted_pinnumber slotdef_arr[pinseq]
      }
      # print "slotted_pinnumber=" slotted_pinnumber > "/dev/stderr"
      attr_string = attr_string "pinnumber=" slotted_pinnumber "\n"
    }
   




    print pin_box[idx] " {" attr_string "}"
    for(j = 1; j <= nattr; j++) {
      xt = pin_x[idx,j]
      yt = pin_y[idx, j]
      size = pin_size[idx, j]
      visible = pin_visible[idx, j]
      show = pin_show[idx, j]
      angle = pin_angle[idx, j]
      align = pin_align[idx, j]
      attr = "@#" npin ":" pin_attr[idx, j]
      len = length(pin_value[idx, j])
      correct_align()
      if( visible ) {
        if(pin_attr[idx, j] ~/^pinnumber$/) text_attr="layer=13" 
        else text_attr=""
        print "T {" attr "} " xt " " (-yt) " " int(angle/90) " " flip " " size " " size " {" text_attr "}"
      }
    }
    npin++
  }
}
 
