#!/usr/bin/awk -f
##
## Does its best to group array of instances in a spice netlist
## XA[2] ...
## XA[1] ...
## XA[0] ...
## --> XA[2:0] ...
## instances that can be grouped must be of subckts with no bussed ports
## run this script before importing a netlist into xschem with make_sch_from_spice.awk
##
## usage: ~/xschem/netlist_compactor.awk netlist > netlist.compact
##
## stefan, 20161226

BEGIN{

  #print compact_label("A[3],A[3],A[3],A[4],A[5],A[6],A[6],A[6],A[6],A[8],A[7],A[7],A[9],A[8],A[7],A[11],A[13]")
  ## 3*A[3],A[4:6],3*A[6],A[8:7],A[7],A[9:7],A[11],A[13]
  #exit

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

   for(i=0;i<netlist_lines; i++) {
     $0=netlist[i]
     if(toupper($1) ~/\.SUBCKT/) {
       bussed_pins[$2]=0
       for(j=3;j<=NF;j++) {
         if($j ~/\[/) bussed_pins[$2]=1
         if($j~/=/) break
       }
     }
   }
   for(i=0;i<netlist_lines; i++) {
     $0=netlist[i]
     compact()
   }
}

function compact(        i,j,num_inst,f1,nf,nfp,curr_line, prev_line)
{
  if( ($1 ~ /^\*/) || ($0 ~/^[ \t]*$/) ) {
    print
    return
  }

  f1 = toupper($1)
  if(f1 ~ /^\.SUBCKT/) {
     print
     return
  }

  if($1 ~/^[xX]/) {
    for(i=2;i<=NF;i++) {
      if($i ~/=/) {
        subckt=$(i-1)
        break
      }
      if(i==NF) subckt=$i
    }
  }
  if( ($1 ~/^[xX]/) && bussed_pins[subckt]) {
    print
    return
  }

  if(f1 ~/^\.ENDS/) {
    num_inst = asorti(arr, dest, "comp")
    for(i=1;i<=num_inst;i++) {
      nf = split(arr[dest[i]], curr_line_arr)
      if(i>1) {
        ##  ---------20170308----------
        if( (curr_line_arr[1] ~/^X/) && lab_name(curr_line_arr[1]) == lab_name(prev_line_arr[1]) ) {
          for(j=1;j<=nf;j++) {
            if( j<nf && (prev_line_arr[j+1] !~ /=/) ) {
              prev_line_arr[j] = prev_line_arr[j] "," curr_line_arr[j]
            }
          }
        } else {
          for(j=1;j<=nfp;j++) {
            #if( j<nfp && (prev_line_arr[j] !~ /=/) ) {
            if( j==1) {
              printf "%s ", compact_label(prev_line_arr[j])
            } else {
              printf "%s ", prev_line_arr[j]
            }
          }
          printf "\n"
          nfp = split(arr[dest[i]], prev_line_arr)
        }
      } else {
        nfp = split(arr[dest[i]], prev_line_arr)
      }

    }
    if(num_inst) {
      for(j=1;j<=nf;j++) {
        #if( j<nf && (prev_line_arr[j] !~ /=/) ) {
        if( j==1) {
          printf "%s ", compact_label(prev_line_arr[j])
        } else {
          printf "%s ", prev_line_arr[j]
        }
      }
      printf "\n"
    }

    print
    delete arr
    delete dest
    return
  }
  arr[$1]=$0
}

function abs(a)
{
 return a>0 ? a: -a
}


# 1  2    3    4    5 6 7     8    9    10   11   12
# PP A[3] A[2] A[1] B C K[10] K[9] K[5] K[4] K[3] K[1]
function compact_label(str,        a, b, ar, ret,start,i)
{
  # print "compact_label_str(): str=" str
  a=1
  b=split(str, ar,",")
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
      else if(lab_name(ar[i])!=lab_name(ar[i-1])) {                     # lab basename changed
        if(start<i-1 && lab_index(ar[start]) == lab_index(ar[i-1]) )
          ret = ret (i-start) "*" ar[i-1] ",";
        else if(start<i-1)
          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) ":" lab_index(ar[i-1]) "],"
        else
          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) "],"
        start=i
      }

      else if( lab_index(ar[i]) != lab_index(ar[i-1])-1 &&                # index not equal, +1,-1
               lab_index(ar[i]) != lab_index(ar[i-1])+1 ) {               # to previous
        if(start<i-1 && lab_index(ar[start]) == lab_index(ar[i-1]) )
          ret = ret (i-start) "*" ar[i-1] ",";
        else if(start<i-1)
          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) ":" lab_index(ar[i-1]) "],"
        else
          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) "],"
        start=i
      }

      else if( lab_index(ar[start])!=lab_index(ar[i]) &&           # range count != element count
               abs(start-i)!=abs(lab_index(ar[start])-lab_index(ar[i]))) {
        if(start<i-1 && lab_index(ar[start]) == lab_index(ar[i-1]) )
          ret = ret (i-start) "*" ar[i-1] ",";
        else if(start<i-1)
          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) ":" lab_index(ar[i-1]) "],"
        else
          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) "],"
        start=i
      }

#      else if( lab_index(ar[i]) != lab_index(ar[start])  ) {
#        if(start<i-1 && lab_index(ar[start]) == lab_index(ar[i-1]) )
#          ret = ret (i-start) "*" ar[i-1] ",";
#        else if(start<i-1)
#          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) ":" lab_index(ar[i-1]) "],"
#        else
#          ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) "],"
#        start=i
#      }


    }
  }
  if(ar[b] !~ /\[/)  {
    if(start < b)  ret = ret (b-start+1) "*" ar[b]
    else ret = ret ar[b]
  }
  else if(start<b && lab_index(ar[start]) == lab_index(ar[b]))
    ret = ret (b-start+1) "*" ar[b]
  else if(start<b)
    ret = ret lab_name(ar[start]) "[" lab_index(ar[start]) ":" lab_index(ar[b]) "]"
  else
    ret = ret lab_name(ar[b]) "[" lab_index(ar[b]) "]"

  # print "compact_label_str(): ret=" ret
  return ret
}


function lab_name(lab)
{
 if(lab ~/[^\[\]]+\[[0-9]+\]$/) {
   sub(/\[.*/,"",lab)
   return lab
 } else {
   return lab
 }
}


function lab_index(lab)
{
 if(lab ~/[^\[\]]+\[[0-9]+\]$/) {
   sub(/.*\[/,"",lab)
   sub(/\].*/,"",lab)
   return lab+0
 } else {
   return ""
 }
}


function comp(a,va,b,vb)
{
 if( lab_name(a) == lab_name(b) ) return lab_index(a) < lab_index(b)
 else return lab_name(a) < lab_name(b)
}

