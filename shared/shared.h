
#ifndef SHARED_INCLUDED
#define SHARED_INCLUDED  1

#include "shared_vector.h"

namespace shared 
{

   template< typename N, typename E = std::equal_to<N>>
   struct term 
   {
      N node;
      shared_vector<term> subterms;

      using builder = typename shared_vector<term> :: builder;

      template< typename A1 >
      explicit term( A1&& a1, const std::vector< term > & sub )
         : node{ std::forward<A1> ( a1 ) }, 
           shared_vector< term > ( sub )
      { }

      template< typename A1, typename A2 >
      explicit term( A1&& a1, A2&& a2, const std::vector< term > & sub )
         : node{ std::forward<A1> ( a1 ), std::forward<A2> ( a2 ) },
           shared_vector< term > ( sub )
      { }

      template< typename A >
      term( A&& a, std::initializer_list< term > subterms = { } )
         : node{ std::forward<A> ( a ) },
           subterms( builder( subterms ) )
      { 
         ( this -> subterms ) = builder( subterms ); 
      }

      term( const term& t ) = default; 
      term( term& t ) = default;
         // This is crazy, but unfortunately necessary because otherwise, 
         // one of the constructors above is picked.
      term( term&& t ) = default;

      term& operator = ( const term& ) = default;
      term& operator = ( term&& ) = default;

      size_t arity( ) const { return subterms. size( ); }
      const term& operator[] ( size_t i ) const { return subterms[i]; }
      const term& at( size_t i ) const { return subterms. at(i); }
      term& getuniquesub( size_t i ) { return subterms. getuniquesub(i); }

      bool hasontop( const N& n, size_t ar ) const
      {
         static E e;
         return e( node, n ) && ar == arity( ); 
      }

      bool certainly_equal( const term& t ) const 
      {
         static E e;
         return e( node, t. node ) && subterms. shared_with( t. subterms );
      }

      // Not sure if one should keep these methods:
 
      void assign( size_t i, const term& t ) 
      {
         if( (*this)[i]. certainly_equal(t) )
            return;

         // If we have an assignment of form t. assign( i, t ), 
         // we will create self-referential node if t is not shared.
         // This can be solved by copying t, after which it will 
         // share with *this, which will force a unique copy for *this. 

         if( subterms. shared_with( t. subterms ))
            subterms. getuniquesub(i) = term(t);
         else
            subterms. getuniquesub(i) = t;
      }


      void assign( size_t i, term&& t )
      {
         if( (*this)[i]. certainly_equal(t) )
            return;

         // See the comment in the other assign. 
         // I think it is reasonable to write t. assign( 0, std::move(t)), 
         // so one has to support it.  

         if( subterms. shared_with( t. subterms )) 
            subterms. getuniquesub(i) = term( std::move(t));
         else
            subterms. getuniquesub(i) = std::move(t);
      }

      
      void print( std::ostream& out, unsigned int depth = 0 ) const 
      { 
         for( size_t i = 0; i < depth; ++ i ) out << "   ";
         out << node << "         ";
         out << "{" << subterms. getaddress( ) << "/" << 
                       subterms. getrefcounter( ) << "}\n";

         for( size_t i = 0; i < arity( ); ++ i )
            (*this)[i]. print( out, depth + 1 );
      }      


      class context
      {
         struct path
         {
            const term<N,E> * current;
            size_t i;

            path( const term<N,E> * current, size_t i )
               : current{ current }, i{i}
            { }
         };

         context* parent;  

         union 
         {
            term<N,E> * nosharing; // No parent means nosharing can be changed
                                   // (guaranteed) without side effects.
            path sharing;          // Having a parent means that we are
                                   // i-th descendent of our parents current. 
         };
            
         context( term<N,E> * t )
            : parent{ nullptr },
              nosharing{t}
         { }

         context( context* parent, size_t i, const term<N,E> * current )
            : parent{ parent },
              sharing{ current, i } 
         { 
         }

      public:
         context( context&& ) = default;
         context( const context& ) = delete;
         context& operator = ( const context& ) = delete;

         const term<N,E> & getterm( ) const 
            { return parent ? *nosharing : *sharing. current; } 

         size_t arity( ) const 
            { return parent ? 
                 nosharing -> arity( ) : 
                 sharing. current -> arity( ); }

         context operator[] ( size_t i ) & ;
            // It must be impossible to take the [] of a temporary.

         void assign( term<N,E> t ); 
            // We don't want to use =, to stress the special nature.

         void print( std::ostream& out ) const;
            // For debugging, not intended to be pretty.

         void operator = ( term<N,E> t );  // Cannot be const&

         friend class term ; 
      };      

      friend context; 
      
      context top( ) { return context{ this }; } 
   };


   template< typename N, typename E > 
   std::ostream& operator << ( std::ostream& out, const term<N,E> & t )
   {
      out << t. node;
      if( t. arity( ))
      {
         out << "( ";
         for( size_t i = 0; i != t. arity( ); ++ i )
         {
            if(i) out << ", ";
            out << t[i]; 
         }
         out << " )";
      }
      return out;
   }


   template< typename N, typename E >
   typename term<N,E> :: context term<N,E> :: context::operator[] ( size_t i ) &
   {
      bool s = parent || getterm( ). subterms. isshared( );
         // If we already have a parent, or our own subterms are shared, 
         // then we store the parent.

      if(s) 
         return context( this, i, &this -> getterm( )[i] );
      else
         return context( &nosharing -> subterms. getuniquesub(i) );
   }


   template< typename N, typename E >
   void term<N,E> :: context::assign( term<N,E> t )
   {
      // Having a parent means that we cannot modify
      // our term, because it will create side effects. 

      auto p = this;
      while( p -> parent ) 
      {
         term copy_of_parent = ( p -> parent ) -> getterm( );
         size_t i = p -> sharing.i;

         copy_of_parent. subterms. getuniquesub(i) = t;

         context* par = p -> parent;

         // We switch the union of p into the other state,
         // because we are not sharing anymore:

         p -> parent = nullptr;
         p -> nosharing = & copy_of_parent. subterms. getuniquesub(i);
         
         p = par;
         t = copy_of_parent;             
      }

      *( p -> nosharing ) = t;
   }


   template< typename N, typename E > 
   void term<N,E> :: context::print( std::ostream& out ) const
   {
      out << "[ ";
      auto p = this;
      while( p -> parent ) 
      {
         if( p != this ) out << ", ";
         out << p -> getterm( ). node << ":";
         out << p -> sharing.i << "/" << p -> getterm( ). arity( );

         p = p -> parent; 
      }
      if( p != this ) out << ", ";
      out << p -> getterm( ). node; 
      out << " ]\n";
   }
 
}

#endif


