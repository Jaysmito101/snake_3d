cmake_minimum_required(VERSION 3.12)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(Snake LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REUIRED True)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REUIRED True)

find_package( OpenGL REQUIRED )


include_directories( 
	./Dependencies/GLFW/include
	./Dependencies/Glad/include
	./Include
 )

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )

add_subdirectory(Dependencies/GLFW)
add_subdirectory(Dependencies/Glad)

if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()


file(GLOB SOURCES
     "./Include/*.h"
     "./Source/*.c"
)


add_executable(SnakeMain ${SOURCES})


target_link_libraries(SnakeMain
	glfw
	${GLFW_LIBRARIES}
	opengl32
	Glad
	)


if( MSVC )
    if(${CMAKE_VERSION} VERSION_LESS "3.6.0") 
        message( "\n\t[ WARNING ]\n\n\tCMake version lower than 3.6.\n\n\t - Please update CMake and rerun; OR\n\t - Manually set 'GLFW-CMake-starter' as StartUp Project in Visual Studio.\n" )
    else()
        set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SnakeMain )
    endif()
endif()