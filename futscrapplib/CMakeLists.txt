cmake_minimum_required(VERSION 3.0)

project(futscrapplib)

# Header and sources
set(FUTSCRAPPLIB_HEADERS ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/Bank.h
						 ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/Club.h
                         ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/Common.h
						 ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/League.h
						 ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/Table.h)

set(FUTSCRAPPLIB_SOURCES ${PROJECT_SOURCE_DIR}/src/Bank.cpp
						 ${PROJECT_SOURCE_DIR}/src/Club.cpp
						 ${PROJECT_SOURCE_DIR}/src/Common.cpp
						 ${PROJECT_SOURCE_DIR}/src/League.cpp
						 ${PROJECT_SOURCE_DIR}/src/Table.cpp)

source_group("include" FILES ${FUTSCRAPPLIB_HEADERS})
source_group("src" FILES ${FUTSCRAPPLIB_SOURCES})

# target definitions
add_library(${PROJECT_NAME} STATIC ${FUTSCRAPPLIB_HEADERS} ${FUTSCRAPPLIB_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include
												  ${LIBXML2_INCLUDE_DIR})

if(ENABLE_TESTING)
	add_subdirectory(test)
endif()

