
#include "term.h"

void term::print( std::ostream& out, unsigned int depth ) const
{
   for( size_t i = 0; i < depth; ++ i ) out << "   ";
   out << getnode( ) << "         ";
   out << "{" << pntr << "/" << pntr -> refcounter << "}\n";

   for( size_t i = 0; i < arity( ); ++ i )
      (*this)[i]. print( out, depth + 1 );
}


std::ostream& operator << ( std::ostream& out, const term& t )
{
   out << t. getnode( );
   if( t. arity( ))
   {
      out << "( ";
      for( size_t i = 0; i < t. arity( ); ++ i ) 
      {
         if(i) out << ", ";
         out << t[i]; 
      }
      out << " )";
   }
   return out;
}

term subst( const term& t, const std::string& node, const term& val )
{
   if( t. arity( ) == 0 )
   {
      if( t. getnode( ) == node )
         return val;
      else
         return t;
   }

   auto result = t;
   for( size_t i = 0; i < t. arity( ); ++ i )
      result. assign( i, subst( t[i], node, val ));

   return result;
}

term subst2( const term& t, const std::string& node, const term& val )
{
   if( t. arity( ) == 0 )
      return t. getnode( ) == node ? val : t;

   bool change = false; 
   std::vector< term > res;
   for( size_t i = 0; i < t. arity( ); ++ i )
   {
      res. push_back( subst2( t[i], node, val ));
      if( ! res. back( ). certainly_equal( t[i] )) change = true;
   }

   return change ? term( t. getnode( ), std::move( res )) : t;
}

