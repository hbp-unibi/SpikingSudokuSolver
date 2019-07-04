import json
import numpy as np
import argparse

parser = argparse.ArgumentParser(description='Remove Spikes >t from json')
parser.add_argument("-time", type=int, required=True, help="Simulation time")
args = parser.parse_args()

with open('spikes.json') as f:
    data = json.load(f)

# data["sudoku"][row][col][num][neuron][spike]

num_neurons = len(data["sudoku"][0][0][0])
if num_neurons > 8:
    print("Only 8 Neurons are supported!")
    exit(1)
# counter = 0 # Index to current entry in active_units
for row in range(0, 4):
    for col in range(0, 4):
        for num in range(0, 4):
            for neuron in range(0, num_neurons):
                data["sudoku"][row][col][num][
                                      neuron] = list(filter(lambda x: x <= args.time, data["sudoku"][row][col][num][ neuron]))

with open('spikes2.json', 'w') as f:
    data = json.dump(data, f)
