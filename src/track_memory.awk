#!/usr/bin/awk -f
# memory leak analyzer. Run xschem with options "-d 3 -l log", do some operations you want to check
# then *from this directory* launch: 
# ./track_memory.awk /path/to/log [nosource]
# it will print the amount of leaked memory (total, leak) 
# and the allocation that was not freed, with the source code line (if 'nosource' not given) 
# total and leak should indicate same amount of bytes, it is a cross check for the script.
BEGIN{
  show_source = 1
  max = 0
  total = 0 
  malloc = 0
  free = 0
  realloc = 0
  if(ARGC == 3) {
    if(ARGV[2] == "nosource") {
      show_source = 0
    }
    ARGC--
  }
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
    if(!($3 in address)) {
      print "Corruption: address " $3 " was freed. Can not realloc. id=" id
      print_source($3)
    }
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
    print_source(i)
  }
  print "Number of unfreed pointers = " stale
  # as a crosscheck 'leak' should be equal to 'total'.
  print "Total leaked memory = " leak
}

function print_source(add)
{
  print "  address[ " add ", " idx[add] " ]= " address[add]
  if(show_source) {
    pipe = "egrep -n 'my_(malloc|calloc|realloc|free|mstrcat|strcat|strncat|strdup|strdup2)\\(" idx[add] \
    ",' *.c xschem.h"
    while( pipe | getline a) print "    " a
    close(pipe)
  }
}
