#!/bin/sh 

awk '

BEGIN{ 
 number="[-+]?([0-9]+|[0-9]*\\.[0-9]+)([eE][+-]?[0-9]+)?"
 operator ="[+-*/]"
 begin="^("
 end=")$"
 space="[ \\t]*"
 parenthesis="\\(.*\\)"
}

{
  print "-----------------------"
  print get_pattern(parenthesis)
  print "-----------------------"
}

function get_pattern(pattern        ,a,n)
{
 n=match($0,pattern)
 a=$0
 $0=substr($0,n+RLENGTH)
 return substr(a,n,RLENGTH)
}

' $@
