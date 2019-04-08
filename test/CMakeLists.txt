cmake_minimum_required (VERSION 3.0)

project(SNN-Sudoku-Solver)

# Copy Test.json to test folder

configure_file(${CMAKE_SOURCE_DIR}/test/sudoku/Test.json ${CMAKE_BINARY_DIR}/test/Test.json COPYONLY)

#
# Sudoku Class Test
#

add_executable(test_sudoku
    sudoku/test_sudoku
	${CMAKE_SOURCE_DIR}/source/utils/Sudoku
)
add_dependencies(test_sudoku cypress_ext)

target_link_libraries(test_sudoku
	${GTEST_LIBRARIES}
	-pthread
)
add_test(test_sudoku test_sudoku)

#
# Sudoku Solver Class Test
#


add_executable(test_solver
    solver/test_solver
 )
 add_dependencies(test_solver cypress_ext)
target_link_libraries(test_solver
	utils 
	${CYPRESS_LIBRARY}
	${GTEST_LIBRARIES}
	-pthread
 )
 add_test(test_solver test_solver)




