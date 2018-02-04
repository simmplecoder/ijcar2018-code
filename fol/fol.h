
// Written by Hans de Nivelle, part of IJCAR 2018 submission.

#ifndef FOL_INCLUDED
#define FOL_INCLUDED   1

#include <iostream>
#include "term.h"
#include "nondet.h" 
#include "../shared/shared_vector.h"


struct fol
{

   enum logop { fol_atom, fol_equals,
                fol_false, fol_true, 
                fol_not, 
                fol_and, fol_or, fol_implies, fol_equiv, 
                fol_forall, fol_exists };

   using builder = shared_vector< fol > :: builder;

   // The implementation is rather colorful, because we want 
   // to make the point that it is possible to be colorful in C++.
   //
   // All these subclasses should be exposed to the user:

   struct atom
   {
      logop op;

      std::string pred;
      shared_vector< term > sub;      // Of unknown length.

      atom( logop op, const std::string& pred, term::builder&& args )
         : op{ op },
           pred{ pred },
           sub{ std::move( args ) }
      { }  

      size_t nrsubterms( ) const { return sub. size( ); } 
      shared_vector< term > :: const_iterator sub_begin( ) const 
         { return sub. begin( ); } 
      shared_vector< term > :: const_iterator sub_end( ) const
         { return sub. end( ); } 

      const term& operator[] ( size_t i ) const { return sub[i]; }
      void assign( size_t i, term t ); 

      bool certainly_equal( const atom& a ) const
         { return op == a. op && pred == a. pred && 
                  sub. shared_with( a. sub ); }
   };

 
   struct equality
   {
      logop op;
      shared_vector< term > sub;  // Always length 2.

      equality( logop op, const term& t1, const term& t2 )
         : op{ op }
      {
         term::builder b{2};
         b. push_back( t1 );
         b. push_back( t2 );
         sub = std::move(b);
      }

      equality( logop op, term&& t1, term&& t2 )
         : op{ op }
      {
         term::builder b{2};
         b. push_back( std::move(t1) );
         b. push_back( std::move(t2) );
         sub = std::move(b);
      }

      const term& operator[] ( size_t i ) const { return sub[i]; }
      void assign( size_t i, term t );

      bool certainly_equal( const equality& eq ) const
         { return op == eq. op && sub. shared_with( eq. sub ); }
   };


   struct nullary 
   { 
      logop op; 

      nullary( logop op )
         : op{ op }
      { }

      bool certainly_equal( const nullary& n ) const 
         { return op == n. op; } 
   };

 
   struct unary  
   {
      logop op; 
      shared_vector< fol > sub;         // Obviously of length 1. 

      unary( logop op, const fol& f )
         : op{ op }
      {
         builder b{1};
         b. push_back(f);
         sub = std::move(b); 
      } 
          
      unary( logop op, fol&& f )
         : op{ op }
      {
         builder b{1};
         b. push_back( std::move(f) );
         sub = std::move(b);
      }

      const fol& operator[]( size_t i ) const { return sub[i]; } 
      void assign( size_t i, fol f );
      bool certainly_equal( const unary& u ) const
         { return op == u. op && sub. shared_with( u. sub ); } 
   };


   struct binary
   {
      logop op; 
      shared_vector<fol> sub;  // Always length 2.
         // One could introduce an additional type shared_array<2,fol>,
         // but I am not sure if it is useful. 

      binary( logop op, const fol& f1, const fol& f2 )
         : op{ op }
      {
         builder b{2};
         b. push_back( f1 );
         b. push_back( f2 );
         sub = std::move(b);
      }

      binary( logop op, fol&& f1, fol&& f2 )
         : op{ op }
      {
         builder b{2};
         b. push_back( std::move(f1) );
         b. push_back( std::move(f2) );
         sub = std::move(b);
      }

      const fol& operator[]( size_t i ) const { return sub[i]; } 
      void assign( size_t i, fol f );
      bool certainly_equal( const binary& b ) const
         { return op == b. op && sub. shared_with( b. sub ); }  
   };


   struct quant
   {
      logop op;

      std::string var;
      shared_vector< fol > sub;   
         // Always size 1.

      quant( logop op, const std::string& var, const fol& f )
         : op{ op },
           var{ var } 
      {
         builder b{1};
         b. push_back(f);
         sub = std::move(b);
      }

      quant( logop op, const std::string& var, fol&& f )
         : op{ op },
           var{ var } 
      {
         builder b{1};
         b. push_back( std::move(f) );
         sub = std::move(b);
      }  

      const fol& body( ) const { return sub[0]; } 
      void assign_body( fol f );
      bool certainly_equal( const quant& q ) const
         { return op == q. op && var == q. var && 
                  sub. shared_with( q. sub ); }

   };


   // Using std::variant would be much nicer here, but we don't have that
   // yet.

   // Note that all members of the union share logop op, which is 
   // therefore a 'common initial sequence'. This is important,
   // because we always need to be able to take the op of the union,
   // so that we can see what is inside. 

   union 
   {
      atom at;
      equality eq;
      nullary sub0;
      unary sub1;
      binary sub2;
      quant q;  
   };

   // Each of the constructors can be moving or not moving.

   fol( const atom& at )
      : at{ at }
   { }

   fol( atom&& at )
      : at{ std::move(at) }
   { }

   fol( const equality& eq )
      : eq{ eq }
   { }

   fol( equality&& eq )
      : eq{ std::move( eq ) } 
   { } 

   fol( const nullary& sub0 )
      : sub0{ sub0 }
   { }

   fol( nullary& sub0 )
      : sub0{ std::move( sub0 ) }
   { } 
 
   fol( const unary& sub1 )
      : sub1{ sub1 }
   { }

   fol( unary&& sub1 )
      : sub1{ std::move( sub1 ) }  
   { } 

   fol( const binary& sub2 )
      : sub2{ sub2 }
   { }

   fol( binary&& sub2 )
      : sub2{ std::move(sub2) }
   { }

   fol( const quant& q )
      : q{q}
   { }

   fol( quant&& q )
      : q{ std::move(q) }
   { }


   fol( const fol& f );
   fol( fol&& f );

   const fol& operator = ( const fol& ); 
   const fol& operator = ( fol&& );
    
   ~fol( ); 

   logop getop( ) const { return at. op; }; 

   // Selection Functions (throwing): 

   const atom& try_atom( ) const;
   const equality& try_equality( ) const;
   const nullary& try_nullary( ) const; 
   const unary& try_unary( ) const;
   const binary& try_binary( ) const;
   const quant& try_quant( ) const;

   bool certainly_equal( const fol& f ) const;

private:
   void* getaddress( ) const;
      // This function exists for debugging only, so that
      // one can verify the allocation strategy. 
      // Don't use it for anything else! 

public:
   void print( std::ostream& out, unsigned int depth = 0 ) const; 
   
};


std::ostream& operator << ( std::ostream& out, fol::logop op );
std::ostream& operator << ( std::ostream& out, const fol& f );

#endif

