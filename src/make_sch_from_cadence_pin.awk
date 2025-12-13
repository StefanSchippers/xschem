#!/bin/sh

# SLVMEASURE inputOutput
# HDTSENSE<3> output
# HDTSENSE<2> output
# HDTSENSE<1> output
# HDTSENSE<0> output
# HDTSENSEOK output
# HDTSENSE_BUSY output
# HDBGIREFOK input
# HLBGIREF input

gawk '
#-----------------------------------------------------
FNR == 1 {
    if (_filename_ != "")
        endfile(_filename_)
    _filename_ = FILENAME
    print "processing: " FILENAME >"/dev/stdout"
    beginfile(FILENAME)
}

END  { endfile(_filename_) }
#-----------------------------------------------------


function beginfile(fname)
{
 ip=0; op=0; i=0
 delete pin
 delete dir
 delete pin_ret
 delete dir_ret
}

{
 gsub(/</,"[",$0)
 gsub(/>/,"]",$0)
}

/ inputOutput *$/{
 op++;
 i++
 pin[i] = $1
 dir[i] = "B"
}

/ output *$/{
 op++;
 i++
 pin[i] = $1
 dir[i] = "O"
}

/ input *$/{
 ip++;
 i++
 pin[i] = $1
 dir[i] = "I"
}

function endfile(fname)
{
 sub(/\/pin\/text\.txt/,"",fname)
 sub(/.*\//,"",fname)
 fname = fname ".sch"

 dir["n"] = pin["n"] = i
 compact_pinlist(pin,dir)

 print_sch(fname,dir_ret,pin_ret)
}


function print_sch(schname, dir, pin,
           n_pin, ip, op,        iii,ooo,y,x,i, pin_dir,sch_x_offset, space)
{
 iii=0
 ooo=0

 print "V {}\nG {}" > schname
 n_pin = dir["n"]

 ip=op=0
 for(i=1;i<=n_pin; i++) {
   if(dir[i] == "I" ) ip++
   else if(dir[i]=="O" || dir[i]=="B") op++
   else {print "ERROR: print_sch(): undefined dir[] i=" i; exit}
 }

 y=0
 x=-40
 space=20
 sch_x_offset=230

 for(i=1;i<=n_pin;i++)
 {
  pin_dir=dir[i]

  if(pin_dir=="I")
  {
   iii++
   printf "C {devices/ipin} " (x+sch_x_offset) " " (y+iii*space) " 0 0 " \
         " {name=p" p_pin++ " lab=" pin[i] " " >schname
   printf "}\n" >schname
  }
  if(pin_dir=="O")
  {
   ooo++
   printf "C {devices/opin} " (-x+sch_x_offset) " " (y+ooo*space) " 0 0 " \
         " {name=p" p_pin++ " lab=" pin[i] " " >schname
   printf "}\n" >schname
  }
  if(pin_dir=="B")
  {
   ooo++
   printf "C {devices/iopin} " (-x+sch_x_offset) " " (y+ooo*space) " 0 0 " \
         " {name=p" p_pin++ " lab=" pin[i] " " >schname
   printf "}\n" >schname
  }
 }
}


#------------------------

function compact_pinlist(pin, dir                 ,i,ii,base,curr,curr_n,np)
{
 delete pin_ret
 delete net_ret
 delete dir_ret

 np=pin["n"]
 if(np) {
   ii=1
   for(i=1;i<=np;i++) {
     base =lab_name( pin[i] )
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
      else if(lab_name(ar[i])!=lab_name(ar[i-1]) ||
              ( lab_index(ar[i]) != lab_index(ar[i-1])-1 &&
              lab_index(ar[i]) != lab_index(ar[i-1])+1) ) {
        if(start<i-1)
          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) ":" lab_index(ar[i-1]) "],"
        else
          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) "],"
        start=i
      }
    }
  }
  if(ar[b] !~ /\[/)  {
    if(start < b)  ret = ret (b-start+1) "*" ar[b]
    else ret = ret ar[b]
  }
  else if(start<b)
    ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) ":" lab_index(ar[b]) "]"
  else
    ret = ret lab_name(ar[b]) "[" lab_index(ar[b]) "]"
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

#------------------------





' $@

