#!/bin/sh
gawk '

BEGIN{
 
#  ar["a", 1] = "A[3]"
#  ar["a", 2] = "A[3]"
#  ar["a", 3] = "A[3]"
#  ar["a", 4] = "A[4]"
#  ar["a", 5] = "A[5]"
#  ar["a", 6] = "A[6]"
#  ar["a", 7] = "A[6]"
#  ar["a", 8] = "A[6]"
#  ar["a", 9] = "A[6]"
#  ar["a",10] = "A[8]"
#  ar["a",11] = "A[7]"
#  ar["a",12] = "A[7]"
#  ar["a",13] = "A[9]"
#  ar["a",14] = "A[11]"
#  ar["a",15] = "A[13]"
#
#  print compact_label("a",ar, 1,15)
#  exit



 # list of cmos_t9 symbol pin coordinates, generated with build_cmoslib.awk


   all_signals=""

##########################   JOIN   ##########################
   netlist_lines=0
   first=1
   while(err= (getline l) >0) {
     gsub(/</,"[",l)
     gsub(/>/,"]",l)
     if(first) {
       $0=l
       first=0
     }
     else if(l !~/^\+/) {
       netlist[netlist_lines++]=$0
       $0=l
     }
     else $0 = $0 " " substr(l,2)
   }
   netlist[netlist_lines++]=$0
########################## END JOIN ##########################

   skip=0
   for(i=0;i<netlist_lines; i++) { 
     $0=netlist[i]
     process_subckts()
   }

   skip=0
   for(i=0;i<netlist_lines; i++) { 
     $0=netlist[i]
     process()
   }

}



function process_subckts(         i,name)
{
 if(skip==1 && toupper($1) ==".ENDS") { skip=0; return } 
 if(skip==1) return
 if(toupper($1) ==".SUBCKT") {
   curr_subckt=$2
   print " process_subckt(): curr_subckt=|" curr_subckt "|"
   if(curr_subckt in cell) {print " process_subckt(); skipping " curr_subckt ; skip=1; return }
   subckt[curr_subckt]=1
   template=0
   for(i=3;i<=NF;i++) {
     if($i~ /=/) { template=i; break}
     pin_ar[curr_subckt,i-2]=$i
   }
   pin_ar[curr_subckt,"n"]=i-3
   pin_ar[curr_subckt,"template"] = get_template(template) 
   print "\n\n\n process_subckt() : " curr_subckt "--> " 
   for(i=1; i<= pin_ar[curr_subckt,"n"]; i++) printf "%s ", pin_ar[curr_subckt,i]; printf "\n"
 }
 else if(toupper($1) ~ /^\*\.PININFO/) {
   for(i=2;i<=NF;i++) {
     name=$i; sub(/:.*/,"",name)
     if($i ~ /:I$/ ) pin_ar[curr_subckt, "dir", name] = "I"
     else if($i ~ /:O$/ ) pin_ar[curr_subckt, "dir", name] = "O"
     else if($i ~ /:B$/ ) pin_ar[curr_subckt, "dir", name] = "B"
     else { print "ERROR" ; exit} 
   }
 }
 else if(toupper($1) ~ /^\*\.(I|O|IO)PIN/) {
  if($1 ~ /^\*\.ipin/) { pin_ar[curr_subckt, "dir", $2] = "I" } 
  else if($1 ~ /^\*\.opin/) { pin_ar[curr_subckt, "dir", $2] = "O" } 
  else if($1 ~ /^\*\.iopin/) { pin_ar[curr_subckt, "dir", $2] = "B" } 
 }
}

function get_template(t,         templ, i) 
{
 templ=""
 if(t) for(i=t;i<=NF;i++) {
  templ = templ $i " " 
 }
 return templ
}


   
function process(         i,name,param)
{
 print "process(): skip = "  skip " --> " $0
 if(skip==1 && toupper($1) ==".ENDS") { skip=0; return }
 if(skip==1) return
 if(toupper($1) ==".SUBCKT") {
   curr_subckt=$2
   if(curr_subckt in cell) {print "process(): skipping " curr_subckt ; skip=1; return }
   sp=ip=op=n_pin=0
   all_signals=""
   delete net_ar

 }
 if(toupper($1) ~ /^.ENDS/) {


   compact_pinlist( "" , curr_subckt)
   print "----------------------------------------------------------"
     
   for(i=1;i<= dir_ret["n"] ; i++) {
     print dir_ret[i] "   " pin_ret[i]
   }   
   print "\n\n"
         
   print_sch(curr_subckt, dir_ret, pin_ret)
   print "----------------------------------------------------------"


   if(all_signals !="") {
     print all_signals > (curr_subckt ".sch")
   }
   close((curr_subckt ".sch"))
 }
 if(toupper($1) ~ /^X/) {
   inst=substr($1,2)
   sub(/\[/,"_",inst)
   sub(/\]/,"_",inst)
   param=0
   for(i=2;i<=NF;i++) {
     sub(/!$/,"",$i)   # remove ! on global nodes
     if(i<NF && $(i+1) ~ /=/) {
       if(!param) param = i+1
       inst_sub=$i
       if(!(inst_sub in cell ) && !(inst_sub in subckt)) {print "ERROR: " inst_sub " NOT DECLARED" ; exit}
       break
     }
     else if($i =="/" ) {
       if(i==NF) {print "ERROR: garbled netlist line : " $0; exit}
       inst_sub=$(i+1)
       if(!param) param = i+2
       if(!(inst_sub in cell ) && !(inst_sub in subckt)) {print "ERROR: " inst_sub " NOT DECLARED" ; exit}
       break
     }
     else if(i==NF) {
       inst_sub=$i
       if(!(inst_sub in cell ) && !(inst_sub in subckt)) {print "ERROR: " inst_sub " NOT DECLARED" ; exit}
       break
     }
     net_ar[inst,i-1] = $i
   }
   net_ar[inst,"n"] = i-2
   print "  net_ar[inst,n]= "  net_ar[inst,"n"]
   compact_pinlist(inst,inst_sub)
   print inst " - " inst_sub  " --> "
   for(i=1;i<= dir_ret["n"] ; i++) {
     print "  dir_ret " i " ------> " dir_ret[i] "   " pin_ret[i] " <-- " net_ret[i]
   }
   print "\n\n"
   param = get_param(param) 
   print_signals( inst, inst_sub, param, pin_ret, dir_ret, net_ret )
 }
}

function get_param(i,               param,j)
{
  param=""
  if(i) for(j=i;j<=NF;j++) {
    param = param $j " "
  }
  return param
}

function compact_pinlist(inst,inst_sub                  ,i,ii,base,curr,curr_n,np)
{
 delete pin_ret
 delete net_ret
 delete dir_ret
 
 np=pin_ar[inst_sub,"n"]
 print " compact_pinlist: np= " np
 if(np) {
   ii=1
   for(i=1;i<=np;i++) {
     base =lab_name( pin_ar[inst_sub,i] )
     if(i==1) {curr=base; curr_n=i}
     else { 
       if(base != curr) {
         pin_ret[ii] = compact_label(inst_sub,pin_ar,curr_n,i-1)
         if(inst) net_ret[ii] = compact_label(inst,net_ar,curr_n,i-1)



         dir_ret[ii] = pin_ar[inst_sub,"dir",pin_ar[inst_sub,i-1] ]
         print " compact_pinlist: dir_ret[" ii "]= " dir_ret[ii]
         ii++
         curr=base;curr_n=i
       }
     }
   }
   pin_ret[ii] = compact_label(inst_sub, pin_ar,curr_n,np)
   if(inst) net_ret[ii] = compact_label(inst, net_ar,curr_n,np)
   dir_ret[ii] = pin_ar[inst_sub,"dir",pin_ar[inst_sub,np] ]
   print " compact_pinlist: dir_ret[" ii "]= " dir_ret[ii]
   pin_ret["n"] =  dir_ret["n"] = ii
   if(inst) net_ret["n"] = ii
 }
}

# 1  2    3    4    5 6 7     8    9    10   11   12
# PP A[3] A[2] A[1] B C K[10] K[9] K[5] K[4] K[3] K[1]
function compact_label(name, ar,a,b,        ret,start,i)
{
  ret=""
  for(i=a;i<=b;i++) {
    if(i==a) {start=a}
    else {
      if(ar[name,i-1] !~ /\[/)  {
        if(ar[name,i-1] != ar[name,i]) {
          if(start < i-1) { ret = ret (i-start) "*" ar[name,i-1] ","; start=i }
          else {ret = ret ar[name,i-1] ","; start=i }
        }
      }
      else if(lab_name(ar[name,i])!=lab_name(ar[name,i-1]) || 			# lab basename changed

          ( lab_index(ar[name,start])!=lab_index(ar[name,i]) && 		# range count != element count
            abs(start-i)!=abs(lab_index(ar[name,start])-lab_index(ar[name,i]))) ||

          ( lab_index(ar[name,i]) != lab_index(ar[name,i-1])-1 && 		# index not equal, +1,-1
            lab_index(ar[name,i]) != lab_index(ar[name,i-1])+1 &&               # to previous
            lab_index(ar[name,i]) != lab_index(ar[name,start])   ) ) {
        if(start<i-1 && lab_index(ar[name,start]) == lab_index(ar[name,i-1]) )
          ret = ret (i-start) "*" ar[name,i-1] ",";
        else if(start<i-1) 
          ret = ret lab_name(ar[name,start]) "[" lab_index(ar[name,start]) ":" lab_index(ar[name,i-1]) "],"
        else
          ret = ret lab_name(ar[name,start]) "[" lab_index(ar[name,start]) "],"
        start=i
      }
    }
  }
  if(ar[name,b] !~ /\[/)  {
    if(start < b)  ret = ret (b-start+1) "*" ar[name,b]
    else ret = ret ar[name,b]
  }
  else if(start<b && lab_index(ar[name,start]) == lab_index(ar[name,b]))   
    ret = ret (b-start+1) "*" ar[name,b] 
  else if(start<b)   
    ret = ret lab_name(ar[name,start]) "[" lab_index(ar[name,start]) ":" lab_index(ar[name,b]) "]"
  else
    ret = ret lab_name(ar[name,b]) "[" lab_index(ar[name,b]) "]"
  return ret
}

function lab_name(lab)
{
 sub(/\[.*/,"",lab)
 return lab
}


function lab_index(lab)
{
 sub(/.*\[/,"",lab)
 sub(/\].*/,"",lab)
 return lab+0
}

function print_sch(schname, dir, pin,
           n_pin, ip, op,cellname,        y,x,i, pin_dir,sch_x_offset)
{
 cellname=schname
 schname = schname ".sch"
 print " --> print_sch called for: " schname
 print "ENTITY " cellname " is"  > schname
 print "PORT(" > schname
 n_pin = dir["n"]

 ip=op=0
 for(i=1;i<=n_pin; i++) {
   if(dir[i] == "I" ) ip++
   else if(dir[i]=="O" || dir[i]=="B") op++
   else {print "ERROR: print_sch(): undefined dir[]"; exit}
 }
 
 for(i=1;i<=n_pin;i++)
 {
  pin_dir=dir[i]

  if(pin_dir=="I")
  {
   printf "   %s : IN STD_LOGIC ", pin[i] > schname
  }
  if(pin_dir=="O")
  {
   printf "   %s : OUT STD_LOGIC ", pin[i] > schname
  }
  if(pin_dir=="B")
  {
   printf "   %s : INOUT STD_LOGIC ", pin[i] > schname
  }
  if(i<n_pin) printf ",\n" > schname
  else        printf "\n" > schname
 }
 print ");\nEND " cellname " ;\n" > schname
}



function print_signals( inst_name, component_name, param, pin,dir,net,
         n_pin,n_dir,n_net,
	 i, curr_dir)  #local vars
{

 n_pin=pin["n"]
 n_net=net["n"]
 n_dir=dir["n"]
 
 print " print_signals() : component_name = ", component_name
 if(n_dir != n_pin) { print " n_dir vs n_pin mismatch: inst / comp = " inst_name " / " component_name ; exit }
 if(n_net != n_pin) { print " n_net vs n_pin mismatch: inst / comp = " inst_name " / " component_name ; exit }

 all_signals=all_signals inst_name " : " component_name "\nPORT MAP(\n" 

 for(i=1;i<=n_net;i++)
 {
   curr_dir=dir[i]
   all_signals = all_signals "    " pin[i] " => " net[i] 

   if(i<n_net) all_signals = all_signals ",\n"
   else        all_signals = all_signals "\n"
 }
 all_signals=all_signals ");\n\n"

}  



#------------------------------

function abs(a) 
{
 return a>0 ? a: -a
}

function format_translate(s,             str,n,i,ss,sss) 
{
 str=""
 n=split(s,ss)
 for(i=1;i<=n;i++) {
   if(ss[i] ~ /.+=.+/) {
     split(ss[i],sss,"=") 
     ss[i] = sss[i] "=@" sss[1]
   }
   str = str ss[i]
   if(i<n) str=str " " 
 }
 return str
}


' $@
