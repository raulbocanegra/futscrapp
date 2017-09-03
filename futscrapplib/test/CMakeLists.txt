cmake_minimum_required(VERSION 3.0)

project(threading C CXX)

message("Build: ${CMAKE_GENERATOR}")

# Specific definitions depending on the platform.
if (UNIX)
  set( PLATFORM "linux")
  add_compile_options(
      -m64 # 64 bits build flag
  )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  set(CMAKE_CXX_STANDARD 11) # C++11...
  set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
  set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
elseif (WIN32 AND MSVC)
  set( PLATFORM "windows")
  add_definitions(
      -DNOMINMAX # https://support.microsoft.com/en-gb/help/143208/prb-using-stl-in-windows-program-can-cause-min-max-conflicts
  )
  add_compile_options(
      /MP # https://msdn.microsoft.com/en-us/library/bb385193.aspx
  )  
endif()

# Add subdirectories
if(ENABLE_TESTING)
	add_subdirectory(test)
endif()



# Header and sources
set(TOOLS_SOURCES Main.cpp)

# target definitions
add_executable(tools ${TOOLS_HEADERS} ${TOOLS_SOURCES})


