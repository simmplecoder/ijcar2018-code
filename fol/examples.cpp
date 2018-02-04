
#include "examples.h"
#include <sstream>


size_t stack::find( const std::string& var ) const
{
   for( size_t i = 0; i < vect. size( ); ++ i )
   {
      if( vect[ vect. size( ) - i - 1 ] == var ) return i;  
   }
   return vect. size( );
}

std::ostream& operator << ( std::ostream& out, const stack& s )
{
   out << "[";
   for( auto p = s. begin( ); p != s. end( ); ++ p )
   {
      if( p != s. begin( )) 
         out << ", ";
      else
         out << " ";
      out << *p;
   }
   out << " ]";
   return out;
}

bool isfree( const std::string& var, const term& t )
{
   if( t. sub. size( ) == 0 ) 
      return var == t. function;
   else
   {
      for( const auto& sub : t )
         if( isfree( var, sub )) return true;
      return false;  
   }
}


bool isfree( const std::string& var, const fol& f )
{


   return try_in_sequence(
      [&var,&f]( ) {  
         auto a = f. try_atom( );
         for( auto p = a. sub_begin( ); p != a. sub_end( ); ++ p )
         {
            if( isfree( var, *p )) 
               return true;
         }
         return false;
      },
      [&var,&f]( ) 
      {  auto e = f. try_equality( );
         return isfree( var, e[0] ) || isfree( var, e[1] );
      },
      [&var,&f]( ) 
      {  auto n = f. try_nullary( ); 
         return false; 
      },
      [&var,&f]( ) 
      {  auto u = f. try_unary( );
         return isfree( var, u[0]);
      },
      [&var,&f]( ) {   
         auto b = f. try_binary( );
         return isfree( var, b[0] ) || isfree( var, b[1] );
      },
      [&var,&f]( ) 
      {
         auto q = f. try_quant( );
         require( q. var != var );
         return isfree( var, q. body( ));
      },
      [&var,&f]( ) 
      {
         auto q = f. try_quant( );
         require( q. var == var );
         return false;  
      },
      [&var,&f]( ) { throw std::runtime_error( "in <<, could not match" ); 
                     return false || true; }
   );

}

bool alpha_equivalent( stack& s1, const term& t1,
                       stack& s2, const term& t2 )
{
   std::cout << "alpha equivalent\n";
   std::cout << s1 << " : " << t1 << "\n";
   std::cout << s2 << " : " << t2 << "\n";

   if( t1. sub. size( ) != t2. sub. size( )) 
      return false;

   if( t1. sub. size( ))
   {
      if( t1. function != t2. function ) return false;

      auto p1 = t1. begin( );
      auto p2 = t2. begin( );
      while( p1 != t1. end( ))
      {
         if( !alpha_equivalent( s1, *p1, s2, *p2 )) 
            return false;
         ++ p1; 
         ++ p2;
      }
      return true;
   }
   else
   {
      size_t i1 = s1. find( t1. function );
      size_t i2 = s2. find( t2. function );

      // They must be both undefined, or defined at the same position.

      if( i1 == s1. size( ) && i2 == s2. size( )) 
         return t1. function == t2. function;

      return i1 == i2;    
         // At least one of them is defined, which implies that the
         // other is defined too. 
   }
}


bool alpha_equivalent( stack& s1, const fol& f1, stack& s2, const fol& f2 )
{
   std::cout << "alpha equivalent\n";
   std::cout << s1 << " : " << f1 << "\n";
   std::cout << s2 << " : " << f2 << "\n";

   if( f1. getop( ) != f2. getop( )) 
      return false;                    // Pleasure was short. 

   return try_in_sequence(
      [&s1,&f1,&s2,&f2]( ) {  
         auto a1 = f1. try_atom( ); auto a2 = f2. try_atom( ); 
         if( a1. nrsubterms( ) != a2. nrsubterms( )) return false; 

         auto p1 = a1. sub_begin( );
         auto p2 = a2. sub_begin( );  
         {
            if( !alpha_equivalent( s1, *p1, s2, *p2 )) 
               return false;
            ++ p1;
            ++ p2;
         }
         return true;
      },
      [&s1,&f1,&s2,&f2]( ) 
      {  auto e1 = f1. try_equality( ); auto e2 = f2. try_equality( ); 
         return alpha_equivalent( s1, e1[0], s2, e2[0] ) || 
                alpha_equivalent( s1, e1[1], s2, e2[1] ); 
      },
      [&s1,&f1,&s2,&f2]( ) 
      {  auto n1 = f1. try_nullary( ); auto n2 = f2. try_nullary( ); 
         return true; 
      },
      [&s1,&f1,&s2,&f2]( ) 
      {  auto u1 = f1. try_unary( ); auto u2 = f2. try_unary( ); 
         return alpha_equivalent( s1, u1[0], s2, u2[0] );
      },
      [&s1,&f1,&s2,&f2]( ) 
      {   
         auto b1 = f1. try_binary( ); auto b2 = f2. try_binary( ); 
         return alpha_equivalent( s1, b1[0], s2, b2[0] ) &&
                alpha_equivalent( s1, b1[1], s2, b2[1] );
      },
      [&s1,&f1,&s2,&f2]( ) 
      {
         auto& q1 = f1. try_quant( ); auto q2 = f2. try_quant( ); 
         s1. push_back( q1. var );     
         s2. push_back( q2. var );
         bool b = alpha_equivalent( s1, q1. body( ), s2, q2. body( )); 
         s1. pop_back( );
         s2. pop_back( ); 
         return b; 
      },
      [&s1,&f1,&s2,&f2]( ) 
      {  throw std::runtime_error( "in <<, could not match" ); 
         return false; 
      }
   );
}


std::string appendindex( const std::string& s, size_t i )
{
   size_t k = s. size( );
   while( k && isdigit( s[k-1] ))
      -- k;

   std::ostringstream res;

   for( size_t j = 0; j < k; ++ j )
      res << s[j];

   if(i) res << i;
   return res. str( );
}


term subst( const term& t, const std::string& var, const term& val )
{
   if( t. sub. size( ) == 0 )
      return t. function == var ? val : t;
   else
   {
      term res = t;
      for( size_t i = 0; i < t. sub. size( ); ++ i )
         res. assign( i, subst( t.sub[i], var, val )); 

      return res; 
   }
}
 

fol subst( const fol& f, const std::string& var, const term& val )
{
   return try_in_sequence(
      [&f,&var,&val]( ) {  
         auto a = f. try_atom( );
         for( size_t i = 0; i < a. sub. size( ); ++ i )
            a. assign( i, subst( a.sub[i], var, val )); 
         return fol( std::move(a));
      },
      [&f,&var,&val]( ) 
      {  fol::equality e = f. try_equality( );
         e. assign( 0, subst( e[0], var, val ));
         e. assign( 1, subst( e[1], var, val )); 
         return fol( std::move(e) ); 
      }, 
      [&f,&var,&val]( ) 
      {  const auto& n = f. try_nullary( ); 
         return f;
      },
      [&f,&var,&val]( ) 
      {  fol::unary u = f. try_unary( );
         u. assign( 0, subst( u[0], var, val ));
         return fol( std::move(u) );
      },
      [&f,&var,&val]( )
      {   
         fol::binary b = f. try_binary( );
         b. assign( 0, subst( b[0], var, val ));
         b. assign( 1, subst( b[1], var, val )); 
         return fol( std::move(b)); 
      },
      [&f,&var,&val]( ) 
      {
         fol::quant q = f. try_quant( );

         // Easy case: var does not occur in f:
         // Writing this test everywhere, would not be an optimization,
         // because we already use CPR.
 
         if( !isfree( var, f ))
            return f;
 
         if( !isfree( q. var, val ))
         {
            // No capture.

            q. assign_body( subst( q. body( ), var, val ));
            return fol( std::move(q));
         }
         else
         {
            // Capture will happen, we need a fresh variable:  

            size_t i = 0;
            std::string fresh = appendindex( q. var, i );
            while( isfree( fresh, q. body( )) || 
                   isfree( fresh, val ))
            {
               ++ i;
               fresh = appendindex( q. var, i );
            }

            // Now we have a fresh variable.

            q. assign_body( subst( q. body( ), q. var, term( fresh )));
            q. var = fresh;

            // Finally, we can do what we already wanted to do so long,
            // with the body of the formula. 

            q. assign_body( subst( q. body( ), var, val ));

            return fol( std::move( q ));
         }
      },
      [&f,&var,&val]( ) 
      { 
         throw std::runtime_error( "in <<, could not match" ); 
                     return f; 
      }
   );
}


