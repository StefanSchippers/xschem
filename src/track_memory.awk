#!/usr/bin/awk -f


BEGIN{
  total = 0 
  malloc = 0
  free = 0
  realloc = 0
}

# my_malloc(234,): allocating 1a01ff0 , 10 bytes
/^my_[mc]alloc\(/{ 
  id = $1
  sub(/.*\(/,"", id)
  sub(/,.*/,"",id)
  idx[$3] = id
  total += $5
  address[$3] = $5
  malloc++
}

# my_free():  freeing 198efc0
/^my_free\(/{ 
  total -= address[$3]
  delete address[$3]
  delete idx[$3]
  free++
}

# my_realloc(235,): reallocating 0 --> 198efa0 to 24 bytes
# my_realloc(237,): reallocating 198efc0 --> 1cfc090 to 28 bytes
/^my_realloc\(/{
  id = $1
  sub(/.*\(/,"", id)
  sub(/,.*/,"",id)
  idx[$3] = id
  if($3=="(nil)") { #malloc
    total+=$7
    address[$5] = $7
    idx[$5] = id
  }
  else { # realloc
    total += $7 - address[$3]
    delete address[$3]
    delete idx[$3]
    address[$5] = $7
    idx[$5] = id
  }
  realloc++
}


END{
  print "total = " total
  print "malloc = " malloc
  print "realloc = " realloc
  print "free = " free
  stale = 0
  leak = 0

  for(i in address) {



 
    stale++
    leak+= address[i]
    print "  address[ " i ", " idx[i] " ]= " address[i]
    pipe = "grep -n ' \([^/][^*]\) *my_.*(" idx[i] ",' *.c xschem.h"
    while( pipe | getline a) print "    " a
    close(pipe)
  }
  print "stale= " stale
  print "leak= " leak
}
