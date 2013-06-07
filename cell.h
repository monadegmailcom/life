#ifndef LIFE_H
#define LIFE_H

// -> declare standard types
#ifdef WIN32
// for windows
typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;

typedef __int32 int32_t;

#else
// for unix
#include <stdint.h>

#endif
// <- declare standard types

/*
Conways game of life in a torus compactification of the plane, where 
"up" is connnected to "down" and "left" to "right". Then the local 
neighbourhood topology is aquivalent to a 3 dimensional torus. 

         up
       _ _|_ _  X=4
      |_|_|_|_|
left _|_|_|_|_|_ right
      |_|_|_|_|
      |_|_|_|_|
  Y=4     |     
        down    

Let X and Y be the number of grid cells in horizontal and vertical direction. 
Each grid cell is address by coordinates (x,y), where x and y are members of 
the additive modulo groups Z(X)={0,...,X-1} and Z(Y)={0,...,Y-1}. 
When X=2^k and Y=2^n are powers of 2, modulo calculation is performed by 
bitwise AND operation with bit masks 0...01....1 with corresponding number
of 1's (0 <= k,n <= 32). 
*/

/*
An interval in Z(X) is a pair (a,b) of numbers from Z(X). a > b is possible.
x in [a,b] means following:

for a<=b:
    a--b
|out|in|out|
0          X-1

for b<=a:
---b   a---
|in|out|in|
0         X-1
*/

#endif

