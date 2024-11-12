#!/usr/bin/awk -f
#
#  File: vhdl.awk
#  
#  This file is part of XSCHEM,
#  a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit 
#  simulation.
#  Copyright (C) 1998-2024 Stefan Frederik Schippers
# 
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA



{  # default is to print the current line if any other rule is not executed.
 no_print=0
}

# 20080213 packages are marked so they will be printed verbatim
/\/\/\/\/ begin package/, /\/\/\/\/ end package/{
 if($0 ~ /^\/\/\/\//) next
 else print $0
 next
}

/\/\/\/\/ begin user declarations/, /\/\/\/\/ end user declarations/{
 if($0 ~ /^\/\/\/\//) next
 else user_declarations = user_declarations "\n" $0
 next
}

/\/\/\/\/ begin user attributes/, /\/\/\/\/ end user attributes/{
 if($0 ~ /^\/\/\/\//) next
 else user_attributes = user_attributes "\n" $0
 next
}

/---- start primitive/{primitive_line=""; primitive_mult=$4;primitive=1; next} # 20071217
/---- end primitive/{ # 20071217
  primitive=0
#  print "<<<<" primitive_line ">>>>"
  $0 = primitive_line
  gsub(/----pin\(/, " ----pin(",$0)
  for(j=1;j<= primitive_mult; j++) {
    for(i=1;i<=NF;i++) {
      prim_field=$i
      if($i ~ /^----pin\(.*\)/) {
        sub(/----pin\(/,"",prim_field)
        sub(/\)$/,"",prim_field)
        pport_mult = split(prim_field, prim_field_array,/,/)


        # if bussed port connected to primitive and primitive mult==1 print only basename of bus
        if(primitive_mult==1 && pport_mult>1) {
          if(check2(prim_field_array, pport_mult))
           printf "%s(%s downto %s) ", s_b(prim_field_array[1]),
              s_i(prim_field_array[1]), s_i(prim_field_array[pport_mult])
          else {
            printf " ( "
            for(s=1;s<= pport_mult; s++) {
              printf "%s", prim_field_array[s]
              if(s<pport_mult) printf "&"
            }
            printf ") "
         }

        } else {

          # printf "%s ", prim_field_array[1]
          ## 20170917
          printf "%s ", to_round_brackets(prim_field_array[1+(j-1)%pport_mult])
        }
      }
      else if($i ~ /^----name\(.*\)/) {
        sub(/----name\(/,"",prim_field)
        sub(/\)$/,"",prim_field)
        split(prim_field, prim_field_array,/,/)
        sub(/\[/,"_", prim_field_array[j])
        sub(/\]/,"", prim_field_array[j])
        printf "%s ", prim_field_array[j]
      }
      else  printf "%s ", prim_field
    } # end for i
    printf "\n"
  } # end for j
  next
}

primitive==1{primitive_line=primitive_line " " $0; next  } # 20071217

/^\/\/\/\/ begin user architecture code/{
 begin_entity=0
 next
}



##### JUST print things before first entity
(begin_entity!=1){
 if($0 ~ /^entity .*is$/) begin_entity=1;
 else
 {
  print $0
  next
 }
}


##### PRINT PROCESSES WITHOUT ANY CHANGES
/^[ \t]*([A-Za-z_][A-Za-z0-9_]*[ \t]*:)?[ \t]*process/, /^[ \t]*end[ \t]+process/{
 print $0
 next
}


#### BEGIN PRINT COMPONENT
/^component /{
 component=1
 component_name=$2
 no_print=1
}
 

(component==1){
 if($4==":=") $5=get_number($5)
 if($5==":=") $6=get_number($6)
 if($1 ~ /\[.*:.*\]/)
 {
  arch_sig_name[component_name,$1]=s_b($1)  # array used to compare 
					    # actual ports in instances, if they match
					    # avoid indexes (to/downto)
  xx=s_i($1)
  split(xx,xxx,":")

  ## don't add _vector if user defined type
  if($4 ~ /^(boolean|bit|real|std_logic|integer)$/)
    vector_type=$4 "_vector ("
  else
    vector_type=$4 " ("



  if( (xxx[1]+0 > xxx[2]+0) || ( xxx[2]~/[01]/ && xxx[1]!~/^0$/) )
    $4=vector_type xxx[1] " downto " xxx[2] ")"
  else
    $4=vector_type xxx[1] " to " xxx[2] ")"
  $1=s_b($1) 
  print "  " $0
 }
 else print $0
 if($0 ~ /^end component/) component=0
 no_print=1
}

#### BEGIN PRINT ENTITY

/^entity /{
 delete entity_ports
 user_declarations=""
 user_attributes=""
 entity=1
 entity_name=$2
 no_print=1
}

(entity==1){
 if($4==":=") $5=get_number($5)
 if($5==":=") $6=get_number($6)
 if($1 ~ /\[.*:.*\]/)
 {
  xx=s_i($1)
  split(xx,xxx,":")

  ## don't add _vector if user defined type
  if($4 ~ /^(boolean|bit|real|std_logic|integer)$/)
    vector_type=$4 "_vector ("
  else
    vector_type=$4 " ("

  if( (xxx[1]+0 > xxx[2]+0) || (xxx[2]~/[01]/ && xxx[1] !~ /^0$/) )
    $4=vector_type xxx[1] " downto " xxx[2] ")"
  else
    $4=vector_type xxx[1] " to " xxx[2] ")"
  $1=s_b($1)
  print "  " $0
 }
 else print $0 
 entity_ports[$1]=1
 if($1=="end" && $2==entity_name) entity=0
 no_print=1
}
 

#### BEGIN RESOLVE SIGNALS

/^[ \t]*architecture[ \t]+.*[ \t]+is[ \t]*$/{
 arch_rep=$2
 arch=$4
 arch_signal_n = 0 # used to preserve order of signals
 delete arch_signal_num # used to preserve order of signals
 delete arch_signal_dir
 delete arch_index_array
 delete arch_sig_type_array
 delete arch_value_array
		       # omit bit ranges ( CNT => CNT(3 downto 0)  --> CNT => CNT )
 print $0
 no_print=1
} 

/^[ \t]*(signal|constant|variable)[ \t]+/{                 ### dangerous <<< 07062002 added "variable"
 
 if( $NF ~ /\[[^:,]+:[^:,]+\]$/)    # 09112003, corrected to recognize only XXX[3:1] and not XXX[3:1],AA[3]
 {
   up=$NF
   sub(/^.*\[/,"",up)
   sub(/:.*$/,"",up)
   up=up+0
   low=$NF
   sub(/^.*:/,"",low)
   sub(/\].*$/,"",low)
   low=low+0
   if(up >=low) sig_dir = " downto "
   else sig_dir = " to "

 }
 else  sig_dir = " downto "

 if( $2 ~ /.*\[[0-9]+\]/)
 {
  sig_basename=s_b($2)
  sig_index=s_i($2)
  # sig_dir not missing, it is set in the previous if-else 09112003
  sig_type=$4
  sig_class=$1
 }
 else
 {
  sig_basename=$2
  sig_index="no_index" #11092003 set no_index to recognize non-buses
  sig_type=$4
  sig_dir=" downto "  # just to have a value
  sig_class=$1
 }
 if(!(sig_basename in arch_signal_dir)) {
   arch_signal_num[arch_signal_n++] = sig_basename # used to preserve order of signals
 }
 arch_signal_dir[sig_basename]=sig_dir
 arch_signal_class[sig_basename]=sig_class
 arch_sig_type_array[sig_basename]=sig_type
 if($0 ~ /:=/) {
 #  arch_value_array[sig_basename]=$(NF-2)

   tmpval=$0				#04062002 allow spaces in value
   sub(/^.* := /,"",tmpval)
   sub(/  *;.*$/,"",tmpval)
   arch_value_array[sig_basename]=tmpval
 }
 if(!(sig_basename in arch_index_array))
 {
  arch_index_array[sig_basename]= sig_index
 }
 else
 {
  arch_index_array[sig_basename]=arch_index_array[sig_basename] "," sig_index
 }
 no_print=1
}

/^begin$/{

 if( user_declarations!="") print user_declarations

 for(ii = 0; ii < arch_signal_n; ii++)
 {
  i = arch_signal_num[ii]
  ## 04062002 don't add _vector if user defined type
  if(arch_sig_type_array[i] ~ /^(boolean|bit|real|std_logic|integer)$/) 
    vector_type=arch_sig_type_array[i] "_vector ("
  else
    vector_type=arch_sig_type_array[i] " ("

  n=split(arch_index_array[i],tmp,",")
  hsort(tmp, n)
  if(n>1 || (arch_index_array[i] !~ /no_index/)  )  #11092003 if not no_index treat as a bus
  {
   if(check(tmp,n)) 
   {
    if(arch_signal_dir[i] == " downto ")
    {
     arch_sig_name[entity_name, i "[" tmp[1] ":" tmp[n] "]"]=i 
     printf "%s",arch_signal_class[i] " " i " : " vector_type tmp[1] " downto " tmp[n] ")"  #04062002
    }
    else
    {
     arch_sig_name[entity_name, i "[" tmp[n] ":" tmp[1] "]"]=i 
     printf "%s",arch_signal_class[i] " " i " : " vector_type tmp[n] " to " tmp[1] ")" #04062002
    }
   }
   else print "\n**** ERROR >>>> " i " non contigous bus ->" n, "|" arch_index_array[i] "|"
  }
  else 
  {
    # we do not declare parametrized subranges as normally will result in redeclaration
    # of a port signal
    if( i ~ /\[.*\]/)
    {
       range = s_i(i)
       basename=s_b(i)				 # on exactly matching ranges
       arch_sig_name[entity_name, basename "[" range "]"]=i  # used later in port maps to avoid specifying ranges
       if( !(basename in entity_ports) )                     #09112003, eror corrected, basename instead of i in
       {							#arch_sig_name[entity_name,...
        if(range ~ /0:/)
          sub(/:/, " to ", range)
        else
          sub(/:/, " downto ", range)
        range= range ")"
        printf "%s",arch_signal_class[i] " " basename " : " vector_type range #04062002
       }
       else continue
    } 
    else
      printf "%s",arch_signal_class[i] " " i " : " arch_sig_type_array[i]
  }
  if(arch_value_array[i] != "") {				#08112004 add quotes on values
    if(tolower( arch_sig_type_array[i]) ~ /std_logic/ ||	#         if not present
       tolower(arch_sig_type_array[i]) ~ /bit/ ) {		# for verilog/VHDL compatiblity

      # if(tolower(arch_sig_type_array[i]) ~ /vector/) sep="\""
      if(n>1 || (arch_index_array[i] !~ /no_index/)  ) sep="\""
      else sep = "'"
      if( arch_value_array[i] !~ sep) 
        arch_value_array[i] = sep arch_value_array[i] sep
    }
    printf "%s"," := " arch_value_array[i]
  }
  printf " ;\n"
 }


 if(user_attributes!="") print user_attributes
 print $0
 no_print=1
}

#### END RESOLVE SIGNALS


#### BEGIN RESOLVE INSTANCES

(NF==4 && $(NF-1)==":" && $(NF)!=";"){		###  dangerous <<<<<<<
 instance=$2
 gsub(/\[/,"_X_",instance)
 gsub(/\]/,"",instance)
 cell=$4
 no_print=1
}

($1==");" && port_map==1){
 nii=split(instance,inst_arr,",")
 for(j=0;j<p;j++) {
   if(j>=g) {    # do not split generics 06042005
     actual_port_mult=split(inst_actual_port[j],actual_port_array,",")
   }
   else {
     actual_port_mult=1 
     actual_port_array[1]=inst_actual_port[j]
   }
   ck2[j] = check2(actual_port_array,actual_port_mult)
 }
 for(i=1;i<=nii;i++) {
   print inst_arr[i] " : " cell
   if(g>0)
     print "generic map ("
   else
     print "port map ("				# p= pin number of current instance
   for(j=0;j<p;j++) {				# g= flag for generic pin assignment
     if(g>0 && j==g) printf "\n)\nport map ("


     if(j>=g) {    # do not split generics 06042005
       actual_port_mult=split(inst_actual_port[j],actual_port_array,",")
     }
     else {
       actual_port_mult=1 
       actual_port_array[1]=inst_actual_port[j]
     }
    
     # force to actual multiplicty unresolved (due to params) symbol pin multiplicity
     parametrized_formal_range=0       # 20100408
     if(inst_formal_port_mult[j]<0) {
       parametrized_formal_range=1     # 20100408 
       inst_formal_port_mult[j] = actual_port_mult
       inst_formal_up[j] = actual_port_mult -1  #assume inst_formal_low=0 in case of parametrized vector port...
     }

     a=((i-1)*inst_formal_port_mult[j]) % actual_port_mult+1
     b=(-1+i*inst_formal_port_mult[j]) % actual_port_mult+1
     #print "\n-- a=" a " b=" b " formal port mult=" inst_formal_port_mult[j] 
     if(j>0 && j!=g) printf " ,\n"
     else if(j>0)    printf "\n"
     if(inst_formal_port_mult[j]>1  && actual_port_mult>1  && ck2[j]) {
       if( s_i(actual_port_array[a])+0 > s_i(actual_port_array[b])+0) {
          actual_port= s_b(actual_port_array[a]) "(" s_i(actual_port_array[a]) " downto " s_i(actual_port_array[b]) ")"
      }
      else {
        actual_port= s_b(actual_port_array[a]) "(" s_i(actual_port_array[a]) " to " s_i(actual_port_array[b]) ")"
      }
     }
     else if( ck2[j] || inst_formal_port_mult[j]==1) {
         actual_port = actual_port_array[a]
         sub(/\[/,"(",actual_port)
         sub(/\]/,")",actual_port)
         sub(/:/," downto ",actual_port)
     }
     # these lines avoid specifying bit ranges (to/downto) for
     # actual ports that exactly match declared signals or entity ports
     tmp_port=actual_port
     sub(/ (to|downto) /,":",tmp_port)
     sub(/\(/,"[",tmp_port)
     sub(/\)/,"]",tmp_port)
     if( (entity_name,tmp_port) in arch_sig_name ) {
        actual_port=arch_sig_name[entity_name,tmp_port]
     }
  	
     if( ck2[j] || (inst_formal_port_mult[j]==1) ) {  # contiguous bus or single bit


        # 20100408 if formal range contains generic and actual port width is 1 specify formal bit index in assignment
        if( (parametrized_formal_range || (inst_formal_port_mult[j]>1)) && actual_port_mult==1) {
								 # patch for single bit actual port 
								 # assigned on vector formal port 23112002

          # 20170920
          if( parametrized_formal_range && inst_actual_port[j] ~/\[.*:.*\]/) {
            printf "%s","   " inst_formal_port[j] " => " get_number(actual_port) 
          } else 
          # /20170920
 	  for(num=inst_formal_low[j]; ; num+=sign(inst_formal_up[j] - inst_formal_low[j])) {
               if(num !=inst_formal_low[j]) printf " ,\n"
               printf "%s","   " inst_formal_port[j] "(" num ") => " get_number(actual_port)
               if(num==inst_formal_up[j]) break
          }
        }
        else printf "%s","   " inst_formal_port[j] " => " get_number(actual_port)  #07062002 aggiunto get_number x generici
     }
     else {  # non contiguous bus , specify assignments on single bits
       for(num=a; ; num+= sign(b-a)) {
         formal_index= (inst_formal_up[j] >= inst_formal_low[j]) ? \
                remainder(inst_formal_up[j]-num+1+inst_formal_low[j], inst_formal_port_mult[j]) + inst_formal_low[j]: \
                remainder(inst_formal_up[j]+num-1+inst_formal_up[j] , inst_formal_port_mult[j]) + inst_formal_up[j]
         
         if(num !=a) printf " ,\n"
         								#07062002 aggiunto get_number x generici
         printf "%s","   " inst_formal_port[j] "(" formal_index ") => " to_round_brackets(get_number(actual_port_array[num]))
         if(num==b) break
       }
     }
   }
   printf "\n);\n"
 }
 delete inst_arr
 port_map=0
 print_arch_definition=1
 no_print=1
}

(port_map==1 && $2=="=>" ){
 inst_actual_port[p] = $0
 sub(/.*=>[ \t]*/,"", inst_actual_port[p])
 sub(/,[ \t]*$/,"", inst_actual_port[p])
 if($1 ~ /\[.*:.*\]/) {
   index_up=$1
   sub(/^.*\[/,"",index_up)
   sub(/:.*$/,"",index_up)
   index_low=$1
   sub(/^.*:/,"",index_low)
   sub(/\].*$/,"",index_low)
   if(  (index_up !~/^[0-9]+$/) || (index_low !~/^[0-9]+$/)  ) {
    inst_formal_port_mult[p]=-1  # component port index contains parameter, multiplicity cant be resolved 
    inst_formal_up[p]=0
    inst_formal_low[p]=0
   }
   else {
     inst_formal_port_mult[p] = abs(index_up - index_low) + 1
     inst_formal_up[p]=index_up
     inst_formal_low[p]=index_low
     #print "--port assignment:  port " s_b($1)  "  formal idx hi=" index_up " lo=" index_low 
   }
 }
 else {
   inst_formal_port_mult[p] = 1;
   inst_formal_up[p]=0
   inst_formal_low[p]=0
 }
 # if formal port = a[2:2] translate as a(2) 
 if(index_up == index_low && $1 ~ /\[.*:.*\]/)
   inst_formal_port[p] = s_b($1) "(" index_up ")"
 else
   inst_formal_port[p] = s_b($1)
 p++
 no_print=1
}

(port_map==1 && $1~ /^\)/) { no_print=1 }

/^generic map/{
 delete inst_actual_port
 delete inst_formal_port
 delete inst_formal_port_mult
 p=0;g=0
 port_map=1
 no_print=1
}
 

/^port map/{
 if(!port_map){
   delete inst_actual_port
   delete inst_formal_port
   delete inst_formal_port_mult
   p=0;g=0
   port_map=1
 }
 else g=p
 no_print=1
}

($1~ /^end$/ && $2==arch_rep){
 arch_rep=""
 print $0
 no_print=1
}


(no_print==0){
 print  $0
}


#### UTILITY FUNCTIONS
function remainder(n,div,     x) {
  x=n%div
  if(x<o) return x+div
  return x
} 

function abs(x) 
{
 if(x+0<0) return -x
 else return x
}

function sign(x)
{
 if(x+0<0) return -1
 else return 1
}


# heap sort, this is a N*log2(N) routine
function hsort(ra,n,    l,j,ir,i,rra)
{
 if(n<=1) return    # bug fix: if only one element in array never returns
 l=int(n/2)+1
 ir=n
 for(;;) {
  if(l>1) {
   rra=ra[--l]
  }
  else {
   rra=ra[ir]
   ra[ir]=ra[1]
   if(--ir==1) {
    ra[1]=rra
    return
   }
  }
  i=l
  j=l*2
  while(j<=ir) {
   if(j<ir && ra[j] >ra[j+1]) ++j
   if(rra>ra[j]) {
    ra[i]=ra[j]
    j+=(i=j)
   }
   else j=ir+1
  }
  ra[i]=rra
 }
}

# check if an array of numbers arr[1],arr[2]..... is contigous and decreeasing
function check(arr,n      ,i,start,ok)
{
 start=arr[1]
 for(i=2;i<=n;i++)
 {
  if(arr[i]+1 == start)
  {
   start=arr[i]
  }
  else
  {
   return 0
  }
 }
 return 1
}

# check if an array of indexes (sig[3]) arr[1],arr[2]..... 
# is contigous and decreeasing
function check2(arr,n     ,a,name,i,start,ok)
{
 name=s_b(arr[1])
 start=s_i(arr[1])
 if(arr[1] !~ /[0-9]+/) 
 {
  if(n>1) return 0
  else return 1
 }
 for(i=2;i<=n;i++)
 {
  if(s_b(arr[i]) != name) return 0
  if(arr[1] !~ /[0-9]+/) return 0
  a= s_i(arr[i])
  if(a+1 == start)
  {
   start=a
  }
  else
  {
   return 0
  }
 }
 return 1
}
 
function s_i(a)
{
 sub(/.*\[/,"",a)
 sub(/\].*$/,"",a)
 return a
}

function s_b(a)
{
 sub(/\[.*$/,"",a)
 return a
}

function get_number(n)   # follows SPICE conventions
{
 sub(/^[ \t]+/,"",n)
 sub(/[ \t]+$/,"",n)
 if(n !~ /^[-.0-9]/) return n   # not a number
 if( n ~ / / ) return n
 n=toupper(n)
 if(n ~ /A/) return vhdl_paranoia(n*1e-18)
 if(n ~ /F/) return vhdl_paranoia(n*1e-15)
 if(n ~ /P/) return vhdl_paranoia(n*1e-12)
 if(n ~ /N/) return vhdl_paranoia(n*1e-9)
 if(n ~ /U/) return vhdl_paranoia(n*1e-6)
 if(n ~ /M[^E][^G]/) return vhdl_paranoia(n*1e-3)
 if(n ~ /K/) return vhdl_paranoia(n*1e3)
 if(n ~ /MEG/) return vhdl_paranoia(n*1e6)
 if(n ~ /G/) return vhdl_paranoia(n*1e9)
 if(n ~ /T/) return vhdl_paranoia(n*1e12)
 return vhdl_paranoia(n)
}

# convert numbers like 4e-7 to 4.0e-7 as VHDL is stupid enough to
# complain about "integers with negative exponent...."
function vhdl_paranoia(n)
{
 if(n ~ /^[0-9]+[eE]/)
   sub(/[eE]/,".0e",n)
 return n
}

function to_round_brackets(s) {
  sub(/\[/,"(",s)
  sub(/\]/,")",s)
  return s
}



function compact_pinlist(pin, dir                 ,i,ii,base,curr,curr_n,np)
{
 delete pin_ret
 delete net_ret
 delete dir_ret
 
 np=pin["n"]
 if(np) {
   ii=1
   for(i=1;i<=np;i++) {
     base =s_b( pin[i] )
     if(i==1) {curr=base; curr_n=i}
     else { 
       if(base != curr) {
         pin_ret[ii] = compact_label(pin,curr_n,i-1)
         dir_ret[ii] = dir[i-1] 
         ii++
         curr=base;curr_n=i
       }
     }
   }
   pin_ret[ii] = compact_label(pin,curr_n,np)
   dir_ret[ii] = dir[np] 
   pin_ret["n"] =  dir_ret["n"] = ii
 }
}

# 1  2    3    4    5 6 7     8    9    10   11   12
# PP A[3] A[2] A[1] B C K[10] K[9] K[5] K[4] K[3] K[1]
function compact_label(ar,a,b,        ret,start,i)
{
  ret=""
  for(i=a;i<=b;i++) {
    if(i==a) {start=a}
    else {
      if(ar[i-1] !~ /\[/)  {
        if(ar[i-1] != ar[i]) {
          if(start < i-1) { ret = ret (i-start) "*" ar[i-1] ","; start=i }
          else {ret = ret ar[i-1] ","; start=i }
        }
      }
      else if(s_b(ar[i])!=s_b(ar[i-1]) || 
              ( lab_index(ar[i]) != lab_index(ar[i-1])-1 && 
              lab_index(ar[i]) != lab_index(ar[i-1])+1) ) {
        if(start<i-1) 
          ret = ret s_b(ar[start]) "[" lab_index(ar[start]) ":" lab_index(ar[i-1]) "],"
        else
          ret = ret s_b(ar[start]) "[" lab_index(ar[start]) "],"
        start=i
      }
    }
  }
  if(ar[b] !~ /\[/)  {
    if(start < b)  ret = ret (b-start+1) "*" ar[b]
    else ret = ret ar[b]
  }
  else if(start<b)   
    ret = ret s_b(ar[start]) "[" lab_index(ar[start]) ":" lab_index(ar[b]) "]"
  else
    ret = ret s_b(ar[b]) "[" lab_index(ar[b]) "]"
  return ret
}



function lab_index(lab)
{
 sub(/.*\[/,"",lab)
 sub(/\].*/,"",lab)
 return lab+0
}



