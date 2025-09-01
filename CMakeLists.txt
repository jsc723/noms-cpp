# CMakeList.txt : CMake project for noms-cpp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20)


# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("nomp") 

# Set C++ standard to 23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL  REQUIRED)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add source to this project's executable.
file(GLOB_RECURSE NOMP_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)
list(FILTER NOMP_SOURCES EXCLUDE REGEX ".*nomp\\.cpp$")
message(STATUS "NOMP_SOURCES contains: ${NOMP_SOURCES}")
add_executable (nomp ${NOMP_SOURCES} "src/nomp.cpp")
target_include_directories(nomp PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_link_libraries(nomp
  PRIVATE
  OpenSSL::Crypto
)



# testing

enable_testing()

file(GLOB_RECURSE NOMP_TESTS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*_test.cc"
)

add_executable(
  nomp_test
  ${NOMP_SOURCES}
  ${NOMP_TESTS}
)
target_link_libraries(
  nomp_test
  OpenSSL::Crypto
  GTest::gtest_main
)
target_include_directories(nomp_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)


include(GoogleTest)
gtest_discover_tests(nomp_test)
