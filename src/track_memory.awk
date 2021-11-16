#!/usr/bin/awk -f
# memory leak analyzer. Run xschem with options "-d 3 -l log", do some operations you want to check
# then *from this directory* launch: 
# ./track_memory.awk /path/to/log
# it will print the amount of leaked memory (total, leak) 
# and the allocation that was not freed, with the source code line.
# total and leak should indicate same amount of bytes, it is a cross check for the script.
BEGIN{
  max = 0
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
  if(total > max) max = total
  address[$3] = $5
  malloc++
}

# my_free(977,):  freeing 198efc0
/^my_free\(/{ 
  if(!($3 in address)) {
    print "Double free: " $0 " Log file line: " NR
  } else {
    total -= address[$3]
    delete address[$3]
    delete idx[$3]
    free++
  }
}

# my_realloc(235,): reallocating (nil) --> 198efa0 to 24 bytes
# my_realloc(237,): reallocating 198efc0 --> 1cfc090 to 28 bytes
/^my_realloc\(/{
  id = $1
  sub(/.*\(/,"", id)
  sub(/,.*/,"",id)
  idx[$3] = id
  if($3=="(nil)") { #malloc
    total+=$7
    if(total > max) max = total
    address[$5] = $7
    idx[$5] = id
  }
  else { # realloc
    total += $7 - address[$3]
    if(total > max) max = total
    delete address[$3]
    delete idx[$3]
    address[$5] = $7
    idx[$5] = id
  }
  realloc++
}

END{
  print "peak allocated memory = " max
  print "Total allocated memory after exit = " total
  print "# of malloc's = " malloc
  print "# of realloc's = " realloc
  print "# of free's = " free
  stale = 0
  leak = 0
  for(i in address) {
    stale++
    leak+= address[i]
    print "  address[ " i ", " idx[i] " ]= " address[i]
    pipe = "egrep -n 'my_(malloc|calloc|realloc|free)\(" idx[i] ",' *.c xschem.h"
    while( pipe | getline a) print "    " a
    close(pipe)
  }
  print "Number of unfreed pointers = " stale
  # as a crosscheck 'leak' should be equal to 'total'.
  print "Total leaked memory = " leak
}
