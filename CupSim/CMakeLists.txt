#----------------------------------------------------------------------------
# Project Name: CupSim
# Description: Simulation library & executable for CupSim package
#----------------------------------------------------------------------------
# Setup the project and require CMake version above 2.8
#----------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(CupSim)

#----------------------------------------------------------------------------
# Add variables for the git revision
#----------------------------------------------------------------------------
# Get the current working branch
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE CupSim_GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

# Get the latest abbreviated commit hash of the working branch
execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE CupSim_GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

#----------------------------------------------------------------------------
# Find ROOT and setup it
#----------------------------------------------------------------------------
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

#----------------------------------------------------------------------------
# Find Geant4 and setup it
#----------------------------------------------------------------------------
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED ui_all vis_all)
else()
    find_package(Geant4 REQUIRED)
endif()
include(${Geant4_USE_FILE})
#For backward compatibility
set(CMAKE_CXX_STANDARD 17)

#----------------------------------------------------------------------------
# Copy the various files to build directory
#----------------------------------------------------------------------------
file(COPY ${PROJECT_SOURCE_DIR}/data DESTINATION ${PROJECT_BINARY_DIR})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#----------------------------------------------------------------------------
file(GLOB CupSim_GitRev_SOURCE ${PROJECT_SOURCE_DIR}/src/CupSimGitRevision.cc)
file(GLOB CupSim_LIB_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB CupSim_LIB_HEADERS ${PROJECT_SOURCE_DIR}/CupSim/*.hh)

#----------------------------------------------------------------------------
# Add definitions for git commit hash
#----------------------------------------------------------------------------
set_source_files_properties(${CupSim_GitRev_SOURCE} PROPERTIES
    COMPILE_DEFINITIONS "CupSim_GIT_BRANCH=${CupSim_GIT_BRANCH};CupSim_GIT_COMMIT_HASH=${CupSim_GIT_COMMIT_HASH}" )

#----------------------------------------------------------------------------
# Add libraries and link it to the Geant4 framework and MCObjs library
#----------------------------------------------------------------------------
add_library(CupSimL SHARED ${CupSim_LIB_SOURCES})
target_link_libraries(CupSimL ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# If you are not using CONAN frameworks, executables named 'cupsim' should
# be built
#----------------------------------------------------------------------------
if(NOT DEFINED CONAN_PACKAGE_NAME)
    file(GLOB CupSim_EXEC_SOURCE ${PROJECT_SOURCE_DIR}/test/cupsim.cc)
    set_source_files_properties(${CupSim_EXEC_SOURCE} PROPERTIES
        COMPILE_DEFINITIONS "CupSim_GIT_BRANCH=${CupSim_GIT_BRANCH};CupSim_GIT_COMMIT_HASH=${CupSim_GIT_COMMIT_HASH}" )
    add_executable(cupsim ${CupSim_EXEC_SOURCE})
    target_link_libraries(cupsim CupSimL ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
    target_include_directories(cupsim PUBLIC ${PROJECT_SOURCE_DIR} ${Geant4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})
    if(CMAKE_COMPILER_IS_GNUCXX)
        target_compile_options(cupsim PUBLIC -fdiagnostics-color=always)
    endif()
endif()

#----------------------------------------------------------------------------
# Check dependencies for this project and set include directories and libraries
#----------------------------------------------------------------------------
if(DEFINED MCObjs)
    include_directories(${MCObjs_INCLUDE_DIR})
    target_include_directories(cupsim PUBLIC ${MCObjs_INCLUDE_DIR})
    target_include_directories(CupSimL PUBLIC ${MCObjs_INCLUDE_DIR})
    target_link_libraries(cupsim MCObjs)
    target_link_libraries(CupSimL MCObjs)
elseif(DEFINED CONAN_MCOBJS_ROOT)
    include_directories(${CONAN_INCLUDE_DIRS_MCOBJS})
    target_include_directories(cupsim PUBLIC ${CONAN_INCLUDE_DIRS_MCOBJS})
    target_include_directories(CupSimL PUBLIC ${CONAN_INCLUDE_DIRS_MCOBJS})
    target_link_libraries(cupsim ${CONAN_LIBS_MCOBJS})
    target_link_libraries(CupSimL ${CONAN_LIBS_MCOBJS})
else()
    message(SEND_ERROR "This project cannot be built without MCObjs. Please check about this.")
endif()

#----------------------------------------------------------------------------
# Setup global and local include directories for this project
#----------------------------------------------------------------------------
include_directories(${PROJECT_SOURCE_DIR} ${Geant4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})
target_include_directories(CupSimL PUBLIC ${PROJECT_SOURCE_DIR} ${Geant4_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS})
if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(CupSimL PUBLIC -fdiagnostics-color=always)
endif()

#----------------------------------------------------------------------------
# Expose this public includes and library to other subprojects through cache
# variable.
#----------------------------------------------------------------------------
set(${PROJECT_NAME}_INCLUDE_DIR ${PROJECT_SOURCE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
set(${PROJECT_NAME}_LIBRARY ${PROJECT_BINARY_DIR}/lib${PROJECT_NAME}.so
    CACHE INTERNAL "${PROJECT_NAME}: Path to library" FORCE)
set(${PROJECT_NAME} "Exist!"
    CACHE INTERNAL "${PROJECT_NAME}: Variable for checking dependencies" FORCE)
