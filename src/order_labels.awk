#!/usr/bin/awk -f
#------------------------
BEGIN{
  i=0
}
NF=2 {
  i++
  pin[i]=$1
  
  # 20111116
  dir[$1] = $2
} 
END{
  pin["n"]=i
  hsort(pin, pin["n"])

  compact_pinlist(pin)
  for(i=1;i<=pin_ret["n"];i++){
    a=pin_ret[i]
#    gsub(/\[/,"<",a)
#    gsub(/\]/,">",a)
    printf "%s  %s\n", a, dir_ret[i]
  }
}


function compact_pinlist(pin                  ,i,ii,base,curr,curr_n,np)
{
 delete pin_ret
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
         dir_ret[ii] = dir[pin[i-1]]  # 20111116
         ii++
         curr=base;curr_n=i
       }
     }
   }
   pin_ret[ii] = compact_label(pin,curr_n,np)
   dir_ret[ii] = dir[pin[np]] # 20111116
   pin_ret["n"] = ii
   
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

# heap sort, this is a N*log2(N) routine
# sort array[1...n] in ascending order using key
function hsort(array,n,    rarray,l,j,ir,i,rra)
{
 if(n<=1) return # this is a bugfix, on one-element array never ends.
 l=int(n/2)+1
 ir=n
 for(;;)
 {
  if(l>1)
  {
   rarray=array[--l]
  }
  else
  {
   rarray=array[ir]
   array[ir]=array[1]
   if(--ir==1)
   {
    array[1]=rarray
    return
   }
  }
  i=l
  j=l*2
  while(j<=ir)
  {
   if(j<ir && comp(array[j],array[j+1]) ) ++j
   if( comp(rarray,array[j]) )
   {
    array[i]=array[j]
    j+=(i=j)
   }
   else j=ir+1
  }
  array[i]=rarray
 }
}                                            

function comp(a,b)
{
 if( lab_name(a) == lab_name(b) ) return lab_index(a) > lab_index(b)
 else return lab_name(a) > lab_name(b)
}

