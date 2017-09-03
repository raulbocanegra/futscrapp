cmake_minimum_required(VERSION 3.0)

project(futscrapp C CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(cmake/platform.cmake)

message("Build: ${CMAKE_GENERATOR}")

set_platform_properties()

# Set global platform variable for source directories
if (UNIX)
	set(PLATFORM "linux")	
elseif (WIN32 AND MSVC)
	set(PLATFORM "windows")	
endif()	

# Testing library include dir
set(CATCH_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/external/test/include")

find_path(CPPREST_INCLUDE cpprest/http_client.h)
find_library(CPPREST_LIB NAMES cpprest_2_9d cpprest_2_9 cpprestd cpprest)
find_package(Boost REQUIRED COMPONENTS random system thread filesystem chrono atomic date_time regex)
find_package(OpenSSL 1.0.0 REQUIRED)
find_package(CURL)
find_package(LibXml2)

# Add subdirectories
add_subdirectory(futscrapplib)

# Header and sources
#set(FUTSCRAPP_HEADERS )
set(FUTSCRAPP_SOURCES Main.cpp)

# target definitions
add_executable(futscrapp ${FUTSCRAPP_SOURCES})
target_include_directories(futscrapp PUBLIC ${CPPREST_INCLUDE})
target_link_libraries(futscrapp
  futscrapplib
  ${CPPREST_LIB}
  ${CURL_LIBRARIES}
  ${LIBXML2_LIBRARIES}
  Boost::boost
  Boost::random
  Boost::system
  Boost::thread
  Boost::filesystem
  Boost::chrono
  Boost::atomic
  Boost::date_time
  Boost::regex
  OpenSSL::SSL
)
target_compile_definitions(futscrapp PUBLIC ${LIBXML2_DEFINITIONS})