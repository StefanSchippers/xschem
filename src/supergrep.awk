#!/bin/sh

awk '
BEGIN{
 pattern=ARGV[1]
 ARGV[1]=""
}

FNR == 1 { a=b=c=d=e=f=g=h=i=j=k=l="" }

{
 a=b
 b=c
 c=d
 d=e
 e=f
 f=g
 g=h
 h=i
 i=j
 j=k
 k=l
 l=$0
}

(e ~ pattern){
 print "--- " FILENAME ":" FNR-2 " ---"
 print a
 print b
 print c
 print d
 print e
 print f
 print g
 print h
 print i
 print ""
} 
 '  $@


