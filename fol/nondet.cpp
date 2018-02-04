
#include "nondet.h"
#include <stdexcept>

#if 0

// With a try_in_sequence, it would look as follows:

std::ostream& operator << ( std::ostream& out, const fol& f )
{
   try_in_sequence( const failure& f )  
   { 
      auto e = match_equality(f);
      out << "( " << e. t1 << " == " << e. t2 << " )";
   }
   else 
   {
      auto u = match_unary(f);
      out << "( " << u. op << " " << u. f << " )";
   }
   else 
   {
      auto b = match_binary(f);
      out << "( " << b. f1 << " " << b. op << " " << b. f2 << " )";
   }
   else 
   { 
      auto n = match_nullary(f);
      out << n. op;
   }
   else
   {
      auto a = match_atom(f);
      out << a. pred;
      if( a. size( ))
      {
         out << "(";
         for( auto p = a. p0; p != a. p1; ++ p )
         {
            if( p != a. p0 )
               out << ", ";
            out << *p; 
         }
         out << ")";

      }
   }
   else
   {
      auto q = match_quantifier(f);
      out << "( " << q. op << " " << q. var << " : " << q. f << " )";
   }

   return out;
}

#endif


