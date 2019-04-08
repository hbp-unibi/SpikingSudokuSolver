/**
 * @file sudokumatrixwidget.cpp
 *
 *
 * @authors Mohith Bhargav Sunkara
 */
#include "sudokumatrixwidget.h"

#include <QDebug>
#include <QFont>
#include <QMessageBox>
#include <QPaintEvent>
#include <QPainter>
#include <QString>
#include <iostream>
#include <list>
#include <string>
#include <typeinfo>
#include <vector>

QT_USE_NAMESPACE
SudokuMatrixWidget::SudokuMatrixWidget(QWidget *parent) : QWidget(parent)
{
	setBackgroundRole(QPalette::Base);
	setMinimumSize(3300, 3300);
}

SudokuMatrixWidget::~SudokuMatrixWidget() {}

void SudokuMatrixWidget::DrawMatrix(QPainter &painter)
{

	const int MainOffset = 5;

	int row_black = row_grey;
	int columns_black = columns_grey;

	int SubSubBlockLength = 0;
	// 0x0 - matrix
	if (row_black == 0 && columns_black == 0) {
		row_black = row_grey * 0;
		columns_black = columns_grey * 0;
		SubSubBlockLength = 55;
	}
	// 2x2 - matrix
	else if (row_black == 2 && columns_black == 2) {
		row_black = row_grey * 2;
		columns_black = columns_grey * 2;
		SubSubBlockLength = 75;
		painter.setPen(QPen(Qt::darkRed, 4));
		painter.drawRect(5, 5, 150, 150);
		painter.drawRect(155, 5, 150, 150);
		painter.drawRect(5, 155, 150, 150);
		painter.drawRect(155, 155, 150, 150);
	}
	// 2x3 - matrix
	else if (row_black == 2 && columns_black == 3) {
		row_black = row_grey * 3;
		columns_black = columns_grey * 2;
		SubSubBlockLength = 65;
		painter.setPen(QPen(Qt::darkRed, 4));
		painter.drawRect(5, 5, 195, 130);
		painter.drawRect(200, 5, 195, 130);
		painter.drawRect(5, 135, 195, 130);
		painter.drawRect(200, 135, 195, 130);
		painter.drawRect(5, 265, 195, 130);
		painter.drawRect(200, 265, 195, 130);
	}
	// 2x4 - matrix
	else if (row_black == 2 && columns_black == 4) {
		row_black = row_grey * 4;
		columns_black = columns_grey * 2;
		SubSubBlockLength = 65;
		painter.setPen(QPen(Qt::darkRed, 4));
		painter.drawRect(5, 5, 260, 130);
		painter.drawRect(265, 5, 260, 130);
		painter.drawRect(5, 135, 260, 130);
		painter.drawRect(265, 135, 260, 130);
		painter.drawRect(5, 265, 260, 130);
		painter.drawRect(265, 265, 260, 130);
		painter.drawRect(5, 395, 260, 130);
		painter.drawRect(265, 395, 260, 130);
	}
	// 2x5 - matrix
	else if (row_black == 2 && columns_black == 5) {
		row_black = row_grey * 5;
		columns_black = columns_grey * 2;
		SubSubBlockLength = 55;
		painter.setPen(QPen(Qt::darkRed, 4));
		painter.drawRect(5, 5, 275, 110);
		painter.drawRect(280, 5, 275, 110);
		painter.drawRect(5, 115, 275, 110);
		painter.drawRect(280, 115, 275, 110);
		painter.drawRect(5, 225, 275, 110);
		painter.drawRect(280, 225, 275, 110);
		painter.drawRect(5, 335, 275, 110);
		painter.drawRect(280, 335, 275, 110);
		painter.drawRect(5, 445, 275, 110);
		painter.drawRect(280, 445, 275, 110);
	}
	// 3x3 - matrix
	else if (row_black == 3 && columns_black == 3) {
		row_black = row_grey * 3;
		columns_black = columns_grey * 3;
		SubSubBlockLength = 55;
		painter.setPen(QPen(Qt::darkRed, 4));
		painter.drawRect(5, 5, 165, 165);
		painter.drawRect(170, 5, 165, 165);
		painter.drawRect(335, 5, 165, 165);
		painter.drawRect(5, 170, 165, 165);
		painter.drawRect(170, 170, 165, 165);
		painter.drawRect(335, 170, 165, 165);
		painter.drawRect(5, 335, 165, 165);
		painter.drawRect(170, 335, 165, 165);
		painter.drawRect(335, 335, 165, 165);
	}
	// 3x4 - matrix
	else if (row_black == 3 && columns_black == 4) {
		row_black = row_grey * 4;
		columns_black = columns_grey * 3;
		SubSubBlockLength = 50;
		painter.setPen(QPen(Qt::darkRed, 4));
		painter.drawRect(5, 5, 200, 150);
		painter.drawRect(205, 5, 200, 150);
		painter.drawRect(405, 5, 200, 150);
		painter.drawRect(5, 155, 200, 150);
		painter.drawRect(205, 155, 200, 150);
		painter.drawRect(405, 155, 200, 150);
		painter.drawRect(5, 305, 200, 150);
		painter.drawRect(205, 305, 200, 150);
		painter.drawRect(405, 305, 200, 150);
		painter.drawRect(5, 455, 200, 150);
		painter.drawRect(205, 455, 200, 150);
		painter.drawRect(405, 455, 200, 150);
	}
	// 3x5 - matrix
	else if (row_black == 3 && columns_black == 5) {
		row_black = row_grey * 5;
		columns_black = columns_grey * 3;
		SubSubBlockLength = 40;
		painter.setPen(QPen(Qt::darkRed, 4));
		painter.drawRect(5, 5, 200, 120);
		painter.drawRect(205, 5, 200, 120);
		painter.drawRect(405, 5, 200, 120);
		painter.drawRect(5, 125, 200, 120);
		painter.drawRect(205, 125, 200, 120);
		painter.drawRect(405, 125, 200, 120);
		painter.drawRect(5, 245, 200, 120);
		painter.drawRect(205, 245, 200, 120);
		painter.drawRect(405, 245, 200, 120);
		painter.drawRect(5, 365, 200, 120);
		painter.drawRect(205, 365, 200, 120);
		painter.drawRect(405, 365, 200, 120);
		painter.drawRect(5, 485, 200, 120);
		painter.drawRect(205, 485, 200, 120);
		painter.drawRect(405, 485, 200, 120);
	}
	// 4x4 - matrix
	else if (row_black == 4 && columns_black == 4) {
		row_black = row_grey * 4;
		columns_black = columns_grey * 4;
		SubSubBlockLength = 40;
		painter.setPen(QPen(Qt::darkRed, 4));
		painter.drawRect(5, 5, 160, 160);
		painter.drawRect(165, 5, 160, 160);
		painter.drawRect(325, 5, 160, 160);
		painter.drawRect(485, 5, 160, 160);
		painter.drawRect(5, 165, 160, 160);
		painter.drawRect(165, 165, 160, 160);
		painter.drawRect(325, 165, 160, 160);
		painter.drawRect(485, 165, 160, 160);
		painter.drawRect(5, 325, 160, 160);
		painter.drawRect(165, 325, 160, 160);
		painter.drawRect(325, 325, 160, 160);
		painter.drawRect(485, 325, 160, 160);
		painter.drawRect(5, 485, 160, 160);
		painter.drawRect(165, 485, 160, 160);
		painter.drawRect(325, 485, 160, 160);
		painter.drawRect(485, 485, 160, 160);
	}
	// 5x5 - matrix
	else if (row_black == 5 && columns_black == 5) {
		row_black = row_grey * 5;
		columns_black = columns_grey * 5;
		SubSubBlockLength = 40;
		painter.setPen(QPen(Qt::darkRed, 4));
		painter.drawRect(5, 5, 200, 200);  // 1
		painter.drawRect(205, 5, 200, 200);
		painter.drawRect(405, 5, 200, 200);
		painter.drawRect(605, 5, 200, 200);
		painter.drawRect(805, 5, 200, 200);
		painter.drawRect(5, 205, 200, 200);  // 2
		painter.drawRect(205, 205, 200, 200);
		painter.drawRect(405, 205, 200, 200);
		painter.drawRect(605, 205, 200, 200);
		painter.drawRect(805, 205, 200, 200);
		painter.drawRect(5, 405, 200, 200);  // 3
		painter.drawRect(205, 405, 200, 200);
		painter.drawRect(405, 405, 200, 200);
		painter.drawRect(605, 405, 200, 200);
		painter.drawRect(805, 405, 200, 200);
		painter.drawRect(5, 605, 200, 200);  // 4
		painter.drawRect(205, 605, 200, 200);
		painter.drawRect(405, 605, 200, 200);
		painter.drawRect(605, 605, 200, 200);
		painter.drawRect(805, 605, 200, 200);
		painter.drawRect(5, 805, 200, 200);  // 5
		painter.drawRect(205, 805, 200, 200);
		painter.drawRect(405, 805, 200, 200);
		painter.drawRect(605, 805, 200, 200);
		painter.drawRect(805, 805, 200, 200);
	}
	// 6x6 - matrix
	else if (row_black == 6 && columns_black == 6) {
		row_black = row_grey * 6;
		columns_black = columns_grey * 6;
		SubSubBlockLength = 40;
		painter.setPen(QPen(Qt::darkRed, 4));
		painter.drawRect(5, 5, 240, 240);  // 1
		painter.drawRect(245, 5, 240, 240);
		painter.drawRect(485, 5, 240, 240);
		painter.drawRect(725, 5, 240, 240);
		painter.drawRect(965, 5, 240, 240);
		painter.drawRect(1205, 5, 240, 240);
		painter.drawRect(5, 245, 240, 240);  // 2
		painter.drawRect(245, 245, 240, 240);
		painter.drawRect(485, 245, 240, 240);
		painter.drawRect(725, 245, 240, 240);
		painter.drawRect(965, 245, 240, 240);
		painter.drawRect(1205, 245, 240, 240);
		painter.drawRect(5, 485, 240, 240);  // 3
		painter.drawRect(245, 485, 240, 240);
		painter.drawRect(485, 485, 240, 240);
		painter.drawRect(725, 485, 240, 240);
		painter.drawRect(965, 485, 240, 240);
		painter.drawRect(1205, 485, 240, 240);
		painter.drawRect(5, 725, 240, 240);  // 4
		painter.drawRect(245, 725, 240, 240);
		painter.drawRect(485, 725, 240, 240);
		painter.drawRect(725, 725, 240, 240);
		painter.drawRect(965, 725, 240, 240);
		painter.drawRect(1205, 725, 240, 240);
		painter.drawRect(5, 965, 240, 240);  // 5
		painter.drawRect(245, 965, 240, 240);
		painter.drawRect(485, 965, 240, 240);
		painter.drawRect(725, 965, 240, 240);
		painter.drawRect(965, 965, 240, 240);
		painter.drawRect(1205, 965, 240, 240);
		painter.drawRect(5, 1205, 240, 240);  // 6
		painter.drawRect(245, 1205, 240, 240);
		painter.drawRect(485, 1205, 240, 240);
		painter.drawRect(725, 1205, 240, 240);
		painter.drawRect(965, 1205, 240, 240);
		painter.drawRect(1205, 1205, 240, 240);
	}
	// 7x7 - matrix
	else if (row_black == 7 && columns_black == 7) {
		row_black = row_grey * 7;
		columns_black = columns_grey * 7;
		SubSubBlockLength = 40;
		painter.setPen(QPen(Qt::darkRed, 4));
		painter.drawRect(5, 5, 280, 280);  // 1
		painter.drawRect(285, 5, 280, 280);
		painter.drawRect(565, 5, 280, 280);
		painter.drawRect(845, 5, 280, 280);
		painter.drawRect(1125, 5, 280, 280);
		painter.drawRect(1405, 5, 280, 280);
		painter.drawRect(1685, 5, 280, 280);
		painter.drawRect(5, 285, 280, 280);  // 2
		painter.drawRect(285, 285, 280, 280);
		painter.drawRect(565, 285, 280, 280);
		painter.drawRect(845, 285, 280, 280);
		painter.drawRect(1125, 285, 280, 280);
		painter.drawRect(1405, 285, 280, 280);
		painter.drawRect(1685, 285, 280, 280);
		painter.drawRect(5, 565, 280, 280);  // 3
		painter.drawRect(285, 565, 280, 280);
		painter.drawRect(565, 565, 280, 280);
		painter.drawRect(845, 565, 280, 280);
		painter.drawRect(1125, 565, 280, 280);
		painter.drawRect(1405, 565, 280, 280);
		painter.drawRect(1685, 565, 280, 280);
		painter.drawRect(5, 845, 280, 280);  // 4
		painter.drawRect(285, 845, 280, 280);
		painter.drawRect(565, 845, 280, 280);
		painter.drawRect(845, 845, 280, 280);
		painter.drawRect(1125, 845, 280, 280);
		painter.drawRect(1405, 845, 280, 280);
		painter.drawRect(1685, 845, 280, 280);
		painter.drawRect(5, 1125, 280, 280);  // 5
		painter.drawRect(285, 1125, 280, 280);
		painter.drawRect(565, 1125, 280, 280);
		painter.drawRect(845, 1125, 280, 280);
		painter.drawRect(1125, 1125, 280, 280);
		painter.drawRect(1405, 1125, 280, 280);
		painter.drawRect(1685, 1125, 280, 280);
		painter.drawRect(5, 1405, 280, 280);  // 6
		painter.drawRect(285, 1405, 280, 280);
		painter.drawRect(565, 1405, 280, 280);
		painter.drawRect(845, 1405, 280, 280);
		painter.drawRect(1125, 1405, 280, 280);
		painter.drawRect(1405, 1405, 280, 280);
		painter.drawRect(1685, 1405, 280, 280);
		painter.drawRect(5, 1685, 280, 280);  // 7
		painter.drawRect(285, 1685, 280, 280);
		painter.drawRect(565, 1685, 280, 280);
		painter.drawRect(845, 1685, 280, 280);
		painter.drawRect(1125, 1685, 280, 280);
		painter.drawRect(1405, 1685, 280, 280);
		painter.drawRect(1685, 1685, 280, 280);
	}
	// 8x8 - matrix
	else if (row_black == 8 && columns_black == 8) {
		row_black = row_grey * 8;
		columns_black = columns_grey * 8;
		SubSubBlockLength = 40;
		painter.setPen(QPen(Qt::darkRed, 4));
		painter.drawRect(5, 5, 320, 320);  // 1
		painter.drawRect(325, 5, 320, 320);
		painter.drawRect(645, 5, 320, 320);
		painter.drawRect(965, 5, 320, 320);
		painter.drawRect(1285, 5, 320, 320);
		painter.drawRect(1605, 5, 320, 320);
		painter.drawRect(1925, 5, 320, 320);
		painter.drawRect(2245, 5, 320, 320);
		painter.drawRect(5, 325, 320, 320);  // 2
		painter.drawRect(325, 325, 320, 320);
		painter.drawRect(645, 325, 320, 320);
		painter.drawRect(965, 325, 320, 320);
		painter.drawRect(1285, 325, 320, 320);
		painter.drawRect(1605, 325, 320, 320);
		painter.drawRect(1925, 325, 320, 320);
		painter.drawRect(2245, 325, 320, 320);
		painter.drawRect(5, 645, 320, 320);  // 3
		painter.drawRect(325, 645, 320, 320);
		painter.drawRect(645, 645, 320, 320);
		painter.drawRect(965, 645, 320, 320);
		painter.drawRect(1285, 645, 320, 320);
		painter.drawRect(1605, 645, 320, 320);
		painter.drawRect(1925, 645, 320, 320);
		painter.drawRect(2245, 645, 320, 320);
		painter.drawRect(5, 965, 320, 320);  // 4
		painter.drawRect(325, 965, 320, 320);
		painter.drawRect(645, 965, 320, 320);
		painter.drawRect(965, 965, 320, 320);
		painter.drawRect(1285, 965, 320, 320);
		painter.drawRect(1605, 965, 320, 320);
		painter.drawRect(1925, 965, 320, 320);
		painter.drawRect(2245, 965, 320, 320);
		painter.drawRect(5, 1285, 320, 320);  // 5
		painter.drawRect(325, 1285, 320, 320);
		painter.drawRect(645, 1285, 320, 320);
		painter.drawRect(965, 1285, 320, 320);
		painter.drawRect(1285, 1285, 320, 320);
		painter.drawRect(1605, 1285, 320, 320);
		painter.drawRect(1925, 1285, 320, 320);
		painter.drawRect(2245, 1285, 320, 320);
		painter.drawRect(5, 1605, 320, 320);  // 6
		painter.drawRect(325, 1605, 320, 320);
		painter.drawRect(645, 1605, 320, 320);
		painter.drawRect(965, 1605, 320, 320);
		painter.drawRect(1285, 1605, 320, 320);
		painter.drawRect(1605, 1605, 320, 320);
		painter.drawRect(1925, 1605, 320, 320);
		painter.drawRect(2245, 1605, 320, 320);
		painter.drawRect(5, 1925, 320, 320);  // 7
		painter.drawRect(325, 1925, 320, 320);
		painter.drawRect(645, 1925, 320, 320);
		painter.drawRect(965, 1925, 320, 320);
		painter.drawRect(1285, 1925, 320, 320);
		painter.drawRect(1605, 1925, 320, 320);
		painter.drawRect(1925, 1925, 320, 320);
		painter.drawRect(2245, 1925, 320, 320);
		painter.drawRect(5, 2245, 320, 320);  // 8
		painter.drawRect(325, 2245, 320, 320);
		painter.drawRect(645, 2245, 320, 320);
		painter.drawRect(965, 2245, 320, 320);
		painter.drawRect(1285, 2245, 320, 320);
		painter.drawRect(1605, 2245, 320, 320);
		painter.drawRect(1925, 2245, 320, 320);
		painter.drawRect(2245, 2245, 320, 320);
	}
	// 9x9 - matrix
	else if (row_black == 9 && columns_black == 9) {
		row_black = row_grey * 9;
		columns_black = columns_grey * 9;
		SubSubBlockLength = 40;
		painter.setPen(QPen(Qt::darkRed, 4));
		painter.drawRect(5, 5, 360, 360);  // 1
		painter.drawRect(365, 5, 360, 360);
		painter.drawRect(725, 5, 360, 360);
		painter.drawRect(1085, 5, 360, 360);
		painter.drawRect(1445, 5, 360, 360);
		painter.drawRect(1805, 5, 360, 360);
		painter.drawRect(2165, 5, 360, 360);
		painter.drawRect(2525, 5, 360, 360);
		painter.drawRect(2885, 5, 360, 360);
		painter.drawRect(5, 365, 360, 360);  // 2
		painter.drawRect(365, 365, 360, 360);
		painter.drawRect(725, 365, 360, 360);
		painter.drawRect(1085, 365, 360, 360);
		painter.drawRect(1445, 365, 360, 360);
		painter.drawRect(1805, 365, 360, 360);
		painter.drawRect(2165, 365, 360, 360);
		painter.drawRect(2525, 365, 360, 360);
		painter.drawRect(2885, 365, 360, 360);
		painter.drawRect(5, 725, 360, 360);  // 3
		painter.drawRect(365, 725, 360, 360);
		painter.drawRect(725, 725, 360, 360);
		painter.drawRect(1085, 725, 360, 360);
		painter.drawRect(1445, 725, 360, 360);
		painter.drawRect(1805, 725, 360, 360);
		painter.drawRect(2165, 725, 360, 360);
		painter.drawRect(2525, 725, 360, 360);
		painter.drawRect(2885, 725, 360, 360);
		painter.drawRect(5, 1085, 360, 360);  // 4
		painter.drawRect(365, 1085, 360, 360);
		painter.drawRect(725, 1085, 360, 360);
		painter.drawRect(1085, 1085, 360, 360);
		painter.drawRect(1445, 1085, 360, 360);
		painter.drawRect(1805, 1085, 360, 360);
		painter.drawRect(2165, 1085, 360, 360);
		painter.drawRect(2525, 1085, 360, 360);
		painter.drawRect(2885, 1085, 360, 360);
		painter.drawRect(5, 1445, 360, 360);  // 5
		painter.drawRect(365, 1445, 360, 360);
		painter.drawRect(725, 1445, 360, 360);
		painter.drawRect(1085, 1445, 360, 360);
		painter.drawRect(1445, 1445, 360, 360);
		painter.drawRect(1805, 1445, 360, 360);
		painter.drawRect(2165, 1445, 360, 360);
		painter.drawRect(2525, 1445, 360, 360);
		painter.drawRect(2885, 1445, 360, 360);
		painter.drawRect(5, 1805, 360, 360);  // 6
		painter.drawRect(365, 1805, 360, 360);
		painter.drawRect(725, 1805, 360, 360);
		painter.drawRect(1085, 1805, 360, 360);
		painter.drawRect(1445, 1805, 360, 360);
		painter.drawRect(1805, 1805, 360, 360);
		painter.drawRect(2165, 1805, 360, 360);
		painter.drawRect(2525, 1805, 360, 360);
		painter.drawRect(2885, 1805, 360, 360);
		painter.drawRect(5, 2165, 360, 360);  // 7
		painter.drawRect(365, 2165, 360, 360);
		painter.drawRect(725, 2165, 360, 360);
		painter.drawRect(1085, 2165, 360, 360);
		painter.drawRect(1445, 2165, 360, 360);
		painter.drawRect(1805, 2165, 360, 360);
		painter.drawRect(2165, 2165, 360, 360);
		painter.drawRect(2525, 2165, 360, 360);
		painter.drawRect(2885, 2165, 360, 360);
		painter.drawRect(5, 2525, 360, 360);  // 8
		painter.drawRect(365, 2525, 360, 360);
		painter.drawRect(725, 2525, 360, 360);
		painter.drawRect(1085, 2525, 360, 360);
		painter.drawRect(1445, 2525, 360, 360);
		painter.drawRect(1805, 2525, 360, 360);
		painter.drawRect(2165, 2525, 360, 360);
		painter.drawRect(2525, 2525, 360, 360);
		painter.drawRect(2885, 2525, 360, 360);
		painter.drawRect(5, 2885, 360, 360);  // 9
		painter.drawRect(365, 2885, 360, 360);
		painter.drawRect(725, 2885, 360, 360);
		painter.drawRect(1085, 2885, 360, 360);
		painter.drawRect(1445, 2885, 360, 360);
		painter.drawRect(1805, 2885, 360, 360);
		painter.drawRect(2165, 2885, 360, 360);
		painter.drawRect(2525, 2885, 360, 360);
		painter.drawRect(2885, 2885, 360, 360);
	}

	// this area is defined to draw the subblocks of the sudoku
	painter.setPen(QPen(Qt::black, 2));
	for (int i = 0; i < row_black; i++) {
		for (int j = 0; j < columns_black; j++) {
			painter.drawRect(MainOffset + i * SubSubBlockLength,
			                 MainOffset + j * SubSubBlockLength,
			                 SubSubBlockLength, SubSubBlockLength);
		}
	}

	// this area is defined to draw the numbers in subblocks which are given by
	// the sudokuclass
	QFont font;
	font.setPointSize(26);
	painter.setFont(font);
	for (int i = 0; i < sudoku.getNumOfSquares(); i++) {
		for (int j = 0; j < sudoku.getNumOfSquares(); j++) {
			if (sudoku.getEntryInSquare(j, i) != 0) {
				QRect rect;
				painter.setPen(QPen(Qt::darkMagenta, 2));
				rect.setRect(MainOffset + i * SubSubBlockLength,
				             MainOffset + j * SubSubBlockLength,
				             SubSubBlockLength, SubSubBlockLength);
				painter.drawText(
				    rect, Qt::AlignCenter,
				    QString(
				        QVariant(sudoku.getEntryInSquare(j, i)).toString()));
			}
		}
	}

	if (wakeup == false) {
		QFont font1;
		font1.setPointSize(26);
		painter.setFont(font1);
		for (int i = 0; i < sudokuresult.getNumOfSquares(); i++) {
			for (int j = 0; j < sudokuresult.getNumOfSquares(); j++) {
				if (sudokuresult.complete() &&
				    sudokuresult.getEntryInSquare(j, i) !=
				        sudoku.getEntryInSquare(j, i)) {
					QRect rect;
					painter.setPen(QPen(Qt::darkGreen, 2));
					rect.setRect(MainOffset + i * SubSubBlockLength,
					             MainOffset + j * SubSubBlockLength,
					             SubSubBlockLength, SubSubBlockLength);
					painter.drawText(
					    rect, Qt::AlignCenter,
					    QString(QVariant(sudokuresult.getEntryInSquare(j, i))
					                .toString()));
				}
				else if (!sudokuresult.complete() &&
				         sudokuresult.getEntryInSquare(j, i) !=
				             sudoku.getEntryInSquare(j, i)) {
					QRect rect;
					rect.setRect(MainOffset + i * SubSubBlockLength,
					             MainOffset + j * SubSubBlockLength,
					             SubSubBlockLength, SubSubBlockLength);
					int sudoku_entry = sudokuresult.getEntryInSquare(j, i);
					int solution_entry = sudoku.getEntryInSquareSolution(j, i);
					if (solution_entry == 0) {
						QBrush brush(QColor(160, 160, 160));  // light grey
						painter.setPen(QPen(QColor(160, 160, 160), 2));
					}
					else if (sudoku_entry != solution_entry) {
						QBrush brush(QColor(255, 160, 122));  // light red
						painter.setPen(QPen(Qt::red, 2));
						painter.fillRect(rect, brush);
					}
					else if (sudoku_entry == solution_entry) {
						QBrush brush(QColor(152, 251, 152));  // light green
						painter.setPen(QPen(Qt::darkGreen, 2));
						painter.fillRect(rect, brush);
					}
					painter.drawText(
					    rect, Qt::AlignCenter,
					    QString(QVariant(sudokuresult.getEntryInSquare(j, i))
					                .toString()));
				}
			}
		}
	}
}

void SudokuMatrixWidget::paintEvent(QPaintEvent *event)
{
	// have a look at it later if you can
	// This class is to write numbers in the subsubBlocks which are given by the
	// sudokuclass
	if (toggle_Painter == true) {
		event->accept();

		QPainter painter(this);

		DrawMatrix(painter);

		painter.end();
	}
}

void SudokuMatrixWidget::ChangeRowColumnValue(int row, int column)
{
	row_grey = row;
	columns_grey = column;

	// qInfo() << "Row: " << row_grey << " :: Column: " << columns_grey;
}

void SudokuMatrixWidget::ChangeInnerValues(Sudoku s) { sudoku = s; }

void SudokuMatrixWidget::ChangeInnerResults(Sudoku sim, bool let)
{
	sudokuresult = sim;
	wakeup = let;
}
void SudokuMatrixWidget::ChangePainterState(bool paintme)
{
	toggle_Painter = paintme;
}
