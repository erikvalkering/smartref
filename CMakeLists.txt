cmake_minimum_required(VERSION 3.5)

project(smartref LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

################################################################################
# Counter library                                                              #
################################################################################
add_library(libcounter INTERFACE)
target_sources(libcounter INTERFACE ${PROJECT_SOURCE_DIR}/src/counter.h)
target_include_directories(libcounter INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_compile_features(libcounter INTERFACE cxx_constexpr cxx_decltype_auto)

add_executable(test_counter tests/counter.cpp)
target_link_libraries(test_counter PRIVATE libcounter)
add_test(counter test_counter)
################################################################################


################################################################################
# Reflection library                                                           #
################################################################################
add_library(libreflection INTERFACE)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/src/reflection.h)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/src/reflection/reflect.h)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/src/reflection/reflect_members.h)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/src/reflection/reify.h)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/src/reflection_common.h)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/src/reflection_intrusive.h)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/src/reflection_nonintrusive.h)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/src/utils.h)
target_include_directories(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_compile_features(libreflection INTERFACE cxx_constexpr cxx_decltype_auto)
target_link_libraries(libreflection INTERFACE libcounter)

# Unit tests
add_executable(test_reflection ${PROJECT_SOURCE_DIR}/tests/main.cpp)
target_sources(test_reflection PRIVATE ${PROJECT_SOURCE_DIR}/tests/reflection.cpp)
target_sources(test_reflection PRIVATE ${PROJECT_SOURCE_DIR}/tests/reflection/reflect.cpp)
target_sources(test_reflection PRIVATE ${PROJECT_SOURCE_DIR}/tests/reflection/reflect_members.cpp)
target_sources(test_reflection PRIVATE ${PROJECT_SOURCE_DIR}/tests/reflection/reify.cpp)
target_sources(test_reflection PRIVATE ${PROJECT_SOURCE_DIR}/tests/reflection_common.cpp)
target_sources(test_reflection PRIVATE ${PROJECT_SOURCE_DIR}/tests/reflection_intrusive.cpp)
target_sources(test_reflection PRIVATE ${PROJECT_SOURCE_DIR}/tests/reflection_nonintrusive.cpp)
target_sources(test_reflection PRIVATE ${PROJECT_SOURCE_DIR}/tests/utils.cpp)
target_compile_features(test_reflection INTERFACE cxx_constexpr cxx_decltype_auto)
target_link_libraries(test_reflection PRIVATE libcounter)
add_test(reflection test_reflection)
################################################################################


################################################################################
# Smartref library                                                             #
################################################################################
add_library(libsmartref INTERFACE)
target_sources(libsmartref INTERFACE ${PROJECT_SOURCE_DIR}/src/smartref.h)
target_include_directories(libsmartref INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_compile_features(libsmartref INTERFACE cxx_constexpr cxx_decltype_auto)
target_link_libraries(libsmartref INTERFACE libreflection)

# Unit tests
add_executable(test_member_types tests/smartref/member_types.cpp)
target_sources(test_member_types PRIVATE ${PROJECT_SOURCE_DIR}/tests/main.cpp)
target_link_libraries(test_member_types PRIVATE libsmartref)
add_test(smartref test_member_types)

# Examples
add_executable(sandbox examples/sandbox.cpp)
target_sources(sandbox PRIVATE ${PROJECT_SOURCE_DIR}/examples/foobar.h)
target_link_libraries(sandbox PRIVATE libsmartref)
add_test(examples sandbox)
################################################################################
