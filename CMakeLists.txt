cmake_minimum_required(VERSION 2.8)

project(unnpk)
add_executable(${PROJECT_NAME} "main.c")

find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
    target_link_libraries( ${PROJECT_NAME} ${ZLIB_LIBRARIES} )
endif( ZLIB_FOUND )