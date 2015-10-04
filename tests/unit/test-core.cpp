/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include <core/Base.hpp>

#include "cppunit/TestAssert.h"
#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/plugin/TestPlugIn.h"

struct A {
  A() = default;
  A( const A& ) = default;
  virtual char test() const = 0;
  virtual ~A() { }
  virtual A* clone() const = 0;

  bool operator==( const A& b ) const {
    return test() == b.test();
  }
};


struct B : public A {
  B() = default;
  B( const B& ) = default;
  char test() const { return 'B'; }
  virtual A* clone() const { return new B; }
};

struct C : public A {
  C() = default;
  C( const C& ) = default;
  char test() const { return 'C'; }
  virtual A* clone() const { return new C; }
};

class TestCowBase : public CppUnit::TestFixture {
public:
  void setUp();
  void testCowBaseVal();
  void testCowBasePtr();
  void testCowBaseArr();

  CPPUNIT_TEST_SUITE(TestCowBase);
  CPPUNIT_TEST(testCowBaseVal);
  CPPUNIT_TEST(testCowBasePtr);
  CPPUNIT_TEST(testCowBaseArr);
  CPPUNIT_TEST_SUITE_END();
};

void TestCowBase::setUp() {
}

void TestCowBase::testCowBaseVal() {
  cow::core::cow_ptr< int > tmp1( 5 );
  cow::core::cow_ptr< int > tmp2( tmp1 );
  cow::core::cow_ptr< int > tmp3( tmp1 );
  CPPUNIT_ASSERT_MESSAGE( "tmp1.use_count() == 3",
                          tmp1.use_count() == 3 );
  CPPUNIT_ASSERT_MESSAGE( "tmp2.use_count() == 3",
                          tmp2.use_count() == 3 );
  CPPUNIT_ASSERT_MESSAGE( "tmp3.use_count() == 3",
                          tmp3.use_count() == 3 );
  *tmp3 = 42;
  CPPUNIT_ASSERT_MESSAGE( "tmp1.use_count() == 2",
                          tmp1.use_count() == 2 );
  CPPUNIT_ASSERT_MESSAGE( "tmp2.use_count() == 2",
                          tmp2.use_count() == 2 );
  CPPUNIT_ASSERT_MESSAGE( "tmp3.use_count() == 1",
                          tmp3.use_count() == 1 );

  CPPUNIT_ASSERT_MESSAGE( "!tmp1.same_instance(tmp3)",
                          !tmp1.same_instance(tmp3) );
  CPPUNIT_ASSERT_MESSAGE( "tmp1.same_instance(tmp2)",
                          tmp1.same_instance(tmp2) );

  CPPUNIT_ASSERT_MESSAGE( "*tmp3 == 42",
                          *tmp3 == 42 );
  CPPUNIT_ASSERT_MESSAGE( "tmp1 == tmp2",
                          tmp1 == tmp2 );
  CPPUNIT_ASSERT_MESSAGE( "tmp2 == tmp1",
                          tmp2 == tmp1 );

  CPPUNIT_ASSERT_MESSAGE( "tmp1 != tmp3",
                          tmp1 != tmp3 );
  CPPUNIT_ASSERT_MESSAGE( "tmp2 != tmp3",
                          tmp2 != tmp3 );
}

void TestCowBase::testCowBasePtr() {
  B b1;
  C c1;
  C c2;
  cow::core::cow_ptr< A > tmp1 ( b1 );
  cow::core::cow_ptr< A > tmp2 ( c1 );
  cow::core::cow_ptr< A > tmp3 ( tmp1 );
  cow::core::cow_ptr< A > tmp4 ( tmp2 );

  CPPUNIT_ASSERT_MESSAGE( "tmp1 == tmp3",
                          tmp1 == tmp3 );
  CPPUNIT_ASSERT_MESSAGE( "tmp1 != tmp4",
                          tmp1 != tmp4 );
  CPPUNIT_ASSERT_MESSAGE( "tmp2 == tmp4",
                          tmp2 == tmp4 );
  CPPUNIT_ASSERT_MESSAGE( "tmp2 != tmp4",
                          tmp2 != tmp3 );

  tmp3 = cow::core::cow_ptr< A >( c2 );

  CPPUNIT_ASSERT_MESSAGE( "!tmp1.same_instance( tmp3 )",
                          !tmp1.same_instance( tmp3 ) );
  CPPUNIT_ASSERT_MESSAGE( "tmp1.use_count() == 1",
                          tmp1.use_count() == 1 );

  tmp2.make_unique();

  CPPUNIT_ASSERT_MESSAGE( "tmp2.use_count() == 1 && tmp4.use_count() == 1",
                          tmp2.use_count() == 1 && tmp4.use_count() == 1 );
}

void TestCowBase::testCowBaseArr() {
  cow::core::cow_ptr< int[] > tmp1( 255 );
  int* x = tmp1.get();
  for( int i = 0; i < 255; ++i ) {
    x[i] = i;
  }
  cow::core::cow_ptr< int[] > tmp2( tmp1 );

  CPPUNIT_ASSERT_MESSAGE( "tmp1 == tmp2",
                          tmp1 == tmp2 );

  cow::core::cow_ptr< int[] > tmp3;

  x = tmp2.get();
  for( int i = 0; i < 3; ++i ) {
    x[i] = i*i;
  }

  CPPUNIT_ASSERT_MESSAGE( "tmp1 != tmp2",
                          tmp1 != tmp2 );
  tmp3 = tmp2;
  CPPUNIT_ASSERT_MESSAGE( "tmp2 == tmp3",
                          tmp2 == tmp3 );

  tmp3 = cow::core::cow_ptr< int[] >();
  cow::core::cow_ptr< int[] > tmp4( 5 );
  tmp3 = std::move( tmp4 );

  CPPUNIT_ASSERT_MESSAGE( "tmp1.use_count() == 1",
                          tmp1.use_count() == 1 );
  CPPUNIT_ASSERT_MESSAGE( "tmp2.use_count() == 1",
                          tmp2.use_count() == 1 );
  CPPUNIT_ASSERT_MESSAGE( "tmp3.use_count() == 1",
                          tmp3.use_count() == 1 );
  CPPUNIT_ASSERT_MESSAGE( "!tmp4.same_instance( tmp3 )",
                          !tmp4.same_instance( tmp3 ) );
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestCowBase);
