cmake_minimum_required(VERSION 3.16)

project(Factiz
    VERSION 1.0
    LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# C++ settings
# ---------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------

include(FetchContent)

# Boost
find_package(Boost REQUIRED)

# CLI11
FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.4.2
)

# spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.14.1
)

FetchContent_MakeAvailable(CLI11 spdlog)

# ---------------------------------------------------------------------------
# Factiz library
# ---------------------------------------------------------------------------

file(GLOB_RECURSE FACTIZ_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/libraries/factiz/src/factiz/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/libraries/factiz/src/factiz/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/libraries/factiz/src/factiz/*.cxx"
)

if(NOT FACTIZ_SOURCES)
    message(FATAL_ERROR
        "No source files found under libraries/factiz/src/factiz"
    )
endif()

add_library(factiz_lib STATIC
    ${FACTIZ_SOURCES}
)

target_include_directories(factiz_lib
    PUBLIC
        ${Boost_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/libraries/factiz/src
)

target_link_libraries(factiz_lib
    PUBLIC
        Boost::boost
        spdlog::spdlog
)

# ---------------------------------------------------------------------------
# Example application
# ---------------------------------------------------------------------------

file(GLOB_RECURSE FACTIZ_EXAMPLE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/applications/factizExample1/src/*.cpp"
)

add_executable(factizExample1
    ${FACTIZ_EXAMPLE_SOURCES}
)

target_link_libraries(factizExample1
    PRIVATE
        factiz_lib
        CLI11::CLI11
)

# ---------------------------------------------------------------------------
# Diagnostics
# ---------------------------------------------------------------------------

message(STATUS "Boost found: ${Boost_INCLUDE_DIRS}")
message(STATUS "Factiz sources:")
foreach(src ${FACTIZ_SOURCES})
    message(STATUS "  ${src}")
endforeach()
