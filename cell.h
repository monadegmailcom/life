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

// set of cells, ordered by coordinates (x, y) packed in uint64_t
// use intrusive container for customized memory management
#include <boost/intrusive/set.hpp>

// to order pairs (x,y) in lexical order address them as uint64_t
union Coord
{
   struct Pair
   {
      uint32_t y; // declare y before x because of little endian
      uint32_t x;
   } p;
   uint64_t c;
};

// signed coordinates are added with offset to avoid negativ values
Coord toCoord( int32_t x, int32_t y );
int32_t getX( Coord c );
int32_t getY( Coord c );

struct Cell 
{
   // default initilize, empty and no neighbours
   Cell( Coord c ); // coordinate of cell

   /* each cell can be in the state occupied or empty 
      state change occupied -> empty     : death
                   occupied -> occupied  : no change
                   empty     -> empty    : no change
                   empty     -> occupied : birth */
   char chg; // -1 death, 0 no change, 1 birth

   // a cell is called void if its in state empty and has a neighbour count
   // of 0. void length is the number of ticks a cell is continously void
   // algorithm removes cells with large vdlen
   uint16_t vdlen;  

   uint8_t occ; // occupied 
   uint8_t nbc; // occupied neighbour count
   
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

   const Coord coord;
};

// lexical coordinate order 
bool operator<( Cell const& lhs, Cell const& rhs );

// typedef for intrusive set
typedef boost::intrusive::member_hook< 
   Cell, boost::intrusive::set_member_hook<>, &Cell::hook > CellMemberOption;
typedef boost::intrusive::set< Cell, CellMemberOption > CellSet;

void add( CellSet& cells, int x, int y );
// proceed to next tick 
void nextTick( CellSet& cells, uint16_t vdlen );

// count cells
std::size_t count( CellSet const& cells );

struct Component
{
   CellSet cells;

   uint32_t minX;
   uint32_t minY;
   uint32_t maxX;
   uint32_t maxY;

   // member hook for intrusive set 
   boost::intrusive::set_member_hook<> hook;
};

bool operator<( Component const&, Component const& );

// typedef for intrusive set
typedef boost::intrusive::member_hook< 
   Component, boost::intrusive::set_member_hook<>, &Component::hook > ComponentMemberOption;
typedef boost::intrusive::set< Component, ComponentMemberOption > ComponentSet;

void divide( CellSet& cells, ComponentSet& components );
void join( ComponentSet& components );

#endif

