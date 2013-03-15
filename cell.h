#ifndef LIFE_H
#define LIFE_H

// set of cells, ordered by coordinates (x, y)
// use intrusive container for customized memory management
#include <boost/intrusive/set.hpp>

struct Cell
{
   // default initilize, empty and no neighbours
   Cell( int x, int y ); // x, y coordinate of cell

   /* each cell can be in the state occupied or empty 
      state change occupied -> empty     : death
                   occupied -> occupied  : no change
                   empty     -> empty    : no change
                   empty     -> occupied : birth */
   char chg; // -1 death, 0 no change, 1 birth

   // a cell is called void if its in state empty and has a neighbour count
   // of 0. void length is the number of ticks a cell is continously void
   unsigned short vdlen;  

   unsigned char occ; // occupied 
   unsigned char nbc; // occupied neighbour count
   
   /* 8 neigbour cells of a 2 dim cartesian grid
      0 1 2
      3   4
      5 6 7
      referenced cells always reference back with index formular:
      i <-> 7-i 
      an unset pointer indicates an empty neighbour
      when pointer is set, the neighbour may be occupied or empty */
   Cell* nb[8]; 

   // member hook for intrusive set 
   boost::intrusive::set_member_hook<> hook;

   const int x; // x coordinate 
   const int y; // y coordinate
};

// lexical coordinate order 
bool operator<( Cell const& lhs, Cell const& rhs );

// typedef for intrusive set
typedef boost::intrusive::member_hook< 
   Cell, boost::intrusive::set_member_hook<>, &Cell::hook > CellMemberOption;
typedef boost::intrusive::set< Cell, CellMemberOption > CellSet;

#endif

