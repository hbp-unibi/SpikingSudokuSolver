/**
 * @file Sudoku.hpp
 *
 *
 * @authors Ferdinand Schlatt
 */

#pragma once

#include <algorithm>
#include <cypress/json.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using json = nlohmann::json;

class Sudoku {
public:
	Sudoku();
	Sudoku(int h, int w);
	Sudoku(string path);
	~Sudoku();
	int getEntryInSquare(int x, int y);
	int getEntryInSquareBlank(int x, int y);
	int getEntryInSquareSolution(int x, int y);
	void setEntryInSquare(int x, int y, int value);
	void print(void);
	bool valid(void);
	void to_file(string path = "Sudoku.json");
	void reset(void);
	void from_file(string path = "Sudoku.json");
	bool complete(void);
	const vector<vector<int>> &getSudoku() const;
	const vector<vector<int>> &getSudokuBlank() const;
	const vector<vector<int>> &getSudokuSolution() const;
	int getHeight();
	int getWidth();
	int getNumOfSquares();
	vector<int> row(int indx);
	vector<int> col(int indx);
	vector<int> subsq(int indx);

private:
	vector<vector<int>> sudoku = {{0}};
	vector<vector<int>> sudokuBlank = {{0}};
	vector<vector<int>> sudokuSolution = {{0}};
	vector<vector<int>> json_to_array(json sudoku, int numOfSquares);
	vector<vector<int>> transpose(const vector<vector<int>> &arr) const;
	json scrape(void);
	int height, width, numOfSquares;
};
