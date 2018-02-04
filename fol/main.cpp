
#include <iostream>
#include <string>
#include <vector>

#include "fol.h"
#include "nondet.h" 
#include "examples.h"


void iszero( unsigned int n )
{
   if( n != 0 ) throw failure( );
}

unsigned int issucc( unsigned int n )
{
   if( n == 0 ) throw failure( );
   return n-1;
}

unsigned int fact( unsigned int n )
{
   return try_in_sequence(
      [n]{ iszero(n); return 1; },
      [n]{ auto m = issucc(n); return n * fact(m); }
   );
}


int main( int argc, char* argv[] )
{
   term a = term( "a" );
   term b = term( "b" );
   term c = term( "c" ); 
 
   term fabc = term( "f", { a, b, b, term( "yy" ), term( "yy1" ) } ); 
   term gaab = term( "g", { a, c, b } );

   term tt = term ( "xx", { term( "yy" ), term( "yy2" ) } );
  
   fol::atom a1 = fol::atom( fol::fol_atom, "p", { fabc, a, b } );
   fol::atom a2 = fol::atom( fol::fol_atom, "q", { b, gaab, a } );
   fol::equality eq = fol::equality( fol::fol_equals, fabc, gaab );

   std::cout << "starting formula:";

   fol ff = fol::binary( fol::fol_and, 
               fol::unary( fol::fol_not, a2 ),
               fol::unary( fol::fol_not, eq ));

   ff = fol::binary( fol::fol_or, a1, ff );

   ff = fol::quant( fol::fol_exists, "yy", ff );

   ff. print( std::cout ); 
   std::string var = "c";
   term val = tt;

   std::cout << "Result of substitution[ " << var << " := " << val << "\n";

   subst( ff, var, val ). print( std::cout ); 

   std::cout << "this should be the original formula unchanged:\n";
   ff. print( std::cout );

   return 0;
 
}


