from scrapeSudoku import scrapeSudoku
import json
import os

if __name__ == '__main__':
    dims = [(2, 2), (2, 3), (3, 3), (2, 4), (3, 4), (4, 4,)]
    for dim in dims:
        print(dim)
        height, width = dim
        for i in range(100):
            print(i)
            sudoku = scrapeSudoku(height, width)
            # show(height, width, sudoku)
            sudoku = {"height": height, "width": width, "sudoku": sudoku}
            directory = '../sudokus/%sx%s' % (height, width)
            if not os.path.exists(directory):
                os.makedirs(directory)
            with open(directory + '/Sudoku_%s.json' % (i), 'w') as f:
                json.dump(sudoku, f)
