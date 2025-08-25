# CMakeList.txt : CMake project for noms-cpp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.20)


# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("noms-cpp") 

# Set C++ standard to 23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add source to this project's executable.
file(GLOB NOMS_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)
list(FILTER NOMS_SOURCES EXCLUDE REGEX ".*noms-cpp.cpp$")
add_executable (noms-cpp ${NOMS_SOURCES} noms-cpp.cpp)
target_include_directories(noms-cpp PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)



# testing

enable_testing()

add_executable(
  noms_test
  ${NOMS_SOURCES}
  common_test.cc
)
target_link_libraries(
  noms_test
  GTest::gtest_main
)
target_include_directories(noms_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

include(GoogleTest)
gtest_discover_tests(noms_test)
