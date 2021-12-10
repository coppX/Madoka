cmake_minimum_required(VERSION 3.10)

# set the project name
project(MThread)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE  Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

set(CHAPTERS
    test
)

set(test
   test_thread
   test_mutex
   test_recursive_mutex
   test_lock_guard
   test_unique_lock
   test_condition_variable
   test_try_lock
   test_lock
   test_condition_variable_wait_for
   test_condition_variable_wait_until
)

foreach(CHAPTER ${CHAPTERS})
    foreach(DEMO ${${CHAPTER}})
        file(GLOB SOURCE
                "${CHAPTER}/${DEMO}.h"
                "${CHAPTER}/${DEMO}.cpp")
        add_executable(${DEMO} ${SOURCE})
    endforeach(DEMO)
endforeach(CHAPTER)
