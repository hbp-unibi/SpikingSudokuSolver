#  Spiking Sudoku Solver
#  Copyright (C) 2018 ISY Project Spiking Sudoku Solver
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

# CMake version and project metadata
cmake_minimum_required(VERSION 3.0)

option(BUILD_EXECUTABLE "Build executable" ON)

project(SNN-Sudoku-Solver LANGUAGES CXX)

# Add the local "cmake" directory to the search path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Useful CMake options for Qt projects
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Enable C++14 support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")


find_package(PythonLibs 2.7 REQUIRED)

# Enable all warnings on MSVC and GCC/Clang/Intel
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
elseif(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR
		("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") OR
		("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

# Set a default build type
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

# Add configuration option for static linking 
set(STATIC_LINKING FALSE CACHE BOOL "True for static linking")

# Link statically
if(STATIC_LINKING)
	SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
	SET(BUILD_SHARED_LIBRARIES OFF)
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc -Wl,--no-export-dynamic")
        SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s -Wl,--no-export-dynamic")
    else()
        SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
        SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")
    endif()
endif()

include_directories(
	${CMAKE_SOURCE_DIR}
)

# Include the Cypress library
include(cypress_external)

add_library(utils
    source/utils/Sudoku
    source/utils/spikingSudokuSolver
	)
add_dependencies(utils cypress_ext)
target_link_libraries(utils
    ${CYPRESS_LIBRARY}
)

enable_testing()
add_subdirectory(test)

if(BUILD_EXECUTABLE)
	# lines needed for building all Qt projects
	find_package( Qt4 REQUIRED )
	include ( ${QT_USE_FILE} )
	add_definitions( ${QT_DEFINITIONS} )
	include_directories( ${CMAKE_BINARY_DIR} )

	#define your header files, .ui files here:
	set(MYPROJECT_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/source/gui/mainwindow.h
	${CMAKE_CURRENT_SOURCE_DIR}/source/gui/sudokumatrixwidget.h
	)
	
	#source and .qrc files go here:
	set(MYPROJECT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/source/gui/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/gui/mainwindow.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/gui/sudokumatrixwidget.cpp
	)
	
	# using Qt meta-system (precompiler)
	QT4_WRAP_CPP( MOC_SRCS ${MYPROJECT_HEADERS} )

	# Set up project build using include and source dirs and link libraries
	add_executable(${PROJECT_NAME} ${MYPROJECT_SOURCES} ${MOC_SRCS}) 
	target_link_libraries(${PROJECT_NAME}
		utils
		pthread
		${QT_LIBRARIES})
	# target_include_directories(${PROJECT_NAME} PRIVATE ${Qt5Widgets_INCLUDES})

	add_executable(main source/executable/main)
	target_link_libraries(main 
		utils
		pthread)
    
    add_executable(parameter_sweep source/executable/parameter_sweep.cpp)
	target_link_libraries(parameter_sweep 
		utils
		pthread)

	add_executable(bruteForce source/executable/bruteForce)
		target_link_libraries(bruteForce 
			utils)

	add_executable(testBruteForce source/executable/testBruteForce)
		target_link_libraries(testBruteForce 
			utils)

	add_executable(testSpikingSudokuSolver source/executable/testSpikingSudokuSolver)
		target_link_libraries(testSpikingSudokuSolver 
			utils)
			
    add_executable(testSpikingSudokuSolver_single source/executable/testSpikingSudokuSolver_single)
		target_link_libraries(testSpikingSudokuSolver_single 
			utils)
endif(BUILD_EXECUTABLE)
