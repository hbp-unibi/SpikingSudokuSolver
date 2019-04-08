/**
 * @file testBruteForce.cpp
 *
 *
 * @authors Ferdinand Schlatt
 */
#include <ctime>
#include <string>
#include <vector>
#include "source/utils/bruteForceSolver.cpp"

using json = nlohmann::json;

int main()
{

	vector<string> heights = {"2", "2", "2", "3", "3"};
	vector<string> widths = {"2", "3", "4", "3", "4"};

	json averages;

	vector<double> durations;

	std::clock_t start;

	std::string file_path = "../evaluation/bruteForceSolver.json";

	for (std::vector<int>::size_type i = 0; i != heights.size(); i++) {
		string height = heights[i];
		string width = widths[i];
		std::cout << height << "x" << width << std::endl;
		for (int j = 0; j < 100; j++) {
			string directory = "../sudokus/" + height + "x" + width +
			                   "/Sudoku_" + std::to_string(j) + ".json";
			Sudoku sudoku(directory);
			start = std::clock();
			sudoku = bruteForceSolver::solve(sudoku);
			double duration =
			    (std::clock() - start) / (double)(CLOCKS_PER_SEC / 1000);
			durations.push_back(duration);
			averages[height + string("x") + width + " times"] = durations;
			std::cout << duration << std::endl;
			std::ofstream file;
			file.open(file_path);
			file << averages;
			file.close();
		}
		double average = accumulate(durations.begin(), durations.end(), 0.0) /
		                 durations.size();
		averages[height + string("x") + width + " average"] = average;
		std::ofstream file;
		file.open(file_path);
		file << averages;
		file.close();
		durations.clear();
	}
}
