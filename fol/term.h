
#ifndef TERM_INCLUDED
#define TERM_INCLUDED   1

#include <string>
#include "../shared/shared_vector.h"

struct term
{
   using builder = shared_vector< term > :: builder;

   std::string function;
   shared_vector< term > sub; 

   term( const std::string& function, std::initializer_list< term > sub  = {} )
      : function{ function }
   {
      builder b{ sub. size( ) };
      for( const auto& t : sub )
         b. push_back(t);
      ( this -> sub ) = std::move(b);
   } 
   
   term( const std::string& function, builder&& b )
      : function{ function },
        sub{ std::move(b) }
   { }

   using const_iterator = shared_vector< term > :: const_iterator;
   const_iterator begin( ) const { return sub. begin( ); }
   const_iterator end( ) const { return sub. end( ); }

   bool certainly_equal( const term& t ) const 
      { return function == t. function && sub. shared_with( t. sub ); }

   void assign( size_t i, const term& t );

   void* getaddress( ) const { return sub. getaddress( ); } 

   void print( std::ostream& out, unsigned int depth = 0 ) const; 
};

std::ostream& operator << ( std::ostream& out, const term& t );

#endif

