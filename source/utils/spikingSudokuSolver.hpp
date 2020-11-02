/**
 * @file spikingSudokuSolver.hpp
 *
 *
 * @authors Tim Westerhold, Balavivek Sivanantham, Ferdinand Schlatt, Christoph
 * Ostrau
 */

#pragma once

#include <cypress/cypress.hpp>
#include <fstream>
#include <iostream>
#include <vector>

#include "source/utils/Sudoku.hpp"

using namespace cypress;

class spikingSudokuSolver {
protected:
	// Sudokuparameter
	int sudoku_height = 0;
	int sudoku_width = 0;
	int block_height = 0;
	int block_width = 0;

	// Testvariables
	int num_connections = 0;  // except trigger- and noiseconnections

	/*
	 * Vector of rows<vector of colums per row<vector of numbers per
	 * field<vector of neurons per population<vector of spikes>>>> example:
	 * spikes[2][4][3][15][100]	-> spike : row 2, column 4, number 3, neuron 15,
	 * spiketime 100
	 */
	std::vector<
	    std::vector<std::vector<std::vector<std::vector<cypress::Real>>>>>
	    spikes;

	// Network
	cypress::Network netw;
	std::vector<std::vector<std::vector<cypress::Population<IfCondExp>>>>
	    population_array;

	// Configration
	Real m_duration = 0.0;
	Json m_config;

public:
	/**
	 * Constructor
	 * reads parameters from json file and creates the network
	 * throws exeption if error state flag of stream is set
	 */
	spikingSudokuSolver();

	/**
	 * Constructor using a user-defined parameters file
	 */
	spikingSudokuSolver(std::string parameter_file);

	/**
	 * Constructor using a user-defined json parameter
	 */
	spikingSudokuSolver(Json config) : m_config(config){};
	virtual ~spikingSudokuSolver() = default;

	/**
	 * initializes the solver
	 * builds neuron populations and connections as well as source populations
	 * for a given sudoku
	 *
	 * @param sudokuGen  A Sudoku from class sudoku
	 */
	virtual void initialize(Sudoku sudokuGen);

	/**
	 * initializes the solver
	 * builds neuron populations and connections as well as source populations
	 * for a given sudoku into the given network object
	 *
	 * @param sudokuGen  A Sudoku from class sudoku
	 * @param netw Network instance
	 */
	virtual void initialize(Sudoku sudokuGen, cypress::Network &netw);

	/**
	 * Executing the network and preparing the spike data
	 *
	 * @param instance argv[0]
	 * @param simulator simulator string
	 */
	virtual void run(const char *instance, const char *simulator,
	                 bool cube = false, bool dot = false,
	                 bool eval_only = false);

	/**
	 * adds filled out numbers to sudoku from results of a simulation
	 *
	 * @param sudoku  A Sudoku from class sudoku
	 * @param result  array of results over time from simulation
	 */
	Sudoku updateSudokuFromResult(Sudoku,
	                              std::vector<std::vector<std::vector<int>>>,
	                              int bin);

	/**
	 * Evaluates the simulation of the solver (by Balavivek)
	 * @param bin size in ms
	 * @return returns 3-D array of sudoku over time
	 */
	vector<vector<vector<int>>> evaluate();

	/**
	 * sets config of solver
	 * throws excpetion if error state flag of stream is set
	 */
	void setConfig(string path);
	void setConfig(Json &json) { m_config = json; }
	Json getConfig() const { return m_config; }

	// Getters for internal configuration members
	/**
	 * @return duration of the network simulation
	 */
	const Real &duration() const;

	/**
	 * @return const pointer to internal config
	 */
	const Json &config() const;
	const cypress::Network &network() const;

	// Testfunctions

	/**
	 * for testing
	 *
	 * @return number of connections between the populations
	 */
	int getNumConnections() const;

	/**
	 * for testing
	 *
	 * @return number of populations
	 */
	int getNumPopulations() const;

	/**
	 * for testing
	 *
	 * @return number of noise populations
	 */
	int getNumNoise() const;

	/**
	 * for testing
	 *
	 * @return number of trigger populations
	 */
	int getNumTrigger() const;

	/**
	 * saves the spikes in a csv file
	 */
	void saveSpikes() const;

	/**
	 * loads the spikes from a csv file
	 */
	void loadSpikes();
};

class SpikingSolverSinglePop : public spikingSudokuSolver {
protected:
	int m_count_numbers = 0, m_pop_neurons_num = 0;

	/*
	 * i is height, j is width, number is the number
	 */
	int index(int i, int j, int number)
	{
		return i * sudoku_width * m_count_numbers * m_pop_neurons_num +
		       j * m_count_numbers * m_pop_neurons_num +
		       number * m_pop_neurons_num;
	};

	std::shared_ptr<PopulationBase> m_pop;

public:
	/**
	 * Constructor. Calls the constructor of the base class reading
	 * config/parameters.json
	 */
	SpikingSolverSinglePop() : spikingSudokuSolver() {}

	/**
	 * Constructor using a user-defined parameter file
	 * @param parameter_file: string with relative or absolute path to config
	 * file (relative to the folder the executable is called from)
	 */
	SpikingSolverSinglePop(std::string parameter_file)
	    : spikingSudokuSolver(parameter_file)
	{
	}

	/**
	 * Constructor using a user-defined json parameter
	 */
	SpikingSolverSinglePop(Json config) : spikingSudokuSolver(config){};

	/**
	 * Initializes the network: Construction of the spiking neural network
	 *
	 * @param sudokuGen The Sudoku instance from which the network is created
	 * from. It contains the dimensions of the network and the given numbers,
	 * which are triggered by additional spike sources
	 * @param netw Network instance
	 */
	virtual void initialize(Sudoku sudokuGen, cypress::Network &netw) override;

	/**
	 * Initializes the network: Construction of the spiking neural network
	 *
	 * @param sudokuGen The Sudoku instance from which the network is created
	 * from. It contains the dimensions of the network and the given numbers,
	 * which are triggered by additional spike sources
	 */
	virtual void initialize(Sudoku sudokuGen) override;

	/**
	 * Executing the network and preparing the spike data
	 *
	 * @param instance argv[0]
	 * @param simulator simulator string
	 */
	virtual void run(const char *instance, const char *simulator,
	                 bool cube = false, bool dot = false,
	                 bool eval_only = false) override;
};

class SSolveMirrorInhib : public SpikingSolverSinglePop {
public:
	/**
	 * Constructor. Calls the constructor of the base class reading
	 * config/parameters.json
	 */
	SSolveMirrorInhib()
	    : SpikingSolverSinglePop(
	          std::string("../config/parameters_mirror.json"))
	{
	}

	/**
	 * Constructor using a user-defined parameter file
	 * @param parameter_file: string with relative or absolute path to config
	 * file (relative to the folder the executable is called from)
	 */
	SSolveMirrorInhib(std::string parameter_file)
	    : SpikingSolverSinglePop(parameter_file)
	{
	}

	/**
	 * Constructor using a user-defined json parameter
	 */
	SSolveMirrorInhib(Json config) : SpikingSolverSinglePop(config){};

	/**
	 * Initializes the network: Construction of the spiking neural network
	 *
	 * @param sudokuGen The Sudoku instance from which the network is created
	 * from. It contains the dimensions of the network and the given numbers,
	 * which are triggered by additional spike sources
	 * @param netw Network instance
	 */
	void initialize(Sudoku sudokuGen, cypress::Network &netw) override;

	/**
	 * Initializes the network: Construction of the spiking neural network
	 *
	 * @param sudokuGen The Sudoku instance from which the network is created
	 * from. It contains the dimensions of the network and the given numbers,
	 * which are triggered by additional spike sources
	 */
	void initialize(Sudoku sudokuGen) override;

	void run(const char *instance, const char *simulator, bool cube = false,
	         bool dot = false, bool eval_only = false) override;
};

class SpikingSolverSingleNeuron : public SpikingSolverSinglePop {
public:
	/**
	 * Constructor. Calls the constructor of the base class reading
	 * config/parameters.json
	 */
	SpikingSolverSingleNeuron() : SpikingSolverSinglePop() {}

	/**
	 * Constructor using a user-defined parameter file
	 * @param parameter_file: string with relative or absolute path to config
	 * file (relative to the folder the executable is called from)
	 */
	SpikingSolverSingleNeuron(std::string parameter_file)
	    : SpikingSolverSinglePop(parameter_file)
	{
	}

	/**
	 * Constructor using a user-defined json parameter
	 */
	SpikingSolverSingleNeuron(Json config) : SpikingSolverSinglePop(config){};

	/**
	 * Initializes the network: Construction of the spiking neural network
	 *
	 * @param sudokuGen The Sudoku instance from which the network is created
	 * from. It contains the dimensions of the network and the given numbers,
	 * which are triggered by additional spike sources
	 * @param netw Network instance
	 */
	virtual void initialize(Sudoku sudokuGen, cypress::Network &netw) override;

	/**
	 * Initializes the network: Construction of the spiking neural network
	 *
	 * @param sudokuGen The Sudoku instance from which the network is created
	 * from. It contains the dimensions of the network and the given numbers,
	 * which are triggered by additional spike sources
	 */
	virtual void initialize(Sudoku sudokuGen) override;

	/**
	 * Executing the network and preparing the spike data
	 *
	 * @param instance argv[0]
	 * @param simulator simulator string
	 */
	virtual void run(const char *instance, const char *simulator,
	                 bool cube = false, bool dot = false,
	                 bool eval_only = false) override;
};
