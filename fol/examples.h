
#ifndef EXAMPLES_INCLUDED
#define EXAMPLES_INCLUDED    1

// This implementation is not sophisticated.
// A good implementation would use a std::unordered_map< > +
// a stack.

#include "nondet.h"
#include "fol.h"

// Stack is used by alpha-equivalence.

class stack 
{
   std::vector< std::string > vect;

public: 
   // Returns size( ) if var does not occur, 
   // otherwise position of the last occurence of var, looking
   // back from the end of vect:

   size_t find( const std::string& var ) const;

   size_t size( ) const { return vect. size( ); }

   void push_back( const std::string& var ) { vect. push_back( var ); }
   void pop_back( ) { vect. pop_back( ); } 
   void restore( size_t k ) 
      { vect. erase( vect. begin( ) + k, vect. end( )); }

   using const_iterator = std::vector< std::string > :: const_iterator;
   const_iterator begin( ) const { return vect. begin( ); }
   const_iterator end( ) const { return vect. end( ); } 
};

std::ostream& operator << ( std::ostream& out, const stack& s );

bool isfree( const std::string& var, const term& t );
bool isfree( const std::string& var, const fol& f );
   // True if variable var is free in f.

bool alpha_equivalent( stack& s1, const term& t1,
                       stack& s2, const term& t2 );

bool alpha_equivalent( stack& s1, const fol& f1,
                       stack& s2, const fol& f2 );

inline bool alpha_equivalent( const fol& f1, const fol& f2 )
{
   stack s1; stack s2;
   return alpha_equivalent( s1, f1, s2, f2 );
}

std::string appendindex( const std::string& var, size_t nr );
   // Append decimal representation of nr to var, in order to get
   // a new variable. If var ends in digits, these are removed first.

term subst( const term& t, const std::string& var, const term& val );
fol subst( const fol& f, const std::string& var, const term& val );

#endif


