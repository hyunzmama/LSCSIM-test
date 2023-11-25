#----------------------------------------------------------------------------
# Project Name: MCObjs
# Description: Classes for Amoresim, Cupsim framework, which are used to store
#              various information.
#----------------------------------------------------------------------------
# Setup the project and require CMake version above 3.4.3
#----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.4.3 FATAL_ERROR)
project(MCObjs)

#----------------------------------------------------------------------------
# Add variables for the git revision
#----------------------------------------------------------------------------
# Get the current working branch
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE MCObjs_GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

# Get the latest abbreviated commit hash of the working branch
execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE MCObjs_GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

#----------------------------------------------------------------------------
# Find ROOT and setup it
#----------------------------------------------------------------------------
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED)
if(NOT DEFINED ROOT_GENERATE_DICTIONARY)
    include(${ROOT_USE_FILE})
endif()

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#----------------------------------------------------------------------------
file(GLOB MCObjs_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB MCObjs_GitRev_SOURCE ${PROJECT_SOURCE_DIR}/src/MCObjsGitRevision.cc)
file(GLOB MCObjs_HEADERS_ORIGINAL ${PROJECT_SOURCE_DIR}/MCObjs/*.hh)

#----------------------------------------------------------------------------
# Add definitions for git commit hash
#----------------------------------------------------------------------------
set_source_files_properties(${MCObjs_GitRev_SOURCE} PROPERTIES
    COMPILE_DEFINITIONS "MCObjs_GIT_BRANCH=${MCObjs_GIT_BRANCH};MCObjs_GIT_COMMIT_HASH=${MCObjs_GIT_COMMIT_HASH}" )

#----------------------------------------------------------------------------
# Copy the header files to build directory
#----------------------------------------------------------------------------
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/MCObjs)
foreach(NOW_HEADER ${MCObjs_HEADERS_ORIGINAL})
    configure_file(${NOW_HEADER} ${PROJECT_BINARY_DIR}/MCObjs COPYONLY)
endforeach(NOW_HEADER)
file(GLOB MCObjs_HEADERS ${PROJECT_BINARY_DIR}/MCObjs/*.hh)

#----------------------------------------------------------------------------
# Generate dictionary for classes of this project
#----------------------------------------------------------------------------
ROOT_GENERATE_DICTIONARY(G__MCObjs
    ${MCObjs_HEADERS}
    LINKDEF dict/linkdef.h
    OPTIONS -I${PROJECT_BINARY_DIR})

#----------------------------------------------------------------------------
# Add the libraries and link it to the ROOT framework and MCObjs library
#----------------------------------------------------------------------------
add_library(MCObjs SHARED ${MCObjs_SOURCES} G__MCObjs.cxx)
target_link_libraries(MCObjs PRIVATE ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Setup global and local include directories for this project
#----------------------------------------------------------------------------
include_directories(${PROJECT_BINARY_DIR})
include_directories(${ROOT_INCLUDE_DIRS})
target_include_directories(MCObjs PUBLIC ${PROJECT_BINARY_DIR})
target_include_directories(MCObjs PUBLIC ${ROOT_INCLUDE_DIRS})

#----------------------------------------------------------------------------
# Expose this public includes to other subprojects through cache variable.
#----------------------------------------------------------------------------
set(${PROJECT_NAME}_INCLUDE_DIR ${PROJECT_BINARY_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
set(${PROJECT_NAME}_LIBRARY ${PROJECT_BINARY_DIR}/lib${PROJECT_NAME}.so
    CACHE INTERNAL "${PROJECT_NAME}: Path to library" FORCE)
set(${PROJECT_NAME} "Exist!"
    CACHE INTERNAL "${PROJECT_NAME}: Variable for checking dependencies" FORCE)

#----------------------------------------------------------------------------
# Adds linker options at the end of a commandline for linker.
# It has been inserted to make "EXECUTABLE" library to display version
# information and so on. But, this method is not a fancy one.
#----------------------------------------------------------------------------
if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(MCObjs PRIVATE "c -pie -Wl,-E")
endif()
