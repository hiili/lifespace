/*
 * Copyright (C) 2004-2005 Paul J. Wagner
 * This file is part of the Lifespace Simulator.
 * 
 * Lifespace Simulator is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Lifespace Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with the Lifespace Simulator; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * For more information about the program:
 *   http://www.cis.hut.fi/pwagner/lifespace/
 */

/**
 * @file src/types.hpp
 *
 * General type and utility classes and macros used throughout the Lifespace
 * Simulator.
 */
#ifndef LS_TYPES_HPP
#define LS_TYPES_HPP


//#define _GNU_SOURCE
//#define _ISOC99_SOURCE


#include <boost/numeric/ublas/blas.hpp>
#include <boost/static_assert.hpp>
#include <boost/shared_ptr.hpp>
#include <cmath>
#include <limits>
#include <memory.h>
#include <cassert>
#include <iostream>




namespace lifespace {
  
  
  /** Abbreviate some namespaces. */
  namespace ublas = boost::numeric::ublas;
  
  
  /** Define the real number precision used in the whole Lifespace
      Simulator. */
  typedef float real;
  
  
}   /* namespace lifespace */




/* Some constants and macros. */
#define EPS         (256.0*std::numeric_limits<lifespace::real>::epsilon())
#define SQUARE(x)   ((x)*(x))
#define FRAND01()   ((lifespace::real)rand() / (lifespace::real)RAND_MAX)


/* Aliases for the first three dimensions. */
#define DIM_X   0
#define DIM_Y   1
#define DIM_Z   2


/* Make sure that NAN and INFINITY are defined. */

// NAN
#ifndef NAN
namespace types_hpp_static_asserts {
  BOOST_STATIC_ASSERT(std::numeric_limits<lifespace::real>::has_quiet_NaN);
}
#define NAN (std::numeric_limits<lifespace::real>::quiet_NaN())
#endif

// INFINITY
#ifndef INFINITY
namespace types_hpp_static_asserts {
  BOOST_STATIC_ASSERT(std::numeric_limits<lifespace::real>::has_infinity);
}
#define INFINITY (std::numeric_limits<lifespace::real>::infinity())
#endif


/*
 * Define some debugging macros
 *
 * The msg argument of assert_user() is fed directly to an ostream, so anything
 * understood by them, including << operators, can be used here (but be careful
 * and keep in mind that this is implemented as a macro).
 */

#ifdef __GNUC__
#define debug_where "function " << __PRETTY_FUNCTION__ <<       \
  ", file " << __FILE__ << ", line " << __LINE__
#else
#define debug_where "file " << __FILE__ << ", line " << __LINE__
#endif

#ifdef NDEBUG
#define assert_internal(exp) ;
#define assert_user(exp,msg) ;
#else
#define assert_internal(exp)                                            \
  if(!( exp )) {                                                        \
    std::cerr                                                           \
      << std::endl                                                      \
      << "Internal error:" << std::endl                                 \
      << "  assertion failed: " #exp << std::endl                       \
      << "  in " << debug_where << std::endl;                           \
    abort();                                                            \
  }
#define assert_user(exp,msg)                                            \
  if(!( exp )) {                                                        \
    std::cerr                                                           \
      << std::endl                                                      \
      << "Error at " << debug_where << ":" << std::endl                 \
      << msg << std::endl;                                              \
    abort();                                                            \
  }
#endif




namespace lifespace {
  
    
  /** Defines an operation or transformation direction (relative to absolute or
      absolute to relative). */
  enum Direction { Normal, Reverse, Rel2Abs = Normal, Abs2Rel = Reverse };
  
  
  
  
  /* shared_ptr helpers */
  
  
  /**
   * A null deleter for some smart pointer programming techiques described at
   * boost.org.
   *
   * @note
   * This is copied directly from the Smart Pointer Programming Techniques
   * section at boost.org.
   */
  struct null_deleter
  { void operator()( void const * ) const {} };
  
  
  /**
   * A helper class for obtaining weak pointers from objects not managed by
   * shared_ptrs. Additionally the internal shared_ptr is checked (with
   * assert()) for uniqueness upon destruction (note that this is not a
   * bullet-proof check in a multi-threaded application: a dangling pointer is
   * created if a weak_ptr is converted into a shared_ptr after the assertion
   * but before the internal shared_ptr's destruction).
   *
   * @note
   * This is an implementation of a technique described in the Smart Pointer
   * Programming Techniques section at boost.org.
   */
  template<class T>
  class enable_get_weak_ptr
  {
    boost::shared_ptr<T> this_;
    
  public:
    enable_get_weak_ptr() :
      this_( (T *)this, null_deleter() ) {}
    enable_get_weak_ptr( const enable_get_weak_ptr & rhs ) :
      this_( (T *)this, null_deleter() ) {}
    ~enable_get_weak_ptr()
    { assert( this_.unique() ); }
    
    enable_get_weak_ptr & operator=( const enable_get_weak_ptr & rhs )
    { return *this; }
    
    boost::weak_ptr<T> get_weak_ptr() { return this_; }
    boost::weak_ptr<const T> get_weak_ptr() const { return this_; }
  };
  
  
  /**
   * Quick conversion from raw pointers to shared pointers.
   *
   * This helper function exists only to make it possible to shorten, if
   * wanted, the following kind of code (which is \em very common in this
   * library):
   * @code
   *   SomeClass someClass
   *   ( shared_ptr<CompositeObjectType1>( new CompositeObjectType1(...) ),
   *     shared_ptr<CompositeObjectType2>( new CompositeObjectType2(...) ) );
   * @endcode
   * into this:
   * @code
   *   SomeClass someClass( sptr( new CompositeObjectType1( ... ) ),
   *                        sptr( new CompositeObjectType2( ... ) ) );
   * @endcode
   * Null shared pointers must still be created explicitly.
   *
   * Note that objects managed by shared pointers are not always meant to be
   * shared among multiple owners: shared pointers are also used to track the
   * existence of external pointers to a normal locally owned object, so that
   * null-pointer bugs can be detected. See the related page 'Conventions' in
   * the doxygen documentation for details.
   */
  template<class T>
  boost::shared_ptr<T> sptr( T * ptr )
  { return boost::shared_ptr<T>( ptr ); }
  
  /**
   * Quick conversion from raw pointers to shared pointers with a custom
   * deleter. See above for details.
   */
  template<class T, class D>
  boost::shared_ptr<T> sptr( T * ptr, D deleter )
  { return boost::shared_ptr<T>( ptr, deleter ); }
  
  
  /**
   * Explicit deletion of shared_ptr -managed objects. The provided shared_ptr
   * must be the last reference to the object. This is checked with an
   * assertion. Note that the argument type is a \em reference to the
   * shared_ptr. The provided pointer will be an empty shared_ptr when this
   * call returns.
   */
  template<class T>
  void delete_shared( boost::shared_ptr<T> & ptr )
  {
    assert_user( ptr.unique(),
                 "Active references to the object to be deleted "
                 "still exist somewhere!" );
    ptr.reset();
  }
  
  
  
  
  /* STL helpers */
  
  
  /**
   * A generic deleter that can be used with for_each().
   */
  template<class T>
  class deleter {
  public:
    void operator()( T * ptr ) { delete ptr; }
  };
  
  
  
  
  /* misc helpers */
  
  
  /**
   * Base class for generator function objects.
   */
  template<typename result_type>
  class generator
  {
  public:
    virtual ~generator() {}
    virtual result_type operator()() = 0;
  };
  
  
  /** A generator function object that binds a nullary method (i.e. a
      generator) to an object instance of the corresponding class. It can be
      deactivated, in which state it always returns the given inactive
      value. */ 
  template<typename result_type, typename mem_fun_result_type,
           typename object_type>
  class mem_fun_generator :
    public generator<result_type>
  {
    mem_fun_result_type (object_type::*mem_fun_ptr)() const;
    object_type * object_ptr;
    
    result_type inactive_value;
    bool active;
    
  public:
    mem_fun_generator( mem_fun_result_type(object_type::*mem_fun_ptr_)() const,
                       object_type * object_ptr_,
                       const result_type & inactive_value_,
                       bool is_active ) :
      mem_fun_ptr( mem_fun_ptr_ ),
      object_ptr( object_ptr_ ),
      inactive_value( inactive_value_ ),
      active( is_active )
    {}
    
    virtual ~mem_fun_generator() {}
    
    virtual result_type operator()()
    {
      return active ?
        static_cast<result_type>( (object_ptr->*mem_fun_ptr)() ) :
        inactive_value;
    }
    
    void activate( bool state )
    { active = state; }
  };
  

}   /* namespace lifespace */




#endif   /* LS_TYPES_HPP */
