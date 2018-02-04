
#include <iostream>
#include <string>
#include <vector>

#include "term.h"


int main( int argc, char* argv[] ) 
{
   auto t1 = term( "p", 
      { term( "g", { term("a"), term("b") } ),
        term( "h", { term("b"), term("c") } ) } );

   auto ss = t1;
   ss = subst2( ss, "a", term( "bb" ));
   t1. print( std::cout );
   std::cout << "\n";
   ss. print( std::cout );
   std::cout << "\n";
 
   term aa{ "aaaa" };
   term bb{ "bbbb" };

   auto t2 = t1;
   auto t3 = t1[0]; 
   t3[1] = t3[0];
   t3. print( std::cout ); return 0;

   aa = term( "ff1", { aa } );
   aa = term( "ff2", { aa } ); 
   bb = term( "gg1", { bb } );
   bb = term( "gg2", { bb } ); 

   auto ff = term( "F", { aa, bb } ); 

   auto ff2 = ff;

   ff. print( std::cout );
   std::cout << ff[0][0] << "\n";
   ff2. print( std::cout );
   ff. print( std::cout );  
   return 0;

}


