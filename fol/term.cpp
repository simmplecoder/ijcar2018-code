
#include "term.h"

void term::assign( size_t i, const term& t )
{
   if( sub[i]. certainly_equal(t) )
      return;

   // If we have an assignment of form t. assign( i, t ),
   // we will create self-referential node if t is not shared.
   // This can be solved by copying t, after which it will
   // share with *this, which will force a unique copy for *this.

   if( sub. shared_with( t. sub ))
      sub. getuniquesub(i) = term(t);
   else
      sub. getuniquesub(i) = t;
}


void term::print( std::ostream& out, unsigned int depth ) const 
{
   for( unsigned int i = 0; i < depth; ++ i )
      out << "   ";

   out << "{" <<
   ( reinterpret_cast< long unsigned int > ( getaddress( ) ) & 0XFFFF )
    << "}:";

   out << function << "\n";

   for( auto p = sub. begin( ); p != sub. end( ); ++ p )
      p -> print( out, depth + 1 );
   
}


std::ostream& operator << ( std::ostream& out, const term& t )
{
   out << "{" <<
   ( reinterpret_cast< long unsigned int > ( t. getaddress( ) ) & 0XFFFF )
    << "}:";

   out << t. function;
   if( t. sub. size( ))
   {
      out << "( ";
      for( auto p = t. sub. begin( ); p != t. sub. end( ); ++ p )
      {
         if( p != t. sub. begin( )) out << ", ";
         out << *p;
      }
      out << " )";
   }

   return out;
}

 
