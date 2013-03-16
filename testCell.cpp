#include "cell.h"
#include <iostream>
using std::cout; using std::endl;

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

int main()
{
   testCoord();
   
   return 0;
}
