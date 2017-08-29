PROJECT(Log)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


################################## control software version ##################################
set(MAR_VERSION 0)
set(MIN_VERSION 0)
set(RELEASE_VERSION 1)

configure_file(
"${PROJECT_SOURCE_DIR}/Config.h.in"  
"${PROJECT_BINARY_DIR}/Config.h" 
)


################################### Reconfigure final output directory ##################################
set(LIBRARY_OUTPUT_PATH                    ${PROJECT_SOURCE_DIR}/../../../lib/Linux/)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)






################################### set include path ###################################################################### 
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../../Global/)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../../Include/)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../../Include/log4cpp/include/)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/../../../bin/Linux/)


################################### compile options ####################################################################
if(Flag)        
message( STATUS "==============================message to Flag" )        
ADD_DEFINITIONS(-g)
else(Flag)        
ADD_DEFINITIONS(-O2)
endif()
ADD_DEFINITIONS("-std=c++11")
#LINK_LIBRARIES(log4cpp)
#LINK_LIBRARIES(pthread)
################################### generate lib file ################################################################
ADD_LIBRARY(${PROJECT_NAME}                     STATIC                      ${DIR_SRCS})
#ADD_EXECUTABLE(${PROJECT_NAME} ${DIR_SRCS})


