
#include "fol.h"

void fol::atom::assign( size_t i, term t )
{
   if( !sub[i]. certainly_equal(t))
      sub. getuniquesub(i) = t;
}

void fol::equality::assign( size_t i, term t )
{
   if( !sub[i]. certainly_equal(t))
      sub. getuniquesub(i) = t;
}

void fol::unary::assign( size_t i, fol f )
{
   if( !sub[i]. certainly_equal(f))
      sub. getuniquesub(i) = f;
}

void fol::binary::assign( size_t i, fol f )
{
   if( !sub[i]. certainly_equal(f))
      sub. getuniquesub(i) = f; 
}

void fol::quant::assign_body( fol f )
{
   if( !sub[0]. certainly_equal(f))
      sub. getuniquesub(0) = f;
}
 
fol::fol( const fol& f )
{
   // std::cout << "fol::fol " << f. sub0. op << "\n";

   switch( f. sub0. op )
   {
      case fol_atom:
         new (&at) atom( f. at ); return;  

      case fol_equals:
         new (&eq) equality( f. eq ); return; 

      case fol_false:
      case fol_true:
         new (&sub0) nullary( f. sub0 ); return;

      case fol_not:
         new (&sub1) unary( f. sub1 ); return;

      case fol_and:
      case fol_or:
      case fol_implies:
      case fol_equiv:
         new (&sub2) binary( f. sub2 ); return;  

      case fol_forall:
      case fol_exists:
         new( &q ) quant( f. q ); return; 
   }
}


fol::fol( fol&& f )
{
   switch( f. sub0. op )
   {
      case fol_atom:
         new (&at) atom( std::move( f. at )); break;            

      case fol_equals:
         new (&eq) equality( std::move( f. eq )); break; 

      case fol_false:
      case fol_true:
         new (&sub0) nullary( std::move( f. sub0 )); break;

      case fol_not:
         new (&sub1) unary( std::move( f. sub1 )); break;

      case fol_and:
      case fol_or:
      case fol_implies:
      case fol_equiv:
         new (&sub2) binary( std::move( f. sub2 )); break; 

      case fol_forall:
      case fol_exists:
         new( &q ) quant( std::move( f. q )); break; 
      default:
         throw std::runtime_error( "impossible" ); 
   }

   // I believe that moving out of f is allowed to
   // change f. op, which would bring the union into an undefined state,
   // which cannot be destroyed or overwritten. 
   // Of course, in practice, it will never happen, but we have
   // to insert this back assignment. 

   f.at.op = at.op;
}


const fol& fol::operator = ( const fol& f )
{
   // We fall through if our state (of our union) differs
   // from the state of f. When that happens, we destroy ourselves,
   // and copy-construct from f. In that case, we are certainly
   // not a self assignment. 

   switch( getop( ))
   {
   case fol_atom:
      if( f. getop( ) == fol_atom ) 
      {
         at = f. at; return *this;
      }
      break;

   case fol_equals:
      if( f. getop( ) == fol_equals ) 
      {
         eq = f. eq; return *this;
      }
      break;

   case fol_false:
   case fol_true:
      if( f. getop( ) == fol_false ||
          f. getop( ) == fol_true )
      {
         sub0 = f. sub0; return *this;
      }
      break;

   case fol_not:
      if( f. getop( ) == fol_not )
      {
         sub1 = f. sub1; return *this;
      }
      break; 

   case fol_and:
   case fol_or:
   case fol_implies:
   case fol_equiv:
      if( f. getop( ) == fol_and ||
          f. getop( ) == fol_or ||
          f. getop( ) == fol_implies ||
          f. getop( ) == fol_equiv )
      {
         sub2 = f. sub2; return *this; 
      }
      break; 

   case fol_forall:
   case fol_exists:
      if( f. getop( ) == fol_forall ||
          f. getop( ) == fol_exists )
      {
         q = f. q; return *this; 
      }
      break;
   }

   this -> ~fol( ); 
   new (this) fol{f}; 
   return *this; 
}


const fol& fol::operator = ( fol&& f )
{
   // We fall through if our state (of our union) differs
   // from the state of f. When that happens, we destroy ourselves,
   // and copy-construct from f. In that case, we are certainly
   // not a self assignment. 

   switch( getop( ))
   {
   case fol_atom:
      if( f. getop( ) == fol_atom ) 
      {
         at = std::move( f. at ); return *this;
      }
      break;

   case fol_equals:
      if( f. getop( ) == fol_equals ) 
      {
         eq = std::move( f. eq ); return *this;
      }
      break;

   case fol_false:
   case fol_true:
      if( f. getop( ) == fol_false ||
          f. getop( ) == fol_true )
      {
         sub0 = std::move( f. sub0 ); return *this;
      }
      break;

   case fol_not:
      if( f. getop( ) == fol_not )
      {
         sub1 = std::move( f. sub1 ); return *this;
      }
      break; 

   case fol_and:
   case fol_or:
   case fol_implies:
   case fol_equiv:
      if( f. getop( ) == fol_and ||
          f. getop( ) == fol_or ||
          f. getop( ) == fol_implies ||
          f. getop( ) == fol_equiv )
      {
         sub2 = std::move( f. sub2 ); return *this; 
      }
      break; 

   case fol_forall:
   case fol_exists:
      if( f. getop( ) == fol_forall ||
          f. getop( ) == fol_exists )
      {
         q = std::move( f. q ); return *this; 
      }
      break;
   }

   this -> ~fol( ); 
   new (this) fol{ std::move(f) }; 
   return *this; 
}


fol::~fol( ) 
{
   // std::cout << "destructor for " << sub0. op << "\n";

   switch( sub0. op )
   {
      case fol_atom:
         at.~atom( ); return;

      case fol_equals:
         eq.~equality( ); return; 

      case fol_false:
      case fol_true:
         sub0. ~nullary( ); return; 
  
      case fol_not:
         sub1. ~unary( ); return;
 
      case fol_and:
      case fol_or:
      case fol_implies:
      case fol_equiv:
         sub2. ~binary( ); return;

      case fol_forall: 
      case fol_exists:
         q. ~quant( ); return; 
   }
}

// Selection Functions (throwing):

const fol::atom& fol::try_atom( ) const
{
   if( getop( ) != fol::fol_atom ) throw failure( );
   return at;
}

const fol::equality& fol::try_equality( ) const
{
   if( getop( ) != fol::fol_equals ) throw failure( );
   return eq; 
}

const fol::nullary& fol::try_nullary( ) const 
{
   if( getop( ) != fol::fol_false &&
       getop( ) != fol::fol_true ) throw failure( );

   return sub0;
}

const fol::unary& fol::try_unary( ) const
{
   if( getop( ) != fol::fol_not ) throw failure( );

   return sub1;
}

const fol::binary& fol::try_binary( ) const
{
   if( getop( ) != fol::fol_and &&
       getop( ) != fol::fol_or &&
       getop( ) != fol::fol_implies &&
       getop( ) != fol::fol_equiv ) throw failure( );

   return sub2;
}

const fol::quant& fol::try_quant( ) const
{
   if( getop( ) != fol::fol_forall &&
       getop( ) != fol::fol_exists ) throw failure( );

   return q;
}
 

std::ostream& operator << ( std::ostream& out, fol::logop op )
{
   switch(op)
   {
      case fol::fol_atom: out << "atom"; return out;
      case fol::fol_equals: out << "equals"; return out;
      case fol::fol_false: out << "false"; return out;
      case fol::fol_true: out << "true"; return out; 
      case fol::fol_not: out << "not"; return out; 
      case fol::fol_and: out << "and"; return out;
      case fol::fol_or: out << "or"; return out;
      case fol::fol_implies: out << "implies"; return out;
      case fol::fol_equiv: out << "equiv"; return out; 
      case fol::fol_forall: out << "forall"; return out; 
      case fol::fol_exists: out << "exists"; return out; 
      default: out << "undefined"; return out; 
   }
}



bool fol::certainly_equal( const fol& f ) const 
{
   if( getop( ) != f. getop( )) 
      return false;

   switch( getop( ))
   {
   case fol_atom:
      return at. certainly_equal( f. at );

   case fol_equals:
      return eq. certainly_equal( f. eq ); 

   case fol_false:
   case fol_true:
      return sub0. certainly_equal( f. sub0 ); 

   case fol_not:
      return sub1. certainly_equal( f. sub1 ); 

   case fol_and:
   case fol_or:
   case fol_implies:
   case fol_equiv:
      return sub2. certainly_equal( f. sub2 ); 

   case fol_forall:
   case fol_exists:
      return q. certainly_equal( f. q );
   }
   throw std::runtime_error( "fall through" );

}

void* fol::getaddress( ) const
{
   switch( getop( ))
   {
   case fol_atom:
      return at. sub. getaddress( );

   case fol_equals:
      return eq. sub. getaddress( );

   case fol_false:
   case fol_true:
      return nullptr; 

   case fol_not:
      return sub1. sub. getaddress( ); 

   case fol_and:
   case fol_or:
   case fol_implies:
   case fol_equiv:
      return sub2. sub. getaddress( );

   case fol_forall:
   case fol_exists:
      return q. sub. getaddress( ); 
   }
   throw std::runtime_error( "fall through" );

}




std::ostream& operator << ( std::ostream& out, const fol& f )
{
   return try_in_sequence(
      [&out,&f]( ) {  
         auto a = f. try_atom( );
         out << a. pred;
         if( a. nrsubterms( ))
         {
            out << "( ";
            for( auto p = a. sub_begin( ); p != a. sub_end( ); ++ p )
            {
               if( p != a. sub_begin( ))
                  out << ", ";
               out << *p;
            }
            out << " )";

         }
         return std::ref(out);
      },
      [&out,&f]( ) 
      {  auto e = f. try_equality( );
         out << "( " << e[0] << " == " << e[1] << " )";
         return std::ref(out); 
      },
      [&out,&f]( ) 
      {  auto n = f. try_nullary( );
         out << n. op;
         return std::ref(out);
      },
      [&out,&f]( ) 
      {  auto u = f. try_unary( );
         out << "( " << u. op << " " << u[0] << " )";
         return std::ref(out); 
      },
      [&out,&f]( ) {   
         auto b = f. try_binary( );
         out << "( " << b[0] << " " << b. op << " " << b[1] << " )";
         return std::ref(out);
      },
      [&out,&f]( ) {
         auto q = f. try_quant( );
         out << "( " << q. op << " " << q. var << " : " << q. body( ) << " )";
         return std::ref(out);
      },
      [&out,&f]( ) { throw std::runtime_error( "in <<, could not match" ); 
                     return std::ref(out); }
   );

}


void fol::print( std::ostream& out, unsigned int depth ) const 
{
   for( unsigned int i = 0; i < depth; ++ i )
      out << "   ";

   out << "{" << 
   ( reinterpret_cast< long unsigned int > ( getaddress( ) ) & 0XFFFF ) 
    << "}:";

   try_in_sequence(
      [&out,this,depth]( ) {  
         auto a = this -> try_atom( );
         out << a. pred << "\n";
         for( auto p = a. sub_begin( ); p != a. sub_end( ); ++ p )
            p -> print( out, depth+1 );
      },
      [&out,this,depth]( ) 
      {  auto e = this -> try_equality( );
         out << "==" << "\n";
         e[0]. print( out, depth+1 ); 
         e[1]. print( out, depth+1 ); 
      },
      [&out,this,depth]( ) 
      {  auto n = this -> try_nullary( );
         out << n. op << "\n";
      },
      [&out,this,depth]( ) 
      {  auto u = this -> try_unary( );
         out << u. op << "\n";
         u[0]. print( out, depth+1 ); 
      },
      [&out,this,depth]( ) {   
         auto b = this -> try_binary( );
         out << b. op << "\n";
         b[0]. print( out, depth+1 ); 
         b[1]. print( out, depth+1 ); 
      },
      [&out,this,depth]( ) {
         auto q = this -> try_quant( );
         out << q. op << " " << q. var << "\n";
         q. body( ). print( out, depth+1 );
      },
      [&out,this, depth]( ) 
         { throw std::runtime_error( "in print( ), could not match" ); }
   );

}



