cmake_minimum_required(VERSION 3.18)
project(orderbook_collector)
set(CMAKE_CXX_STANDARD 20)

include(ExternalProject)
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    include_directories("/opt/homebrew/include")
else()
    include("${CMAKE_SOURCE_DIR}/vendor/rapidjson.cmake")
    include_directories(${RAPIDJSON_INCLUDE_DIR})
endif()

include_directories(
        ${CMAKE_SOURCE_DIR}/include
        ${Boost_INCLUDE_DIRS}
)

# Add source files
file(GLOB_RECURSE SRC_FILES "src/*")
add_executable(orderbook_collector ${SRC_FILES})

# Find the CURL library
find_package(CURL REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(Threads REQUIRED)


target_link_libraries(orderbook_collector ${CURL_LIBRARIES} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})


if(NOT CMAKE_SYSTEM_NAME MATCHES "Darwin")
    add_dependencies(orderbook_collector rapidjson)
endif()

# Copy config files
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
