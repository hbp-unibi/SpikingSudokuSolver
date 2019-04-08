/**
 * @file testSpikingSudokuSolver.cpp
 *
 *
 * @authors Ferdinand Schlatt
 */
#include "source/utils/spikingSudokuSolver.cpp"

#include <string>
#include <vector>

using json = nlohmann::json;

bool overwrite(Sudoku sudoku)
{
	vector<vector<int>> blank = sudoku.getSudokuBlank();
	for (int row = 0; row < sudoku.getNumOfSquares(); row++) {
		for (int col = 0; col < sudoku.getNumOfSquares(); col++) {
			if (blank[row][col] != 0 &&
			    blank[row][col] != sudoku.getEntryInSquare(row, col)) {
				return true;
			}
		}
	}
	return false;
}

int main(int, const char *argv[])
{
	vector<string> heights = {"2", "2", "2", "3", "3"};
	vector<string> widths = {"2", "3", "4", "3", "4"};

	// vector<string> heights = {"3"};
	// vector<string> widths = {"3"};

	json averages;

	vector<double> durations;
	vector<int> not_solved;
	vector<double> finalization_times;
	vector<double> initialization_times;
	vector<double> simulation_times;

	spikingSudokuSolver solver;

	for (std::vector<int>::size_type i = 0; i != heights.size(); i++) {
		string height = heights[i];
		string width = widths[i];
		std::cout << height << "x" << width << std::endl;
		string config_path =
		    "../config/" + height + "X" + width + "parameters.json";
		solver.setConfig(config_path);
		for (int j = 0; j < 100; j++) {
			string directory = "../sudokus/" + height + "x" + width +
			                   "/Sudoku_" + std::to_string(j) + ".json";
			Sudoku sudoku(directory);
			solver.initialize(sudoku);
			int bin_size = solver.config()["bin_size"];
			const char *simulator = "pynn.nest={\"threads\": 8}";
			solver.run(argv[0], simulator);
			vector<vector<vector<int>>> result = solver.evaluate();
			double duration = 0.0;
			for (size_t k = 0; k < result[0][0].size() - 1; k++) {
				sudoku = solver.updateSudokuFromResult(sudoku, result, k);
				if (sudoku.complete() && !overwrite(sudoku)) {
					duration = (double)k * (double)bin_size;
					break;
				}
			}
			if (duration) {
				durations.push_back(duration);
			}
			else {
				not_solved.push_back(j);
			}
			NetworkRuntime runtime = solver.network().runtime();
			finalization_times.push_back(runtime.finalize);
			initialization_times.push_back(runtime.initialize);
			simulation_times.push_back(runtime.sim);
			averages[height + string("x") + width + " times"] = durations;
			averages[height + string("x") + width + " not solved"] = not_solved;
			averages[height + string("x") + width + " finalize time"] =
			    finalization_times;
			averages[height + string("x") + width + " initialize time"] =
			    initialization_times;
			averages[height + string("x") + width + " simulation time"] =
			    simulation_times;
			std::cout << duration << std::endl;
			std::ofstream file;
			file.open("../evaluation/spikingSudokuSolver.json");
			file << averages;
			file.close();
		}
		double average = accumulate(durations.begin(), durations.end(), 0.0) /
		                 durations.size();
		averages[height + string("x") + width + " average"] = average;
		std::ofstream file;
		file.open("../evaluation/spikingSudokuSolver.json");
		file << averages;
		file.close();
		durations.clear();
		not_solved.clear();
		finalization_times.clear();
		initialization_times.clear();
		simulation_times.clear();
	}
}
