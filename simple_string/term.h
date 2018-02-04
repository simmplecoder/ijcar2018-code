
// Written by Hans de Nivelle, Part of IJCAR 2018 Contribution.
// Straightforward term implementation using std::vector< >.

#ifndef TERM_INCLUDED
#define TERM_INCLUDED  1

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

struct term;

struct heapnode
{
   std::string node;
   std::vector<term> subterms;
   size_t refcounter;

   heapnode( std::string&& node, std::vector< term > && subterms )
      : node{ std::move( node ) }, 
        subterms{ std::move( subterms ) }, refcounter{1} { }

   heapnode( const std::string& node, std::vector< term > && subterms )
      : node{ node },
        subterms{ std::move( subterms ) }, refcounter{1} { }

   heapnode( const heapnode& ) = delete;
   heapnode& operator = ( const heapnode& ) = delete;
};


class term
{
   heapnode* pntr;

public: 
   term( const std::string& node, 
         std::vector< term > && sub = std::vector<term> ())
      : pntr{ new heapnode( node, std::move( sub )) }
   { }

   term( std::string&& node,
         std::vector< term > && sub = std::vector<term> ( ))
      : pntr{ new heapnode( std::move(node), std::move( sub )) }
   { }


   term( const term& other ) noexcept
      : pntr{ other. pntr }
   {
      ++ pntr -> refcounter;
   }

   const term& operator = ( term&& other ) noexcept
      { std::swap( pntr, other. pntr ); 
        return *this; }

   const term& operator = ( const term& other ) noexcept 
      { return (*this) = term( other ); }

   ~term( )
   { 
      if( -- ( pntr -> refcounter ) == 0 ) 
         delete pntr;
   }

   const std::string& getnode( ) const { return pntr -> node; } 
   size_t arity( ) const { return pntr -> subterms. size( ); }
 
   const term& operator[] ( size_t i ) const
      { return pntr -> subterms[i]; }

#if 1
   // Wrong versions, should not be used: 

   std::string& getnode( ) 
      { ensure_not_shared( ); return pntr -> node; }
   term& operator[] ( size_t i ) 
      { ensure_not_shared( ); return pntr -> subterms[i]; } 
#endif

   bool certainly_equal( const term& t ) const { return pntr == t. pntr; } 
      
   // Using reference here is dangerous, in case of have t = t[i]. 

   void assign( size_t i, term t )
   { 
      if( ! pntr -> subterms[i]. certainly_equal(t))
         getuniquesub(i) = t; 
   } 

   term& getuniquesub( size_t i )
      { ensure_not_shared( ); return pntr -> subterms[i]; } 

   void print( std::ostream& out, unsigned int depth = 0 ) const;

private:
public: 
   void ensure_not_shared( )
   {
      if( pntr -> refcounter > 1 )
      {
         -- ( pntr -> refcounter );
         pntr = new heapnode{ pntr -> node, 
                              std::vector< term > ( pntr -> subterms )};
      }
   }

};

std::ostream& operator << ( std::ostream& out, const term& t );

term subst( const term& t, const std::string& node, const term& val );
term subst2( const term& t, const std::string& node, const term& val );

#endif 

