
// This is partially an adaption of lazy_list from 2007.
// It was written during April-July 2017. 

#ifndef SHARED_VECTOR_INCLUDED
#define SHARED_VECTOR_INCLUDED  1

#include <list>
#include <vector> 
#include <stdexcept>
#include <iostream>

template< typename D > 
class shared_vector 
{

   template< size_t s > 
   struct share
   {
      size_t refcounter; 
      D dd[s];
   };


   size_t ss;            // Our size.
   share<1> *dd;         // Present only when ss != 0.
                         // True type is share<ss> ; 

   // Builder class exists because of the following reasons:
   // 1. To allow more flexibility in construction of shared_vector.
   //    For example, one may want to construct a shared_vector from
   //    two containers, or a single D and a container<D>. 
   //    The user can create a builder and insert the elements.
   //    After that, the shared_vector can be constructed.
   // 2. Any other way of constructing shared_vector is not exception
   //    safe. If D holds resources, and construction of the i-th (i > 1)
   //    D fails, we need to clean up D[0 .. i-1]. This can be done only 
   //    with RAI, so we need an extra object. 


public: 
   class builder
   {
      share<1> * dd;    // Builder does not use the refcounter in dd. 
      size_t ss;        // Current size.
      size_t cc;        // Capacity.

   public: 
      // Number of bytes that we will allocate 
      // for given ss. 
      // sizeof( share<s> ) =should= allocsize(s) .

      static constexpr size_t allocsize( size_t ss )
      {
         return ( ss == 1 ) ? sizeof( share<1> )
            : sizeof( share<2> ) + ( ss - 2 ) * sizeof(D);
      }


      // Builder with given capacity:

      explicit builder( size_t cc )
         : dd{ nullptr },
           ss{ 0 },
           cc{ cc }
      {
         // It is a well-known fact that every property that holds for
         // 1,2,3,4,5, holds for all natural numbers:

         static_assert( sizeof( share<1> ) == allocsize(1), "failure at 1" );
         static_assert( sizeof( share<2> ) == allocsize(2), "failure at 2" );
         static_assert( sizeof( share<3> ) == allocsize(3), "failure at 3" );
         static_assert( sizeof( share<4> ) == allocsize(4), "failure at 4" );
         static_assert( sizeof( share<5> ) == allocsize(5), "failure at 5" );

         if( cc == 0 ) return;
   
         if( cc == 1 )
            dd = static_cast< share<1> * > ( operator new( sizeof( share<1> )));
         else
            dd = static_cast< share<1> * > ( operator new( allocsize( cc )));

         // If we don't get the memory, nothing happens. 
      }

      // I think it is nicer to put all shared_vector constructors
      // here.

      template <typename ForwardIterator>
      builder(ForwardIterator first, ForwardIterator last) :
           builder(static_cast<std::size_t>(std::distance(first, last)))
      {
          push_back(first, last);
      }

      builder( std::initializer_list<D> init )
         : builder(init.begin(), init.end())
      {}

      
      builder( builder&& b ) noexcept
         : dd{ b.dd },
           ss{ b.ss },
           cc{ b.cc }
      {
         b.dd = nullptr;
         b.ss = 0;
         b.cc = 0;
      }

      // builder( const std::vector<D> & vect );

      builder( const builder& b ) = delete;

      const builder& operator = ( builder&& b ) 
      {
         std::swap( dd, b.dd );
         std::swap( ss, b.ss );
         std::swap( cc, b.cc );

         return *this;
      }
      
      const builder& operator = ( const builder& b ) = delete; 

      // Should one check? Currently, we don't.

      void push_back( const D& d )
      {
         new( dd -> dd + ss ) D(d);
         ++ ss; 
      }          

      void push_back( D&& d )
      {
         new( dd -> dd + ss ) D( std::move(d) );
         ++ ss; 
      }

      // .... Add an emplace_back operator when you are in the mood. 

      template< typename I > void push_back( I i0, I i1 )
      {
         while( i0 != i1 )
         {   
            new( dd -> dd + ss ) D( *i0 );
            ++ i0; ++ ss;
         }
      }

      void print( std::ostream& out ) const
      {
         std::cout << "builder:\n";
         for( size_t i = 0; i != ss; ++ i )
            out << "   " << ( dd -> dd[i] ) << "\n";
         out << "capacity = " << cc << "\n";
      }

      void destroy( )
      {
         if(dd)
         {
            std::cout << "destroying with " << ss << "\n";
            for( size_t i = 0; i != ss; ++ i )
               ( dd -> dd + i ) -> ~D( ); 
            operator delete( dd );
         }
      } 
   
      ~builder( ) 
         { destroy( ); }

      friend class shared_vector ;
   };

public: 
   shared_vector( ) noexcept
      : ss{0},
        dd{nullptr}
   { } 

   // The builder must be full, which means that capacity == size.

   shared_vector( builder&& b ) 
      : ss{ b. ss },
        dd{ b. dd }
   {
      if(dd) dd -> refcounter = 1;
      if( b. ss != b. cc )
      {
         // What do we do now?
         // Our own destructor will be not called, and
         // the destructor of b will be called automatically.
         // So, we just throw the exception. 

         throw std::runtime_error( "builder is incomplete" );
      }

      b. dd = nullptr;
      b. ss = 0;
      b. cc = 0;
   }

   template <typename ForwardIterator>
   shared_vector(ForwardIterator first, ForwardIterator last) :
           shared_vector(builder(first, last))
   {}
          
#if 0
   shared_vector( std::initializer_list<D> init )
      : ss{0},
        dd{nullptr} 
   { 
      builder b( init. size( ));
      b. push_back( init. begin( ), init. end( ));
      *this = std::move(b);
   }

   shared_vector( const std::vector<D> & vect )
      : ss{0},
        dd{nullptr} 
   {
      builder b( vect. size( ));
      b. push_back( vect. begin( ), vect. end( ));
      *this = std::move(b);
   }

   shared_vector( std::vector<D> && vect )
      : ss{0},
        dd{nullptr} 
   { 
      builder b( vect. size( ));
      b. push_back( make_move_iterator( vect. begin( )), 
                    make_move_iterator( vect. end( ))); 
      *this = std::move(b);
   }

   shared_vector( const std::list<D> & lst )
      : ss{0},
        dd{nullptr}
   { 
      builder b( lst. size( ));
      b. push_back( lst. begin( ), lst. end( ));
      *this = std::move(b);
   }

   shared_vector( std::list<D> && lst )
      : ss{0},
        dd{nullptr}
   {  builder b( lst. size( ));
      b. push_back( make_move_iterator( lst.begin( )),
                    make_move_iterator( lst. end( )));
      *this = std::move(b);
   }
#endif 

   shared_vector( const shared_vector& other ) noexcept;
   shared_vector( shared_vector&& other ) noexcept;
   void operator = ( shared_vector&& other ) noexcept;
   void operator = ( const shared_vector& other ) noexcept
      { *this = shared_vector( other ); } 
   ~shared_vector( );

   const D& front( ) const          { return dd -> dd[0]; } 
   const D& back( ) const           { return dd -> dd[ss-1]; } 

   size_t size( ) const { return ss; } 

   using const_iterator = const D* ;
   const_iterator begin( ) const { return dd -> dd; }
   const_iterator end( ) const { return dd -> dd + ss; }

   const D& operator[] ( size_t i ) const { return dd -> dd[i]; } 
   const D& at( size_t i ) const 
      {  if( i >= ss ) throw std::out_of_range( "shared_vector::at" );
         return dd -> dd[i];
      }

   // true if we and tr, can be traced back to the same
   // point of construction, or both are empty.
   // This function is useful for 
   // implementation of rewriting, for checking that a 
   // replacement happened.  

   bool shared_with( const shared_vector& other ) const
      { return dd == other. dd; }

   // A subterm that one can safely assign to.
   // Allowing non-const [] creates problems. 

   D& getuniquesub( size_t i )
   {
      make_not_shared( );
      return dd -> dd[i];
   }

   bool isshared( ) const
   {
      return ss && ( dd -> refcounter > 1 );
   }

   // So that it can be printed. Don't use it for anything else!

   void* getaddress( ) const
   {  if( ss == 0 ) 
         return nullptr;
      else
         return & ( dd -> dd[0] );
   }
 
   // So that it can be printed. Don't use it for anything else.

   size_t getrefcounter( ) const
   {
      if( ss == 0 )  
         return 0;
      else
         return dd -> refcounter;
   }

private: 
public:  
   void make_not_shared( );
};


template< typename D >
shared_vector<D> :: shared_vector( const shared_vector& other ) noexcept
   : ss{ other. ss },
     dd{ other. dd }
{
   if(ss) 
      ++ ( dd -> refcounter ); 
}

template< typename D >
shared_vector<D> :: shared_vector( shared_vector&& other ) noexcept
   : ss{ other. ss }, 
     dd{ other. dd }
{
   other. ss = 0;
   other. dd = nullptr;
}

template< typename D >
void shared_vector<D> :: operator = ( shared_vector&& other ) noexcept
{
   std::swap( ss, other. ss );
   std::swap( dd, other. dd );
}

template< typename D >
shared_vector<D> :: ~shared_vector( )
{
   if(ss)
   {
      if( -- ( dd -> refcounter ) == 0 ) 
      {
         for( size_t i = 0; i != ss; ++ i )
            ( dd -> dd + i ) -> ~D( );
         operator delete( dd );
      }
   }
}



template< typename D >
void shared_vector<D> :: make_not_shared( )
{
   if( isshared( )) 
   {
      builder b{ ss };

      // the push_back part may throw, but it should be OK,
      // because we didn't change anything in *this yet. 

      b. push_back( begin( ), end( ));  
    
      // The rest cannot throw, we are sure that ss > 0.

      -- ( dd -> refcounter );

      dd = b.dd; 
      dd -> refcounter = 1;

      b.dd = nullptr;
      b.ss = 0;
      b.cc = 0;
   }
}

#endif 


