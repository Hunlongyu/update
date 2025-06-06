cmake_minimum_required(VERSION 3.28)

project(update LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(/utf-8)

include(FetchContent)

FetchContent_Declare(
    plog
    GIT_REPOSITORY https://github.com/SergiusTheBest/plog.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(plog)

FetchContent_Declare(
    cpp-semver
    GIT_REPOSITORY https://github.com/z4kn4fein/cpp-semver.git
    GIT_TAG v0.4.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(cpp-semver)

FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(argparse)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    sw
    GIT_REPOSITORY https://github.com/Mzying2001/sw.git
    GIT_TAG main
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR sw
)
FetchContent_MakeAvailable(sw)

set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

set(LIBHV_STATIC_LIB ON CACHE BOOL "Build libhv as a static library" FORCE)
FetchContent_Declare(
  libhv
  GIT_REPOSITORY https://github.com/ithewei/libhv.git
  GIT_TAG v1.3.3
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(libhv)

set(SOURCES 
    "src/main.cpp"
    "src/views/home.h"
    "src/views/home.cpp"
    "src/utils/utils.h"
    "src/utils/utils.cpp"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
    add_executable(${PROJECT_NAME} ${SOURCES})
else()
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
endif()

target_include_directories( ${PROJECT_NAME} PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${plog_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    plog
    semver
    argparse
    nlohmann_json::nlohmann_json
    sw
    OpenSSL::SSL
    OpenSSL::Crypto
    hv
)