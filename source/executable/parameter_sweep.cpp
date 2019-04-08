#include "source/utils/spikingSudokuSolver.cpp"

std::vector<double> linspace(double start, double stop, double step_size)
{
	size_t steps = size_t((stop - start) / step_size);
	std::vector<double> res(steps + 1, 0);
	for (size_t i = 0; i <= steps; i++) {
		res[i] = start + i * step_size;
	}
	return res;
}
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
	const char *simulator = "pynn.nest={\"threads\": 2}";
	int height = atoi(argv[1]);
	int width = atoi(argv[2]);
	int neurons = atoi(argv[3]);
    std::cout << height << width << neurons<<std::endl;

	string config_path = "../config/" + to_string(height) + "X" +
	                     to_string(width) + "parameters.json";
	SpikingSolverSinglePop solver(config_path);
	auto config = solver.getConfig();
	int bin_size = solver.config()["bin_size"];

	std::vector<double> parameters1 = linspace(0.01, 0.5, 0.02);
	std::vector<double> parameters2 = linspace(-0.5, -0.01, 0.02);

	for (auto par1 : parameters1) {
		for (auto par2 : parameters2) {

			vector<double> durations;
			vector<int> not_solved;

			config["population"]["weight_exh"] = par1;
			config["population"]["weight_inh"] = par2;
			config["population"]["number"] = neurons;
			solver.setConfig(config);

			for (int j = 0; j < 100; j++) {
				string directory = "../sudokus/" + to_string(height) + "x" +
				                   to_string(width) + "/Sudoku_" +
				                   to_string(j) + ".json";
				Sudoku sudoku(directory);
				solver.initialize(sudoku);
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
				if (not_solved.size() > 25) {
					break;
				}
				std::cout << duration << std::endl;
			}

			std::ofstream file;
			file.open(("sweep_" + to_string(width) + "x" + to_string(height) +
			           "_" + to_string(neurons) + ".txt")
			              .c_str(),
			          std::ios_base::app);
			file << par1 << " , " << par2 << " , " << not_solved.size() << ", "
			     << accumulate(durations.begin(), durations.end(), 0.0) /
			            durations.size()
			     << std::endl;
			file.close();
		}
	}
	return 0;
}
