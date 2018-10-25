cmake_minimum_required (VERSION 3.0)

project (Simple-Http-Server)

option(USE_STANDALONE_ASIO "set ON to use standalone Asio instead of Boost.Asio" OFF)
option(BUILD_DEMO "set ON to build demos" ON)

if(NOT MSVC)
    add_compile_options(-std=c++11 -x -Wall -Wextra -Wsign-conversion)
else()
    add_compile_options(/W1)
endif()

add_library(simple-web-server INTERFACE)

set(My_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(My_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


target_include_directories(simple-web-server INTERFACE ${My_INCLUDE_DIR})

find_package(Threads REQUIRED)
target_link_libraries(simple-web-server INTERFACE ${CMAKE_THREAD_LIBS_INIT})

# TODO 2020 when Debian Jessie LTS ends:
# Remove Boost system, thread, regex components; use Boost::<component> aliases; remove Boost target_include_directories
if(USE_STANDALONE_ASIO)
    target_compile_definitions(simple-web-server INTERFACE USE_STANDALONE_ASIO)
    include(CheckIncludeFileCXX)
    CHECK_INCLUDE_FILE_CXX(asio.hpp HAVE_ASIO)
    if(NOT HAVE_ASIO)
        message(FATAL_ERROR "Standalone Asio not found")
    endif()
else()
    find_package(Boost 1.53.0 COMPONENTS system thread REQUIRED)
    target_link_libraries(simple-web-server INTERFACE ${Boost_LIBRARIES})
    target_include_directories(simple-web-server INTERFACE ${Boost_INCLUDE_DIR})
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
        target_compile_definitions(simple-web-server INTERFACE USE_BOOST_REGEX)
        find_package(Boost 1.53.0 COMPONENTS regex REQUIRED)
        target_link_libraries(simple-web-server INTERFACE ${Boost_LIBRARIES})
        target_include_directories(simple-web-server INTERFACE ${Boost_INCLUDE_DIR})
    endif()
endif()
if(WIN32)
    target_link_libraries(simple-web-server INTERFACE ws2_32 wsock32)
endif()

if(APPLE)
    set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
endif()
find_package(OpenSSL)
if(OPENSSL_FOUND)
    target_compile_definitions(simple-web-server INTERFACE HAVE_OPENSSL)
    target_link_libraries(simple-web-server INTERFACE ${OPENSSL_LIBRARIES})
    target_include_directories(simple-web-server INTERFACE ${OPENSSL_INCLUDE_DIR})
endif()

# If Simple-Web-Server is not a sub-project:
# CMAKE_SOURCE_DIR: This is the full path to the top level of the current CMake source tree. For an in-source build, this would be the same as CMAKE_BINARY_DIR.
# CMAKE_CURRENT_SOURCE_DIR: The current dir of the processed cmake file
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    if(BUILD_DEMO)
        message(STATUS "BUILDING DEMOS in ${My_SRC_DIR}")
        add_subdirectory(${My_SRC_DIR})
    endif()

    set(BUILD_TESTING OFF)

    # Specify a dir to install all the include files
    # THE DESTINATION is defaults to /usr/local on UNIX and c:/Program Files on Windows.
    install(FILES server_http.hpp client_http.hpp server_https.hpp client_https.hpp crypto.hpp utility.hpp status_code.hpp DESTINATION include/simple-web-server)
endif()

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

IF( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
  EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
  )
ENDIF()
