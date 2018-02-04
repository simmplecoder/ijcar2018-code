
#ifndef SUBST_INCLUDED
#define SUBST_INCLUDED


// This implementation is not sophisticated.
// A good implementation would use a std::unordered_map< > +
// a stack.

struct
{
   std::vector< std::string > stack;

   size_t find( const std::string& var ) const;
      // Returns size( ) if var does not occur, 
      // otherwise position of last occurence.

   size_t size( ) const { return stack. size( ); }

   void push_back( const std::string& var );
   void restore( size_t k );
}


