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
	string height = {"3"};
	string width = {"3"};

	json averages;

	int sudoku_num = atoi(argv[1]);
    std::cout << sudoku_num << "Sudoku"<<std::endl;

	SpikingSolverSinglePop solver;

	string config_path =
	    "../config/" + height + "X" + width + "parameters_curr.json";
	solver.setConfig(config_path);
	string directory = "../sudokus/" + height + "x" + width + "/Sudoku_" +
	                   std::to_string(sudoku_num) + ".json";
	Sudoku sudoku(directory);
	solver.initialize(sudoku);
	int bin_size = solver.config()["bin_size"];
	const char *simulator = "nest={\"threads\": 4}";
	/*const char *simulator =
	    "spinnaker={\"source_neurons_per_core\": 40, \"neurons_per_core\" : "
	    "40, \"timestep\": 0.1}";
	*/solver.run(argv[0], simulator);
	vector<vector<vector<int>>> result = solver.evaluate();
	double duration = 0.0;
	for (size_t k = 0; k < result[0][0].size() - 1; k++) {
		sudoku = solver.updateSudokuFromResult(sudoku, result, k);
		if (sudoku.complete() && !overwrite(sudoku)) {
			duration = (double)k * (double)bin_size;
			break;
		}
	}

	NetworkRuntime runtime = solver.network().runtime();

	std::ofstream file("res.txt", std::ofstream::app);
	file << sudoku_num << "," << duration << "," << runtime.initialize << ","
	     << runtime.sim << "," << runtime.finalize << std::endl;
	file.close();
}
