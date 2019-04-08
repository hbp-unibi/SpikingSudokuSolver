/**
 * @file bruteForceSolver.cpp
 *
 *
 * @authors Ferdinand Schlatt
 */

#include "bruteForceSolver.hpp"

Sudoku bruteForceSolver::solve(Sudoku sudoku)
{
	vector<vector<int>> sudoku_blank_vec = sudoku.getSudokuBlank();
	int numOfSquares = sudoku.getNumOfSquares();
	vector<vector<vector<bool>>> checked = vector<vector<vector<bool>>>(
	    numOfSquares,
	    vector<vector<bool>>(numOfSquares, vector<bool>(numOfSquares, false)));
	vector<vector<int>> last_changed;
	for (int row = 0; row < numOfSquares; row++) {
		for (int col = 0; col < numOfSquares; col++) {
			if (sudoku_blank_vec[row][col] != 0) {
				continue;
			}
			int number = getElligible(sudoku, checked, row, col, numOfSquares);
			// sudoku.print();
			// std::cout << "row: " << row << "  col: " << col << std::endl;
			// for (int i = 0; i < numOfSquares; i++) {
			//     std::cout << i+1 << ": " << checked[row][col][i] << "  ";
			// }
			// std::cout << std::endl;
			// std::cout << "elligible number: " << number << std::endl;
			if (number) {
				checked[row][col][number - 1] = true;
				sudoku.setEntryInSquare(row, col, number);
				last_changed.push_back(vector<int>({row, col}));
			}
			else {
				// std::cout << "reset at " << row << " " << col << std::endl;
				checked = resetChecked(checked, row, col, numOfSquares);
				vector<int> idcs = last_changed.back();
				last_changed.pop_back();
				row = idcs[0];
				col = idcs[1];
				if (sudoku_blank_vec[row][col] == 0) {
					sudoku.setEntryInSquare(row, col, 0);
				}
				// std::cout << "backtrack to " << row << " " << col <<
				// std::endl;
				if (col == 0) {
					row--;
					col = numOfSquares - 1;
				}
				else {
					col--;
				}
			}
			// cin.ignore();
			if (sudoku.complete()) {
				return sudoku;
			}
		}
	}
	return sudoku;
}

vector<vector<vector<bool>>> bruteForceSolver::resetChecked(
    vector<vector<vector<bool>>> checked, int row, int col, int numOfSquares)
{
	for (int number = 0; number < numOfSquares; number++) {
		checked[row][col][number] = false;
	}
	return checked;
}

int bruteForceSolver::getElligible(Sudoku sudoku,
                                   vector<vector<vector<bool>>> checked,
                                   int row, int col, int numOfSquares)
{
	for (int number = 0; number < numOfSquares; number++) {
		if (!checked[row][col][number]) {
			int value = number + 1;
			if (valid(sudoku, row, col, value)) {
				sudoku.setEntryInSquare(row, col, value);
				return value;
			}
		}
	}
	return 0;
}

int bruteForceSolver::getSubSquare(int row, int col, int rows, int cols)
{
	return ((row / rows) * rows) + (col / cols);
}

bool bruteForceSolver::valid(Sudoku sudoku, int row, int col, int value)
{
	int cols = sudoku.getWidth();
	int rows = sudoku.getHeight();
	int subsq = getSubSquare(row, col, rows, cols);
	vector<int> sudoku_row = sudoku.row(row);
	for (std::vector<int>::size_type i = 0; i != sudoku_row.size(); i++) {
		if (sudoku_row[i] == value) {
			return false;
		}
	}
	vector<int> sudoku_col = sudoku.col(col);
	for (std::vector<int>::size_type i = 0; i != sudoku_col.size(); i++) {
		if (sudoku_col[i] == value) {
			return false;
		}
	}
	vector<int> sudoku_subsq = sudoku.subsq(subsq);
	for (std::vector<int>::size_type i = 0; i != sudoku_subsq.size(); i++) {
		if (sudoku_subsq[i] == value) {
			return false;
		}
	}
	return true;
}
