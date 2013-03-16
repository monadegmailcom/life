#include "cell.h"

// use local memory management to avoid heap allocations
#include <boost/pool/object_pool.hpp>

boost::object_pool< Cell > pool;

// offset for coordinates to avoid negative values
const uint32_t offset = 0x0fffffff; 

Coord toCoord( const int32_t x, const int32_t y )
{
   Coord c;
   c.p.x = x+offset;
   c.p.y = y+offset;
   return c;
}

int32_t getX( Coord c )
{
   return c.p.x-offset;
}

int32_t getY( Coord c )
{
   return c.p.y-offset;
}

// lexical coordinate order functor
// instrusive set is order by this functor
struct KeyCmp
{
   bool operator()( Cell const& cell, uint64_t const& c ) const
   { return cell.coord.c < c; }
};

// order operator
bool operator<( Cell const& lhs, Cell const& rhs )
{
   return lhs.coord.c < rhs.coord.c;
}

// dispose functor for intrusive set
struct Disposer
{
   void operator()( Cell* cell ) { pool.destroy( cell ); }
};

Cell::Cell( const Coord c )
: 
   occ( 0 ), // empty 
   nbc( 0 ), // no neighbours
   chg( 0 ), // no change
   coord( c )
{
   nb[0] = nb[1] = nb[2] = nb[3] = nb[4] = nb[5] = nb[6] = nb[7] = 0;
}

// helper function for untangle
template< char I >
void untangleHelp( Cell& cell )
{
   Cell* const c = cell.nb[I];
   c->nb[7-I] = 0; // unset reference
}

// untangle cell from neighbours, set all neighbour pointer to 0
void untangle( Cell& cell )
{
   untangleHelp< 0 >( cell ); 
   untangleHelp< 1 >( cell ); 
   untangleHelp< 2 >( cell ); 
   untangleHelp< 3 >( cell ); 
   untangleHelp< 4 >( cell ); 
   untangleHelp< 5 >( cell ); 
   untangleHelp< 6 >( cell ); 
   untangleHelp< 7 >( cell ); 
}

// conways life and death rule 
void updateCell( Cell& cell )
{
   cell.chg = 0;
   if (cell.occ) // cell occupied?
   {
      if (cell.nbc < 2 || cell.nbc > 3) // death?
      {
         cell.occ = 0;
         cell.chg = -1;
      }
   }
   else // else cell is empty
   {
      if (cell.nbc == 0) // void?
         ++cell.vdlen; // increment void length
      else // not void
      {
         cell.vdlen = 0;
         if (cell.nbc == 3) // birth?
         {
            cell.occ = 1;
            cell.chg = 1;
         }
      }         
   }
}

Cell* getCell( const Coord c, CellSet& cells )
{
   CellSet::iterator itr = cells.lower_bound( c.c, KeyCmp());
   if (itr == cells.end() || itr->coord.c != c.c) // no cell found?
      // insert new empty cell 
      itr = cells.insert( itr, *(pool.construct( c )));
   return &*itr; 
}

template< char X, char Y, char I >
void updateNbHelp( Cell& cell, CellSet& cells )
{
   Cell* c = cell.nb[I];
   if (!c)
   {
      Coord coord = cell.coord;
      coord.p.x += X;
      coord.p.y += Y;
      c = getCell( coord, cells );
      c->nb[7-I] = &cell;
      cell.nb[I] = c;
   }
   ++c->nbc;
}
 
/* update neighbour count of all 8 neighbours 
 * set new neigbour references for birth cells */
void updateNb( Cell& cell, CellSet& cells )
{
   const unsigned char chg = cell.chg;

   // update neighbours
  
   if (chg == 1) // birth?
   {
      updateNbHelp< -1, -1, 0 >( cell, cells );
      updateNbHelp< 0, -1, 1 >( cell, cells );
      updateNbHelp< 1, -1, 2 >( cell, cells );
      updateNbHelp< -1, 0, 3 >( cell, cells );
      updateNbHelp< 1, 0, 4 >( cell, cells );
      updateNbHelp< -1, 1, 5 >( cell, cells );
      updateNbHelp< 0, 1, 6 >( cell, cells );
      updateNbHelp< 1, 1, 7 >( cell, cells );
  }
   else if (chg == -1) // death?
   {
      // assert neighbour references are set
      --cell.nb[0]->nbc; // decrement neighbour count
      --cell.nb[1]->nbc; 
      --cell.nb[2]->nbc; 
      --cell.nb[3]->nbc; 
      --cell.nb[4]->nbc; 
      --cell.nb[5]->nbc; 
      --cell.nb[6]->nbc; 
      --cell.nb[7]->nbc; 
   }
}

// proceed to next tick 
void nextTick( CellSet& cells, const uint16_t vdlen )
{
   // update new occupied flag for all cells in list
   for (CellSet::iterator itr = cells.begin(), end = cells.end(); itr != end;)
   {
      updateCell( *itr );  
      if (itr->vdlen > vdlen) // void length exceeded?
      {
         // remove cell from set
         untangle( *itr ); // untangle all references to me
         itr = cells.erase_and_dispose( itr, Disposer()); 
      }
      else 
         ++itr->vdlen;
   }

   // update neighbourhood for all cells
   // may insert new cells and delete void cells 
   for (CellSet::iterator itr = cells.begin(); itr != cells.end(); ++itr)
      updateNb( *itr, cells );  
}
 
// add cell to list (may be empty)
void add( CellSet& cells, int x, int y )
{
   Cell* const cell = pool.construct( toCoord( x, y )); // create new cell
   cell->occ = 1; // occupied
   cell->chg = 1; // just born

   cells.insert( cells.begin(), *cell );

   updateNb( *cell, cells ); // update neighbours
} 
