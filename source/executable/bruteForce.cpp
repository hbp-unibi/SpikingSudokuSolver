/**
 * @file bruteForce.cpp
 *
 *
 * @authors Ferdinand Schlatt
 */
#include <ctime>
#include <sstream>
#include "source/utils/bruteForceSolver.cpp"

int main(int argc, const char *argv[])
{

	if (argc != 3) {
		std::cout << "Usage: " << argv[0]
		          << " {num of sudoku rows} {num of sudoku columns}"
		          << std::endl;
		return 1;
	}

	stringstream strValue_row;
	strValue_row << argv[1];
	int row;
	strValue_row >> row;

	stringstream strValue_col;
	strValue_col << argv[2];
	int col;
	strValue_col >> col;

	Sudoku sudoku(row, col);
	sudoku.print();
	std::clock_t start;
	start = std::clock();
	sudoku = bruteForceSolver::solve(sudoku);
	double duration = (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000);
	sudoku.print();
	if (sudoku.complete()) {
		std::cout << "Sudoku is complete" << std::endl;
	}
	else {
		std::cout << "Sudoku is incomplete" << std::endl;
	}
	std::cout << duration << std::endl;
	return 0;
}
