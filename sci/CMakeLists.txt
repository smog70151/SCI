# Redo LinearHE CMakeLists.txt
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(cmake/source_of_randomness.cmake)

find_package(OpenSSL REQUIRED)
find_package(GMP REQUIRED)

# GLOB FILES
file(GLOB LinearHE_H "LinearHE/*.h")
file(GLOB LinearHE_CPP "LinearHE/*.cpp")
file(GLOB Millionaire_H "Millionaire/*.h")
file(GLOB NonLinear_H "NonLinear/*.h")
file(GLOB OT_H "OT/*.h")
file(GLOB utils_H "utils/*.h")
file(GLOB ArgMapping_H "utils/ArgMapping/*.h")

# ADD FILES TO SCI LIBRARY
add_library(sci
    ${LinearHE_H}
    ${LinearHE_CPP}
    ${Millionaire_H}
    ${NonLinear_H}
    ${OT_H}
    ${utils_H}
    ${ArgMapping_H}
)

find_package(OpenMP REQUIRED)
find_package(SEAL 3.3.2 EXACT REQUIRED PATHS "${PROJECT_SOURCE_DIR}/build/")    
find_package(Eigen3 3.3 REQUIRED NO_MODULE PATHS "${PROJECT_SOURCE_DIR}/build/")

add_library(SCI::sci ALIAS sci)

if(USE_RANDOM_DEVICE)
    target_compile_definitions(sci PUBLIC EMP_USE_RANDOM_DEVICE=1)
endif(USE_RANDOM_DEVICE)

target_link_libraries(sci
    PUBLIC SEAL::seal
    PUBLIC OpenMP::OpenMP_CXX
    PUBLIC Eigen3::Eigen
)

target_link_libraries(sci
    PUBLIC ${OPENSSL_LIBRARIES} ${GMP_LIBRARIES}
)

target_include_directories(sci
    PUBLIC ${OPENSSL_INCLUDE_DIR} ${GMP_INCLUDE_DIR}
)

# Compile Related
target_compile_features(sci PUBLIC cxx_std_17)
target_compile_options(sci PUBLIC "-fpermissive" "-pthread" "-Wall" "-maes" "-msse4.1" "-mavx" "-mavx2" "-mrdseed" "-faligned-new" "-w" "-O3")

target_include_directories(sci
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_link_libraries(sci
	PRIVATE stdc++fs
)

install(TARGETS sci
        EXPORT "${PROJECT_NAME}Targets"
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        INCLUDES DESTINATION lib
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        DESTINATION include
        FILES_MATCHING PATTERN "*.h"
)

export(TARGETS sci NAMESPACE "${PROJECT_NAME}::" FILE "${PROJECT_NAME}Targets.cmake")
install(EXPORT "${PROJECT_NAME}Targets"
        NAMESPACE "${PROJECT_NAME}::"
        DESTINATION "lib/cmake/${PROJECT_NAME}"
)
message(STATUS "[+] PROJECT_NAME: ${PROJECT_NAME}")

include(CMakePackageConfigHelpers)

configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION "lib/cmake/${PROJECT_NAME}"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    DESTINATION "lib/cmake/${PROJECT_NAME}"
)