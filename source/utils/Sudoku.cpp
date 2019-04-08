/**
 * @file Sudoku.cpp
 *
 *
 * @authors Ferdinand Schlatt
 */
#include "source/utils/Sudoku.hpp"

#include <algorithm>
#include <cypress/json.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
using json = nlohmann::json;

Sudoku::Sudoku(void)
/**
creates a sudoku from default sudoku file path "Sudoku.json"

throws exception if no file is found at path
*/
{
}

Sudoku::Sudoku(int h, int w)
/**
scrapes and creates sudoku given height and width of a subsquare,
scraped from 'http://www.menneske.no/sudoku/'

subsquares may be non-square, dimensions must be between 2 and 9
for non-square sudoku maximum size is 3x5 or 5x3

@param h height of a subsquare
@param w width of a subsquare
*/
{
	if (h < 2 || w < 2 || h > 9 || w > 9) {
		throw std::invalid_argument("Incorrect Sudoku Dimensions, min 2 max 9");
	}
	else if (h != w && (h > 5 || w > 5)) {
		throw std::invalid_argument(
		    "Incorrect Sudoku Dimensions, unsymmetrical maximum of 3x5 or 5x3 "
		    "allowed");
	}
	else if ((h == 4 && w == 5) || (h == 5 && w == 4)) {
		throw std::invalid_argument(
		    "Incorrect Sudoku Dimensions, unsymmetrical maximum of 3x5 or 5x3 "
		    "allowed");
	}
	height = h;
	width = w;
	numOfSquares = height * width;
	json sudoku_file = scrape();
	sudoku = json_to_array(sudoku_file["sudoku"], numOfSquares);
	auto solution_found = sudoku_file.find("solution");
	if (solution_found != sudoku_file.end()) {
		sudokuSolution = json_to_array(sudoku_file["solution"], numOfSquares);
	}
	else {
		sudokuSolution = json_to_array(sudoku_file["sudoku"], numOfSquares);
	}
	if (height > width) {
		sudoku = transpose(sudoku);
		sudokuSolution = transpose(sudokuSolution);
	}
	sudokuBlank = sudoku;
	valid();
}

Sudoku::Sudoku(string path)
/**
creates a sudoku from a given sudoku file path

throws exception if no file is found at path

@param path sudoku file path as .json file
*/
{
	from_file(path);
	valid();
}

Sudoku::~Sudoku() {}

void Sudoku::reset(void)
/**
resets all non predetermined spaces of sudoku to blank
*/
{
	sudoku = sudokuBlank;
}

void Sudoku::from_file(string path)
/**
reads a sudoku given a json file path, throws runtime_error
if the path doesn't exist

sets height and width of sudoku given the parameters in sudoku file
*/
{
	json sudoku_file;
	// std::cout << path << std::endl;
	std::ifstream file(path.c_str());
	if (file.good()) {
		file >> sudoku_file;
		height = int(sudoku_file["height"]);
		width = int(sudoku_file["width"]);
		numOfSquares = height * width;
		sudoku = json_to_array(sudoku_file["sudoku"], numOfSquares);
		auto solution_found = sudoku_file.find("solution");
		if (solution_found != sudoku_file.end()) {
			sudokuSolution =
			    json_to_array(sudoku_file["solution"], numOfSquares);
		}
		else {
			sudokuSolution = json_to_array(sudoku_file["sudoku"], numOfSquares);
		}
		sudokuBlank = sudoku;
	}
	else {
		throw std::runtime_error("File path: " + path + " not found!");
	}
}

json Sudoku::scrape()
/**
scrapes a sudoku from 'http://www.menneske.no/sudoku/' using a python script
and saves and then reads it from a "Sudoku.json" file

throws a runtime_error if the scraping failed
*/
{
	json sudoku_file;
	int ret = system(("python ../scrape/scrapeSudoku.py " + to_string(height) +
	                  " " + to_string(width))
	                     .c_str());
	if (ret != 0) {
		throw std::runtime_error("Scraping failed, website may be down!");
	}
	std::ifstream file("Sudoku.json");
	file >> sudoku_file;
	return sudoku_file;
}

vector<vector<int>> Sudoku::json_to_array(json sudoku_file, int numOfSquares)
/**
converts an input sudoku json file to an array/vector

@param sudoku_file input file from which the sudoku should be converted
*/
{
	vector<vector<int>> new_sudoku;
	new_sudoku = vector<vector<int>>(numOfSquares, vector<int>(numOfSquares));
	for (int i = 0; i < numOfSquares; ++i) {
		for (int j = 0; j < numOfSquares; ++j) {
			new_sudoku[i][j] = int(sudoku_file[i][j]);
		}
	}
	return new_sudoku;
}

vector<int> Sudoku::subsq(int indx)
/**
gets the subsquare at an index of a sudoku

@param indx the index of the subsquare which should be returned
@return a subsquare as a 1D vector of length width * height
*/
{
	vector<int> temp(numOfSquares);
	for (int j = 0; j < height; ++j) {
		for (int k = 0; k < width; ++k) {
			temp[j * width + k] = sudoku[(indx / height) * height + j]
			                            [(indx % height) * width + k];
		}
	}
	return temp;
}

vector<int> Sudoku::col(int indx)
/**
gets the column at an index of a sudoku

@param indx the index of the column which should be returned
@return a column as a 1D vector of length width * height
*/
{
	vector<int> temp(numOfSquares);
	for (int i = 0; i < numOfSquares; ++i) {
		temp[i] = sudoku[i][indx];
	}
	return temp;
}

vector<int> Sudoku::row(int indx)
/**
gets the row at an index of a sudoku

@param indx the index of the row which should be returned
@return a row as a 1D vector of length width * height
*/
{
	return sudoku[indx];
}

vector<vector<int>> Sudoku::transpose(const vector<vector<int>> &arr) const
/**
transposes a 2D vector, here used to convert between representations
of unsymmetrical sudokus, i.e. between 2x4 and 4x2

@param arr 2D vector which should be transposed
@return transposed 2D vector
*/
{
	vector<vector<int>> newarr =
	    vector<vector<int>>(numOfSquares, vector<int>(numOfSquares));
	for (int i = 0; i < numOfSquares; ++i) {
		for (int j = 0; j < numOfSquares; ++j) {
			newarr[i][j] = arr[j][i];
		}
	}
	return newarr;
}

int Sudoku::getEntryInSquare(int x, int y)
/**
gets the entry in a sudoku at the coordinates x and y,

throws an invalid argument error if the coordinates are outside
the range of valid values

@param x x coordinate of the entry
@param y y coordinate of the entry
@return entry at the coordinate x,y
*/
{
	if (x < 0 || x > numOfSquares - 1 || y < 0 || y > numOfSquares - 1) {
		throw std::invalid_argument(
		    "Invalid x or y coordinate, needs to be 0 <= x,y < " +
		    to_string(numOfSquares - 1));
	}
	return sudoku[x][y];
}

int Sudoku::getEntryInSquareBlank(int x, int y)
/**
gets the entry in a blank sudoku at the coordinates x and y,

throws an invalid argument error if the coordinates are outside
the range of valid values

@param x x coordinate of the entry
@param y y coordinate of the entry
@return entry at the coordinate x,y
*/
{
	if (x < 0 || x > numOfSquares - 1 || y < 0 || y > numOfSquares - 1) {
		throw std::invalid_argument(
		    "Invalid x or y coordinate, needs to be 0 <= x,y < " +
		    to_string(numOfSquares - 1));
	}
	return sudokuBlank[x][y];
}

int Sudoku::getEntryInSquareSolution(int x, int y)
/**
gets the entry in a solved sudoku at the coordinates x and y,

throws an invalid argument error if the coordinates are outside
the range of valid values

@param x x coordinate of the entry
@param y y coordinate of the entry
@return entry at the coordinate x,y
*/
{
	if (x < 0 || x > numOfSquares - 1 || y < 0 || y > numOfSquares - 1) {
		throw std::invalid_argument(
		    "Invalid x or y coordinate, needs to be 0 <= x,y < " +
		    to_string(numOfSquares - 1));
	}
	return sudokuSolution[x][y];
}

void Sudoku::setEntryInSquare(int x, int y, int value)
/**
sets an entry in a sudoku given the x,y coordinates and a value

@param x x coordinate at which value should be written
@param y y coordinate at which value should be written
@param value the value which should be written at position x,y
*/
{
	if (value > numOfSquares || value < 0) {
		throw std::invalid_argument(
		    "Invalid number entry, needs to be 0 <= value <= " +
		    to_string(numOfSquares - 1));
	}
	if (x < 0 || x > numOfSquares - 1 || y < 0 || y > numOfSquares - 1) {
		throw std::invalid_argument(
		    "Invalid x or y coordinate, needs to be 0 <= x < " +
		    to_string(numOfSquares - 1));
	}
	sudoku[x][y] = value;
}

void Sudoku::to_file(string path)
/**
writes the sudoku as well as height and width information
to a json at a specified path

@param path sudoku file path at which the sudoku should be written
*/
{
	json out;
	out["sudoku"] = sudoku;
	out["width"] = width;
	out["height"] = height;
	std::ofstream file;
	file.open(path);
	file << out;
	file.close();
}

void Sudoku::print(void)
/**
prints a suduko in ASCII style to the console, useful for debugging
*/
{
	for (int i = 0; i < numOfSquares; ++i) {
		if (i % height == 0)
			cout << std::string(height * width * 4 + height + 1, '-') << "\n";
		for (int j = 0; j < numOfSquares; ++j) {
			if (j % width == 0)
				cout << "|";
			if (sudoku[i][j] < 10) {
				cout << "  " << sudoku[i][j] << " ";
			}
			else {
				cout << " " << sudoku[i][j] << " ";
			}
		}
		cout << "|\n";
	}
	cout << std::string(height * width * 4 + height + 1, '-') << "\n";
}

bool Sudoku::valid(void)
/**
checks if a sudoku is still valid by checking if mupltiple of the same
numbers are written in the same column, row and/or subsquare

@return true if the sudoku is stil valid, false otherwise
*/
{
	for (int i = 0; i < numOfSquares; ++i) {
		vector<int> row_vec = row(i);
		vector<int> col_vec = col(i);
		vector<int> subsq_vec = subsq(i);
		vector<int> row_count(numOfSquares + 1, 0);
		vector<int> col_count(numOfSquares + 1, 0);
		vector<int> subsq_count(numOfSquares + 1, 0);
		for (int j = 0; j < numOfSquares; ++j) {
			if (row_vec[j] != 0 && row_count[row_vec[j]] > 0) {
				// cout << "Invalid Sudoku in row " << i+1 << "!" << std::endl;
				return false;
			}
			if (col_vec[j] != 0 && col_count[col_vec[j]] > 0) {
				// cout << "Invalid Sudoku in column " << i+1 << "!" <<
				// std::endl;
				return false;
			}
			if (subsq_vec[j] != 0 && subsq_count[subsq_vec[j]] > 0) {
				// cout << "Invalid Sudoku in subsquare " << i+1 << "!" <<
				// std::endl;
				return false;
			}
			row_count[row_vec[j]] += 1;
			col_count[col_vec[j]] += 1;
			subsq_count[subsq_vec[j]] += 1;
		}
	}
	return true;
}

bool Sudoku::complete(void)
/**
checks if a sudoku is complete by checking if it is valid
and contains no zeros

@return true if the sudoku is complete, false otherwise
*/
{
	if (!valid()) {
		return false;
	}
	for (int row = 0; row < numOfSquares; row++) {
		for (int col = 0; col < numOfSquares; col++) {
			if (sudoku[row][col] == 0) {
				return false;
			}
		}
	}
	return true;
}

const vector<vector<int>> &Sudoku::getSudoku() const
/**
getter function to get 2D sudoku vector

@return sudoku as 2D vector
*/
{
	return sudoku;
}

const vector<vector<int>> &Sudoku::getSudokuBlank() const
/**
getter function to get the blank 2D sudoku vector
(all non predetermined fields are blank)

@return the blank sudoku as 2D vector
*/
{
	return sudokuBlank;
}

const vector<vector<int>> &Sudoku::getSudokuSolution() const
/**
getter function to get the solved 2D sudoku vector

@return the solved sudoku as 2D vector
*/
{
	return sudokuSolution;
}

int Sudoku::getHeight()
/**
getter function for height of sudoku subblock

@return height of sudoku subblock
*/
{
	return height;
}
int Sudoku::getWidth()
/**
getter function for width of sudoku subblock

@return width of sudoku subblock
*/
{
	return width;
}

int Sudoku::getNumOfSquares()
/**
getter function for height, width and number of
squares per subblock of entire sudoku
(all values are the same)

@return height/width/number of squares per sublock
*/
{
	return numOfSquares;
}
