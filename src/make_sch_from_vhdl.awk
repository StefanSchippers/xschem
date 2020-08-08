#!/bin/sh
gawk '
BEGIN{
 print " **** schematic-ing  " ARGV[1] "  *****"

 # stdcell_lib = "stdcell_lvt_lib"
 # stdcell_lib = "cmoslib_t11"
 #  stdcell_lib = "cmoslib_t9"

 stdcell_lib=""
 space=20
 if( ARGV[1] ~ /^[1-9][0-9]*$/ )  {
   width=ARGV[1]+0
   ARGV[1]=""
 } else {
   width=150
 }
 lwidth=20
 textdist=5
 labsize=0.2
 titlesize=0.3
 text_voffset=20
 xoffset=1300
 yoffset=0
 prev_size=0
 lab_voffset=4
 sp=ip=op=n_pin=0
 end_entity=1
 end_component=1
 all_signals=""
 package_decl=0


  # list of stdcell symbol pin coordinates, generated with build_cmoslib.awk

 if( stdcell_lib ) while ( ("ls $HOME/xschem/library/" stdcell_lib "/*.sym") |getline sym )
 {
  print "---> " sym
  tmp=sym
  sub(/.*\//,"", tmp)
  sub(/\..*/,"", tmp)
  cellname=toupper(tmp)
  cell[cellname]=1
  cell[cellname "_AN"]=1
  while( getline <sym)
  {

   if($0 ~ /^B 5 /)
   {
    name=toupper($7)
    sub(/.*=/,"",name)
    dir=$0
    sub(/.*dir[ \t]*=/,"",dir)
    sub(/[ \t]*\}.*/,"",dir)
    cell_pin_x[cellname,name]=($3+$5)/2
    cell_pin_y[cellname,name]=($4+$6)/2
    cell_pin_dir[cellname,name]=toupper(dir)
    cell_pin_x[cellname "_AN",name]=($3+$5)/2
    cell_pin_y[cellname "_AN",name]=($4+$6)/2
    cell_pin_dir[cellname "_AN",name]=toupper(dir)
    if(dir=="in") {
     cell_ipin_num[cellname]++
     cell_ipin_num[cellname "_AN"]++
    }
    else {
     cell_opin_num[cellname]++
     cell_opin_num[cellname "_AN"]++
    }
   }
  }
 }

}

{ cleanup_line() }


($1=="library" || $1=="use"){
 if(!index(use,$0)) use = use $0 "\n"
 next
}

/^[ \t]*attribute[ \t+]/,/;/{
 if(end_entity==0)
 {
  port_attribute = port_attribute $0 "\n"
  next
 }
 else
 {
  attribute = attribute $0 "\n"
  next
 }
}

(($0 ~ /^[ \t]*package[ \t]+.*is[ \t]*$/) && ($0 !~ /[ \t]+body[ \t]+/)) {
 print "start package : " $2
 package_name=$2
 package_decl=1
}

(package_decl){
 print "package -->" $0
 if( $0 !~ /[ \t]+body[ \t]+/ && !package_body)
 {
  package=package "\n" $0
 }
}

($1=="end" &&  package_name ~ $2 && package_decl==1){
 package_decl=0
}

/^[ \t]*package[ \t]+body/ && !package_decl{
 package_body=1
}

(package_body && !package_decl) {
 package=package "\n" $0
}

(package_body && $1~/function|procedure|case/ && !package_decl) {
 package_body++
}

/^[ \t]*end / && package_body && $2 !~/if|loop|for/  && !package_decl{
 package_body--
 if(package_body==0)
 {
   package=use "\n" package
   use=""
 }
}

(package_body>0 || package_decl) { next }
 



($0 ~ /^[ \t]*component[ \t]+/ && !package_decl){
  end_component=0
  name=$2
  schname= name ".sch.make_sch"
  delete type_pin
  delete dir_pin
  delete value_pin
  delete class_pin
  delete num_pin
  delete opin
  delete ipin
  delete spin
  n_pin=ip=op=sp=0
}

{
 sig_type="std_logic"
 pin_label=""
 value=""
 type=""
}

# component ports
/^[ \t]*port[ \t]*\(/,/^[ \t]*\)[ \t]*;/{

 if($0 ~ /:/ && !end_component && !package_decl)
 {
  if($0 ~ /port[ \t]+\(/) sub(/port[ \t]+\(/,"",$0)
  if($1=="signal") {  # 20071213 remove "signal" keyword written by triad netlister
   $1=""
   $0=$0
  }
  sig_type=$4
  sub(/_vector.*/,"",sig_type)
  sub(/\(.*/,"",sig_type)
  sub(/\).*/,"",sig_type)
  is_bus=0
  if($0 ~ /\(.*[ \t]+(to|downto)[ \t]+.*\)/)
  {
   is_bus=1
   index_high=$0
   sub(/^[^(]*\(/,"", index_high)
   index_low=index_high
   sub(/^[ \t]+/,"",index_high)
   sub(/[ \t]+(to|downto)[ \t]+.*$/,"",index_high)
   sub(/^.*[ \t]+(to|downto)[ \t]+/,"",index_low)
   sub(/\).*$/,"", index_low)
   gsub(/[ \t]+/,"",index_low)     #20071213 remove spaces in indexes
   gsub(/[ \t]+/,"",index_high)

  }
  else 
  {
   index_high=index_low=0
  }
  pin_label=$1 #toupper


# these assignments are used to resolve single bit assignments on vector ports, that is:
#        A_READ(22) => ADD_READ(22),
#        A_READ(21) => ADD_READ2_N(21),
# where A_READ is a component pin declared as:
# A_READ : IN std_logic_vector(21 TO 22);
# 
# then in the schematic the assignment will look like:
#
#                                      |
#   ADD_READ2_N[21],ADD_READ[22] o-----| A_READ[21:22]
#                                      |

  component_pins[name,toupper($1)]=abs(index_high-index_low)+1  # for bit slice port assignments
  component_hi[name,toupper($1)]= index_high+0
  component_low[name,toupper($1)]= index_low+0
  #print "component: " name, "pins:" component_pins[name,toupper($1)], toupper($1)
  #print "component: " name, "  hi:" component_hi[name,toupper($1)]
  #print "component: " name, "  lo:" component_low[name,toupper($1)]
#
#
#





  if(is_bus) pin_label=pin_label "[" index_high ":" index_low "]"

  if($0 ~ /:=/) 
  {  
   value=$0
   sub(/^.*:= */,"", value) #15122003 added spaces after :=
   sub(/;/,"", value)
   gsub(/"/,"\\\\\"", value)
   value= "\"" value "\""
  }
 
  type=$3
  if(type=="in") 
  {
    type="ipin"
    type_pin[n_pin]=sig_type
    dir_pin[n_pin]=type
    value_pin[n_pin]=value
    num_pin[n_pin++]=ip
    component_sig_pos[name,basename(toupper(pin_label))] = ip  # 20090407
    ipin[ip++] =  pin_label
  }
  if(type=="out")
  {
    type="opin"
    type_pin[n_pin]=sig_type
    dir_pin[n_pin]=type
    value_pin[n_pin]=value
    num_pin[n_pin++]=op
    component_sig_pos[name,basename(toupper(pin_label))] = op  # 20090407
    opin[op++] = pin_label
  }
  if(type=="inout")
  {
    type="iopin"
    type_pin[n_pin]=sig_type
    dir_pin[n_pin]=type
    value_pin[n_pin]=value
    num_pin[n_pin++]=op
    component_sig_pos[name,basename(toupper(pin_label))] = op  # 20090407
    opin[op++] = pin_label
  }
  component_sig_dir[name,basename(toupper(pin_label))]=type
  component_sig_type[name,basename(toupper(pin_label))]=sig_type
  component_sig_value[name,basename(toupper(pin_label))]=value

  #print "component: " pin_label, type,sig_type
 }
}


# components generic
/^[ \t]*generic[ \t]*\(/,/^[ \t]*\)[ \t]*;/{
 if($0 ~/:/ && !end_component && !package_decl)
 {
  if($0 ~ /generic[ \t]+\(/) sub(/generic[ \t]+\(/,"",$0)
#  sig_type=$3;
  sig_type=$0                           #03062002: allow spaces in sig_type
  sub(/^.* : [ \t]*/,"",sig_type)
  sub(/[ \t]*(\:=|;).*$/,"",sig_type)
  if(sig_type ~ /[ \t]+/) sig_type="\"" sig_type "\""

  pin_label=$1
  if($0 ~ /:=/) 
  {  
   value=$0
   sub(/^.*:= */,"", value)   #15122003 spaces after := added
   sub(/;/,"", value)
   gsub(/"/,"\\\\\"", value)
   if(value ~ / /) value="\"" value "\""
  }
  type_pin[n_pin]=sig_type
  dir_pin[n_pin]="generic"
  value_pin[n_pin]=value
  num_pin[n_pin++]=ip
  ipin[ip++] =  pin_label
  component_sig_dir[name,basename(toupper(pin_label))]= "generic"
  component_sig_type[name,basename(toupper(pin_label))]=sig_type
  component_sig_value[name,basename(toupper(pin_label))]=value
 }
}


# end component
( $1=="end" && $2 ~ "component" && !package_decl){
  end_component=1 
  if(!(toupper(name) in cell))
  {
   #print ">>>>> end component: " name
   print "G {}" > schname 
   print_sch(schname,type_pin, dir_pin, value_pin, class_pin, num_pin, opin, ipin,spin,
            n_pin, ip, op, sp)
   close(schname)
  }
}


# start architecture
/^[ \t]*architecture/ {
 print "start architecture: " $2
 start_arch=1
 arch_label=$2
}


# function defined inside architecture (not in package)
($0 ~ /^[ \t]*(function|procedure)[ \t]+/ && !start_code){
 inline_function_name=$2
 sub(/\(.*/,"",inline_function_name)
 print "inline function name: " inline_function_name
 inline_function=1
}

inline_function==1{
 if(start_arch==1 && !package_decl && !package_body)
 {
  print "function in architecture: " $0
  architecture_decl=architecture_decl $0 "\n"

  if( ($1=="end" && $2==";") || ($1=="end" && $2 == inline_function_name)  )
  {
    print "end inline function"
    if( !package_decl && !package_body)
      inline_function=0
  }
  next
 }
}


# # architecture types / subtypes
# ($1 ~ /^(type|subtype|constant)$/) , /;/{
#  if(start_arch==1)
#  {
#   architecture_decl=architecture_decl $0 "\n"
#   next
#  }
# }

# architecture types / subtypes
($1 ~ /^(type|subtype|constant|alias)$/){    # 20111121 added alias
 if(start_arch==1)
 {
  architecture_decl=architecture_decl $0 "\n"
  if($0 !~ /;(([ \t]*)|([ \t]*--.*))$/) type_decl=1
  if($0 ~ /^[ \t]*type[ \t]+.*record/) record=1		# 04062002 better parsing of record declarations
  next
 }
}

(type_decl && start_arch){
  architecture_decl=architecture_decl $0 "\n"
  if($1=="record")  record=1
  if(record) {
    if($0 ~ /end[ \t]+record[ \t]*;[ \t]*$/) {type_decl=0; record=0}
  }
  else if($0 ~ /;[ \t]*$/) type_decl=0
  next
}


# mark generate statements to not confuse with instantiation statements
/(^|[ \t]+)for[ \t]+.*[ \t]+generate[ \t]*$/{
 start_generate++
}
/^[ \t]+end[ \t]+generate[ \t]*;/{
 start_generate--
}


# instance
{
  tmpsave=$0
  sub(/--.*/,"")
  if(NF==3 && start_code==1 && $2==":" && $3 !~/^block$/ && $3 !~/^process *[(]*/ && !start_generate) {
    start_inst=1
    print ">>start inst: " $0
    delete i_type_pin
    delete i_dir_pin
    delete i_value_pin
    delete i_class_pin
    delete i_num_pin
    delete i_opin
    delete i_ipin
    i_n_pin=i_ip=i_op=0

    component_name=$3
    inst_name=$1
  }
  $0=tmpsave
 }

# instance port assignment
( start_inst==1 && $0 ~/=>/ ){
  sub(/--.*$/,"",$0)		# 04062002 hide nasty comments on port assignments
  #print "port assignment: --------------------------------"

  # if assignment line is wrapped get next line also 17052002
  if($0 ~ /=>[ \t]*$/) {
    tmp=$0
    getline ; cleanup_line() ;
    $0 = tmp " " $0
  }

  sub(/^[ \t]*port[ \t]+map[ \t]*\(/,"",$0)
  sub(/,/,"",$0)
  split($0,kk, /[ \t]*=>[ \t]*/)
  pin_label=vhdl_to_cad(kk[2])
  gsub(/"/,"\\\\\"",pin_label)     # 03062002 escape quotes (string generics 4 example)
  if(pin_label ~ / /) pin_label="\"" pin_label "\""


  formal_basename = toupper(kk[1])
  sub(/\(.*/,"",formal_basename)
  gsub(/[ \t]*/,"",formal_basename)
  formal_index=kk[1]
  sub(/^.*\([ \t]*/,"",formal_index)
  sub(/[ \t]*\).*/,"",formal_index)
  sub(/[ \t]+(to|downto)[ \t]+/,":",formal_index)

  #print "port assignment: " formal_index, pin_label, component_pins[component_name,formal_basename], formal_basename
  #print "port assignment: " component_hi[component_name,formal_basename], component_low[component_name,formal_basename]
#
#  This piece of code deals with single bit assignments on
#  vector ports, so we group the single bits in a unique node label, 
#  preserving the right order 
#
  if(doing_slice && prev_formal_basename!=formal_basename)
  {
   print_slice()
  }

  if(formal_index ~ /^[0-9]+$/ && component_pins[component_name,formal_basename]>1)
  { 
    #print "port assignment: slice port assign: " pin_label, "formal_index=" formal_index
    prev_formal_basename=formal_basename
    doing_slice=1
    slice_pin_array[formal_index]=pin_label
    next
  } 
  else if(formal_index ~ /[0-9]+:[0-9]+/ && component_pins[component_name,formal_basename]>1)
  {
     #print "port assignment:  slice bus port assign" 
     split(formal_index, formal_index_array,":")
     formal_index_array[1]= formal_index_array[1]+0
     formal_index_array[0]= formal_index_array[0]+0
     if(abs(formal_index_array[2] - formal_index_array[1])+1 < component_pins[component_name,formal_basename] )
     {
      #print "port assignment: slice"
      k=formal_index_array[1]
      actual_k=idx_hi(pin_label)
      actual_dir=sign(idx_low(pin_label) - idx_hi(pin_label) )
      while(1)
      {
       slice_pin_array[k]= basename(pin_label) "[" actual_k "]"

       if(k == formal_index_array[2]) break
       k+= sign(formal_index_array[2]-formal_index_array[1])
       actual_k+=actual_dir
      }
      #printf "\n"
      prev_formal_basename=formal_basename
      doing_slice=1
      next
     }
  }

  add_inst_pin(pin_label)
}

#
#  This piece of code deals with single bit assignments on
#  vector ports, so we group the single bits in a unique node label, 
#  preserving the right order 
#
function print_slice(       c_h,c_l, slice, slice_pin )
{
    if( (c_h=component_hi[component_name,prev_formal_basename]) > (c_l=component_low[component_name,prev_formal_basename]) )
     for(slice=c_h; slice >= c_l; slice--)
     {
      #print "..>" slice
      if(slice !=c_h) slice_pin=slice_pin ","
      slice_pin=slice_pin slice_pin_array[slice]
     }
    else
     for(slice=c_h; slice <= c_l; slice++)
     {
      #print "..>" slice
      if(slice !=c_h) slice_pin=slice_pin ","
      slice_pin=slice_pin slice_pin_array[slice]
     }
    #print "slice_pin=" slice_pin
    add_inst_pin(slice_pin)
    slice_pin=""
    delete slice_pin_array
    doing_slice=0
    prev_formal_basename=""
}


function add_inst_pin(pin_label)
{
  if(doing_slice) 			# nasty bug fixed 24112002
     formal_name=prev_formal_basename
  else
     formal_name = basename(vhdl_to_cad(kk[1]))
  type=component_sig_dir[component_name, formal_name]
  sig_type=component_sig_type[component_name, formal_name]

  # 20070823 value not inherited from component
  # value=component_sig_value[component_name, formal_name]
  value = ""
  # /20070823

  # print "add instance pin: " formal_name " => "  pin_label "   " type, sig_type

  if(type=="ipin")
  {
    i_name_pin[i_n_pin]=formal_name
    i_type_pin[i_n_pin]=sig_type
    i_dir_pin[i_n_pin]=type
    i_value_pin[i_n_pin]=value
    i_num_pin[i_n_pin++]=i_ip
    i_ipin[i_ip++] =  pin_label
  }
  if(type=="opin")
  {
    i_name_pin[i_n_pin]=formal_name
    i_type_pin[i_n_pin]=sig_type
    i_dir_pin[i_n_pin]=type
    i_value_pin[i_n_pin]=value
    i_num_pin[i_n_pin++]=i_op
    i_opin[i_op++] = pin_label
  }
  if(type=="iopin")
  {
    i_name_pin[i_n_pin]=formal_name
    i_type_pin[i_n_pin]=sig_type
    i_dir_pin[i_n_pin]=type
    i_value_pin[i_n_pin]=value
    i_num_pin[i_n_pin++]=i_op
    i_opin[i_op++] = pin_label
  }

  if(type=="generic")
  {
   i_name_pin[i_n_pin]=formal_name
   i_type_pin[i_n_pin]=sig_type
   i_dir_pin[i_n_pin]=type
   i_value_pin[i_n_pin]=value
   i_num_pin[i_n_pin++]=i_ip
   i_ipin[i_ip++] =  pin_label
  }
}


# user architecture VHDL code
(start_code==1 && !start_inst && !inline_function){
 if($1=="end")
 {
  if($2=="architecture") tmp=$3  # 20071213 handle "end arhitecture name" vs "end name"
  else tmp=$2
  
 }
 if(tmp==arch_label) 
 {
  start_code=0
  next
 }
 else
 {
   code_lines=code_lines $0 "\n"
 }
}


# end instance
 ($1==")" && $2==";" && start_inst){

  if(doing_slice)
  {
   #print " slice port assign 3" 
   print_slice()
  }

  start_inst=0
  print_signals(inst_name, component_name, i_name_pin, i_type_pin, i_dir_pin, i_value_pin,i_class_pin, i_num_pin, 
                i_opin, i_ipin,i_n_pin, i_ip, i_op)
 }


# begin user VHDL code,  end of arch types/subtypes / functions
/^[ \t]*begin(([ \t]*)|([ \t]*--.*))$/{

 if(start_arch=1 && !start_code)
 {
  print "begin VHDL code " $0
  start_arch=0
  start_code=1
 }
}

# begin entity decl
/^[ \t]*entity[ \t]+/{
  end_entity=0
  name=$2
  #print "entity=" name
  entname= name ".sch"
  delete e_type_pin
  delete e_dir_pin
  delete e_value_pin
  delete e_class_pin
  delete e_num_pin
  delete e_opin
  delete e_ipin
  delete e_spin
  e_n_pin=e_ip=e_op=e_sp=0
}

{
 sig_type="std_logic"
 pin_label=""
 value=""
 type=""
}

# entity ports
/^[ \t]*port[ \t]*\(/,/^[ \t]*\)[ \t]*;/{
 if($0 ~ /:/ && !end_entity)
 {
  if($0 ~ /port[ \t]+\(/) sub(/port[ \t]+\(/,"",$0)
  if($1=="signal") {  # 20071213 remove "signal" keyword written by triad netlister
   $1=""
   $0=$0
  }
  sig_type=$4
  sub(/_vector.*/,"",sig_type)
  sub(/\(.*/,"",sig_type)
  is_bus=0
  if($0 ~ /\(.*[ \t]+(to|downto)[ \t]+.*\)/)
  {
   is_bus=1
   index_high=$0
   sub(/^[^(]*\(/,"", index_high)
   index_low=index_high
   sub(/^[ \t]+/,"",index_high)
   sub(/[ \t]+(to|downto)[ \t]+.*$/,"",index_high)
   sub(/^.*[ \t]+(to|downto)[ \t]+/,"",index_low)
   sub(/\).*$/,"", index_low)
   gsub(/[ \t]+/,"",index_low)     #20071213 remove spaces in indexes
   gsub(/[ \t]+/,"",index_high)

  }
  pin_label=$1  #toupper
  if(is_bus) pin_label=pin_label "[" index_high ":" index_low "]"
  if($0 ~ /:=/) 
  {  
   value=$0
   sub(/^.*:= */,"", value) #15122003 spaces after := added
   sub(/;/,"", value)
   gsub(/"/,"\\\\\"", value)
   value= "\"" value "\""
  }
 
  type=$3
  if(type=="in") 
  {
    type="ipin"
    e_type_pin[e_n_pin]=sig_type
    e_dir_pin[e_n_pin]=type
    e_value_pin[e_n_pin]=value
    e_num_pin[e_n_pin++]=e_ip
    e_ipin[e_ip++] =  pin_label
    entity_full_name[basename(pin_label)]=pin_label
    entity_pin_type[basename(pin_label)]=sig_type
  }
  if(type=="out")
  {
    type="opin"
    e_type_pin[e_n_pin]=sig_type
    e_dir_pin[e_n_pin]=type
    e_value_pin[e_n_pin]=value
    e_num_pin[e_n_pin++]=e_op
    e_opin[e_op++] = pin_label
    entity_full_name[basename(pin_label)]=pin_label
    entity_pin_type[basename(pin_label)]=sig_type
  }
  if(type=="inout")
  {
    type="iopin"
    e_type_pin[e_n_pin]=sig_type
    e_dir_pin[e_n_pin]=type
    e_value_pin[e_n_pin]=value
    e_num_pin[e_n_pin++]=e_op
    e_opin[e_op++] = pin_label
    entity_full_name[basename(pin_label)]=pin_label
    entity_pin_type[basename(pin_label)]=sig_type
  }
 }
}


# entity generics
/^[ \t]*generic[ \t]*\(/,/^[ \t]*\)[ \t]*;/{
 if($0 ~/:/ && !end_entity)
 {
  if($0 ~ /generic[ \t]+\(/) sub(/generic[ \t]+\(/,"",$0)
#  sig_type=$3;
  sig_type=$0				#03062002: allow spaces in sig_type
  sub(/^.* : [ \t]*/,"",sig_type)
  sub(/[ \t]*(\:=|;).*$/,"",sig_type)
  if(sig_type ~ /[ \t]+/) sig_type="\"" sig_type "\""
  pin_label=$1
  if($0 ~ /:=/) 
  {  
   value=$0
   sub(/^.*:= */,"", value) # 15122003 spaces after := added
   sub(/;/,"", value)
   gsub(/"/,"\\\\\"", value)
   if(value ~ / /) value="\"" value "\""
  }
  e_type_pin[e_n_pin]=sig_type
  e_dir_pin[e_n_pin]="generic"
  e_value_pin[e_n_pin]=value
  e_num_pin[e_n_pin++]=e_ip
  e_ipin[e_ip++] =  pin_label
  entity_full_name[basename(pin_label)]=pin_label
  entity_pin_type[basename(pin_label)]=sig_type
 }
}


# end entity
( $1=="end" && $2 ~ name) || ( $1=="end" && $2=="entity"  && $3 ~ name) {  # 20071213 handle "end entity name"
  end_entity=1 

}


# eliminated constant, put in arch declarative section
# architecture signals
/^[ \t]*signal[ \t]/,/;/ {
 if(start_arch==1)
 {
   if($1=="signal" || $1=="constant" )   
   {
    list=""
    class=$1
   }
  
   list= list " " $0 " " 
   
   if($0 ~ /;/)
   {
    signals = list
    sub(class,"",signals)
    sub(/:.*$/,"",signals)
    gsub(/[ \t,]+/, " ", signals)
  
    sig_type=list
    sub(/[ \t]*:=.*$/,"", sig_type)
    sub(/^.*:[ \t]*/,"", sig_type)
    sub(/[ \t]*;.*$/,"", sig_type)
  
       if(list ~ /:=/)
       {
        value=list
        sub(/^.*:=[ \t]*/,"", value)
        sub(/[ \t]*;[ \t]*.*/,"", value)
        gsub(/"/,"\\\\\"", value)
        value= "\"" value "\""
       }
    n_sig=split(signals,pin_label_array)
   
    for(i=1;i<=n_sig;i++)
    {
       pin_label=pin_label_array[i] #toupper
  
       #print "encountered signal\n"
       s_type=sig_type
       sub(/_vector.*/,"",s_type)
       sub(/\(.*/,"",s_type)
       is_bus=0
       if(sig_type ~ /\(.*([ \t]+to[ \t]+|[ \t]+downto[ \t]+).*\)/)
       {
        is_bus=1
        index_high=sig_type
        sub(/^[^(]*\(/,"", index_high)    #23122003 stop on first bracket
        index_low=index_high
        sub(/^[ \t]+/,"",index_high)
        sub(/[ \t]+(to|downto)[ \t]+.*$/,"",index_high)
        sub(/^.*[ \t]+(to|downto)[ \t]+/,"",index_low)
        sub(/\).*$/,"", index_low)
        gsub(/[ \t]+/,"",index_low)	#23122003 remove spaces in indexes
        gsub(/[ \t]+/,"",index_high)
       }
       
       if(is_bus) pin_label=pin_label "[" index_high ":" index_low "]"
     
       type="signal"
       e_type_pin[e_n_pin]=s_type
       e_class_pin[e_n_pin]=class
       e_dir_pin[e_n_pin]=type
       e_value_pin[e_n_pin]=value
       e_num_pin[e_n_pin++]=e_sp
       e_spin[e_sp++] = pin_label
       entity_full_name[basename(pin_label)]=pin_label
       entity_pin_type[basename(pin_label)]=s_type
    }
   } 
 } 
  
}

 


 

END{

 for( i in entity_pin_type ) { print i " - " entity_pin_type[i] }
  

 print "G {" code_lines "}" >entname
 
 if(attribute !~/^[ \t]*$/) print "C {devices/attributes} 0 -200 0 0 {" attribute " }" >entname
 if(port_attribute !~/^[ \t]*$/) print "C {devices/port_attributes} 0 -400 0 0 {" port_attribute " }" >entname
 if(use !~/^[ \t]*$/) print "C {devices/use} 400 -400 0 0 {" use " }" >entname
 if(architecture_decl !~/^[ \t]*$/) print "C {devices/arch_declarations} 800 -400 0 0 {" architecture_decl " }" >entname
 if(package !~/^[ \t]*$/) print "C {devices/package} 1200 -400 0 0 {" package " }" >entname
 print_sch(entname,e_type_pin, e_dir_pin, e_value_pin, e_class_pin, e_num_pin, e_opin, e_ipin,e_spin,
           e_n_pin, e_ip, e_op, e_sp)
 print all_signals >entname

}

function vhdl_to_cad(s)
{
 
 sub(/[ \t]*\([ \t]*/,"[",s)
 sub(/[ \t]+(to|downto)[ \t]+/,":",s) 
 sub(/[ \t]*\)[ \t]*/,"]",s)
 gsub(/[ \t]/,"",s)
 if(s in entity_full_name) s=entity_full_name[s]
 return toupper(s)
}

function idx_hi(s)
{
 sub(/.*\[/,"",s)
 sub(/:.*/,"",s)
 return s
}
function idx_low(s)
{
 sub(/.*:/,"",s)
 sub(/\].*/,"",s)
 return s
}
function basename(s)
{
 sub(/\[.*/,"",s)
 return s
}

function print_sch(schname,type_pin, dir_pin, value_pin, class_pin, num_pin, opin, ipin,spin,
           n_pin, ip, op, sp,        n,m,y,x,i,class, value, num,sig_type,dir,sch_x_offset)

{

 n=ip;if(op>n) n=op
 if(n==0) n=1
 m=(n-1)/2
 y=0
 x=-40
 sch_x_offset=230
 
 for(i=0;i<n_pin;i++)
 {
  dir=dir_pin[i]
  value=value_pin[i]
  num=num_pin[i]
  class=class_pin[i]
  sig_type=type_pin[i]

  if(dir=="generic")
  {
   ipin[num]=toupper(ipin[num])
   printf "C {devices/generic_pin} " (x+sch_x_offset) " " (y+num*space) " 0 0" \
         " {name=g" g_pin++ " generic_type=" sig_type " lab=" ipin[num] " " >schname

   # it seems values were not picked from port assignments but from default values and this is
   # an error!.
   if(value !="") printf "value=" value "}\n" >schname
   else printf "}\n" >schname
  }

  if(dir=="ipin")
  {
   ipin[num]=toupper(ipin[num])
   printf "C {devices/ipin} " (x+sch_x_offset) " " (y+num*space) " 0 0 " \
         " {name=p" p_pin++ " sig_type=" sig_type " lab=" ipin[num] " " >schname
   if(value !="") printf "value=" value "}\n" >schname
   else printf "}\n" >schname
  }

  if(dir=="opin")
  {
   opin[num]=toupper(opin[num])
   printf "C {devices/opin} " (-x+sch_x_offset) " " (y+num*space) " 0 0 " \
         " {name=p" p_pin++ " sig_type=" sig_type " lab=" opin[num] " " >schname
   if(value !="") printf "value=" value "}\n" >schname
   else printf "}\n" >schname
  }

  if(dir=="iopin")
  {
   opin[num]=toupper(opin[num])
   printf "C {devices/iopin} " (-x+sch_x_offset) " " (y+num*space) " 0 0 " \
         " {name=p" p_pin++ " sig_type=" sig_type " lab=" opin[num] " " >schname
   if(value !="") printf "value=" value "}\n" >schname
   else printf "}\n" >schname
  }

  if(dir=="signal")
  {
   spin[num]=toupper(spin[num])
   printf "C {devices/lab_pin} " (-x+sch_x_offset+500) " " (y+num*space) " 0 0 " \
         " {name=p" p_pin++ " sig_type=\"" sig_type "\" lab=" spin[num] " " >schname
   if(class !="") printf "class=" class " " >schname
   if(value !="") printf "value=" value "}\n" >schname
   else printf "}\n" >schname
  }
 }
}

function print_signals(inst_name, component_name, name_pin, type_pin, dir_pin, 
	value_pin,class_pin, num_pin, i_opin, i_ipin, n_pin, ip, op,  # 20090407 ioin and opin renamed to i_ipin and i_opin
                  n,m,y,x,i,value, num,sig_type,dir)  #local vars       to avoid clash with component arrays
{
 n=ip;if(op>n) n=op
 if(n==0) n=4
 m=(n-1)/2
 y=-m*space
 x=-width

 if(yoffset >= 1400)
 {
   yoffset=prev_size=0
   xoffset+=920
 } 
 yoffset += ((prev_size+n)/2+1)*space


 
 for(i=0;i<n_pin;i++)
 {
  dir=dir_pin[i]
  value=value_pin[i]
  num=num_pin[i]
  sig_type=type_pin[i]
  idx=(toupper(component_name) SUBSEP name_pin[i])
  if( idx in cell_pin_x)
  {
   xpin=xoffset+cell_pin_x[idx]
   ypin=yoffset+cell_pin_y[idx]
  }
  else
  {
   if(dir=="opin" || dir=="iopin") xpin=-x+xoffset
   else xpin=x+xoffset

   # 20090407
   # get vertical position from component pin ordering

   if( dir=="ipin" && ( (component_name,name_pin[i]) in component_sig_pos) )  {
     xnum = component_sig_pos[component_name,name_pin[i] ]
     print ">>> 2009 " component_name, name_pin[i]
   }
   else if( (dir=="opin" || dir=="iopin") && ( (component_name,name_pin[i]) in component_sig_pos) )  {
     xnum = component_sig_pos[component_name,name_pin[i] ]
   }
   else {
     xnum = num
     print ">>> xx 2009 " component_name, name_pin[i]
   }
   ypin=y+xnum*space+yoffset

   # ypin=y+num*space+yoffset
   # /20090407
  }
 
  if(dir=="generic")
  {
   pin_lower=basename(tolower(i_ipin[num]))
   # 15112002 check type congruence
   if( pin_lower in entity_pin_type &&  entity_pin_type[pin_lower] != sig_type) {
     print "print_signals(): type mismatch: "  inst_name ":" i_ipin[num] ":" entity_pin_type[pin_lower] "=>" sig_type
     sig_type =  entity_pin_type[pin_lower]
   }
   all_signals = all_signals   "C {devices/lab_pin} " xpin " " ypin " 0 0" \
         " {name=g" g_pin++ " type=" sig_type " lab=" i_ipin[num] " " 

   # if(value !="") all_signals = all_signals   "value=" value "}\n" 
   # else all_signals = all_signals   "}\n" 
   # bug fix: since generics assigned to instances are not declared elsewhere
   # use the generic name also for the value property
   all_signals = all_signals   "value=" i_ipin[num] "}\n"   # <<< 21042002
  }
 
  if(dir=="ipin")
  {
   pin_lower=basename(tolower(i_ipin[num]))
   # 15112002 check type congruence
   if( pin_lower in entity_pin_type &&  entity_pin_type[pin_lower] != sig_type) {
     print "print_signals(): type mismatch: " inst_name ":"  i_ipin[num] ":" entity_pin_type[pin_lower] "=>" sig_type
     sig_type =  entity_pin_type[pin_lower]
   }
   all_signals = all_signals   "C {devices/lab_pin} " xpin " " ypin " 0 0 " \
         " {name=p" p_pin++ " sig_type=" sig_type " lab=" i_ipin[num] " " 
   if(value !="") all_signals = all_signals   "value=" value "}\n" 
   else all_signals = all_signals   "}\n" 
  }
 
  if(dir=="opin")
  {
   pin_lower=basename(tolower(i_opin[num]))
   # 15112002 check type congruence
   if( pin_lower in entity_pin_type &&  entity_pin_type[pin_lower] != sig_type) {
     print "print_signals(): type mismatch: "  inst_name ":" i_opin[num] ":" entity_pin_type[pin_lower] "=>" sig_type
     sig_type =  entity_pin_type[pin_lower]
   }
   all_signals = all_signals   "C {devices/lab_pin} " xpin " " ypin " 0 1 " \
         " {name=p" p_pin++ " sig_type=" sig_type " lab=" i_opin[num] " " 
   if(value !="") all_signals = all_signals   "value=" value "}\n" 
   else all_signals = all_signals   "}\n" 
  }
 
  if(dir=="iopin")
  {
   pin_lower=basename(tolower(i_opin[num]))
   # 15112002 check type congruence
   if( pin_lower in entity_pin_type &&  entity_pin_type[pin_lower] != sig_type) {
     print "print_signals(): type mismatch: "  inst_name ":" i_opin[num] ":" entity_pin_type[pin_lower] "=>" sig_type
     sig_type =  entity_pin_type[pin_lower]
   }
   all_signals = all_signals   "C {devices/lab_pin} " xpin " " ypin " 0 1 " \
         " {name=p" p_pin++ " sig_type=" sig_type " lab=" i_opin[num] " " 
   if(value !="") all_signals = all_signals   "value=" value "}\n" 
   else all_signals = all_signals   "}\n" 
  }
 }

 #  C {micro2/pump_logic} 4700 0 0 0 {name=x1}
 cur_path=ENVIRON["PWD"]
 sub(/^.*\//,"",cur_path)
 if(idx in cell_pin_x)
  all_signals=all_signals "C {" stdcell_lib "/" toupper(component_name) "} " xoffset  " " yoffset " 0 0 {name=x" inst_name "}\n"
 else
  all_signals=all_signals "C {" cur_path "/" component_name "} " xoffset  " " yoffset " 0 0 {name=x" inst_name "}\n"


 prev_size=n
 if( idx in cell_pin_x) prev_size+=8
}  

function sign(x)
{
 if(x+0<0) return -1
 else return 1
}
function abs(x)
{
 if(x+0<0) return -x
 else return x+0
}

function cleanup_line() {
   $0=tolower($0)
   if($0 ~/;[ \t]*--.*$/) sub(/;[ \t]*/, " ;")
   sub(/;[ \t]*$/," ;",$0) #28082002
   if($0 ~ /; /) sub(/; /,";",$0)
   gsub(/:/," : ",$0)
   gsub(/'\''z'\''/,"'\''Z'\''",$0)
   gsub(/'\''u'\''/,"'\''U'\''",$0)
   gsub(/'\''x'\''/,"'\''X'\''",$0)
   gsub(/: =/,":=",$0)
   if(!end_component || !end_entity) gsub(/--.*$/,"",$0)
}

' $@
