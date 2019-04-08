/**
 * @file main.cpp
 *
 *
 * @authors Ferdinand Schlatt, Christoph Ostrau
 */

#include "source/utils/spikingSudokuSolver.cpp"

#include <stdlib.h>

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

int main(int argc, const char *argv[])
{
	if (argc != 7) {
		std::cerr << "Wrong number of arguments! Usage: " << std::endl;
		std::cerr << argv[0]
		          << " height width neurons weight_exh weight_inh weight_rand"
		          << std::endl;
		return 1;
	}

	const char *simulator = "nest={\"threads\": 10}";
	// Parameters
	int height = atoi(argv[1]);
	int width = atoi(argv[2]);
	int neurons = atoi(argv[3]);
	double weight_exh = atof(argv[4]);
	double weight_inh = atof(argv[5]);
	double weight_rand = atof(argv[6]);

	string config_path;
	SpikingSolverSinglePop solver;
	try {
		config_path = "../config/" + to_string(height) + "X" +
		              to_string(width) + "parameters_curr.json";
		solver = SpikingSolverSinglePop(config_path);
		std::cerr << "Use current based neurons" << std::endl;
	}
	catch (...) {
		try {
			config_path = "../config/" + to_string(height) + "X" +
			              to_string(width) + "parameters.json";
			solver = SpikingSolverSinglePop(config_path);
			std::cerr << "Use conductance based neurons" << std::endl;
		}
		catch (...) {
			config_path = "../config/parameters.json";
			solver = SpikingSolverSinglePop(config_path);
			std::cerr << "Use conductance based neurons with default parameters"
			          << std::endl;
		}
	}
	// Adapt config
	auto config = solver.getConfig();
	config["population"]["weight_exh"] = weight_exh;
	config["population"]["weight_inh"] = weight_inh;
	config["population"]["number"] = neurons;
	config["population"]["fan_in_exh"] = neurons;
	config["population"]["fan_in_inh"] = neurons;
	config["noise"]["weight_rand"] = weight_rand;
	solver.setConfig(config);

	vector<double> durations;
	vector<int> not_solved;

	for (int j = 0; j < 100; j++) {
		string directory = "../sudokus/" + to_string(height) + "x" +
		                   to_string(width) + "/Sudoku_" + to_string(j) +
		                   ".json";
		Sudoku sudoku(directory);
		solver.initialize(sudoku);
		solver.run(argv[0], simulator);
		vector<vector<vector<int>>> result = solver.evaluate();
		double duration = 0.0;
		for (size_t k = 0; k < result[0][0].size() - 1; k++) {
			sudoku = solver.updateSudokuFromResult(sudoku, result, k);
			if (sudoku.complete() && !overwrite(sudoku)) {
				duration = (double)k * (double)solver.config()["bin_size"];
				break;
			}
		}
		if (duration) {
			durations.push_back(duration);
		}
		else {
			not_solved.push_back(j);
		}
		if (not_solved.size() > 14) {
			break;
		}
		std::cout << duration << std::endl;
	}

	/*std::string filename = std::string(argv[1]);
	for (size_t i = 2; i < 7; i++) {
		filename = filename + "_" + std::string(argv[i]);
	}

	std::ofstream file;
	file.open(filename, std::ios_base::out);*/
	std::cout << "==>>" << not_solved.size() << "," << durations.size() << ",";
	if (durations.size() > 0) {
		std::cout << std::accumulate(durations.begin(), durations.end(), 0.0) /
		            double(durations.size());
	}
	else {
		std::cout << 0;
	}
	std::cout <<std::endl;
	//file.close();
	return 0;
}
