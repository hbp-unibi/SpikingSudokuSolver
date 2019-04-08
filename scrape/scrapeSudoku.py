import argparse
import json
import re
from contextlib import closing

import numpy as np
import requests
from bs4 import BeautifulSoup
from requests.exceptions import RequestException


def convert_sud(height, width, sud):
    sud_subsq = []
    for i in range(height * width):
        temp = []
        for j in range(height):
            for k in range(width):
                temp.append(sud[int(i / height) * height + j]
                            [(i % height) * width + k])
        sud_subsq.append(temp)
    return sud_subsq


def parse_h_w(height, width):
    if height == width:
        return 'http://www.menneske.no/sudoku/' + '%s' % (height)
    else:
        if height > width:
            return 'http://www.menneske.no/sudoku/' + '%sx%s' % (width, height)
        else:
            return 'http://www.menneske.no/sudoku/' + '%sx%s' % (height, width)


def scrapeSudoku(height, width):
    sudoku = []
    solution = []

    website = parse_h_w(height, width)

    page = requests.get(website)

    soup = BeautifulSoup(page.content, 'html.parser')

    grid = soup.find('div', {'class': 'grid'})

    table = grid.find('table')

    table = table.findAll('tr', {'class': 'grid'})

    for row in table:
        sudoku_row = []
        for entry in row.findAll('td'):
            try:
                sudoku_row.append(int(entry.text))
            except ValueError:
                sudoku_row.append(0)
        sudoku.append(sudoku_row)

    solution_link = grid.find('a', href=re.compile('solution'))

    page = requests.get(website + '/' + solution_link['href'])

    soup = BeautifulSoup(page.content, 'html.parser')

    grid = soup.find('div', {'class': 'grid'})

    table = grid.find('table')

    table = table.findAll('tr', {'class': 'grid'})

    for row in table:
        sudoku_row = []
        for entry in row.findAll('td'):
            try:
                sudoku_row.append(int(entry.text))
            except ValueError:
                sudoku_row.append(0)
        solution.append(sudoku_row)

    return sudoku, solution

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Scrape Sudoku from "http://www.menneske.no/sudoku/"')
    parser.add_argument('height', metavar='h', type=int,
                        help='height of sudoku subblock')
    parser.add_argument('width', metavar='w', type=int,
                        help='width of sudoku subblock')
    args = parser.parse_args()
    height, width = args.height, args.width
    sudoku, solution = scrapeSudoku(height, width)
    # show(height, width, sudoku)
    sudoku = {"height": height, "width": width,
              "sudoku": sudoku, "solution": solution}
    with open('Sudoku.json', 'w') as f:
        json.dump(sudoku, f)
