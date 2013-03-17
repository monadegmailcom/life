#include "cell.h"

#include <iostream>
using std::cout; using std::endl;
using std::ostream;

void testCoord()
{
   // test assignment and coord read
   {
      int32_t inX = -1;
      int32_t inY = 1;
      Coord c = toCoord( inX, inY );

      int32_t outX = getX( c );
      cout << outX << ' ' << (outX == inX ? "OK" : "FAIL") << endl;

      int32_t outY = getY( c );
      cout << outY << ' ' << (outY == inY ? "OK" : "FAIL") << endl;
   }

   // test ordering
   {
      {
         Coord c1 = toCoord( -1, 1 );
         Coord c2 = toCoord( 0, 0 );

         cout << c1.c << " < " << c2.c << ' ' << (c1.c < c2.c ? "OK" : "FAIL") << endl;
      }

      {
         Coord c1 = toCoord( -1, 1 );
         Coord c2 = toCoord( -1, 7 );

         cout << c1.c << " < " << c2.c << ' ' << (c1.c < c2.c ? "OK" : "FAIL") << endl;
      }

      {
         Coord c1 = toCoord( -1, 7 );
         Coord c2 = toCoord( -1, 1 );

         cout << c1.c << " > " << c2.c << ' ' << (c1.c > c2.c ? "OK" : "FAIL") << endl;
      }

      {
         Coord c1 = toCoord( 1, 7 );
         Coord c2 = toCoord( 1, 7 );

         cout << c1.c << " != " << c2.c << ' ' << (c1.c < c2.c ? "FAIL" : "OK") << endl;
         cout << c2.c << " != " << c1.c << ' ' << (c2.c < c1.c ? "FAIL" : "OK") << endl;
      }
   }
}

template< int I >
void dumpNb( Cell const& c )
{
   Cell const* const nb = c.nb[I];
   if (!nb)
      return;

   if (nb->occ) 
      cout << I;
   else
      cout << "[" << I << "]";
      
   cout << ", ";
}

ostream& operator<<( ostream& s, Cell const& c )
{
   s
      << "coord x/y     = " << getX( c.coord ) << '/' << getY( c.coord ) << '\n'
      << "occ/nbc/vdlen = " << int( c.occ ) << '/' << int( c.nbc ) << '/' << c.vdlen << '\n'
      << "nb            = ";
   dumpNb< 0 >( c );
   dumpNb< 1 >( c );
   dumpNb< 2 >( c );
   dumpNb< 3 >( c );
   dumpNb< 4 >( c );
   dumpNb< 5 >( c );
   dumpNb< 6 >( c );
   dumpNb< 7 >( c );
   cout << "\n";

   return s;
}

ostream& operator<<( ostream& s, CellSet const& cells )
{
   for (CellSet::const_iterator itr = cells.begin(), end = cells.end(); 
        itr != end; ++itr)
      s << *itr << endl;

   return s;
}

void testCellAdd()
{
   CellSet cells;

   // construct glider
   add( cells, -1, 1 );
   add( cells, 0, -1 );
   add( cells, 0, 1 );
   add( cells, 1, 0 );
   add( cells, 1, 1 );

   cout << cells << endl;
   nextTick( cells, 1 );
   cout << cells << endl;
}

int main()
{
   testCoord();
   testCellAdd();

   return 0;
}
