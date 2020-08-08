#!/bin/sh

awk '

{ $0 = tolower($0)}

/port map\(/{ 
  while($0 !~/; *$/) {
    a=$0
    getline
    $0=a $0
  }
  gsub(/  */," ",$0)
  gsub(/  *;/,";",$0)
#  print ">>>>" $0 "<<<<" 
}


/port map\(/,/\) *;/{
 sub(/--.*$/,"",$0)
 if($0 ~ /port map\(/) a=""
 a = a $0 " "
 if($0 ~ /\) *;/)
 {
  gsub(/ +/," ", a)
  $0=a
 }
 else next
}


/[ \t]*port\(/{
 end_port_list=0
 sub( /port\(/,"port(\n       ",$0 )
 port_list=1
}

(port_list==1){
 if($0 ~ /\) *;/) 
 { 
  sub(/\) *;/,"",$0) 
  end_port_list=1
 }
 gsub(/;/, ";\n      ",$0 )
 if(end_port_list)
 {
  $0 = $0 "\n      );\n"
  port_list=0
 }
}



($2==":" && $4=="port" && $5~/^map\(/){

 sub(/port map\(/,"\n   port map(\n    ",$0)
 port_map=1
 end_port_map=0
}

(port_map==1){
 if($0 ~ /\);/)
 {
  sub(/\);/,"",$0)
  end_port_map=1
 }
 gsub(/,/,",\n    ",$0)
 if(end_port_map)
 {
   $0 = $0 "\n   );\n"
   port_map=0
 }
}

{print}

' $@
