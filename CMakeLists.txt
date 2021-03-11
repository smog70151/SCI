cmake_minimum_required (VERSION 3.12)

project (SCI)

# Write built executables and libraries to bin/ and lib/, respectively.
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
endif()

# External libraries
# SEAL
find_package(SEAL QUIET)
if(SEAL_FOUND)
    message(STATUS "Found SEAL")
elseif (NOT SEAL_FOUND AND NOT TARGET SEAL::seal)
    message("SEAL was not found: add SEAL subdirectory")
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/SEAL/native/srcCMakeLists.txt")
        find_package(Git REQUIRED)
        message("initialize Git submodule: extern/SEAL")
        execute_process(COMMAND git submodule update --init --recursive extern/SEAL
                        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
    endif()
endif()
execute_process(COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/build .
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/extern/SEAL/native/src")
execute_process(COMMAND ${CMAKE_COMMAND} --build . --target install
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/extern/SEAL/native/src")
find_package(SEAL 3.3.2 EXACT REQUIRED PATHS "${PROJECT_SOURCE_DIR}/build/")

# Eigen
find_package(Eigen3 QUIET)
if(Eigen3_FOUND)
    message(STATUS "Found Eigen3")
elseif (NOT Eigen3_FOUND AND NOT TARGET Eigen3::eigen)
    message("Eigen3 was not found: add Eigen3 subdirectory")
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/eigen/CMakeLists.txt")
        find_package(Git REQUIRED)
        message("initialize Git submodule: extern/eigen")
        execute_process(COMMAND git submodule update --init --recursive extern/eigen
                        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
    endif()
    add_subdirectory(extern/eigen)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sci)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type (CMAKE_BUILD_TYPE): ${CMAKE_BUILD_TYPE}")

add_subdirectory(sci)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif (BUILD_TESTS)
