cmake_minimum_required(VERSION 3.29)
project(Boids)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
add_executable(Test
        main_test.cpp
        Flock.hpp
        Flock.cpp
        Utility.hpp
        Flock_Parallel.hpp
        Flock_Parallel.cpp


)

add_executable(Run
        Flock.hpp
        Flock.cpp
        Utility.hpp
        Flock_Parallel.hpp
        Flock_Parallel.cpp
        main_run.cpp

)
target_link_libraries(Run PRIVATE sfml-graphics)
target_compile_features(Run PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
            TARGET Run
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:Run>
            VERBATIM)
endif()
target_link_libraries(Test PRIVATE sfml-graphics)
target_compile_features(Test PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
            TARGET Test
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:Test>
            VERBATIM)
endif()