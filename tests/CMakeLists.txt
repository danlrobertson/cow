enable_testing()

find_path(CPPUNIT_INCLUDE_DIR cppunit/TestCase.h /usr/local/include
  /usr/include /include)
include(CTest)
include_directories(${COW_SOURCE_DIR}/src/inc)
include_directories(${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include_test)
add_definitions(-std=c++11 -Wall -Werror)

file(GLOB TEST_SRCS relative ${CMAKE_CURRENT_SOURCE_DIR}
  "unit/test-*.cpp" )

add_executable(runner runner.cpp ${TEST_SRCS})
target_link_libraries(runner cppunit)

foreach(test ${TEST_SRCS})
  get_filename_component(TEST_NAME ${test} NAME_WE)
  add_test(runner runner ${TEST_NAME})
endforeach(test)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
