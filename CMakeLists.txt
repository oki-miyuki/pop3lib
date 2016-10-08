#
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
#
# windows:  cmake -G"NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
# 
# others:   cmake .
#
PROJECT (Pop3template)

FIND_PACKAGE(Boost)
#SET (Boost_INCLUDE_DIRS c:/downloads/boost_1_38_0)
#SET (Boost_LIBRARYDIR c:/downloads/boost_1_38_0/stage/lib)
IF (NOT Boost_INCLUDE_DIRS)
  MESSAGE( FATAL ERROR ": Boost needed" )
ELSE (NOT Boost_INCLUDE_DIRS)
  INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} ${INCLUDE_DIRECTORIES} ${PROJECT_SOURCE_DIR}/include )
  LINK_DIRECTORIES( ${Boost_LIBRARYDIR} ${LINK_DIRECTORIES} )
ENDIF (NOT Boost_INCLUDE_DIRS)

ADD_EXECUTABLE(pop3templ sample/pop3templ.cpp src/pop3parser.cpp)

ADD_EXECUTABLE(getuntil src/getuntil.cpp)
