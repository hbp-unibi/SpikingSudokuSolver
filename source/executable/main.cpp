/**
 * @file main.cpp
 *
 *
 * @authors Ferdinand Schlatt
 */

#include "source/utils/spikingSudokuSolver.cpp"

int main(int, const char *argv[])
{
	const char *argv_ = argv[0];
	Sudoku sudoku(2, 3);
	spikingSudokuSolver solver;
	solver.initialize(sudoku);
	solver.run(argv_, argv[1]);
	vector<vector<vector<int>>> result = solver.evaluate();
	int num_of_bins = int(solver.duration()) / int(solver.config()["bin_size"]);
	sudoku = solver.updateSudokuFromResult(sudoku, result, num_of_bins - 1);
	sudoku.print();
	if (sudoku.complete()) {
		std::cout << "Sudoku is complete" << std::endl;
	}
	else {
		std::cout << "Sudoku is incomplete" << std::endl;
	}
	return 0;
}
