#include "cell.h"

// use intrusive container for customized memory management
#include <boost/intrusive/list.hpp>

// use local memory management to avoid heap allocations
#include <boost/pool/object_pool.hpp>

#include <boost/scoped_array.hpp>
using boost::scoped_array;

#include <vector>
using std::vector;

using std::size_t;
using std::pair;

namespace {

// x is in interval [a,b], x,a,b in Z(X)
bool inInterval( const uint32_t x, const uint32_t a, const uint32_t b )
{
   if (a < b)
      return (a <= x && x <= b);
   else
      return (a <= x || x <= b);
}

// order pairs (x,y) in lexical order address them as uint64_t
union Coord
{
   struct Pair
   {
      uint32_t y; // declare y before x because of little endian
      uint32_t x;
   } p;
   uint64_t c;
};

Coord toCoord( const uint32_t x, const uint32_t y )
{
   Coord c;
   c.p.x = x;
   c.p.y = y;
   return c;
}

uint32_t getX( Coord c )
{
   return c.p.x;
}

uint32_t getY( Coord c )
{
   return c.p.y;
}

/*
an octet is a collection of 8 cells surrounded by 16
neighbour cells

******
*0123*
*4567*
******

012345
6****7
8****9
abcdef

*/

struct Rect
{
   Coord leftUpper;
   uint32_t width;
   uint32_t height;

   static bool intersect( Rect const& lhs, Rect const& rhs );
   static void join( Rect& lhs, Rect const& rhs );
};

template< typename R >
struct AreaBase : public boost::intrusive::list_base_hook<>    
{
   R rect;

   // typedef for intrusive list
   typedef boost::intrusive::list< AreaBase > List;
};

/* join a collection of overlapping rectangles. The resulting "list"
   consists of disjunct rectangles which covers the original list.
   L: type of rectangle list
   require: 
   L::iterator, L::begin(), L::end(), L::erase(), L::iterator::operator++()
   R = L::value_type, join( R, R ), intersect( R, R )
*/
template< typename L >
void joinRectangles( L& list )
{
   for (L::iterator next = list.begin(), itr = next++; itr != list.end(); 
        ++itr, ++next)
   {
      for (L::iterator itr2 = next; itr2 != list.end();)
      {
         if (intersect( *itr, *itr2 ))
         {
            join( *itr, *itr2 );
            list.erase( itr2 );
            itr2 = next;
         }
         else
            ++itr2;
      }
   }
}

typedef AreaBase< Rect > Area;

void join( Area& lhs, Area const& rhs )
{
   Rect::join( lhs.rect, rhs.rect );
}

bool intersect( Area const& lhs, Area const& rhs )
{
   return Rect::intersect( lhs.rect, rhs.rect );
}

void syntaxCheck()
{
   Area::List list;
   joinRectangles( list );
}

/*
   C: content
   require:
   C::State& C::state( C::Coord )
   C::State C::newState( C::Coord ) const
   bool operator!=( C::State, C::State )
   void C::neighbours( C::Coord, C::State, vector< C::Coord >& ) const
   uint8_t& C::visited( C::Coord )
   bool C::border( C::Coord ) const
*/

template< typename C >
bool nextTick( 
   vector< typename C::Coord >& candidates, 
   vector< typename C::Coord >& tmp1, 
   vector< typename C::Coord >& tmp2, 
   vector< typename C::State >& newStates,
   C& content )
{
   bool border = false;

   // calc new states of candidates
   newStates.clear();
   for (vector< C::Coord >::iterator itr = candidates.begin(), 
        end = candidates.end(); itr != end; ++itr)
      newStates.push_back( content.newState( *itr ));

   tmp2.clear();
   for (size_t idx = candidates.size; idx; --idx)
   {
      C::Coord& c = candidates[idx];
      C::State& newState = newStates[idx];
      content.state( c ) = newState;

      content.neighbours( c, newState, tmp1 );
      for (vector< C::Coord >::iterator nbItr = tmp1.begin(), 
           nbEnd = tmp1.end(); nbItr != nbEnd; ++nbItr)
      {
         uint8_t& visited = content.visited( *nbItr );
         if (!visited)
         {
            visited = 1;
            border |= content.border( *nbItr );
            tmp2.push_back( *nbItr );
         }
      }
   }

   for (vector< C::Coord >::iterator itr = tmp2.begin(), 
        end = tmp2.end(); itr != end; ++itr)
      content.visited( *itr ) = 0;

   swap( candidates, tmp2 );
   return border;
}

struct Dim2
{
   typedef uint8_t State; // octed
   typedef uint64_t Coord;

   Dim2( uint32_t width, uint32_t height );
   uint64_t maxCoord; // N = width*height

   scoped_array< uint8_t > cells; // [N]
   scoped_array< uint8_t > visited; // [N]
   scoped_array< uint8_t > stateChangeTable; // [2^(8+16)] ~ 16Mb

   struct TableEntry
   {
      uint8_t mask;
      uint64_t offset;
   };
   TableEntry neighbourTable[2*2*2];

   uint8_t& state( uint64_t );
   uint8_t& visited( uint64_t );

   uint8_t newState( uint64_t ) const;
   void neighbours( Change< Dim2 > const&, vector< uint64_t >& ) const;
   bool border( uint64_t ) const;
};

Dim2::Dim2( uint32_t width, uint32_t height )
: N( width*height), cells( new uint8_t[N] ), visited( new uint8_t[N] )
{
   // calculate state change table

   // fill neighbour table
}

uint8_t& Dim2::state( const uint64_t c )
{
   return cells[c];
}

uint8_t& Dim2::visited( const uint64_t c )
{
   return cells[c];
}

uint8_t Dim2::newState( const uint64_t c ) const
{
   // todo: calc index
   uint32_t idx = 0;
   return stateChangeTable[idx];
}

void Dim2::neighbours( 
   Change< Dim2 > const& change, vector< uint64_t >& neighbours ) const
{
   const uint8_t diff = change.state^change.newState;
   uint64_t& c = change.coord;

   neighbours.clear();

   const size_t tableSize = sizeof( neighbourTable )/sizeof( TableEntry );
   for (size_t idx = tableSize; idx; --idx)
   {
      TableEntry const& e = neighbourTable[idx];
      if (diff & e.mask)
         neighbours.push_back( c+e.offset );
   }
}

bool Dim2::border( const uint64_t c ) const
{
   // todo: implement
   return false;
}

void syntaxCheck2()
{
   vector< uint64_t > candidates;
   vector< Dim2::Coord > neighbours;
   vector< Change< Dim2 > > changes;
   Dim2 content( 17, 4 );
   nextTick( candidates, neighbours, changes, content );
}

//boost::object_pool< Area > areaPool;

struct Torus
{
   Torus( unsigned powX, unsigned powY ); // 2^powX/Y, 0 <= powX/Y <= 32

   // modulo masks
   uint32_t maskX; 
   uint32_t maskY;

   Area::List areas;
};

uint32_t calcBitmask( unsigned k ) // 0 <= k <= 32
{
   uint32_t result = 0;
   for (;k;--k)
   {
      result <<= 1;
      result |= 0x01;
   }

   return result;
}

Torus::Torus( unsigned powX, unsigned powY )
: 
   maskX( calcBitmask( powX )), maskY( calcBitmask( powY ))
{}

} //   namespace {
