cmake_minimum_required( VERSION 2.8.12 )
project( rpi_stream_receive )
SET( SRCS )
aux_source_directory( "./common_util" SRCS )
aux_source_directory( "./app" SRCS )
aux_source_directory( "./udp_setup" SRCS )
aux_source_directory( "./ffmpeg_setup" SRCS )
aux_source_directory( "./computer_vision" SRCS )

add_executable( ${CMAKE_PROJECT_NAME} ${SRCS} )

find_package( OpenCV REQUIRED )

set( GCC_COVERAGE_COMPILE_FLAGS -DSTANDALONE -D__STDC_CONSTANT_MACROS -pipe -Werror -g -Wall )
set( GCC_COVERAGE_LINK_FLAGS -lpthread -lavformat -lavcodec -lavutil -lswscale )

target_compile_options( ${CMAKE_PROJECT_NAME} PRIVATE ${GCC_COVERAGE_COMPILE_FLAGS} )
target_link_libraries( ${CMAKE_PROJECT_NAME} ${GCC_COVERAGE_LINK_FLAGS} ${OpenCV_LIBS})
