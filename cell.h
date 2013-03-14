#ifndef LIFE_H
#define LIFE_H

#include <boost/intrusive/set.hpp>

struct Cell
{
   // default initilize, empty and no neighbours, no next cell
   Cell( int x, int y );

   unsigned char occ; // occupied 
   unsigned char nbc; // occupied neighbour count
   char chg; // -1 death, 0 no change, 1 birth
   /* 8 neigbour cells of a 2 dim cartesian grid
      0 1 2
      3   4
      5 6 7
      referenced cells always reference back with index formular:
      i <-> 7-i */
   Cell* nb[8]; 

   boost::intrusive::set_member_hook<> hook;

   const int x; // x coordinate
   const int y; // y coordinate
};

bool operator<( Cell const& lhs, Cell const& rhs );
typedef boost::intrusive::member_hook< 
   Cell, boost::intrusive::set_member_hook<>, &Cell::hook > CellMemberOption;
typedef boost::intrusive::set< Cell, CellMemberOption > CellSet;

#endif

