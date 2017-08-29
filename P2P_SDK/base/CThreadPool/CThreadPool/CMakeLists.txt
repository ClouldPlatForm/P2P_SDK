PROJECT(CThreadPool)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
################################## set prg name ################################## 


################################## control software version ##################################
set(MAR_VERSION 0)
set(MIN_VERSION 0)
set(RELEASE_VERSION 1)
set(PrgName ${PROJECT_NAME})
configure_file(
"${PROJECT_SOURCE_DIR}/Config.h.in"  
"${PROJECT_BINARY_DIR}/Config.h" 
)

INCLUDE_DIRECTORIES(../../../Include/)
################################### Reconfigure final output directory ##################################
LINK_DIRECTORIES(../../../bin/Linux/)


LINK_DIRECTORIES(./)
LINK_DIRECTORIES(../../../bin/Linux/)
LINK_DIRECTORIES(../../../lib/Linux/)

LINK_DIRECTORIES(./)
set(LIBRARY_OUTPUT_PATH                    ${PROJECT_SOURCE_DIR}/../../../bin/Linux/)


AUX_SOURCE_DIRECTORY(. DIR_SRCS)






################################### set include path ###################################################################### 
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../../Global/)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../../Include/)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../../Include/log4cpp/include/)



################################### compile options ####################################################################


LINK_LIBRARIES(avxcompat)

if(Flag)        
message( STATUS "==============================message to Flag" )        
ADD_DEFINITIONS(-g)
else(Flag)        
ADD_DEFINITIONS(-O2)
endif()
ADD_DEFINITIONS("-std=c++11")
################################### generate lib file ################################################################
ADD_LIBRARY(${PROJECT_NAME}                     SHARED                      ${DIR_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}      avxcompat)

