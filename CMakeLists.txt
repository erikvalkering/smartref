cmake_minimum_required(VERSION 3.5)

project(delegation)

enable_testing()

################################################################################
# Counter library                                                              #
################################################################################
add_library(libcounter INTERFACE)
target_sources(libcounter INTERFACE ${PROJECT_SOURCE_DIR}/counter.h)

add_executable(test_counter counter.cpp)
target_link_libraries(test_counter PRIVATE libcounter)
add_test(counter test_counter)
################################################################################


################################################################################
# Reflection library                                                           #
################################################################################
add_library(libreflection INTERFACE)
target_sources(libreflection INTERFACE ${PROJECT_SOURCE_DIR}/reflection.h)
target_link_libraries(libreflection PRIVATE libcounter)

add_executable(test_reflection reflection.cpp)
target_link_libraries(test_reflection PRIVATE libcounter)
add_test(reflection test_reflection)
################################################################################


################################################################################
# Delegation library                                                           #
################################################################################
add_library(libdelegation INTERFACE)
target_sources(libdelegation INTERFACE ${PROJECT_SOURCE_DIR}/using.h)
target_link_libraries(libdelegation PRIVATE libreflection)

add_executable(test_delegation delegation.cpp)
target_sources(test_delegation PRIVATE ${PROJECT_SOURCE_DIR}/foobar.h)
target_link_libraries(test_delegation PRIVATE libdelegation)
add_test(delegation test_delegation)
################################################################################
