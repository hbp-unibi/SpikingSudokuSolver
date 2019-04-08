/**
 * @file bruteForceSolver.hpp
 *
 *
 * @authors Ferdinand Schlatt
 */
#include <vector>

#include "algorithm"
#include "source/utils/Sudoku.hpp"

class bruteForceSolver {
private:
	static vector<vector<vector<bool>>> resetChecked(
	    vector<vector<vector<bool>>>, int, int, int);
	static int getElligible(Sudoku, vector<vector<vector<bool>>>, int, int,
	                        int);
	static vector<int> backOne(int, int, int);
	static int getSubSquare(int, int, int, int);
	static bool valid(Sudoku, int, int, int);

public:
	static Sudoku solve(Sudoku);
};
