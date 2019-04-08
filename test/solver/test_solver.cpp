#include "source/utils/spikingSudokuSolver.cpp"
#include "gtest/gtest.h"



//test of constructor
TEST(spikingSudokuSolver, spikingSudokuSolver)
{
	spikingSudokuSolver s = spikingSudokuSolver();
	EXPECT_NO_THROW(spikingSudokuSolver s = spikingSudokuSolver());//<----------------------
}

std::string path_to_testfile = "Test.json";

//test of spiking network initialization
TEST(spikingSudokuSolver, initialize)
{
	spikingSudokuSolver s = spikingSudokuSolver();
	// Check where the testfile is located relative to executable
	std::ifstream ifs(path_to_testfile);
	if(!ifs.good()){
		path_to_testfile = "test/Test.json";
		ifs.open(path_to_testfile);
		if(!ifs.good()){
			throw "Could not locate Test.json. Please execute tests from test or build directory";
		}
	}
	ifs.close();
	Sudoku su = Sudoku(path_to_testfile);
	EXPECT_NO_THROW(s.initialize(su));
}


//test network parameters of spiking network initialization
TEST(spikingSudokuSolver, initialize_parameters)
{
	spikingSudokuSolver s = spikingSudokuSolver();
	Sudoku su = Sudoku(path_to_testfile);
	s.initialize(su);
    //4*4*4 Populations, each inhibiting 3 pops in row, 3 in col, 3 other numbers, 1 in Block. In addition there is 1 random input, 1 selfexcitation. ->12*4*4*4 + 6 inputs for given numbers in the Sudoku in test.json
	EXPECT_EQ(774, s.getNumConnections());
	EXPECT_EQ(64, s.getNumPopulations());
	EXPECT_EQ(64, s.getNumNoise());
	EXPECT_EQ(1, s.getNumTrigger());
}

TEST(SpikingSolverSinglePop, initialize_parameters)
{
	SpikingSolverSinglePop s = SpikingSolverSinglePop();
	Sudoku su = Sudoku(path_to_testfile);
	s.initialize(su);	
	EXPECT_EQ(774, s.getNumConnections());
    // Random input is represented by a single connection
	EXPECT_EQ(1, s.getNumPopulations());
	EXPECT_EQ(1, s.getNumNoise());
	EXPECT_EQ(1, s.getNumTrigger());
}

//test of evaluation
TEST(spikingSudokuSolver, evaluate)
{
	spikingSudokuSolver s = spikingSudokuSolver();
	Sudoku su(path_to_testfile);
	s.initialize(su);
	s.loadSpikes();
	EXPECT_NO_THROW(s.evaluate());
}
