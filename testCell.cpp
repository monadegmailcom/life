#include "cell.h"

#include <iostream>
using std::cout; using std::endl;
using std::ostream;

void testCoord()
{
   // test assignment and coord read
   {
      uint32_t inX = 0;
      uint32_t inY = 1;
      Coord c = toCoord( inX, inY );

      int32_t outX = getX( c );
      cout << outX << ' ' << (outX == inX ? "OK" : "FAIL") << endl;

      int32_t outY = getY( c );
      cout << outY << ' ' << (outY == inY ? "OK" : "FAIL") << endl;
   }

   // test ordering
   {
      {
         Coord c1 = toCoord( 0, 2 );
         Coord c2 = toCoord( 1, 1 );

         cout << c1.c << " < " << c2.c << ' ' << (c1.c < c2.c ? "OK" : "FAIL") << endl;
      }

      {
         Coord c1 = toCoord( 0, 2 );
         Coord c2 = toCoord( 0, 8 );

         cout << c1.c << " < " << c2.c << ' ' << (c1.c < c2.c ? "OK" : "FAIL") << endl;
      }

      {
         Coord c1 = toCoord( 0, 8 );
         Coord c2 = toCoord( 0, 2 );

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
      << "coord x/y = " << getX( c.coord ) << '/' << getY( c.coord ) << '\n'
      << "occ/nbc   = " << int( c.occ ) << '/' << int( c.nbc ) << '\n'
      << "nb        = ";
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

void testGlider()
{
   CellSet cells;

   // construct glider
   add( cells, -1, 1 );
   add( cells, 0, -1 );
   add( cells, 0, 1 );
   add( cells, 1, 0 );
   add( cells, 1, 1 ); 

   unsigned i = 10;
   for (; i; --i)
   {
//      cout << cells << endl;
//      cout << count( cells ) << '/' << cells.size() << '\n';
      nextTick( cells );
      if (count( cells ) != 5)
         break;
   }

   cout << (i ? "FAIL" : "OK" ) << endl;
}

void testBitmask()
{
   {
      const uint8_t k = 0;
      const uint32_t bm = calcBitmask( k ); 
      cout 
         << int( k ) << " -> " << std::hex << bm << " " 
         << (bm == 0 ? "OK" : "FAIL") << endl;
   }

   {
      const uint8_t k = 1;
      const uint32_t bm = calcBitmask( k ); 
      cout 
         << int( k ) << " -> " << std::hex << bm << " " 
         << (bm == 0x00000001 ? "OK" : "FAIL") << endl;
   }

   {
      const uint8_t k = 2;
      const uint32_t bm = calcBitmask( k ); 
      cout 
         << int( k ) << " -> " << std::hex << bm << " " 
         << (bm == 0x00000003 ? "OK" : "FAIL") << endl;
   }

   {
      const uint8_t k = 32;
      const uint32_t bm = calcBitmask( k ); 
      cout 
         << std::dec << int( k ) << " -> " << std::hex << bm << " " 
         << (bm == 0xffffffff ? "OK" : "FAIL") << endl;
   }
}

void testInterval()
{
   {
      uint32_t a = 1;
      uint32_t b = 2;
      uint32_t x = 0;
      cout 
         << x << " not in [" << a << "," << b << "] ? " 
         << (!inInterval( x, a, b ) ? "OK" : "FAIL") << endl;
   }

   {
      uint32_t a = 1;
      uint32_t b = 2;
      uint32_t x = 1;
      cout 
         << x << " in [" << a << "," << b << "] ? " 
         << (inInterval( x, a, b ) ? "OK" : "FAIL") << endl;
   }

   {
      uint32_t a = 1;
      uint32_t b = 2;
      uint32_t x = 3;
      cout 
         << x << " not in [" << a << "," << b << "] ? " 
         << (!inInterval( x, a, b ) ? "OK" : "FAIL") << endl;
   }

   {
      uint32_t a = 3;
      uint32_t b = 1;
      uint32_t x = 0;
      cout 
         << x << " in [" << a << "," << b << "] ? " 
         << (inInterval( x, a, b ) ? "OK" : "FAIL") << endl;
   }

   {
      uint32_t a = 3;
      uint32_t b = 1;
      uint32_t x = 2;
      cout 
         << x << " not in [" << a << "," << b << "] ? " 
         << (!inInterval( x, a, b ) ? "OK" : "FAIL") << endl;
   }

   {
      uint32_t a = 3;
      uint32_t b = 1;
      uint32_t x = 4;
      cout 
         << x << " in [" << a << "," << b << "] ? " 
         << (inInterval( x, a, b ) ? "OK" : "FAIL") << endl;
   }
}

int main()
{
   testBitmask();
   testInterval();
   testCoord();
   //testGlider();

   return 0;
}
