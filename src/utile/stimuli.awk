#!/usr/bin/awk -f
# File: stimuli.awk
# 
# This file is part of XSCHEM,
# a schematic capture and Spice/Vhdl/Verilog netlisting tool for circuit
# simulation.
# Copyright (C) 1998-2023 Stefan Frederik Schippers
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

# convert from Lsim like stimuli file to spice PWL form:
#		Stefan Schippers, 02-08-1999
#	04-08-1999: -bus notation is now supported
#	03-09-1999: -avoid writing pwl pairs if signal does non change
#	09-09-1999: -allow "z" values,
#		    -allow binary form mixed with hex: 12aa'0010011'ffff
#		     any character other than the allowed ones can be put
#		     in between.
#		    -correctly pad with z's missing MSB's (but see below....)
#		    -avoid negative time steps for (incorrectly given)
#		     stimuli files like:
#			set wen 1
#			set wen 0
#			set wen 1
#			....
#		    -allow hi-Z state (!!!!!!) . This is done using a
#		     resistor vith value equal to a pwl auxiliary source
#		     in series to the voltage source
#		    -"eldo" or "spice" or "dump" keyword, which allows writing text that is
#		     copied directly without any interpretation, ex:
#		     eldo vwen_pad wen_pad 0 pulse 0 1.8 0 1n 1n 50n 100n
#	14-09-1999: - fixed horrible bug: string compare instead of numeric compare
#		      ....if(time <= pwltmp2)....
#		    - new feature: unspecified bits in bus assignment will
#		      preserve old values, added "x" or "X" for a don't
#		      change condition
#		    - powermill vector file is now ordered: signal list is in the
#		      same order as signal / bus declaration, for easier debugging
#		    - some performance optimizations (less split() calls).
#	17-09-1999: - added beginfile filename ..... endfile
#		      to write commands that are dumped to the named file,
#		      without any translation
#	20-09-1999  - eliminated the use of concatenated strings and split functions.
#	20-09-1999  - added "new_powermill" format that writes epic stimuli files
#		      using force_node_v ... and rel_node_v ...
#	23-09-1999  - added "eldo_simple" format, which prints only ideal voltage
#		      sources, not allowing hi-z state, used to generate powermill
#		      spicelike stimuli.
#		    - the routine that breaks long lines is now embedded
#		    - new sintax: stimuli are enclosed in beginfila name ... endfile
#		      keywords,  allowing mixed formats on different files
#	25-09-1999  - new "origin" keyword that shifts the stimulus @ origin
#		      to time zero. Used to restart powermill simulations.
#	27-09-1999  - bugfixes (dump lines are printed at the end of stimuli file)
#	28-09-1999  - heap sort routine replaces insertion sort, (N log2(N) vs N^2)
#	11-11-1999  - When using hierarchical nodes (xi0.iref) substitute dots with
#		      underscores when using that node name for auxiliary nodes and
#		      voltage generator names, in eldo format.
#	30-12-1999  - fixed a bug in the hsort routine when one element arrays are passed.
#		    - prefixing a bus assignment with the tilde char (~) produces inverted data:
#		      ex: set apad ~AA'0101'FF ---> 0101 0101 1010 0000 0000
#		    - NEW!!!! added relative_powermill format, which generates stimuli files
#		      with no explicit reference to absolute time, usefull for interactive mode 
#		      simulations, (generated file can be read with read_cmd_file command)
#		      example:
#			rel_node_v no=aaa
#			cont 0.002u
#			force_node_v no=aaa v=0
#			cont 1u
#			rel_node_v no=aaa
#			cont 0.002u
#			force_node_v no=aaa v=3
#			cont 1u
#			rel_node_v no=aaa
#			cont 0.002u
#			force_node_v no=aaa v=0
# 	01/01/2000  - Fixed a bug when printing the new relative_powermill format:
#		      when bus assignment is incomplete, tobinary() pads with
#		      spaces the undefined bus MSBits; in this case nothing must
#		      be forced on these signals. a test on space char is introduced.
#		      By the way, why not padding with "x" chars which mean don't
#		      change?
#	01/01/2000  - tobinary() pads missing bits with "x" chars, removed the test
#		      on space described above. I hope this does not introduce 
#		      side effects/bugs.
#	10/01/2000  - Removed explicit pathnames. this is no more an awk script but a 
#		      sh script, which in turn runs awk.
#	21/01/2000  - bugfix: the last "set whatever_node 1" assigment in stimuli file
#		      was incorrectly translated to "force_node_v no=whatever_node v=1"
#		      forcing thus to 1V instead of the Vcc value (which is normally <> 1V).
#	02-02-2000  - better precision using OFMT variable
#	30-03-2000  - added "release" keyword, to release signals before set for the first
#		      assignment (relative_powermill only)
#	01-06-2000  - dump lines are printed in the position they appear, not at end of file.
#		      eldo and spice keywords work as before
#	24-10-2001  - fixed a bug in write_pwl_pair.(incorrect eldo stimuli generation in 
#		      case of assignment at time=previous time+slope).
#	21-11-2001  - NEW! ncsim format
#       03-12-2001  - clock and stop_clock directives, handled by external clock.awk program
#       04-12-2001  - Improvements in ncsim format (dont cares, single bit and bus slice 
#		      assignments	
#	12-07-2002  - Improvements in the clock keyword, now high and low voltage levels can 
#		      be specified too, no backward compatibility issues, see help file
#		      When pressing the Translate button the simulated time is printed
#       24-07-2002  - various bug fixes (ncsim bus slice notation using () instead of [] to
#		      avoid confusing the ncsim TCL interpreter,when using the ncsim source 
#	  	      command )
#       23-08-2002  - in ncsim format comments are printed on translated file to indicate the 
#                     beginning of an alias
#	28-10-2002  - optional index delimiters in buswidth declaration: ex: buswidth add 4 < > 
#		      --> add<3> add<2> add<1> add<0>
#	28-10-2002  - create_eldo_bus keyword can be specified to force bus creation for xelga
#		      plots even when the eldo_simple option is used
#	30-10-2002  - Optional 4th parameter on set command specifies series resistance:
#		      set vxp 8.0 20000 ==> 20K resistance
#       29-10-2003  - New keyword "ground" to specify low voltage level (Vil), similarly to "voltage" for Vih
#       08-09-2005  - New ground_node keyword for specifying different gnd node for sources
#       20100301    - new syntax for set instruction, 
#                       set <signal> <sigval> [res=<resval>] [vhi=<hival>] [vlo=<lowval>] [slope=<slopeval>]
#                     all values can be parametrs and simple arithmetic expressions with parameters
#                   - added simple expression parser on params, supported operators : +, -, *, /, expressions 
#                     evaluated frol left to right, no parenthesis, and no operator priority..
#                     examples:
#                       param tck 2500
#                       param tck_2 tck/2
#                       param tds 300
#                       param vx 5.5
#                       set a vx-0.3
#                       s tck-tds
#
#                    
#
# sample input:
#   time 0
#   slope 0.002
#   unit u
#   voltage 1.8
#   bus apad apad8 apad7 apad6 apad5 apad4 apad3 apad2 apad1 apad0
#    
#   set vneg_h 1 ; set initial state
#   set vnegon 1
#   set vxws_off 1
#   set arrpwh 1
#   set split_negv 0
#   set sw_to_neg 1
#   set vxh 0
#   set apad 000
#    
#   s 110
#    
#   set apad 1ff
#   set arrpwh 0
#   s 8
#   set vxh 1
#   s 0.04
#   set vnegon 0
#   s 7
#
# usage: 
#  expand_alias.awk stimuli3 | clock.awk |stimuli.awk

BEGIN{
 OFMT="%.9g" # better precision
 filename= ARGV[1]
 ARGC=1
 invert=0  # used for invert bus flag (tilde prefix)
 origin=0
 format="relative_powermill"
 roff=1e9
 ron=0.001
 time=0
 slope=0.002
 voltage=1.8
 ground=0
 unit="u"
 gndnode="0"
 signals=0
 powmill_statement=0
 halfvoltageset=0 # 20160412
 file="stimuli.ace"
 for(i=0;i<=127;i++)
 {			
  val[sprintf("%c",i)]=i  # value of char: val["0"]=45
  char[i]=sprintf("%c",i) # char[48]="0"
 }
}

/^[ \t]*beginfile[ \t]+/{ 
  reset_all();file=$2
  next 
}

/<<<alias>>>/{ 
  if(format != "ncsim")  next 
  $0 = $1 " " $2  " " $3 " " $4 # 20100212
}
  
/^[ \t]*(eldo|spice)[ \t]+/{$1="";$0=substr($0,2);dump[++dumpline]=$0;next}

# 20170419 added {time} to be replaced with actual time value in dump string
/^[ \t]*dump[ \t]+/{$1="";gsub(/{time}/,time); $0=substr($0,2);print $0 >file     ;next}
/^[ \t]*origin[ \t]+/{ origin=$2; time=-origin}
/^[ \t]*time[ \t]+/{ time=$2-origin}
/^[ \t]*release[ \t]*/{ if(NF==1) powmill_statement=1}
/^[ \t]*create_eldo_bus/{ create_eldo_bus=1}
/^[ \t]*slope[ \t]+/{ slope=$2}
/^[ \t]*roff[ \t]+/{ roff=$2} # 20160412
/^[ \t]*ron[ \t]+/{ ron=$2} # 20170227
/^[ \t]*halfvoltage[ \t]+/{ halfvoltage=$2; halfvoltageset=1} # 20160412
/^[ \t]*unit[ \t]+/{ unit=$2}
/^[ \t]*ground_node[ \t]+/{ gndnode=$2}
/^[ \t]*format[ \t]+/{ 
  if($2=="hspice") {
    format="eldo"
    variant="hspice"
  }
  else if($2=="hspice_simple") {
    format="eldo_simple"
    variant="hspice"
  }
  else format=$2
}
/^[ \t]*ground[ \t]+/{
  ground=$2
  if(!halfvoltageset) {   # 20170405
    if(is_number(voltage) && is_number(ground)) {
      halfvoltage=(voltage+ground) / 2 
    } else {
      halfvoltage = "'(" voltage "+" ground ")/2)'"
    }
  }
}
/^[ \t]*voltage[ \t]+/{
  voltage = $2
  if(!halfvoltageset) {  # 20170405
    if(is_number(voltage) && is_number(ground)) {
      halfvoltage=(voltage+ground) / 2 
    } else {
      halfvoltage = "'(" voltage "+" ground ")/2'"
    }
  }
}
   
/^[ \t]*reset_sim[ \t]*/{if(format=="ncsim") print "reset" >file}    # usefull in ncsim
/^[ \t]*s[ \t]+/{
 time+=$2
 if(format=="ncsim" && time >=0 ) 
 {
  print "run " $2 " " unit "s" >file
 }
 if(format=="relative_powermill" && time >=0)
 {
  if(powmill_statement>0)
  {
   for(i in set_list)
    print "rel_node_v no=" i>file
   print "\ncont " slope  unit "\n">file
  }
  for(i in set_list)
  {
     v=set_list[i]
     if(v ~ /^[10]$/) {vv=(v==1 ? voltage:ground)}
     else if(v ~ /^[zZ]$/) {vv=halfvoltage}
     else {vv = v}
     if(v !~ /[zZ]/) print "force_node_v no=" i " v=" vv>file
  }
  print "\ncont " $2  unit "\n">file
  print "; time= " time > file
  powmill_statement++
  delete set_list
  
 } 
}

/^[ \t]*buswidth[ \t]+/{
  openbracket= $4 != "" ? $4 : "["
  closebracket= $5 != "" ? $5 : ( $4 !="" ? "": "]" )
  buswidth[$2]=$3
  for(i=($3); i>=1;i--)
   bus[$2,i]=$2 openbracket  $3-i closebracket

   
}

/^[ \t]*bus[ \t]+/{
 buswidth[$2]=NF-2
 for(i=3;i<=NF;i++)
 {
  bus[$2 , i-2]=$i
 }
}

{ if(file=="") next}

/^[ \t]*endfile[ \t]*$/{ end_file();next }
/^[ \t]*endfile[ \t]*;.*$/{ end_file();next }

/^[ \t]*set[ \t]+/{

 # 20100301
 res = ron 
 vhi = voltage
 vlo = ground
 slp=slope
 sub(/[\t ]*;.*/,"")
 for(i=4; i<=NF; i++) {
   if($i ~ /^;/) break
   #20150918 delete $i after using it
   if((i==4) && ($i+0>0) && NF==4 ) { res=$i;$i=""; break} 
   if(tolower($i)  ~ /^r(es)?=/) { res=$i; sub(/^.*=/,"",res); $i="" }
   if(tolower($i)  ~ /^vhi(gh)?=/) { vhi=$i; sub(/^.*=/,"",vhi); $i="" }
   if(tolower($i)  ~ /^vlow?=/) { vlo=$i; sub(/^.*=/,"",vlo); $i="" }
   if(tolower($i)  ~ /^slo(pe)?=/) { slp=$i; sub(/^.*=/,"",slp); $i="" }
 }
 $0 = $0  # 20150918

 if(!halfvoltageset) { # 20170405
   if(is_number(vhi) && is_number(vlo)) {
     halfvoltage=(vhi+vlo) / 2
   } else {
     halfvoltage = "'(" vhi "+" vlo ")/2'"
   }
 }
     
 if($2 in buswidth)
 {
  if($3 ~ /^~/)  # inversion flag on bus assignment
  {
   sub(/^~/,"",$3)
   invert=1
  }
  else invert=0
  binstring=tobinary($3,buswidth[$2],invert)

  if(format=="ncsim" && time >=0)
  {
   sig= "." $2
   gsub(/\./,":",sig)
   sub(/^x+/,"",binstring)
   num = length(binstring)
   quote="\""
   w=buswidth[$2]
   low=s_i(bus[$2,w])		# LSB bit of the bus
   high=s_i(bus[$2,w-num+1])		# MSB bit of the bus
   if(num < w && num >1 ) 
   { 
    sig=sig "(" high ":" low ")" ; quote="\"" 
   }
   if(num < w && num ==1) 
   {
    sig=sig "(" low ")" ; quote="'" 
   }
   if($3 !~ /[zZxX]/)
   {
    print "force " sig " = {" quote binstring quote "}" >file
   }
   else if($3 ~ /^[zZ]+$/)
   {
    print "release " sig >file
   }
   else 
   {
    sig= "." $2
    gsub(/\./,":",sig)
    for(idx=1; idx<=num;idx++)
    {
     current_bit=substr(binstring,num - idx+1,1)
     if(current_bit ~ /[zZ]/)
     {
       print "release " sig "(" s_i(bus[$2, w-idx+1]) ")"  >file
     }
     else if(current_bit !~ /[xX]/)
     {
      print "force " sig "(" s_i(bus[$2, w-idx+1]) ") = {'" current_bit "'}" >file
     }
    }
   }
  }

  for(i=1;i<=buswidth[$2];i++)
  {
   s=bus[$2,i]
   v=substr(binstring,i,1)
   if(v !~ /[xX]/)
   {
     set_list[s]=v      # used to build the set of signals forced between 
			# successive s lines
   }

   if(v ~ /[01zZ]/) write_pwl_pair(s,v,res,vhi,vlo,slp)
  }
 }
 else  ## not a bus
 {

  if($3 ~ /^~1/)  # inversion flag on signal assignment 20140623
  {
    $3=0
  }
  if($3 ~ /^~0/)  # inversion flag on signal assignment 20140623
  {
    $3=1
  }
 
  if($3 !~ /^[xX]$/)
  {
   $3=$0
   sub(/^[\t ]*set[\t ]*[^ \t ]+[ \t]*/,"",$3)  #20150918
   sub(/[ \t]*$/,"",$3)

   set_list[$2]=$3  # used to build the set of signals forced between
                    # successive s lines
   write_pwl_pair($2, $3, res, vhi, vlo,slp)
  }
  if(format=="ncsim" && time >=0)
  {
   sig= "." $2
   gsub(/\./,":",sig)
   if($3 !~ /[zZ]/)
   {
    if($3 ~ /[0-9]*\.[0-9]+/)
    {
     print "force " sig " = {" $3 "}" >file
    }
    else if($3 ~ /[0-9]+%/)
    {
     sub(/%/,"",$3)
     print "force " sig " = {" $3 "}" >file
    }
    else
    {
     print "force " sig " = {'" $3 "'}" >file
    }
   }
   else
   {
    print "release " sig >file
   }
  }

 }
}

END{
 print  total_time unit "s"> "inutile.simulationtime"
}

function end_file(){
 if(time > total_time) total_time=time
 if(format ~ /^eldo/)
 {
  print "**************************************" > file
  print "* SPICE STIMULI FILE">file
  print "* generated from " filename > file
  print "**************************************\n" > file
  for(i in signalname)
  {
    use_z = ( (format !~ /_simple/) && signalz[i] ) 
    ix=i
    gsub(/\./,"_",ix)
    if(!use_z) 
      printf simplesignalname[i]>file
    else
      printf signalname[i]>file
    for(j=1;j<=signal[i,"n"];j++)
      printf "%.9g%c  %s ",signal[i,"time",j],unit,signal[i,"value",j] >file
    printf "\n">file
    if(use_z)
    {
     if(variant!="hspice")
       # 20100224 quotes instead of parenthesis used for value=....
       # as per new eldo syntax. this makes it compatible w eldo and hspice
       print "R" ix " XX" ix " " i " value='v(VR" ix ")'">file
     else 
       print "R" ix " XX" ix " " i " 'v(VR" ix ")'">file
     printf "%s",signalres[i]>file
     for(j=1;j<=signal[i,"n"];j++)
       printf "%.9g%c %s ",signal[i,"time",j],unit,signalres[i,"value",j] >file
     printf "\n">file
    }
  }
  if(use_z || create_eldo_bus )
   for(i in buswidth)
   {
     if( variant != "hspice")  {
       printf ".setbus " i " " >file
       for(j=1;j<=buswidth[i];j++) printf bus[i,j] " " >file;printf "\n">file
       print ".plotbus " i " vth=" halfvoltage >file
     }
   }
 }
 else if(format ~ /^pow.*mill/)
 {
  if(unit=="u") mult=1000
  else if(unit=="n") mult=1
  else {mult=1; print "ERROR: UNKNOWN UNIT">file}
  print "; POWERMILL VECTOR FILE">file
  print "type vec">file
  printf "signal ">file
  for(i=1;i<=signals;i++)  
    printf signal_list[i] " " >file
  printf "\n" >file
  print "slope " slope*mult>file
  print "high " voltage>file
  print "low " ground >file
  printf "radix ">file
  for(i=1;i<=signals;i++)  printf "1">file
  printf "\n" >file
  for(k=1;k<=signals;k++)
  {
   i=signal_list[k]
   n=signal[i,"n"]
   signal[i,"time",1]=0
   for(j=1;j<=n;j+=2)
   {
    x=signal[i,"time",j]
    if(j>1) x-=slp
    v=signal[i,"value",j]
    if(v>halfvoltage) vv="1"
    if(v==halfvoltage) vv="z" 
    if(v<halfvoltage) vv="0"
    if(output[x]=="") output[x]=chars(signals,"_")
    output[x]=setbit(output[x],vv,signals-k)
   }
  }
  lines=0
  for(i in output)
  {
   lines++
   line[lines]=output[i]
   key[lines]=i*mult
  }
  hsort(line,key,lines)
  powmillstate=chars(signals,"z")
  for(i=1;i<=lines;i++)
  {
   line[i]=preserve_state(line[i],powmillstate,signals)
   print key[i],line[i]>file
   powmillstate=line[i]
  }
 }
 else if(format=="new_powermill")
 {
  print "; POWERMILL STIMULI FILE">file
  for(k=1;k<=signals;k++)
  {
   i=signal_list[k]
   n=signal[i,"n"]
   for(j=1;j<=n;j+=2)
   {
    x=signal[i,"time",j]
    if(j>1) x-=slope
    v=signal[i,"value",j]
    imped=signalres[i,"value",j]
    if(j>=3 && signalres[i,"value",j-2]!=roff) print "rel_node_v no=" i " ti=" x unit>file
    else if(j==1 ) print "rel_node_v no=" i " ti=" x unit>file

# <<<<< powermill 5.3 bug: release and force cannot be at same time and release is mandatory before force
#                                                                ||
#                                                                \/
    if(imped!=roff)  print "force_node_v no=" i " v=" v " ti=" x+slope unit>file
   }
  }
 }
 else if(format=="relative_powermill")
 {
  if(powmill_statement>0)
  {
   for(i in set_list)
    print "rel_node_v no=" i>file
   print "\ncont " slope  unit "\n">file
  }
  for(i in set_list)
  {
     v=set_list[i]
     if(v ~ /^[10]$/) { vv=(v==1? voltage:ground ) }
     else if(v ~ /^[zZ]$/) {vv=halfvoltage}
     else {vv = v}
     if(v !~ /[zZ]/) print "force_node_v no=" i " v=" vv>file
  }
 }
 powmill_statement++
 break_lines(file)
 print_dump(file)
}

# 20100301 added vhi, vlo
function write_pwl_pair( name, value,res, vhi, vlo,slope,   timex, namex,vv,vv1,vv2,tt2,tt1,halfvoltageloc,i,v,n,pwlres,pwltmpres,pwltmp2 )
{
 if(!halfvoltageset) { # 20170405
   if(is_number(vhi) && is_number(vlo)) {
     halfvoltageloc=(vhi+vlo) / 2
   } else {
     halfvoltageloc = "'(" vhi "+" vlo ")/2'"
   }
 } else {
   halfvoltageloc = halfvoltage
 }

  if(res != ron) signalz[name] = 1
  if(value ~ /^[10]$/) {v= ( value==1 ? vhi : vlo )  ; pwlres=res}
  else if(value ~ /^[zZ]$/) {signalz[name] = 1; v=halfvoltageloc; pwlres=roff}
  else {v = value; pwlres=res}
  namex=name
  gsub(/\./,"_",namex)
  if(!(name in signalname) ) 
  {
   timex=time < 0 ? 0: time
   signal_list[++signals]=name
   signalname[name]="V" namex " XX" namex " " gndnode " PWL " 
   simplesignalname[name]="V" namex " " name " " gndnode " PWL " 
   signal[name,"time",1]= timex
   signal[name,"value",1]= v
   signal[name,"n"]= 1
   #                                         20170810 0 instead of gndnode
   #                                          |
   signalres[name]="VR" namex " VR" namex " " 0 " PWL " 
   signalres[name,"value",1]= pwlres
  }
  if(signal[name,"value", signal[name,"n"] ]!=v || signalres[name,"value", signal[name,"n"] ]!=pwlres)
  {
   timex=time < 0 ? 0: time
   if(timex==0)
   {
    n = signal[name,"n"]=1
    signal[name,"value",n]= v
    signal[name,"time",n]=0
    signalres[name,"value",n]= pwlres
   }
   ## 20151112
   else if(timex+slope == signal[name,"time",signal[name,"n"]] ) {
    n = signal[name,"n"] 
    signal[name,"value",n]= v
    signalres[name,"value",n]= pwlres
   } 
   else
   {
     if( timex > signal[name,"time",signal[name,"n"]] )
     {
      n = (signal[name,"n"] += 1)
      signal[name,"time",n]=timex
      signal[name,"value",n]= signal[name,"value",n-1]
      signalres[name,"value",n]= signalres[name,"value",n-1]
     }

     ## 20100302 interpolation if signal change occurs before conpleting previous transaction
     else if(timex < signal[name,"time",signal[name,"n"]] ){
      n = signal[name,"n"] 
      vv1 = signal[name,"value",n-1] 
      vv2 = signal[name,"value",n] 
      tt1 = signal[name,"time",n-1] 
      tt2 = signal[name,"time",n] 
      vv = vv1 + (vv2-vv1)/(tt2-tt1) * (timex-tt1)
      signal[name,"time",n]=timex
      signal[name,"value",n]= vv
     }
     ## /20100302

     if( timex+slope > signal[name,"time",signal[name,"n"]] )
     {
      n = (signal[name,"n"] += 1)
      signal[name,"time",n]=timex+slope
      signal[name,"value",n]= v
      signalres[name,"value",n]= pwlres
     } 
   }
  }
}

function hex(n,  i,dec,ch)
{
 dec=0
 for(i=1;i<=length(n);i++)
 {
  ch=toupper(substr(n,i,1))
  if(val[ch]>=val["A"]) ch=val[ch]-val["A"]+10
  else if(val[ch]>=val["0"]) ch=val[ch]-val["0"]
  dec=dec*16+ch
 }
 return dec
}

function bin_invert(s     ,a,i,ch  )
{
 a=""
 for(i=1;i<=length(s);i++)
 {
  ch=substr(s,i,1)
  if(ch=="1") ch="0"
  else if(ch=="0") ch="1"
  a=a ch
 }
 return a
}

function tobinary(n,width,invert,         bin,i,ch,binbegin)
# transform an input numeric string in the allowed format (see help)
# to a binary string, using only 1,0,z, x
{
 bin=""
 binbegin=0
 for(i=1;i<=length(n);i++)
 {
  ch=substr(n,i,1)
  if(ch =="'") 	# since awk program is quoted, quotes must be escaped
  {
   binbegin=!binbegin;continue
  }
  else if(binbegin && ch ~/[01zZ]/)
   bin=bin tolower(ch)
  else if(binbegin && ch ~/[xX]/)
   bin=bin "x"
  else if(ch ~ /[0-9a-fA-F]/)
   bin=bin binary(hex(ch),4)
  else if(ch ~ /[zZ]/)
   bin=bin "zzzz"
  else if(ch ~ /[xX]/)
   bin=bin "xxxx"

  else continue

 }
 bin=substr(bin,1+length(bin)-width)
 if(invert) return bin_invert( chars(width-length(bin),"x") bin )
 else return chars(width-length(bin),"x") bin
}

function binary( n, width,          a,i,str)
# builds a <width> digit binary representation of <n> encoded in a string
{
 for(;;)
 {
  a[i++]=(n%2 ? "1" : "0")
  n=int(n/2)
  if(!n) break
 }
 for(i=width-1;i>=0;i--)
 {
  str = str (a[i] ? a[i]:"0")
 }
 return str
}
 
function binval(str,       i,n)
# returns the decimal value of the binary number encoded in "str"
{
 for(i=1;i<=length(str);i++)  n=2*n + substr(str,i,1)
 return n
}

# heap sort, this is a N*log2(N) routine
function hsort(array,ra,n,    rarray,l,j,ir,i,rra)
{
 if(n<=1) return    # bug fix: if only one element in array never returns
 l=int(n/2)+1
 ir=n
 for(;;)
 {
  if(l>1)
  {
   rra=ra[--l]
   rarray=array[l]
  }
  else
  {
   rra=ra[ir]
   rarray=array[ir]
   ra[ir]=ra[1]
   array[ir]=array[1]
   if(--ir==1)
   {
    array[1]=rarray
    ra[1]=rra
    return
   }
  }
  i=l
  j=l*2
  while(j<=ir)
  {
   if(j<ir && ra[j] <ra[j+1]) ++j
   if(rra<ra[j])
   {
    array[i]=array[j]
    ra[i]=ra[j]
    j+=(i=j)
   }
   else j=ir+1
  }
  ra[i]=rra
  array[i]=rarray
 }
}
   
function setbit( string, replace, pos,        reversepos,len)
# returns <string> with bits starting at position <pos>
# set to <replace>, position 0 being relative to the rightmost bit
{
 reversepos=length(string)-pos
 len = length(replace)
 return substr(string,1,reversepos-len) replace substr(string,reversepos+1)
}

function chars(n,c,   i,str)
{
 str=""
 for(i=1;i<=n;i++) str=str c
 return str
}

function preserve_state(new,old,n,    nn,oo,i,str)
{
 str=""
 for(i=1;i<=n;i++)
 {
  nn=substr(new,i,1)
  oo=substr(old,i,1)
  if(nn=="_") str=str oo
  else       str=str nn
 }
 return str
}

function reset_all()
{
 signals=0
 file=""
 dumpline=0
 powmill_statement=0

 delete signal_list
 delete signalname
 delete simplesignalname
 delete signal
 delete signalres
 delete output
 delete line
 delete key
 delete dump
 time=0
 origin=0
 
}

function break_lines(filename,    quote, j, lines, line, l, i, count, style )
{
 lines=0
 style="+"
 close(filename)
 while ((getline line[++lines] < filename) > 0);
 close(filename)

			# 20120410
 lines = lines + 0	# do NOT remove, solves a problem with gawk 3.0.5 where a variable used as 
			# array index is "magically" converted to string


 quote=0
 for(l=1;l<=lines;l++)
 {
  $0=line[l]
 
  if($0 ~ /^\* ELDO/) {style="+"}
  if($0 ~ /^; POWERMILL/) {style="\\"}
 
  count=0
  if(NF==0) print "">filename


  # 20150918 dont break quoted expressions
  for(i=1;i<=NF;i++)
  {
   for(j=1; j<=length($i); j++) {
     if(substr($i,j,1)=="'") quote = !quote
   }
   # print "+++ " quote, $i, count
     
   printf "%s ", $i>filename
   count++
   if(( count>=15 && !quote)  || i==NF)
   {
    # print "break", quote, count, NF
    if(style=="+")
    {
     printf "\n">filename
     if(i < NF) printf "+">filename
     count=0
    }
    else if(style=="\\")
    {
     if(i < NF) printf "\\">filename
     printf "\n">filename
     count=0
    }
   }
  }
 }
}

function is_number(i) # 20170405
{
  return i+0==i
}

function s_i(s)
{
 sub(/^.*\[/,"",s)
 sub(/\].*$/,"",s)
 return s
}

function print_dump(filename,   i)
{
 for(i=1;i<=dumpline;i++) print dump[i] >filename
}
