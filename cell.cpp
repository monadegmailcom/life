#include "cell.h"

// (x,y) coordinate typedef 
typedef std::pair< int, int > Coord;

// lexical coordinate order functor
// instrusive set is order by this functor
struct KeyCmp
{
   bool operator()( Coord& coord, Cell const& cell ) 
   {
      if (coord.first < rhs.x)
         return true;
      else if (coord.first == rhs.x && coord.second < rhs.y)
         return true;
      else
         return false; 
   }
};

// order operator
bool operator<( Cell const& lhs, Cell const& rhs )
{
   return KeyComp()( Coord( lhs.x, lhs.y ), rhs );
}

// dispose functor for intrusive set
// to be replaced by memory pool management
struct Disposer
{
   void operator()( Cell* cell ) { delete cell; }
};

Cell::Cell( int _x, int _y )
: 
   occ( 0 ), // empty 
   nbc( 0 ), // no neighbours
   chg( 0 ), // no change
   x( _x ), y( _y )
{
   nb[0] = nb[1] = nb[2] = nb[3] = nb[4] = nb[5] = nb[6] = nb[7] = 0;
}

// helper function for untangle
template< char I >
untangleHelp( Cell& cell )
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
         chg = -1;
      }
   }
   // else cell is empty
   else if (cell.nbc == 3) // birth?
   {
      cell.occ = 1;
      chg = 1;
   }
}

// proceed to next tick 
void nextTick( CellSet& cells, unsigned short vdlen )
{
   // update new occupied flag for all cells in list
   for (CellSet::iterator itr = cells.begin(), end = cells.end(); itr != end;
        ++itr)
      updateCell( *itr );  

   // update neighbourhood for all cells
   // may insert new cells and delete void cells 
   for (CellSet::iterator itr = cells.begin(); itr != cells.end(); ++itr)
      updateNb( *itr, cells, vdlen );  

   // trim list
   for (CellSet::iterator itr = cells.begin(); itr != cells.end();)
      if (!itr->occ && !itr->nbc) // empty cell with no neighbours?
      {
         untangle( *itr ); // untangle all references to me
         itr = cells.erase_and_dispose( itr, Disposer()); 
      }  
      else
         ++itr;
}
 
Cell* getCell( const int x, const int y, CellSet& cells )
{
   CellSet::iterator itr = cells.find( Coord( x, y ), KeyComp());
   if (itr == cells.end()) // no cell found?
      // insert new empty cell 
      itr = cells.insert( cells.begin(), *(new Cell( x, y ))); 
   return &*itr; 
}

template< char X, char Y, char I >
void updateNbHelp( Cell& cell, CellMap& cells )
{
   Cell* c = cell.nb[I];
   if (!c)
   {
      c = getCell( cell.x+X, cell.y+Y, cells );
      c->nb[7-I] = &cell;
      cell.nb[I] = c; 
   }
   ++c->nbc;
}
 
/* update neighbour count of all 8 neighbours 
 * set new neigbour references for birth cells */
void updateNb( Cell& cell, CellMap cells )
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
      --cell.nb[0].nbc; // decrement neighbour count
      --cell.nb[1].nbc; 
      --cell.nb[2].nbc; 
      --cell.nb[3].nbc; 
      --cell.nb[4].nbc; 
      --cell.nb[5].nbc; 
      --cell.nb[6].nbc; 
      --cell.nb[7].nbc; 
   }
}

// add cell to list (may be empty)
void add( CellSet& cells, int x, int y )
{
   Cell* const cell = new Cell( x, y ); // create new cell
   cell->occ = 1; // occupied
   cell->chg = 1; // just born

   cells.insert( cells.begin(), *cell );

   updateNb( *cell, cells ); // update neighbours
} 

// delete all cells in list
void deallocate( Cell* cell )
{
   for (;cell;)
   {
      Cell* next = cell->next; 
      delete cell;
      cell = next;
   } 
}
