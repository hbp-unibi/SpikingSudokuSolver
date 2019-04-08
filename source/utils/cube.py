import bisect as b
import json
import numpy as np
import serial
import time
import argparse

parser = argparse.ArgumentParser(description='Print spikes to cube')

# Required arguments
parser.add_argument("-hist", type=int, required=True,
                    help="Number of steps to show at the same time")
parser.add_argument("-time", type=int, required=True, help="Simulation time")
parser.add_argument("-wait", type=float, required=True,
                    help="Wait every time step")

parser.add_argument("-s", action="store_true", help="Spikey Mode")

args = parser.parse_args()


serial_port = serial.Serial("/dev/ttyACM0", 115200)
colors = {
    "red": [0xF8, 0x00],
    "green": [0x07, 0xE0],
    "blue": [0x00, 0x1F],
    "orange": [0xFC, 0x00],
    "yellow_dark": [0xFF, 0xE0],
    "yellow_light": [0xFF, 0xF0],
    "green1": [0x07, 0xE0],
    "green2": [0x07, 0xEF],
    "blue1": [0x04, 0x1F],
    "blue2": [0x00, 0x14],
}

with open('spikes.json') as f:
    data = json.load(f)

# data["sudoku"][row][col][num][neuron][spike]

t_max = args.time
hist_length = args.hist

# active_units[row][col][neuron]
# active_units = np.zeros((hist_length,8,8,8), dtype=np.int16()) # Store
# active units for hist_length steps
active_units = np.zeros((t_max * 10, 8, 8, 8), dtype=np.int16())

num_neurons = len(data["sudoku"][0][0][0])
if num_neurons > 8:
    print("Only 8 Neurons are supported!")
    exit(1)

# counter = 0 # Index to current entry in active_units
for timestep in range(0, t_max * 10, 1):  # 0.1 ms resolution
    # active_units[counter] = np.zeros((8,8,8)) # reset
    for row in range(0, 4):
        for col in range(0, 4):
            for num in range(0, 4):
                for neuron in range(0, num_neurons):
                    index = b.bisect_left(data["sudoku"][row][col][num][
                                          neuron], float(timestep) / 10.0)
                    if index < len(data["sudoku"][row][col][num][neuron]):
                        spike = data["sudoku"][row][col][num][neuron][index]
                        if spike >= float(timestep) / 10.0 and spike < float(timestep + 1) / 10.0:
                            active_units[timestep][
                                row * 2 + int(num / 2)][col * 2 + int(num % 2)][neuron] = 1
                if args.s:
                    index = b.bisect_left(data["inhibit"][row][col][num][
                                          0], float(timestep) / 10.0)
                    if index < len(data["inhibit"][row][col][num][0]):
                        spike = data["inhibit"][row][col][num][0][index]
                        if spike >= float(timestep) / 10.0 and spike < float(timestep + 1) / 10.0:
                            active_units[timestep][
                                row * 2 + int(num / 2)][col * 2 + int(num % 2)][7] = 1
                    index = b.bisect_left(data["self"][row][col][num][
                                          0], float(timestep) / 10.0)
                    if index < len(data["self"][row][col][num][0]):
                        spike = data["self"][row][col][num][0][index]
                        if spike >= float(timestep) / 10.0 and spike < float(timestep + 1) / 10.0:
                            active_units[timestep][
                                row * 2 + int(num / 2)][col * 2 + int(num % 2)][5] = 1


def print_mat_to_cube(current_matrix):
    frame = bytearray(512 * 2)
    for i in range(len(frame)):
        frame[i] = 0x00
    for row in range(0, 8):
        for col in range(0, 8):
            for neuron in range(0, 8):
                z_id = row % 2
                x_id = col % 2
                index = col * 2 + 16 * neuron + 128 * (7 - row)
                if not current_matrix[row][col][neuron]:
                    continue

                if x_id == 0 and z_id == 0:
                    frame[index] = colors["red"][0]
                    frame[index + 1] = colors["red"][1]
                elif x_id == 0 and z_id == 1:
                    frame[index] = colors["green"][0]
                    frame[index + 1] = colors["green"][1]
                elif x_id == 1 and z_id == 0:
                    frame[index] = colors["blue"][0]
                    frame[index + 1] = colors["blue"][1]
                elif x_id == 1 and z_id == 1:
                    frame[index] = colors["yellow_dark"][0]
                    frame[index + 1] = colors["yellow_dark"][1]

    serial_port.write(bytearray(frame))
    serial_port.flush()
    time.sleep(args.wait)

while True:
    # Start printing to cube
    for timestep in range(0, t_max * 10, 1):
        current_matrix = np.zeros((8, 8, 8), dtype=np.int16)

        # Overlap spikes of all last 20 steps
        hist_length_ = min(timestep + 1, hist_length)
        for i in range(0, hist_length_):
            current_matrix = current_matrix + active_units[timestep - i]
        # update cube
        print_mat_to_cube(current_matrix)

    frame = bytearray(512 * 2)
    for i in range(len(frame)):
        frame[i] = 0x00
    serial_port.write(bytearray(frame))
    serial_port.flush()
    time.sleep(0.5)

    current_matrix = np.zeros((8, 8, 8), dtype=np.int16)
    # Overlap spikes of all last 20 steps
    hist_length_ = min(timestep + 1, hist_length)
    for i in range(0, 50):
        current_matrix = current_matrix + active_units[t_max * 10 - 1 - i]

    print_mat_to_cube(current_matrix)

    time.sleep(10)
    print("Reset")


serial_port.close()
