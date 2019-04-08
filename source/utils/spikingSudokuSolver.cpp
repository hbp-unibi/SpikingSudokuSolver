/**
 * @file spikingSudokuSolver.cpp
 *
 *
 * @authors Tim Westerhold, Balavivek Sivanantham, Ferdinand Schlatt, Christoph
 * Ostrau
 */

#include <cypress/cypress.hpp>

#include <cypress/backend/power/power.hpp>

#include "source/utils/spikingSudokuSolver.hpp"

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <vector>

#include "source/utils/Sudoku.hpp"

using namespace cypress;

spikingSudokuSolver::spikingSudokuSolver() : netw()
{

	// Reading the configuration file
	std::ifstream ifs("../config/parameters.json");

	if (!ifs.good()) {
		// Maybe we are a in a folder above that
		ifs.open("../../config/parameters.json");
		if (!ifs.good()) {
			throw std::runtime_error("Could not open config file");
		}
	}
	// Json config;
	m_config << ifs;
}

spikingSudokuSolver::spikingSudokuSolver(std::string parameter_file)
{
	if (parameter_file == "") {
		parameter_file = "../config/parameters.json";
	}
	// Reading the configuration file
	std::ifstream ifs(parameter_file);

	if (!ifs.good()) {
		// Maybe we are a in a folder above that
		ifs.open("../" + parameter_file);
		if (!ifs.good()) {
			throw std::runtime_error("Could not open config file");
		}
	}
	// Json config
	m_config << ifs;
}

const Real &spikingSudokuSolver::duration() const { return m_duration; }
const Json &spikingSudokuSolver::config() const { return m_config; }
const cypress::Network &spikingSudokuSolver::network() const { return netw; }

void spikingSudokuSolver::setConfig(string path)
{
	// Reading the configuration file
	std::ifstream ifs(path);

	if (!ifs.good()) {
		throw std::runtime_error("Could not open config file");
	}
	m_config << ifs;
}

namespace {
void convert_to_json(
    std::vector<
        std::vector<std::vector<std::vector<std::vector<cypress::Real>>>>>
        &spikes,
    std::string path, cypress::Json &json, bool to_file)
{
	json[path] = spikes;
	if (to_file) {
		std::ofstream file;
		file.open("spikes.json", std::ofstream::out);
		if (!file.good()) {
			throw std::runtime_error("Could not open spikes.json file");
		}
		file << json;
		file.close();
	}
}
}  // namespace

void spikingSudokuSolver::initialize(Sudoku sudokuGen)
{

	// clear network and population array
	population_array.clear();
	netw = Network();

	// Neuronparameter
	NeuronParameter neuro_params(IfCondExp::inst(), m_config["neuron_params"]);

	// Networkparameter
	Real weight_trigger = Real(m_config["trigger"]["weight"]);
	Real weight_rand = Real(m_config["noise"]["weight_rand"]);
	Real weight_exh = Real(m_config["population"]["weight_exh"]);
	Real weight_inh = Real(m_config["population"]["weight_inh"]);
	size_t fan_in_exh = Real(m_config["population"]["fan_in_exh"]);
	size_t fan_in_inh = Real(m_config["population"]["fan_in_inh"]);

	Real start = Real(m_config["start"]);
	int pop_neurons_num = (m_config["population"]["number"]);
	int trigger_neurons_num = (m_config["trigger"]["number"]);
	Real noise_rate = Real(m_config["noise"]["noise_rate"]);
	Real trigger_rate = Real(m_config["trigger"]["trigger_rate"]);

	Real delay = 1.0;

	// Simulation-parameter
	m_duration = m_config["duration"];

	if (m_duration <= 1.0) {
		throw std::invalid_argument(
		    "Duration of simulation must be greater 1.0");
	}

	// Sudokuparameter
	block_height = sudokuGen.getHeight();
	block_width = sudokuGen.getWidth();
	sudoku_height = sudokuGen.getNumOfSquares();
	sudoku_width = sudokuGen.getNumOfSquares();
	int count_blocks_vertical = sudoku_height / block_height;
	int count_blocks_horizontal = sudoku_width / block_width;
	int count_numbers = block_height * block_width;
	const vector<vector<int>> sudoku = sudokuGen.getSudokuBlank();

	// Initializing the population_array
	std::vector<cypress::Population<IfCondExp>> help_vector_1d;
	std::vector<std::vector<cypress::Population<IfCondExp>>> help_vector_2d;

	// Creating the population_array
	for (int i = 0; i < sudoku_height; i++) {
		help_vector_2d.clear();
		for (int j = 0; j < sudoku_width; j++) {
			help_vector_1d.clear();
			for (int k = 0; k < count_numbers; k++) {
				help_vector_1d.push_back(netw.create_population<IfCondExp>(
				    pop_neurons_num, neuro_params.parameter(),
				    IfCondExpSignals({"spikes"})));
			}
			help_vector_2d.push_back(help_vector_1d);
		}
		population_array.push_back(help_vector_2d);
	}

	// Creating the noise sources and connecting to population_array
	for (int i = 0; i < sudoku_height; i++) {
		for (int j = 0; j < sudoku_width; j++) {
			for (int k = 0; k < count_numbers; k++) {
				netw.add_connection(netw.create_population<SpikeSourcePoisson>(
				                        pop_neurons_num,
				                        SpikeSourcePoissonParameters()
				                            .rate(noise_rate)
				                            .start(start)
				                            .duration(m_duration),
				                        SpikeSourcePoissonSignals(), "noise"),
				                    population_array[i][j][k],
				                    Connector::one_to_one(weight_rand, delay));
				num_connections++;
			}
		}
	}

	// Creating and connecting trigger sources
	auto trigger = netw.create_population<SpikeSourcePoisson>(
	    trigger_neurons_num,
	    SpikeSourcePoissonParameters()
	        .rate(trigger_rate)
	        .start(start)
	        .duration(m_duration),
	    SpikeSourcePoissonSignals(), "trigger");
	for (int i = 0; i < sudoku_height; i++) {
		for (int j = 0; j < sudoku_width; j++) {
			if (sudokuGen.getEntryInSquareBlank(i, j) != 0) {
				int number = sudokuGen.getEntryInSquareBlank(i, j);
				netw.add_connection(
				    trigger, population_array[i][j][number - 1],
				    Connector::all_to_all(weight_trigger, delay));
				num_connections++;
			}
		}
	}

	// Connecting populations
	for (int f = 0; f < count_numbers; f++) {
		for (int g = 0; g < sudoku_width; g++) {
			for (int h = 0; h < sudoku_height; h++) {
				netw.add_connection(
				    population_array[h][g][f], population_array[h][g][f],
				    Connector::fixed_fan_in(fan_in_exh, weight_exh, delay));
				// for testing
				num_connections++;
				// inhibitory internumberconnections
				for (int k = f + 1; k < count_numbers; k++) {
					netw.add_connection(
					    population_array[h][g][f], population_array[h][g][k],
					    Connector::fixed_fan_in(fan_in_inh, weight_inh, delay));
					netw.add_connection(
					    population_array[h][g][k], population_array[h][g][f],
					    Connector::fixed_fan_in(fan_in_inh, weight_inh, delay));
					// for testing
					num_connections += 2;
				}
				// inhibitory horizontal connections
				for (int k = g + 1; k < sudoku_width; k++) {
					netw.add_connection(
					    population_array[h][g][f], population_array[h][k][f],
					    Connector::fixed_fan_in(fan_in_inh, weight_inh, delay));
					netw.add_connection(
					    population_array[h][k][f], population_array[h][g][f],
					    Connector::fixed_fan_in(fan_in_inh, weight_inh, delay));
					// for testing
					num_connections += 2;
				}
				// inhibitory vertical connections
				for (int k = h + 1; k < sudoku_height; k++) {
					netw.add_connection(
					    population_array[h][g][f], population_array[k][g][f],
					    Connector::fixed_fan_in(fan_in_inh, weight_inh, delay));
					netw.add_connection(
					    population_array[k][g][f], population_array[h][g][f],
					    Connector::fixed_fan_in(fan_in_inh, weight_inh, delay));
					// for testing
					num_connections += 2;
				}
			}
		}

		// insideblockconnections
		for (int g = 0; g < count_blocks_horizontal; g++) {
			for (int h = 0; h < count_blocks_vertical; h++) {
				for (int i = 0; i < block_width; i++) {
					for (int j = 0; j < block_height; j++) {
						for (int k = 0; k < block_width; k++) {
							for (int l = 0; l < block_height; l++) {
								if ((k != i) & (l != j)) {
									netw.add_connection(
									    population_array[h * block_height + j]
									                    [g * block_width + i]
									                    [f],
									    population_array[h * block_height + l]
									                    [g * block_width + k]
									                    [f],
									    Connector::fixed_fan_in(
									        fan_in_inh, weight_inh, delay));
									// for testing
									num_connections++;
								}
							}
						}
					}
				}
			}
		}
	}
}

void spikingSudokuSolver::run(const char *, const char *simulator = "pynn.nest",
                              bool cube)
{
	global_logger().min_level(LogSeverity::INFO);

	int count_numbers = block_height * block_width;

	// Run the simulation for "duration" seconds
	cypress::PowerManagementBackend pwbackend(
	    cypress::Network::make_backend(simulator));
	netw.run(pwbackend, this->m_duration);

	// Save spikes to disk, plot spike times
	// row<col<number<population<spikes>>>>
	std::vector<
	    std::vector<std::vector<std::vector<std::vector<cypress::Real>>>>>
	    network_spikes;
	std::vector<std::vector<std::vector<std::vector<cypress::Real>>>>
	    row_spikes;
	std::vector<std::vector<std::vector<cypress::Real>>> column_spikes;
	std::vector<std::vector<cypress::Real>> number_spikes;
	int size = population_array[0][0][0].size();

	network_spikes.clear();
	for (int h = 0; h < sudoku_height; h++) {
		row_spikes.clear();
		for (int g = 0; g < sudoku_width; g++) {
			column_spikes.clear();
			for (int f = 0; f < count_numbers; f++) {
				number_spikes.clear();
				for (int i = 0; i < size; i++) {
					number_spikes.push_back(
					    population_array[h][g][f][i].signals().data(0));
				}
				column_spikes.push_back(number_spikes);
			}
			row_spikes.push_back(column_spikes);
		}
		network_spikes.push_back(row_spikes);
	}
	spikes = network_spikes;
	if (cube) {
		Json json;
		convert_to_json(spikes, "sudoku", json, true);
		system("killall python3");
		system(("python3 ../source/utils/cube.py -hist 30 -time " +
		        std::to_string(int(this->m_duration)) + " -wait 0.001 &")
		           .c_str());
	}
}

vector<vector<vector<int>>> spikingSudokuSolver::evaluate()
{
	// number of bins needed to split simulation time into bins
	int bin_size = m_config["bin_size"];
	int num_of_bins = m_duration / bin_size;

	// count vector to count all spikes of a number in a subsquare for ever bin
	vector<vector<vector<vector<int>>>> counts =
	    vector<vector<vector<vector<int>>>>(
	        sudoku_height,
	        vector<vector<vector<int>>>(
	            sudoku_width, vector<vector<int>>(
	                              sudoku_width, vector<int>(num_of_bins, 0))));

	// result vector used to hold max spike count over all numbers
	vector<vector<vector<int>>> result = vector<vector<vector<int>>>(
	    sudoku_height,
	    vector<vector<int>>(sudoku_width, vector<int>(num_of_bins, 0)));
	// go through all rows columns numbers and neurons and count the spikes
	// and add up the spikes for every number
	for (int row = 0; row < sudoku_height; row++) {
		for (int col = 0; col < sudoku_width; col++) {
			for (int number = 0; number < sudoku_width; number++) {
				for (size_t neuron = 0;
				     neuron < spikes[row][col][number].size(); neuron++) {
					for (size_t spike = 0;
					     spike < spikes[row][col][number][neuron].size();
					     spike++) {
						int bin_idx =
						    spikes[row][col][number][neuron][spike] / bin_size;
						if (bin_idx <= num_of_bins - 1) {
							counts[row][col][number][bin_idx] += 1;
						}
					}
				}
			}
		}
	}
	// find the number with the maximum amount of spikes for every sudoku block
	// in every bin
	for (int row = 0; row < sudoku_height; row++) {
		for (int col = 0; col < sudoku_width; col++) {
			for (int bin = 0; bin < num_of_bins; bin++) {
				int max = 0;
				int argmax = 0;
				for (int number = 0; number < sudoku_width; number++) {
					if (counts[row][col][number][bin] > max) {
						max = counts[row][col][number][bin];
						argmax = number + 1;
					}
				}
				result[row][col][bin] = argmax;
			}
		}
	}
	return result;
}

Sudoku spikingSudokuSolver::updateSudokuFromResult(
    Sudoku sudoku, vector<vector<vector<int>>> result, int bin)
{
	for (int row = 0; row < sudoku.getNumOfSquares(); row++) {
		for (int col = 0; col < sudoku.getNumOfSquares(); col++) {
			int value = result[row][col][bin];
			sudoku.setEntryInSquare(row, col, value);
		}
	}
	return sudoku;
}

int spikingSudokuSolver::getNumConnections() const { return num_connections; }

int spikingSudokuSolver::getNumPopulations() const
{
	return netw.population_count<IfCondExp>();
}

int spikingSudokuSolver::getNumNoise() const
{
	return netw.populations("noise", cypress::SpikeSourcePoisson::inst())
	    .size();
}

int spikingSudokuSolver::getNumTrigger() const
{
	return netw.populations("trigger", cypress::SpikeSourcePoisson::inst())
	    .size();
}

void spikingSudokuSolver::saveSpikes() const
{
	std::ofstream file;
	unsigned int row, col, num, neuron, spike;

	file.open("../test/solver/spikes.csv", std::ofstream::out);
	if (!file.good()) {
		file.open("../../test/solver/spikes.csv", std::ofstream::out);
		if (!file.good()) {
			throw std::runtime_error("Could not open csv file");
		}
	}

	for (row = 0; row < spikes.size(); row++) {
		file << "{";
		for (col = 0; col < spikes[row].size(); col++) {
			file << "[";
			for (num = 0; num < spikes[row][col].size(); num++) {
				file << "(";
				for (neuron = 0; neuron < spikes[row][col][num].size();
				     neuron++) {
					for (spike = 0;
					     spike < spikes[row][col][num][neuron].size();
					     spike++) {
						file << spikes[row][col][num][neuron][spike] << ", ";
					}
					file << "; ";
				}
				file << ")";
			}
			file << "]";
		}
		file << "}";
	}
	file << std::endl;
	file.close();
}

void spikingSudokuSolver::loadSpikes()
{
	std::ifstream file;
	char c;
	std::string buffer;
	const char *buf;
	float spike_time = 0.0;
	std::vector<
	    std::vector<std::vector<std::vector<std::vector<cypress::Real>>>>>
	    network_spikes;
	std::vector<std::vector<std::vector<std::vector<cypress::Real>>>>
	    row_spikes;
	std::vector<std::vector<std::vector<cypress::Real>>> column_spikes;
	std::vector<std::vector<cypress::Real>> number_spikes;
	std::vector<cypress::Real> neuron_spikes;

	file.open("../test/solver/spikes.csv", std::ofstream::out);
	if (!file.good()) {
		file.open("../../test/solver/spikes.csv", std::ofstream::out);
		if (!file.good()) {
			throw std::runtime_error("Could not open csv file");
		}
	}

	for (;;) {
		file.get(c);
		if (file.eof() == true)
			break;
		else {
			if ((c == ' ') | (c == '{') | (c == '[') | (c == '(')) {}
			else {
				if (c == '}') {
					network_spikes.push_back(row_spikes);
					row_spikes.clear();
				}
				else {
					if (c == ']') {
						row_spikes.push_back(column_spikes);
						column_spikes.clear();
					}
					else {
						if (c == ')') {
							column_spikes.push_back(number_spikes);
							number_spikes.clear();
						}
						else {
							if (c == ';') {
								number_spikes.push_back(neuron_spikes);
								neuron_spikes.clear();
							}
							else {
								if (c == ',') {
									buf = buffer.c_str();
									spike_time = atof(buf);
									neuron_spikes.push_back(spike_time);
									buffer.clear();
								}
								else {
									buffer.push_back(c);
								}
							}
						}
					}
				}
			}
		}
	}
	spikes = network_spikes;
	file.close();
}

inline void fixed_fan_in(size_t start_id, size_t start_id2, size_t n_neurons,
                         size_t fan, vector<LocalConnection> &conns,
                         Real weight, Real delay)
{
	std::default_random_engine gen;

	for (size_t target = 0; target < n_neurons; target++) {
		vector<LocalConnection> temp_cons;
		for (size_t source = 0; source < n_neurons; source++) {
			temp_cons.push_back(LocalConnection(
			    start_id + source, start_id2 + target, weight, delay));
		}

		if (fan != n_neurons) {
			for (size_t index = 0; index < n_neurons - fan; index++) {
				std::uniform_int_distribution<size_t> distr(
				    0, temp_cons.size() - 1);
				temp_cons.erase(temp_cons.begin() + distr(gen));
			}
		}
		conns.insert(conns.end(), temp_cons.begin(), temp_cons.end());
	}
}

void SpikingSolverSinglePop::initialize(Sudoku sudokuGen)
{
	// clear network and population array
	population_array.clear();
	netw = cypress::Network();

	bool current_based = false;

	if (m_config.find("current_based") != m_config.end()) {
		current_based = m_config["current_based"];
	}
	// Neuronparameter
	NeuronParameter neuro_params;
	if (!current_based) {
		neuro_params =
		    NeuronParameter(IfCondExp::inst(), m_config["neuron_params"]);
	}
	else {
		neuro_params =
		    NeuronParameter(IfCurrExp::inst(), m_config["neuron_params"]);
	}
	// Networkparameter
	Real weight_trigger = Real(m_config["trigger"]["weight"]);
	Real weight_rand = Real(m_config["noise"]["weight_rand"]);
	Real weight_exh = Real(m_config["population"]["weight_exh"]);
	Real weight_inh = Real(m_config["population"]["weight_inh"]);
	size_t fan_in_exh = Real(m_config["population"]["fan_in_exh"]);
	size_t fan_in_inh = Real(m_config["population"]["fan_in_inh"]);

	Real start = Real(m_config["start"]);
	m_pop_neurons_num = m_config["population"]["number"];
	int trigger_neurons_num = m_config["trigger"]["number"];
	Real noise_rate = Real(m_config["noise"]["noise_rate"]);
	Real trigger_rate = Real(m_config["trigger"]["trigger_rate"]);

	Real delay = 1.0;

	m_duration = m_config["duration"];
	if (m_duration <= 1.0) {
		throw std::invalid_argument(
		    "Duration of simulation must be greater 1.0");
	}

	// Sudoku parameter
	block_height = sudokuGen.getHeight();
	block_width = sudokuGen.getWidth();
	sudoku_height = sudokuGen.getNumOfSquares();
	sudoku_width = sudokuGen.getNumOfSquares();
	int count_blocks_vertical = sudoku_height / block_height;
	int count_blocks_horizontal = sudoku_width / block_width;
	m_count_numbers = block_height * block_width;
	const vector<vector<int>> sudoku = sudokuGen.getSudokuBlank();

	int num_neurons =
	    m_pop_neurons_num * m_count_numbers * sudoku_height * sudoku_width;
	std::shared_ptr<PopulationBase> pop;
    if(!current_based){
        pop =  std::make_shared<PopulationBase>(netw.create_population<IfCondExp>(
	    num_neurons, neuro_params.parameter(), IfCondExpSignals({"spikes"}),
	    "target"));
    }
    else{
        pop =  std::make_shared<PopulationBase>(netw.create_population<IfCurrExp>(
	    num_neurons, neuro_params.parameter(), IfCurrExpSignals({"spikes"}),
	    "target"));
    }

	auto source = netw.create_population<SpikeSourcePoisson>(
	    num_neurons,
	    SpikeSourcePoissonParameters()
	        .rate(noise_rate)
	        .start(start)
	        .duration(m_duration),
	    SpikeSourcePoissonSignals(), "noise");

	netw.add_connection(source, *pop,
	                    Connector::one_to_one(weight_rand, delay));
	num_connections += m_count_numbers * sudoku_height * sudoku_width;

	// Create and connect trigger sources
	auto trigger = netw.create_population<SpikeSourcePoisson>(
	    trigger_neurons_num,
	    SpikeSourcePoissonParameters()
	        .rate(trigger_rate)
	        .start(start)
	        .duration(m_duration),
	    SpikeSourcePoissonSignals(), "trigger");
	for (int i = 0; i < sudoku_height; i++) {
		for (int j = 0; j < sudoku_width; j++) {
			if (sudokuGen.getEntryInSquareBlank(i, j) != 0) {
				int number = sudokuGen.getEntryInSquareBlank(i, j) - 1;
				int start_id = index(i, j, number);

				PopulationView<IfCondExp> popview(netw, pop->pid(), start_id,
				                                  start_id + m_pop_neurons_num);
				netw.add_connection(
				    trigger, popview,
				    Connector::all_to_all(weight_trigger, delay));
				num_connections++;
			}
		}
	}
	std::vector<LocalConnection> inhib_connections, exc_connections;
	// Inhibitory connections
	for (int i = 0; i < sudoku_height; i++) {
		for (int j = 0; j < sudoku_width; j++) {
			for (int k = 0; k < m_count_numbers; k++) {
				int start_id = index(i, j, k);

				// PopulationView<IfCondExp> popviewsource(
				//   netw, m_pop.pid(), start_id, start_id + m_pop_neurons_num);
				// Row wise
				for (int l = 0; l < sudoku_width; l++) {
					if (l == j) {
						continue;
					}
					int start_id2 = index(i, l, k);
					fixed_fan_in(start_id, start_id2, m_pop_neurons_num,
					             fan_in_inh, inhib_connections, weight_inh,
					             delay);
					num_connections++;
				}

				// Col wise
				for (int l = 0; l < sudoku_height; l++) {
					if (l == i) {
						continue;
					}
					int start_id2 = index(l, j, k);
					fixed_fan_in(start_id, start_id2, m_pop_neurons_num,
					             fan_in_inh, inhib_connections, weight_inh,
					             delay);
					num_connections++;
				}

				// Suppress other numbers
				for (int l = 0; l < m_count_numbers; l++) {
					if (l == k) {
						continue;
					}
					int start_id2 = index(i, j, l);
					fixed_fan_in(start_id, start_id2, m_pop_neurons_num,
					             fan_in_inh, inhib_connections, weight_inh,
					             delay);
					num_connections++;
				}

				// Suppress in Block
				// Current Block
				int block_id_y = i / count_blocks_horizontal;
				int block_id_x = j / count_blocks_vertical;
				// Current field in Block
				int field_id_y = i % count_blocks_horizontal;
				int field_id_x = j % count_blocks_vertical;

				for (int l = 0; l < block_height; l++) {     // y
					for (int m = 0; m < block_width; m++) {  // x

						if ((field_id_x == m) || (field_id_y == l)) {
							continue;
						}
						int start_id2 =
						    index(block_id_y * block_height + l,
						          (block_id_x * block_width + m), k);
						fixed_fan_in(start_id, start_id2, m_pop_neurons_num,
						             fan_in_inh, inhib_connections, weight_inh,
						             delay);
						num_connections++;
						num_connections++;
					}
				}
				// Self-Connection

				fixed_fan_in(start_id, start_id, m_pop_neurons_num, fan_in_exh,
				             exc_connections, weight_exh, delay);
				num_connections++;
				num_connections++;
			}
		}
	}
	netw.add_connection(*pop, *pop, Connector::from_list(inhib_connections));
	netw.add_connection(*pop, *pop, Connector::from_list(exc_connections));
}

void SpikingSolverSinglePop::run(const char *, const char *simulator, bool cube)
{
	global_logger().min_level(LogSeverity::INFO);

	m_count_numbers = block_height * block_width;

	// Run the simulation for "duration" seconds
	cypress::PowerManagementBackend pwbackend(
	    cypress::Network::make_backend(simulator));
	netw.run(pwbackend, this->m_duration);

	// Save spikes to disk, plot spike times
	// row<col<number<population<spikes>>>>
	std::vector<
	    std::vector<std::vector<std::vector<std::vector<cypress::Real>>>>>
	    network_spikes;
	std::vector<std::vector<std::vector<std::vector<cypress::Real>>>>
	    row_spikes;
	std::vector<std::vector<std::vector<cypress::Real>>> column_spikes;
	std::vector<std::vector<cypress::Real>> number_spikes;

	m_pop_neurons_num = (m_config["population"]["number"]);

	network_spikes.clear();
	auto m_pop = netw.populations("target")[0];

	for (int h = 0; h < sudoku_height; h++) {
		row_spikes.clear();
		for (int g = 0; g < sudoku_width; g++) {
			column_spikes.clear();
			for (int f = 0; f < m_count_numbers; f++) {
				number_spikes.clear();
				int start_id = index(h, g, f);
				for (int i = 0; i < m_pop_neurons_num; i++) {
					number_spikes.push_back(
					    m_pop[start_id + i].signals().data(0));
				}
				column_spikes.push_back(number_spikes);
			}
			row_spikes.push_back(column_spikes);
		}
		network_spikes.push_back(row_spikes);
	}
	spikes = network_spikes;

	if (cube) {
		Json json;
		convert_to_json(spikes, "sudoku", json, true);
		system("killall python3");
		system(("python3 ../source/utils/cube.py -hist 30 -time " +
		        std::to_string(int(this->m_duration)) + " -wait 0.001 &")
		           .c_str());
	}
}

void SSolveMirrorInhib::initialize(Sudoku sudokuGen)
{
	// clear network and population array
	population_array.clear();
	netw = cypress::Network();

	// Neuronparameter
	NeuronParameter neuro_params(IfFacetsHardware1::inst(),
	                             m_config["neuron_params"]);
	NeuronParameter neuro_params_inhib(IfFacetsHardware1::inst(),
	                                   m_config["neuron_params_inhib"]);

	// Networkparameter
	float weight_trigger = cypress::Real(m_config["trigger"]["weight"]);
	float weight_rand = cypress::Real(m_config["noise"]["weight_rand"]);
	float probability = cypress::Real(m_config["noise"]["probability"]);
	float weight_exh = cypress::Real(m_config["population"]["weight_exh"]);
	float weight_exh_self =
	    cypress::Real(m_config["population"]["weight_exh_self"]);
	float weight_inh = cypress::Real(m_config["population"]["weight_inh"]);
	float start = cypress::Real(m_config["start"]);
	m_pop_neurons_num = (m_config["population"]["number"]);
	int noise_neurons_num = (m_config["noise"]["number"]);
	int inhib_neurons_num = (m_config["population"]["inhib_number"]);
	int self_neurons_num = (m_config["population"]["self_exc_number"]);
	float noise_rate = cypress::Real(m_config["noise"]["noise_rate"]);
	m_duration = m_config["duration"];
	if (m_duration <= 1.0) {
		throw std::invalid_argument(
		    "Duration of simulation must be greater 1.0");
	}

	// Sudoku parameter
	block_height = sudokuGen.getHeight();
	block_width = sudokuGen.getWidth();
	sudoku_height = sudokuGen.getNumOfSquares();
	sudoku_width = sudokuGen.getNumOfSquares();
	int count_blocks_vertical = sudoku_height / block_height;
	int count_blocks_horizontal = sudoku_width / block_width;
	m_count_numbers = block_height * block_width;
	const vector<vector<int>> sudoku = sudokuGen.getSudokuBlank();

	int num_neurons =
	    m_pop_neurons_num * m_count_numbers * sudoku_height * sudoku_width;
	auto pop = netw.create_population<IfFacetsHardware1>(
	    num_neurons, neuro_params.parameter(),
	    IfFacetsHardware1Signals({"spikes"}), "target");
	int num_all_inhib_neurons =
	    inhib_neurons_num * m_count_numbers * sudoku_height * sudoku_width;
	auto pop_inhib = netw.create_population<IfFacetsHardware1>(
	    num_all_inhib_neurons, neuro_params_inhib.parameter(),
	    IfFacetsHardware1Signals({"spikes"}), "inhibit");

	int num_all_self_neurons =
	    self_neurons_num * m_count_numbers * sudoku_height * sudoku_width;
	auto pop_self_exc = netw.create_population<IfFacetsHardware1>(
	    num_all_self_neurons, neuro_params_inhib.parameter(),
	    IfFacetsHardware1Signals({"spikes"}), "self_exc");

	auto source = netw.create_population<SpikeSourcePoisson>(
	    noise_neurons_num * m_count_numbers * sudoku_height * sudoku_width,
	    SpikeSourcePoissonParameters()
	        .rate(noise_rate)
	        .start(start)
	        .duration(m_duration),
	    SpikeSourcePoissonSignals(), "noise");

	Real delay = 0.0;
	std::vector<LocalConnection> inputs;

	std::uniform_real_distribution<Real> distr(0.0, 1.0);
	std::default_random_engine eng;
	// Create the noise sources and connect to population_array
	for (int i = 0; i < sudoku_height; i++) {
		for (int j = 0; j < sudoku_width; j++) {
			for (int k = 0; k < m_count_numbers; k++) {
				int start_id = index(i, j, k);
				int start_id2 =
				    i * sudoku_width * m_count_numbers * noise_neurons_num +
				    j * m_count_numbers * noise_neurons_num +
				    k * noise_neurons_num;
				Real weight_ = weight_rand;
				Real probability_ = probability;
				if (sudokuGen.getEntryInSquareBlank(i, j) - 1 == k) {
					// For triggered neurons
					probability_ = 1;
					weight_ = weight_trigger;
				}
				for (int n_src = 0; n_src < noise_neurons_num; n_src++) {
					for (int n_tar = 0; n_tar < m_pop_neurons_num; n_tar++) {
						if (distr(eng) <= probability_) {
							inputs.push_back(LocalConnection(start_id2 + n_src,
							                                 start_id + n_tar,
							                                 weight_, delay));
						}
					}
				}
				num_connections++;
			}
		}
	}
	netw.add_connection(source, pop, Connector::from_list(inputs));

	// Inhibitory connections
	for (int i = 0; i < sudoku_height; i++) {
		for (int j = 0; j < sudoku_width; j++) {
			for (int k = 0; k < m_count_numbers; k++) {
				int start_id = index(i, j, k);
				int start_id_inhib =
				    i * sudoku_width * m_count_numbers * inhib_neurons_num +
				    j * m_count_numbers * inhib_neurons_num +
				    k * inhib_neurons_num;
				int start_id_self =
				    i * sudoku_width * m_count_numbers * self_neurons_num +
				    j * m_count_numbers * self_neurons_num +
				    k * self_neurons_num;

				PopulationView<IfCondExp> popview(netw, pop.pid(), start_id,
				                                  start_id + m_pop_neurons_num);
				PopulationView<IfCondExp> popview_inhib(
				    netw, pop_inhib.pid(), start_id_inhib,
				    start_id_inhib + inhib_neurons_num);

				netw.add_connection(popview, popview_inhib,
				                    Connector::all_to_all(weight_exh, delay));

				PopulationView<IfCondExp> popview_self(
				    netw, pop_self_exc.pid(), start_id_self,
				    start_id_self + self_neurons_num);
				// Self-Connection
				netw.add_connection(
				    popview, popview_self,
				    Connector::all_to_all(weight_exh_self, delay));
				netw.add_connection(
				    popview_self, popview,
				    Connector::all_to_all(weight_exh_self, delay));
				num_connections++;

				// Row wise
				for (int l = 0; l < sudoku_width; l++) {
					if (l == j) {
						continue;
					}
					int start_id2 = index(i, l, k);
					PopulationView<IfCondExp> popviewtar(
					    netw, pop.pid(), start_id2,
					    start_id2 + m_pop_neurons_num);
					netw.add_connection(
					    popview_inhib, popviewtar,
					    Connector::all_to_all(weight_inh, delay));
					num_connections++;
				}

				// Col wise
				for (int l = 0; l < sudoku_height; l++) {
					if (l == i) {
						continue;
					}
					int start_id2 = index(l, j, k);
					PopulationView<IfCondExp> popviewtar(
					    netw, pop.pid(), start_id2,
					    start_id2 + m_pop_neurons_num);
					netw.add_connection(
					    popview_inhib, popviewtar,
					    Connector::all_to_all(weight_inh, delay));
					num_connections++;
				}

				// Suppress other numbers
				for (int l = 0; l < m_count_numbers; l++) {
					if (l == k) {
						continue;
					}
					int start_id2 = index(i, j, l);
					PopulationView<IfCondExp> popviewtar(
					    netw, pop.pid(), start_id2,
					    start_id2 + m_pop_neurons_num);
					netw.add_connection(
					    popview_inhib, popviewtar,
					    Connector::all_to_all(weight_inh, delay));
					num_connections++;
				}

				// Suppress in Block
				// Current Block
				int block_id_x = j / count_blocks_horizontal;
				int block_id_y = i / count_blocks_vertical;
				// Current field in Block
				int field_id_x = j % count_blocks_horizontal;
				int field_id_y = i % count_blocks_vertical;
				for (int l = 0; l < block_height; l++) {
					for (int m = 0; m < block_width; m++) {
						if ((field_id_x == m) || (field_id_y == l)) {
							continue;
						}
						int start_id2 =
						    index(block_id_y * block_height + l,
						          (block_id_x * block_width + m), k);
						PopulationView<IfCondExp> popviewtar(
						    netw, pop.pid(), start_id2,
						    start_id2 + m_pop_neurons_num);
						netw.add_connection(
						    popview_inhib, popviewtar,
						    Connector::all_to_all(weight_inh, delay));
						num_connections++;
					}
				}
			}
		}
	}
}

void SSolveMirrorInhib::run(const char *, const char *simulator, bool cube)
{
	global_logger().min_level(LogSeverity::INFO);

	m_count_numbers = block_height * block_width;

	// Run the simulation for "duration" seconds
	cypress::PowerManagementBackend pwbackend(
	    cypress::Network::make_backend(simulator));
	netw.run(pwbackend, this->m_duration);

	// Save spikes to disk, plot spike times
	// row<col<number<population<spikes>>>>
	std::vector<
	    std::vector<std::vector<std::vector<std::vector<cypress::Real>>>>>
	    network_spikes;
	std::vector<std::vector<std::vector<std::vector<cypress::Real>>>>
	    row_spikes;
	std::vector<std::vector<std::vector<cypress::Real>>> column_spikes;
	std::vector<std::vector<cypress::Real>> number_spikes;

	network_spikes.clear();
	auto m_pop = netw.population("target");

	for (int h = 0; h < sudoku_height; h++) {
		row_spikes.clear();
		for (int g = 0; g < sudoku_width; g++) {
			column_spikes.clear();
			for (int f = 0; f < m_count_numbers; f++) {
				number_spikes.clear();
				int start_id = index(h, g, f);
				for (int i = 0; i < m_pop_neurons_num; i++) {
					number_spikes.push_back(
					    m_pop[start_id + i].signals().data(0));
				}
				column_spikes.push_back(number_spikes);
			}
			row_spikes.push_back(column_spikes);
		}
		network_spikes.push_back(row_spikes);
	}
	spikes = network_spikes;

	if (cube) {
		int inhib_neurons_num = (m_config["population"]["inhib_number"]);
		int self_neurons_num = (m_config["population"]["self_exc_number"]);
		network_spikes.clear();
		auto pop = netw.population("inhibit");
		for (int h = 0; h < sudoku_height; h++) {
			row_spikes.clear();
			for (int g = 0; g < sudoku_width; g++) {
				column_spikes.clear();
				for (int f = 0; f < m_count_numbers; f++) {
					number_spikes.clear();
					int start_id =
					    h * sudoku_width * m_count_numbers * inhib_neurons_num +
					    g * m_count_numbers * inhib_neurons_num +
					    f * inhib_neurons_num;
					for (int i = 0; i < inhib_neurons_num; i++) {
						number_spikes.push_back(
						    pop[start_id + i].signals().data(0));
					}
					column_spikes.push_back(number_spikes);
				}
				row_spikes.push_back(column_spikes);
			}
			network_spikes.push_back(row_spikes);
		}
		Json json;
		convert_to_json(spikes, "sudoku", json, false);
		convert_to_json(network_spikes, "inhibit", json, false);

		network_spikes.clear();
		pop = netw.population("self_exc");
		for (int h = 0; h < sudoku_height; h++) {
			row_spikes.clear();
			for (int g = 0; g < sudoku_width; g++) {
				column_spikes.clear();
				for (int f = 0; f < m_count_numbers; f++) {
					number_spikes.clear();
					int start_id =
					    h * sudoku_width * m_count_numbers * self_neurons_num +
					    g * m_count_numbers * self_neurons_num +
					    f * self_neurons_num;
					for (int i = 0; i < self_neurons_num; i++) {
						number_spikes.push_back(
						    pop[start_id + i].signals().data(0));
					}
					column_spikes.push_back(number_spikes);
				}
				row_spikes.push_back(column_spikes);
			}
			network_spikes.push_back(row_spikes);
		}
		convert_to_json(network_spikes, "self", json, true);
		system("killall python3");
		system(("python3 ../source/utils/cube.py -hist 100 -time " +
		        std::to_string(int(this->m_duration)) + " -wait 0.01 -s&")
		           .c_str());
	}
}

void SpikingSolverSingleNeuron::initialize(Sudoku sudokuGen)
{
	// clear network and population array
	this->population_array.clear();
	this->netw = cypress::Network();

	// Neuronparameter
	NeuronParameter neuro_params(IfCondExp::inst(), m_config["neuron_params"]);

	// Networkparameter
	Real weight = Real(m_config["trigger"]["weight"]);
	Real weight_rand = Real(m_config["noise"]["weight_rand"]);
	Real weight_exh = Real(m_config["population"]["weight_exh"]);
	Real weight_inh = Real(m_config["population"]["weight_inh"]);
	Real start = Real(m_config["start"]);
	u_int32_t trigger_neurons_num = u_int32_t(m_config["trigger"]["number"]);
	Real noise_rate = Real(m_config["noise"]["noise_rate"]);
	Real trigger_rate = Real(m_config["trigger"]["trigger_rate"]);
	m_duration = m_config["duration"];
	if (m_duration <= 1.0) {
		throw std::invalid_argument(
		    "Duration of simulation must be greater 1.0");
	}

	// Sudoku parameter
	block_height = sudokuGen.getHeight();
	block_width = sudokuGen.getWidth();
	sudoku_height = sudokuGen.getNumOfSquares();
	sudoku_width = sudokuGen.getNumOfSquares();
	int count_blocks_vertical = sudoku_height / block_height;
	int count_blocks_horizontal = sudoku_width / block_width;
	m_count_numbers = block_height * block_width;
	const vector<vector<int>> sudoku = sudokuGen.getSudokuBlank();

	int num_neurons = m_count_numbers * sudoku_height * sudoku_width;
	auto m_pop = netw.create_population<IfCondExp>(
	    num_neurons, neuro_params.parameter(), IfCondExpSignals({"spikes"}),
	    "target");

	auto source = netw.create_population<SpikeSourcePoisson>(
	    num_neurons,
	    SpikeSourcePoissonParameters()
	        .rate(noise_rate)
	        .start(start)
	        .duration(m_duration),
	    SpikeSourcePoissonSignals({"spikes"}), "noise");

	Real delay = 0.0;
	netw.add_connection(source, m_pop,
	                    Connector::one_to_one(weight_rand, delay));

	// Self Connection
	netw.add_connection(m_pop, m_pop, Connector::one_to_one(weight_exh, delay));

	num_connections += m_pop.size();

	// Create and connect trigger sources
	auto pop_trigger = netw.create_population<SpikeSourcePoisson>(
	    trigger_neurons_num,
	    SpikeSourcePoissonParameters()
	        .rate(trigger_rate)
	        .start(start)
	        .duration(m_duration),
	    SpikeSourcePoissonSignals({"spikes"}), "trigger");

	std::vector<LocalConnection> connections;
	for (int i = 0; i < sudoku_height; i++) {
		for (int j = 0; j < sudoku_width; j++) {
			if (sudokuGen.getEntryInSquareBlank(i, j) != 0) {
				int number = sudokuGen.getEntryInSquareBlank(i, j) - 1;

				int start_id = i * sudoku_width * m_count_numbers +
				               j * m_count_numbers + number;
				connections.push_back(
				    LocalConnection(0, start_id, weight, delay));
				num_connections++;
			}
		}
	}
	netw.add_connection(pop_trigger, m_pop, Connector::from_list(connections));

	std::vector<LocalConnection> inhib_connections;
	// Inhibitory connections
	for (int i = 0; i < sudoku_height; i++) {
		for (int j = 0; j < sudoku_width; j++) {
			for (int k = 0; k < m_count_numbers; k++) {
				uint32_t start_id = i * sudoku_width * m_count_numbers +
				                    j * m_count_numbers + k;

				// Row wise
				for (int l = 0; l < sudoku_width; l++) {
					if (l == j) {
						continue;
					}
					uint32_t start_id2 = i * sudoku_width * m_count_numbers +
					                     l * m_count_numbers + k;

					inhib_connections.push_back(LocalConnection(
					    start_id, start_id2, weight_inh, delay));
					num_connections++;
				}

				// Col wise
				for (int l = 0; l < sudoku_height; l++) {
					if (l == i) {
						continue;
					}
					uint32_t start_id2 = l * sudoku_width * m_count_numbers +
					                     j * m_count_numbers + k;
					inhib_connections.push_back(LocalConnection(
					    start_id, start_id2, weight_inh, delay));
					num_connections++;
				}

				// Suppress other numbers
				for (int l = 0; l < m_count_numbers; l++) {
					if (l == k) {
						continue;
					}
					uint32_t start_id2 = i * sudoku_width * m_count_numbers +
					                     j * m_count_numbers + l;
					inhib_connections.push_back(LocalConnection(
					    start_id, start_id2, weight_inh, delay));
					num_connections++;
				}

				// Suppress in Block
				// Current Block
				int block_id_x = j / count_blocks_horizontal;
				int block_id_y = i / count_blocks_vertical;
				// Current field in Block
				int field_id_x = j % count_blocks_horizontal;
				int field_id_y = i % count_blocks_vertical;
				for (int l = 0; l < block_height; l++) {
					for (int m = 0; m < block_width; m++) {
						if ((field_id_x == m) || (field_id_y == l)) {
							continue;
						}
						uint32_t start_id2 =
						    (block_id_y * block_height + l) * sudoku_width *
						        m_count_numbers +
						    (block_id_x * block_width + m) * m_count_numbers +
						    k;
						inhib_connections.push_back(LocalConnection(
						    start_id, start_id2, weight_inh, delay));
						num_connections++;
					}
				}
			}
		}
	}
	netw.add_connection(m_pop, m_pop, Connector::from_list(inhib_connections));
}
void SpikingSolverSingleNeuron::run(const char *, const char *simulator,
                                    bool cube)
{
	global_logger().min_level(LogSeverity::INFO);

	m_count_numbers = block_height * block_width;

	// Run the simulation for "duration" seconds
	cypress::PowerManagementBackend pwbackend(
	    cypress::Network::make_backend(simulator));
	netw.run(pwbackend, this->m_duration);

	// Save spikes to disk, plot spike times
	// row<col<number<population<spikes>>>>
	std::vector<
	    std::vector<std::vector<std::vector<std::vector<cypress::Real>>>>>
	    network_spikes;
	std::vector<std::vector<std::vector<std::vector<cypress::Real>>>>
	    row_spikes;
	std::vector<std::vector<std::vector<cypress::Real>>> column_spikes;
	std::vector<std::vector<cypress::Real>> number_spikes;

	network_spikes.clear();
	auto m_pop = netw.population("target");

	for (int h = 0; h < sudoku_height; h++) {
		row_spikes.clear();
		for (int g = 0; g < sudoku_width; g++) {
			column_spikes.clear();
			for (int f = 0; f < m_count_numbers; f++) {
				number_spikes.clear();
				int start_id = h * sudoku_width * m_count_numbers +
				               g * m_count_numbers + f;
				number_spikes.push_back(m_pop[start_id].signals().data(0));
				column_spikes.push_back(number_spikes);
			}
			row_spikes.push_back(column_spikes);
		}
		network_spikes.push_back(row_spikes);
	}
	spikes = network_spikes;

	if (cube) {
		Json json;
		convert_to_json(spikes, "sudoku", json, true);
		system("killall python3");
		system(("python3 ../source/utils/cube.py -hist 30 -time " +
		        std::to_string(int(this->m_duration)) + " -wait 0.001 &")
		           .c_str());
	}
}
