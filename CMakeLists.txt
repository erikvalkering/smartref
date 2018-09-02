cmake_minimum_required(VERSION 3.5)

project(smartref LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

################################################################################
# Utils library                                                                #
################################################################################
add_library(libutils INTERFACE)
target_sources(libutils INTERFACE ${PROJECT_SOURCE_DIR}/include/utils/counter.h)
target_sources(libutils INTERFACE ${PROJECT_SOURCE_DIR}/include/utils/cpp17.h)
target_sources(libutils INTERFACE ${PROJECT_SOURCE_DIR}/include/utils/utils.h)
target_include_directories(libutils INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(libutils INTERFACE cxx_constexpr cxx_decltype_auto)

add_executable(test_utils ${PROJECT_SOURCE_DIR}/tests/main.cpp)
target_sources(test_utils PRIVATE ${PROJECT_SOURCE_DIR}/tests/utils/counter.cpp)
target_sources(test_utils PRIVATE ${PROJECT_SOURCE_DIR}/tests/utils/cpp17.cpp)
target_sources(test_utils PRIVATE ${PROJECT_SOURCE_DIR}/tests/utils/utils.cpp)
target_link_libraries(test_utils PRIVATE libutils)
add_test(utils test_utils)
################################################################################


################################################################################
# Reflection library                                                           #
################################################################################
add_library(libreflection INTERFACE)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/include/reflection/reflect.h)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/include/reflection/reflect_auto.h)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/include/reflection/reflect_members.h)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/include/reflection/reflectors.h)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/include/reflection/reflectable.h)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/include/reflection/reflection.h)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/include/reflection/reify.h)
target_include_directories(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(libreflection INTERFACE cxx_constexpr cxx_decltype_auto)
target_link_libraries(libreflection INTERFACE libutils)

# Unit tests
add_executable(test_reflection ${PROJECT_SOURCE_DIR}/tests/main.cpp)
target_sources(test_reflection PRIVATE ${PROJECT_SOURCE_DIR}/tests/reflection/reflect.cpp)
target_sources(test_reflection PRIVATE ${PROJECT_SOURCE_DIR}/tests/reflection/reflect_auto.cpp)
target_sources(test_reflection PRIVATE ${PROJECT_SOURCE_DIR}/tests/reflection/reflect_members.cpp)
target_sources(test_reflection PRIVATE ${PROJECT_SOURCE_DIR}/tests/reflection/reflect_operators.cpp)
target_sources(test_reflection PRIVATE ${PROJECT_SOURCE_DIR}/tests/reflection/reflectors.cpp)
target_sources(test_reflection PRIVATE ${PROJECT_SOURCE_DIR}/tests/reflection/reflectable.cpp)
target_sources(test_reflection PRIVATE ${PROJECT_SOURCE_DIR}/tests/reflection/reflection.cpp)
target_sources(test_reflection PRIVATE ${PROJECT_SOURCE_DIR}/tests/reflection/reify.cpp)
target_compile_features(test_reflection INTERFACE cxx_constexpr cxx_decltype_auto)
target_link_libraries(test_reflection PRIVATE libreflection)
add_test(reflection test_reflection)
################################################################################


################################################################################
# Smartref library                                                             #
################################################################################
add_library(libsmartref INTERFACE)
target_sources(libsmartref INTERFACE ${PROJECT_SOURCE_DIR}/include/smartref/members.h)
target_sources(libsmartref INTERFACE ${PROJECT_SOURCE_DIR}/include/smartref/smartref.h)
target_sources(libsmartref INTERFACE ${PROJECT_SOURCE_DIR}/include/smartref/operators.h)
target_sources(libsmartref INTERFACE ${PROJECT_SOURCE_DIR}/include/smartref/stl.h)
target_include_directories(libsmartref INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(libsmartref INTERFACE cxx_constexpr cxx_decltype_auto)
target_link_libraries(libsmartref INTERFACE libreflection)
target_link_libraries(libsmartref INTERFACE libutils)

# Unit tests
add_executable(test_smartref ${PROJECT_SOURCE_DIR}/tests/main.cpp)
target_sources(test_smartref PRIVATE ${PROJECT_SOURCE_DIR}/tests/smartref/members.cpp)
target_sources(test_smartref PRIVATE ${PROJECT_SOURCE_DIR}/tests/smartref/members/member_functions.cpp)
target_sources(test_smartref PRIVATE ${PROJECT_SOURCE_DIR}/tests/smartref/members/member_types.cpp)
target_sources(test_smartref PRIVATE ${PROJECT_SOURCE_DIR}/tests/smartref/operators.cpp)
target_sources(test_smartref PRIVATE ${PROJECT_SOURCE_DIR}/tests/smartref/operators/assignment.cpp)
target_sources(test_smartref PRIVATE ${PROJECT_SOURCE_DIR}/tests/smartref/smartref.cpp)
target_sources(test_smartref PRIVATE ${PROJECT_SOURCE_DIR}/tests/smartref/stl.cpp)
target_sources(test_smartref PRIVATE ${PROJECT_SOURCE_DIR}/tests/smartref/stl/containers.cpp)
target_link_libraries(test_smartref PRIVATE libsmartref)
add_test(smartref test_smartref)

# Examples
add_executable(sandbox examples/sandbox.cpp)
target_sources(sandbox PRIVATE ${PROJECT_SOURCE_DIR}/examples/foobar.h)
target_sources(sandbox PRIVATE ${PROJECT_SOURCE_DIR}/examples/stdlibs_for_debugging.h)
target_link_libraries(sandbox PRIVATE libreflection)
target_link_libraries(sandbox PRIVATE libsmartref)
add_test(examples sandbox)
################################################################################
