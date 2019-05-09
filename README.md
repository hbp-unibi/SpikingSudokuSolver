# SpikingSudokuSolver
A spiking Sudoku Solver written in C++ using Cypress (supporting NEST, SpiNNaker, Spikey, BrainScaleS)

In order to build the program you need a C++14 compliant compiler and QT4 packages installed. Execute the following to download and build the program

```bash
git clone https://github.com/hbp-unibi/SpikingSudokuSolver
mkdir SpikingSudokuSolver/build && cd SpikingSudokuSolver/build
cmake ..
make
```

For running simulations, we assume that either the simulator NEST is already installed (using python2.7 at the moment) or one of the hardware systems Spikey, SpiNNaker or BrainScaleS available and installed.

In the created `build` folder you can now execute `./SNN-Sudoku-Solver`. The GUI allows you to change several settings:

 * **Solvers**:
   * `Spiking`: The normal implementation using a population for every number  
   * `Spiking, Single Pop`: Using a single population, manually addressing sub-pops and using a single projection, specialized for the SpiNNaker platform, network itself is the same as above
   * `Spiking, Single Neuron`: The same implementation as above, using a simplified structure and only one neuron per population
   * `Spiking, Mirror Inhib`: Mirror inhibition and self-excitation, network design for the Spikey platform
 * **Parameter File**: Choose the parameter file in dependence of solver and target platform
 * **Simulator**: The target simulator
 * **Sudoku Drawing Tools**:
   * Either choose a given Sudoku using **Get Sudoku**, or define a Sudoku size and use **Update Sudoku**, which fetches a freshly generated Sudoku from the Internet
 * **Solve By Spiking**: Executes the simulation, results will be displayed. Use the scroll bar to go through the time development of the network
 * **Make Video**: After scrolling through all steps, use this button to auto-generate a video, which is saved in `build/Save_Sudoku_png/` in a separate folder
 * **LED Cube**: This is used to visualize a small Sudoku on a 8x8x8 LED Cube available in our LAB
