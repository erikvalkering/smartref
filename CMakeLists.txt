cmake_minimum_required(VERSION 3.5)

project(smartref LANGUAGES CXX)

enable_testing()

################################################################################
# Counter library                                                              #
################################################################################
add_library(libcounter INTERFACE)
target_sources(libcounter INTERFACE ${PROJECT_SOURCE_DIR}/src/counter.h)
target_include_directories(libcounter INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_compile_features(libcounter INTERFACE cxx_constexpr)

add_executable(test_counter tests/counter.cpp)
target_link_libraries(test_counter PRIVATE libcounter)
add_test(counter test_counter)
################################################################################


################################################################################
# Reflection library                                                           #
################################################################################
add_library(libreflection INTERFACE)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/src/reflection.h)
target_include_directories(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_compile_features(libreflection INTERFACE cxx_constexpr)
target_link_libraries(libreflection INTERFACE libcounter)

add_executable(test_reflection tests/reflection.cpp)
target_compile_features(test_reflection INTERFACE cxx_constexpr)
target_link_libraries(test_reflection PRIVATE libcounter)
add_test(reflection test_reflection)
################################################################################


################################################################################
# Smartref library                                                           #
################################################################################
add_library(libsmartref INTERFACE)
target_sources(libsmartref INTERFACE ${PROJECT_SOURCE_DIR}/src/using.h)
target_include_directories(libsmartref INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_compile_features(libsmartref INTERFACE cxx_constexpr)
target_link_libraries(libsmartref INTERFACE libreflection)

add_executable(test_smartref examples/smartref.cpp)
target_sources(test_smartref PRIVATE ${PROJECT_SOURCE_DIR}/examples/foobar.h)
target_link_libraries(test_smartref PRIVATE libsmartref)
add_test(smartref test_smartref)
################################################################################
